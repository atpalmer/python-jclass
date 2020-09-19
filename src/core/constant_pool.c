#include "membuff.h"
#include "constant_pool.h"
#include "mem.h"

void *constant_pool_item(struct constant_pool *this, uint16_t i) {
    if(i < 1 || i > this->count - 1)
        return NULL;
    return this->items[i - 1];
}

static struct pool_CONSTANT *_read_Utf8(struct membuff *reader) {
    uint8_t tag = membuff_next_uint8(reader);
    uint16_t length = membuff_next_uint16(reader);

    struct pool_Utf8 *new = mem_malloc(sizeof(*new) + length);
    if(!new)
        return NULL;

    new->tag = tag;
    new->length = length;
    membuff_copy_next(reader, length, new->bytes);

    return (struct pool_CONSTANT *)new;
}

static struct pool_CONSTANT *_read_Class(struct membuff *reader) {
    struct pool_Class *new = mem_malloc(sizeof(*new));
    if(!new)
        return NULL;

    new->tag = membuff_next_uint8(reader);
    new->name_index = membuff_next_uint16(reader);

    return (struct pool_CONSTANT *)new;
}

static struct pool_CONSTANT *_read_String(struct membuff *reader) {
    struct pool_String *new = mem_malloc(sizeof(*new));
    if(!new)
        return NULL;

    new->tag = membuff_next_uint8(reader);
    new->string_index = membuff_next_uint16(reader);

    return (struct pool_CONSTANT *)new;
}

static struct pool_CONSTANT *_read_Fieldref(struct membuff *reader) {
    struct pool_Fieldref *new = mem_malloc(sizeof(*new));
    if(!new)
        return NULL;

    new->tag = membuff_next_uint8(reader);
    new->class_index = membuff_next_uint16(reader);
    new->name_and_type_index = membuff_next_uint16(reader);

    return (struct pool_CONSTANT *)new;
}

static struct pool_CONSTANT *_read_Methodref(struct membuff *reader) {
    return _read_Fieldref(reader);
}

static struct pool_CONSTANT *_read_InterfaceMethodref(struct membuff *reader) {
    return _read_Fieldref(reader);
}

static struct pool_CONSTANT *_read_NameAndType(struct membuff *reader) {
    struct pool_NameAndType *new = mem_malloc(sizeof(*new));
    if(!new)
        return NULL;

    new->tag = membuff_next_uint8(reader);
    new->name_index = membuff_next_uint16(reader);
    new->descriptor_index = membuff_next_uint16(reader);

    return (struct pool_CONSTANT *)new;
}

static struct pool_CONSTANT *_read_CONSTANT(struct membuff *reader) {
    uint8_t constant_tag = membuff_peek_uint8(reader);

    switch(constant_tag) {
    case CONSTANT_TAG_Utf8:
        return _read_Utf8(reader);
    case CONSTANT_TAG_Class:
        return _read_Class(reader);
    case CONSTANT_TAG_String:
        return _read_String(reader);
    case CONSTANT_TAG_Fieldref:
        return _read_Fieldref(reader);
    case CONSTANT_TAG_Methodref:
        return _read_Methodref(reader);
    case CONSTANT_TAG_InterfaceMethodref:
        return _read_InterfaceMethodref(reader);
    case CONSTANT_TAG_NameAndType:
        return _read_NameAndType(reader);
    default:
        return NULL;
    }
}

struct constant_pool *constant_pool_parse(struct membuff *reader) {
    uint16_t count = membuff_next_uint16(reader);
    struct constant_pool *obj = mem_malloc(sizeof(struct constant_pool) + (sizeof(struct pool_CONSTANT *) * count));
    if(!obj)
        return NULL;

    obj->count = count;

    for(uint16_t i = 1; i < count; ++i) {
        struct pool_CONSTANT *c = _read_CONSTANT(reader);
        if(!c)
            goto fail;
        obj->items[i - 1] = c;
    }

    return obj;

fail:
    constant_pool_free(obj);
    return NULL;
}

void constant_pool_free(struct constant_pool *this) {
    if(!this)
        return;
    for(int i = 0; i < this->count - 1; ++i)
        mem_free(this->items[i]);
    mem_free(this);
}
