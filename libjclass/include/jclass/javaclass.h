#ifndef JCLASS_JAVACLASS_H
#define JCLASS_JAVACLASS_H

#include "internal/access.h"
#include "internal/constant_pool.h"
#include "internal/interfaces.h"
#include "internal/fields.h"
#include "internal/methods.h"
#include "internal/attributes.h"

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
