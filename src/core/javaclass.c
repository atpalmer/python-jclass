#include <Python.h>
#include "structmember.h"
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

static struct attribute_items *_attributes_ensure_integrity(struct attribute_items *this, struct constant_pool *pool) {
    if(!this)
        return NULL;
    for(uint16_t i = 0; i < this->count; ++i) {
        struct attribute *attr = this->items[i];
        if(!attr)
            return NULL;
        if(!constant_pool_item(pool, attr->name_index))
            return NULL;
    }
    return this;
}

static struct field_items *_fields_ensure_integrity(struct field_items *this, struct constant_pool *pool) {
    if(!this)
        return NULL;
    for(uint16_t i = 0; i < this->count; ++i) {
        struct field *field = this->items[i];
        if(!field)
            return NULL;
        if(!constant_pool_item(pool, field->name_index))
            return NULL;
        if(!constant_pool_item(pool, field->descriptor_index))
            return NULL;
        if(!_attributes_ensure_integrity(field->attributes, pool))
            return NULL;
    }
    return this;
}

static struct method_items *_methods_ensure_integrity(struct method_items *this, struct constant_pool *pool) {
    if(!this)
        return NULL;
    for(uint16_t i = 0; i < this->count; ++i) {
        struct method *method = this->items[i];
        if(!method)
            return NULL;
        if(!constant_pool_item(pool, method->name_index))
            return NULL;
        if(!constant_pool_item(pool, method->descriptor_index))
            return NULL;
        if(!_attributes_ensure_integrity(method->attributes, pool))
            return NULL;
    }
    return this;
}

static struct interfaces *_interfaces_ensure_integrity(struct interfaces *this, struct constant_pool *pool) {
    if(!this)
        return NULL;
    for(uint16_t i = 0; i < this->count; ++i) {
        if(!constant_pool_item(pool, this->indexes[i]))
            return NULL;
    }
    return this;
}

static struct javaclass *_javaclass_ensure_integrity(struct javaclass *this) {
    if(!this)
        return NULL;
    if(!this->pool)
        return NULL;

    if(!constant_pool_item(this->pool, this->this_class))
        return NULL;
    if(!constant_pool_item(this->pool, this->super_class))
        return NULL;

    if(!_interfaces_ensure_integrity(this->interfaces, this->pool))
        return NULL;
    if(!_fields_ensure_integrity(this->fields, this->pool))
        return NULL;
    if(!_methods_ensure_integrity(this->methods, this->pool))
        return NULL;
    if(!_attributes_ensure_integrity(this->attributes, this->pool))
        return NULL;

    return this;
}

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
        /* TODO: remove Python */
        PyErr_SetString(PyExc_ValueError, "Unsupported version");
        goto fail;
    }

    new->pool = constant_pool_parse(r);

    new->access_flags = membuff_next_uint16(r);
    new->this_class = membuff_next_uint16(r);
    new->super_class = membuff_next_uint16(r);

    new->interfaces = interfaces_parse(r);
    new->fields = fields_parse(r);
    new->methods = methods_parse(r);
    new->attributes = attributes_parse(r);

    if(membuff_has_error(r)) {
        /* TODO: remove Python */
        PyErr_SetString(PyExc_ValueError, "Parse error");
        goto fail;
    }

    struct javaclass *result = _javaclass_ensure_integrity(new);
    if(!result) {
        /* TODO: remove Python */
        PyErr_SetString(PyExc_ValueError, "Parse error");
        goto fail;
    }
    return result;

fail:
    javaclass_free(new);
    return NULL;
}

enum javaclass_errcode _set_pyerr(void) {
    /* TODO: remove Python. */
    const char *msg = NULL;
    enum javaclass_errcode code = javaclass_error_get(&msg);
    if(code == JAVACLASS_ERR_OS)
        PyErr_SetString(PyExc_OSError, msg);
    if(code == JAVACLASS_ERR_CAFEBABE)
        PyErr_SetString(PyExc_ValueError, msg);
    return code;
}

struct javaclass *javaclass_from_filename(const char *filename) {
    struct membuff *r;
    int errno_ = membuff_from_filename(filename, &r);
    if(errno_) {
        javaclass_error_set(JAVACLASS_ERR_OS, strerror(errno_));
        goto fail;
    }
    struct javaclass *new = javaclass_from_membuff(r);
    if(!new)
        goto fail;
    membuff_free(r);
    return new;

fail:
    _set_pyerr();
    membuff_free(r);
    return NULL;
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
