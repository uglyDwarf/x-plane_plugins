#define _GNU_SOURCE 1
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMCamera.h>

static intptr_t camCntr;
static PyObject *camDict;


static int cameraControl(XPLMCameraPosition_t *outCameraPosition, int inIsLosingControl, void *inRefcon)
{
  PyObject *ref = PyLong_FromVoidPtr(inRefcon);
  PyObject *callbackInfo = PyDict_GetItem(camDict, ref);
  Py_DECREF(ref);
  if(callbackInfo == NULL){
    printf("Couldn't find cameraControl callback with id = %p.", inRefcon); 
    return 0;
  }

  PyObject *pos;
  if(!inIsLosingControl){
    pos = PyList_New(7);
    PyList_SetItem(pos, 0, PyFloat_FromDouble(outCameraPosition->x));
    PyList_SetItem(pos, 1, PyFloat_FromDouble(outCameraPosition->y));
    PyList_SetItem(pos, 2, PyFloat_FromDouble(outCameraPosition->z));
    PyList_SetItem(pos, 3, PyFloat_FromDouble(outCameraPosition->pitch));
    PyList_SetItem(pos, 4, PyFloat_FromDouble(outCameraPosition->heading));
    PyList_SetItem(pos, 5, PyFloat_FromDouble(outCameraPosition->roll));
    PyList_SetItem(pos, 6, PyFloat_FromDouble(outCameraPosition->zoom));
  }else{
    pos = Py_None;
    Py_INCREF(pos);
  }

  PyObject *fun = PyTuple_GetItem(callbackInfo, 2);
  PyObject *lc = PyLong_FromLong(inIsLosingControl);
  PyObject *refcon = PyTuple_GetItem(callbackInfo, 3);
  PyObject *resObj = PyObject_CallFunctionObjArgs(fun, pos, lc, refcon, NULL);
  Py_DECREF(lc);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }

  if((outCameraPosition != NULL) && !inIsLosingControl){
    PyObject *elem;
    if(PyList_Size(pos) != 7){
      PyErr_SetString(PyExc_RuntimeError ,"outCameraPosition must contain 7 floats.\n");
      return -1;
    }

    for(int i = 0; i < 7; ++i){
      elem = PyList_GetItem(pos, i);
      switch(i){
        case 0:
          outCameraPosition->x = PyFloat_AsDouble(elem);
          break;
        case 1:
          outCameraPosition->y = PyFloat_AsDouble(elem);
          break;
        case 2:
          outCameraPosition->z = PyFloat_AsDouble(elem);
          break;
        case 3:
          outCameraPosition->pitch = PyFloat_AsDouble(elem);
          break;
        case 4:
          outCameraPosition->heading = PyFloat_AsDouble(elem);
          break;
        case 5:
          outCameraPosition->roll = PyFloat_AsDouble(elem);
          break;
        case 6:
          outCameraPosition->zoom = PyFloat_AsDouble(elem);
          break;
      }
    }
  }
  Py_DECREF(pos);
  int res = PyLong_AsLong(resObj);
  Py_DECREF(resObj);
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
  Py_DECREF(refconObj);
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
  PyObject *resArray = NULL;
  bool returnRes = false;
  if(PyTuple_Size(args) == 1){
    if(!PyArg_ParseTuple(args, "O", &resArray)){
      return NULL;
    }
  }
  if(!resArray){
    resArray = PyList_New(0);
    returnRes = true;
  }
  if(!PyList_Check(resArray)){
    PyErr_SetString(PyExc_RuntimeError ,"Argument must be list.\n");
    return NULL;
  }
  XPLMCameraPosition_t pos;
  XPLMReadCameraPosition(&pos);
  PyObject *tmp;
  tmp = PyFloat_FromDouble(pos.x);
  PyList_Append(resArray, tmp);
  Py_DECREF(tmp);
  
  tmp = PyFloat_FromDouble(pos.y);
  PyList_Append(resArray, tmp);
  Py_DECREF(tmp);
  
  tmp = PyFloat_FromDouble(pos.z);
  PyList_Append(resArray, tmp);
  Py_DECREF(tmp);
  
  tmp = PyFloat_FromDouble(pos.pitch);
  PyList_Append(resArray, tmp);
  Py_DECREF(tmp);
  
  tmp = PyFloat_FromDouble(pos.heading);
  PyList_Append(resArray, tmp);
  Py_DECREF(tmp);
  
  tmp = PyFloat_FromDouble(pos.roll);
  PyList_Append(resArray, tmp);
  Py_DECREF(tmp);
  
  tmp = PyFloat_FromDouble(pos.zoom);
  PyList_Append(resArray, tmp);
  Py_DECREF(tmp);

  if(returnRes){
    return resArray;
  }else{
    Py_RETURN_NONE;
  }
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(camDict);
  Py_DECREF(camDict);
  Py_RETURN_NONE;
}

static PyMethodDef XPLMCameraMethods[] = {
  {"XPLMControlCamera", XPLMControlCameraFun, METH_VARARGS, ""},
  {"XPLMDontControlCamera", XPLMDontControlCameraFun, METH_VARARGS, ""},
  {"XPLMIsCameraBeingControlled", XPLMIsCameraBeingControlledFun, METH_VARARGS, ""},
  {"XPLMReadCameraPosition", XPLMReadCameraPositionFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
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




