#ifndef CONV_H
#define CONV_H

#include <Python.h>
#include "attributes.h"
#include "constant_pool.h"

PyObject *conv_flags_to_PySet(uint16_t flags);
PyObject *conv_attributes_to_PyDict(struct attribute_items *attributes, struct constant_pool *pool);

#endif
