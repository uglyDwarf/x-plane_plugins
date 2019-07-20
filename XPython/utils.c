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

