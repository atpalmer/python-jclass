#ifndef INTERFACES_H
#define INTERFACES_H

#include "membuff.h"
#include "constant_pool.h"

struct interfaces {
    uint16_t count;
    uint16_t indexes[];
};

struct interfaces *interfaces_ensure_integrity(struct interfaces *this, struct constant_pool *pool);
struct interfaces *interfaces_parse(struct membuff *reader);
void interfaces_free(struct interfaces *this);

#endif
