#include <stdlib.h>
#include "access.h"
#include "attributes.h"
#include "fields.h"
#include "membuff.h"


struct field_items *fields_parse(MemReader *reader) {
    uint16_t count = MemReader_next_uint16(reader);
    struct field_items *obj = calloc(1, sizeof(struct field_items) + (sizeof(struct field *) * count));
    if(!obj)
        return NULL;
    obj->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        struct field **field = &obj->items[i];

        *field = malloc(sizeof(struct field));
        if(!*field)
            goto fail;

        (*field)->access_flags = MemReader_next_uint16(reader);
        (*field)->name_index = MemReader_next_uint16(reader);
        (*field)->descriptor_index = MemReader_next_uint16(reader);
        (*field)->attributes = attributes_parse(reader);
    }

    return obj;

fail:
    fields_free(obj);
    return NULL;
}

static void field_free(struct field *this) {
    if(!this)
        return
    attributes_free(this->attributes);
    free(this);
}

void fields_free(struct field_items *this) {
    if(!this)
        return;
    for(uint16_t i = 0; i < this->count; ++i)
        field_free(this->items[i]);
    free(this);
}
