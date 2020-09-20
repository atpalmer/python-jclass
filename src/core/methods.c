#include "access.h"
#include "attributes.h"
#include "methods.h"
#include "membuff.h"
#include "mem.h"

struct method_items *methods_ensure_integrity(struct method_items *this, struct constant_pool *pool) {
    if(!this)
        return NULL;
    for(uint16_t i = 0; i < this->count; ++i) {
        struct method *method = this->items[i];
        if(!method)
            return NULL;
        if(!constant_pool_item(pool, method->name_index))
            return NULL;
        if(!constant_pool_item(pool, method->descriptor_index))
            return NULL;
        if(!attributes_ensure_integrity(method->attributes, pool))
            return NULL;
    }
    return this;
}

struct method_items *methods_parse(struct membuff *reader) {
    uint16_t count = membuff_next_uint16(reader);
    struct method_items *obj = mem_malloc(sizeof(struct method_items) + (sizeof(struct method *) * count));
    if(!obj)
        return NULL;
    obj->count = count;

    for(int i = 0; i < count; ++i) {
        struct method **method = &obj->items[i];

        *method = mem_malloc(sizeof(struct method));
        if(!*method)
            goto fail;

        (*method)->access_flags = membuff_next_uint16(reader);
        (*method)->name_index = membuff_next_uint16(reader);
        (*method)->descriptor_index = membuff_next_uint16(reader);
        (*method)->attributes = attributes_parse(reader);
    }

    return obj;

fail:
    methods_free(obj);
    return NULL;
}

static void method_free(struct method *this) {
    if(!this)
        return;
    attributes_free(this->attributes);
    mem_free(this);
}

void methods_free(struct method_items *this) {
    if(!this)
        return;
    for(uint16_t i = 0; i < this->count; ++i)
        method_free(this->items[i]);
    mem_free(this);
}
