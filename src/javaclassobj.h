#ifndef JAVACLASSOBJ_H
#define JAVACLASSOBJ_H

#include <Python.h>
#include "core/constant_pool.h"
#include "core/interfaces.h"
#include "core/fields.h"
#include "core/methods.h"
#include "core/attributes.h"

typedef struct {
    PyObject_HEAD
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    struct constant_pool *pool;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;
    struct interfaces *interfaces;
    struct field_items *fields;
    struct method_items *methods;
    struct attribute_items *attributes;
} JavaClass;

JavaClass *JavaClass_from_filename(const char *filename);

extern PyTypeObject JavaClass_Type;

#endif
