#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMNavigation.h>
#include "utils.h"

static PyObject *XPLMGetFirstNavAidFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetFirstNavAid());
}

static PyObject *XPLMGetNextNavAidFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inNavAidRef;
  if(!PyArg_ParseTuple(args, "i", &inNavAidRef)){
    return NULL;
  }
  return PyLong_FromLong(XPLMGetNextNavAid(inNavAidRef));
}

static PyObject *XPLMFindFirstNavAidOfTypeFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inType;
  if(!PyArg_ParseTuple(args, "i", &inType)){
    return NULL;
  }
  return PyLong_FromLong(XPLMFindFirstNavAidOfType(inType));
}

static PyObject *XPLMFindLastNavAidOfTypeFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inType;
  if(!PyArg_ParseTuple(args, "i", &inType)){
    return NULL;
  }
  return PyLong_FromLong(XPLMFindLastNavAidOfType(inType));
}

static PyObject *XPLMFindNavAidFun(PyObject *self, PyObject *args)
{
  (void)self;
  const char *inNameFragment = NULL;
  const char *inIDFragment = NULL;
  PyObject *objLat, *objLon, *objFreq;
  float lat, *inLat = NULL;
  float lon, *inLon = NULL;
  int frequency, *inFrequency = NULL;
  int inType;
  if(!PyArg_ParseTuple(args, "zzOOOi", &inNameFragment, &inIDFragment, &objLat, &objLon, &objFreq, &inType)){
    return NULL;
  }
  PyObject *tmp;
  if(objLat != Py_None){
    tmp = PyNumber_Float(objLat);
    lat = PyFloat_AsDouble(tmp);
    Py_DECREF(tmp);
    inLat = &lat;
  }
  if(objLon != Py_None){
    tmp = PyNumber_Float(objLon);
    lon = PyFloat_AsDouble(tmp);
    Py_DECREF(tmp);
    inLon = &lon;
  }
  if(objFreq != Py_None){
    tmp = PyNumber_Long(objFreq);
    frequency = PyLong_AsLong(tmp);
    Py_DECREF(tmp);
    inFrequency = &frequency;
  }
  return PyLong_FromLong(XPLMFindNavAid(inNameFragment, inIDFragment, inLat, inLon, inFrequency, inType));
}

static PyObject *XPLMGetNavAidInfoFun(PyObject *self, PyObject *args)
{
  (void)self;
  XPLMNavRef inRef;
  PyObject *type;
  PyObject *latitude;
  PyObject *longitude;
  PyObject *height;
  PyObject *frequency;
  PyObject *heading;
  PyObject *id;
  PyObject *name;
  PyObject *reg;
  if(!PyArg_ParseTuple(args, "iOOOOOOOOO", &inRef, &type, &latitude, &longitude, &height, &frequency, &heading, &id,
                       &name, &reg)){
    return NULL;
  }
  XPLMNavType outType;
  float outLatitude, outLongitude, outHeight, outHeading;
  int outFrequency;
  char outID[512];
  char outName[512];
  char outReg[512];
  XPLMGetNavAidInfo(inRef, &outType, &outLatitude, &outLongitude, &outHeight, &outFrequency, &outHeading, 
                    outID, outName, outReg);
  if(type != Py_None){
    PyList_SetItem(type, 0, PyLong_FromLong(outType));
  }
  if(latitude != Py_None){
    PyList_SetItem(latitude, 0, PyFloat_FromDouble(outLatitude));
  }
  if(longitude != Py_None){
    PyList_SetItem(longitude, 0, PyFloat_FromDouble(outLongitude));
  }
  if(height != Py_None){
    PyList_SetItem(height, 0, PyFloat_FromDouble(outHeight));
  }
  if(frequency != Py_None){
    PyList_SetItem(frequency, 0, PyLong_FromLong(outFrequency));
  }
  if(heading != Py_None){
    PyList_SetItem(heading, 0, PyFloat_FromDouble(outHeading));
  }
  if(id != Py_None){
    PyList_SetItem(id, 0, PyUnicode_DecodeUTF8(outID, strlen(outID), NULL));
  }
  if(name != Py_None){
    PyList_SetItem(name, 0, PyUnicode_DecodeUTF8(outName, strlen(outName), NULL));
  }
  if(reg != Py_None){
    PyList_SetItem(reg, 0, PyUnicode_DecodeUTF8(outReg, strlen(outReg), NULL));
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMCountFMSEntriesFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMCountFMSEntries());
}

static PyObject *XPLMGetDisplayedFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetDisplayedFMSEntry());
}

static PyObject *XPLMGetDestinationFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetDestinationFMSEntry());
}

static PyObject *XPLMSetDisplayedFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inIndex;
  if(!PyArg_ParseTuple(args, "i", &inIndex)){
    return NULL;
  }
  XPLMSetDisplayedFMSEntry(inIndex);
  Py_RETURN_NONE;
}

static PyObject *XPLMSetDestinationFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inIndex;
  if(!PyArg_ParseTuple(args, "i", &inIndex)){
    return NULL;
  }
  XPLMSetDestinationFMSEntry(inIndex);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetFMSEntryInfoFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inIndex;
  PyObject *type;
  PyObject *id;
  PyObject *ref;
  PyObject *altitude;
  PyObject *lat;
  PyObject *lon;
  if(!PyArg_ParseTuple(args, "iOOOOOO", &inIndex, &type, &id, &ref, &altitude, &lat, &lon)){
    return NULL;
  }
  XPLMNavType outType;
  float outLat, outLon;
  char outID[512];
  XPLMNavRef outRef;
  int outAltitude;

  XPLMGetFMSEntryInfo(inIndex, &outType, outID, &outRef, &outAltitude, &outLat, &outLon);
  if(type != Py_None){
    PyList_SetItem(type, 0, PyLong_FromLong(outType));
  }
  if(id != Py_None){
    PyList_SetItem(id, 0, PyUnicode_DecodeUTF8(outID, strlen(outID), NULL));
  }
  if(ref != Py_None){
    PyList_SetItem(ref, 0, PyLong_FromLong(outRef));
  }
  if(altitude != Py_None){
    PyList_SetItem(altitude, 0, PyLong_FromLong(outAltitude));
  }
  if(lat != Py_None){
    PyList_SetItem(lat, 0, PyFloat_FromDouble(outLat));
  }
  if(lon != Py_None){
    PyList_SetItem(lon, 0, PyFloat_FromDouble(outLon));
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMSetFMSEntryInfoFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inIndex;
  XPLMNavRef inRef;
  int inAltitude;
  if(!PyArg_ParseTuple(args, "iii", &inIndex, &inRef, &inAltitude)){
    return NULL;
  }
  XPLMSetFMSEntryInfo(inIndex, inRef, inAltitude);
  Py_RETURN_NONE;
}

static PyObject *XPLMSetFMSEntryLatLonFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inIndex;
  float inLat;
  float inLon;
  int inAltitude;
  if(!PyArg_ParseTuple(args, "iffi", &inIndex, &inLat, &inLon, &inAltitude)){
    return NULL;
  }
  XPLMSetFMSEntryLatLon(inIndex, inLat, inLon, inAltitude);
  Py_RETURN_NONE;
}

static PyObject *XPLMClearFMSEntryFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inIndex;
  if(!PyArg_ParseTuple(args, "i", &inIndex)){
    return NULL;
  }
  XPLMClearFMSEntry(inIndex);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetGPSDestinationTypeFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetGPSDestinationType());
}

static PyObject *XPLMGetGPSDestinationFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromLong(XPLMGetGPSDestination());
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyMethodDef XPLMNavigationMethods[] = {
  {"XPLMGetFirstNavAid", XPLMGetFirstNavAidFun, METH_VARARGS, ""},
  {"XPLMGetNextNavAid", XPLMGetNextNavAidFun, METH_VARARGS, ""},
  {"XPLMFindFirstNavAidOfType", XPLMFindFirstNavAidOfTypeFun, METH_VARARGS, ""},
  {"XPLMFindLastNavAidOfType", XPLMFindLastNavAidOfTypeFun, METH_VARARGS, ""},
  {"XPLMFindNavAid", XPLMFindNavAidFun, METH_VARARGS, ""},
  {"XPLMGetNavAidInfo", XPLMGetNavAidInfoFun, METH_VARARGS, ""},
  {"XPLMCountFMSEntries", XPLMCountFMSEntriesFun, METH_VARARGS, ""},
  {"XPLMGetDisplayedFMSEntry", XPLMGetDisplayedFMSEntryFun, METH_VARARGS, ""},
  {"XPLMGetDestinationFMSEntry", XPLMGetDestinationFMSEntryFun, METH_VARARGS, ""},
  {"XPLMSetDisplayedFMSEntry", XPLMSetDisplayedFMSEntryFun, METH_VARARGS, ""},
  {"XPLMSetDestinationFMSEntry", XPLMSetDestinationFMSEntryFun, METH_VARARGS, ""},
  {"XPLMGetFMSEntryInfo", XPLMGetFMSEntryInfoFun, METH_VARARGS, ""},
  {"XPLMSetFMSEntryInfo", XPLMSetFMSEntryInfoFun, METH_VARARGS, ""},
  {"XPLMSetFMSEntryLatLon", XPLMSetFMSEntryLatLonFun, METH_VARARGS, ""},
  {"XPLMClearFMSEntry", XPLMClearFMSEntryFun, METH_VARARGS, ""},
  {"XPLMGetGPSDestinationType", XPLMGetGPSDestinationTypeFun, METH_VARARGS, ""},
  {"XPLMGetGPSDestination", XPLMGetGPSDestinationFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMNavigationModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMNavigation",
  NULL,
  -1,
  XPLMNavigationMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMNavigation(void)
{
  PyObject *mod = PyModule_Create(&XPLMNavigationModule);
  if(mod){
    PyModule_AddIntConstant(mod, "xplm_Nav_Unknown", xplm_Nav_Unknown);
    PyModule_AddIntConstant(mod, "xplm_Nav_Airport", xplm_Nav_Airport);
    PyModule_AddIntConstant(mod, "xplm_Nav_NDB", xplm_Nav_NDB);
    PyModule_AddIntConstant(mod, "xplm_Nav_VOR", xplm_Nav_VOR);
    PyModule_AddIntConstant(mod, "xplm_Nav_ILS", xplm_Nav_ILS);
    PyModule_AddIntConstant(mod, "xplm_Nav_Localizer", xplm_Nav_Localizer);
    PyModule_AddIntConstant(mod, "xplm_Nav_GlideSlope", xplm_Nav_GlideSlope);
    PyModule_AddIntConstant(mod, "xplm_Nav_OuterMarker", xplm_Nav_OuterMarker);
    PyModule_AddIntConstant(mod, "xplm_Nav_MiddleMarker", xplm_Nav_MiddleMarker);
    PyModule_AddIntConstant(mod, "xplm_Nav_InnerMarker", xplm_Nav_InnerMarker);
    PyModule_AddIntConstant(mod, "xplm_Nav_Fix", xplm_Nav_Fix);
    PyModule_AddIntConstant(mod, "xplm_Nav_DME", xplm_Nav_DME);
    PyModule_AddIntConstant(mod, "xplm_Nav_LatLon", xplm_Nav_LatLon);
    
    PyModule_AddIntConstant(mod, "XPLM_NAV_NOT_FOUND", XPLM_NAV_NOT_FOUND);
  }

  return mod;
}


