#import <Python.h>
#import "constant_pool.h"

enum access_flag {
    ACC_PUBLIC      =0x0001,
    ACC_FINAL       =0x0010,
    ACC_SUPER       =0x0020,
    ACC_INTERFACE   =0x0200,
    ACC_ABSTRACT    =0x0400,
    ACC_SYNTHETIC   =0x1000,
    ACC_ANNOTATION  =0x2000,
    ACC_ENUM        =0x4000,
};

#define CLASS_HAS_ACCESS(class, flag)  (ntohs(*(class)->access_flags) & (flag))


/* TODO: make PyObject */
typedef struct {
    /* ptrs to .data[] (big-endian order!) */
    uint32_t *magic_number;
    uint16_t *minor_version;
    uint16_t *major_version;
    uint16_t *constant_pool_count;
    uint8_t *constant_pool;
    uint16_t *access_flags;

    Py_ssize_t size;
    uint8_t data[];
} JavaClass;

static size_t parse_constant_pool(uint8_t *pool, int count) {
    size_t pool_bytes = 0;
    for(int i = 1; i < count; ++i) {
        uint8_t *c = &pool[pool_bytes];
        printf("%d.) %s=%u: bytes index: %u\n", i, CONSTANT_DESC(c), Constant_tag(c), pool_bytes);

        switch(Constant_tag(c)) {
        case CONSTANT_TYPE_Utf8:
            printf("        * tag(%u), length(%u), bytes(\"%.*s\")\n",
                Constant_tag(c), Utf8_length(c), Utf8_length(c), Utf8_bytes(c));
            pool_bytes += 3 + Utf8_length(c);
            break;

        case CONSTANT_TYPE_Class:
            printf("        * tag(%u), name_index(%u)\n", Constant_tag(c), Class_name_index(c));
            pool_bytes += 3;
            break;

        case CONSTANT_TYPE_String:
            printf("        * tag(%u), string_index(%u)\n", Constant_tag(c), String_string_index(c));
            pool_bytes += 3;
            break;

        case CONSTANT_TYPE_Fieldref:
            printf("        * tag(%u), class_index(%u), name_and_type_index(%u)\n",
                Constant_tag(c), Fieldref_class_index(c), Fieldref_name_and_type_index(c));
            pool_bytes += 5;
            break;

        case CONSTANT_TYPE_Methodref:
            printf("        * tag(%u), class_index(%u), name_and_type_index(%u)\n",
                Constant_tag(c), Methodref_class_index(c), Methodref_name_and_type_index(c));
            pool_bytes += 5;
            break;

        case CONSTANT_TYPE_NameAndType:
            printf("        * tag(%u), name_index(%u), descriptor_index(%u)\n",
                Constant_tag(c), NameAndType_name_index(c), NameAndType_descriptor_index(c));
            pool_bytes += 5;
            break;
        }
    }

    return pool_bytes;
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
    class->constant_pool_count = &class->data[8];
    class->constant_pool = &class->data[10];

    printf("Magic Number: %X\n", ntohl(*class->magic_number));
    printf("Version: %u.%u\n", ntohs(*class->major_version), ntohs(*class->minor_version));
    printf("Constant Pool Count: %u\n", ntohs(*class->constant_pool_count));

    size_t pool_bytes = parse_constant_pool(class->constant_pool, ntohs(*class->constant_pool_count));

    class->access_flags = class->constant_pool + pool_bytes;

    printf("Raw Access Flags: %u\n", *class->access_flags);
    printf("(Local) Access Flags: %u\n", ntohs(*class->access_flags));
    printf("PUBLIC Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_PUBLIC));
    printf("FINAL Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_FINAL));
    printf("SUPER Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_SUPER));
    printf("INTERFACE Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_INTERFACE));
    printf("ABSTRACT Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_ABSTRACT));
    printf("SYNTHETIC Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_SYNTHETIC));
    printf("ANNOTATION Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_ANNOTATION));
    printf("ENUM Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_ENUM));

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

