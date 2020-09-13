#include <Python.h>
#include "access.h"
#include "parse.h"
#include "constant_pool.h"
#include "interfaces.h"


/* TODO: make PyObject */
typedef struct {
    uint32_t magic_number;
    uint16_t minor_version;
    uint16_t major_version;

    JavaClassConstantPool *constant_pool;

    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;

    JavaClassInterfaces *interfaces;

    uint16_t fields_count;
    uint8_t *fields;

    uint16_t methods_count;
    uint8_t *methods;

    uint16_t attributes_count;
    uint8_t *attributes;

    Py_ssize_t size;
    uint8_t data[];
} JavaClass;


static size_t parse_attributes(uint8_t *attrs, int count, uint8_t **obj) {
    *obj = attrs;
    printf("Attributes count: %d\n", count);
    size_t attrs_bytes = 0;
    for(int i = 0; i < count; ++i) {
        uint8_t *attr = &attrs[attrs_bytes];
        printf("** Attr name index: %u\n", Attribute_name_index(attr));
        printf("** Attr length: %u\n", Attribute_length(attr));
        printf("** Attr info: %.*s\n", Attribute_length(attr), (char *)Attribute_info(attr));

        attrs_bytes += 6 + Attribute_length(attr);
    }

    return attrs_bytes;
}

static size_t parse_methods(uint8_t *methods, int count, uint8_t **obj) {
    *obj = methods;
    printf("Methods count: %d\n", count);
    size_t methods_bytes = 0;
    for(int i = 0; i < count; ++i) {
        uint8_t *method = &methods[methods_bytes];
        printf("* Method access_flags: %u\n", Method_access_flags(method));
        printf("* Method name_index: %u\n", Method_name_index(method));
        printf("* Method descriptor_index: %u\n", Method_descriptor_index(method));
        printf("* Method attributes_count: %u\n", Method_attributes_count(method));

        uint8_t *attrs;
        size_t attrs_bytes = parse_attributes(Method_attributes(method), Method_attributes_count(method), &attrs);

        methods_bytes += 8 + attrs_bytes;
    }
    return methods_bytes;
}

static size_t parse_fields(uint8_t *fields, int count, uint8_t **obj) {
    *obj = fields;
    printf("Fields count: %d\n", count);
    size_t fields_bytes = 0;
    for(int i = 0; i < count; ++i) {
        uint8_t *field = &fields[fields_bytes];
        printf("* Field access flags: %u\n", Field_access_flags(field));
        printf("* Field name index: %u\n", Field_name_index(field));
        printf("* Field descriptor index: %u\n", Field_descriptor_index(field));
        printf("* Field attributes count: %u\n", Field_attributes_count(field));

        uint8_t *attrs;
        size_t attrs_bytes = parse_attributes(Field_attributes(field), Field_attributes_count(field), &attrs);

        fields_bytes += 8 + attrs_bytes;
    }
    return fields_bytes;
}

static JavaClass *_JavaClass_from_filename(const char *filename) {
    JavaClass *new = PyMem_Malloc(sizeof(JavaClass) + 4096);

    FILE *fp = fopen(filename, "rb");
    new->size = fread(new->data, 1, 4096, fp);
    fclose(fp);

    return new;
}

static void JavaClassConstantPool_free(JavaClassConstantPool *this) {
    for(int i = 0; i < this->constant_pool_count - 1; ++i)
        if(this->constant_pool[i])
            PyMem_Free(this->constant_pool[i]);
    PyMem_Free(this);
}

static void _JavaClass_free(JavaClass *this) {
    JavaClassConstantPool_free(this->constant_pool);
    JavaClassInterfaces_free(this->interfaces);
    PyMem_Free(this->constant_pool);
    PyMem_Free(this);
}

static PyObject *jclass_load(PyObject *self, PyObject *args) {
    char *fname;
    if(!PyArg_ParseTuple(args, "s", &fname))
        return NULL;

    JavaClass *class = _JavaClass_from_filename(fname);

    size_t curr_bytes = 0;

    curr_bytes += parse32(&class->data[curr_bytes], &class->magic_number);
    curr_bytes += parse16(&class->data[curr_bytes], &class->minor_version);
    curr_bytes += parse16(&class->data[curr_bytes], &class->major_version);
    curr_bytes += constant_pool_parse(&class->data[curr_bytes], &class->constant_pool);
    curr_bytes += parse16(&class->data[curr_bytes], &class->access_flags);
    curr_bytes += parse16(&class->data[curr_bytes], &class->this_class);
    curr_bytes += parse16(&class->data[curr_bytes], &class->super_class);

    printf("Magic Number: %X\n", class->magic_number);
    printf("Version: %u.%u\n", class->major_version, class->minor_version);
    constant_pool_print(class->constant_pool);
    access_flags_print(class->access_flags);
    printf("This Class Pool Index: %u\n", class->this_class);
    printf("Super Class Pool Index: %u\n", class->super_class);

    curr_bytes += interfaces_parse(&class->data[curr_bytes], &class->interfaces);
    interfaces_print(class->interfaces);
    curr_bytes += parse16(&class->data[curr_bytes], &class->fields_count);
    curr_bytes += parse_fields(&class->data[curr_bytes], class->fields_count, &class->fields);
    curr_bytes += parse16(&class->data[curr_bytes], &class->methods_count);
    curr_bytes += parse_methods(&class->data[curr_bytes], class->methods_count, &class->methods);

    printf("CLASS ATTRIBUTES:\n");

    curr_bytes += parse16(&class->data[curr_bytes], &class->attributes_count);
    curr_bytes += parse_attributes(&class->data[curr_bytes], class->attributes_count, &class->attributes);

    printf("Total Bytes: %lu\n", curr_bytes);

    PyObject *result = PyBytes_FromStringAndSize((char *)class->data, class->size);
    _JavaClass_free(class);
    return result;

    /* TODO: return JavaClass object (as PyObject) */
}

static PyMethodDef module_methods[] = {
    {"load", jclass_load, METH_VARARGS},
    {0},
};

static PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "jclass",
    .m_size = 0,
    .m_methods = module_methods,
};

PyMODINIT_FUNC PyInit_jclass(void) {
    PyObject *m = PyModule_Create(&module);
    return m;
}

