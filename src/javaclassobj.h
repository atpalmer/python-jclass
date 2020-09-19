#ifndef JAVACLASSOBJ_H
#define JAVACLASSOBJ_H

#include <Python.h>

PyObject *JavaClass_from_filename(const char *filename);

extern PyTypeObject JavaClass_Type;

#endif
