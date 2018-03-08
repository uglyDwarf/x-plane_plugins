#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMScenery.h>
#include <XPLM/XPLMInstance.h>


static PyObject *XPLMCreateInstanceFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *obj, *drefList;
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
  for(i = 0; i < len; ++i){
    PyObject *s = PyObject_Str(PySequence_GetItem(drefList, i));
    datarefs[i] = PyUnicode_AsUTF8(s);
    Py_DECREF(s);
  }

  XPLMObjectRef inObj = PyLong_AsVoidPtr(obj);

  XPLMInstanceRef res = XPLMCreateInstance(inObj, datarefs);
  free(datarefs);
  return PyLong_FromVoidPtr(res);
}

static PyObject *XPLMDestroyInstanceFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyObject *instance;
  if(!PyArg_ParseTuple(args, "O", &instance)){
    return NULL;
  }
  XPLMDestroyInstance(PyLong_AsVoidPtr(instance));
  Py_RETURN_NONE;
}

inline static float getFloat(PyObject *seq, Py_ssize_t i)
{
  PyObject *tmp = PyNumber_Float(PySequence_GetItem(seq, i));
  float val = PyFloat_AsDouble(tmp);
  Py_DECREF(tmp);
  return val;
}

static PyObject *XPLMInstanceSetPositionFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *instance, *new_position, *data;
  if(!PyArg_ParseTuple(args, "OOO", &instance, &new_position, &data)){
    return NULL;
  }

  XPLMDrawInfo_t inNewPosition;
  inNewPosition.structSize = sizeof(XPLMDrawInfo_t);
  inNewPosition.x = getFloat(new_position, 0);
  inNewPosition.y = getFloat(new_position, 1);
  inNewPosition.z = getFloat(new_position, 2);
  inNewPosition.pitch = getFloat(new_position, 3);
  inNewPosition.heading = getFloat(new_position, 4);
  inNewPosition.roll = getFloat(new_position, 5);

  Py_ssize_t len = PySequence_Length(data);
  float *inData = malloc(sizeof(float) * len);
  if(inData == NULL){
    return NULL;
  }
  Py_ssize_t i;
  for(i = 0; i < len; ++i){
    inData[i] = getFloat(data, i);
  }
  XPLMInstanceSetPosition(PyLong_AsVoidPtr(instance), &inNewPosition, inData);
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




