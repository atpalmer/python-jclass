#include <Python.h>
#include "constant_pool.h"
#include "fields.h"
#include "attributes.h"
#include "methods.h"

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

#define CLASS_HAS_ACCESS(class, flag)  ((class)->access_flags & (flag))

typedef struct {
    uint8_t tag;
} JavaClassConstant;

typedef struct {
    uint8_t tag;
    uint16_t length;
    char bytes[];
} JavaClassUtf8Constant;

typedef struct {
    uint8_t tag;
    uint16_t name_index;
} JavaClassClassConstant;

typedef struct {
    uint8_t tag;
    uint16_t string_index;
} JavaClassStringConstant;

typedef struct {
    uint8_t tag;
    uint16_t class_index;
    uint16_t name_and_type_index;
} JavaClassFieldrefConstant;

typedef JavaClassFieldrefConstant JavaClassMethodrefConstant;
typedef JavaClassFieldrefConstant JavaClassInterfaceMethodrefConstant;

typedef struct {
    uint8_t tag;
    uint16_t name_index;
    uint16_t descriptor_index;
} JavaClassNameAndTypeConstant;

/* TODO: make PyObject */
typedef struct {
    uint32_t magic_number;
    uint16_t minor_version;
    uint16_t major_version;

    uint16_t constant_pool_count;
    JavaClassConstant **constant_pool;

    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;

    uint16_t interfaces_count;
    uint16_t *interfaces;  /* interface_indexes */

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

static size_t parse_interfaces(uint8_t *data, int count, uint8_t **obj) {
    *obj = data;
    printf("Interfaces count: %d\n", count);
    return 2 * count;
}

static JavaClassConstant *_JavaClassUtf8Constant_from_data(uint8_t *data) {
    JavaClassUtf8Constant *new = PyMem_Malloc(sizeof(*new) + Utf8_length(data));
    new->tag = Constant_tag(data);
    new->length = Utf8_length(data);
    memcpy(new->bytes, Utf8_bytes(data), Utf8_length(data));
    return (JavaClassConstant *)new;
}

static void _JavaClassUtf8Constant_print(JavaClassUtf8Constant *this) {
    printf("Utf8=tag(%u), length(%u), bytes(\"%.*s\")\n",
        this->tag, this->length, this->length, this->bytes);
}


static JavaClassConstant *_JavaClassClassConstant_from_data(uint8_t *data) {
    JavaClassClassConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = Constant_tag(data);
    new->name_index = Class_name_index(data);
    return (JavaClassConstant *)new;
}

static void _JavaClassClassConstant_print(JavaClassClassConstant *this) {
    printf("Class=tag(%u), name_index(%u)\n",
        this->tag, this->name_index);
}


static JavaClassConstant *_JavaClassStringConstant_from_data(uint8_t *data) {
    JavaClassStringConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = Constant_tag(data);
    new->string_index = String_string_index(data);
    return (JavaClassConstant *)new;
}

static void _JavaClassStringConstant_print(JavaClassStringConstant *this) {
    printf("String=tag(%u), string_index(%u)\n",
        this->tag, this->string_index);
}


static JavaClassConstant *_JavaClassFieldrefConstant_from_data(uint8_t *data) {
    JavaClassFieldrefConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = Constant_tag(data);
    new->class_index = Fieldref_class_index(data);
    new->name_and_type_index = Fieldref_name_and_type_index(data);
    return (JavaClassConstant *)new;
}

static void _JavaClassFieldrefConstant_print(JavaClassFieldrefConstant *this) {
    printf("Fieldref=tag(%u), class_index(%u), name_and_type_index(%u)\n",
        this->tag, this->class_index, this->name_and_type_index);
}


static JavaClassConstant *_JavaClassMethodrefConstant_from_data(uint8_t *data) {
    JavaClassMethodrefConstant *new = (JavaClassMethodrefConstant *)_JavaClassFieldrefConstant_from_data(data);
    return (JavaClassConstant *)new;
}

static void _JavaClassMethodrefConstant_print(JavaClassMethodrefConstant *this) {
    printf("Methodref=tag(%u), class_index(%u), name_and_type_index(%u)\n",
        this->tag, this->class_index, this->name_and_type_index);
}


static JavaClassConstant *_JavaClassInterfaceMethodrefConstant_from_data(uint8_t *data) {
    JavaClassInterfaceMethodrefConstant *new = (JavaClassInterfaceMethodrefConstant *)_JavaClassFieldrefConstant_from_data(data);
    return (JavaClassConstant *)new;
}

static void _JavaClassInterfaceMethodrefConstant_print(JavaClassInterfaceMethodrefConstant *this) {
    printf("InterfaceMethodref=tag(%u), class_index(%u), name_and_type_index(%u)\n",
        this->tag, this->class_index, this->name_and_type_index);
}


static JavaClassConstant *_JavaClassNameAndTypeConstant_from_data(uint8_t *data) {
    JavaClassNameAndTypeConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = Constant_tag(data);
    new->name_index = NameAndType_name_index(data);
    new->descriptor_index = NameAndType_descriptor_index(data);
    return (JavaClassConstant *)new;
}

static void _JavaClassNameAndTypeConstant_print(JavaClassNameAndTypeConstant *this) {
    printf("NameAndType=tag(%u), name_index(%u), descriptor_index(%u)\n",
        this->tag, this->name_index, this->descriptor_index);
}


#define CONSTANT_AT(constants, i)                       ((constants)[(i) - 1])
#define CONSTANT_AT_MEMBER(constants, i, tp, member)    (((tp *)CONSTANT_AT((constants), (i)))->member)

static void print_constant_pool(JavaClassConstant **constants, int count) {
    for(int i = 1; i < count; ++i) {
        JavaClassConstant *c = constants[i - 1];

        printf("  **%4d.) ", i);
        if(!c) {
            /* TODO: this branch should be deleted when all types are implemented */
            printf("Not implemented.\n");
            continue;
        }

        switch(c->tag) {
        case CONSTANT_TYPE_Utf8:
            _JavaClassUtf8Constant_print(c);
            break;
        case CONSTANT_TYPE_Class:
            _JavaClassClassConstant_print(c);
            break;
        case CONSTANT_TYPE_String:
            _JavaClassStringConstant_print(c);
            break;
        case CONSTANT_TYPE_Fieldref:
            _JavaClassFieldrefConstant_print(c);
            break;
        case CONSTANT_TYPE_Methodref:
            _JavaClassMethodrefConstant_print(c);
            break;
        case CONSTANT_TYPE_InterfaceMethodref:
            _JavaClassInterfaceMethodrefConstant_print(c);
            break;
        case CONSTANT_TYPE_NameAndType:
            _JavaClassNameAndTypeConstant_print(c);
            break;
        default:
            break;
        }
    }
}

static size_t parse_constant_pool(uint8_t *pool, int count, JavaClassConstant ***obj) {
    *obj = PyMem_Malloc(sizeof(JavaClassConstant *) * count);

    printf("Constant Pool Count: %d\n", count);

    size_t pool_bytes = 0;
    for(int i = 1; i < count; ++i) {
        uint8_t *c = &pool[pool_bytes];

        switch(Constant_tag(c)) {
        case CONSTANT_TYPE_Utf8:
            CONSTANT_AT(*obj, i) = _JavaClassUtf8Constant_from_data(c);
            pool_bytes += 3 + Utf8_length(c);
            break;

        case CONSTANT_TYPE_Class:
            CONSTANT_AT(*obj, i) = _JavaClassClassConstant_from_data(c);
            pool_bytes += 3;
            break;

        case CONSTANT_TYPE_String:
            CONSTANT_AT(*obj, i) = _JavaClassStringConstant_from_data(c);
            pool_bytes += 3;
            break;

        case CONSTANT_TYPE_Fieldref:
            CONSTANT_AT(*obj, i) = _JavaClassFieldrefConstant_from_data(c);
            pool_bytes += 5;
            break;

        case CONSTANT_TYPE_Methodref:
            CONSTANT_AT(*obj, i) = _JavaClassMethodrefConstant_from_data(c);
            pool_bytes += 5;
            break;

        case CONSTANT_TYPE_InterfaceMethodref:
            CONSTANT_AT(*obj, i) = _JavaClassInterfaceMethodrefConstant_from_data(c);
            pool_bytes += 5;
            break;

        case CONSTANT_TYPE_NameAndType:
            CONSTANT_AT(*obj, i) = _JavaClassNameAndTypeConstant_from_data(c);
            pool_bytes += 5;
            break;

        default:
            CONSTANT_AT(*obj, i) = NULL;
            break;
        }
    }

    print_constant_pool(*obj, count);

    return pool_bytes;
}

static JavaClass *_JavaClass_from_filename(const char *filename) {
    JavaClass *new = PyMem_Malloc(sizeof(JavaClass) + 4096);

    FILE *fp = fopen(filename, "rb");
    new->size = fread(new->data, 1, 4096, fp);
    fclose(fp);

    return new;
}

static void _JavaClass_free(JavaClass *this) {
    /* TODO: free constant_pool items */
    for(int i = 0; i < this->constant_pool_count - 1; ++i)
        if(this->constant_pool[i])
            PyMem_Free(this->constant_pool[i]);
    PyMem_Free(this->constant_pool);
    PyMem_Free(this);
}

#define NEXT_PTR(last)   ((void *)(((uint8_t *)(last)) + sizeof(*(last))))

static PyObject *jclass_load(PyObject *self, PyObject *args) {
    char *fname;
    if(!PyArg_ParseTuple(args, "s", &fname))
        return NULL;

    JavaClass *class = _JavaClass_from_filename(fname);

    size_t curr_bytes = 0;

    curr_bytes += parse32(&class->data[curr_bytes], &class->magic_number);
    curr_bytes += parse16(&class->data[curr_bytes], &class->minor_version);
    curr_bytes += parse16(&class->data[curr_bytes], &class->major_version);

    printf("Magic Number: %X\n", class->magic_number);
    printf("Version: %u.%u\n", class->major_version, class->minor_version);

    curr_bytes += parse16(&class->data[curr_bytes], &class->constant_pool_count);
    curr_bytes += parse_constant_pool(&class->data[curr_bytes], class->constant_pool_count, &class->constant_pool);
    curr_bytes += parse16(&class->data[curr_bytes], &class->access_flags);
    curr_bytes += parse16(&class->data[curr_bytes], &class->this_class);
    curr_bytes += parse16(&class->data[curr_bytes], &class->super_class);

    printf("Access Flags: %u\n", class->access_flags);
    printf("* PUBLIC Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_PUBLIC));
    printf("* FINAL Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_FINAL));
    printf("* SUPER Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_SUPER));
    printf("* INTERFACE Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_INTERFACE));
    printf("* ABSTRACT Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_ABSTRACT));
    printf("* SYNTHETIC Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_SYNTHETIC));
    printf("* ANNOTATION Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_ANNOTATION));
    printf("* ENUM Flag: %x\n", CLASS_HAS_ACCESS(class, ACC_ENUM));

    printf("This Class Pool Index: %u\n", class->this_class);
    printf("Super Class Pool Index: %u\n", class->super_class);

    curr_bytes += parse16(&class->data[curr_bytes], &class->interfaces_count);
    curr_bytes += parse_interfaces(&class->data[curr_bytes], class->interfaces_count, &class->interfaces);
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

