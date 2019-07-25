#define _GNU_SOURCE 1
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils.h"

const char *objRefName = "XPLMObjectRef"; 
const char *commandRefName = "XPLMCommandRef"; 

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
  return PyCapsule_New(ptr, refName, NULL);
}

PyObject *getPtrRef(void *ptr, PyObject *dict, const char *refName)
{
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
  return PyCapsule_GetPointer(ref, refName);
}

void removePtrRef(void *ptr, PyObject *dict)
{
  PyObject *key = PyLong_FromVoidPtr(ptr);
  PyDict_DelItem(dict, key);
  Py_DECREF(key);
}



