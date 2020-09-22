#include <Python.h>
#include "core/access.h"
#include "core/attributes.h"
#include "core/constant_pool.h"
#include "conv.h"

PyObject *conv_flags_to_PySet(uint16_t flags) {
    PyObject *set = PySet_New(NULL);

    if(flags & ACC_PUBLIC)
        PySet_Add(set, PyUnicode_FromString("public"));
    if(flags & ACC_FINAL)
        PySet_Add(set, PyUnicode_FromString("final"));
    if(flags & ACC_SUPER)
        PySet_Add(set, PyUnicode_FromString("super"));
    if(flags & ACC_INTERFACE)
        PySet_Add(set, PyUnicode_FromString("interface"));
    if(flags & ACC_ABSTRACT)
        PySet_Add(set, PyUnicode_FromString("abstract"));
    if(flags & ACC_SYNTHETIC)
        PySet_Add(set, PyUnicode_FromString("synthetic"));
    if(flags & ACC_ANNOTATION)
        PySet_Add(set, PyUnicode_FromString("annotation"));
    if(flags & ACC_ENUM)
        PySet_Add(set, PyUnicode_FromString("enum"));

    return set;
}

PyObject *conv_attributes_to_PyDict(struct attribute_items *attributes, struct constant_pool *pool) {
    PyObject *dict = PyDict_New();

    for(uint16_t i = 0; i < attributes->count; ++i) {
        struct attribute *attr = attributes->items[i];

        struct pool_Utf8 *name = attr->name;
        PyObject *key = PyUnicode_FromStringAndSize(name->bytes, name->length);
        PyObject *value = PyBytes_FromStringAndSize((void *)attr->info, attr->length);

        PyDict_SetItem(dict, key, value);
    }

    return dict;
}
