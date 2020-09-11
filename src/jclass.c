#import <Python.h>

static const char *CONSTANT_POOL_DESC[] = {
    [0] = "",
    [1] = NULL,
    [2] = NULL,
    [3] = NULL,
    [4] = NULL,
    [5] = NULL,
    [6] = NULL,
    [7] = NULL,
    [8] = NULL,
    [9] = NULL,
    [10] = "CONSTANT_Methodref_info",
    [11] = NULL,
    [12] = NULL,
    [13] = NULL,
    [14] = NULL,
    [15] = NULL,
    [16] = NULL,
};

struct constant_pool_base {
    uint8_t tag;
    uint8_t info[];
};

/* TODO: make PyObject */
typedef struct {
    /* ptrs to .data[] (big-endian order!) */
    uint32_t *magic_number;
    uint16_t *minor_version;
    uint16_t *major_version;
    uint16_t *constant_pool_count;
    struct constant_pool_base *constant_pool;

    Py_ssize_t size;
    uint8_t data[];
} JavaClass;

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
    class->constant_pool_count = &class->data[8];
    class->constant_pool = &class->data[10];

    printf("Magic Number: %X\n", ntohl(*class->magic_number));
    printf("Version: %u.%u\n", ntohs(*class->major_version), ntohs(*class->minor_version));
    printf("Constant Pool Count: %u\n", ntohs(*class->constant_pool_count));

    for(int i = ntohs(*class->constant_pool_count); i > 0; --i) {
        printf(" TAG: %u (%s)\n", class->constant_pool->tag, CONSTANT_POOL_DESC[class->constant_pool->tag]);
        break;
    }

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

