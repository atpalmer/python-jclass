#include <Python.h>
#include "access.h"
#include "attributes.h"
#include "fields.h"
#include "membuff.h"


struct field_items *fields_parse(MemReader *reader) {
    uint16_t count = MemReader_next_uint16(reader);
    struct field_items *obj = PyMem_Malloc(sizeof(struct field_items) + (sizeof(struct field *) * count));
    obj->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        struct field **field = &obj->items[i];

        *field = PyMem_Malloc(sizeof(struct field));

        (*field)->access_flags = MemReader_next_uint16(reader);
        (*field)->name_index = MemReader_next_uint16(reader);
        (*field)->descriptor_index = MemReader_next_uint16(reader);
        (*field)->attributes = attributes_parse(reader);
    }

    return obj;
}

static void field_free(struct field *this) {
    if(!this)
        return
    attributes_free(this->attributes);
    PyMem_Free(this);
}

void fields_free(struct field_items *this) {
    if(!this)
        return;
    for(uint16_t i = 0; i < this->count; ++i)
        field_free(this->items[i]);
    PyMem_Free(this);
}
