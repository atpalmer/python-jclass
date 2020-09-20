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

struct attribute_items *attributes_parse(struct membuff *reader, struct constant_pool *pool);
void attributes_free(struct attribute_items *this);

#endif
