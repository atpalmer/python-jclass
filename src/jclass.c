#include <Python.h>
#include "access.h"
#include "parse.h"
#include "constant_pool.h"
#include "interfaces.h"
#include "fields.h"
#include "methods.h"
#include "attributes.h"


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
    JavaClassFields *fields;
    JavaClassMethods *methods;
    JavaClassAttributes *attributes;

    Py_ssize_t size;
    uint8_t data[];
} JavaClass;


static JavaClass *_JavaClass_from_filename(const char *filename) {
    JavaClass *new = PyMem_Malloc(sizeof(JavaClass) + 4096);

    FILE *fp = fopen(filename, "rb");
    new->size = fread(new->data, 1, 4096, fp);
    fclose(fp);

    return new;
}

static void _JavaClass_free(JavaClass *this) {
    JavaClassConstantPool_free(this->constant_pool);
    JavaClassInterfaces_free(this->interfaces);
    JavaClassFields_free(this->fields);
    JavaClassMethods_free(this->methods);
    JavaClassAttributes_free(this->attributes);
    PyMem_Free(this);
}

static void JavaClass_print(JavaClass *class) {
    printf("Magic Number: %X\n", class->magic_number);
    printf("Version: %u.%u\n", class->major_version, class->minor_version);
    constant_pool_print(class->constant_pool);
    access_flags_print(class->access_flags);
    printf("This Class Pool Index: %u\n", class->this_class);
    printf("Super Class Pool Index: %u\n", class->super_class);
    interfaces_print(class->interfaces);
    fields_print(class->fields);
    methods_print(class->methods);
    printf("CLASS ATTRIBUTES:\n");
    attributes_print(class->attributes);
    printf("File Bytes: %lu\n", class->size);
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
    curr_bytes += interfaces_parse(&class->data[curr_bytes], &class->interfaces);
    curr_bytes += fields_parse(&class->data[curr_bytes], &class->fields);
    curr_bytes += parse_methods(&class->data[curr_bytes], &class->methods);
    curr_bytes += attributes_parse(&class->data[curr_bytes], &class->attributes);

    JavaClass_print(class);
    printf("Bytes Read: %lu\n", curr_bytes);

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

