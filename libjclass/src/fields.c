#include <jclass/javaclass.h>
#include <jclass/error.h>
#include <jclass/internal/stream.h>
#include <jclass/internal/mem.h>

struct field_items *fields_parse(FILE *reader, struct constant_pool *pool) {
    uint16_t count = stream_next_uint16(reader);
    struct field_items *obj = mem_calloc(1, sizeof(struct field_items) + (sizeof(struct field *) * count));
    if(!obj)
        return NULL;
    obj->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        struct field **field = &obj->items[i];

        *field = mem_calloc(1, sizeof(struct field));
        if(!*field)
            goto fail;

        (*field)->access_flags = stream_next_uint16(reader);

        (*field)->name_index = stream_next_uint16(reader);
        if(!constant_pool_Utf8_item(pool, (*field)->name_index)) {
            javaclass_error_set(JAVACLASS_ERR_PARSE, "Invalid field name index");
            goto fail;
        }

        (*field)->descriptor_index = stream_next_uint16(reader);
        if(!constant_pool_Utf8_item(pool, (*field)->descriptor_index)) {
            javaclass_error_set(JAVACLASS_ERR_PARSE, "Invalid field descriptor index");
            goto fail;
        }

        (*field)->attributes = attributes_parse(reader, pool);
        if(!(*field)->attributes)
            goto fail;
    }

    return obj;

fail:
    fields_free(obj);
    return NULL;
}

static void field_free(struct field *this) {
    if(!this)
        return;
    attributes_free(this->attributes);
    mem_free(this);
}

void fields_free(struct field_items *this) {
    if(!this)
        return;
    for(uint16_t i = 0; i < this->count; ++i)
        field_free(this->items[i]);
    mem_free(this);
}
