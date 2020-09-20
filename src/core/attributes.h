#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include "membuff.h"
#include "constant_pool.h"

struct attribute {
    uint16_t name_index;
    uint32_t length;
    uint8_t info[];
};

struct attribute_items {
    uint16_t count;
    struct attribute *items[];
};

struct attribute_items *attributes_ensure_integrity(struct attribute_items *this, struct constant_pool *pool);
struct attribute_items *attributes_parse(struct membuff *reader);
void attributes_free(struct attribute_items *this);

#endif
