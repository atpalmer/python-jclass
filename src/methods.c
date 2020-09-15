#include <Python.h>
#include "access.h"
#include "parse.h"
#include "attributes.h"
#include "methods.h"


size_t parse_methods(uint8_t *data, JavaClassMethods **obj) {
    size_t curr_bytes = 0;

    uint16_t count;
    curr_bytes += parse16(&data[curr_bytes], &count);

    *obj = PyMem_Malloc(sizeof(JavaClassMethods) + (sizeof(JavaClassMethod *) * count));
    (*obj)->methods_count = count;

    for(int i = 0; i < count; ++i) {
        uint8_t *p = &data[curr_bytes];
        JavaClassMethod **method = &(*obj)->methods[i];

        JavaClassAttributes *attributes;
        size_t attr_bytes = attributes_parse(((uint8_t *)Method_attributes(p)) - 2, &attributes);  /* TODO: cleanup ptr math */

        *method = PyMem_Malloc(sizeof(JavaClassMethod));
        (*method)->access_flags = Method_access_flags(p);
        (*method)->name_index = Method_name_index(p);
        (*method)->descriptor_index = Method_descriptor_index(p);
        (*method)->attributes = attributes;

        curr_bytes += attr_bytes;
        curr_bytes += 6;
    }
    return curr_bytes;
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
