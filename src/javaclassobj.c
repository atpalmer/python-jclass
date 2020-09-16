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


static inline JavaClass *JavaClass_cast(PyObject *self) {
    return (JavaClass *)self;
}

static inline void *JavaClass_constant(JavaClass *self, uint16_t i) {
    return self->constant_pool->constant_pool[i - 1];
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

void JavaClass_print(JavaClass *class) {
    printf("Magic Number: %X\n", class->magic);
    printf("Version: %u.%u\n", class->major_version, class->minor_version);
    constant_pool_print(class->constant_pool);
    access_flags_print(class->access_flags);
    printf("This Class Pool Index: %u\n", class->this_class);
    printf("Super Class Pool Index: %u\n", class->super_class);
    interfaces_print(class->interfaces);
    fields_print(class->fields);
    methods_print(class->methods);
    printf("CLASS ATTRIBUTES:\n");
    attributes_print(class->attributes);
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
    if(index < 1 || index > pool->constant_pool_count - 1) {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *_fields(PyObject *self, PyObject *arg) {
    JavaClassFields *fields = JavaClass_cast(self)->fields;

    PyObject *result = PyList_New(fields->fields_count);
    for(int i = 0; i < fields->fields_count; ++i) {
        JavaClassField *field = fields->fields[i];

        JavaClassUtf8Constant *name = JavaClass_constant(self, field->name_index);
        JavaClassUtf8Constant *descriptor = JavaClass_constant(self, field->descriptor_index);

        PyObject *t = PyTuple_New(3);
        PyTuple_SetItem(t, 0, _flags_to_PySet(fields->fields_count));
        PyTuple_SetItem(t, 1, PyUnicode_FromStringAndSize(descriptor->bytes, descriptor->length));
        PyTuple_SetItem(t, 2, PyUnicode_FromStringAndSize(name->bytes, name->length));

        PyList_SetItem(result, i, t);
    }

    return result;
}

static PyObject *_methods(PyObject *self, PyObject *arg) {
    JavaClassMethods *methods = JavaClass_cast(self)->methods;

    PyObject *result = PyList_New(methods->methods_count);
    for(int i = 0; i < methods->methods_count; ++i) {
        JavaClassMethod *method= methods->methods[i];

        JavaClassUtf8Constant *name = JavaClass_constant(self, method->name_index);
        JavaClassUtf8Constant *descriptor = JavaClass_constant(self, method->descriptor_index);

        PyObject *t = PyTuple_New(3);
        PyTuple_SetItem(t, 0, _flags_to_PySet(method->access_flags));
        PyTuple_SetItem(t, 1, PyUnicode_FromStringAndSize(descriptor->bytes, descriptor->length));
        PyTuple_SetItem(t, 2, PyUnicode_FromStringAndSize(name->bytes, name->length));

        PyList_SetItem(result, i, t);
    }

    return result;
}

static PyObject *_access_set(PyObject *self, void *closure) {
    uint16_t flags = JavaClass_cast(self)->access_flags;
    return _flags_to_PySet(flags);
}

static PyObject *_is_public(PyObject *self, void *closure) {
    uint16_t flags = JavaClass_cast(self)->access_flags;
    return PyBool_FromLong(flags & ACC_PUBLIC);
}

static PyObject *_is_final(PyObject *self, void *closure) {
    uint16_t flags = JavaClass_cast(self)->access_flags;
    return PyBool_FromLong(flags & ACC_FINAL);
}

static PyObject *_is_super(PyObject *self, void *closure) {
    uint16_t flags = JavaClass_cast(self)->access_flags;
    return PyBool_FromLong(flags & ACC_SUPER);
}

static PyObject *_is_interface(PyObject *self, void *closure) {
    uint16_t flags = JavaClass_cast(self)->access_flags;
    return PyBool_FromLong(flags & ACC_INTERFACE);
}

static PyObject *_is_abstract(PyObject *self, void *closure) {
    uint16_t flags = JavaClass_cast(self)->access_flags;
    return PyBool_FromLong(flags & ACC_ABSTRACT);
}

static PyObject *_is_synthetic(PyObject *self, void *closure) {
    uint16_t flags = JavaClass_cast(self)->access_flags;
    return PyBool_FromLong(flags & ACC_SYNTHETIC);
}

static PyObject *_is_annotation(PyObject *self, void *closure) {
    uint16_t flags = JavaClass_cast(self)->access_flags;
    return PyBool_FromLong(flags & ACC_ANNOTATION);
}

static PyObject *_is_enum(PyObject *self, void *closure) {
    uint16_t flags = JavaClass_cast(self)->access_flags;
    return PyBool_FromLong(flags & ACC_ENUM);
}

static PyObject *_name(PyObject *self, void *closure) {
    JavaClassClassConstant *class = JavaClass_constant(self, JavaClass_cast(self)->this_class);
    JavaClassUtf8Constant *name = JavaClass_constant(self, class->name_index);
    return PyUnicode_FromStringAndSize(name->bytes, name->length);
}

static PyObject *_superclass_name(PyObject *self, void *closure) {
    JavaClassClassConstant *class = JavaClass_constant(self, JavaClass_cast(self)->super_class);
    JavaClassUtf8Constant *name = JavaClass_constant(self, class->name_index);
    return PyUnicode_FromStringAndSize(name->bytes, name->length);
}

static PyMethodDef methods[] = {
    {"constant", _constant, METH_O, 0},
    {"fields", _fields, METH_NOARGS, 0},
    {"methods", _methods, METH_NOARGS, 0},
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
