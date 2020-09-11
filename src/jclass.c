#import <Python.h>

enum constant_type {
    CONSTANT_TYPE_Utf8= 1,
    CONSTANT_TYPE_Class = 7,
    CONSTANT_TYPE_String = 8,
    CONSTANT_TYPE_Fieldref = 9,
    CONSTANT_TYPE_Methodref = 10,
    CONSTANT_TYPE_InterfaceMethodref = 11,
    CONSTANT_TYPE_NameAndType = 12,
};

static const char *Constant_description[] = {
    [0] = "",
    [1] = "CONSTANT_Utf8",
    [2] = NULL,
    [3] = NULL,
    [4] = NULL,
    [5] = NULL,
    [6] = NULL,
    [7] = "CONSTANT_Class",
    [8] = "CONSTANT_String",
    [9] = "CONSTANT_Fieldref",
    [10] = "CONSTANT_Methodref",
    [11] = "CONSTANT_InterfaceMethodref",
    [12] = "CONSTANT_NameAndType",
    [13] = NULL,
    [14] = NULL,
    [15] = NULL,
    [16] = NULL,
};

#define UINT8(p)    (*(p))
#define UINT16(p)   (ntohs(*(uint16_t *)p))

#define UINT8_AT(p, i)      UINT8(&(((uint8_t *)(p))[(i)]))
#define UINT16_AT(p, i)     UINT16(&(((uint8_t *)(p))[(i)]))
#define POINTER_AT(p, i)    ((void *)&(((uint8_t *)(p))[(i)]))


/* TODO: make PyObject */
typedef struct {
    /* ptrs to .data[] (big-endian order!) */
    uint32_t *magic_number;
    uint16_t *minor_version;
    uint16_t *major_version;
    uint16_t *constant_pool_count;
    uint8_t *constant_pool;

    Py_ssize_t size;
    uint8_t data[];
} JavaClass;

/* common header byte */

static inline uint8_t Constant_tag(void *head) {
    return UINT8_AT(head, 0);
}


/* 1 = Utf8 */

static inline uint16_t Utf8_length(void *head) {
    return UINT16_AT(head, 1);
}

static inline char *Utf8_bytes(void *head) {
    return POINTER_AT(head, 3);
}


/* 7 = Class */

static inline uint16_t Class_name_index(void *head) {
    return UINT16_AT(head, 1);
}


/* 9 = Fieldref */

static inline uint16_t Fieldref_class_index(void *head) {
    return UINT16_AT(head, 1);
}

static inline uint16_t Fieldref_name_and_type_index(void *head) {
    return UINT16_AT(head, 3);
}


/* 10 = Methodref */

static inline uint16_t Methodref_class_index(void *head) {
    return UINT16_AT(head, 1);
}

static inline uint16_t Methodref_name_and_type_index(void *head) {
    return UINT16_AT(head, 3);
}


/* 12 = NameAndType */

static inline uint16_t NameAndType_name_index(void *head) {
    return UINT16_AT(head, 1);
}

static inline uint16_t NameAndType_descriptor_index(void *head) {
    return UINT16_AT(head, 3);
}


#define CONSTANT_DESC(c)    (Constant_description[Constant_tag(c)])

static void parse_constant_pool(uint8_t *pool, int count) {
    size_t pool_bytes = 0;
    for(int i = count; i > 0; --i) {
        uint8_t *c = &pool[pool_bytes];
        printf("*BYTES INDEX: %u;TAG: %u (%s)\n", pool_bytes, Constant_tag(c), CONSTANT_DESC(c));

        if(Constant_tag(c) == CONSTANT_TYPE_Utf8) {
            printf("**tag(%u), length(%u), bytes(\"%.*s\")\n",
                Constant_tag(c), Utf8_length(c), Utf8_length(c), Utf8_bytes(c));
            pool_bytes += 3 + Utf8_length(c);
            continue;
        }

        if(Constant_tag(c) == CONSTANT_TYPE_Class) {
            printf("**tag(%u), name_index(%u)\n", Constant_tag(c), Class_name_index(c));
            pool_bytes += 3;
            continue;
        }

        if(Constant_tag(c) == CONSTANT_TYPE_Fieldref) {
            printf("**tag(%u), class_index(%u), name_and_type_index(%u)\n",
                Constant_tag(c), Fieldref_class_index(c), Fieldref_name_and_type_index(c));
            pool_bytes += 5;
            continue;
        }

        if(Constant_tag(c) == CONSTANT_TYPE_Methodref) {
            printf("**tag(%u), class_index(%u), name_and_type_index(%u)\n",
                Constant_tag(c), Methodref_class_index(c), Methodref_name_and_type_index(c));
            pool_bytes += 5;
            continue;
        }

        if(Constant_tag(c) == CONSTANT_TYPE_NameAndType) {
            printf("**tag(%u), name_index(%u), descriptor_index(%u)\n",
                Constant_tag(c), NameAndType_name_index(c), NameAndType_descriptor_index(c));
            pool_bytes += 5;
            continue;
        }


        break;
    }

    return;
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

    parse_constant_pool(class->constant_pool, ntohs(*class->constant_pool_count));

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

