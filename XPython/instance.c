#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#define XPLM210
#include "plugin_dl.h"
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMScenery.h>
#include <XPLM/XPLMInstance.h>


static PyObject *XPLMCreateInstanceFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *obj, *drefList;
  if(!XPLMCreateInstance_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateInstance is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OO", &obj, &drefList)){
    return NULL;
  }
  Py_ssize_t len = PySequence_Length(drefList);
  const char **datarefs = malloc(sizeof(char *) * (len + 1));
  if(datarefs == NULL){
    return NULL;
  }
  // Malloced len + 1, so we're not overflowing!
  datarefs[len] = NULL;

  Py_ssize_t i;
  PyObject *drefListTuple = PySequence_Tuple(drefList);
  for(i = 0; i < len; ++i){
    PyObject *s = PyObject_Str(PyTuple_GetItem(drefList, i));
    datarefs[i] = PyUnicode_AsUTF8(s);
    Py_DECREF(s);
  }
  Py_DECREF(drefListTuple);
  XPLMObjectRef inObj = PyLong_AsVoidPtr(obj);

  XPLMInstanceRef res = XPLMCreateInstance_ptr(inObj, datarefs);
  free(datarefs);
  return PyLong_FromVoidPtr(res);
}

static PyObject *XPLMDestroyInstanceFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyObject *instance;
  if(!XPLMDestroyInstance_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyInstance is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "O", &instance)){
    return NULL;
  }
  XPLMDestroyInstance_ptr(PyLong_AsVoidPtr(instance));
  Py_RETURN_NONE;
}

inline static float getFloat(PyObject *seq, Py_ssize_t i)
{
  PyObject *tmp = PyNumber_Float(PyTuple_GetItem(seq, i));
  float val = PyFloat_AsDouble(tmp);
  Py_DECREF(tmp);
  return val;
}

static PyObject *XPLMInstanceSetPositionFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *instance, *newPositionSeq, *data;
  if(!XPLMInstanceSetPosition_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMInstanceSetPosition is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OOO", &instance, &newPositionSeq, &data)){
    return NULL;
  }
  PyObject *newPosition = PySequence_Tuple(newPositionSeq);
  XPLMDrawInfo_t inNewPosition;
  inNewPosition.structSize = sizeof(XPLMDrawInfo_t);
  inNewPosition.x = getFloat(newPosition, 0);
  inNewPosition.y = getFloat(newPosition, 1);
  inNewPosition.z = getFloat(newPosition, 2);
  inNewPosition.pitch = getFloat(newPosition, 3);
  inNewPosition.heading = getFloat(newPosition, 4);
  inNewPosition.roll = getFloat(newPosition, 5);

  Py_ssize_t len = PySequence_Length(data);
  float *inData = malloc(sizeof(float) * len);
  if(inData == NULL){
    return NULL;
  }
  Py_ssize_t i;
  for(i = 0; i < len; ++i){
    inData[i] = getFloat(data, i);
  }
  XPLMInstanceSetPosition_ptr(PyLong_AsVoidPtr(instance), &inNewPosition, inData);
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyMethodDef XPLMInstanceMethods[] = {
  {"XPLMCreateInstance", XPLMCreateInstanceFun, METH_VARARGS, ""},
  {"XPLMDestroyInstance", XPLMDestroyInstanceFun, METH_VARARGS, ""},
  {"XPLMInstanceSetPosition", XPLMInstanceSetPositionFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMInstanceModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMInstance",
  NULL,
  -1,
  XPLMInstanceMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMInstance(void)
{
  PyObject *mod = PyModule_Create(&XPLMInstanceModule);
  return mod;
}




