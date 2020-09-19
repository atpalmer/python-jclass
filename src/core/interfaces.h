#ifndef INTERFACES_H
#define INTERFACES_H

#include "membuff.h"

struct interfaces {
    uint16_t count;
    uint16_t indexes[];
};

struct interfaces *interfaces_parse(struct membuff *reader);
void interfaces_free(struct interfaces *this);

#endif
