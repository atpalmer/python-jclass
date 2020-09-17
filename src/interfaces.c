#include <Python.h>
#include "interfaces.h"
#include "membuff.h"

void interfaces_parse(MemReader *reader, struct interfaces **obj) {
    uint16_t count = MemReader_next_uint16(reader);
    *obj = PyMem_Malloc(sizeof(struct interfaces) + (sizeof(uint16_t) * count));
    (*obj)->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        (*obj)->indexes[i] = MemReader_next_uint16(reader);
    }
}

void interfaces_free(struct interfaces *this) {
    PyMem_Free(this);
}
