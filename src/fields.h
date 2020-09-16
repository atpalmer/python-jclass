#ifndef FIELDS_H
#define FIELDS_H

#include "attributes.h"
#include "membuff.h"

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    JavaClassAttributes *attributes;
} JavaClassField;

typedef struct {
    uint16_t count;
    JavaClassField *items[];
} JavaClassFields;

void fields_parse(MemReader *reader, JavaClassFields **obj);
void JavaClassField_free(JavaClassField *this);
void JavaClassFields_free(JavaClassFields *this);

#endif
