#ifndef METHODS_H
#define METHODS_H

#include "attributes.h"
#include "membuff.h"

struct method {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    JavaClassAttributes *attributes;
};

struct method_items {
    uint16_t count;
    struct method *items[];
};

void methods_parse(MemReader *reader, struct method_items **obj);
void methods_free(struct method_items *this);

#endif
