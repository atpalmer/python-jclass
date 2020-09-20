#ifndef METHODS_H
#define METHODS_H

#include "attributes.h"
#include "membuff.h"

struct method {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    struct attribute_items *attributes;
};

struct method_items {
    uint16_t count;
    struct method *items[];
};

struct method_items *methods_ensure_integrity(struct method_items *this, struct constant_pool *pool);
struct method_items *methods_parse(struct membuff *reader);
void methods_free(struct method_items *this);

#endif
