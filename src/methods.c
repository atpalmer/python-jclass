#include <Python.h>
#include "access.h"
#include "attributes.h"
#include "methods.h"
#include "membuff.h"


void parse_methods(MemReader *reader, JavaClassMethods **obj) {
    uint16_t count = MemReader_next_uint16(reader);

    *obj = PyMem_Malloc(sizeof(JavaClassMethods) + (sizeof(JavaClassMethod *) * count));
    (*obj)->count = count;

    for(int i = 0; i < count; ++i) {
        JavaClassMethod **method = &(*obj)->items[i];

        *method = PyMem_Malloc(sizeof(JavaClassMethod));
        (*method)->access_flags = MemReader_next_uint16(reader);
        (*method)->name_index = MemReader_next_uint16(reader);
        (*method)->descriptor_index = MemReader_next_uint16(reader);

        JavaClassAttributes *attributes;
        attributes_parse(reader, &attributes);

        (*method)->attributes = attributes;
    }
}

void JavaClassMethod_free(JavaClassMethod *this) {
    JavaClassAttributes_free(this->attributes);
    PyMem_Free(this);
}

void JavaClassMethods_free(JavaClassMethods *this) {
    for(uint16_t i = 0; i < this->count; ++i) {
        JavaClassMethod_free(this->items[i]);
    }
    PyMem_Free(this);
}
