#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include "utils.h"

//static PyObject *rwCallbackDict;
//static intptr_t rwCallbackCntr;
static PyObject *accessorDict;
static PyObject *drefDict;
static intptr_t accessorCntr;
static PyObject *sharedDict;
static intptr_t sharedCntr;

static PyObject *XPLMFindDataRefFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 's', "inDataRefName"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  char *inDataRefName = PyUnicode_AsUTF8(PySequence_GetItem(args, 0));

  return PyLong_FromVoidPtr(XPLMFindDataRef(inDataRefName));
}

static PyObject *XPLMCanWriteDataRefFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inDataRef"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  XPLMDataRef inDataRef = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  if(XPLMCanWriteDataRef(inDataRef)){
    Py_RETURN_TRUE;
  }else{
    Py_RETURN_FALSE;
  }
}

/* DEPRECATED
static PyObject *XPLMIsDataRefGoodFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inDataRef"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  XPLMDataRef inDataRef = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  if(XPLMIsDataRefGood(inDataRef)){
    Py_RETURN_TRUE;
  }else{
    Py_RETURN_FALSE;
  }
}
*/

static PyObject *XPLMGetDataRefTypesFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inDataRef"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  XPLMDataRef inDataRef = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  return PyLong_FromLong(XPLMGetDataRefTypes(inDataRef));
}

static PyObject *XPLMGetDataiFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inDataRef"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  XPLMDataRef inDataRef = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  return PyLong_FromLong(XPLMGetDatai(inDataRef));
}

static PyObject *XPLMSetDataiFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inDataRef"},
    {1, 'i', "inValue"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 2, paramChecks)){
    return NULL;
  }
  XPLMDataRef inDataRef = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  int inValue = PyLong_AsLong(PySequence_GetItem(args, 1));
  XPLMSetDatai(inDataRef, inValue);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetDatafFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inDataRef"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  XPLMDataRef inDataRef = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  return PyFloat_FromDouble(XPLMGetDataf(inDataRef));
}

static PyObject *XPLMSetDatafFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *tmp;
  float inValue;
  if(PyArg_ParseTuple(args, "Of", &tmp, &inValue)){
    XPLMDataRef inDataRef = (XPLMDataRef)PyLong_AsVoidPtr(PyNumber_Long(tmp));
    XPLMSetDataf(inDataRef, inValue);
    Py_RETURN_NONE;
  }else{
    return NULL;
  }
}

static PyObject *XPLMGetDatadFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inDataRef"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  XPLMDataRef inDataRef = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  return PyFloat_FromDouble(XPLMGetDatad(inDataRef));
}

static PyObject *XPLMSetDatadFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *tmp;
  double inValue;
  if(PyArg_ParseTuple(args, "Od", &tmp, &inValue)){
    XPLMDataRef inDataRef = (XPLMDataRef)PyLong_AsVoidPtr(PyNumber_Long(tmp));
    XPLMSetDatad(inDataRef, inValue);
    Py_RETURN_NONE;
  }else{
    return NULL;
  }
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
  XPLMDataRef inDataRef = (XPLMDataRef)PyLong_AsVoidPtr(PyNumber_Long(drefObj));
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
    if(PySequence_Length(outValuesObj) > 0){
      PySequence_DelSlice(outValuesObj, 0, PySequence_Length(outValuesObj));
    }
    //If res < inMax, copy only res elements;
    //  if inMax < res, copy only inMax elements
    for(int i = 0; i < (res < inMax ? res : inMax); ++i){
      PyList_Append(outValuesObj, PyLong_FromLong(outValues[i]));
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
  XPLMDataRef inDataRef = (XPLMDataRef)PyLong_AsVoidPtr(PyNumber_Long(drefObj));
  if(!PySequence_Check(inValuesObj)){
    PyErr_SetString(PyExc_TypeError, "XPLMSetDatavi expects list as the inValues parameter.");
    return NULL;
  }
  if(PySequence_Length(inValuesObj) < inCount){
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetDatavi list too short.");
    return NULL;
  }
  inValues = (int *)malloc(inCount * sizeof(int));
  for(int i = 0; i < inCount; ++i){
    inValues[i] = PyLong_AsLong(PyNumber_Long(PySequence_GetItem(inValuesObj, i)));
  }
  
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
  XPLMDataRef inDataRef = (XPLMDataRef)PyLong_AsVoidPtr(PyNumber_Long(drefObj));
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
    if(PySequence_Length(outValuesObj) > 0){
      PySequence_DelSlice(outValuesObj, 0, PySequence_Length(outValuesObj));
    }
    //If res < inMax, copy only res elements;
    //  if inMax < res, copy only inMax elements
    for(int i = 0; i < (res < inMax ? res : inMax); ++i){
      PyList_Append(outValuesObj, PyFloat_FromDouble(outValues[i]));
    }
  }
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
  XPLMDataRef inDataRef = (XPLMDataRef)PyLong_AsVoidPtr(PyNumber_Long(drefObj));
  if(!PySequence_Check(inValuesObj)){
    PyErr_SetString(PyExc_TypeError, "XPLMSetDatavf expects list as the inValues parameter.");
    return NULL;
  }
  if(PySequence_Length(inValuesObj) < inCount){
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetDatavf list too short.");
    return NULL;
  }
  inValues = (float *)malloc(inCount * sizeof(float));
  for(int i = 0; i < inCount; ++i){
    inValues[i] = PyFloat_AsDouble(PyNumber_Float(PySequence_GetItem(inValuesObj, i)));
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
  XPLMDataRef inDataRef = (XPLMDataRef)PyLong_AsVoidPtr(PyNumber_Long(drefObj));
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
    if(PySequence_Length(outValuesObj) > 0){
      PySequence_DelSlice(outValuesObj, 0, PySequence_Length(outValuesObj));
    }
    //If res < inMax, copy only res elements;
    //  if inMax < res, copy only inMax elements
    for(int i = 0; i < (res < inMax ? res : inMax); ++i){
      PyList_Append(outValuesObj, PyLong_FromLong(outValues[i]));
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
  XPLMDataRef inDataRef = (XPLMDataRef)PyLong_AsVoidPtr(PyNumber_Long(drefObj));
  if(!PySequence_Check(inValuesObj)){
    PyErr_SetString(PyExc_TypeError, "XPLMSetDatab expects list as the inValues parameter.");
    return NULL;
  }
  if(PySequence_Length(inValuesObj) < inCount){
    PyErr_SetString(PyExc_RuntimeError, "XPLMSetDatab list too short.");
    return NULL;
  }
  inValues = (uint8_t *)malloc(inCount * sizeof(uint8_t));
  for(int i = 0; i < inCount; ++i){
    inValues[i] = PyLong_AsLong(PyNumber_Long(PySequence_GetItem(inValuesObj, i)));
  }
  
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
  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 4), "(O)",
                                        PySequence_GetItem(pCbks, 16));
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
  int res = PyLong_AsLong(PyNumber_Long(oRes));
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
  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 5), "(Oi)",
                                        PySequence_GetItem(pCbks, 17), inValue);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
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
  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 6), "(O)",
                                        PySequence_GetItem(pCbks, 16));
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
  float res = PyFloat_AsDouble(PyNumber_Float(oRes));
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
  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 7), "(Of)",
                                        PySequence_GetItem(pCbks, 17), inValue);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
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
  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 8), "(O)",
                                        PySequence_GetItem(pCbks, 16));
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
  double res = PyFloat_AsDouble(PyNumber_Float(oRes));
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
  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 9), "(Od)",
                                        PySequence_GetItem(pCbks, 17), inValue);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
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
  }

  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 10), "(OOii)",
                                        PySequence_GetItem(pCbks, 16), outValuesObj, inOffset, inMax);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
  int res = PyLong_AsLong(PyNumber_Long(oRes));
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
    res = 0;
  }else{
    if(outValues){
      for(int i = 0; i < res; ++i){
        outValues[i] = PyLong_AsLong(PyNumber_Long(PySequence_GetItem(outValuesObj, i)));
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
    PyList_Append(inValuesObj, PyLong_FromLong(inValues[i]));
  }

  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 11), "(OOii)",
                                        PySequence_GetItem(pCbks, 17), inValuesObj, inOffset, inCount);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
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
  }

  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 12), "(OOii)",
                                        PySequence_GetItem(pCbks, 16), outValuesObj, inOffset, inMax);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
  int res = PyLong_AsLong(PyNumber_Long(oRes));
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
    res = 0;
  }else{
    if(outValues){
      for(int i = 0; i < res; ++i){
        outValues[i] = PyFloat_AsDouble(PyNumber_Float(PySequence_GetItem(outValuesObj, i)));
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
    PyList_Append(inValuesObj, PyFloat_FromDouble(inValues[i]));
  }

  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 13), "(OOii)",
                                        PySequence_GetItem(pCbks, 17), inValuesObj, inOffset, inCount);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
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
  }

  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 14), "(OOii)",
                                        PySequence_GetItem(pCbks, 16), outValuesObj, inOffset, inMax);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
  int res = PyLong_AsLong(PyNumber_Long(oRes));
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
    res = 0;
  }else{
    if(outValue){
      uint8_t *pOutValue = (uint8_t *)outValue;
      for(int i = 0; i < res; ++i){
        pOutValue[i] = PyLong_AsLong(PyNumber_Long(PySequence_GetItem(outValuesObj, i)));
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
    PyList_Append(inValuesObj, PyLong_FromLong(pInValue[i]));
  }

  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 15), "(OOii)",
                                        PySequence_GetItem(pCbks, 17), inValuesObj, inOffset, inCount);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
  Py_DECREF(inValuesObj);
  Py_DECREF(oRes);
  return;
}


static PyObject *XPLMRegisterDataAccessorFun(PyObject *self, PyObject *args)
{
  (void)self;
  void *refcon = (void *)accessorCntr++;
  PyObject *refconObj = PyLong_FromVoidPtr(refcon);

  
  char *inDataName = PyUnicode_AsUTF8(PySequence_GetItem(args, 1));
  int inDataType = PyLong_AsLong(PySequence_GetItem(args, 2));
  int inIsWritable = PyLong_AsLong(PySequence_GetItem(args, 3));
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
  if(PyDict_SetItem(accessorDict, refconObj, args) != 0){
    Py_RETURN_NONE;
  }
  PyObject *resObj = PyLong_FromVoidPtr(res);
  PyDict_SetItem(drefDict, resObj, refconObj);
  Py_DECREF(refconObj);
  return resObj;
}

static PyObject *XPLMUnregisterDataAccessorFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *drefObj = PySequence_GetItem(args, 1);
  PyObject *refconObj = PyDict_GetItem(drefDict, drefObj);
  if(refconObj == NULL){
    printf("XPLMUnregisterDataref: No such dataref registered!\n");
    Py_RETURN_NONE;
  }
  PyObject *accessor = PyDict_GetItem(accessorDict, refconObj);
  if(accessor == NULL){
    printf("XPLMUnregisterDataref: No such refcon registered!\n");
    Py_RETURN_NONE;
  }
  PyObject *registerer = PySequence_GetItem(accessor, 0); 
  if(PySequence_GetItem(args, 0) != registerer){
    printf("XPLMUnregisterDataref: Don't unregister dataref you didn't register!!\n");
    Py_RETURN_NONE;
  }
  if(PyDict_DelItem(accessorDict, refconObj)){
    printf("XPLMUnregisterDataref: Couldn't remove the refcon.\n");
  }
  if(PyDict_DelItem(drefDict, drefObj)){
    printf("XPLMUnregisterDataref: Couldn't remove the dref.\n");
  }
  Py_RETURN_NONE;
}

static void dataChanged(void *inRefcon)
{
  PyObject *sharedObj = PyDict_GetItem(sharedDict, PyLong_FromVoidPtr(inRefcon));
  if(sharedObj == NULL){
    printf("Shared data callback called with wrong inRefcon: %p\n", inRefcon);
    return;
  }
  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(sharedObj, 3), "(O)",
                                        PySequence_GetItem(sharedObj, 4));
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oRes);
}

static PyObject *XPLMShareDataFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inDataName = PyUnicode_AsUTF8(PySequence_GetItem(args, 1));
  XPLMDataTypeID inDataType = PyLong_AsLong(PySequence_GetItem(args, 2));
  void *refcon = (void *)sharedCntr++;
  int res = XPLMShareData(inDataName, inDataType, dataChanged, refcon);
  if(res != 1){
    return PyLong_FromLong(res);
  }
  PyDict_SetItem(sharedDict, PyLong_FromVoidPtr(refcon), args);
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
  PyObject *selfObj = NULL;
  if(!PyArg_ParseTuple(args, "OsiOO", &selfObj, &inDataName, &tmpInDataType, &callbackObj, &refconObj)){
    return NULL;
  }
  inDataType = (int)tmpInDataType;
  while(PyDict_Next(sharedDict, &cnt, &pKey, &pVal)){
    if(PySequence_GetItem(pVal, 0) != selfObj){
      continue;
    }
    if(strcmp(inDataName, PyUnicode_AsUTF8(PyObject_Str(PySequence_GetItem(pVal, 1)))) != 0){
      continue;
    }
    if(PyLong_AsLong(PyNumber_Long(PySequence_GetItem(pVal, 2))) != inDataType){
      continue;
    }
    if(PySequence_GetItem(pVal, 3) != callbackObj){
      continue;
    }
    if(PySequence_GetItem(pVal, 4) != refconObj){
      continue;
    }
    PyDict_DelItem(sharedDict, pKey);
    int res = XPLMUnshareData(inDataName, inDataType, dataChanged, PyLong_AsVoidPtr(pKey));
    return PyLong_FromLong(res);
  }
  printf("Couldn't find the right shared data...\n");
  return PyLong_FromLong(0);
}
//TODO: check every refcon - doesn't have to be integer!!!
static PyMethodDef XPLMDataAccessMethods[] = {
  {"XPLMFindDataRef", XPLMFindDataRefFun, METH_VARARGS, "Find a dataref"},
  {"XPLMCanWriteDataRef", XPLMCanWriteDataRefFun, METH_VARARGS, "Check dataref writeability"},
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

    // s/^[[:blank:]]\+,\?\(xplm[[:alnum:]_]\+\)[[:blank:]]\+=[[:blank:]]\+\([[:digit:]]\+\)/    PyModule_AddIntConstant(mod, "\1", \2);/
    // s/ \/\* /#/
    // s/\*\///
    // s/[[:blank:]]\+$//

    /* Data of a type the current XPLM doesn't do.                                 */
    PyModule_AddIntConstant(mod, "xplmType_Unknown", 0);

    /* A single 4-byte integer, native endian.                                     */
    PyModule_AddIntConstant(mod, "xplmType_Int", 1);

    /* A single 4-byte float, native endian.                                       */
    PyModule_AddIntConstant(mod, "xplmType_Float", 2);

    /* A single 8-byte double, native endian.                                      */
    PyModule_AddIntConstant(mod, "xplmType_Double", 4);

    /* An array of 4-byte floats, native endian.                                   */
    PyModule_AddIntConstant(mod, "xplmType_FloatArray", 8);

    /* An array of 4-byte integers, native endian.                                 */
    PyModule_AddIntConstant(mod, "xplmType_IntArray", 16);

    /* A variable block of data.                                                   */
    PyModule_AddIntConstant(mod, "xplmType_Data", 32);
  }

  return mod;
}


