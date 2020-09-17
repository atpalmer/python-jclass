#include <Python.h>
#include "structmember.h"
#include "membuff.h"
#include "access.h"
#include "constant_pool.h"
#include "interfaces.h"
#include "fields.h"
#include "methods.h"
#include "attributes.h"
#include "javaclassobj.h"

static inline void *_JavaClassConstantPool_item(JavaClassConstantPool *this, uint16_t i) {
    if(i < 1 || i > this->count - 1) {
        PyErr_SetString(PyExc_IndexError, "Invalid index");
        return NULL;
    }
    return this->constants[i - 1];
}

static PyObject *_flags_to_PySet(uint16_t flags) {
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

JavaClass *JavaClass_from_MemReader(MemReader *r) {
    JavaClass *class = (JavaClass *)JavaClass_Type.tp_alloc(&JavaClass_Type, 0);

    class->magic = MemReader_next_uint32(r);
    class->minor_version = MemReader_next_uint16(r);
    class->major_version = MemReader_next_uint16(r);

    constant_pool_parse(r, &class->constant_pool);

    class->access_flags = MemReader_next_uint16(r);
    class->this_class = MemReader_next_uint16(r);
    class->super_class = MemReader_next_uint16(r);

    interfaces_parse(r, &class->interfaces);
    fields_parse(r, &class->fields);
    parse_methods(r, &class->methods);
    attributes_parse(r, &class->attributes);

    return class;
}

static void _dealloc(PyObject *self) {
    JavaClass *class = (JavaClass *)self;
    JavaClassConstantPool_free(class->constant_pool);
    JavaClassInterfaces_free(class->interfaces);
    JavaClassFields_free(class->fields);
    JavaClassMethods_free(class->methods);
    JavaClassAttributes_free(class->attributes);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *_constant(PyObject *self, PyObject *arg) {
    if(!PyLong_Check(arg))
        return NULL;
    unsigned long index = PyLong_AsUnsignedLong(arg);
    JavaClassConstantPool *pool = ((JavaClass *)self)->constant_pool;
    if(index < 1 || index > pool->count - 1) {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *_attributes_to_PyDict(JavaClassAttributes *attributes, JavaClassConstantPool *pool) {
    PyObject *dict = PyDict_New();

    for(uint16_t i = 0; i < attributes->count; ++i) {
        JavaClassAttribute *attr = attributes->items[i];

        JavaClassUtf8Constant *name = _JavaClassConstantPool_item(pool, attr->name_index);
        PyObject *key = PyUnicode_FromStringAndSize(name->bytes, name->length);
        PyObject *value = PyBytes_FromStringAndSize((void *)attr->info, attr->length);

        PyDict_SetItem(dict, key, value);
    }

    return dict;
}

static PyObject *_fields(PyObject *self, PyObject *arg) {
    JavaClass *class = (JavaClass *)self;
    JavaClassFields *fields = class->fields;
    JavaClassConstantPool *pool = class->constant_pool;

    PyObject *result = PyList_New(fields->count);
    for(int i = 0; i < fields->count; ++i) {
        JavaClassField *field = fields->items[i];

        JavaClassUtf8Constant *name = _JavaClassConstantPool_item(pool, field->name_index);
        JavaClassUtf8Constant *descriptor = _JavaClassConstantPool_item(pool, field->descriptor_index);

        PyObject *t = PyTuple_New(4);
        PyTuple_SetItem(t, 0, _flags_to_PySet(fields->count));
        PyTuple_SetItem(t, 1, PyUnicode_FromStringAndSize(descriptor->bytes, descriptor->length));
        PyTuple_SetItem(t, 2, PyUnicode_FromStringAndSize(name->bytes, name->length));
        PyTuple_SetItem(t, 3, _attributes_to_PyDict(field->attributes, pool));

        PyList_SetItem(result, i, t);
    }

    return result;
}

static PyObject *_methods(PyObject *self, PyObject *arg) {
    JavaClass *class = (JavaClass *)self;
    JavaClassMethods *methods = class->methods;
    JavaClassConstantPool *pool = class->constant_pool;

    PyObject *result = PyList_New(methods->count);
    for(int i = 0; i < methods->count; ++i) {
        JavaClassMethod *method = methods->items[i];

        JavaClassUtf8Constant *name = _JavaClassConstantPool_item(pool, method->name_index);
        JavaClassUtf8Constant *descriptor = _JavaClassConstantPool_item(pool, method->descriptor_index);

        PyObject *t = PyTuple_New(4);
        PyTuple_SetItem(t, 0, _flags_to_PySet(method->access_flags));
        PyTuple_SetItem(t, 1, PyUnicode_FromStringAndSize(descriptor->bytes, descriptor->length));
        PyTuple_SetItem(t, 2, PyUnicode_FromStringAndSize(name->bytes, name->length));
        PyTuple_SetItem(t, 3, _attributes_to_PyDict(method->attributes, pool));

        PyList_SetItem(result, i, t);
    }

    return result;
}

static PyObject *_attributes(PyObject *self, PyObject *arg) {
    JavaClass *class = (JavaClass *)self;
    JavaClassAttributes *attributes = class->attributes;
    JavaClassConstantPool *pool = class->constant_pool;
    return _attributes_to_PyDict(attributes, pool);
}

static PyObject *_access_set(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    return _flags_to_PySet(class->access_flags);
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
    JavaClassConstantPool *pool = class->constant_pool;
    JavaClassClassConstant *this_class = _JavaClassConstantPool_item(pool, class->this_class);
    JavaClassUtf8Constant *name = _JavaClassConstantPool_item(pool, this_class->name_index);
    return PyUnicode_FromStringAndSize(name->bytes, name->length);
}

static PyObject *_superclass_name(PyObject *self, void *closure) {
    JavaClass *class = (JavaClass *)self;
    JavaClassConstantPool *pool = class->constant_pool;
    JavaClassClassConstant *super_class = _JavaClassConstantPool_item(pool, class->super_class);
    JavaClassUtf8Constant *name = _JavaClassConstantPool_item(pool, super_class->name_index);
    return PyUnicode_FromStringAndSize(name->bytes, name->length);
}

static PyMethodDef methods[] = {
    {"constant", _constant, METH_O, 0},
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
