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
  paramCheck_t paramChecks[] = {
    {0, 'i', "inEnableFog"},
    {1, 'i', "inNumberTexUnits"},
    {2, 'i', "inEnableLighting"},
    {3, 'i', "inEnableAlbhaTesting"},
    {4, 'i', "inEnableAlphaBlending"},
    {5, 'i', "inEnableDepthTesting"},
    {6, 'i', "inEnableDepthWriting"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 7, paramChecks)){
    return NULL;
  }

  int inEnableFog = PyLong_AsLong(PySequence_GetItem(args, 0));
  int inNumberTexUnits = PyLong_AsLong(PySequence_GetItem(args, 1));
  int inEnableLighting = PyLong_AsLong(PySequence_GetItem(args, 2));
  int inEnableAlbhaTesting = PyLong_AsLong(PySequence_GetItem(args, 3));
  int inEnableAlphaBlending = PyLong_AsLong(PySequence_GetItem(args, 4));
  int inEnableDepthTesting = PyLong_AsLong(PySequence_GetItem(args, 5));
  int inEnableDepthWriting = PyLong_AsLong(PySequence_GetItem(args, 6));

  XPLMSetGraphicsState(inEnableFog, inNumberTexUnits, inEnableLighting,
                       inEnableAlbhaTesting, inEnableAlphaBlending,
                       inEnableDepthTesting, inEnableDepthWriting);
  Py_RETURN_NONE;
} 

static PyObject *XPLMBindTexture2dFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inTextureNum"},
    {1, 'i', "inTextureUnit"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 2, paramChecks)){
    return NULL;
  }

  int inTextureNum = PyLong_AsLong(PySequence_GetItem(args, 0));
  int inTextureUnit = PyLong_AsLong(PySequence_GetItem(args, 1));

  XPLMBindTexture2d(inTextureNum, inTextureUnit); 
  Py_RETURN_NONE;
} 

static PyObject *XPLMGenerateTextureNumbersFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'l', "outTextureIDs"},
    {1, 'i', "inCount"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 2, paramChecks)){
    return NULL;
  }

  PyObject *outTextureIDs = PySequence_GetItem(args, 0);
  int inCount = PyLong_AsLong(PySequence_GetItem(args, 1));

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
  Py_RETURN_NONE;
} 

static PyObject *XPLMGetTextureFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inTexture"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }

  int inTexture = PyLong_AsLong(PySequence_GetItem(args, 0));

  int tex = XPLMGetTexture(inTexture);
  return PyLong_FromLong(tex);
}

static PyObject *XPLMWorldToLocalFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'd', "inLatitude"},
    {1, 'd', "inLongitude"},
    {2, 'd', "inAltitude"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 3, paramChecks)){
    return NULL;
  }
  double inLatitude = PyFloat_AsDouble(PySequence_GetItem(args, 0));
  double inLongitude = PyFloat_AsDouble(PySequence_GetItem(args, 1));
  double inAltitude = PyFloat_AsDouble(PySequence_GetItem(args, 2));
  double outX, outY, outZ;
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
  paramCheck_t paramChecks[] = {
    {0, 'd', "inX"},
    {1, 'd', "inY"},
    {2, 'd', "inZ"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 3, paramChecks)){
    return NULL;
  }
  double inX = PyFloat_AsDouble(PySequence_GetItem(args, 0));
  double inY = PyFloat_AsDouble(PySequence_GetItem(args, 1));
  double inZ = PyFloat_AsDouble(PySequence_GetItem(args, 2));
  double outLatitude, outLongitude, outAltitude;
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
  paramCheck_t paramChecks[] = {
    {0, 'i', "inLeft"},
    {1, 'i', "inTop"},
    {2, 'i', "inRight"},
    {2, 'i', "inBottom"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 4, paramChecks)){
    return NULL;
  }
  double inLeft = PyLong_AsLong(PySequence_GetItem(args, 0));
  double inTop = PyLong_AsLong(PySequence_GetItem(args, 1));
  double inRight = PyLong_AsLong(PySequence_GetItem(args, 2));
  double inBottom = PyLong_AsLong(PySequence_GetItem(args, 3));

  XPLMDrawTranslucentDarkBox(inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

static PyObject *XPLMDrawStringFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'q', "inColorRGB"},
    {1, 'i', "inXOffset"},
    {2, 'i', "inYOffset"},
    {3, 's', "inChar"},
    {4, 'i', "inWordWrapWidth"},
    {5, 'i', "inFontID"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 6, paramChecks)){
    return NULL;
  }
  PyObject *rgbList = PySequence_GetItem(args, 0);
  if(PySequence_Size(rgbList) != 3){
    PyErr_SetString(PyExc_TypeError , "inColourRGB must have 3 items");
    return NULL;
  }
  float inColorRGB[3] = {PyFloat_AsDouble(PySequence_GetItem(rgbList, 0)),
                         PyFloat_AsDouble(PySequence_GetItem(rgbList, 1)),
                         PyFloat_AsDouble(PySequence_GetItem(rgbList, 2))};
  
  int inXOffset = PyLong_AsLong(PySequence_GetItem(args, 1));
  int inYOffset = PyLong_AsLong(PySequence_GetItem(args, 2));
  char *inChar = PyUnicode_AsUTF8(PySequence_GetItem(args, 3));
  int inWordWrapWidth = PyLong_AsLong(PySequence_GetItem(args, 4));
  int inFontID = PyLong_AsLong(PySequence_GetItem(args, 5));

  XPLMDrawString(inColorRGB, inXOffset, inYOffset, inChar, &inWordWrapWidth, inFontID);

  Py_RETURN_NONE;
}

static PyObject *XPLMDrawNumberFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'q', "inColorRGB"},
    {1, 'i', "inXOffset"},
    {2, 'i', "inYOffset"},
    {3, 'n', "inValue"},
    {4, 'i', "inDigits"},
    {5, 'i', "inDecimals"},
    {6, 'i', "inShowSign"},
    {7, 'i', "inFontID"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 8, paramChecks)){
    return NULL;
  }
  PyObject *rgbList = PySequence_GetItem(args, 0);
  if(PySequence_Size(rgbList) != 3){
    PyErr_SetString(PyExc_TypeError , "inColourRGB must have 3 items");
    return NULL;
  }
  float inColorRGB[3] = {PyFloat_AsDouble(PySequence_GetItem(rgbList, 0)),
                         PyFloat_AsDouble(PySequence_GetItem(rgbList, 1)),
                         PyFloat_AsDouble(PySequence_GetItem(rgbList, 2))};
  
  int inXOffset = PyLong_AsLong(PySequence_GetItem(args, 1));
  int inYOffset = PyLong_AsLong(PySequence_GetItem(args, 2));
  double inValue = PyFloat_AsDouble(PyNumber_Float(PySequence_GetItem(args, 3)));
  int inDigits = PyLong_AsLong(PySequence_GetItem(args, 4));
  int inDecimals = PyLong_AsLong(PySequence_GetItem(args, 5));
  int inShowSign = PyLong_AsLong(PySequence_GetItem(args, 6));
  int inFontID = PyLong_AsLong(PySequence_GetItem(args, 7));

  XPLMDrawNumber(inColorRGB, inXOffset, inYOffset, inValue, inDigits, inDecimals, inShowSign, inFontID);

  Py_RETURN_NONE;
}

static PyObject *XPLMGetFontDimensionsFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inFontID"},
    {1, 'l', "outCharWidth"},
    {2, 'l', "outCharHeight"},
    {3, 'l', "outDigitsOnly"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 4, paramChecks)){
    return NULL;
  }
  int inFontID = PyLong_AsLong(PySequence_GetItem(args, 0));

  int outCharWidth, outCharHeight, outDigitsOnly;

  XPLMGetFontDimensions(inFontID, &outCharWidth, &outCharHeight, &outDigitsOnly);
  PyList_Insert(PySequence_GetItem(args, 1), 0, PyLong_FromLong(outCharWidth));
  PyList_Insert(PySequence_GetItem(args, 2), 0, PyLong_FromLong(outCharHeight));
  PyList_Insert(PySequence_GetItem(args, 3), 0, PyLong_FromLong(outDigitsOnly));

  Py_RETURN_NONE;
}

static PyObject *XPLMMeasureStringFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inFontID"},
    {1, 's', "inChar"},
    {2, 'i', "inNumChars"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 3, paramChecks)){
    return NULL;
  }
  int inFontID = PyLong_AsLong(PySequence_GetItem(args, 0));
  char *inChar = PyUnicode_AsUTF8(PySequence_GetItem(args, 1));
  int inNumChars = PyLong_AsLong(PySequence_GetItem(args, 2));
  
  return PyFloat_FromDouble(XPLMMeasureString(inFontID, inChar, inNumChars));
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


