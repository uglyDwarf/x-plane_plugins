#define _GNU_SOURCE 1
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils.h"

const char *objRefName = "XPLMObjectRef";
const char *commandRefName = "XPLMCommandRef";
const char *widgetRefName = "XPLMWidgetID";

void dbg(const char *msg){
  printf("Going to check %s\n", msg);
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the %s call:\n", msg);
    PyErr_Print();
  }
}

bool objToList(PyObject *item, PyObject *list)
{
  if(!PyList_Check(list)){
    return false;
  }
  if(PyList_Size(list) > 0){
    PyList_SetItem(list, 0, item);
  }else{
    PyList_Append(list, item);
    Py_DECREF(item);
  }
  return true;
}

float getFloatFromTuple(PyObject *seq, Py_ssize_t i)
{
  return PyFloat_AsDouble(PyTuple_GetItem(seq, i));
}

long getLongFromTuple(PyObject *seq, Py_ssize_t i)
{
  return PyLong_AsLong(PyTuple_GetItem(seq, i));
}

// To avoid Python code messing with raw pointers (when passed
//   in using PyLong_FromVoidPtr), these are hidden in the capsules.

// Can be used where no callbacks are involved in passing the capsule
PyObject *getPtrRefOneshot(void *ptr, const char *refName)
{
  if(ptr){
    return PyCapsule_New(ptr, refName, NULL);
  }else{
    Py_RETURN_NONE;
  }
}

PyObject *getPtrRef(void *ptr, PyObject *dict, const char *refName)
{
  if(!ptr){
    Py_RETURN_NONE;
  }
  // Check if the refernece is known
  PyObject *key = PyLong_FromVoidPtr(ptr);
  PyObject *res = PyDict_GetItem(dict, key);
  if(res == NULL){
    // New ref, register it
    res = getPtrRefOneshot(ptr, refName);
    PyDict_SetItem(dict, key, res);
  }
  Py_INCREF(res);
  return res;
}

void *refToPtr(PyObject *ref, const char *refName)
{
  if(PyCapsule_CheckExact(ref)){
    return PyCapsule_GetPointer(ref, refName);
  }else if(ref == Py_None){
    return NULL;
  }else if(PyLong_Check(ref) && (PyLong_AsLong(ref) == 0)){
    return NULL;
  }
  // If we got here, then ref is something we didn't expect.
  PyErr_Format(PyExc_RuntimeError, "Error extracting %s reference from %S", refName, ref);
  return NULL;
}

void removePtrRef(void *ptr, PyObject *dict)
{
  if(!ptr){
    return;
  }
  PyObject *key = PyLong_FromVoidPtr(ptr);
  PyDict_DelItem(dict, key);
  Py_DECREF(key);
}



