#include <stdlib.h>
#include "interfaces.h"
#include "membuff.h"

struct interfaces *interfaces_parse(struct membuff *reader) {
    uint16_t count = membuff_next_uint16(reader);
    struct interfaces *obj = malloc(sizeof(struct interfaces) + (sizeof(uint16_t) * count));
    if(!obj)
        return NULL;
    obj->count = count;

    for(uint16_t i = 0; i < count; ++i)
        obj->indexes[i] = membuff_next_uint16(reader);

    return obj;
}

void interfaces_free(struct interfaces *this) {
    if(!this)
        return;
    free(this);
}
