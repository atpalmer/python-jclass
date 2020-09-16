#include <Python.h>
#include "membuff.h"
#include "constant_pool.h"


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

static JavaClassConstant *_JavaClassStringConstant_from_reader(MemReader *reader) {
    JavaClassStringConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = MemReader_next_uint8(reader);
    new->string_index = MemReader_next_uint16(reader);
    return (JavaClassConstant *)new;
}

static JavaClassConstant *_JavaClassFieldrefConstant_from_reader(MemReader *reader) {
    JavaClassFieldrefConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = MemReader_next_uint8(reader);
    new->class_index = MemReader_next_uint16(reader);
    new->name_and_type_index = MemReader_next_uint16(reader);
    return (JavaClassConstant *)new;
}

static JavaClassConstant *_JavaClassMethodrefConstant_from_reader(MemReader *reader) {
    JavaClassMethodrefConstant *new = (JavaClassMethodrefConstant *)_JavaClassFieldrefConstant_from_reader(reader);
    return (JavaClassConstant *)new;
}

static JavaClassConstant *_JavaClassInterfaceMethodrefConstant_from_reader(MemReader *reader) {
    JavaClassInterfaceMethodrefConstant *new = (JavaClassInterfaceMethodrefConstant *)_JavaClassFieldrefConstant_from_reader(reader);
    return (JavaClassConstant *)new;
}

static JavaClassConstant *_JavaClassNameAndTypeConstant_from_reader(MemReader *reader) {
    JavaClassNameAndTypeConstant *new = PyMem_Malloc(sizeof(*new));
    new->tag = MemReader_next_uint8(reader);
    new->name_index = MemReader_next_uint16(reader);
    new->descriptor_index = MemReader_next_uint16(reader);
    return (JavaClassConstant *)new;
}

void constant_pool_parse(MemReader *reader, JavaClassConstantPool **obj) {
    uint16_t count = MemReader_next_uint16(reader);

    *obj = PyMem_Malloc(sizeof(JavaClassConstantPool) + (sizeof(JavaClassConstant *) * count));
    (*obj)->count = count;

    for(uint16_t i = 1; i < count; ++i) {
        JavaClassConstant **c = &((*obj)->constants[i - 1]);
        uint8_t constant_type = MemReader_peek_uint8(reader);

        switch(constant_type) {
        case CONSTANT_TYPE_Utf8:
            *c = _JavaClassUtf8Constant_from_reader(reader);
            break;
        case CONSTANT_TYPE_Class:
            *c = _JavaClassClassConstant_from_reader(reader);
            break;
        case CONSTANT_TYPE_String:
            *c = _JavaClassStringConstant_from_reader(reader);
            break;
        case CONSTANT_TYPE_Fieldref:
            *c = _JavaClassFieldrefConstant_from_reader(reader);
            break;
        case CONSTANT_TYPE_Methodref:
            *c = _JavaClassMethodrefConstant_from_reader(reader);
            break;
        case CONSTANT_TYPE_InterfaceMethodref:
            *c = _JavaClassInterfaceMethodrefConstant_from_reader(reader);
            break;
        case CONSTANT_TYPE_NameAndType:
            *c = _JavaClassNameAndTypeConstant_from_reader(reader);
            break;
        default:
            *c = NULL;
            break;
        }
    }
}

void JavaClassConstantPool_free(JavaClassConstantPool *this) {
    for(int i = 0; i < this->count - 1; ++i)
        if(this->constants[i])
            PyMem_Free(this->constants[i]);
    PyMem_Free(this);
}
