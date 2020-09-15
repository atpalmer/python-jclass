#include <Python.h>
#include "membuff.h"
#include "access.h"
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
} JavaClass;

static void _JavaClass_free(JavaClass *this) {
    JavaClassConstantPool_free(this->constant_pool);
    JavaClassInterfaces_free(this->interfaces);
    JavaClassFields_free(this->fields);
    JavaClassMethods_free(this->methods);
    JavaClassAttributes_free(this->attributes);
    PyMem_Free(this);
}

static JavaClass *JavaClass_from_MemReader(MemReader *r) {
    JavaClass *class = PyMem_Malloc(sizeof(*class));

    class->magic_number = MemReader_next_uint32(r);
    class->minor_version = MemReader_next_uint16(r);
    class->major_version = MemReader_next_uint16(r);

    constant_pool_parse(r, &class->constant_pool);

    class->access_flags = MemReader_next_uint16(r);
    class->this_class = MemReader_next_uint16(r);
    class->super_class = MemReader_next_uint16(r);

    interfaces_parse(r, &class->interfaces);
    fields_parse(r, &class->fields);
    parse_methods(r, &class->methods);
    attributes_parse(r, &class->attributes);

    return class;
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
}

static PyObject *jclass_load(PyObject *self, PyObject *args) {
    char *fname;
    if(!PyArg_ParseTuple(args, "s", &fname))
        return NULL;

    MemReader *r = MemReader_from_filename(fname);
    JavaClass *class = JavaClass_from_MemReader(r);
    JavaClass_print(class);
    _JavaClass_free(class);

    printf("Bytes Read: %lu\n", r->pos);
    printf("File Bytes: %lu\n", r->size);

    PyObject *result = PyBytes_FromStringAndSize((char *)r->data, r->size);
    MemReader_free(r);
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

