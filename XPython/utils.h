#ifndef UTILS__H
#define UTILS__H

#include <Python.h>
#include <stdbool.h>

extern const char *objRefName;
extern const char *commandRefName;
extern const char *widgetRefName;
extern const char *windowIDRef;
extern PyObject *widgetIDCapsules;
extern PyObject *windowIDCapsules;


void dbg(const char *msg);
bool objToList(PyObject *item, PyObject *list);
float getFloatFromTuple(PyObject *seq, Py_ssize_t i);
long getLongFromTuple(PyObject *seq, Py_ssize_t i);

PyObject *getPtrRef(void *ptr, PyObject *dict, const char *refName);
PyObject *getPtrRefOneshot(void *ptr, const char *refName);
void *refToPtr(PyObject *ref, const char *refName);
void removePtrRef(void *ptr, PyObject *dict);
char *get_module(PyThreadState *tstate);
PyObject *get_pluginSelf(/*PyThreadState *tstate*/);
char *objToStr(PyObject *item);

#endif
