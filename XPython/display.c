#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDisplay.h>
#include "utils.h"

static PyObject *drawCallbackDict;
static intptr_t drawCallbackCntr;
static PyObject *keySniffCallbackDict;
static intptr_t keySniffCallbackCntr;

//draw, key,mouse, cursor, wheel
static PyObject *windowDict;
static intptr_t hotkeyCntr;
static PyObject *hotkeyDict;
static PyObject *hotkeyIDDict;

int XPLMDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon);
int XPLMKeySnifferCallback(char inChar, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefCon);

static PyObject *XPLMRegisterDrawCallbackFun(PyObject *self, PyObject *args)
//PyObject *inCallback, PyObject *inPhase, PyObject *inWantsBefore, PyObject *inRefcon)
{
  (void) self;

  paramCheck_t paramChecks[] = {
    {1, 'f', "inCallback"},
    {2, 'i', "inPhase"},
    {3, 'i', "inWantsBefore"},
    {4, 'i', "inRefcon"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 5, paramChecks)){
    return NULL;
  }

  PyObject *idx = PyLong_FromLong(++drawCallbackCntr);
  if(!idx){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return NULL;
  }
  PyObject *pSelf = PyTuple_GetItem(args, 0);
  PyObject *pCallback = PyTuple_GetItem(args, 1);
  PyObject *pPhase = PyTuple_GetItem(args, 2);
  PyObject *pWantsBefore = PyTuple_GetItem(args, 3);
  PyObject *pRefCon = PyTuple_GetItem(args, 4);

  PyObject *cbkArgs = Py_BuildValue("(OOOOO)", pSelf, pCallback, pPhase, pWantsBefore, pRefCon);

  PyDict_SetItem(drawCallbackDict, idx, cbkArgs);
  Py_DECREF(idx);
  Py_DECREF(cbkArgs);
  long wantsBefore = PyLong_AsLong(pWantsBefore);
  long phase = PyLong_AsLong(pPhase);
  int res = XPLMRegisterDrawCallback(XPLMDrawCallback, phase, wantsBefore, (void *)drawCallbackCntr);
  if(!res){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterDrawCallback failed.\n");
    return NULL;
  }
  return PyLong_FromLong(res);
}

static PyObject *XPLMRegisterKeySnifferFun(PyObject *self, PyObject *args)
//PyObject *inCallback, PyObject *inBeforeWindows, PyObject *inRefcon)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {1, 'f', "inCallback"},
    {2, 'i', "inBeforeWindows"},
    {3, 'i', "inRefcon"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 4, paramChecks)){
    return NULL;
  }

  PyObject *idx = PyLong_FromLong(++keySniffCallbackCntr);
  if(!idx){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return NULL;
  }
  PyObject *pSelf = PyTuple_GetItem(args, 0);
  PyObject *pCallback = PyTuple_GetItem(args, 1);
  PyObject *pBeforeWindows = PyTuple_GetItem(args, 2);
  PyObject *pRefCon = PyTuple_GetItem(args, 3);

  PyObject *cbkArgs = Py_BuildValue("(OOOO)", pSelf, pCallback, pBeforeWindows, pRefCon);

  PyDict_SetItem(keySniffCallbackDict, idx, cbkArgs);
  Py_DECREF(idx);
  Py_DECREF(cbkArgs);
  long beforeWindows = PyLong_AsLong(pBeforeWindows);
  int res = XPLMRegisterKeySniffer(XPLMKeySnifferCallback, beforeWindows, (void *)keySniffCallbackCntr);
  if(!res){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterKeySnifferCallback failed.\n");
    return NULL;
  }
  return PyLong_FromLong(res);
}


static PyObject *XPLMUnregisterDrawCallbackFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {1, 'f', "inCallback"},
    {2, 'i', "inPhase"},
    {3, 'i', "inWantsBefore"},
    {4, 'i', "inRefcon"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 5, paramChecks)){
    return NULL;
  }

  PyObject *pSelf = PyTuple_GetItem(args, 0);
  PyObject *pCallback = PyTuple_GetItem(args, 1);
  PyObject *pPhase = PyTuple_GetItem(args, 2);
  PyObject *pWantsBefore = PyTuple_GetItem(args, 3);
  PyObject *pRefCon = PyTuple_GetItem(args, 4);
  PyObject *cmpTgt = Py_BuildValue("(OOOOO)", pSelf, pCallback, pPhase, pWantsBefore, pRefCon);
  long res = 0;// returning 0 means problem

  PyObject *pKey = NULL, *pVal = NULL;
  Py_ssize_t pos = 0;

  while(PyDict_Next(drawCallbackDict, &pos, &pKey, &pVal)){
    if(PyObject_RichCompareBool(cmpTgt, pKey, Py_EQ)){
      if(PyDict_DelItem(drawCallbackDict, cmpTgt) == 0){
        res = XPLMUnregisterDrawCallback(XPLMDrawCallback, PyLong_AsLong(pPhase),
                                         PyLong_AsLong(pWantsBefore), PyLong_AsVoidPtr(pKey));
      }
      break;
    }
  }

  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the XPLMUnregisterDrawCallback call:\n");
    PyErr_Print();
  }
  Py_DECREF(cmpTgt);
  return PyLong_FromLong(res);
}

static PyObject *XPLMUnregisterKeySnifferFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {1, 'f', "inCallback"},
    {2, 'i', "inBeforeWindows"},
    {3, 'i', "inRefcon"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 4, paramChecks)){
    return NULL;
  }

  PyObject *pSelf = PyTuple_GetItem(args, 0);
  PyObject *pCallback = PyTuple_GetItem(args, 1);
  PyObject *pBeforeWindows = PyTuple_GetItem(args, 2);
  PyObject *pRefCon = PyTuple_GetItem(args, 3);
  PyObject *cmpTgt = Py_BuildValue("(OOOO)", pSelf, pCallback, pBeforeWindows, pRefCon);
  long res = 0;// returning 0 means problem

  PyObject *pKey = NULL, *pVal = NULL;
  Py_ssize_t pos = 0;

  while(PyDict_Next(keySniffCallbackDict, &pos, &pKey, &pVal)){
    if(PyObject_RichCompareBool(cmpTgt, pKey, Py_EQ)){
      if(PyDict_DelItem(keySniffCallbackDict, cmpTgt) == 0){
        res = XPLMUnregisterKeySniffer(XPLMKeySnifferCallback, 
                                         PyLong_AsLong(pBeforeWindows), PyLong_AsVoidPtr(pKey));
      }
      break;
    }
  }

  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the XPLMUnregisterKeySnifferCallback call:\n");
    PyErr_Print();
  }
  Py_DECREF(cmpTgt);
  return PyLong_FromLong(res);
}


static void drawWindow(XPLMWindowID  inWindowID,
                void         *inRefcon)
{
  PyObject *pID = PyLong_FromVoidPtr(inWindowID);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to drawWindow (%p).\n", inWindowID);
    return;
  }
  PyObject_CallFunction(PySequence_GetItem(pCbks, 0), "(Oi)", pID, (intptr_t)inRefcon);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
}

static void handleKey(XPLMWindowID  inWindowID,
               char          inKey,
               XPLMKeyFlags  inFlags,
               char          inVirtualKey,
               void         *inRefcon,
               int           losingFocus)
{
  PyObject *pID = PyLong_FromVoidPtr(inWindowID);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to handleKey (%p).\n", inWindowID);
    return;
  }
  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 1), "(Oiiiii)",
                        pID, (int)inKey, inFlags, (unsigned int)inVirtualKey, (intptr_t)inRefcon, losingFocus);
  Py_DECREF(oRes);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
}

static int handleMouseClick(XPLMWindowID     inWindowID,
                     int              x,
                     int              y,
                     XPLMMouseStatus  inMouse,
                     void            *inRefcon)
{
  PyObject *pID = PyLong_FromVoidPtr(inWindowID);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to handleMouseClick (%p).\n", inWindowID);
    return 1;
  }
  PyObject *pRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 2), "(Oiiii)", pID, x, y, inMouse, (intptr_t)inRefcon);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(pID);
  if(err){
    PyErr_Print();
    return 1;
  }
  int res = (int)PyLong_AsLong(pRes);
  Py_DECREF(pRes);
  return res;
}

static XPLMCursorStatus handleCursor(XPLMWindowID  inWindowID,
                              int           x,
                              int           y,
                              void         *inRefcon)
{
  PyObject *pID = PyLong_FromVoidPtr(inWindowID);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to handleCursor (%p).\n", inWindowID);
    return 0;
  }
  PyObject *cbk = PySequence_GetItem(pCbks, 3);
  if((cbk == NULL) || (cbk == Py_None)){
    return 0;
  }
  PyObject *pRes = PyObject_CallFunction(cbk, "(Oiii)", pID, x, y, (intptr_t)inRefcon);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(pID);
  if(err){
    PyErr_Print();
    PyErr_Clear();
    return 0;
  }
  int res = (int)PyLong_AsLong(pRes);
  Py_DECREF(pRes);
  return res;
}

static int handleMouseWheel(XPLMWindowID  inWindowID,
                     int           x,
                     int           y,
                     int           wheel,
                     int           clicks,
                     void         *inRefcon)
{
  PyObject *pID = PyLong_FromVoidPtr(inWindowID);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to handleMouseWheel (%p).\n", inWindowID);
    return 1;
  }
  PyObject *cbk = PySequence_GetItem(pCbks, 4);
  if((cbk == NULL) || (cbk == Py_None)){
    return 1;
  }
  PyObject *pRes = PyObject_CallFunction(cbk, "(Oiiiii)", 
                                         pID, x, y, wheel, clicks, (intptr_t)inRefcon);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(pID);
  if(err){
    PyErr_Print();
    PyErr_Clear();
    return 1;
  }
  int res = (int)PyLong_AsLong(pRes);
  Py_DECREF(pRes);
  return res;
}


static PyObject *XPLMCreateWindowExFun(PyObject *self, PyObject *args)
{
  (void) self;
  if(PyTuple_Size(args) != 2){
    PyErr_SetString(PyExc_SyntaxError, "XPLMCreateWindowEx takes two arguments.");
    return NULL;
  }
  PyObject *pParams = PySequence_GetItem(args, 1);
  paramCheck_t paramChecks[] = {
    {0, 'i', "left"},
    {1, 'i', "top"},
    {2, 'i', "right"},
    {3, 'i', "bottom"},
    {4, 'i', "visible"},
    {5, 'f', "drawWindowFunc"},
    {6, 'f', "handleKeyFunc"}, //!!! different from XP SDK!!!
    {7, 'f', "handleMouseClickFunc"},
    {8, 'f', "handleCursorFunc"},
    {9, 'f', "handleMouseWheelFunc"},
    {10, 'i', "refcon"},
    {-1, '\0', NULL}
  };
  if(!checkParams(pParams, 11, paramChecks)){
    return NULL;
  }

  PyObject *cbkTuple = PySequence_GetSlice(pParams, 5, 10);
  if(!cbkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMCreateWindowEx couldn't create a sequence slice.\n");
    return NULL;
  }

  XPLMCreateWindow_t params;
  params.structSize = sizeof(params);
  params.left = PyLong_AsLong(PySequence_GetItem(pParams, 0));
  params.top = PyLong_AsLong(PySequence_GetItem(pParams, 1));
  params.right = PyLong_AsLong(PySequence_GetItem(pParams, 2));
  params.bottom = PyLong_AsLong(PySequence_GetItem(pParams, 3));
  params.visible = PyLong_AsLong(PySequence_GetItem(pParams, 4));
  params.drawWindowFunc = drawWindow;
  params.handleKeyFunc = handleKey;
  params.handleMouseClickFunc = handleMouseClick;
  params.handleCursorFunc = handleCursor;
  params.handleMouseWheelFunc = handleMouseWheel;
  params.refcon = PyLong_AsVoidPtr(PySequence_GetItem(pParams, 10));

  XPLMWindowID id = XPLMCreateWindowEx(&params);

  PyObject *pID = PyLong_FromVoidPtr(id); 
  PyDict_SetItem(windowDict, pID, cbkTuple);
  Py_DECREF(cbkTuple);
  return pID;
}

static PyObject *XPLMCreateWindowFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {1, 'i', "left"},
    {2, 'i', "top"},
    {3, 'i', "right"},
    {4, 'i', "bottom"},
    {5, 'i', "visible"},
    {6, 'f', "inDrawCallback"},
    {7, 'f', "inKeyCallback"},
    {8, 'f', "inMouseCallback"},
    {9, 'i', "refcon"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 10, paramChecks)){
    return NULL;
  }

  PyObject *cbkTuple = Py_BuildValue("OOOss", PySequence_GetItem(args, 6), PySequence_GetItem(args, 7),
                                              PySequence_GetItem(args, 8), NULL, NULL);
  if(!cbkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMCreateWindow couldn't create a tuple.\n");
    return NULL;
  }

  XPLMCreateWindow_t p;
  p.left = PyLong_AsLong(PySequence_GetItem(args, 1));
  p.top = PyLong_AsLong(PySequence_GetItem(args, 2));
  p.right = PyLong_AsLong(PySequence_GetItem(args, 3));
  p.bottom = PyLong_AsLong(PySequence_GetItem(args, 4));
  p.visible = PyLong_AsLong(PySequence_GetItem(args, 5));
  p.drawWindowFunc = drawWindow;
  p.handleKeyFunc = handleKey;
  p.handleMouseClickFunc = handleMouseClick;
  p.refcon = PyLong_AsVoidPtr(PySequence_GetItem(args, 9));

  XPLMWindowID id = XPLMCreateWindow(p.left, p.top, p.right, p.bottom, p.visible, 
                                     p.drawWindowFunc, p.handleKeyFunc, p.handleMouseClickFunc, p.refcon);

  PyObject *pID = PyLong_FromVoidPtr(id);
  PyDict_SetItem(windowDict, pID, cbkTuple);
  Py_DECREF(cbkTuple);
  return pID;
}

static PyObject *XPLMDestroyWindowFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {1, 'i', "WindowID"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 2, paramChecks)){
    return NULL;
  }
  PyObject *pID = PySequence_GetItem(args, 1);
  int res = PyDict_DelItem(windowDict, pID);
  
  if(res == 0){
    XPLMDestroyWindow(PyLong_AsVoidPtr(pID));
  }
  return PyLong_FromLong(res);
}

static PyObject *XPLMGetScreenSizeFun(PyObject *self, PyObject *args)
{
  (void) self;
  if(PySequence_Size(args) != 2){
    PyErr_SetString(PyExc_TypeError , "XPLMGetScreenSize expects two arguments.");
  }
  int w, h;
  XPLMGetScreenSize(&w, &h);
  PyObject *p = PySequence_GetItem(args, 0);
  if(PyList_Check(p)){
    PyList_Insert(p, 0, PyLong_FromLong(w));
  }
  p = PySequence_GetItem(args, 1);
  if(PyList_Check(p)){
    PyList_Insert(p, 0, PyLong_FromLong(h));
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMGetMouseLocationFun(PyObject *self, PyObject *args)
{
  (void) self;
  if(PySequence_Size(args) != 2){
    PyErr_SetString(PyExc_TypeError , "XPLMGetMouseLocation expects two arguments.");
  }
  int x, y;
  XPLMGetMouseLocation(&x, &y);
  PyObject *p = PySequence_GetItem(args, 0);
  if(PyList_Check(p)){
    PyList_Insert(p, 0, PyLong_FromLong(x));
  }
  p = PySequence_GetItem(args, 1);
  if(PyList_Check(p)){
    PyList_Insert(p, 0, PyLong_FromLong(y));
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMGetWindowGeometryFun(PyObject *self, PyObject *args)
{
  (void) self;
  if(PySequence_Size(args) != 5){
    PyErr_SetString(PyExc_TypeError , "XPLMGetWindowGeometry expects five arguments.");
  }
  int left, top, right, bottom;
  XPLMGetWindowGeometry(PyLong_AsVoidPtr(PySequence_GetItem(args, 0)), &left, &top, &right, &bottom);
  PyObject *p = PySequence_GetItem(args, 1);
  if(PyList_Check(p)){
    PyList_Insert(p, 0, PyLong_FromLong(left));
  }
  p = PySequence_GetItem(args, 2);
  if(PyList_Check(p)){
    PyList_Insert(p, 0, PyLong_FromLong(top));
  }
  p = PySequence_GetItem(args, 3);
  if(PyList_Check(p)){
    PyList_Insert(p, 0, PyLong_FromLong(right));
  }
  p = PySequence_GetItem(args, 4);
  if(PyList_Check(p)){
    PyList_Insert(p, 0, PyLong_FromLong(bottom));
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMSetWindowGeometryFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inWindowID"},
    {1, 'i', "inLeft"},
    {2, 'i', "inTop"},
    {3, 'i', "inRight"},
    {4, 'i', "inBottom"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 5, paramChecks)){
    return NULL;
  }
  void *inWindowID = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  int inLeft = PyLong_AsLong(PySequence_GetItem(args, 1));
  int inTop = PyLong_AsLong(PySequence_GetItem(args, 2));
  int inRight = PyLong_AsLong(PySequence_GetItem(args, 3));
  int inBottom = PyLong_AsLong(PySequence_GetItem(args, 4));
  XPLMSetWindowGeometry(inWindowID, inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetWindowIsVisibleFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inWindowID"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  void *inWindowID = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  return PyLong_FromLong(XPLMGetWindowIsVisible(inWindowID));
}

static PyObject *XPLMSetWindowIsVisibleFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inWindowID"},
    {1, 'i', "inIsVisible"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 2, paramChecks)){
    return NULL;
  }
  void *inWindowID = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  int inIsVisible = PyLong_AsLong(PySequence_GetItem(args, 1));
  XPLMSetWindowIsVisible(inWindowID, inIsVisible);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetWindowRefConFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inWindowID"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  void *inWindowID = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  return PyLong_FromVoidPtr(XPLMGetWindowRefCon(inWindowID));
}

static PyObject *XPLMSetWindowRefConFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inWindowID"},
    {1, 'i', "inRefcon"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 2, paramChecks)){
    return NULL;
  }
  void *inWindowID = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  void *inRefcon = PyLong_AsVoidPtr(PySequence_GetItem(args, 1));
  XPLMSetWindowRefCon(inWindowID, inRefcon);
  Py_RETURN_NONE;
}

static PyObject *XPLMTakeKeyboardFocusFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inWindowID"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  void *inWindowID = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  XPLMTakeKeyboardFocus(inWindowID);
  Py_RETURN_NONE;
}

static PyObject *XPLMBringWindowToFrontFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inWindowID"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  void *inWindowID = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  XPLMBringWindowToFront(inWindowID);
  Py_RETURN_NONE;
}

static PyObject *XPLMIsWindowInFrontFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inWindowID"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  void *inWindowID = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  return PyLong_FromLong(XPLMIsWindowInFront(inWindowID));
}

void hotkeyCallback(void *inRefcon)
{
  PyObject *pRefcon = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbk = PyDict_GetItem(hotkeyDict, pRefcon);
  if(pCbk == NULL){
    printf("Unknown refcon passed to hotkeyCallback (%p).\n", inRefcon);
    Py_DECREF(pRefcon);
    return;
  }
  PyObject_CallFunction(PySequence_GetItem(pCbk, 0), "(O)", PySequence_GetItem(pCbk, 1));
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pRefcon);
}

static PyObject *XPLMRegisterHotKeyFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {1, 'i', "inVirtualKey"},
    {2, 'i', "inFlags"},
    {3, 's', "inDescription"},
    {4, 'f', "inCallback"},
    {5, 'i', "inRefcon"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 6, paramChecks)){
    return NULL;
  }
  char inVirtualKey = PyLong_AsLong(PySequence_GetItem(args, 1));
  int inFlags = PyLong_AsLong(PySequence_GetItem(args, 2));
  char *inDescription = PyUnicode_AsUTF8(PySequence_GetItem(args, 3));

  PyObject *hkTuple = PySequence_GetSlice(args, 4, 6);
  if(!hkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterHotKey couldn't create a sequence slice.\n");
    return NULL;
  }

  void *refcon = (void *)++hotkeyCntr;
  PyObject *pRefcon = PyLong_FromVoidPtr(refcon);
  //Store the callback and original refcon
  PyDict_SetItem(hotkeyDict, pRefcon, hkTuple);

  XPLMHotKeyID id = XPLMRegisterHotKey(inVirtualKey, inFlags, inDescription, hotkeyCallback, refcon);
  PyObject *pId = PyLong_FromVoidPtr(id);
  //Allows me to identify my unique refcon based on hotkey id 
  PyDict_SetItem(hotkeyIDDict, pId, pRefcon);
  return pId;
} 

static PyObject *XPLMUnregisterHotKeyFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {1, 'i', "inHotKey"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 2, paramChecks)){
    return NULL;
  }
  PyObject *pId = PySequence_GetItem(args, 1);
  PyObject *pRefcon = PyDict_GetItem(hotkeyIDDict, pId);
  if(pRefcon == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterHotKey couldn't find hotkey ID.\n");
    Py_DECREF(pId);
    Py_RETURN_NONE;
  }
  PyObject *pCbk = PyDict_GetItem(hotkeyDict, pRefcon);
  if(pCbk == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterHotKey couldn't find refcon.\n");
    Py_DECREF(pId);
    Py_DECREF(pRefcon);
    Py_RETURN_NONE;
  }

  XPLMUnregisterHotKey(PyLong_AsVoidPtr(pId));
  PyDict_DelItem(hotkeyDict, pRefcon);
  PyDict_DelItem(hotkeyIDDict, pId);
  Py_DECREF(pId);
  Py_DECREF(pRefcon);
  Py_RETURN_NONE;
} 

static PyObject *XPLMCountHotKeysFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  return PyLong_FromLong(XPLMCountHotKeys());
} 

static PyObject *XPLMGetNthHotKeyFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inIndex"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 1, paramChecks)){
    return NULL;
  }
  int inIndex = (int)PyLong_AsLong(PySequence_GetItem(args, 0));
  return PyLong_FromVoidPtr(XPLMGetNthHotKey(inIndex));
} 

static PyObject *XPLMGetHotKeyInfoFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inHotKey"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 5, paramChecks)){
    return NULL;
  }

  XPLMHotKeyID inHotKey = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  char outVirtualKey;
  XPLMKeyFlags outFlags;
  char outDescription[1024];
  XPLMPluginID outPlugin;
  XPLMGetHotKeyInfo(inHotKey, &outVirtualKey, &outFlags, outDescription, &outPlugin);
  PyObject *p = PySequence_GetItem(args, 1);
  if(PyList_Check(p)){
    PyList_Insert(p, 0, PyLong_FromLong((unsigned int)outVirtualKey));
  }

  p = PySequence_GetItem(args, 2);
  if(PyList_Check(p)){
    PyList_Insert(p, 0, PyLong_FromLong(outFlags));
  }

  PyObject *description = PyUnicode_DecodeUTF8(outDescription, strlen(outDescription), NULL);
  if(description){
    p = PySequence_GetItem(args, 3);
    if(PyList_Check(p)){
      PyList_Insert(p, 0, description);
    }
  }
  
  p = PySequence_GetItem(args, 4);
  if(PyList_Check(p)){
    PyList_Insert(p, 0, PyLong_FromLong(outPlugin));
  }

  Py_RETURN_NONE;
} 

static PyObject *XPLMSetHotKeyCombinationFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 'i', "inHotKey"},
    {1, 'i', "inVirtualKey"},
    {2, 'i', "inFlags"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 3, paramChecks)){
    return NULL;
  }
  void *inHotkey = PyLong_AsVoidPtr(PySequence_GetItem(args, 0));
  char inVirtualKey = PyLong_AsLong(PySequence_GetItem(args, 1));
  int inFlags = PyLong_AsLong(PySequence_GetItem(args, 2));
  XPLMSetHotKeyCombination(inHotkey, inVirtualKey, inFlags);
  Py_RETURN_NONE;
} 



static PyMethodDef XPLMDisplayMethods[] = {
  {"XPLMRegisterDrawCallback", XPLMRegisterDrawCallbackFun, METH_VARARGS, "Register drawing callback."},
  {"XPLMUnregisterDrawCallback", XPLMUnregisterDrawCallbackFun, METH_VARARGS, "Unregister drawing callback."},
  {"XPLMRegisterKeySniffer", XPLMRegisterKeySnifferFun, METH_VARARGS, "Register key sniffer callback."},
  {"XPLMUnregisterKeySniffer", XPLMUnregisterKeySnifferFun, METH_VARARGS, "Unregister key sniffer callback."},
  {"XPLMCreateWindowEx", XPLMCreateWindowExFun, METH_VARARGS, "Create a window (extended interface)."},
  {"XPLMCreateWindow", XPLMCreateWindowFun, METH_VARARGS, "Create a window."},
  {"XPLMDestroyWindow", XPLMDestroyWindowFun, METH_VARARGS, "Destroy a window."},
  {"XPLMGetScreenSize", XPLMGetScreenSizeFun, METH_VARARGS, "Get screen size."},
  {"XPLMGetMouseLocation", XPLMGetMouseLocationFun, METH_VARARGS, "Get mouse location."},
  {"XPLMGetWindowGeometry", XPLMGetWindowGeometryFun, METH_VARARGS, "Get window geometry."},
  {"XPLMSetWindowGeometry", XPLMSetWindowGeometryFun, METH_VARARGS, "Set window geometry."},
  {"XPLMGetWindowIsVisible", XPLMGetWindowIsVisibleFun, METH_VARARGS, "Get window visibility."},
  {"XPLMSetWindowIsVisible", XPLMSetWindowIsVisibleFun, METH_VARARGS, "Set window visibility."},
  {"XPLMGetWindowRefCon", XPLMGetWindowRefConFun, METH_VARARGS, "Get window refcon."},
  {"XPLMSetWindowRefCon", XPLMSetWindowRefConFun, METH_VARARGS, "Set window refcon."},
  {"XPLMTakeKeyboardFocus", XPLMTakeKeyboardFocusFun, METH_VARARGS, "Take keyboard focus."},
  {"XPLMBringWindowToFront", XPLMBringWindowToFrontFun, METH_VARARGS, "Bring window to front."},
  {"XPLMIsWindowInFront", XPLMIsWindowInFrontFun, METH_VARARGS, "Checks if window is the frontmost visible."},
  {"XPLMRegisterHotKey", XPLMRegisterHotKeyFun, METH_VARARGS, "Register a hotkey."},
  {"XPLMUnregisterHotKey", XPLMUnregisterHotKeyFun, METH_VARARGS, "Unregister a hotkey."},
  {"XPLMCountHotKeys", XPLMCountHotKeysFun, METH_VARARGS, "Return number of hotkeys defined."},
  {"XPLMGetNthHotKey", XPLMGetNthHotKeyFun, METH_VARARGS, "Return Nth hotkey ID."},
  {"XPLMGetHotKeyInfo", XPLMGetHotKeyInfoFun, METH_VARARGS, "Get hotkey info."},
  {"XPLMSetHotKeyCombination", XPLMSetHotKeyCombinationFun, METH_VARARGS, "Set new hotkey key combination."},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMDisplayModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMDisplay",
  NULL,
  -1,
  XPLMDisplayMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMDisplay(void)
{
  if(!(drawCallbackDict = PyDict_New())){
    return NULL;
  }
  if(!(keySniffCallbackDict = PyDict_New())){
    return NULL;
  }
  if(!(windowDict = PyDict_New())){
    return NULL;
  }
  if(!(hotkeyDict = PyDict_New())){
    return NULL;
  }
  if(!(hotkeyIDDict = PyDict_New())){
    return NULL;
  }
  
  PyObject *mod = PyModule_Create(&XPLMDisplayModule);
  if(mod){
    /* This is the earliest point at which you can draw in 3-d.                    */
    PyModule_AddIntConstant(mod, "xplm_Phase_FirstScene", 0);
    /* Drawing of land and water.                                                  */
    PyModule_AddIntConstant(mod, "xplm_Phase_Terrain", 5);
    /* Drawing runways and other airport detail.                                   */
    PyModule_AddIntConstant(mod, "xplm_Phase_Airports", 10);
    /* Drawing roads", trails, trains, etc.                                         */
    PyModule_AddIntConstant(mod, "xplm_Phase_Vectors", 15);
    /* 3-d objects (houses", smokestacks, etc.                                      */
    PyModule_AddIntConstant(mod, "xplm_Phase_Objects", 20);
    /* External views of airplanes", both yours and the AI aircraft.                */
    PyModule_AddIntConstant(mod, "xplm_Phase_Airplanes", 25);
    /* This is the last point at which you can draw in 3-d.                        */
    PyModule_AddIntConstant(mod, "xplm_Phase_LastScene", 30);
    /* This is the first phase where you can draw in 2-d.                          */
    PyModule_AddIntConstant(mod, "xplm_Phase_FirstCockpit", 35);
    /* The non-moving parts of the aircraft panel.                                 */
    PyModule_AddIntConstant(mod, "xplm_Phase_Panel", 40);
    /* The moving parts of the aircraft panel.                                     */
    PyModule_AddIntConstant(mod, "xplm_Phase_Gauges", 45);
    /* Floating windows from plugins.                                              */
    PyModule_AddIntConstant(mod, "xplm_Phase_Window", 50);
    /* The last change to draw in 2d.                                              */
    PyModule_AddIntConstant(mod, "xplm_Phase_LastCockpit", 55);
    /* 3-d Drawing for the local map.  Use regular OpenGL coordinates to draw in   *
     * this phase.                                                                 */
    PyModule_AddIntConstant(mod, "xplm_Phase_LocalMap3D", 100);
    /* 2-d Drawing of text over the local map.                                     */
    PyModule_AddIntConstant(mod, "xplm_Phase_LocalMap2D", 101);
    /* Drawing of the side-profile view in the local map screen.                   */
    PyModule_AddIntConstant(mod, "xplm_Phase_LocalMapProfile", 102);

    PyModule_AddIntConstant(mod, "xplm_MouseDown", 1);
    PyModule_AddIntConstant(mod, "xplm_MouseDrag", 2);
    PyModule_AddIntConstant(mod, "xplm_MouseUp",   3);

    PyModule_AddIntConstant(mod, "xplm_CursorDefault", 0);
    PyModule_AddIntConstant(mod, "xplm_CursorHidden",  1);
    PyModule_AddIntConstant(mod, "xplm_CursorArrow",   2);
    PyModule_AddIntConstant(mod, "xplm_CursorCustom",  3);
  }

  return mod;
}

int XPLMDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon)
{
  PyObject *pl = NULL, *fun = NULL, *refcon = NULL, *pRes = NULL, *err = NULL;
  int res = 1;

  pl = PyLong_FromVoidPtr(inRefcon);
  if(pl == NULL){
    printf("Can't create PyLong.");
    goto cleanup;
  }

  if(PyDict_Contains(drawCallbackDict, pl)){
    PyObject *tup = PyDict_GetItem(drawCallbackDict, pl);
    if(!tup){
      printf("Couldn't get dict item.\n");
      goto cleanup;
    }
    PyObject *fun =    PyTuple_GetItem(tup, 1);
    PyObject *refcon = PyTuple_GetItem(tup, 4);
    PyObject *pRes = PyObject_CallFunction(fun, "(iiO)", inPhase, inIsBefore, refcon);
    if(!pRes){
      printf("Draw callback failed.\n");
      goto cleanup;
    }
    if(!PyLong_Check(pRes)){
      printf("Draw callback returned a wrong type.\n");
      goto cleanup;
    }
    res = (int)PyLong_AsLong(pRes);
  }else{
    printf("Got unknown inRefcon (%p)!", inRefcon);
    goto cleanup;
  }

 cleanup:
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }

  Py_XDECREF(pl);
  Py_XDECREF(fun);
  Py_XDECREF(refcon);
  Py_XDECREF(pRes);
  return res;
}

int XPLMKeySnifferCallback(char inChar, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon)
{
  PyObject *pl = NULL, *fun = NULL, *refcon = NULL, *pRes = NULL, *err = NULL;
  int res = 1;

  pl = PyLong_FromVoidPtr(inRefcon);
  if(pl == NULL){
    printf("Can't create PyLong.");
    goto cleanup;
  }

  if(PyDict_Contains(keySniffCallbackDict, pl)){
    PyObject *tup = PyDict_GetItem(keySniffCallbackDict, pl);
    if(!tup){
      printf("Couldn't get dict item.\n");
      goto cleanup;
    }
    PyObject *fun =    PyTuple_GetItem(tup, 1);
    PyObject *refcon = PyTuple_GetItem(tup, 3);
    PyObject *pRes = PyObject_CallFunction(fun, "(iiiO)", inChar, inFlags, (unsigned int)inVirtualKey, refcon);
    if(!pRes){
      printf("Key sniffer callback failed.\n");
      goto cleanup;
    }
    if(!PyLong_Check(pRes)){
      printf("Key sniffer callback returned a wrong type.\n");
      goto cleanup;
    }
    res = (int)PyLong_AsLong(pRes);
  }else{
    printf("Got unknown inRefcon (%p)!", inRefcon);
    goto cleanup;
  }

 cleanup:
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }

  Py_XDECREF(pl);
  Py_XDECREF(fun);
  Py_XDECREF(refcon);
  Py_XDECREF(pRes);
  return res;
}



