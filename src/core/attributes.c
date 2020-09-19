#include <stdlib.h>
#include "attributes.h"
#include "membuff.h"


struct attribute_items *attributes_parse(struct membuff *reader) {
    uint16_t count = membuff_next_uint16(reader);
    struct attribute_items *obj = calloc(1, sizeof(struct attribute_items) + (sizeof(struct attribute *) * count));
    if(!obj)
        return NULL;
    obj->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        struct attribute **attr = &obj->items[i];

        uint16_t name_index = membuff_next_uint16(reader);
        uint32_t length = membuff_next_uint32(reader);

        *attr = malloc(sizeof(struct attribute) + length);
        if(!*attr)
            goto fail;

        (*attr)->name_index = name_index;
        (*attr)->length = length;

        membuff_copy_next(reader, length, (*attr)->info);
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
        if(this->items[i])
            free(this->items[i]);
    free(this);
}
