#include <stdlib.h>
#include "attributes.h"
#include "membuff.h"


struct attribute_items *attributes_parse(MemReader *reader) {
    uint16_t count = MemReader_next_uint16(reader);
    struct attribute_items *obj = calloc(1, sizeof(struct attribute_items) + (sizeof(struct attribute *) * count));
    obj->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        struct attribute **attr = &obj->items[i];

        uint16_t name_index = MemReader_next_uint16(reader);
        uint32_t length = MemReader_next_uint32(reader);

        *attr = malloc(sizeof(struct attribute) + length);

        (*attr)->name_index = name_index;
        (*attr)->length = length;

        MemReader_copy_next(reader, length, (*attr)->info);
    }

    return obj;
}

void attributes_free(struct attribute_items *this) {
    if(!this)
        return;
    for(uint16_t i = 0; i < this->count; ++i)
        if(this->items[i])
            free(this->items[i]);
    free(this);
}
