#ifndef JCLASS_ATTRIBUTES_H
#define JCLASS_ATTRIBUTES_H

#include "constant_pool.h"

struct attr_BASE {
    int is_raw;
    struct pool_Utf8 *name;
};

struct attr_ConstantValue {
    int is_raw;
    struct pool_Utf8 *name;
    struct pool_CONSTANT *constant;
};

struct attr_SourceFile {
    int is_raw;
    struct pool_Utf8 *name;
    struct pool_Utf8 *sourcefile;
};

struct attr_RAW {
    int is_raw;
    struct pool_Utf8 *name;
    uint32_t length;
    uint8_t info[];
};

struct attribute_items {
    uint16_t count;
    struct attr_BASE *items[];
};

struct attribute_items *attributes_parse(FILE *reader, struct constant_pool *pool);
void attributes_free(struct attribute_items *this);

#endif
