#include <Python.h>
#include "parse.h"
#include "attributes.h"
#include "membuff.h"


size_t attributes_parse(uint8_t *data, JavaClassAttributes **obj) {
    size_t curr_bytes = 0;
    uint16_t count;
    curr_bytes += parse16(&data[curr_bytes], &count);

    *obj = PyMem_Malloc(sizeof(JavaClassAttributes) + (sizeof(JavaClassAttribute *) * count));

    (*obj)->attributes_count = count;

    for(uint16_t i = 0; i < count; ++i) {
        uint8_t *p = &data[curr_bytes];

        JavaClassAttribute **attr = &(*obj)->attributes[i];
        *attr = PyMem_Malloc(sizeof(JavaClassAttribute) + Attribute_length(p));

        (*attr)->attribute_name_index = Attribute_name_index(p);
        (*attr)->attribute_length = Attribute_length(p);
        memcpy((*attr)->info, Attribute_info(p), Attribute_length(p));

        curr_bytes += 6 + Attribute_length(p);
    }

    return curr_bytes;
}

void attributes_parse_wrapper(MemReader *reader, JavaClassAttributes **obj) {
    reader->pos += attributes_parse(MEMREADER_CURR(reader), obj);
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
