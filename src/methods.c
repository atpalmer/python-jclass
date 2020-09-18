#include <stdlib.h>
#include "access.h"
#include "attributes.h"
#include "methods.h"
#include "membuff.h"


struct method_items *methods_parse(MemReader *reader) {
    uint16_t count = MemReader_next_uint16(reader);
    struct method_items *obj = malloc(sizeof(struct method_items) + (sizeof(struct method *) * count));
    if(!obj)
        return NULL;
    obj->count = count;

    for(int i = 0; i < count; ++i) {
        struct method **method = &obj->items[i];

        *method = malloc(sizeof(struct method));
        if(!*method)
            goto fail;

        (*method)->access_flags = MemReader_next_uint16(reader);
        (*method)->name_index = MemReader_next_uint16(reader);
        (*method)->descriptor_index = MemReader_next_uint16(reader);
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
    free(this);
}

void methods_free(struct method_items *this) {
    if(!this)
        return;
    for(uint16_t i = 0; i < this->count; ++i)
        method_free(this->items[i]);
    free(this);
}
