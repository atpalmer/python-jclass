#include <Python.h>
#include "parse.h"
#include "attributes.h"
#include "membuff.h"


void attributes_parse(MemReader *reader, JavaClassAttributes **obj) {
    uint16_t count = MemReader_next_uint16(reader);
    *obj = PyMem_Malloc(sizeof(JavaClassAttributes) + (sizeof(JavaClassAttribute *) * count));
    (*obj)->attributes_count = count;

    for(uint16_t i = 0; i < count; ++i) {
        JavaClassAttribute **attr = &(*obj)->attributes[i];

        uint16_t name_index = MemReader_next_uint16(reader);
        uint32_t length = MemReader_next_uint32(reader);

        *attr = PyMem_Malloc(sizeof(JavaClassAttribute) + length);

        (*attr)->attribute_name_index = name_index;
        (*attr)->attribute_length = length;

        MemReader_copy_next(reader, length, (*attr)->info);
    }
}

void attributes_print(JavaClassAttributes *this) {
    printf("attr count: %u\n", this->attributes_count);
    for(uint16_t i = 0; i < this->attributes_count; ++i) {
        JavaClassAttribute *attr = this->attributes[i];
        printf("** Attr name index: %u\n", attr->attribute_name_index);
        printf("** Attr length: %u\n", attr->attribute_length);
        printf("** Attr info: %.*s\n", attr->attribute_length, attr->info);
    }
}

void JavaClassAttributes_free(JavaClassAttributes *this) {
    for(uint16_t i = 0; i < this->attributes_count; ++i)
        PyMem_Free(this->attributes[i]);
    PyMem_Free(this);
}
