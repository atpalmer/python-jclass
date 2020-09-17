#include <Python.h>
#include "access.h"
#include "attributes.h"
#include "fields.h"
#include "membuff.h"


void fields_parse(MemReader *reader, struct field_items **obj) {
    uint16_t count = MemReader_next_uint16(reader);
    *obj = PyMem_Malloc(sizeof(struct field_items) + (sizeof(struct field *) * count));
    (*obj)->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        struct field **field = &(*obj)->items[i];

        *field = PyMem_Malloc(sizeof(struct field));

        (*field)->access_flags = MemReader_next_uint16(reader);
        (*field)->name_index = MemReader_next_uint16(reader);
        (*field)->descriptor_index = MemReader_next_uint16(reader);

        struct attribute_items *attributes;
        attributes_parse(reader, &attributes);

        (*field)->attributes = attributes;
    }
}

static void field_free(struct field *this) {
    attributes_free(this->attributes);
    PyMem_Free(this);
}

void fields_free(struct field_items *this) {
    for(uint16_t i = 0; i < this->count; ++i) {
        field_free(this->items[i]);
    }
    PyMem_Free(this);
}
