#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDisplay.h>
#include "utils.h"

static PyObject *drawCallbackDict, *drawCallbackIDDict;
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
  PyObject *pluginSelf;
  PyObject *callback;
  int inPhase;
  int inWantsBefore;
  PyObject *refcon;
  if(!PyArg_ParseTuple(args, "OOiiO", &pluginSelf, &callback, &inPhase, &inWantsBefore, &refcon)){
    return NULL;
  }
  PyObject *idx = PyLong_FromLong(++drawCallbackCntr);
  if(!idx){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return NULL;
  }

  PyDict_SetItem(drawCallbackDict, idx, args);
  PyDict_SetItem(drawCallbackIDDict, PyLong_FromVoidPtr(refcon), idx);

  Py_DECREF(idx);
  int res = XPLMRegisterDrawCallback(XPLMDrawCallback, inPhase, inWantsBefore, (void *)drawCallbackCntr);
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
  PyObject *pluginSelf, *callback, *refcon;
  int inBeforeWindows;
  if(!PyArg_ParseTuple(args, "OOiO", &pluginSelf, &callback, &inBeforeWindows, &refcon)){
    return NULL;
  }

  PyObject *idx = PyLong_FromLong(++keySniffCallbackCntr);
  if(!idx){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return NULL;
  }

  PyDict_SetItem(keySniffCallbackDict, idx, args);
  Py_DECREF(idx);
  int res = XPLMRegisterKeySniffer(XPLMKeySnifferCallback, inBeforeWindows, (void *)keySniffCallbackCntr);
  if(!res){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterKeySnifferCallback failed.\n");
    return NULL;
  }
  return PyLong_FromLong(res);
}


static PyObject *XPLMUnregisterDrawCallbackFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf, *callback, *refcon;
  int inPhase, inWantsBefore;
  if(!PyArg_ParseTuple(args, "OOiiO", &pluginSelf, &callback, &inPhase, &inWantsBefore, &refcon)){
    return NULL;
  }
  PyObject *pyRefcon = PyLong_FromVoidPtr(refcon);
  PyObject *pID = PyDict_GetItem(drawCallbackIDDict, pyRefcon);
  if(pID == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterDrawCallback failed to find the callback.\n");
    Py_XDECREF(pyRefcon);
    return NULL;
  }
  PyDict_DelItem(drawCallbackIDDict, pyRefcon);
  PyDict_DelItem(drawCallbackDict, pID);
  Py_XDECREF(pyRefcon);

  int res = XPLMUnregisterDrawCallback(XPLMDrawCallback, inPhase,
                                       inWantsBefore, PyLong_AsVoidPtr(pID));
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the XPLMUnregisterDrawCallback call:\n");
    PyErr_Print();
  }
  return PyLong_FromLong(res);
}

static PyObject *XPLMUnregisterKeySnifferFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf, *callback, *refcon;
  int inBeforeWindows;
  if(!PyArg_ParseTuple(args, "OOiO", &pluginSelf, &callback, &inBeforeWindows, &refcon)){
    return NULL;
  }
  PyObject *pKey = NULL, *pVal = NULL;
  Py_ssize_t pos = 0;
  int res = 0;
  while(PyDict_Next(keySniffCallbackDict, &pos, &pKey, &pVal)){
    if(PyObject_RichCompareBool(pVal, args, Py_EQ)){
      if(PyDict_DelItem(keySniffCallbackDict, pKey) == 0){
        res = XPLMUnregisterKeySniffer(XPLMKeySnifferCallback, 
                                         inBeforeWindows, PyLong_AsVoidPtr(pKey));
      }
      break;
    }
  }

  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the XPLMUnregisterKeySnifferCallback call:\n");
    PyErr_Print();
  }
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
  PyObject_CallFunction(PySequence_GetItem(pCbks, 0), "(OO)", pID, inRefcon);
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
  PyObject *oRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 1), "(OiiiOi)",
                        pID, (int)inKey, inFlags, (unsigned int)inVirtualKey, inRefcon, losingFocus);
  Py_XDECREF(oRes);
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
  PyObject *pRes = PyObject_CallFunction(PySequence_GetItem(pCbks, 2), "(OiiiO)", pID, x, y, inMouse, inRefcon);
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
  PyObject *pRes = PyObject_CallFunction(cbk, "(OiiO)", pID, x, y, inRefcon);
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
  PyObject *pRes = PyObject_CallFunction(cbk, "(OiiiiO)", 
                                         pID, x, y, wheel, clicks, inRefcon);
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
  PyObject *pluginSelf, *paramsObj;
  if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &paramsObj)){
    return NULL;
  }
  
  XPLMCreateWindow_t params;
  params.structSize = sizeof(params);
  params.left = PyLong_AsLong(PyNumber_Long(PySequence_GetItem(paramsObj, 0)));
  params.top = PyLong_AsLong(PyNumber_Long(PySequence_GetItem(paramsObj, 1)));
  params.right = PyLong_AsLong(PyNumber_Long(PySequence_GetItem(paramsObj, 2)));
  params.bottom = PyLong_AsLong(PyNumber_Long(PySequence_GetItem(paramsObj, 3)));
  params.visible = PyLong_AsLong(PyNumber_Long(PySequence_GetItem(paramsObj, 4)));
  params.drawWindowFunc = drawWindow;
  params.handleKeyFunc = handleKey;
  params.handleMouseClickFunc = handleMouseClick;
  params.handleCursorFunc = handleCursor;
  params.handleMouseWheelFunc = handleMouseWheel;
  params.refcon = PySequence_GetItem(paramsObj, 10);

  PyObject *cbkTuple = PySequence_GetSlice(paramsObj, 5, 10);
  if(!cbkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMCreateWindowEx couldn't create a sequence slice.\n");
    return NULL;
  }

  XPLMWindowID id = XPLMCreateWindowEx(&params);
  PyObject *pID = PyLong_FromVoidPtr(id); 
  PyDict_SetItem(windowDict, pID, cbkTuple);
  Py_DECREF(cbkTuple);
  return pID;
}

static PyObject *XPLMCreateWindowFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf, *drawCallback, *keyCallback, *mouseCallback, *refcon;
  int left, top, right, bottom, visible;
  if(!PyArg_ParseTuple(args, "OiiiiiOOOO", &pluginSelf, &left, &top, &right, &bottom, &visible, 
                       &drawCallback, &keyCallback, &mouseCallback, &refcon)){
    return NULL;
  }
  Py_INCREF(refcon);
  PyObject *cbkTuple = Py_BuildValue("OOOOO", drawCallback, keyCallback, mouseCallback, Py_None, Py_None);
  if(!cbkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMCreateWindow couldn't create a tuple.\n");
    return NULL;
  }

  XPLMWindowID id = XPLMCreateWindow(left, top, right, bottom, visible, drawWindow, handleKey, handleMouseClick, refcon);

  PyObject *pID = PyLong_FromVoidPtr(id);
  PyDict_SetItem(windowDict, pID, cbkTuple);
  Py_DECREF(cbkTuple);
  return pID;
}

static PyObject *XPLMDestroyWindowFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pID, *pluginSelf;
  if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &pID)){
    return NULL;
  }
  if(PyDict_Contains(windowDict, pID)){
    XPLMWindowID winID = PyLong_AsVoidPtr(pID);
    PyObject *tmp = XPLMGetWindowRefCon(winID);
    Py_DECREF(tmp);
    XPLMDestroyWindow(winID);
    PyDict_DelItem(windowDict, pID);
  }else{
    PyErr_SetString(PyExc_RuntimeError ,"XPLMDestroyWindow couldn't find the window to destroy.\n");
    return NULL;
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMGetScreenSizeFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *wObj, *hObj;
  if(!PyArg_ParseTuple(args, "OO", &wObj, &hObj)){
    return NULL;
  }
  int w, h;
  XPLMGetScreenSize(&w, &h);
  if(PyList_Check(wObj)){
    PyList_Insert(wObj, 0, PyLong_FromLong(w));
  }
  if(PyList_Check(hObj)){
    PyList_Insert(hObj, 0, PyLong_FromLong(h));
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMGetMouseLocationFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *xObj, *yObj;
  if(!PyArg_ParseTuple(args, "OO", &xObj, &yObj)){
    return NULL;
  }
  int x, y;
  XPLMGetMouseLocation(&x, &y);
  if(PyList_Check(xObj)){
    PyList_Insert(xObj, 0, PyLong_FromLong(x));
  }
  if(PyList_Check(yObj)){
    PyList_Insert(yObj, 0, PyLong_FromLong(y));
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMGetWindowGeometryFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win, *leftObj, *topObj, *rightObj, *bottomObj;
  if(!PyArg_ParseTuple(args, "OOOOO", &win, &leftObj, &topObj, &rightObj, &bottomObj)){
    return NULL;
  }
  int left, top, right, bottom;
  XPLMGetWindowGeometry(PyLong_AsVoidPtr(win), &left, &top, &right, &bottom);
  if(PyList_Check(leftObj)){
    PyList_Insert(leftObj, 0, PyLong_FromLong(left));
  }
  if(PyList_Check(topObj)){
    PyList_Insert(topObj, 0, PyLong_FromLong(top));
  }
  if(PyList_Check(rightObj)){
    PyList_Insert(rightObj, 0, PyLong_FromLong(right));
  }
  if(PyList_Check(bottomObj)){
    PyList_Insert(bottomObj, 0, PyLong_FromLong(bottom));
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMSetWindowGeometryFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  int inLeft, inTop, inRight, inBottom;
  if(!PyArg_ParseTuple(args, "Oiiii", &win, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  void *inWindowID = PyLong_AsVoidPtr(win);
  XPLMSetWindowGeometry(inWindowID, inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetWindowIsVisibleFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTuple(args, "O", &win)){
    return NULL;
  }
  void *inWindowID = PyLong_AsVoidPtr(win);
  return PyLong_FromLong(XPLMGetWindowIsVisible(inWindowID));
}

static PyObject *XPLMSetWindowIsVisibleFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  int inIsVisible;
  if(!PyArg_ParseTuple(args, "Oi", &win, &inIsVisible)){
    return NULL;
  }
  XPLMWindowID inWindowID = PyLong_AsVoidPtr(win);
  XPLMSetWindowIsVisible(inWindowID, inIsVisible);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetWindowRefConFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTuple(args, "O", &win)){
    return NULL;
  }
  XPLMWindowID inWindowID = PyLong_AsVoidPtr(win);
  PyObject *res = XPLMGetWindowRefCon(inWindowID);
  // Needs to be done, because python decrefs it when the function
  //   that called us returns; otherwise all hell breaks loose!!!
  // TODO: Check no other instance of such a problem lingers in the interface!!!
  Py_INCREF(res);
  return res;
}

static PyObject *XPLMSetWindowRefConFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win, *inRefcon;
  if(!PyArg_ParseTuple(args, "OO", &win, &inRefcon)){
    return NULL;
  }
  XPLMWindowID inWindowID = PyLong_AsVoidPtr(win);
  // Decref the old refcon
  PyObject *tmp = XPLMGetWindowRefCon(inWindowID);
  Py_DECREF(tmp);
  // Make sure it stays with us
  Py_INCREF(inRefcon);
  XPLMSetWindowRefCon(inWindowID, inRefcon);
  Py_RETURN_NONE;
}

static PyObject *XPLMTakeKeyboardFocusFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTuple(args, "O", &win)){
    return NULL;
  }
  XPLMWindowID inWindowID = PyLong_AsVoidPtr(win);
  XPLMTakeKeyboardFocus(inWindowID);
  Py_RETURN_NONE;
}

static PyObject *XPLMBringWindowToFrontFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTuple(args, "O", &win)){
    return NULL;
  }
  XPLMWindowID inWindowID = PyLong_AsVoidPtr(win);
  XPLMBringWindowToFront(inWindowID);
  Py_RETURN_NONE;
}

static PyObject *XPLMIsWindowInFrontFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTuple(args, "O", &win)){
    return NULL;
  }
  XPLMWindowID inWindowID = PyLong_AsVoidPtr(win);
  return PyLong_FromLong(XPLMIsWindowInFront(inWindowID));
}

void hotkeyCallback(void *inRefcon)
{
  PyObject *pRefcon = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbk = PyDict_GetItem(hotkeyDict, pRefcon);
  Py_DECREF(pRefcon);
  if(pCbk == NULL){
    printf("Unknown refcon passed to hotkeyCallback (%p).\n", inRefcon);
    return;
  }
  PyObject_CallFunction(PySequence_GetItem(pCbk, 0), "(O)", PySequence_GetItem(pCbk, 1));
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
}

static PyObject *XPLMRegisterHotKeyFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf, *inCallback, *refcon;
  int inVirtualKey, inFlags;
  const char *inDescription;
  if(!PyArg_ParseTuple(args, "OiisOO", &pluginSelf, &inVirtualKey, &inFlags, &inDescription, &inCallback, &refcon)){
    return NULL;
  }
  PyObject *hkTuple = PySequence_GetSlice(args, 4, 6);
  if(!hkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterHotKey couldn't create a sequence slice.\n");
    return NULL;
  }

  void *inRefcon = (void *)++hotkeyCntr;
  PyObject *pRefcon = PyLong_FromVoidPtr(inRefcon);
  //Store the callback and original refcon
  PyDict_SetItem(hotkeyDict, pRefcon, hkTuple);

  XPLMHotKeyID id = XPLMRegisterHotKey(inVirtualKey, inFlags, inDescription, hotkeyCallback, inRefcon);
  PyObject *pId = PyLong_FromVoidPtr(id);
  //Allows me to identify my unique refcon based on hotkey id 
  PyDict_SetItem(hotkeyIDDict, pId, pRefcon);
  return pId;
} 

static PyObject *XPLMUnregisterHotKeyFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf, *hotKey;
  if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &hotKey)){
    return NULL;
  }
  PyObject *pRefcon = PyDict_GetItem(hotkeyIDDict, hotKey);
  if(pRefcon == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterHotKey couldn't find hotkey ID.\n");
    Py_RETURN_NONE;
  }
  PyObject *pCbk = PyDict_GetItem(hotkeyDict, pRefcon);
  if(pCbk == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterHotKey couldn't find refcon.\n");
    Py_DECREF(pRefcon);
    Py_RETURN_NONE;
  }

  XPLMUnregisterHotKey(PyLong_AsVoidPtr(hotKey));
  PyDict_DelItem(hotkeyDict, pRefcon);
  PyDict_DelItem(hotkeyIDDict, hotKey);
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
  int inIndex;
  if(!PyArg_ParseTuple(args, "i", &inIndex)){
    return NULL;
  }
  return PyLong_FromVoidPtr(XPLMGetNthHotKey(inIndex));
} 

static PyObject *XPLMGetHotKeyInfoFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *hotKey, *virtualKey, *flags, *description, *plugin;
  if(!PyArg_ParseTuple(args, "OOOOO", &hotKey, &virtualKey, &flags, &description, &plugin)){
    return NULL;
  }
  XPLMHotKeyID inHotKey = PyLong_AsVoidPtr(hotKey);
  char outVirtualKey;
  XPLMKeyFlags outFlags;
  char outDescription[1024];
  XPLMPluginID outPlugin;
  XPLMGetHotKeyInfo(inHotKey, &outVirtualKey, &outFlags, outDescription, &outPlugin);
  if(PyList_Check(virtualKey)){
    PyList_Insert(virtualKey, 0, PyLong_FromLong((unsigned int)outVirtualKey));
  }

  if(PyList_Check(flags)){
    PyList_Insert(flags, 0, PyLong_FromLong(outFlags));
  }

  PyObject *descStr = PyUnicode_DecodeUTF8(outDescription, strlen(outDescription), NULL);
  if(descStr){
    if(PyList_Check(description)){
      PyList_Insert(description, 0, descStr);
    }
  }
  
  if(PyList_Check(plugin)){
    PyList_Insert(plugin, 0, PyLong_FromLong(outPlugin));
  }

  Py_RETURN_NONE;
} 

static PyObject *XPLMSetHotKeyCombinationFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *hotKey;
  int inVirtualKey;
  int inFlags;
  if(!PyArg_ParseTuple(args, "Oii", &hotKey, &inVirtualKey, &inFlags)){
    return NULL;
  }
  void *inHotkey = PyLong_AsVoidPtr(hotKey);
  XPLMSetHotKeyCombination(inHotkey, (char)inVirtualKey, inFlags);
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
  if(!(drawCallbackIDDict = PyDict_New())){
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



