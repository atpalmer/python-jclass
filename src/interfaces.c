#include <Python.h>
#include "interfaces.h"
#include "membuff.h"

struct interfaces *interfaces_parse(MemReader *reader) {
    uint16_t count = MemReader_next_uint16(reader);
    struct interfaces *obj = PyMem_Malloc(sizeof(struct interfaces) + (sizeof(uint16_t) * count));
    obj->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        obj->indexes[i] = MemReader_next_uint16(reader);
    }

    return obj;
}

void interfaces_free(struct interfaces *this) {
    PyMem_Free(this);
}
