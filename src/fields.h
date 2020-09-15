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
    uint16_t fields_count;
    JavaClassField *fields[];
} JavaClassFields;

void fields_parse(MemReader *reader, JavaClassFields **obj);
void fields_print(JavaClassFields *this);
void JavaClassField_free(JavaClassField *this);
void JavaClassFields_free(JavaClassFields *this);

#endif
