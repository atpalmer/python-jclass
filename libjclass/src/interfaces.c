#include <jclass/javaclass.h>
#include <jclass/error.h>
#include <jclass/internal/stream.h>
#include <jclass/internal/mem.h>

struct interfaces *interfaces_parse(FILE *reader, struct constant_pool *pool) {
    uint16_t count = stream_next_uint16(reader);
    struct interfaces *obj = mem_malloc(sizeof(struct interfaces) + (sizeof(uint16_t) * count));
    if(!obj)
        return NULL;
    obj->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        obj->indexes[i] = stream_next_uint16(reader);
        if(!constant_pool_Class_item(pool, obj->indexes[i])) {
            javaclass_error_set(JAVACLASS_ERR_PARSE, "Bad interface pool index");
            goto fail;
        }
    }

    return obj;

fail:
    interfaces_free(obj);
    return NULL;
}

void interfaces_free(struct interfaces *this) {
    mem_free(this);
}
