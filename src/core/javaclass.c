#include "membuff.h"
#include "access.h"
#include "constant_pool.h"
#include "interfaces.h"
#include "fields.h"
#include "methods.h"
#include "attributes.h"
#include "javaclass.h"
#include "mem.h"
#include "error.h"


struct javaclass *javaclass_from_membuff(struct membuff *r) {
    struct javaclass *new = mem_calloc(1, sizeof(*new));
    if(!new)
        return NULL;

    new->magic = membuff_next_uint32(r);

    if(new->magic != 0xCAFEBABE) {
        javaclass_error_set(JAVACLASS_ERR_CAFEBABE, "File is not a class file");
        goto fail;
    }

    new->minor_version = membuff_next_uint16(r);
    new->major_version = membuff_next_uint16(r);

    if(!(new->major_version == 58 && new->minor_version == 0)) {
        javaclass_error_set(JAVACLASS_ERR_BADVER, "Unsupported version");
        goto fail;
    }

    new->pool = constant_pool_parse(r);
    if(!new->pool)
        goto fail;

    new->access_flags = membuff_next_uint16(r);

    new->this_class = membuff_next_uint16(r);
    if(!constant_pool_item(new->pool, new->this_class)) {
        javaclass_error_set(JAVACLASS_ERR_PARSE, "Invalid this_class pool address");
        goto fail;
    }

    new->super_class = membuff_next_uint16(r);
    if(!constant_pool_item(new->pool, new->super_class)) {
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

    new->attributes = attributes_parse(r);
    if(!attributes_ensure_integrity(new->attributes, new->pool)) {
        javaclass_error_set(JAVACLASS_ERR_PARSE, "Invalid pool address");
        goto fail;
    }

    if(membuff_has_error(r)) {
        javaclass_error_set(JAVACLASS_ERR_PARSE, "Parse error: length mismatch");
        goto fail;
    }

    return new;

fail:
    javaclass_free(new);
    return NULL;
}

struct javaclass *javaclass_from_filename(const char *filename) {
    struct javaclass *new = NULL;
    struct membuff *r;
    int errno_ = membuff_from_filename(filename, &r);
    if(errno_) {
        javaclass_error_set(JAVACLASS_ERR_OS, strerror(errno_));
        goto out;
    }
    new = javaclass_from_membuff(r);
out:
    membuff_free(r);
    return new;
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
