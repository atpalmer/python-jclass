#include <Python.h>
#include <jclass/error.h>

void error_set_pyerr_from_javaclass_error(void)
{
    const char *msg = NULL;
    enum javaclass_errcode code = javaclass_error_get(&msg);

    switch (code) {
    case JAVACLASS_ERR_MEMORY:
        PyErr_SetString(PyExc_MemoryError, msg);
        break;
    case JAVACLASS_ERR_OS:
        PyErr_SetString(PyExc_OSError, msg);
        break;
    case JAVACLASS_ERR_CAFEBABE:
    case JAVACLASS_ERR_BADVER:
    case JAVACLASS_ERR_PARSE:
        PyErr_SetString(PyExc_ValueError, msg);
        break;
    case JAVACLASS_ERR_OK:
        break;
    }
}
