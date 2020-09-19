#include <Python.h>
#include "structmember.h"
#include "core/javaclass.h"
#include "conv.h"
#include "javaclassobj.h"

typedef struct {
    PyObject_HEAD
    struct javaclass *javaclass;
} JavaClass;

PyObject *JavaClass_from_filename(const char *filename) {
    JavaClass *new = (JavaClass *)JavaClass_Type.tp_alloc(&JavaClass_Type, 0);
    if(!new)
        return NULL;
    new->javaclass = javaclass_from_filename(filename);
    if(!new->javaclass)
        goto fail;
    return (PyObject *)new;

fail:
    Py_DECREF(new);
    return NULL;
}

static void _dealloc(PyObject *self) {
    JavaClass *obj = (JavaClass *)self;
    javaclass_free(obj->javaclass);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *_fields(PyObject *self, PyObject *arg) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
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
    struct javaclass *class = ((JavaClass *)self)->javaclass;
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
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return conv_attributes_to_PyDict(class->attributes, class->pool);
}

static PyObject *_magic(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return PyLong_FromUnsignedLong(class->magic);
}

static PyObject *_minor_version(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return PyLong_FromUnsignedLong(class->minor_version);
}

static PyObject *_major_version(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return PyLong_FromUnsignedLong(class->major_version);
}

static PyObject *_access_set(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return conv_flags_to_PySet(class->access_flags);
}

static PyObject *_is_public(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return PyBool_FromLong(class->access_flags & ACC_PUBLIC);
}

static PyObject *_is_final(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return PyBool_FromLong(class->access_flags & ACC_FINAL);
}

static PyObject *_is_super(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return PyBool_FromLong(class->access_flags & ACC_SUPER);
}

static PyObject *_is_interface(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return PyBool_FromLong(class->access_flags & ACC_INTERFACE);
}

static PyObject *_is_abstract(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return PyBool_FromLong(class->access_flags & ACC_ABSTRACT);
}

static PyObject *_is_synthetic(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return PyBool_FromLong(class->access_flags & ACC_SYNTHETIC);
}

static PyObject *_is_annotation(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return PyBool_FromLong(class->access_flags & ACC_SYNTHETIC);
    return PyBool_FromLong(class->access_flags & ACC_ANNOTATION);
}

static PyObject *_is_enum(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    return PyBool_FromLong(class->access_flags & ACC_ENUM);
}

static PyObject *_name(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
    struct pool_Class *this_class = constant_pool_item(class->pool, class->this_class);
    struct pool_Utf8 *name = constant_pool_item(class->pool, this_class->name_index);
    return conv_utf8_to_PyUnicode(name);
}

static PyObject *_superclass_name(PyObject *self, void *closure) {
    struct javaclass *class = ((JavaClass *)self)->javaclass;
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
    {"magic", _magic, NULL, NULL, NULL},
    {"minor_version", _minor_version, NULL, NULL, NULL},
    {"major_version", _major_version, NULL, NULL, NULL},
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

PyTypeObject JavaClass_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "JavaClass",
    .tp_doc = NULL,
    .tp_basicsize = sizeof(JavaClass),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = NULL,
    .tp_dealloc = _dealloc,
    .tp_methods = methods,
    .tp_getset = getset,
};
