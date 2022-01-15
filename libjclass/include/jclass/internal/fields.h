#ifndef JCLASS_FIELDS_H
#define JCLASS_FIELDS_H

#include "attributes.h"

struct field {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    struct attribute_items *attributes;
};

struct field_items {
    uint16_t count;
    struct field *items[];
};

struct field_items *fields_parse(FILE *reader, struct constant_pool *pool);
void fields_free(struct field_items *this);

#endif
