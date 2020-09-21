#include "constant_pool.h"
#include "error.h"
#include "membuff.h"
#include "mem.h"

void *constant_pool_item(struct constant_pool *this, uint16_t i) {
    if(i < 1 || i > this->count - 1)
        return NULL;
    return this->items[i - 1];
}

/* 1 */
struct pool_Utf8 *constant_pool_Utf8_item(struct constant_pool *this, uint16_t i) {
    struct pool_CONSTANT *item = constant_pool_item(this, i);
    if(!item)
        return NULL;
    if(item->tag != CONSTANT_TAG_Utf8)
        return NULL;
    return (struct pool_Utf8 *)item;
}

/* 7 */
struct pool_Class *constant_pool_Class_item(struct constant_pool *this, uint16_t i) {
    struct pool_CONSTANT *item = constant_pool_item(this, i);
    if(!item)
        return NULL;
    if(item->tag != CONSTANT_TAG_Class)
        return NULL;
    return (struct pool_Class *)item;
}


/* 1 */
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

/* no 2 */

/* 3 */
static struct pool_CONSTANT *_read_Integer(struct membuff *reader) {
    struct pool_Integer *new = mem_malloc(sizeof(*new));
    if(!new)
        return NULL;

    new->tag = membuff_next_uint8(reader);
    new->bytes = membuff_next_uint32(reader);

    return (struct pool_CONSTANT *)new;
}

/* 4 */
static struct pool_CONSTANT *_read_Float(struct membuff *reader) {
    struct pool_Float *new = mem_malloc(sizeof(*new));
    if(!new)
        return NULL;

    new->tag = membuff_next_uint8(reader);
    new->bytes = membuff_next_uint32(reader);

    return (struct pool_CONSTANT *)new;
}

/* 5 */
static struct pool_CONSTANT *_read_Long(struct membuff *reader) {
    struct pool_Long *new = mem_malloc(sizeof(*new));
    if(!new)
        return NULL;

    new->tag = membuff_next_uint8(reader);
    uint32_t high = membuff_next_uint32(reader);
    uint32_t low = membuff_next_uint32(reader);
    new->bytes = ((uint64_t)high << 32) | low;

    return (struct pool_CONSTANT *)new;
}

/* 6 */
static struct pool_CONSTANT *_read_Double(struct membuff *reader) {
    struct pool_Double *new = mem_malloc(sizeof(*new));
    if(!new)
        return NULL;

    new->tag = membuff_next_uint8(reader);
    uint32_t high = membuff_next_uint32(reader);
    uint32_t low = membuff_next_uint32(reader);
    uint64_t bytes = ((uint64_t)high << 32) | low;
    new->bytes = *(double *)&bytes;

    return (struct pool_CONSTANT *)new;
}

/* 7 */
static struct pool_CONSTANT *_read_Class(struct membuff *reader) {
    struct pool_Class *new = mem_malloc(sizeof(*new));
    if(!new)
        return NULL;

    new->tag = membuff_next_uint8(reader);
    new->name_index = membuff_next_uint16(reader);

    return (struct pool_CONSTANT *)new;
}

/* 8 */
static struct pool_CONSTANT *_read_String(struct membuff *reader) {
    struct pool_String *new = mem_malloc(sizeof(*new));
    if(!new)
        return NULL;

    new->tag = membuff_next_uint8(reader);
    new->string_index = membuff_next_uint16(reader);

    return (struct pool_CONSTANT *)new;
}

/* 9 */
static struct pool_CONSTANT *_read_Fieldref(struct membuff *reader) {
    struct pool_Fieldref *new = mem_malloc(sizeof(*new));
    if(!new)
        return NULL;

    new->tag = membuff_next_uint8(reader);
    new->class_index = membuff_next_uint16(reader);
    new->name_and_type_index = membuff_next_uint16(reader);

    return (struct pool_CONSTANT *)new;
}

/* 10 */
static struct pool_CONSTANT *_read_Methodref(struct membuff *reader) {
    return _read_Fieldref(reader);
}

/* 11 */
static struct pool_CONSTANT *_read_InterfaceMethodref(struct membuff *reader) {
    return _read_Fieldref(reader);
}

/* 12 */
static struct pool_CONSTANT *_read_NameAndType(struct membuff *reader) {
    struct pool_NameAndType *new = mem_malloc(sizeof(*new));
    if(!new)
        return NULL;

    new->tag = membuff_next_uint8(reader);
    new->name_index = membuff_next_uint16(reader);
    new->descriptor_index = membuff_next_uint16(reader);

    return (struct pool_CONSTANT *)new;
}

/* no 13 */
/* no 14 */
/* TODO: 15 */
/* TODO: 16 */
/* no 17 */
/* TODO: 18 */

static struct pool_CONSTANT *_read_CONSTANT(struct membuff *reader) {
    uint8_t constant_tag = membuff_peek_uint8(reader);

    switch(constant_tag) {
    case CONSTANT_TAG_Utf8:  /* 1 */
        return _read_Utf8(reader);
    /* no 2 */
    case CONSTANT_TAG_Integer:  /* 3 */
        return _read_Integer(reader);
    case CONSTANT_TAG_Float:  /* 4 */
        return _read_Float(reader);
    case CONSTANT_TAG_Long:  /* 5 */
        return _read_Long(reader);
    case CONSTANT_TAG_Double:  /* 6 */
        return _read_Double(reader);
    case CONSTANT_TAG_Class:  /* 7 */
        return _read_Class(reader);
    case CONSTANT_TAG_String:  /* 8 */
        return _read_String(reader);
    case CONSTANT_TAG_Fieldref:  /* 9 */
        return _read_Fieldref(reader);
    case CONSTANT_TAG_Methodref:  /* 10 */
        return _read_Methodref(reader);
    case CONSTANT_TAG_InterfaceMethodref:  /* 11 */
        return _read_InterfaceMethodref(reader);
    case CONSTANT_TAG_NameAndType:  /* 12 */
        return _read_NameAndType(reader);
    /* no 13 */
    /* no 14 */
    /* TODO: 15 */
    /* TODO: 16 */
    /* no 17 */
    /* TODO: 18 */
    default:
        javaclass_error_set(JAVACLASS_ERR_PARSE, "Unknown constant tag");
        return NULL;
    }
}

struct constant_pool *constant_pool_parse(struct membuff *reader) {
    uint16_t count = membuff_next_uint16(reader);
    struct constant_pool *obj = mem_calloc(1, sizeof(struct constant_pool) + (sizeof(struct pool_CONSTANT *) * count));
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
    for(uint16_t i = 1; i < this->count; ++i)
        mem_free(this->items[i - 1]);
    mem_free(this);
}
