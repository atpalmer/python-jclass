#include <Python.h>
#include "membuff.h"
#include "javaclassobj.h"


static PyObject *jclass_load(PyObject *self, PyObject *args) {
    char *fname;
    if(!PyArg_ParseTuple(args, "s", &fname))
        return NULL;

    MemReader *r = MemReader_from_filename(fname);
    JavaClass *new = JavaClass_from_MemReader(r);
    MemReader_free(r);

    return (PyObject *)new;
}

static PyMethodDef module_methods[] = {
    {"load", jclass_load, METH_VARARGS},
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

