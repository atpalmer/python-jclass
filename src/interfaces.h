#ifndef INTERFACES_H
#define INTERFACES_H

#include "membuff.h"

typedef struct {
    uint16_t interfaces_count;
    uint16_t interfaces[];
} JavaClassInterfaces;

void interfaces_parse(MemReader *reader, JavaClassInterfaces **obj);
void interfaces_print(JavaClassInterfaces *this);
void JavaClassInterfaces_free(JavaClassInterfaces *this);

#endif
