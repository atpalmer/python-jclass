#ifndef METHODS_H
#define METHODS_H

#include "attributes.h"
#include "membuff.h"

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    JavaClassAttributes *attributes;
} JavaClassMethod;

typedef struct {
    uint16_t count;
    JavaClassMethod *items[];
} JavaClassMethods;

void parse_methods(MemReader *reader, JavaClassMethods **obj);
void methods_print(JavaClassMethods *this);
void JavaClassMethod_free(JavaClassMethod *this);
void JavaClassMethods_free(JavaClassMethods *this);

#endif
