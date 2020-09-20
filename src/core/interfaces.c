#include "interfaces.h"
#include "constant_pool.h"
#include "membuff.h"
#include "mem.h"

struct interfaces *interfaces_ensure_integrity(struct interfaces *this, struct constant_pool *pool) {
    if(!this)
        return NULL;
    for(uint16_t i = 0; i < this->count; ++i) {
        if(!constant_pool_item(pool, this->indexes[i]))
            return NULL;
    }
    return this;
}

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
