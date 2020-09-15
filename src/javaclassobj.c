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

static PyMethodDef methods[] = {
    {"constant", _constant, METH_O, 0},
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
};
