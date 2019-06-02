#define _GNU_SOURCE 1
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMCamera.h>

static intptr camCntr;
static PyObject *camDict;

static int cameraControl(XPLMCameraPosition_t *outCameraPosition, int inIsLosingControl, void *inRefcon)
{
  PyObject *ref = PyLong_FromVoidPtr(inRefCon);
  PyObject *id = PyDict_GetItem(camDict, ref);
  if(id == NULL){
    return NULL;
  }
  return 0;
}

static PyObject *XPLMDontControlCameraFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inHowLong;
  PyObject *pluginSelf, *controlFunc, *refcon;
  if(!PyArg_ParseTuple(args, "OiOO", &pluginSelf, &inHowLong, &controlFunc, &refcon)){
    return NULL;
  }
  void *inRefcon = (void *)++camCntr;
  PyObject *refconObj = PyLong_FromVoidPtr(inRefcon);
  PyDict_SetItem(camDict, refconObj, args);
  XPLMControlCamera(inHowLong, cameraControl, inRefcon);
  Py_XDECREF(refconObj);
  Py_RETURN_NONE;
}

static PyObject *XPLMDontControlCameraFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  XPLMDontControlCamera();
  Py_RETURN_NONE;
}

static PyObject *XPLMIsCameraBeingontrolledFun(PyObject *self, PyObject *args)
{
  (void) self;
  XPLMCameraControlDuration dur;
  int res = XPLMIsCameraBeingontrolled(&dur);
  return Py_BuildValue("(ii)", res, dur);
}

static PyObject *XPLMReadCameraPositionFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *resArray;
  if(!PyArg_ParseTuple("O", &resArray)){
    return NULL;
  }
  if(!PyList_Check(resArray)){
    PyErr_SetString(PyExc_RuntimeError ,"Argument must be list.\n");
    return NULL;
  }
  XPLMCameraPosition_t pos;
  XPLMReadCameraPosition(&pos);
  PyList_Append(resArray, pos.x);
  PyList_Append(resArray, pos.y);
  PyList_Append(resArray, pos.z);
  PyList_Append(resArray, pos.pitch);
  PyList_Append(resArray, pos.heading);
  PyList_Append(resArray, pos.roll);
  PyList_Append(resArray, pos.zoom);
  Py_RETURN_NONE;
}

static PyMethodDef XPLMCameraMethods[] = {
  {"XPLMXPLMControlCamera", XPLMXPLMControlCameraFun, METH_VARARGS, ""},
  {"XPLMDontControlCamera", XPLMDontControlCameraFun, METH_VARARGS, ""},
  {"XPLMIsCameraBeingontrolled", XPLMIsCameraBeingontrolledFun, METH_VARARGS, ""},
  {"XPLMReadCameraPosition", XPLMReadCameraPositionFun, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMCameraModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMCamera",
  NULL,
  -1,
  XPLMCameraMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMCamera(void)
{
  if(!(camDict = PyDict_New())){
    return NULL;
  }
  PyObject *mod = PyModule_Create(&XPLMCameraModule);
  if(mod){
    PyModule_AddIntConstant(mod, "xplm_ControlCameraUntilViewChanges", xplm_ControlCameraUntilViewChanges);
    PyModule_AddIntConstant(mod, "xplm_ControlCameraForever", xplm_ControlCameraForever);
  }

  return mod;
}




