#include <Python.h>
#include "parse.h"
#include "constant_pool.h"
#include "membuff.h"


static inline size_t parse32(void *data, uint32_t *target) {
    *target = UINT32(data);
    return 4;
}

static inline size_t parse16(void *data, uint16_t *target) {
    *target = UINT16(data);
    return 2;
}


/* CONSTANT POOL */

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


/* 8 = String */

static inline uint16_t String_string_index(void *head) {
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


/* print functions */

static void _JavaClassUtf8Constant_print(JavaClassUtf8Constant *this) {
    printf("Utf8=tag(%u), length(%u), bytes(\"%.*s\")\n",
        this->tag, this->length, this->length, this->bytes);
}

static void _JavaClassClassConstant_print(JavaClassClassConstant *this) {
    printf("Class=tag(%u), name_index(%u)\n",
        this->tag, this->name_index);
}

static void _JavaClassStringConstant_print(JavaClassStringConstant *this) {
    printf("String=tag(%u), string_index(%u)\n",
        this->tag, this->string_index);
}

static void _JavaClassFieldrefConstant_print(JavaClassFieldrefConstant *this) {
    printf("Fieldref=tag(%u), class_index(%u), name_and_type_index(%u)\n",
        this->tag, this->class_index, this->name_and_type_index);
}

static void _JavaClassMethodrefConstant_print(JavaClassMethodrefConstant *this) {
    printf("Methodref=tag(%u), class_index(%u), name_and_type_index(%u)\n",
        this->tag, this->class_index, this->name_and_type_index);
}

static void _JavaClassInterfaceMethodrefConstant_print(JavaClassInterfaceMethodrefConstant *this) {
    printf("InterfaceMethodref=tag(%u), class_index(%u), name_and_type_index(%u)\n",
        this->tag, this->class_index, this->name_and_type_index);
}

static void _JavaClassNameAndTypeConstant_print(JavaClassNameAndTypeConstant *this) {
    printf("NameAndType=tag(%u), name_index(%u), descriptor_index(%u)\n",
        this->tag, this->name_index, this->descriptor_index);
}

void constant_pool_print(JavaClassConstantPool *this) {
    printf("Constant Pool Count: %d\n", this->constant_pool_count);

    for(int i = 1; i < this->constant_pool_count; ++i) {
        void *c = this->constant_pool[i - 1];

        printf("  **%4d.) ", i);
        if(!c) {
            /* TODO: this branch should be deleted when all types are implemented */
            printf("Not implemented.\n");
            continue;
        }

        switch(((JavaClassConstant *)c)->tag) {
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


/* parser functions */

static JavaClassConstant *_JavaClassUtf8Constant_from_reader(MemReader *reader) {
    uint8_t tag = MemReader_next_uint8(reader);
    uint16_t length = MemReader_next_uint16(reader);

    JavaClassUtf8Constant *new = PyMem_Malloc(sizeof(*new) + length);
    new->tag = tag;
    new->length = length;
    MemReader_copy_next(reader, length, new->bytes);

    return (JavaClassConstant *)new;
}

static JavaClassConstant *_JavaClassClassConstant_from_reader(MemReader *reader) {
    JavaClassClassConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = MemReader_next_uint8(reader);
    new->name_index = MemReader_next_uint16(reader);
    return (JavaClassConstant *)new;
}

static JavaClassConstant *_JavaClassStringConstant_from_data(uint8_t *data) {
    JavaClassStringConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = Constant_tag(data);
    new->string_index = String_string_index(data);
    return (JavaClassConstant *)new;
}

static JavaClassConstant *_JavaClassFieldrefConstant_from_data(uint8_t *data) {
    JavaClassFieldrefConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = Constant_tag(data);
    new->class_index = Fieldref_class_index(data);
    new->name_and_type_index = Fieldref_name_and_type_index(data);
    return (JavaClassConstant *)new;
}

static JavaClassConstant *_JavaClassMethodrefConstant_from_data(uint8_t *data) {
    JavaClassMethodrefConstant *new = (JavaClassMethodrefConstant *)_JavaClassFieldrefConstant_from_data(data);
    return (JavaClassConstant *)new;
}

static JavaClassConstant *_JavaClassInterfaceMethodrefConstant_from_data(uint8_t *data) {
    JavaClassInterfaceMethodrefConstant *new = (JavaClassInterfaceMethodrefConstant *)_JavaClassFieldrefConstant_from_data(data);
    return (JavaClassConstant *)new;
}

static JavaClassConstant *_JavaClassNameAndTypeConstant_from_data(uint8_t *data) {
    JavaClassNameAndTypeConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = Constant_tag(data);
    new->name_index = NameAndType_name_index(data);
    new->descriptor_index = NameAndType_descriptor_index(data);
    return (JavaClassConstant *)new;
}


void constant_pool_parse(MemReader *reader, JavaClassConstantPool **obj) {
    uint16_t count;
    reader->pos += parse16(MEMREADER_CURR(reader), &count);

    *obj = PyMem_Malloc(sizeof(JavaClassConstantPool) + (sizeof(JavaClassConstant *) * count));
    (*obj)->constant_pool_count = count;

    for(uint16_t i = 1; i < count; ++i) {
        uint8_t *p = MEMREADER_CURR(reader);
        JavaClassConstant **c = &((*obj)->constant_pool[i - 1]);

        switch(Constant_tag(p)) {
        case CONSTANT_TYPE_Utf8:
            *c = _JavaClassUtf8Constant_from_reader(reader);
            break;
        case CONSTANT_TYPE_Class:
            *c = _JavaClassClassConstant_from_reader(reader);
            break;

        case CONSTANT_TYPE_String:
            *c = _JavaClassStringConstant_from_data(p);
            reader->pos += 3;
            break;

        case CONSTANT_TYPE_Fieldref:
            *c = _JavaClassFieldrefConstant_from_data(p);
            reader->pos += 5;
            break;

        case CONSTANT_TYPE_Methodref:
            *c = _JavaClassMethodrefConstant_from_data(p);
            reader->pos += 5;
            break;

        case CONSTANT_TYPE_InterfaceMethodref:
            *c = _JavaClassInterfaceMethodrefConstant_from_data(p);
            reader->pos += 5;
            break;

        case CONSTANT_TYPE_NameAndType:
            *c = _JavaClassNameAndTypeConstant_from_data(p);
            reader->pos += 5;
            break;

        default:
            *c = NULL;
            break;
        }
    }
}

void JavaClassConstantPool_free(JavaClassConstantPool *this) {
    for(int i = 0; i < this->constant_pool_count - 1; ++i)
        if(this->constant_pool[i])
            PyMem_Free(this->constant_pool[i]);
    PyMem_Free(this);
}
