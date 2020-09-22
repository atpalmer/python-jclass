#include "error.h"
#include "attributes.h"
#include "constant_pool.h"
#include "membuff.h"
#include "mem.h"

static struct attr_BASE *_parse_attr_info_Raw(struct membuff *reader, uint32_t len, struct pool_Utf8 *name) {
    struct attr_RAW *new = mem_malloc(sizeof(*new) + len);
    if(!new)
        return NULL;

    new->is_raw = 1;
    new->name = name;
    new->length = len;
    membuff_copy_next(reader, len, new->info);
    return (struct attr_BASE *)new;
}

static struct attr_BASE *_parse_attr_info(struct membuff *reader, uint32_t len, uint16_t name_index, struct constant_pool *pool) {
    struct pool_Utf8 *name = constant_pool_Utf8_item(pool, name_index);
    if(!name) {
        javaclass_error_set(JAVACLASS_ERR_PARSE, "Invalid attribute name index");
        return NULL;
    }

    struct attr_BASE *new = _parse_attr_info_Raw(reader, len, name);
    if(!new)
        return NULL;
    return new;
}

struct attribute_items *attributes_parse(struct membuff *reader, struct constant_pool *pool) {
    uint16_t count = membuff_next_uint16(reader);
    struct attribute_items *obj = mem_calloc(1, sizeof(struct attribute_items) + (sizeof(struct attribute *) * count));
    if(!obj)
        return NULL;
    obj->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        struct attr_BASE **attr = &obj->items[i];

        uint16_t name_index = membuff_next_uint16(reader);
        uint32_t length = membuff_next_uint32(reader);

        *attr = _parse_attr_info(reader, length, name_index, pool);
        if(!*attr)
            goto fail;
    }

    return obj;

fail:
    attributes_free(obj);
    return NULL;
}

void attributes_free(struct attribute_items *this) {
    if(!this)
        return;
    for(uint16_t i = 0; i < this->count; ++i)
        mem_free(this->items[i]);
    mem_free(this);
}
