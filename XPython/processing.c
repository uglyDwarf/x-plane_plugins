#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMProcessing.h>
#include "plugin_dl.h"

static intptr_t flCntr;
static PyObject *flDict;
static PyObject *flRevDict;
static PyObject *flIDDict;

/*
void dbg(const char *msg){
  printf("Going to check %s\n", msg);
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the %s call:\n", msg);
    PyErr_Print();
  }
}
*/
static float flightLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, 
                                int counter, void * inRefcon)
{
  PyObject *key = PyLong_FromVoidPtr(inRefcon);
  PyObject *callbackInfo = PyDict_GetItem(flDict, key);
  Py_XDECREF(key);
  if(callbackInfo == NULL){
    printf("Unknown flightLoop callback requested! (inRefcon = %p)\n", inRefcon);
    return 0.0;
  }
  PyObject *res = PyObject_CallFunction(PySequence_GetItem(callbackInfo, 1), "(ffiO)",
                                        inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop,
                                        counter, PySequence_GetItem(callbackInfo, 3));
  float tmp;
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the flightLoop callback(inRefcon = %p):\n", inRefcon);
    PyErr_Print();
    tmp = -1.0f;
  }else{
    PyObject *f = PyNumber_Float(res);
    tmp = PyFloat_AsDouble(f);
    Py_XDECREF(f);
  }
  Py_XDECREF(res);
  return tmp;
}

static PyObject *XPLMGetElapsedTimeFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  float res = XPLMGetElapsedTime();
  return PyFloat_FromDouble(res);
}

static PyObject *XPLMGetCycleNumberFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  int res = XPLMGetCycleNumber();
  return PyLong_FromLong(res);
}



static PyObject *XPLMRegisterFlightLoopCallbackFun(PyObject* self, PyObject *args)
{
  (void)self;
  PyObject *pluginSelf, *callback, *refcon;
  float inInterval;
  if(!PyArg_ParseTuple(args, "OOfO", &pluginSelf, &callback, &inInterval, &refcon)){
    printf("Params Problem!!!\n");
    return NULL;
  }
  void *inRefcon = (void *)++flCntr;
  PyObject *id = PyLong_FromVoidPtr(inRefcon);
  //I don't like this at all...
  PyObject *refconAddr = PyLong_FromVoidPtr(refcon);
  PyDict_SetItem(flDict, id, args);
  //we need to uniquely identify the id of the callback based on the caller and inRefcon
  PyObject *revId = Py_BuildValue("(OOO)", pluginSelf, callback, refconAddr);
  PyDict_SetItem(flRevDict, revId, id);
  Py_XDECREF(revId);
  Py_XDECREF(id);
  XPLMRegisterFlightLoopCallback(flightLoopCallback, inInterval, inRefcon);
  Py_RETURN_NONE;
}

static PyObject *XPLMUnregisterFlightLoopCallbackFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *pluginSelf, *callback, *refcon;
  if(!PyArg_ParseTuple(args, "OOO", &pluginSelf, &callback, &refcon)){
    return NULL;
  }
  PyObject *refconAddr = PyLong_FromVoidPtr(refcon);
  PyObject *revId = Py_BuildValue("(OOO)", pluginSelf, callback, refconAddr);
  PyObject *id = PyDict_GetItem(flRevDict, revId);
  if(id == NULL){
    Py_XDECREF(id);
    Py_XDECREF(revId);
    printf("Couldn't find the id of the requested callback.\n");
    return NULL;
  }
  PyDict_DelItem(flRevDict, revId);
  PyDict_DelItem(flDict, id);
  XPLMUnregisterFlightLoopCallback(flightLoopCallback, PyLong_AsVoidPtr(id));
  Py_XDECREF(id);
  Py_XDECREF(revId);
  Py_RETURN_NONE;
}

static PyObject *XPLMSetFlightLoopCallbackIntervalFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *pluginSelf, *callback, *refcon;
  float inInterval;
  int inRelativeToNow;
  if(!PyArg_ParseTuple(args, "OOfiO", &pluginSelf, &callback, &inInterval, &inRelativeToNow, &refcon)){
    return NULL;
  }
  PyObject *refconAddr = PyLong_FromVoidPtr(refcon);
  PyObject *revId = Py_BuildValue("(OOO)", pluginSelf, callback, refconAddr);
  PyObject *id = PyDict_GetItem(flRevDict, revId);
  if(id == NULL){
    printf("Couldn't find the id of the requested callback.\n");
    return NULL;
  }
  XPLMSetFlightLoopCallbackInterval(flightLoopCallback, inInterval, inRelativeToNow, PyLong_AsVoidPtr(id));
  Py_XDECREF(id);
  Py_XDECREF(revId);
  Py_RETURN_NONE;
}

static PyObject *XPLMCreateFlightLoopFun(PyObject* self, PyObject *args)
{
  (void)self;
  PyObject *pluginSelf, *params;
  if(!XPLMCreateFlightLoop_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMCreateFlightLoop is available only in XPLM210 and up.\n");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &params)){
    return NULL;
  }

  XPLMCreateFlightLoop_t fl;
  fl.structSize = sizeof(fl);
  PyObject *tmp = PyNumber_Long(PySequence_GetItem(params, 0));
  fl.phase = PyLong_AsLong(tmp);
  Py_DECREF(tmp);
  fl.callbackFunc = flightLoopCallback;
  fl.refcon = (void *)++flCntr;
  
  XPLMFlightLoopID res = XPLMCreateFlightLoop_ptr(&fl);

  PyObject *id = PyLong_FromVoidPtr(fl.refcon);
  PyObject *argObj = Py_BuildValue("(OOfO)", pluginSelf, PySequence_GetItem(params, 1),
                                             -1.0, PySequence_GetItem(params, 2));
  PyDict_SetItem(flDict, id, argObj);
  Py_XDECREF(argObj);
  //we need to uniquely identify the id of the callback based on the caller and inRefcon
  PyObject *resObj = PyLong_FromVoidPtr(res);
  PyDict_SetItem(flRevDict, resObj, id);
  Py_XDECREF(id);
  return resObj;
}

static PyObject *XPLMDestroyFlightLoopFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *revId, *pluginSelf;
  if(!XPLMDestroyFlightLoop_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDestroyFlightLoop is available only in XPLM210 and up.\n");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &revId)){
    return NULL;
  }
  PyObject *id = PyDict_GetItem(flRevDict, revId);
  if(id == NULL){
    Py_XDECREF(id);
    printf("Couldn't find the id of the requested flight loop.\n");
    return NULL;
  }
  PyDict_DelItem(flRevDict, revId);
  PyDict_DelItem(flDict, id);
  XPLMDestroyFlightLoop_ptr(PyLong_AsVoidPtr(revId));
  Py_XDECREF(id);
  Py_RETURN_NONE;
}

static PyObject *XPLMScheduleFlightLoopFun(PyObject *self, PyObject*args)
{
  (void)self;
  PyObject *pluginSelf, *flightLoopID;
  float inInterval;
  int inRelativeToNow;
  if(!XPLMScheduleFlightLoop_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMScheduleFlightLoop is available only in XPLM210 and up.\n");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OOfi", &pluginSelf, &flightLoopID, &inInterval, &inRelativeToNow)){
    return NULL;
  }
  XPLMFlightLoopID inFlightLoopID = PyLong_AsVoidPtr(flightLoopID);
  XPLMScheduleFlightLoop_ptr(inFlightLoopID, inInterval, inRelativeToNow);
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(flDict);
  Py_DECREF(flDict);
  PyDict_Clear(flRevDict);
  Py_DECREF(flRevDict);
  PyDict_Clear(flIDDict);
  Py_DECREF(flIDDict);
  Py_RETURN_NONE;
}


static PyMethodDef XPLMProcessingMethods[] = {
  {"XPLMGetElapsedTime", XPLMGetElapsedTimeFun, METH_VARARGS, ""},
  {"XPLMGetElapsedTime", XPLMGetElapsedTimeFun, METH_VARARGS, ""},
  {"XPLMGetCycleNumber", XPLMGetCycleNumberFun, METH_VARARGS, ""},
  {"XPLMRegisterFlightLoopCallback", XPLMRegisterFlightLoopCallbackFun, METH_VARARGS, ""},
  {"XPLMUnregisterFlightLoopCallback", XPLMUnregisterFlightLoopCallbackFun, METH_VARARGS, ""},
  {"XPLMSetFlightLoopCallbackInterval", XPLMSetFlightLoopCallbackIntervalFun, METH_VARARGS, ""},
  {"XPLMCreateFlightLoop", XPLMCreateFlightLoopFun, METH_VARARGS, ""},
  {"XPLMDestroyFlightLoop", XPLMDestroyFlightLoopFun, METH_VARARGS, ""},
  {"XPLMScheduleFlightLoop", XPLMScheduleFlightLoopFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMProcessingModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMProcessing",
  NULL,
  -1,
  XPLMProcessingMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMProcessing(void)
{
  if(!(flDict = PyDict_New())){
    return NULL;
  }
  if(!(flRevDict = PyDict_New())){
    return NULL;
  }
  if(!(flIDDict = PyDict_New())){
    return NULL;
  }
  PyObject *mod = PyModule_Create(&XPLMProcessingModule);
  if(mod){
    PyModule_AddIntConstant(mod, "xplm_FlightLoop_Phase_BeforeFlightModel", xplm_FlightLoop_Phase_BeforeFlightModel);
    PyModule_AddIntConstant(mod, "xplm_FlightLoop_Phase_AfterFlightModel", xplm_FlightLoop_Phase_AfterFlightModel);
  }

  return mod;
}



