#include <Python.h>
#include "access.h"
#include "attributes.h"
#include "methods.h"
#include "membuff.h"


struct method_items *methods_parse(MemReader *reader) {
    uint16_t count = MemReader_next_uint16(reader);
    struct method_items *obj = PyMem_Malloc(sizeof(struct method_items) + (sizeof(struct method *) * count));
    obj->count = count;

    for(int i = 0; i < count; ++i) {
        struct method **method = &obj->items[i];

        *method = PyMem_Malloc(sizeof(struct method));

        (*method)->access_flags = MemReader_next_uint16(reader);
        (*method)->name_index = MemReader_next_uint16(reader);
        (*method)->descriptor_index = MemReader_next_uint16(reader);
        (*method)->attributes = attributes_parse(reader);
    }

    return obj;
}

static void method_free(struct method *this) {
    attributes_free(this->attributes);
    PyMem_Free(this);
}

void methods_free(struct method_items *this) {
    for(uint16_t i = 0; i < this->count; ++i) {
        method_free(this->items[i]);
    }
    PyMem_Free(this);
}
