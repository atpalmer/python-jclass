#include <jclass/internal/stream.h>
#include <jclass/internal/mem.h>
#include <jclass/javaclass.h>
#include <jclass/error.h>


struct javaclass *javaclass_from_file(FILE *r) {
    struct javaclass *new = mem_calloc(1, sizeof(*new));
    if(!new)
        return NULL;

    new->magic = stream_next_uint32(r);
    if(new->magic != 0xCAFEBABE) {
        javaclass_error_set(JAVACLASS_ERR_CAFEBABE, "File is not a class file");
        goto fail;
    }

    new->minor_version = stream_next_uint16(r);
    new->major_version = stream_next_uint16(r);
    if(!(new->major_version == 58 && new->minor_version == 0)) {
        javaclass_error_set(JAVACLASS_ERR_BADVER, "Unsupported version");
        goto fail;
    }

    new->pool = constant_pool_parse(r);
    if(!new->pool)
        goto fail;

    new->access_flags = stream_next_uint16(r);

    new->this_class = stream_next_uint16(r);
    if(!constant_pool_Class_item(new->pool, new->this_class)) {
        javaclass_error_set(JAVACLASS_ERR_PARSE, "Invalid this_class pool address");
        goto fail;
    }

    new->super_class = stream_next_uint16(r);
    if(!constant_pool_Class_item(new->pool, new->super_class)) {
        javaclass_error_set(JAVACLASS_ERR_PARSE, "Invalid super_class pool address");
        goto fail;
    }

    new->interfaces = interfaces_parse(r, new->pool);
    if(!new->interfaces)
        goto fail;

    new->fields = fields_parse(r, new->pool);
    if(!new->fields)
        goto fail;

    new->methods = methods_parse(r, new->pool);
    if(!new->methods)
        goto fail;

    new->attributes = attributes_parse(r, new->pool);
    if(!new->attributes)
        goto fail;

    if(ferror(r)) {
        javaclass_error_set(JAVACLASS_ERR_PARSE, "Error reading file");
        goto fail;
    }

    return new;

fail:
    javaclass_free(new);
    return NULL;
}

struct javaclass *javaclass_from_filename(const char *filename) {
    FILE *r = fopen(filename, "rb");
    if(!r) {
        javaclass_error_set(JAVACLASS_ERR_OS, strerror(errno));
        return NULL;
    }
    struct javaclass *result = javaclass_from_file(r);
    fclose(r);
    return result;
}

void javaclass_free(struct javaclass *this) {
    if(!this)
        return;
    constant_pool_free(this->pool);
    interfaces_free(this->interfaces);
    fields_free(this->fields);
    methods_free(this->methods);
    attributes_free(this->attributes);
    mem_free(this);
}
