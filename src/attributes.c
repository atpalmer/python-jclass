#include <Python.h>
#include "attributes.h"
#include "membuff.h"


void attributes_parse(MemReader *reader, JavaClassAttributes **obj) {
    uint16_t count = MemReader_next_uint16(reader);
    *obj = PyMem_Malloc(sizeof(JavaClassAttributes) + (sizeof(JavaClassAttribute *) * count));
    (*obj)->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        JavaClassAttribute **attr = &(*obj)->items[i];

        uint16_t name_index = MemReader_next_uint16(reader);
        uint32_t length = MemReader_next_uint32(reader);

        *attr = PyMem_Malloc(sizeof(JavaClassAttribute) + length);

        (*attr)->name_index = name_index;
        (*attr)->length = length;

        MemReader_copy_next(reader, length, (*attr)->info);
    }
}

void JavaClassAttributes_free(JavaClassAttributes *this) {
    for(uint16_t i = 0; i < this->count; ++i)
        PyMem_Free(this->items[i]);
    PyMem_Free(this);
}
