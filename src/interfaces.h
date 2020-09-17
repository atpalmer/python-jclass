#ifndef INTERFACES_H
#define INTERFACES_H

#include "membuff.h"

struct interfaces {
    uint16_t count;
    uint16_t indexes[];
};

void interfaces_parse(MemReader *reader, struct interfaces **obj);
void interfaces_free(struct interfaces *this);

#endif
