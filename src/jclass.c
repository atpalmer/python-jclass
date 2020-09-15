#include <Python.h>
#include "membuff.h"
#include "javaclassobj.h"


static PyObject *jclass_load(PyObject *self, PyObject *args) {
    char *fname;
    if(!PyArg_ParseTuple(args, "s", &fname))
        return NULL;

    MemReader *r = MemReader_from_filename(fname);
    JavaClass *class = JavaClass_from_MemReader(r);
    JavaClass_print(class);
    JavaClass_free(class);

    MemReader_print(r);

    PyObject *result = PyBytes_FromStringAndSize((char *)r->data, r->size);
    MemReader_free(r);
    return result;

    /* TODO: return JavaClass object (as PyObject) */
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
    return m;
}

