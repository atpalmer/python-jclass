#ifndef METHODS_H
#define METHODS_H

#include "attributes.h"

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    JavaClassAttributes *attributes;
} JavaClassMethod;

typedef struct {
    uint16_t methods_count;
    JavaClassMethod *methods[];
} JavaClassMethods;

size_t parse_methods(uint8_t *data, JavaClassMethods **obj);
void methods_print(JavaClassMethods *this);
void JavaClassMethod_free(JavaClassMethod *this);
void JavaClassMethods_free(JavaClassMethods *this);

#endif
