#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMCamera.h>

static intptr_t camCntr;
static PyObject *camDict;


void dbg(const char *msg){
  printf("Going to check %s\n", msg);
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the %s call:\n", msg);
    PyErr_Print();
  }
}


static int cameraControl(XPLMCameraPosition_t *outCameraPosition, int inIsLosingControl, void *inRefcon)
{
  PyObject *ref = PyLong_FromVoidPtr(inRefcon);
  PyObject *callbackInfo = PyDict_GetItem(camDict, ref);
  Py_XDECREF(ref);
  if(callbackInfo == NULL){
    printf("Couldn't find cameraControl callback with id = %p.", inRefcon); 
    return 0;
  }
  PyObject *pos = PyList_New(0);
  PyObject *resObj = PyObject_CallFunction(PySequence_GetItem(callbackInfo, 2), "OiO", pos, inIsLosingControl,
                                        PySequence_GetItem(callbackInfo, 3));
  if(outCameraPosition != NULL){
    if(PyList_Size(pos) != 7){
      PyErr_SetString(PyExc_RuntimeError ,"outCameraPosition must contain 7 floats.\n");
      return -1;
    }
    outCameraPosition->x = PyFloat_AsDouble(PyNumber_Float(PySequence_GetItem(pos, 0)));
    outCameraPosition->y = PyFloat_AsDouble(PyNumber_Float(PySequence_GetItem(pos, 1)));
    outCameraPosition->z = PyFloat_AsDouble(PyNumber_Float(PySequence_GetItem(pos, 2)));
    outCameraPosition->pitch = PyFloat_AsDouble(PyNumber_Float(PySequence_GetItem(pos, 3)));
    outCameraPosition->heading = PyFloat_AsDouble(PyNumber_Float(PySequence_GetItem(pos, 4)));
    outCameraPosition->roll = PyFloat_AsDouble(PyNumber_Float(PySequence_GetItem(pos, 5)));
    outCameraPosition->zoom = PyFloat_AsDouble(PyNumber_Float(PySequence_GetItem(pos, 6)));
  }
  int res = PyLong_AsLong(PyNumber_Long(resObj));
  Py_XDECREF(resObj);
  return res;
}

static PyObject *XPLMControlCameraFun(PyObject *self, PyObject *args)
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
  dbg("Here too!\n");
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

static PyObject *XPLMIsCameraBeingControlledFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  XPLMCameraControlDuration dur;
  int res = XPLMIsCameraBeingControlled(&dur);
  return Py_BuildValue("(ii)", res, dur);
}

static PyObject *XPLMReadCameraPositionFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *resArray;
  if(!PyArg_ParseTuple(args, "O", &resArray)){
    return NULL;
  }
  if(!PyList_Check(resArray)){
    PyErr_SetString(PyExc_RuntimeError ,"Argument must be list.\n");
    return NULL;
  }
  XPLMCameraPosition_t pos;
  XPLMReadCameraPosition(&pos);
  PyList_Append(resArray, PyFloat_FromDouble(pos.x));
  PyList_Append(resArray, PyFloat_FromDouble(pos.y));
  PyList_Append(resArray, PyFloat_FromDouble(pos.z));
  PyList_Append(resArray, PyFloat_FromDouble(pos.pitch));
  PyList_Append(resArray, PyFloat_FromDouble(pos.heading));
  PyList_Append(resArray, PyFloat_FromDouble(pos.roll));
  PyList_Append(resArray, PyFloat_FromDouble(pos.zoom));
  Py_RETURN_NONE;
}

static PyMethodDef XPLMCameraMethods[] = {
  {"XPLMControlCamera", XPLMControlCameraFun, METH_VARARGS, ""},
  {"XPLMDontControlCamera", XPLMDontControlCameraFun, METH_VARARGS, ""},
  {"XPLMIsCameraBeingControlled", XPLMIsCameraBeingControlledFun, METH_VARARGS, ""},
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




