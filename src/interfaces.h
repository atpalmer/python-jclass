#ifndef INTERFACES_H
#define INTERFACES_H

#include "membuff.h"

typedef struct {
    uint16_t count;
    uint16_t indexes[];
} JavaClassInterfaces;

void interfaces_parse(MemReader *reader, JavaClassInterfaces **obj);
void JavaClassInterfaces_free(JavaClassInterfaces *this);

#endif
