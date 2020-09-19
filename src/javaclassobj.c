#include <Python.h>
#include "structmember.h"
#include "conv.h"
#include "membuff.h"
#include "access.h"
#include "constant_pool.h"
#include "interfaces.h"
#include "fields.h"
#include "methods.h"
#include "attributes.h"
#include "javaclassobj.h"

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

static JavaClass *_JavaClass_ensure_integrity(JavaClass *this) {
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

JavaClass *JavaClass_from_MemReader(MemReader *r) {
    JavaClass *class = (JavaClass *)JavaClass_Type.tp_alloc(&JavaClass_Type, 0);
    if(!class)
        return NULL;

    class->magic = MemReader_next_uint32(r);

    if(class->magic != 0xCAFEBABE) {
        PyErr_SetString(PyExc_ValueError, "File is not a class file");
        goto fail;
    }

    class->minor_version = MemReader_next_uint16(r);
    class->major_version = MemReader_next_uint16(r);

    if(!(class->major_version == 58 && class->minor_version == 0)) {
        PyErr_SetString(PyExc_ValueError, "Unsupported version");
        goto fail;
    }

    class->pool = constant_pool_parse(r);

    class->access_flags = MemReader_next_uint16(r);
    class->this_class = MemReader_next_uint16(r);
    class->super_class = MemReader_next_uint16(r);

    class->interfaces = interfaces_parse(r);
    class->fields = fields_parse(r);
    class->methods = methods_parse(r);
    class->attributes = attributes_parse(r);

    if(MemReader_has_error(r)) {
        PyErr_SetString(PyExc_ValueError, "Parse error");
        goto fail;
    }

    JavaClass *result = _JavaClass_ensure_integrity(class);
    if(!result) {
        PyErr_SetString(PyExc_ValueError, "Parse error");
        goto fail;
    }
    return result;

fail:
    Py_DECREF(class);
    return NULL;
}

JavaClass *JavaClass_from_filename(const char *filename) {
    MemReader *r;
    int errno_ = MemReader_from_filename(filename, &r);
    if(errno_) {
        PyErr_SetString(PyExc_OSError, strerror(errno_));
        return NULL;
    }
    JavaClass *new = JavaClass_from_MemReader(r);
    MemReader_free(r);
    return new;
}

static void _dealloc(PyObject *self) {
    JavaClass *class = (JavaClass *)self;

    constant_pool_free(class->pool);
    interfaces_free(class->interfaces);
    fields_free(class->fields);
    methods_free(class->methods);
    attributes_free(class->attributes);

    Py_TYPE(self)->tp_free(self);
}

static PyObject *_fields(PyObject *self, PyObject *arg) {
    JavaClass *class = (JavaClass *)self;
    struct field_items *fields = class->fields;
    struct constant_pool *pool = class->pool;

    PyObject *result = PyList_New(fields->count);
    for(int i = 0; i < fields->count; ++i) {
        struct field *field = fields->items[i];

        struct pool_Utf8 *name = constant_pool_item(pool, field->name_index);
        struct pool_Utf8 *descriptor = constant_pool_item(pool, field->descriptor_index);

        PyObject *t = PyTuple_New(4);
        PyTuple_SetItem(t, 0, conv_flags_to_PySet(field->access_flags));
        PyTuple_SetItem(t, 1, conv_utf8_to_PyUnicode(descriptor));
        PyTuple_SetItem(t, 2, conv_utf8_to_PyUnicode(name));
        PyTuple_SetItem(t, 3, conv_attributes_to_PyDict(field->attributes, pool));

        PyList_SetItem(result, i, t);
    }

    return result;
}

static PyObject *_methods(PyObject *self, PyObject *arg) {
    JavaClass *class = (JavaClass *)self;
    struct method_items *methods = class->methods;
    struct constant_pool *pool = class->pool;

    PyObject *result = PyList_New(methods->count);
    for(int i = 0; i < methods->count; ++i) {
        struct method *method = methods->items[i];

        struct pool_Utf8 *name = constant_pool_item(pool, method->name_index);
        struct pool_Utf8 *descriptor = constant_pool_item(pool, method->descriptor_index);

        PyObject *t = PyTuple_New(4);
        PyTuple_SetItem(t, 0, conv_flags_to_PySet(method->access_flags));
        PyTuple_SetItem(t, 1, conv_utf8_to_PyUnicode(descriptor));
        PyTuple_SetItem(t, 2, conv_utf8_to_PyUnicode(name));
        PyTuple_SetItem(t, 3, conv_attributes_to_PyDict(method->attributes, pool));

        PyList_SetItem(result, i, t);
    }

    return result;
}

static PyObject *_attributes(PyObject *self, PyObject *arg) {
    JavaClass *class = (JavaClass *)self;
    return conv_attributes_to_PyDict(class->attributes, class->pool);
}

static PyObject *_access_set(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    return conv_flags_to_PySet(class->access_flags);
}

static PyObject *_is_public(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    return PyBool_FromLong(class->access_flags & ACC_PUBLIC);
}

static PyObject *_is_final(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    return PyBool_FromLong(class->access_flags & ACC_FINAL);
}

static PyObject *_is_super(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    return PyBool_FromLong(class->access_flags & ACC_SUPER);
}

static PyObject *_is_interface(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    return PyBool_FromLong(class->access_flags & ACC_INTERFACE);
}

static PyObject *_is_abstract(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    return PyBool_FromLong(class->access_flags & ACC_ABSTRACT);
}

static PyObject *_is_synthetic(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    return PyBool_FromLong(class->access_flags & ACC_SYNTHETIC);
}

static PyObject *_is_annotation(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    return PyBool_FromLong(class->access_flags & ACC_ANNOTATION);
}

static PyObject *_is_enum(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    return PyBool_FromLong(class->access_flags & ACC_ENUM);
}

static PyObject *_name(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    struct pool_Class *this_class = constant_pool_item(class->pool, class->this_class);
    struct pool_Utf8 *name = constant_pool_item(class->pool, this_class->name_index);
    return conv_utf8_to_PyUnicode(name);
}

static PyObject *_superclass_name(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    struct pool_Class *super_class = constant_pool_item(class->pool, class->super_class);
    struct pool_Utf8 *name = constant_pool_item(class->pool, super_class->name_index);
    return conv_utf8_to_PyUnicode(name);
}

static PyMethodDef methods[] = {
    {"fields", _fields, METH_NOARGS, 0},
    {"methods", _methods, METH_NOARGS, 0},
    {"attributes", _attributes, METH_NOARGS, 0},
    {0},
};

static PyGetSetDef getset[] = {
    {"access_set", _access_set, NULL, NULL, NULL},
    {"is_public", _is_public, NULL, NULL, NULL},
    {"is_final", _is_final, NULL, NULL, NULL},
    {"is_super", _is_super, NULL, NULL, NULL},
    {"is_interface", _is_interface, NULL, NULL, NULL},
    {"is_abstract", _is_abstract, NULL, NULL, NULL},
    {"is_synthetic", _is_synthetic, NULL, NULL, NULL},
    {"is_annotation", _is_annotation, NULL, NULL, NULL},
    {"is_enum", _is_enum, NULL, NULL, NULL},
    {"name", _name, NULL, NULL, NULL},
    {"superclass_name", _superclass_name, NULL, NULL, NULL},
    {0},
};

static PyMemberDef members[] = {
    {"magic", T_UINT, offsetof(JavaClass, magic), READONLY, 0},
    {"minor_version", T_USHORT, offsetof(JavaClass, minor_version), READONLY, 0},
    {"major_version", T_USHORT, offsetof(JavaClass, major_version), READONLY, 0},
    {0},
};

PyTypeObject JavaClass_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "JavaClass",
    .tp_doc = NULL,
    .tp_basicsize = sizeof(JavaClass),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = NULL,
    .tp_dealloc = _dealloc,
    .tp_members = members,
    .tp_methods = methods,
    .tp_getset = getset,
};
