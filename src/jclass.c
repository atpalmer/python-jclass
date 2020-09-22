#include <Python.h>
#include <errno.h>
#include "javaclassobj.h"


static PyObject *jclass_load(PyObject *self, PyObject *args) {
    char *fname;
    if(!PyArg_ParseTuple(args, "s", &fname))
        return NULL;

    PyObject *new = JavaClass_from_filename(fname);
    if(!new)
        return NULL;

    return new;
}

static PyMethodDef module_methods[] = {
    {"load", jclass_load, METH_VARARGS, NULL},
    {0},
};

static PyModuleDef module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "jclass",
    .m_size = 0,
    .m_methods = module_methods,
};

PyMODINIT_FUNC PyInit_jclass(void) {
    PyObject *m = PyModule_Create(&module);

    if(PyType_Ready(&JavaClass_Type) < 0)
        return NULL;

    Py_INCREF(&JavaClass_Type);

    PyModule_AddObject(m, JavaClass_Type.tp_name, (PyObject *)&JavaClass_Type);

    return m;
}

