#include <Python.h>
#include "access.h"
#include "parse.h"
#include "attributes.h"
#include "fields.h"


/* TODO: clean this up */
#define FIELD_ATTR_OBJ_PTR(p) (((uint8_t *)Field_attributes(p)) - 2)


size_t fields_parse(uint8_t *data, JavaClassFields **obj) {
    size_t curr_bytes = 0;

    uint16_t count;
    curr_bytes += parse16(&data[curr_bytes], &count);

    *obj = PyMem_Malloc(sizeof(JavaClassFields) + (sizeof(JavaClassField *) * count));
    (*obj)->fields_count = count;

    for(uint16_t i = 0; i < count; ++i) {
        uint8_t *p = &data[curr_bytes];

        JavaClassField **field = &(*obj)->fields[i];
        JavaClassAttributes *attributes;
        size_t attr_size = attributes_obj_parse(FIELD_ATTR_OBJ_PTR(p), &attributes);
        *field = PyMem_Malloc(sizeof(JavaClassField) + (sizeof(JavaClassAttribute *) * attributes->attributes_count));
        (*field)->access_flags = Field_access_flags(p);
        (*field)->name_index = Field_name_index(p);
        (*field)->descriptor_index = Field_descriptor_index(p);
        (*field)->attributes = attributes;
        curr_bytes += attr_size;
        curr_bytes += 6;  /* exclude sizeof(attributes_count) */
    }
    return curr_bytes;
}

void fields_print(JavaClassFields *this) {
    printf("Fields count: %u\n", this->fields_count);
    for(uint16_t i = 0; i < this->fields_count; ++i) {
        JavaClassField *field = this->fields[i];
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
    for(uint16_t i = 0; i < this->fields_count; ++i) {
        JavaClassField_free(this->fields[i]);
    }
    PyMem_Free(this);
}
