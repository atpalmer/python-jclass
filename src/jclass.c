#import <Python.h>

/* TODO: make PyObject */
typedef struct {
    /* ptrs to .data[] (big-endian order!) */
    uint8_t *magic_number;
    uint8_t *minor_version;
    uint8_t *major_version;

    Py_ssize_t size;
    uint8_t data[];
} JavaClass;

static uint16_t deref16(uint8_t *val) {
    uint8_t result[2];
    result[0] = val[1];
    result[1] = val[0];
    return *(uint16_t *)&result;
}

static uint32_t deref32(uint8_t *val) {
    uint8_t result[4];
    result[0] = val[3];
    result[1] = val[2];
    result[2] = val[1];
    result[3] = val[0];
    return *(uint32_t *)&result;
}

static PyObject *jclass_load(PyObject *self, PyObject *args) {
    char *fname;
    if(!PyArg_ParseTuple(args, "s", &fname))
        return NULL;

    JavaClass *class = PyMem_Malloc(sizeof(JavaClass) + 4096);

    FILE *fp = fopen(fname, "rb");
    class->size = fread(class->data, 1, 4096, fp);
    fclose(fp);

    class->magic_number = &class->data[0];
    class->minor_version = &class->data[4];
    class->major_version = &class->data[6];

    printf("Magic Number: %X\n", deref32(class->magic_number));
    printf("Version: %u.%u\n", deref16(class->major_version), deref16(class->minor_version));

    PyObject *result = PyBytes_FromStringAndSize(class->data, class->size);
    PyMem_Free(class);
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

