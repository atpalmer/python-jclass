#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include "membuff.h"

typedef struct {
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t info[];
} JavaClassAttribute;

typedef struct {
    uint16_t attributes_count;
    JavaClassAttribute *attributes[];
} JavaClassAttributes;

void attributes_parse(MemReader *reader, JavaClassAttributes **obj);
void JavaClassAttributes_free(JavaClassAttributes *this);

#endif
