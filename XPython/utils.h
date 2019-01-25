#ifndef UTILS__H
#define UTILS__H

#include <Python.h>
#include <stdbool.h>

void dbg(const char *msg);
bool objToList(PyObject *item, PyObject *list);

#endif
