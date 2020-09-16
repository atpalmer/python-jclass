#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include "membuff.h"

typedef struct {
    uint16_t name_index;
    uint32_t length;
    uint8_t info[];
} JavaClassAttribute;

typedef struct {
    uint16_t count;
    JavaClassAttribute *items[];
} JavaClassAttributes;

void attributes_parse(MemReader *reader, JavaClassAttributes **obj);
void JavaClassAttributes_free(JavaClassAttributes *this);

#endif
