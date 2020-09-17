#include <Python.h>
#include "membuff.h"
#include "constant_pool.h"


static struct pool_CONSTANT *_read_Utf8(MemReader *reader) {
    uint8_t tag = MemReader_next_uint8(reader);
    uint16_t length = MemReader_next_uint16(reader);

    struct pool_Utf8 *new = PyMem_Malloc(sizeof(*new) + length);
    new->tag = tag;
    new->length = length;
    MemReader_copy_next(reader, length, new->bytes);

    return (struct pool_CONSTANT *)new;
}

static struct pool_CONSTANT *_read_Class(MemReader *reader) {
    struct pool_Class *new = PyMem_Malloc(sizeof(*new));
    new->tag = MemReader_next_uint8(reader);
    new->name_index = MemReader_next_uint16(reader);
    return (struct pool_CONSTANT *)new;
}

static struct pool_CONSTANT *_read_String(MemReader *reader) {
    struct pool_String *new = PyMem_Malloc(sizeof(*new));
    new->tag = MemReader_next_uint8(reader);
    new->string_index = MemReader_next_uint16(reader);
    return (struct pool_CONSTANT *)new;
}

static struct pool_CONSTANT *_read_Fieldref(MemReader *reader) {
    struct pool_Fieldref *new = PyMem_Malloc(sizeof(*new));
    new->tag = MemReader_next_uint8(reader);
    new->class_index = MemReader_next_uint16(reader);
    new->name_and_type_index = MemReader_next_uint16(reader);
    return (struct pool_CONSTANT *)new;
}

static struct pool_CONSTANT *_read_Methodref(MemReader *reader) {
    return _read_Fieldref(reader);
}

static struct pool_CONSTANT *_read_InterfaceMethodref(MemReader *reader) {
    return _read_Fieldref(reader);
}

static struct pool_CONSTANT *_read_NameAndType(MemReader *reader) {
    struct pool_NameAndType *new = PyMem_Malloc(sizeof(*new));
    new->tag = MemReader_next_uint8(reader);
    new->name_index = MemReader_next_uint16(reader);
    new->descriptor_index = MemReader_next_uint16(reader);
    return (struct pool_CONSTANT *)new;
}

struct constant_pool *constant_pool_parse(MemReader *reader) {
    uint16_t count = MemReader_next_uint16(reader);
    struct constant_pool *obj = PyMem_Malloc(sizeof(struct constant_pool) + (sizeof(struct pool_CONSTANT *) * count));
    obj->count = count;

    for(uint16_t i = 1; i < count; ++i) {
        struct pool_CONSTANT **c = &obj->items[i - 1];
        uint8_t constant_type = MemReader_peek_uint8(reader);

        switch(constant_type) {
        case CONSTANT_TAG_Utf8:
            *c = _read_Utf8(reader);
            break;
        case CONSTANT_TAG_Class:
            *c = _read_Class(reader);
            break;
        case CONSTANT_TAG_String:
            *c = _read_String(reader);
            break;
        case CONSTANT_TAG_Fieldref:
            *c = _read_Fieldref(reader);
            break;
        case CONSTANT_TAG_Methodref:
            *c = _read_Methodref(reader);
            break;
        case CONSTANT_TAG_InterfaceMethodref:
            *c = _read_InterfaceMethodref(reader);
            break;
        case CONSTANT_TAG_NameAndType:
            *c = _read_NameAndType(reader);
            break;
        default:
            *c = NULL;
            break;
        }
    }

    return obj;
}

void constant_pool_free(struct constant_pool *this) {
    for(int i = 0; i < this->count - 1; ++i)
        if(this->items[i])
            PyMem_Free(this->items[i]);
    PyMem_Free(this);
}
