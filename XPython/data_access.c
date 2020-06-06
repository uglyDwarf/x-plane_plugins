#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMUtilities.h>
#include "utils.h"

//static PyObject *rwCallbackDict;
//static intptr_t rwCallbackCntr;
static PyObject *accessorDict;
static PyObject *drefDict;
static intptr_t accessorCntr;
static PyObject *sharedDict;
static intptr_t sharedCntr;

static const char dataRefName[] = "datarefRef";

static PyObject *XPLMFindDataRefFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inDataRefName;
  if(!PyArg_ParseTuple(args, "s", &inDataRefName)){
    return NULL;
  }
  XPLMDataRef ref = XPLMFindDataRef(inDataRefName);
  if(ref){
    return getPtrRefOneshot(ref, dataRefName);
  }else{
    Py_RETURN_NONE;
  }
}

static PyObject *XPLMCanWriteDataRefFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *dataRef;
  if(!PyArg_ParseTuple(args, "O", &dataRef)){
    return NULL;
  }
  XPLMDataRef inDataRef = refToPtr(dataRef, dataRefName);
  if(XPLMCanWriteDataRef(inDataRef)){
    Py_RETURN_TRUE;
  }else{
    Py_RETURN_FALSE;
  }
}

static PyObject *XPLMIsDataRefGoodFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *dataRef;
  if(!PyArg_ParseTuple(args, "O", &dataRef)){
    return NULL;
  }
  XPLMDataRef inDataRef = refToPtr(dataRef, dataRefName);
  if (XPLMIsDataRefGood(inDataRef)){
    Py_RETURN_TRUE;
  } else {
    Py_RETURN_FALSE;
  }
}

static PyObject *XPLMGetDataRefTypesFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *dataRef;
  if(!PyArg_ParseTuple(args, "O", &dataRef)){
    return NULL;
  }
  XPLMDataRef inDataRef = refToPtr(dataRef, dataRefName);
  return PyLong_FromLong(XPLMGetDataRefTypes(inDataRef));
}

static PyObject *XPLMGetDataiFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *dataRef;
  if(!PyArg_ParseTuple(args, "O", &dataRef)){
    return NULL;
  }
  XPLMDataRef inDataRef = refToPtr(dataRef, dataRefName);
  return PyLong_FromLong(XPLMGetDatai(inDataRef));
}

static PyObject *XPLMSetDataiFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *dataRef;
  int inValue;
  if(!PyArg_ParseTuple(args, "Oi", &dataRef, &inValue)){
    return NULL;
  }
  XPLMDataRef inDataRef = refToPtr(dataRef, dataRefName);
  XPLMSetDatai(inDataRef, inValue);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetDatafFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *dataRef;
  if(!PyArg_ParseTuple(args, "O", &dataRef)){
    return NULL;
  }
  XPLMDataRef inDataRef = refToPtr(dataRef, dataRefName);
  return PyFloat_FromDouble(XPLMGetDataf(inDataRef));
}

static PyObject *XPLMSetDatafFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *dataRef;
  float inValue;
  if(!PyArg_ParseTuple(args, "Of", &dataRef, &inValue)){
    return NULL;
  }
  XPLMDataRef inDataRef = refToPtr(dataRef, dataRefName);
  XPLMSetDataf(inDataRef, inValue);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetDatadFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *dataRef;
  if(!PyArg_ParseTuple(args, "O", &dataRef)){
    return NULL;
  }
  XPLMDataRef inDataRef = refToPtr(dataRef, dataRefName);
  return PyFloat_FromDouble(XPLMGetDatad(inDataRef));
}

static PyObject *XPLMSetDatadFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *dataRef;
  double inValue;
  if(!PyArg_ParseTuple(args, "Od", &dataRef, &inValue)){
    return NULL;
  }
  XPLMDataRef inDataRef = refToPtr(dataRef, dataRefName);
  XPLMSetDatad(inDataRef, inValue);
  Py_RETURN_NONE;
}

static inline XPLMDataRef drefFromObj(PyObject *obj)
{
  return (XPLMDataRef)refToPtr(obj, dataRefName);
}

static PyObject *XPLMGetDataviFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *drefObj;
  PyObject *outValuesObj;
  int *outValues = NULL;
  int inOffset, inMax;
  if(!PyArg_ParseTuple(args, "OOii", &drefObj, &outValuesObj, &inOffset, &inMax)){
    return NULL;
  }
  XPLMDataRef inDataRef= drefFromObj(drefObj);
  if(outValuesObj && (outValuesObj != Py_None)){
    if(!PyList_Check(outValuesObj)){
      PyErr_SetString(PyExc_TypeError, "XPLMGetDatavi expects list or None as the outValues parameter.");
      return NULL;
    }
    if(inMax > 0){
      outValues = (int *)malloc(inMax * sizeof(int));
    }else{
      PyErr_SetString(PyExc_RuntimeError, "XPLMGetDatavi inMax value must be positive.");
      return NULL;
    }
  }
  
  int res = XPLMGetDatavi(inDataRef, outValues, inOffset, inMax);
  if(outValues != NULL){
    if(PyList_Size(outValuesObj) > 0){
      PySequence_DelSlice(outValuesObj, 0, PyList_Size(outValuesObj));
    }
    //If res < inMax, copy only res elements;
    //  if inMax < res, copy only inMax elements
    for(int i = 0; i < (res < inMax ? res : inMax); ++i){
      PyObject *tmp = PyLong_FromLong(outValues[i]);
      PyList_Append(outValuesObj, tmp);
      Py_DECREF(tmp);
    }
    free(outValues);
  }
  return PyLong_FromLong(res);
}

static PyObject *XPLMSetDataviFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *drefObj;
  PyObject *inValuesObj;
  int *inValues = NULL;
  int inOffset, inCount;
  if(!PyArg_ParseTuple(args, "OOii", &drefObj, &inValuesObj, &inOffset, &inCount)){
    return NULL;
  }
  XPLMDataRef inDataRef= drefFromObj(drefObj);
  if(!PySequence_Check(inValuesObj)){
    PyErr_SetString(PyExc_TypeError, "XPLMSetDatavi expects list as the inValues parameter.");
    return NULL;
  }
  if(PySequence_Length(inValuesObj) < inCount){
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetDatavi list too short.");
    return NULL;
  }
  inValues = (int *)malloc(inCount * sizeof(int));
  PyObject *tup = PySequence_Tuple(inValuesObj);
  for(int i = 0; i < inCount; ++i){
    inValues[i] = PyLong_AsLong(PyTuple_GetItem(tup, i));
  }
  Py_DECREF(tup);
  
  XPLMSetDatavi(inDataRef, inValues, inOffset, inCount);
  free(inValues);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetDatavfFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *drefObj;
  PyObject *outValuesObj;
  float *outValues = NULL;
  int inOffset, inMax;
  if(!PyArg_ParseTuple(args, "OOii", &drefObj, &outValuesObj, &inOffset, &inMax)){
    return NULL;
  }
  XPLMDataRef inDataRef = drefFromObj(drefObj);
  if(outValuesObj && (outValuesObj != Py_None)){
    if(!PyList_Check(outValuesObj)){
      PyErr_SetString(PyExc_TypeError, "XPLMGetDatavf expects list or None as the outValues parameter.");
      return NULL;
    }
    if(inMax > 0){
      outValues = (float *)malloc(inMax * sizeof(float));
    }else{
      PyErr_SetString(PyExc_RuntimeError, "XPLMGetDatavf inMax value must be positive.");
      return NULL;
    }
  }
  
  int res = XPLMGetDatavf(inDataRef, outValues, inOffset, inMax);
  
  if(outValues != NULL){
    if(PyList_Size(outValuesObj) > 0){
      PySequence_DelSlice(outValuesObj, 0, PyList_Size(outValuesObj));
    }
    //If res < inMax, copy only res elements;
    //  if inMax < res, copy only inMax elements
    for(int i = 0; i < (res < inMax ? res : inMax); ++i){
      PyObject *tmp = PyFloat_FromDouble(outValues[i]);
      PyList_Append(outValuesObj, tmp);
      Py_DECREF(tmp);
    }
  }
  free(outValues);
  return PyLong_FromLong(res);
}

static PyObject *XPLMSetDatavfFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *drefObj;
  PyObject *inValuesObj;
  float *inValues = NULL;
  int inOffset, inCount;
  if(!PyArg_ParseTuple(args, "OOii", &drefObj, &inValuesObj, &inOffset, &inCount)){
    return NULL;
  }
  XPLMDataRef inDataRef = drefFromObj(drefObj);
  if(!PySequence_Check(inValuesObj)){
    PyErr_SetString(PyExc_TypeError, "XPLMSetDatavf expects list as the inValues parameter.");
    return NULL;
  }
  if(PySequence_Length(inValuesObj) < inCount){
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetDatavf list too short.");
    return NULL;
  }
  inValues = (float *)malloc(inCount * sizeof(float));
  PyObject *tup = PySequence_Tuple(inValuesObj);
  for(int i = 0; i < inCount; ++i){
    inValues[i] = PyFloat_AsDouble(PyTuple_GetItem(tup, i));
  }
  
  XPLMSetDatavf(inDataRef, inValues, inOffset, inCount);
  free(inValues);
  Py_RETURN_NONE;
}


static PyObject *XPLMGetDatabFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *drefObj;
  PyObject *outValuesObj;
  uint8_t *outValues = NULL;
  int inOffset, inMax;
  if(!PyArg_ParseTuple(args, "OOii", &drefObj, &outValuesObj, &inOffset, &inMax)){
    return NULL;
  }
  XPLMDataRef inDataRef= drefFromObj(drefObj);
  if(outValuesObj && (outValuesObj != Py_None)){
    if(!PyList_Check(outValuesObj)){
      PyErr_SetString(PyExc_TypeError, "XPLMGetDatab expects list or None as the outValues parameter.");
      return NULL;
    }
    if(inMax > 0){
      outValues = (uint8_t *)malloc(inMax * sizeof(uint8_t));
    }else{
      PyErr_SetString(PyExc_RuntimeError, "XPLMGetDatab inMax value must be positive.");
      return NULL;
    }
  }
  
  int res = XPLMGetDatab(inDataRef, outValues, inOffset, inMax);
  if(outValues != NULL){
    if(PyList_Size(outValuesObj) > 0){
      PySequence_DelSlice(outValuesObj, 0, PyList_Size(outValuesObj));
    }
    //If res < inMax, copy only res elements;
    //  if inMax < res, copy only inMax elements
    for(int i = 0; i < (res < inMax ? res : inMax); ++i){
      PyObject *tmp = PyLong_FromLong(outValues[i]);
      PyList_Append(outValuesObj, tmp);
    }
    free(outValues);
  }
  return PyLong_FromLong(res);
}

static PyObject *XPLMSetDatabFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *drefObj;
  PyObject *inValuesObj;
  uint8_t *inValues = NULL;
  int inOffset, inCount;
  if(!PyArg_ParseTuple(args, "OOii", &drefObj, &inValuesObj, &inOffset, &inCount)){
    return NULL;
  }
  XPLMDataRef inDataRef= drefFromObj(drefObj);
  if(!PySequence_Check(inValuesObj)){
    PyErr_SetString(PyExc_TypeError, "XPLMSetDatab expects list as the inValues parameter.");
    return NULL;
  }
  if(PySequence_Length(inValuesObj) < inCount){
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetDatab list too short.");
    return NULL;
  }
  inValues = (uint8_t *)malloc(inCount * sizeof(uint8_t));
  PyObject *tup = PySequence_Tuple(inValuesObj);
  for(int i = 0; i < inCount; ++i){
    inValues[i] = PyLong_AsLong(PyTuple_GetItem(tup, i));
  }
  Py_DECREF(tup);
  
  XPLMSetDatab(inDataRef, inValues, inOffset, inCount);
  free(inValues);
  Py_RETURN_NONE;
}

//TODO: check all PyObjectCallFunction calls to see if they relese the return value!

static int getDatai(void *inRefcon)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbks = PyDict_GetItem(accessorDict, pID);
  if(pCbks == NULL){
    printf("Unknown dataAccessor refCon passed to getDatai (%p).\n", inRefcon);
    return -1;
  }
  PyObject *oFun = PySequence_GetItem(pCbks, 4);
  PyObject *oArg = PySequence_GetItem(pCbks, 16);
  PyObject *oRes = PyObject_CallFunctionObjArgs(oFun, oArg, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oFun);
  Py_DECREF(oArg);
  Py_DECREF(pID);
  int res = PyLong_AsLong(oRes);
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oRes);
  return res;
}

static void setDatai(void *inRefcon, int inValue)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbks = PyDict_GetItem(accessorDict, pID);
  if(pCbks == NULL){
    printf("Unknown dataAccessor refCon passed to setDatai (%p).\n", inRefcon);
    return;
  }
  PyObject *oFun = PySequence_GetItem(pCbks, 5);
  PyObject *oArg1 = PySequence_GetItem(pCbks, 17);
  PyObject *oArg2 = PyLong_FromLong(inValue);
  PyObject *oRes = PyObject_CallFunctionObjArgs(oFun, oArg1, oArg2, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oFun);
  Py_DECREF(oArg1);
  Py_DECREF(oArg2);
  Py_DECREF(pID);
  Py_DECREF(oRes);
  return;
}

static float getDataf(void *inRefcon)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbks = PyDict_GetItem(accessorDict, pID);
  if(pCbks == NULL){
    printf("Unknown dataAccessor refCon passed to getDataf (%p).\n", inRefcon);
    return -1;
  }
  PyObject *oFun = PySequence_GetItem(pCbks, 6);
  PyObject *oArg1 = PySequence_GetItem(pCbks, 16);
  PyObject *oRes = PyObject_CallFunctionObjArgs(oFun, oArg1, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oFun);
  Py_DECREF(oArg1);
  Py_DECREF(pID);

  float res = PyFloat_AsDouble(oRes);
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oRes);
  return res;
}

static void setDataf(void *inRefcon, float inValue)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbks = PyDict_GetItem(accessorDict, pID);
  if(pCbks == NULL){
    printf("Unknown dataAccessor refCon passed to setDataf (%p).\n", inRefcon);
    return;
  }
  PyObject *oFun = PySequence_GetItem(pCbks, 7);
  PyObject *oArg1 = PySequence_GetItem(pCbks, 17);
  PyObject *oArg2 = PyFloat_FromDouble((double)inValue);
  PyObject *oRes = PyObject_CallFunctionObjArgs(oFun, oArg1, oArg2, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oFun);
  Py_DECREF(oArg1);
  Py_DECREF(oArg2);
  Py_DECREF(pID);
  Py_DECREF(oRes);
  return;
}

static double getDatad(void *inRefcon)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbks = PyDict_GetItem(accessorDict, pID);
  if(pCbks == NULL){
    printf("Unknown dataAccessor refCon passed to getDatad (%p).\n", inRefcon);
    return -1;
  }
  PyObject *oFun = PySequence_GetItem(pCbks, 8);
  PyObject *oArg = PySequence_GetItem(pCbks, 16);
  PyObject *oRes = PyObject_CallFunctionObjArgs(oFun, oArg, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oFun);
  Py_DECREF(oArg);
  Py_DECREF(pID);
  double res = PyFloat_AsDouble(oRes);
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oRes);
  return res;
}

static void setDatad(void *inRefcon, double inValue)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbks = PyDict_GetItem(accessorDict, pID);
  if(pCbks == NULL){
    printf("Unknown dataAccessor refCon passed to setDatad (%p).\n", inRefcon);
    return;
  }
  PyObject *oFun = PySequence_GetItem(pCbks, 9);
  PyObject *oArg1 = PySequence_GetItem(pCbks, 17);
  PyObject *oArg2 = PyFloat_FromDouble(inValue);
  PyObject *oRes = PyObject_CallFunctionObjArgs(oFun, oArg1, oArg2, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oFun);
  Py_DECREF(oArg1);
  Py_DECREF(oArg2);
  Py_DECREF(pID);
  Py_DECREF(oRes);
  return;
}

static int getDatavi(void *inRefcon, int *outValues, int inOffset, int inMax)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbks = PyDict_GetItem(accessorDict, pID);
  if(pCbks == NULL){
    printf("Unknown dataAccessor refCon passed to getDatavi (%p).\n", inRefcon);
    return -1;
  }
  PyObject *outValuesObj;
  if(outValues != NULL){
    outValuesObj = PyList_New(inMax);
  }else{
    outValuesObj = Py_None;
    Py_INCREF(outValuesObj);
  }

  PyObject *oFun = PySequence_GetItem(pCbks, 10);
  PyObject *oArg1 = PySequence_GetItem(pCbks, 16);
  PyObject *oArg2 = PyLong_FromLong(inOffset);
  PyObject *oArg3 = PyLong_FromLong(inMax);
  PyObject *oRes = PyObject_CallFunctionObjArgs(oFun, oArg1, outValuesObj, oArg2, oArg3, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
  Py_DECREF(oFun);
  Py_DECREF(oArg1);
  Py_DECREF(oArg2);
  Py_DECREF(oArg3);
  int res = PyLong_AsLong(oRes);
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
    res = 0;
  }else{
    if(outValues){
      for(int i = 0; i < res; ++i){
        outValues[i] = PyLong_AsLong(PyList_GetItem(outValuesObj, i));
      }
    }
  }
  Py_DECREF(outValuesObj);
  Py_XDECREF(oRes);
  return res;
}

static void setDatavi(void *inRefcon, int *inValues, int inOffset, int inCount)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbks = PyDict_GetItem(accessorDict, pID);
  if(pCbks == NULL){
    printf("Unknown dataAccessor refCon passed to setDatavi (%p).\n", inRefcon);
    return;
  }
  PyObject *inValuesObj = PyList_New(0);
  for(int i = 0; i < inCount; ++i){
    PyObject *tmp = PyLong_FromLong(inValues[i]);
    PyList_Append(inValuesObj, tmp);
    Py_DECREF(tmp);
  }

  PyObject *oFun = PySequence_GetItem(pCbks, 11);
  PyObject *oArg1 = PySequence_GetItem(pCbks, 17);
  PyObject *oArg2 = PyLong_FromLong(inOffset);
  PyObject *oArg3 = PyLong_FromLong(inCount);
  PyObject *oRes = PyObject_CallFunctionObjArgs(oFun, oArg1, inValuesObj, oArg2, oArg3, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oFun);
  Py_DECREF(oArg1);
  Py_DECREF(oArg2);
  Py_DECREF(oArg3);
  Py_DECREF(pID);
  Py_DECREF(inValuesObj);
  Py_DECREF(oRes);
  return;
}

static int getDatavf(void *inRefcon, float *outValues, int inOffset, int inMax)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbks = PyDict_GetItem(accessorDict, pID);
  if(pCbks == NULL){
    printf("Unknown dataAccessor refCon passed to getDatavf (%p).\n", inRefcon);
    return -1;
  }
  PyObject *outValuesObj;
  if(outValues != NULL){
    outValuesObj = PyList_New(inMax);
  }else{
    outValuesObj = Py_None;
    Py_INCREF(outValuesObj);
  }

  PyObject *oFun = PySequence_GetItem(pCbks, 12);
  PyObject *oArg1 = PySequence_GetItem(pCbks, 16);
  PyObject *oArg2 = PyLong_FromLong(inOffset);
  PyObject *oArg3 = PyLong_FromLong(inMax);
  PyObject *oRes = PyObject_CallFunctionObjArgs(oFun, oArg1, outValuesObj, oArg2, oArg3, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
    Py_DECREF(oFun);
  Py_DECREF(oArg1);
  Py_DECREF(oArg2);
  Py_DECREF(oArg3);
  int res = PyLong_AsLong(oRes);
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
    res = 0;
  }else{
    if(outValues){
      for(int i = 0; i < res; ++i){
        outValues[i] = PyFloat_AsDouble(PyList_GetItem(outValuesObj, i));
      }
    }
  }
  Py_DECREF(outValuesObj);
  Py_XDECREF(oRes);
  return res;
}

static void setDatavf(void *inRefcon, float *inValues, int inOffset, int inCount)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbks = PyDict_GetItem(accessorDict, pID);
  if(pCbks == NULL){
    printf("Unknown dataAccessor refCon passed to setDatavf (%p).\n", inRefcon);
    return;
  }
  PyObject *inValuesObj = PyList_New(0);
  for(int i = 0; i < inCount; ++i){
    PyObject *tmp = PyFloat_FromDouble(inValues[i]);
    PyList_Append(inValuesObj, tmp);
    Py_DECREF(tmp);
  }

  PyObject *oFun = PySequence_GetItem(pCbks, 13);
  PyObject *oArg1 = PySequence_GetItem(pCbks, 17);
  PyObject *oArg2 = PyLong_FromLong(inOffset);
  PyObject *oArg3 = PyLong_FromLong(inCount);
  PyObject *oRes = PyObject_CallFunctionObjArgs(oFun, oArg1, inValuesObj, oArg2, oArg3, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oFun);
  Py_DECREF(oArg1);
  Py_DECREF(oArg2);
  Py_DECREF(oArg3);
  Py_DECREF(pID);
  Py_DECREF(inValuesObj);
  Py_DECREF(oRes);
  return;
}

static int getDatab(void *inRefcon, void *outValue, int inOffset, int inMax)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbks = PyDict_GetItem(accessorDict, pID);
  if(pCbks == NULL){
    printf("Unknown dataAccessor refCon passed to getDatab (%p).\n", inRefcon);
    return -1;
  }
  PyObject *outValuesObj;
  if(outValue != NULL){
    outValuesObj = PyList_New(inMax);
  }else{
    outValuesObj = Py_None;
    Py_INCREF(outValuesObj);
  }

  PyObject *oFun = PySequence_GetItem(pCbks, 14);
  PyObject *oArg1 = PySequence_GetItem(pCbks, 16);
  PyObject *oArg2 = PyLong_FromLong(inOffset);
  PyObject *oArg3 = PyLong_FromLong(inMax);
  PyObject *oRes = PyObject_CallFunctionObjArgs(oFun, oArg1, outValuesObj, oArg2, oArg3, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
  Py_DECREF(oFun);
  Py_DECREF(oArg1);
  Py_DECREF(oArg2);
  Py_DECREF(oArg3);
  int res = PyLong_AsLong(oRes);
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
    res = 0;
  }else{
    if(outValue){
      uint8_t *pOutValue = (uint8_t *)outValue;
      for(int i = 0; i < res; ++i){
        pOutValue[i] = PyLong_AsLong(PyList_GetItem(outValuesObj, i));
      }
    }
  }
  Py_DECREF(outValuesObj);
  Py_DECREF(oRes);
  return res;
}

static void setDatab(void *inRefcon, void *inValue, int inOffset, int inCount)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbks = PyDict_GetItem(accessorDict, pID);
  if(pCbks == NULL){
    printf("Unknown dataAccessor refCon passed to setDatab (%p).\n", inRefcon);
    return;
  }
  PyObject *inValuesObj = PyList_New(0);
  uint8_t *pInValue = (uint8_t *)inValue;
  for(int i = 0; i < inCount; ++i){
    PyObject *tmp = PyLong_FromLong(pInValue[i]);
    PyList_Append(inValuesObj, tmp);
    Py_DECREF(tmp);
  }

  PyObject *oFun = PySequence_GetItem(pCbks, 15);
  PyObject *oArg1 = PySequence_GetItem(pCbks, 17);
  PyObject *oArg2 = PyLong_FromLong(inOffset);
  PyObject *oArg3 = PyLong_FromLong(inCount);
  PyObject *oRes = PyObject_CallFunctionObjArgs(oFun, oArg1, inValuesObj, oArg2, oArg3, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oFun);
  Py_DECREF(oArg1);
  Py_DECREF(oArg2);
  Py_DECREF(oArg3);
  Py_DECREF(pID);
  Py_DECREF(inValuesObj);
  Py_DECREF(oRes);
  return;
}


static PyObject *XPLMRegisterDataAccessorFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *pluginSelf;
  const char *inDataName;
  int inDataType, inIsWritable;
  PyObject *ri, *wi, *rf, *wf, *rd, *wd, *rai, *wai, *raf, *waf, *rab, *wab, *rRef, *wRef;
  if(!PyArg_ParseTuple(args, "OsiiOOOOOOOOOOOOOO", &pluginSelf, &inDataName, &inDataType, &inIsWritable,
                       &ri, &wi, &rf, &wf, &rd, &wd, &rai, &wai, &raf, &waf, &rab, &wab, &rRef, &wRef)){
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "siiOOOOOOOOOOOOOO", &inDataName, &inDataType, &inIsWritable,
                         &ri, &wi, &rf, &wf, &rd, &wd, &rai, &wai, &raf, &waf, &rab, &wab, &rRef, &wRef))
      return NULL;
    pluginSelf = get_pluginSelf(/*PyThreadState_GET()*/);
  }

  void *refcon = (void *)accessorCntr++;
  PyObject *refconObj = PyLong_FromVoidPtr(refcon);

  XPLMDataRef res = XPLMRegisterDataAccessor(
                                          inDataName,
                                          inDataType,
                                          inIsWritable,
                                          getDatai,   setDatai,
                                          getDataf,   setDataf,
                                          getDatad,   setDatad,
                                          getDatavi,  setDatavi,
                                          getDatavf,  setDatavf,
                                          getDatab,   setDatab,
                                          refcon,     refcon);

  PyObject *argsObj = Py_BuildValue("(OsiiOOOOOOOOOOOOOO)", pluginSelf, inDataName, inDataType, inIsWritable,
                                    ri, wi, rf, wf, rd, wd, rai, wai, raf, waf, rab, wab, rRef, wRef);
  if(PyDict_SetItem(accessorDict, refconObj, argsObj) != 0){
    Py_RETURN_NONE;
  }
  Py_DECREF(argsObj);
  PyObject *resObj = getPtrRefOneshot(res, dataRefName);
  PyDict_SetItem(drefDict, resObj, refconObj);
  Py_DECREF(refconObj);
  return resObj;
}

static PyObject *XPLMUnregisterDataAccessorFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *pluginSelf;
  PyObject *drefObj;
  if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &drefObj)){
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "O", &drefObj))
      return NULL;
    pluginSelf = get_pluginSelf(/*PyThreadState_GET()*/);
  } else {
    Py_INCREF(pluginSelf);
  }
  PyObject *refconObj = PyDict_GetItem(drefDict, drefObj);
  if(refconObj == NULL){
    Py_DECREF(pluginSelf);
    printf("XPLMUnregisterDataref: No such dataref registered!\n");
    Py_RETURN_NONE;
  }
  PyObject *accessor = PyDict_GetItem(accessorDict, refconObj);
  if(accessor == NULL){
    Py_DECREF(pluginSelf);
    printf("XPLMUnregisterDataref: No such refcon registered!\n");
    Py_RETURN_NONE;
  }
  PyObject *registerer = PySequence_GetItem(accessor, 0);
  if(PyObject_RichCompareBool(pluginSelf, registerer, Py_NE)) {
    Py_DECREF(registerer);
    Py_DECREF(pluginSelf);
    printf("XPLMUnregisterDataref: Don't unregister dataref you didn't register!!\n");
    Py_RETURN_NONE;
  }
  Py_DECREF(pluginSelf);
  Py_DECREF(registerer);
  if(PyDict_DelItem(accessorDict, refconObj)){
    printf("XPLMUnregisterDataref: Couldn't remove the refcon.\n");
  }
  if(PyDict_DelItem(drefDict, drefObj)){
    printf("XPLMUnregisterDataref: Couldn't remove the dref.\n");
  }
  XPLMUnregisterDataAccessor(refToPtr(drefObj, dataRefName));
  Py_RETURN_NONE;
}

static void dataChanged(void *inRefcon)
{
  PyObject *refconObj = PyLong_FromVoidPtr(inRefcon);
  PyObject *sharedObj = PyDict_GetItem(sharedDict, refconObj);
  Py_DECREF(refconObj);
  if(sharedObj == NULL){
    printf("Shared data callback called with wrong inRefcon: %p\n", inRefcon);
    return;
  }
  PyObject *callbackFun = PySequence_GetItem(sharedObj, 3);
  PyObject *arg = PySequence_GetItem(sharedObj, 4);
  PyObject *oRes = PyObject_CallFunctionObjArgs(callbackFun, arg, NULL);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_XDECREF(oRes);
  Py_DECREF(arg);
  Py_DECREF(callbackFun);
}

static PyObject *XPLMShareDataFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf;
  const char *inDataName;
  XPLMDataTypeID inDataType;
  PyObject *inNotificationFunc, *inNotificationRefcon;
  if(!PyArg_ParseTuple(args, "OsiOO", &pluginSelf, &inDataName, &inDataType, &inNotificationFunc, &inNotificationRefcon)){
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "siOO", &inDataName, &inDataType, &inNotificationFunc, &inNotificationRefcon))
      return NULL;
    pluginSelf = get_pluginSelf(/*PyThreadState_GET()*/);
  }
  void *refcon = (void *)sharedCntr++;
  int res = XPLMShareData(inDataName, inDataType, dataChanged, refcon);
  if(res != 1){
    return PyLong_FromLong(res);
  }
  PyObject *refconObj =  PyLong_FromVoidPtr(refcon);
  if(PyErr_Occurred()) {
    PyErr_Print();
    return NULL;
  }
  PyObject *argsObj = Py_BuildValue("(OsiOO)", pluginSelf, inDataName, inDataType, inNotificationFunc, inNotificationRefcon);
  if (!argsObj || PyErr_Occurred()) {
    PyErr_Print();
    return NULL;
  }
  PyDict_SetItem(sharedDict,refconObj, argsObj);
  Py_DECREF(argsObj);
  Py_DECREF(refconObj);
  return PyLong_FromLong(res);
}

static PyObject *XPLMUnshareDataFun(PyObject *self, PyObject *args)
{
  (void) self;
  Py_ssize_t cnt = 0;
  PyObject *pKey = NULL, *pVal = NULL;
  const char *inDataName = NULL;
  XPLMDataTypeID inDataType;
  long tmpInDataType;
  PyObject *callbackObj = NULL;
  PyObject *refconObj = NULL;
  PyObject *pluginSelf = NULL;
  if(!PyArg_ParseTuple(args, "OsiOO", &pluginSelf, &inDataName, &tmpInDataType, &callbackObj, &refconObj)){
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "siOO", &inDataName, &tmpInDataType, &callbackObj, &refconObj))
      return NULL;
    pluginSelf = get_pluginSelf(/*PyThreadState_GET()*/);
  } else {
    Py_INCREF(pluginSelf);
  }
  inDataType = (XPLMDataTypeID)tmpInDataType;
  PyObject *inDataNameObj, *target, *inDataNameUTF8Obj;
  char *dict_inDataName;
  target = NULL;
  while(PyDict_Next(sharedDict, &cnt, &pKey, &pVal)){
    // only look for things this plugin is sharing...
    if (PyObject_RichCompareBool(pluginSelf, PyTuple_GetItem(pVal, 0), Py_NE)){
      continue;
    }

    // Look for inDataName match
    inDataNameObj = PyTuple_GetItem(pVal, 1);
    inDataNameUTF8Obj = PyUnicode_AsUTF8String(inDataNameObj);
    dict_inDataName = PyBytes_AsString(inDataNameUTF8Obj);
    if (PyErr_Occurred()) {
      Py_DECREF(inDataNameUTF8Obj);
      Py_DECREF(pluginSelf);
      return NULL;
    }
    if(strcmp(inDataName, dict_inDataName) != 0){
      /* printf("inDataNames do not match '%s' - '%s'\n", inDataName, dict_inDataName);*/
      Py_DECREF(inDataNameUTF8Obj);
      continue;
    }
    Py_DECREF(inDataNameUTF8Obj);
    if(PyLong_AsLong(PyTuple_GetItem(pVal, 2)) != inDataType){
      /* printf("in data types do not match %d - %d\n", (int)PyLong_AsLong(PyTuple_GetItem(pVal, 2)), inDataType);*/
      continue;
    }
    if(PyTuple_GetItem(pVal, 3) != callbackObj){
      /* printf("callbckObject do not match %s - %s\n", objToStr(PyTuple_GetItem(pVal, 3)), objToStr(callbackObj)); */
      continue;
    }
    if(PyTuple_GetItem(pVal, 4) != refconObj){
      /* printf("refconObj do not match %s - %s\n", objToStr(PyTuple_GetItem(pVal, 4)), objToStr(refconObj));*/
      continue;
    }
    target = pKey;
    break;
  }
  Py_DECREF(pluginSelf);
  if(target){
    // Found and deleting unshared
    int res = XPLMUnshareData(inDataName, inDataType, dataChanged, PyLong_AsVoidPtr(target));
    PyDict_DelItem(sharedDict, target);
    return PyLong_FromLong(res);
  }else{
    // Couldn't find the right shared data...
    return PyLong_FromLong(0);
  }
}


static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(accessorDict);
  Py_DECREF(accessorDict);
  PyDict_Clear(drefDict);
  Py_DECREF(drefDict);
  PyDict_Clear(sharedDict);
  Py_DECREF(sharedDict);
  Py_RETURN_NONE;
}



static PyMethodDef XPLMDataAccessMethods[] = {
  {"XPLMFindDataRef", XPLMFindDataRefFun, METH_VARARGS, "Find a dataref"},
  {"XPLMCanWriteDataRef", XPLMCanWriteDataRefFun, METH_VARARGS, "Check dataref writeability"},
  {"XPLMIsDataRefGood", XPLMIsDataRefGoodFun, METH_VARARGS, ""},
  {"XPLMGetDataRefTypes", XPLMGetDataRefTypesFun, METH_VARARGS, "Check dataref type"},
  {"XPLMGetDatai", XPLMGetDataiFun, METH_VARARGS, ""},
  {"XPLMSetDatai", XPLMSetDataiFun, METH_VARARGS, ""},
  {"XPLMGetDataf", XPLMGetDatafFun, METH_VARARGS, ""},
  {"XPLMSetDataf", XPLMSetDatafFun, METH_VARARGS, ""},
  {"XPLMGetDatad", XPLMGetDatadFun, METH_VARARGS, ""},
  {"XPLMSetDatad", XPLMSetDatadFun, METH_VARARGS, ""},
  {"XPLMGetDatavi", XPLMGetDataviFun, METH_VARARGS, ""},
  {"XPLMSetDatavi", XPLMSetDataviFun, METH_VARARGS, ""},
  {"XPLMGetDatavf", XPLMGetDatavfFun, METH_VARARGS, ""},
  {"XPLMSetDatavf", XPLMSetDatavfFun, METH_VARARGS, ""},
  {"XPLMGetDatab", XPLMGetDatabFun, METH_VARARGS, ""},
  {"XPLMSetDatab", XPLMSetDatabFun, METH_VARARGS, ""},
  {"XPLMRegisterDataAccessor", XPLMRegisterDataAccessorFun, METH_VARARGS, ""},
  {"XPLMUnregisterDataAccessor", XPLMUnregisterDataAccessorFun, METH_VARARGS, ""},
  {"XPLMShareData", XPLMShareDataFun, METH_VARARGS, ""},
  {"XPLMUnshareData", XPLMUnshareDataFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMDataAccessModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMDataAccess",
  NULL,
  -1,
  XPLMDataAccessMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMDataAccess(void)
{
  PyObject *mod = PyModule_Create(&XPLMDataAccessModule);
  if(mod){

    if(!(accessorDict = PyDict_New())){
      return NULL;
    }
    if(!(drefDict = PyDict_New())){
      return NULL;
    }
    if(!(sharedDict = PyDict_New())){
      return NULL;
    }

    /* Data of a type the current XPLM doesn't do.                                 */
    PyModule_AddIntConstant(mod, "xplmType_Unknown", xplmType_Unknown);

    /* A single 4-byte integer, native endian.                                     */
    PyModule_AddIntConstant(mod, "xplmType_Int", xplmType_Int);

    /* A single 4-byte float, native endian.                                       */
    PyModule_AddIntConstant(mod, "xplmType_Float", xplmType_Float);

    /* A single 8-byte double, native endian.                                      */
    PyModule_AddIntConstant(mod, "xplmType_Double", xplmType_Double);

    /* An array of 4-byte floats, native endian.                                   */
    PyModule_AddIntConstant(mod, "xplmType_FloatArray", xplmType_FloatArray);

    /* An array of 4-byte integers, native endian.                                 */
    PyModule_AddIntConstant(mod, "xplmType_IntArray", xplmType_IntArray);

    /* A variable block of data.                                                   */
    PyModule_AddIntConstant(mod, "xplmType_Data", xplmType_Data);
  }

  return mod;
}


