#ifndef FIELDS_H
#define FIELDS_H

#include "attributes.h"
#include "membuff.h"

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

struct field_items *fields_ensure_integrity(struct field_items *this, struct constant_pool *pool);
struct field_items *fields_parse(struct membuff *reader);
void fields_free(struct field_items *this);

#endif
