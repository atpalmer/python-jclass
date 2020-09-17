#ifndef FIELDS_H
#define FIELDS_H

#include "attributes.h"
#include "membuff.h"

struct field {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    JavaClassAttributes *attributes;
};

struct field_items {
    uint16_t count;
    struct field *items[];
};

void fields_parse(MemReader *reader, struct field_items **obj);
void fields_free(struct field_items *this);

#endif
