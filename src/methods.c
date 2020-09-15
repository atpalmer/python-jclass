#include <Python.h>
#include "access.h"
#include "parse.h"
#include "attributes.h"
#include "methods.h"
#include "membuff.h"


void parse_methods(MemReader *reader, JavaClassMethods **obj) {
    uint16_t count = MemReader_next_uint16(reader);

    *obj = PyMem_Malloc(sizeof(JavaClassMethods) + (sizeof(JavaClassMethod *) * count));
    (*obj)->methods_count = count;

    for(int i = 0; i < count; ++i) {
        JavaClassMethod **method = &(*obj)->methods[i];

        *method = PyMem_Malloc(sizeof(JavaClassMethod));
        (*method)->access_flags = MemReader_next_uint16(reader);
        (*method)->name_index = MemReader_next_uint16(reader);
        (*method)->descriptor_index = MemReader_next_uint16(reader);

        JavaClassAttributes *attributes;
        attributes_parse_wrapper(reader, &attributes);

        (*method)->attributes = attributes;
    }
}

void methods_print(JavaClassMethods *this) {
    printf("Methods count: %d\n", this->methods_count);
    for(uint16_t i = 0; i < this->methods_count; ++i) {
        JavaClassMethod *m = this->methods[i];
        printf("* Method access_flags: %u\n", m->access_flags);
        printf("* Method name_index: %u\n", m->name_index);
        printf("* Method descriptor_index: %u\n", m->descriptor_index);
        attributes_print(m->attributes);
    }
}

void JavaClassMethod_free(JavaClassMethod *this) {
    JavaClassAttributes_free(this->attributes);
    PyMem_Free(this);
}

void JavaClassMethods_free(JavaClassMethods *this) {
    for(uint16_t i = 0; i < this->methods_count; ++i) {
        JavaClassMethod_free(this->methods[i]);
    }
    PyMem_Free(this);
}
