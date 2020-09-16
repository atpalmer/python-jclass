#include <Python.h>
#include "access.h"
#include "attributes.h"
#include "fields.h"
#include "membuff.h"


void fields_parse(MemReader *reader, JavaClassFields **obj) {
    uint16_t count = MemReader_next_uint16(reader);
    *obj = PyMem_Malloc(sizeof(JavaClassFields) + (sizeof(JavaClassField *) * count));
    (*obj)->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        JavaClassField **field = &(*obj)->items[i];

        *field = PyMem_Malloc(sizeof(JavaClassField));

        (*field)->access_flags = MemReader_next_uint16(reader);
        (*field)->name_index = MemReader_next_uint16(reader);
        (*field)->descriptor_index = MemReader_next_uint16(reader);

        JavaClassAttributes *attributes;
        attributes_parse(reader, &attributes);

        (*field)->attributes = attributes;
    }
}

void fields_print(JavaClassFields *this) {
    printf("Fields count: %u\n", this->count);
    for(uint16_t i = 0; i < this->count; ++i) {
        JavaClassField *field = this->items[i];
        printf("* Field access flags: %u\n", field->access_flags);
        printf("* Field name index: %u\n", field->name_index);
        printf("* Field descriptor index: %u\n", field->descriptor_index);
        printf("* Field attributes:\n");
        attributes_print(field->attributes);
    }
}

void JavaClassField_free(JavaClassField *this) {
    JavaClassAttributes_free(this->attributes);
    PyMem_Free(this);
}

void JavaClassFields_free(JavaClassFields *this) {
    for(uint16_t i = 0; i < this->count; ++i) {
        JavaClassField_free(this->items[i]);
    }
    PyMem_Free(this);
}
