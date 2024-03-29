#include <jclass/javaclass.h>
#include <jclass/internal/stream.h>
#include <jclass/internal/mem.h>

static struct attr_BASE *_parse_attr_info_ConstantValue(
        FILE *reader, uint32_t len, struct pool_Utf8 *name, struct constant_pool *pool) {
    uint16_t index = stream_next_uint16(reader);
    struct pool_CONSTANT *constant = constant_pool_item(pool, index);
    if(!constant)
        return NULL;
    switch(constant->tag) {
    case CONSTANT_TAG_Long:
    case CONSTANT_TAG_Float:
    case CONSTANT_TAG_Double:
    case CONSTANT_TAG_Integer:
    case CONSTANT_TAG_String:
        break;
    default:
        return NULL;
    }

    struct attr_ConstantValue *new = mem_malloc(sizeof(*new) + len);
    if(!new)
        return NULL;

    new->is_raw = 0;
    new->name = name;
    new->constant = constant;
    return (struct attr_BASE *)new;
}

static struct attr_BASE *_parse_attr_info_SourceFile(
        FILE *reader, uint32_t len, struct pool_Utf8 *name, struct constant_pool *pool) {
    uint16_t sourcestream_index = stream_next_uint16(reader);
    struct pool_Utf8 *sourcefile = constant_pool_Utf8_item(pool, sourcestream_index);
    if(!sourcefile)
        return NULL;

    struct attr_SourceFile *new = mem_malloc(sizeof(*new) + len);
    if(!new)
        return NULL;

    new->is_raw = 0;
    new->name = name;
    new->sourcefile = sourcefile;
    return (struct attr_BASE *)new;
}

static struct attr_BASE *_parse_attr_info_RAW(FILE *reader, uint32_t len, struct pool_Utf8 *name) {
    struct attr_RAW *new = mem_malloc(sizeof(*new) + len);
    if(!new)
        return NULL;

    new->is_raw = 1;
    new->name = name;
    new->length = len;
    fread(new->info, 1, len, reader);
    return (struct attr_BASE *)new;
}

static struct attr_BASE *_parse_attr_info(FILE *reader, struct constant_pool *pool) {
    uint16_t name_index = stream_next_uint16(reader);
    uint32_t length = stream_next_uint32(reader);

    struct pool_Utf8 *name = constant_pool_Utf8_item(pool, name_index);
    if(!name) {
        javaclass_error_set(JAVACLASS_ERR_PARSE, "Invalid attribute name index");
        return NULL;
    }

    if(pool_Utf8_eq_str(name, "ConstantValue"))
        return _parse_attr_info_ConstantValue(reader, length, name, pool);
    if(pool_Utf8_eq_str(name, "SourceFile"))
        return _parse_attr_info_SourceFile(reader, length, name, pool);

    return _parse_attr_info_RAW(reader, length, name);
}

struct attribute_items *attributes_parse(FILE *reader, struct constant_pool *pool) {
    uint16_t count = stream_next_uint16(reader);
    struct attribute_items *obj = mem_calloc(1, sizeof(struct attribute_items) + (sizeof(struct attribute *) * count));
    if(!obj)
        return NULL;
    obj->count = count;

    for(uint16_t i = 0; i < count; ++i) {
        struct attr_BASE **attr = &obj->items[i];
        *attr = _parse_attr_info(reader, pool);
        if(!*attr)
            goto fail;
    }

    return obj;

fail:
    attributes_free(obj);
    return NULL;
}

void attributes_free(struct attribute_items *this) {
    if(!this)
        return;
    for(uint16_t i = 0; i < this->count; ++i)
        mem_free(this->items[i]);
    mem_free(this);
}
