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
    uint16_t *this_class;
    uint16_t *super_class;
    uint16_t *interfaces_count;
    uint16_t *interfaces;  /* interface_indexes */

    uint16_t *fields_count;
    uint8_t *fields;

    uint16_t *methods_count;
    uint8_t *methods;

    uint16_t *attributes_count;
    uint8_t *attributes;

    Py_ssize_t size;
    uint8_t data[];
} JavaClass;


static uint16_t Field_access_flags(void *head) {
    return UINT16_AT(head, 0);
}

static uint16_t Field_name_index(void *head) {
    return UINT16_AT(head, 2);
}

static uint16_t Field_descriptor_index(void *head) {
    return UINT16_AT(head, 4);
}

static uint16_t Field_attributes_count(void *head) {
    return UINT16_AT(head, 6);
}

static uint8_t *Field_attributes(void *head) {
    return POINTER_AT(head, 8);
}


static size_t parse_constant_pool(uint8_t *pool, int count) {
    size_t pool_bytes = 0;
    for(int i = 1; i < count; ++i) {
        uint8_t *c = &pool[pool_bytes];
        printf("%d.) %s=%u: bytes index: %zu\n", i, CONSTANT_DESC(c), Constant_tag(c), pool_bytes);

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

#define NEXT_PTR(last)   ((void *)(((uint8_t *)(last)) + sizeof(*(last))))

    class->magic_number = (void *)class->data;
    class->minor_version = NEXT_PTR(class->magic_number);
    class->major_version = NEXT_PTR(class->minor_version);
    class->constant_pool_count = NEXT_PTR(class->major_version);
    class->constant_pool = NEXT_PTR(class->constant_pool_count);

    printf("Magic Number: %X\n", ntohl(*class->magic_number));
    printf("Version: %u.%u\n", ntohs(*class->major_version), ntohs(*class->minor_version));
    printf("Constant Pool Count: %u\n", ntohs(*class->constant_pool_count));

    size_t pool_bytes = parse_constant_pool(class->constant_pool, ntohs(*class->constant_pool_count));

    class->access_flags = (void *)&class->constant_pool[pool_bytes];

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

    class->this_class = NEXT_PTR(class->access_flags);
    printf("This Class Pool Index: %u\n", ntohs(*class->this_class));

    class->super_class = NEXT_PTR(class->this_class);
    printf("Super Class Pool Index: %u\n", ntohs(*class->super_class));

    class->interfaces_count = NEXT_PTR(class->super_class);
    printf("Interfaces count: %u\n", ntohs(*class->interfaces_count));

    class->interfaces = NEXT_PTR(class->interfaces_count);
    /* interfaces items are fixed-width; skip parsing for now */

    class->fields_count = class->interfaces + ntohs(*class->interfaces_count);
    class->fields = NEXT_PTR(class->fields_count);

    printf("Fields count: %u\n", ntohs(*class->fields_count));

    size_t fields_bytes = 0;
    for(int i = 0; i < ntohs(*class->fields_count); ++i) {
        uint8_t *field = &class->fields[fields_bytes];
        printf("* Field access flags: %u\n", Field_access_flags(field));
        printf("* Field name index: %u\n", Field_name_index(field));
        printf("* Field descriptor index: %u\n", Field_descriptor_index(field));
        printf("* Field attributes count: %u\n", Field_attributes_count(field));
        break;
    }

    PyObject *result = PyBytes_FromStringAndSize((char *)class->data, class->size);
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

