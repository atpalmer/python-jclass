#ifndef FIELDS_H
#define FIELDS_H

#include "attributes.h"

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

size_t fields_parse(uint8_t *data, JavaClassFields **obj);
void fields_print(JavaClassFields *this);
void JavaClassField_free(JavaClassField *this);
void JavaClassFields_free(JavaClassFields *this);

#endif
