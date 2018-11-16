#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#define XPLM210
#define XPLM300
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMScenery.h>
#include "utils.h"
#include "plugin_dl.h"

static PyObject *XPLMCreateProbeFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inProbeType;
  if(!PyArg_ParseTuple(args, "i", &inProbeType)){
    return NULL;
  }
  return PyLong_FromVoidPtr(XPLMCreateProbe(inProbeType));
}

static PyObject *XPLMDestroyProbeFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *inProbe = NULL;
  if(!PyArg_ParseTuple(args, "O", &inProbe)){
    return NULL;
  }
  XPLMDestroyProbe(PyLong_AsVoidPtr(inProbe));
  Py_RETURN_NONE;
}

static PyObject *XPLMProbeTerrainXYZFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *probe;
  float inX, inY, inZ;
  PyObject *info;

  if(!PyArg_ParseTuple(args, "OfffO", &probe, &inX, &inY, &inZ, &info)){
    return NULL;
  }
  XPLMProbeRef inProbe = PyLong_AsVoidPtr(probe);
  XPLMProbeInfo_t outInfo;
  XPLMProbeResult res = XPLMProbeTerrainXYZ(inProbe, inX, inY, inZ, &outInfo);

  if(PySequence_Size(info) > 0){
    PySequence_DelSlice(info, 0, PySequence_Size(info));
  }
  PyList_Append(info, PyLong_FromLong(outInfo.structSize));
  PyList_Append(info, PyFloat_FromDouble(outInfo.locationX));
  PyList_Append(info, PyFloat_FromDouble(outInfo.locationY));
  PyList_Append(info, PyFloat_FromDouble(outInfo.locationZ));
  PyList_Append(info, PyFloat_FromDouble(outInfo.normalX));
  PyList_Append(info, PyFloat_FromDouble(outInfo.normalY));
  PyList_Append(info, PyFloat_FromDouble(outInfo.normalZ));
  PyList_Append(info, PyFloat_FromDouble(outInfo.velocityX));
  PyList_Append(info, PyFloat_FromDouble(outInfo.velocityY));
  PyList_Append(info, PyFloat_FromDouble(outInfo.velocityZ));
  PyList_Append(info, PyLong_FromLong(outInfo.is_wet));

  return PyLong_FromLong(res);
}

static PyObject *XPLMGetMagneticVariationFun(PyObject *self, PyObject *args)
{
  (void)self;
  if(!XPLMGetMagneticVariation_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetMagneticVariation is available only in XPLM300 and up.");
    return NULL;
  }
  double latitude, longitude;
  if(!PyArg_ParseTuple(args, "dd", &latitude, &longitude)){
    return NULL;
  }
  return PyFloat_FromDouble(XPLMGetMagneticVariation_ptr(latitude, longitude));
}

static PyObject *XPLMDegTrueToDegMagneticFun(PyObject *self, PyObject *args)
{
  (void)self;
  if(!XPLMDegTrueToDegMagnetic_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDegTrueToDegMagnetic is available only in XPLM300 and up.");
    return NULL;
  }
  float headingDegreesTrue;
  if(!PyArg_ParseTuple(args, "f", &headingDegreesTrue)){
    return NULL;
  }
  return PyFloat_FromDouble(XPLMDegTrueToDegMagnetic_ptr(headingDegreesTrue));
}

static PyObject *XPLMDegMagneticToDegTrueFun(PyObject *self, PyObject *args)
{
  (void)self;
  if(!XPLMDegMagneticToDegTrue_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMDegMagneticToDegTrue is available only in XPLM300 and up.");
    return NULL;
  }
  float headingDegreesMagnetic;
  if(!PyArg_ParseTuple(args, "f", &headingDegreesMagnetic)){
    return NULL;
  }
  return PyFloat_FromDouble(XPLMDegMagneticToDegTrue_ptr(headingDegreesMagnetic));
}


static PyObject *XPLMLoadObjectFun(PyObject *self, PyObject *args)
{
  (void)self;
  const char *inPath;
  if(!PyArg_ParseTuple(args, "s", &inPath)){
    return NULL;
  }
  XPLMObjectRef res = XPLMLoadObject(inPath);
  return PyLong_FromVoidPtr(res);
}



static PyObject *loaderDict;
static intptr_t loaderCntr;

static void objectLoaded(XPLMObjectRef inObject, void *inRefcon)
{
  PyObject *object = PyLong_FromVoidPtr(inObject);
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *loaderCallbackInfo = PyDict_GetItem(loaderDict, pID);
  Py_XDECREF(pID);
  if(loaderCallbackInfo == NULL){
    printf("Unknown callback requested in objectLoaded(%p).\n", inRefcon);
    return;
  }
  PyObject *res = PyObject_CallFunction(PySequence_GetItem(loaderCallbackInfo, 2), "(OO)",
                                           object, PySequence_GetItem(loaderCallbackInfo, 3));
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the flightLoop callback(inRefcon = %p):\n", inRefcon);
    PyErr_Print();
  }
  PyDict_DelItem(loaderDict, pID);
  Py_XDECREF(res);
  Py_XDECREF(object);
}


static PyObject *XPLMLoadObjectAsyncFun(PyObject *self, PyObject *args)
{
  (void)self;
  if(!XPLMLoadObjectAsync_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMLoadObjectAsync is available only in XPLM210 and up.");
    return NULL;
  }
  char *inPath = PyUnicode_AsUTF8(PySequence_GetItem(args, 1));
  void *refcon = (void *)++loaderCntr;
  PyObject *key = PyLong_FromVoidPtr(refcon);
  PyDict_SetItem(loaderDict, key, args);
  Py_DECREF(key);
  XPLMLoadObjectAsync_ptr(inPath, objectLoaded, refcon);
  Py_RETURN_NONE;
}

static PyObject *XPLMDrawObjectsFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *object;
  int inCount;
  PyObject *locations, *tmp;
  int lighting;
  int earth_relative;
  if(!PyArg_ParseTuple(args, "OiOii", &object, &inCount, &locations, &lighting, &earth_relative)){
    return NULL;
  }
  XPLMObjectRef inObject = PyLong_AsVoidPtr(object);
  Py_DECREF(object);
  XPLMDrawInfo_t *inLocations = (XPLMDrawInfo_t *)malloc(inCount * sizeof(XPLMDrawInfo_t));
  int i;
  for(i = 0; i < inCount; ++i){
    PyObject *loc = PySequence_GetItem(locations, i);
    inLocations[i].structSize = sizeof(XPLMDrawInfo_t);
    tmp = PyNumber_Float(PySequence_GetItem(loc, 0));
    inLocations[i].x = PyFloat_AsDouble(tmp);
    Py_DECREF(tmp);
    tmp = PyNumber_Float(PySequence_GetItem(loc, 1));
    inLocations[i].y = PyFloat_AsDouble(tmp);
    Py_DECREF(tmp);
    tmp = PyNumber_Float(PySequence_GetItem(loc, 2));
    inLocations[i].z = PyFloat_AsDouble(tmp);
    Py_DECREF(tmp);
    tmp = PyNumber_Float(PySequence_GetItem(loc, 3));
    inLocations[i].pitch = PyFloat_AsDouble(tmp);
    Py_DECREF(tmp);
    tmp = PyNumber_Float(PySequence_GetItem(loc, 4));
    inLocations[i].heading = PyFloat_AsDouble(tmp);
    Py_DECREF(tmp);
    tmp = PyNumber_Float(PySequence_GetItem(loc, 5));
    inLocations[i].roll = PyFloat_AsDouble(tmp);
    Py_DECREF(tmp);
  }

  XPLMDrawObjects(inObject, inCount, inLocations, lighting, earth_relative);
  free(inLocations);
  Py_RETURN_NONE;
}

static PyObject *XPLMUnloadObjectFun(PyObject *self, PyObject *args)
{
  (void)self;
  XPLMObjectRef inObject = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  XPLMUnloadObject(inObject);
  Py_RETURN_NONE;
}

static PyObject *libEnumDict;
static intptr_t libEnumCntr;

static void libraryEnumerator(const char *inFilePath, void *inRef)
{
  PyObject *pID = PyLong_FromVoidPtr(inRef);
  PyObject *libEnumCallbackInfo = PyDict_GetItem(libEnumDict, pID);
  Py_XDECREF(pID);
  if(libEnumCallbackInfo == NULL){
    printf("Unknown callback requested from libraryEnumerator(%p).\n", inRef);
    return;
  }
  PyObject *res = PyObject_CallFunction(PySequence_GetItem(libEnumCallbackInfo, 4), "(sO)",
                                           inFilePath, PySequence_GetItem(libEnumCallbackInfo, 5));
  Py_XDECREF(res);
}

static PyObject *XPLMLookupObjectsFun(PyObject *self, PyObject *args)
{
  (void)self;
  const char *inPath;
  float inLatitude, inLongitude;
  PyObject *enumerator;
  PyObject *ref;
  if(!PyArg_ParseTuple(args, "OsffOO", &self, &inPath, &inLatitude, &inLongitude, &enumerator, &ref)){
    return NULL;
  }
  void *myRef = (void *)++libEnumCntr;
  PyObject *refObj = PyLong_FromVoidPtr(myRef);
  PyDict_SetItem(libEnumDict, refObj, args);
  Py_XDECREF(refObj);
  int res = XPLMLookupObjects(inPath, inLatitude, inLongitude, libraryEnumerator, myRef);
  return PyLong_FromLong(res);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(loaderDict);
  Py_DECREF(loaderDict);
  PyDict_Clear(libEnumDict);
  Py_DECREF(libEnumDict);
  Py_RETURN_NONE;
}

static PyMethodDef XPLMSceneryMethods[] = {
  {"XPLMCreateProbe", XPLMCreateProbeFun, METH_VARARGS, ""},
  {"XPLMDestroyProbe", XPLMDestroyProbeFun, METH_VARARGS, ""},
  {"XPLMProbeTerrainXYZ", XPLMProbeTerrainXYZFun, METH_VARARGS, ""},
  {"XPLMDegMagneticToDegTrue", XPLMDegMagneticToDegTrueFun, METH_VARARGS, ""},
  {"XPLMGetMagneticVariation", XPLMGetMagneticVariationFun, METH_VARARGS, ""},
  {"XPLMDegTrueToDegMagnetic", XPLMDegTrueToDegMagneticFun, METH_VARARGS, ""},
  {"XPLMLoadObject", XPLMLoadObjectFun, METH_VARARGS, ""},
  {"XPLMLoadObjectAsync", XPLMLoadObjectAsyncFun, METH_VARARGS, ""},
  {"XPLMDrawObjects", XPLMDrawObjectsFun, METH_VARARGS, ""},
  {"XPLMUnloadObject", XPLMUnloadObjectFun, METH_VARARGS, ""},
  {"XPLMLookupObjects", XPLMLookupObjectsFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMSceneryModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMScenery",
  NULL,
  -1,
  XPLMSceneryMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMScenery(void)
{
  if(!(loaderDict = PyDict_New())){
    return NULL;
  }
  if(!(libEnumDict = PyDict_New())){
    return NULL;
  }
  PyObject *mod = PyModule_Create(&XPLMSceneryModule);
  if(mod){
    PyModule_AddIntConstant(mod, "xplm_ProbeY", xplm_ProbeY);

    PyModule_AddIntConstant(mod, "xplm_ProbeHitTerrain", xplm_ProbeHitTerrain);
    PyModule_AddIntConstant(mod, "xplm_ProbeError", xplm_ProbeError);
    PyModule_AddIntConstant(mod, "xplm_ProbeMissed ", xplm_ProbeMissed );

  }

  return mod;
}




