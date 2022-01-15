#ifndef JCLASS_INTERFACES_H
#define JCLASS_INTERFACES_H

#include "constant_pool.h"

struct interfaces {
    uint16_t count;
    uint16_t indexes[];
};

struct interfaces *interfaces_parse(FILE *reader, struct constant_pool *pool);
void interfaces_free(struct interfaces *this);

#endif
