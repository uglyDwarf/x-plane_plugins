#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMGraphics.h>
#include "utils.h"


static PyObject *XPLMSetGraphicsStateFun(PyObject *self, PyObject *args)
{
  (void) self;

  int inEnableFog;
  int inNumberTexUnits;
  int inEnableLighting;
  int inEnableAlphaTesting;
  int inEnableAlphaBlending;
  int inEnableDepthTesting;
  int inEnableDepthWriting;

  if(!PyArg_ParseTuple(args, "iiiiiii", &inEnableFog, &inNumberTexUnits, &inEnableLighting, &inEnableAlphaTesting,
                                        &inEnableAlphaBlending, &inEnableDepthTesting, &inEnableDepthWriting)){
    return NULL;
  }

  XPLMSetGraphicsState(inEnableFog, inNumberTexUnits, inEnableLighting,
                       inEnableAlphaTesting, inEnableAlphaBlending,
                       inEnableDepthTesting, inEnableDepthWriting);
  Py_RETURN_NONE;
} 

static PyObject *XPLMBindTexture2dFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inTextureNum;
  int inTextureUnit;

  if(!PyArg_ParseTuple(args, "ii", &inTextureNum, &inTextureUnit)){
    return NULL;
  }

  XPLMBindTexture2d(inTextureNum, inTextureUnit); 
  Py_RETURN_NONE;
} 

static PyObject *XPLMGenerateTextureNumbersFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *outTextureIDs;
  int inCount;

  if(!PyArg_ParseTuple(args, "Oi", &outTextureIDs, &inCount)){
    return NULL;
  }

  int *array = (int *)malloc(sizeof(int) * inCount);
  if(!array){
    PyErr_SetString(PyExc_RuntimeError , "Can't malloc outTextureIDs.");
    return NULL;
  }
  
  XPLMGenerateTextureNumbers(array, inCount);
  int i;
  Py_ssize_t len = PyList_Size(outTextureIDs);
  for(i = 0; i < inCount; ++i){
    if(i < len){
      if(PyList_SetItem(outTextureIDs, i, PyLong_FromLong(array[i]))){
        printf("Problem setting item!\n");
      }
    }else{
      if(PyList_Append(outTextureIDs, PyLong_FromLong(array[i]))){
        printf("Problem appending item!\n");
      }
    }
  }
  free(array);
  Py_RETURN_NONE;
} 

static PyObject *XPLMGetTextureFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inTexture;
  if(!PyArg_ParseTuple(args, "i", &inTexture)){
    return NULL;
  }

  int tex = XPLMGetTexture(inTexture);
  return PyLong_FromLong(tex);
}

static PyObject *XPLMWorldToLocalFun(PyObject *self, PyObject *args)
{
  (void) self;
  double inLatitude;
  double inLongitude;
  double inAltitude;
  double outX, outY, outZ;
  if(!PyArg_ParseTuple(args, "ddd", &inLatitude, &inLongitude, &inAltitude)){
    return NULL;
  }
  XPLMWorldToLocal(inLatitude, inLongitude, inAltitude, &outX, &outY, &outZ);
  
  PyObject *res = PyTuple_New(3);
  PyTuple_SetItem(res, 0, PyFloat_FromDouble(outX));
  PyTuple_SetItem(res, 1, PyFloat_FromDouble(outY));
  PyTuple_SetItem(res, 2, PyFloat_FromDouble(outZ));
  return res;
}

static PyObject *XPLMLocalToWorldFun(PyObject *self, PyObject *args)
{
  (void) self;
  double inX;
  double inY;
  double inZ;
  double outLatitude, outLongitude, outAltitude;
  if(!PyArg_ParseTuple(args, "ddd", &inX, &inY, &inZ)){
    return NULL;
  }
  XPLMLocalToWorld(inX, inY, inZ, &outLatitude, &outLongitude, &outAltitude);
  
  PyObject *res = PyTuple_New(3);
  PyTuple_SetItem(res, 0, PyFloat_FromDouble(outLatitude));
  PyTuple_SetItem(res, 1, PyFloat_FromDouble(outLongitude));
  PyTuple_SetItem(res, 2, PyFloat_FromDouble(outAltitude));
  return res;
}

static PyObject *XPLMDrawTranslucentDarkBoxFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inLeft;
  int inTop;
  int inRight;
  int inBottom;

  if(!PyArg_ParseTuple(args, "iiii", &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  XPLMDrawTranslucentDarkBox(inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

static PyObject *XPLMDrawStringFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *rgbList;
  int inXOffset;
  int inYOffset;
  const char *inCharC;
  char *inChar;
  PyObject *wordWrapWidthObj;
  int wordWrapWidth;
  int *inWordWrapWidth;
  int inFontID;

  if(!PyArg_ParseTuple(args, "OiisOi", &rgbList, &inXOffset, &inYOffset, &inCharC, &wordWrapWidthObj, &inFontID)){
    return NULL;
  }
  if(PySequence_Size(rgbList) != 3){
    PyErr_SetString(PyExc_TypeError , "inColourRGB must have 3 items");
    return NULL;
  }
  PyObject *r, *g, *b;
  r = PyNumber_Float(PySequence_GetItem(rgbList, 0));
  g = PyNumber_Float(PySequence_GetItem(rgbList, 1));
  b = PyNumber_Float(PySequence_GetItem(rgbList, 2));
  float inColorRGB[3] = {PyFloat_AsDouble(r),
                         PyFloat_AsDouble(g),
                         PyFloat_AsDouble(b)};
  Py_DECREF(r);
  Py_DECREF(g);
  Py_DECREF(b);
  if(wordWrapWidthObj == Py_None){
    inWordWrapWidth = NULL;
  }else{
    inWordWrapWidth = &wordWrapWidth;
    PyObject *tmp = PyNumber_Long(wordWrapWidthObj);
    wordWrapWidth = PyLong_AsLong(tmp);
    Py_DECREF(tmp);
  }
  inChar = strdup(inCharC);
  XPLMDrawString(inColorRGB, inXOffset, inYOffset, inChar, inWordWrapWidth, inFontID);
  free(inChar);
  Py_RETURN_NONE;
}

static PyObject *XPLMDrawNumberFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *rgbList;
  int inXOffset;
  int inYOffset;
  double inValue;
  int inDigits;
  int inDecimals;
  int inShowSign;
  int inFontID;

  if(!PyArg_ParseTuple(args, "Oiidiiii", &rgbList, &inXOffset, &inYOffset, &inValue,
                       &inDigits, &inDecimals, &inShowSign, &inFontID)){
    return NULL;
  }
  if(PySequence_Size(rgbList) != 3){
    PyErr_SetString(PyExc_TypeError , "inColourRGB must have 3 items");
    return NULL;
  }
  PyObject *r, *g, *b;
  r = PyNumber_Float(PySequence_GetItem(rgbList, 0));
  g = PyNumber_Float(PySequence_GetItem(rgbList, 1));
  b = PyNumber_Float(PySequence_GetItem(rgbList, 2));
  float inColorRGB[3] = {PyFloat_AsDouble(r),
                         PyFloat_AsDouble(g),
                         PyFloat_AsDouble(b)};
  Py_DECREF(r);
  Py_DECREF(g);
  Py_DECREF(b);

  XPLMDrawNumber(inColorRGB, inXOffset, inYOffset, inValue, inDigits, inDecimals, inShowSign, inFontID);

  Py_RETURN_NONE;
}

static PyObject *XPLMGetFontDimensionsFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inFontID;
  PyObject *charWidth, *charHeight, *digitsOnly;
  if(!PyArg_ParseTuple(args, "iOOO", &inFontID, &charWidth, &charHeight, &digitsOnly)){
    return NULL;
  }

  int outCharWidth, outCharHeight, outDigitsOnly;

  XPLMGetFontDimensions(inFontID, &outCharWidth, &outCharHeight, &outDigitsOnly);
  if(PyList_Check(charWidth)){
    PyList_Append(charWidth, PyLong_FromLong(outCharWidth));
  }
  if(PyList_Check(charHeight)){
    PyList_Append(charHeight, PyLong_FromLong(outCharHeight));
  }
  if(PyList_Check(digitsOnly)){
    PyList_Append(digitsOnly, PyLong_FromLong(outDigitsOnly));
  }

  Py_RETURN_NONE;
}

static PyObject *XPLMMeasureStringFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inFontID;
  char *inChar;
  int inNumChars;

  if(!PyArg_ParseTuple(args, "isi", &inFontID, &inChar, &inNumChars)){
    return NULL;
  }
  
  return PyFloat_FromDouble(XPLMMeasureString(inFontID, inChar, inNumChars));
}


static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyMethodDef XPLMGraphicsMethods[] = {
  {"XPLMSetGraphicsState", XPLMSetGraphicsStateFun, METH_VARARGS, "Sets state of the graphics pipeline."},
  {"XPLMBindTexture2d", XPLMBindTexture2dFun, METH_VARARGS, "Bind a 2D texture."},
  {"XPLMGenerateTextureNumbers", XPLMGenerateTextureNumbersFun, METH_VARARGS, "Generates number of texture IDs."},
  {"XPLMGetTexture", XPLMGetTextureFun, METH_VARARGS, "Get texture."},
  {"XPLMWorldToLocal", XPLMWorldToLocalFun, METH_VARARGS, "Transform world coordinates to local."},
  {"XPLMLocalToWorld", XPLMLocalToWorldFun, METH_VARARGS, "Transform local coordinates to world."},
  {"XPLMDrawTranslucentDarkBox", XPLMDrawTranslucentDarkBoxFun, METH_VARARGS, "Draw translucent window."},
  {"XPLMDrawString", XPLMDrawStringFun, METH_VARARGS, "Draw string."},
  {"XPLMDrawNumber", XPLMDrawNumberFun, METH_VARARGS, "Draw number."},
  {"XPLMGetFontDimensions", XPLMGetFontDimensionsFun, METH_VARARGS, "Get fond dimmensions."},
  {"XPLMMeasureString", XPLMMeasureStringFun, METH_VARARGS, "Measure a string."},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMGraphicsModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMGraphics",
  NULL,
  -1,
  XPLMGraphicsMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMGraphics(void)
{
  PyObject *mod = PyModule_Create(&XPLMGraphicsModule);
  if(mod){
    // s/^[[:blank:]]\+,\?\(xplm[[:alnum:]_]\+\)[[:blank:]]\+=[[:blank:]]\+\([[:digit:]]\+\)/    PyModule_AddIntConstant(mod, "\1", \2);/

     /* The bitmap that contains window outlines, button outlines, fonts, etc.      */
    PyModule_AddIntConstant(mod, "xplm_Tex_GeneralInterface", 0);
     /* The exterior paint for the user's aircraft (daytime).                       */
    PyModule_AddIntConstant(mod, "xplm_Tex_AircraftPaint", 1);
     /* The exterior light map for the user's aircraft.                             */
    PyModule_AddIntConstant(mod, "xplm_Tex_AircraftLiteMap", 2);

     /* Mono-spaced font for user interface.  Available in all versions of the SDK. */
    PyModule_AddIntConstant(mod, "xplmFont_Basic", 0);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_Menus", 1);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_Metal", 2);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_Led", 3);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_LedWide", 4);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_PanelHUD", 5);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_PanelEFIS", 6);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_PanelGPS", 7);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_RadiosGA", 8);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_RadiosBC", 9);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_RadiosHM", 10);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_RadiosGANarrow", 11);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_RadiosBCNarrow", 12);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_RadiosHMNarrow", 13);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_Timer", 14);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_FullRound", 15);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_SmallRound", 16);
     /* Deprecated, do not use.                                                     */
    PyModule_AddIntConstant(mod, "xplmFont_Menus_Localized", 17);
     /* Proportional UI font.                                                       */
    PyModule_AddIntConstant(mod, "xplmFont_Proportional", 18);
  }

  return mod;
}


