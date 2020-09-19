#include "access.h"
#include "attributes.h"
#include "fields.h"
#include "membuff.h"
#include "mem.h"


struct field_items *fields_parse(struct membuff *reader) {
    uint16_t count = membuff_next_uint16(reader);
    struct field_items *obj = mem_calloc(1, sizeof(struct field_items) + (sizeof(struct field *) * count));
    if(!obj)
        return NULL;
    obj->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        struct field **field = &obj->items[i];

        *field = mem_malloc(sizeof(struct field));
        if(!*field)
            goto fail;

        (*field)->access_flags = membuff_next_uint16(reader);
        (*field)->name_index = membuff_next_uint16(reader);
        (*field)->descriptor_index = membuff_next_uint16(reader);
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
    mem_free(this);
}

void fields_free(struct field_items *this) {
    if(!this)
        return;
    for(uint16_t i = 0; i < this->count; ++i)
        field_free(this->items[i]);
    mem_free(this);
}
