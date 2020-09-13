#include <Python.h>
#include "parse.h"
#include "interfaces.h"


size_t interfaces_parse(uint8_t *data, JavaClassInterfaces **obj) {
    size_t curr_bytes = 0;

    uint16_t count;
    curr_bytes += parse16(&data[curr_bytes], &count);
    *obj = PyMem_Malloc(sizeof(JavaClassInterfaces) + (sizeof(uint16_t) * count));
    (*obj)->interfaces_count = count;

    for(uint16_t i = 0; i < count; ++i) {
        uint16_t *p = &((*obj)->interfaces[i]);
        curr_bytes += parse16(&data[curr_bytes], p);
    }

    return curr_bytes;
}

void interfaces_print(JavaClassInterfaces *this) {
    printf("Interfaces count: %d\n", this->interfaces_count);
    for(uint16_t i = 0; i < this->interfaces_count; ++i) {
        printf("  * interface: %u\n", this->interfaces[i]);
    }
}

void JavaClassInterfaces_free(JavaClassInterfaces *this) {
    PyMem_Free(this);
}
