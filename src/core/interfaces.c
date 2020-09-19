#include "interfaces.h"
#include "membuff.h"
#include "mem.h"

struct interfaces *interfaces_parse(struct membuff *reader) {
    uint16_t count = membuff_next_uint16(reader);
    struct interfaces *obj = mem_malloc(sizeof(struct interfaces) + (sizeof(uint16_t) * count));
    if(!obj)
        return NULL;
    obj->count = count;

    for(uint16_t i = 0; i < count; ++i)
        obj->indexes[i] = membuff_next_uint16(reader);

    return obj;
}

void interfaces_free(struct interfaces *this) {
    mem_free(this);
}
