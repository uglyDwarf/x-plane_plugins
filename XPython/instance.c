#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include "plugin_dl.h"
#include "utils.h"
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMScenery.h>
#include <XPLM/XPLMInstance.h>


static const char instanceRefName[] = "XPLMInstanceRef";

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
  const char *tmp;
  for(i = 0; i < len; ++i){
    PyObject *s = PyTuple_GetItem(drefListTuple, i);
    tmp = asString(s);
    if (PyErr_Occurred()) {
      stringCleanup();
      return NULL;
    }
    datarefs[i] = tmp;
  }
  Py_DECREF(drefListTuple);
  XPLMObjectRef inObj = refToPtr(obj, objRefName);

  XPLMInstanceRef res = XPLMCreateInstance_ptr(inObj, datarefs);
  stringCleanup();
  free(datarefs);
  return getPtrRefOneshot(res, instanceRefName);
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
  XPLMDestroyInstance_ptr(refToPtr(instance, instanceRefName));
  Py_RETURN_NONE;
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
  inNewPosition.x = getFloatFromTuple(newPosition, 0);
  inNewPosition.y = getFloatFromTuple(newPosition, 1);
  inNewPosition.z = getFloatFromTuple(newPosition, 2);
  inNewPosition.pitch = getFloatFromTuple(newPosition, 3);
  inNewPosition.heading = getFloatFromTuple(newPosition, 4);
  inNewPosition.roll = getFloatFromTuple(newPosition, 5);
  Py_DECREF(newPosition);
  Py_ssize_t len = PySequence_Length(data);
  float *inData = malloc(sizeof(float) * len);
  if(inData == NULL){
    return NULL;
  }
  Py_ssize_t i;
  PyObject *dataTuple = PySequence_Tuple(data);
  for(i = 0; i < len; ++i){
    inData[i] = getFloatFromTuple(dataTuple, i);
  }
  Py_DECREF(dataTuple);
  XPLMInstanceSetPosition_ptr(refToPtr(instance, instanceRefName), &inNewPosition, inData);
  free(inData);
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




