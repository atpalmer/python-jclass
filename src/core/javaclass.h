#ifndef JAVACLASS_H
#define JAVACLASS_H

#include "access.h"
#include "constant_pool.h"
#include "interfaces.h"
#include "fields.h"
#include "methods.h"
#include "attributes.h"

struct javaclass {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    struct constant_pool *pool;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;
    struct interfaces *interfaces;
    struct field_items *fields;
    struct method_items *methods;
    struct attribute_items *attributes;
};

struct javaclass *javaclass_from_filename(const char *filename);
void javaclass_free(struct javaclass *this);

#endif
