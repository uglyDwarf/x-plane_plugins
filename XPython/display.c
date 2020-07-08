#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDisplay.h>
#include "utils.h"
#include "plugin_dl.h"

static PyObject *drawCallbackDict, *drawCallbackIDDict;
static intptr_t drawCallbackCntr;
static PyObject *keySniffCallbackDict;
static intptr_t keySniffCallbackCntr;

//draw, key,mouse, cursor, wheel
static PyObject *windowDict;
static intptr_t hotkeyCntr;
static PyObject *hotkeyDict;
static PyObject *hotkeyIDDict;

static PyObject *monitorBndsCallback;

PyObject *windowIDCapsules;
static PyObject *hotkeyIDCapsules;
const char *windowIDRef = "XPLMWindowIDRef";
static const char hotkeyIDRef[] = "XPLMHotkeyIDRef";


static void receiveMonitorBounds(int inMonitorIndex, int inLeftBx, int inTopBx,
                                       int inRightBx, int inBottomBx, void *refcon)
{
  PyObject *pRes = PyObject_CallFunction(monitorBndsCallback, "(iiiiiO)", inMonitorIndex,
                                         inLeftBx, inTopBx, inRightBx, inBottomBx, (PyObject*)refcon);
  if(!pRes){
    printf("ReceiveMonitorBoundsGlobal callback failed.\n");
    PyObject *err = PyErr_Occurred();
    if(err){
      PyErr_Print();
    }
  }else{
    Py_DECREF(pRes);
  }
}

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
  if(PyTuple_Size(args) == 5){
    if(!PyArg_ParseTuple(args, "OOiiO", &pluginSelf, &callback, &inPhase, &inWantsBefore, &refcon)){
      return NULL;
    }
  }else{
    if(!PyArg_ParseTuple(args, "OiiO", &callback, &inPhase, &inWantsBefore, &refcon)){
      return NULL;
    } 
    pluginSelf = get_pluginSelf(/*PyThreadState_GET()*/);
  }
  PyObject *idx = PyLong_FromLong(++drawCallbackCntr);
  if(!idx){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return NULL;
  }

  PyObject *argObj = Py_BuildValue("(OOiiO)", pluginSelf, callback, inPhase, inWantsBefore, refcon);
  PyDict_SetItem(drawCallbackDict, idx, argObj);
  Py_DECREF(argObj);
  PyObject *tmp = PyLong_FromVoidPtr(refcon);
  PyDict_SetItem(drawCallbackIDDict, tmp, idx);
  Py_DECREF(tmp);
  
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
  if(PyTuple_Size(args) == 4){
    if(!PyArg_ParseTuple(args, "OOiO", &pluginSelf, &callback, &inBeforeWindows, &refcon)){
      return NULL;
    }
  }else{
    if(!PyArg_ParseTuple(args, "OiO", &callback, &inBeforeWindows, &refcon)){
      return NULL;
    }
    pluginSelf = get_pluginSelf(/*PyThreadState_GET()*/);
  }

  PyObject *idx = PyLong_FromLong(++keySniffCallbackCntr);
  if(!idx){
    PyErr_SetString(PyExc_RuntimeError ,"Couldn't create long.\n");
    return NULL;
  }

  PyObject *argObj = Py_BuildValue("(OOiO)", pluginSelf, callback, inBeforeWindows, refcon);
  PyDict_SetItem(keySniffCallbackDict, idx, argObj);
  Py_DECREF(idx);
  Py_DECREF(argObj);
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
  if(PyTuple_Size(args) == 5){
    if(!PyArg_ParseTuple(args, "OOiiO", &pluginSelf, &callback, &inPhase, &inWantsBefore, &refcon)){
      return NULL;
    }
  }else{
    if(!PyArg_ParseTuple(args, "OiiO", &callback, &inPhase, &inWantsBefore, &refcon)){
      return NULL;
    }
  }
  PyObject *pyRefcon = PyLong_FromVoidPtr(refcon);
  PyObject *pID = PyDict_GetItem(drawCallbackIDDict, pyRefcon);
  if(pID == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterDrawCallback failed to find the callback.\n");
    Py_DECREF(pyRefcon);
    return NULL;
  }
  int res = XPLMUnregisterDrawCallback(XPLMDrawCallback, inPhase,
                                       inWantsBefore, PyLong_AsVoidPtr(pID));
  PyDict_DelItem(drawCallbackIDDict, pyRefcon);
  PyDict_DelItem(drawCallbackDict, pID);
  Py_DECREF(pyRefcon);

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
  if(PyTuple_Size(args) == 4){
    if(!PyArg_ParseTuple(args, "OOiO", &pluginSelf, &callback, &inBeforeWindows, &refcon)){
      return NULL;
    }
  }else{
    if(!PyArg_ParseTuple(args, "OiO", &callback, &inBeforeWindows, &refcon)){
      return NULL;
    }
    pluginSelf = get_pluginSelf(/*PyThreadState_GET()*/);
  }
  PyObject *pKey = NULL, *pVal = NULL;
  PyObject *toDelete = NULL;
  Py_ssize_t pos = 0;
  int res = -1;
  PyObject *argObj = Py_BuildValue("OOiO", pluginSelf, callback, inBeforeWindows, refcon);
  while(PyDict_Next(keySniffCallbackDict, &pos, &pKey, &pVal)){
    if(PyObject_RichCompareBool(pVal, argObj, Py_EQ)){
      toDelete = pKey;
      break;
    }
  }
  Py_DECREF(argObj);
  if(toDelete){
    res = XPLMUnregisterKeySniffer(XPLMKeySnifferCallback, 
                                   inBeforeWindows, PyLong_AsVoidPtr(toDelete));
    PyDict_DelItem(keySniffCallbackDict, toDelete);
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
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to drawWindow (%p).\n", inWindowID);
    return;
  }
  PyObject *oRes = PyObject_CallFunctionObjArgs(PyTuple_GetItem(pCbks, 0), pID, inRefcon, NULL);
  if(PyErr_Occurred()) {
    PyErr_Print();
  }
  Py_XDECREF(oRes);
  Py_DECREF(pID);
}

static void handleKey(XPLMWindowID  inWindowID,
               char          inKey,
               XPLMKeyFlags  inFlags,
               char          inVirtualKey,
               void         *inRefcon,
               int           losingFocus)
{
  (void) inRefcon;
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  char msg[2024];
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    if (inWindowID == NULL && losingFocus) {
      /* This occurs only when I have a window with keyboard focus and then
         I destroy the window (or otherwise lose focus.)
         I get callback to handle key but the window ID set zero.
         Verified this happens in simple C program, so it's not Python's fault.
         So far, it appears safe to just ignore.
         Filed with Laminar 18-May-2020 as XPD-10834
      */
      XPLMDebugString("NULL window passed to handleKey. Ignoring\n");
      return;
    }
    sprintf(msg, "Unknown window passed to handleKey (%p) [%ld].\n", inWindowID, (long)inWindowID);
    XPLMDebugString(msg);
    return;
  }
  PyObject *arg1 = PyLong_FromLong(inKey);
  PyObject *arg2 = PyLong_FromLong(inFlags);
  PyObject *arg3 = PyLong_FromLong((unsigned int)inVirtualKey);
  PyObject *arg4 = PyLong_FromLong(losingFocus);
  // printf("Calling handleKey callback. inWindowID = %p, pPID = %s, losingFocus = %d\n", inWindowID, objToStr(pID), losingFocus);
  PyObject *oRes = PyObject_CallFunctionObjArgs(PyTuple_GetItem(pCbks, 2), pID, arg1, arg2, arg3, inRefcon, arg4, NULL);
  Py_XDECREF(arg1);
  Py_XDECREF(arg2);
  Py_XDECREF(arg3);
  Py_XDECREF(arg4);
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
  (void) inRefcon;
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to handleMouseClick (%p).\n", inWindowID);
    return 1;
  }
  PyObject *arg1 = PyLong_FromLong(x);
  PyObject *arg2 = PyLong_FromLong(y);
  PyObject *arg3 = PyLong_FromLong(inMouse);
  PyObject *pRes = PyObject_CallFunctionObjArgs(PyTuple_GetItem(pCbks, 1), pID, arg1, arg2, arg3, inRefcon, NULL);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  Py_DECREF(arg3);
  Py_DECREF(pID);
  if(err){
    PyErr_Print();
    return 1;
  }
  int res = (int)PyLong_AsLong(pRes);
  Py_DECREF(pRes);
  return res;
}

static int handleRightClick(XPLMWindowID     inWindowID,
                     int              x,
                     int              y,
                     XPLMMouseStatus  inMouse,
                     void            *inRefcon)
{
  (void) inRefcon;
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to handleMouseClick (%p).\n", inWindowID);
    return 1;
  }
  PyObject *arg1 = PyLong_FromLong(x);
  PyObject *arg2 = PyLong_FromLong(y);
  PyObject *arg3 = PyLong_FromLong(inMouse);
  PyObject *pRes = PyObject_CallFunctionObjArgs(PyTuple_GetItem(pCbks, 5), pID, arg1, arg2, arg3, inRefcon, NULL);
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  Py_DECREF(arg3);
  Py_DECREF(pID);
  PyObject *err = PyErr_Occurred();
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
  (void) inRefcon;
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to handleCursor (%p).\n", inWindowID);
    return 0;
  }
  PyObject *cbk = PyTuple_GetItem(pCbks, 3);
  if((cbk == NULL) || (cbk == Py_None)){
    return 0;
  }
  PyObject *arg1 = PyLong_FromLong(x);
  PyObject *arg2 = PyLong_FromLong(y);
  PyObject *pRes = PyObject_CallFunctionObjArgs(cbk, pID, arg1, arg2, inRefcon, NULL);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(arg1);
  Py_DECREF(arg2);
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
  (void) inRefcon;
  PyObject *pID = getPtrRef(inWindowID, windowIDCapsules, windowIDRef);
  PyObject *pCbks = PyDict_GetItem(windowDict, pID);
  if(pCbks == NULL){
    printf("Unknown window passed to handleMouseWheel (%p).\n", inWindowID);
    return 1;
  }
  PyObject *cbk = PyTuple_GetItem(pCbks, 4);
  if((cbk == NULL) || (cbk == Py_None)){
    return 1;
  }
  PyObject *arg1 = PyLong_FromLong(x);
  PyObject *arg2 = PyLong_FromLong(y);
  PyObject *arg3 = PyLong_FromLong(wheel);
  PyObject *arg4 = PyLong_FromLong(clicks);
  PyObject *pRes = PyObject_CallFunctionObjArgs(cbk, pID, arg1, arg2, arg3, arg4, inRefcon, NULL);
  PyObject *err = PyErr_Occurred();
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  Py_DECREF(arg3);
  Py_DECREF(arg4);
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
  if(PyTuple_Size(args) == 2){
    if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &paramsObj)){
      return NULL;
    }
  }else{
    if(!PyArg_ParseTuple(args, "O", &paramsObj)){
      return NULL;
    }
  }
  
  XPLMCreateWindow_t params;
  PyObject *drawWindowFunc, *handleMouseClickFunc, *handleKeyFunc,
           *handleCursorFunc, *handleMouseWheelFunc;
  PyObject *handleRightClickFunc;
  params.structSize = sizeof(params);
  PyObject *paramsTuple = PySequence_Tuple(paramsObj);
  params.left = getLongFromTuple(paramsTuple, 0);
  params.top = getLongFromTuple(paramsTuple, 1);
  params.right = getLongFromTuple(paramsTuple, 2);
  params.bottom = getLongFromTuple(paramsTuple, 3);
  params.visible = getLongFromTuple(paramsTuple, 4);
  // PySequence_GetItem returns new reference!
  drawWindowFunc = PyTuple_GetItem(paramsTuple, 5);
  Py_INCREF(drawWindowFunc);
  handleMouseClickFunc = PyTuple_GetItem(paramsTuple, 6);
  Py_INCREF(handleMouseClickFunc);
  handleKeyFunc = PyTuple_GetItem(paramsTuple, 7);
  Py_INCREF(handleKeyFunc);
  handleCursorFunc = PyTuple_GetItem(paramsTuple, 8);
  Py_INCREF(handleCursorFunc);
  handleMouseWheelFunc = PyTuple_GetItem(paramsTuple, 9);
  Py_INCREF(handleMouseWheelFunc);
  params.drawWindowFunc = drawWindow;
  params.handleMouseClickFunc = handleMouseClick;
  params.handleKeyFunc = handleKey;
  params.handleCursorFunc = handleCursor;
  params.handleMouseWheelFunc = handleMouseWheel;
  params.refcon = PyTuple_GetItem(paramsTuple, 10);
  Py_INCREF(params.refcon);
  //SDK 3.0+
  if(PySequence_Length(paramsObj) > 11){
    params.decorateAsFloatingWindow = getLongFromTuple(paramsTuple, 11);
    params.layer = getLongFromTuple(paramsTuple, 12);
    params.handleRightClickFunc = handleRightClick;
    handleRightClickFunc = PyTuple_GetItem(paramsTuple, 13);
  }else{
    handleRightClickFunc = Py_None;
  }
  Py_INCREF(handleRightClickFunc);
  
  PyObject *cbkTuple = Py_BuildValue("(OOOOOO)",
                                     drawWindowFunc, handleMouseClickFunc, handleKeyFunc,
                                     handleCursorFunc, handleMouseWheelFunc, handleRightClickFunc);
  Py_DECREF(paramsTuple);
  if(!cbkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMCreateWindowEx couldn't create a callback tuple.\n");
    return NULL;
  }
  XPLMWindowID id = XPLMCreateWindowEx(&params);
  PyObject *pID = getPtrRef(id, windowIDCapsules, windowIDRef);
  PyDict_SetItem(windowDict, pID, cbkTuple);
  Py_DECREF(cbkTuple);
  return pID;
}

static PyObject *XPLMCreateWindowFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf, *drawCallback, *keyCallback, *mouseCallback, *refcon;
  int left, top, right, bottom, visible;
  if(PyTuple_Size(args) == 10){
    if(!PyArg_ParseTuple(args, "OiiiiiOOOO", &pluginSelf, &left, &top, &right, &bottom, &visible, 
                         &drawCallback, &keyCallback, &mouseCallback, &refcon)){
      return NULL;
    }
  }else{
    if(!PyArg_ParseTuple(args, "iiiiiOOOO", &left, &top, &right, &bottom, &visible, 
                         &drawCallback, &keyCallback, &mouseCallback, &refcon)){
      return NULL;
    }
  }
  PyObject *cbkTuple = Py_BuildValue("(OOOOOO)", drawCallback, mouseCallback, keyCallback, Py_None, Py_None, Py_None);
  if(!cbkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMCreateWindow couldn't create a tuple.\n");
    return NULL;
  }

  Py_INCREF(refcon);
  XPLMWindowID id = XPLMCreateWindow(left, top, right, bottom, visible, drawWindow, handleKey, handleMouseClick, refcon);

  PyObject *pID = getPtrRef(id, windowIDCapsules, windowIDRef);
  PyDict_SetItem(windowDict, pID, cbkTuple);
  Py_DECREF(cbkTuple);
  return pID;
}

static PyObject *XPLMDestroyWindowFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pID, *pluginSelf;
  if(PyTuple_Size(args) == 2){
    if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &pID)){
      return NULL;
    }
  }else{
    if(!PyArg_ParseTuple(args, "O", &pID)){
      return NULL;
    }
  }
  if(PyDict_Contains(windowDict, pID)){
    XPLMWindowID winID = refToPtr(pID, windowIDRef);
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
  PyObject *tmp;
  XPLMGetScreenSize(&w, &h);
  if(PyList_Check(wObj)){
    tmp = PyLong_FromLong(w);
    PyList_Insert(wObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(hObj)){
    tmp = PyLong_FromLong(h);
    PyList_Insert(hObj, 0, tmp);
    Py_DECREF(tmp);
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMGetScreenBoundsGlobalFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *lObj, *tObj, *rObj, *bObj;
  if(!XPLMGetScreenBoundsGlobal_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetScreenBoundsGlobal is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OOOO", &lObj, &tObj, &rObj, &bObj)){
    return NULL;
  }
  int outLeft, outTop, outRight, outBottom;
  XPLMGetScreenBoundsGlobal_ptr(&outLeft, &outTop, &outRight, &outBottom);
  PyObject *tmp;
  if(PyList_Check(lObj)){
    tmp = PyLong_FromLong(outLeft);
    PyList_Insert(lObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(tObj)){
    tmp = PyLong_FromLong(outTop);
    PyList_Insert(tObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(rObj)){
    tmp = PyLong_FromLong(outRight);
    PyList_Insert(rObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(bObj)){
    tmp = PyLong_FromLong(outBottom);
    PyList_Insert(bObj, 0, tmp);
    Py_DECREF(tmp);
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMGetAllMonitorBoundsGlobalFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf, *refconObj;
  if(!XPLMGetAllMonitorBoundsGlobal_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetAllMonitorBoundsGlobal is available only in XPLM300 and up.");
    return NULL;
  }
  if(PyTuple_Size(args) == 3){
    if(!PyArg_ParseTuple(args, "OOO", &pluginSelf, &monitorBndsCallback, &refconObj)){
      return NULL;
    }
  }else{
    if(!PyArg_ParseTuple(args, "OO", &monitorBndsCallback, &refconObj)){
      return NULL;
    }
  }
  XPLMGetAllMonitorBoundsGlobal_ptr(receiveMonitorBounds, (void *)refconObj);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetAllMonitorBoundsOSFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf, *refconObj;
  if(!XPLMGetAllMonitorBoundsOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetAllMonitorBoundsOS is available only in XPLM300 and up.");
    return NULL;
  }
  if(PyTuple_Size(args) == 3){
    if(!PyArg_ParseTuple(args, "OOO", &pluginSelf, &monitorBndsCallback, &refconObj)){
      return NULL;
    }
  }else{
    if(!PyArg_ParseTuple(args, "OO", &monitorBndsCallback, &refconObj)){
      return NULL;
    }
  }
  XPLMGetAllMonitorBoundsOS_ptr(receiveMonitorBounds, (void *)refconObj);
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
  PyObject *tmp;
  if(PyList_Check(xObj)){
    tmp = PyLong_FromLong(x);
    PyList_Insert(xObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(yObj)){
    tmp = PyLong_FromLong(y);
    PyList_Insert(yObj, 0, tmp);
    Py_DECREF(tmp);
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMGetMouseLocationGlobalFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *xObj, *yObj;
  if(!XPLMGetMouseLocationGlobal_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetMouseLocationGlobal is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OO", &xObj, &yObj)){
    return NULL;
  }
  int x, y;
  XPLMGetMouseLocationGlobal_ptr(&x, &y);
  PyObject *tmp;
  if(PyList_Check(xObj)){
    tmp = PyLong_FromLong(x);
    PyList_Insert(xObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(yObj)){
    tmp = PyLong_FromLong(y);
    PyList_Insert(yObj, 0, tmp);
    Py_DECREF(tmp);
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
  XPLMGetWindowGeometry(refToPtr(win, windowIDRef), &left, &top, &right, &bottom);
  PyObject *tmp;
  if(PyList_Check(leftObj)){
    tmp = PyLong_FromLong(left);
    PyList_Insert(leftObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(topObj)){
    tmp = PyLong_FromLong(top);
    PyList_Insert(topObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(rightObj)){
    tmp = PyLong_FromLong(right);
    PyList_Insert(rightObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(bottomObj)){
    tmp = PyLong_FromLong(bottom);
    PyList_Insert(bottomObj, 0, tmp);
    Py_DECREF(tmp);
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
  void *inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowGeometry(inWindowID, inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetWindowGeometryOSFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win, *leftObj, *topObj, *rightObj, *bottomObj;
  if(!XPLMGetWindowGeometryOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetWindowGeometryOS is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OOOOO", &win, &leftObj, &topObj, &rightObj, &bottomObj)){
    return NULL;
  }
  int left, top, right, bottom;
  XPLMGetWindowGeometryOS_ptr(refToPtr(win, windowIDRef), &left, &top, &right, &bottom);
  PyObject *tmp;
  if(PyList_Check(leftObj)){
    tmp = PyLong_FromLong(left);
    PyList_Insert(leftObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(topObj)){
    tmp = PyLong_FromLong(top);
    PyList_Insert(topObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(rightObj)){
    tmp = PyLong_FromLong(right);
    PyList_Insert(rightObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(bottomObj)){
    tmp = PyLong_FromLong(bottom);
    PyList_Insert(bottomObj, 0, tmp);
    Py_DECREF(tmp);
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMSetWindowGeometryOSFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  int inLeft, inTop, inRight, inBottom;
  if(!XPLMSetWindowGeometryOS_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowGeometryOS is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "Oiiii", &win, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  void *inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowGeometryOS_ptr(inWindowID, inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetWindowGeometryVRFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win, *outWidthBoxelsObj, *outHeightBoxelsObj;
  if(!XPLMGetWindowGeometryVR_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMGetWindowGeometryVR is available only in XPLM301 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OOO", &win, &outWidthBoxelsObj, &outHeightBoxelsObj)){
    return NULL;
  }
  int outWidthBoxels, outHeightBoxels;
  XPLMGetWindowGeometryVR_ptr(refToPtr(win, windowIDRef), &outWidthBoxels, &outHeightBoxels);
  PyObject *tmp;
  if(PyList_Check(outWidthBoxelsObj)){
    tmp = PyLong_FromLong(outWidthBoxels);
    PyList_Insert(outWidthBoxelsObj, 0, tmp);
    Py_DECREF(tmp);
  }
  if(PyList_Check(outHeightBoxelsObj)){
    tmp = PyLong_FromLong(outHeightBoxels);
    PyList_Insert(outHeightBoxelsObj, 0, tmp);
    Py_DECREF(tmp);
  }
  Py_RETURN_NONE;
}

static PyObject *XPLMSetWindowGeometryVRFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  int widthBoxels, heightBoxels;
  if(!XPLMSetWindowGeometryVR_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowGeometryVR is available only in XPLM301 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "Oii", &win, &widthBoxels, &heightBoxels)){
    return NULL;
  }
  void *inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowGeometryVR_ptr(inWindowID, widthBoxels, heightBoxels);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetWindowIsVisibleFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTuple(args, "O", &win)){
    return NULL;
  }
  void *inWindowID = refToPtr(win, windowIDRef);
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
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowIsVisible(inWindowID, inIsVisible);
  Py_RETURN_NONE;
}

static PyObject *XPLMWindowIsPoppedOutFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  if(!XPLMWindowIsPoppedOut_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMWindowIsPoppedOut is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "O", &win)){
    return NULL;
  }
  void *inWindowID = refToPtr(win, windowIDRef);
  return PyLong_FromLong(XPLMWindowIsPoppedOut_ptr(inWindowID));
}

static PyObject *XPLMWindowIsInVRFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  if(!XPLMWindowIsInVR_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMWindowIsInVR is available only in XPLM301 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "O", &win)){
    return NULL;
  }
  void *inWindowID = refToPtr(win, windowIDRef);
  return PyLong_FromLong(XPLMWindowIsInVR_ptr(inWindowID));
}

static PyObject *XPLMSetWindowGravityFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  float inLeftGravity, inTopGravity, inRightGravity, inBottomGravity;
  if(!XPLMSetWindowGravity_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowGravity is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "Offff", &win, &inLeftGravity, &inTopGravity, &inRightGravity, &inBottomGravity)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowGravity_ptr(inWindowID, inLeftGravity, inTopGravity, inRightGravity, inBottomGravity);
  Py_RETURN_NONE;
}

static PyObject *XPLMSetWindowResizingLimitsFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  int inMinWidthBoxels, inMinHeightBoxels, inMaxWidthBoxels, inMaxHeightBoxels;
  if(!XPLMSetWindowResizingLimits_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowResizingLimits is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "Oiiii", &win, &inMinWidthBoxels, &inMinHeightBoxels, &inMaxWidthBoxels, &inMaxHeightBoxels)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowResizingLimits_ptr(inWindowID, inMinWidthBoxels, inMinHeightBoxels, inMaxWidthBoxels, inMaxHeightBoxels);
  Py_RETURN_NONE;
}

static PyObject *XPLMSetWindowPositioningModeFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  int inPositioningMode, inMonitorIndex;
  if(!XPLMSetWindowPositioningMode_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowPositioningMode is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "Oii", &win, &inPositioningMode, &inMonitorIndex)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowPositioningMode_ptr(inWindowID, inPositioningMode, inMonitorIndex);
  Py_RETURN_NONE;
}

static PyObject *XPLMSetWindowTitleFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  const char *inWindowTitle;
  if(!XPLMSetWindowTitle_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMSetWindowTitle is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "Os", &win, &inWindowTitle)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
  XPLMSetWindowTitle_ptr(inWindowID, strdup(inWindowTitle));
  Py_RETURN_NONE;
}

static PyObject *XPLMGetWindowRefConFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTuple(args, "O", &win)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
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
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
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
  // use inWindowID 0, if passed in value of 0
  XPLMWindowID inWindowID;
  if (PyLong_Check(win) && PyLong_AsLong(win) == 0) {
    inWindowID = 0;
  } else {
    inWindowID = refToPtr(win, windowIDRef);
  }
  XPLMTakeKeyboardFocus(inWindowID);
  Py_RETURN_NONE;
}

static PyObject *XPLMHasKeyboardFocusFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  if(!XPLMHasKeyboardFocus_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMHasKeyboardFocus is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "O", &win)){
    return NULL;
  }
  // use inWindowID 0, if passed in value of 0
  XPLMWindowID inWindowID;
  if (PyLong_Check(win) && PyLong_AsLong(win) == 0) {
    inWindowID = 0;
  } else {
    inWindowID = refToPtr(win, windowIDRef);
  }
  
  return PyLong_FromLong(XPLMHasKeyboardFocus_ptr(inWindowID));
}

static PyObject *XPLMBringWindowToFrontFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *win;
  if(!PyArg_ParseTuple(args, "O", &win)){
    return NULL;
  }
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
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
  XPLMWindowID inWindowID = refToPtr(win, windowIDRef);
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
  PyObject *res = PyObject_CallFunctionObjArgs(PyTuple_GetItem(pCbk, 0), PyTuple_GetItem(pCbk, 1), NULL);
  PyObject *err = PyErr_Occurred();
  Py_XDECREF(res);  // in case hotkey doesn't happent to return anything
  if(err){
    PyErr_Print();
  }
}

static PyObject *XPLMRegisterHotKeyFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf, *inCallback, *refcon;
  int inVirtualKey, inFlags;
  PyObject *hkTuple;
  const char *inDescription;
  if(PyTuple_Size(args) == 6){
    if(!PyArg_ParseTuple(args, "OiisOO", &pluginSelf, &inVirtualKey, &inFlags, &inDescription, &inCallback, &refcon)){
      return NULL;
    }
    hkTuple = PyTuple_GetSlice(args, 4, 6);
  }else{
    if(!PyArg_ParseTuple(args, "iisOO", &inVirtualKey, &inFlags, &inDescription, &inCallback, &refcon)){
      return NULL;
    }
    hkTuple = PyTuple_GetSlice(args, 3, 5);
  }
  if(!hkTuple){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMRegisterHotKey couldn't create a sequence slice.\n");
    return NULL;
  }

  void *inRefcon = (void *)++hotkeyCntr;
  PyObject *pRefcon = PyLong_FromVoidPtr(inRefcon);
  //Store the callback and original refcon
  PyDict_SetItem(hotkeyDict, pRefcon, hkTuple);

  XPLMHotKeyID id = XPLMRegisterHotKey(inVirtualKey, inFlags, inDescription, hotkeyCallback, inRefcon);
  PyObject *pId = getPtrRef(id, hotkeyIDCapsules, hotkeyIDRef);
  //Allows me to identify my unique refcon based on hotkey id 
  PyDict_SetItem(hotkeyIDDict, pId, pRefcon);
  Py_DECREF(pRefcon);
  return pId;
} 

static PyObject *XPLMUnregisterHotKeyFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf, *hotKey;
  if(PyTuple_Size(args) == 2){
    if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &hotKey)){
      return NULL;
    }
  }else{
    if(!PyArg_ParseTuple(args, "O", &hotKey)){
      return NULL;
    }
  }
  PyObject *pRefcon = PyDict_GetItem(hotkeyIDDict, hotKey);
  if(pRefcon == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterHotKey couldn't find hotkey ID.\n");
    Py_RETURN_NONE;
  }
  PyObject *pCbk = PyDict_GetItem(hotkeyDict, pRefcon);
  if(pCbk == NULL){
    PyErr_SetString(PyExc_RuntimeError ,"XPLMUnregisterHotKey couldn't find refcon.\n");
    Py_RETURN_NONE;
  }
  XPLMHotKeyID *hk = refToPtr(hotKey, hotkeyIDRef);
  XPLMUnregisterHotKey(hk);
  PyDict_DelItem(hotkeyDict, pRefcon);
  PyDict_DelItem(hotkeyIDDict, hotKey);
  removePtrRef(hk, hotkeyIDCapsules);
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
  return getPtrRef(XPLMGetNthHotKey(inIndex), hotkeyIDCapsules, hotkeyIDRef);
} 

static PyObject *XPLMGetHotKeyInfoFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *hotKey, *virtualKey, *flags, *description, *plugin;
  if(!PyArg_ParseTuple(args, "OOOOO", &hotKey, &virtualKey, &flags, &description, &plugin)){
    return NULL;
  }
  XPLMHotKeyID inHotKey = refToPtr(hotKey, hotkeyIDRef);
  char outVirtualKey;
  XPLMKeyFlags outFlags;
  char outDescription[1024];
  XPLMPluginID outPlugin;
  PyObject *tmp;
  XPLMGetHotKeyInfo(inHotKey, &outVirtualKey, &outFlags, outDescription, &outPlugin);
  if(PyList_Check(virtualKey)){
    tmp = PyLong_FromLong((unsigned int)outVirtualKey);
    PyList_Insert(virtualKey, 0, tmp);
    Py_DECREF(tmp);
  }

  if(PyList_Check(flags)){
    tmp = PyLong_FromLong(outFlags);
    PyList_Insert(flags, 0, tmp);
    Py_DECREF(tmp);
  }

  if(PyList_Check(description)){
    PyObject *descStr = PyUnicode_DecodeUTF8(outDescription, strlen(outDescription), NULL);
    if(descStr){
      PyList_Insert(description, 0, descStr);
      Py_DECREF(descStr);
    }
  }
  
  if(PyList_Check(plugin)){
    tmp = PyLong_FromLong(outPlugin);
    PyList_Insert(plugin, 0, tmp);
    Py_DECREF(tmp);
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
  void *inHotkey = refToPtr(hotKey, hotkeyIDRef);
  XPLMSetHotKeyCombination(inHotkey, (char)inVirtualKey, inFlags);
  Py_RETURN_NONE;
} 

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(drawCallbackDict);
  Py_DECREF(drawCallbackDict);
  PyDict_Clear(drawCallbackIDDict);
  Py_DECREF(drawCallbackIDDict);
  PyDict_Clear(keySniffCallbackDict);
  Py_DECREF(keySniffCallbackDict);
  PyDict_Clear(windowDict);
  Py_DECREF(windowDict);
  PyDict_Clear(hotkeyDict);
  Py_DECREF(hotkeyDict);
  PyDict_Clear(hotkeyIDDict);
  Py_DECREF(hotkeyIDDict);
  PyDict_Clear(windowIDCapsules);
  Py_DECREF(windowIDCapsules);
  PyDict_Clear(hotkeyIDCapsules);
  Py_DECREF(hotkeyIDCapsules);
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
  {"XPLMGetScreenBoundsGlobal", XPLMGetScreenBoundsGlobalFun, METH_VARARGS, "Get screen bounds."},
  {"XPLMGetAllMonitorBoundsGlobal", XPLMGetAllMonitorBoundsGlobalFun, METH_VARARGS, "Get monitors bounds."},
  {"XPLMGetAllMonitorBoundsOS", XPLMGetAllMonitorBoundsOSFun, METH_VARARGS, "Get all monitors bounds."},
  {"XPLMGetMouseLocation", XPLMGetMouseLocationFun, METH_VARARGS, "Get mouse location."},
  {"XPLMGetMouseLocationGlobal", XPLMGetMouseLocationGlobalFun, METH_VARARGS, "Get global mouse location."},
  {"XPLMGetWindowGeometry", XPLMGetWindowGeometryFun, METH_VARARGS, "Get window geometry."},
  {"XPLMSetWindowGeometry", XPLMSetWindowGeometryFun, METH_VARARGS, "Set window geometry."},
  {"XPLMGetWindowGeometryOS", XPLMGetWindowGeometryOSFun, METH_VARARGS, "Get window geometry."},
  {"XPLMSetWindowGeometryOS", XPLMSetWindowGeometryOSFun, METH_VARARGS, "Set window geometry."},
  {"XPLMGetWindowIsVisible", XPLMGetWindowIsVisibleFun, METH_VARARGS, "Get window visibility."},
  {"XPLMSetWindowIsVisible", XPLMSetWindowIsVisibleFun, METH_VARARGS, "Set window visibility."},
  {"XPLMWindowIsPoppedOut", XPLMWindowIsPoppedOutFun, METH_VARARGS, ""},
  {"XPLMSetWindowGravity", XPLMSetWindowGravityFun, METH_VARARGS, ""},
  {"XPLMSetWindowResizingLimits", XPLMSetWindowResizingLimitsFun, METH_VARARGS, ""},
  {"XPLMSetWindowPositioningMode", XPLMSetWindowPositioningModeFun, METH_VARARGS, ""},
  {"XPLMSetWindowTitle", XPLMSetWindowTitleFun, METH_VARARGS, ""},
  {"XPLMGetWindowRefCon", XPLMGetWindowRefConFun, METH_VARARGS, "Get window refcon."},
  {"XPLMSetWindowRefCon", XPLMSetWindowRefConFun, METH_VARARGS, "Set window refcon."},
  {"XPLMTakeKeyboardFocus", XPLMTakeKeyboardFocusFun, METH_VARARGS, "Take keyboard focus."},
  {"XPLMHasKeyboardFocus", XPLMHasKeyboardFocusFun, METH_VARARGS, "Check if window has keyboard focus."},
  {"XPLMBringWindowToFront", XPLMBringWindowToFrontFun, METH_VARARGS, "Bring window to front."},
  {"XPLMIsWindowInFront", XPLMIsWindowInFrontFun, METH_VARARGS, "Checks if window is the frontmost visible."},
  {"XPLMRegisterHotKey", XPLMRegisterHotKeyFun, METH_VARARGS, "Register a hotkey."},
  {"XPLMUnregisterHotKey", XPLMUnregisterHotKeyFun, METH_VARARGS, "Unregister a hotkey."},
  {"XPLMCountHotKeys", XPLMCountHotKeysFun, METH_VARARGS, "Return number of hotkeys defined."},
  {"XPLMGetNthHotKey", XPLMGetNthHotKeyFun, METH_VARARGS, "Return Nth hotkey ID."},
  {"XPLMGetHotKeyInfo", XPLMGetHotKeyInfoFun, METH_VARARGS, "Get hotkey info."},
  {"XPLMSetHotKeyCombination", XPLMSetHotKeyCombinationFun, METH_VARARGS, "Set new hotkey key combination."},
  {"XPLMGetWindowGeometryVR", XPLMGetWindowGeometryVRFun, METH_VARARGS, ""},
  {"XPLMSetWindowGeometryVR", XPLMSetWindowGeometryVRFun, METH_VARARGS, ""},
  {"XPLMWindowIsInVR", XPLMWindowIsInVRFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, "cleanup"},
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
  if(!(windowIDCapsules = PyDict_New())){
    return NULL;
  }
  if(!(hotkeyIDCapsules = PyDict_New())){
    return NULL;
  }
  PyObject *mod = PyModule_Create(&XPLMDisplayModule);
  if(mod){
#if defined(XPLM_DEPRECATED)
    /* This is the earliest point at which you can draw in 3-d.                    */
    PyModule_AddIntConstant(mod, "xplm_Phase_FirstScene", xplm_Phase_FirstScene);
    /* Drawing of land and water.                                                  */
    PyModule_AddIntConstant(mod, "xplm_Phase_Terrain", xplm_Phase_Terrain);
    /* Drawing runways and other airport detail.                                   */
    PyModule_AddIntConstant(mod, "xplm_Phase_Airports", xplm_Phase_Airports);
    /* Drawing roads", trails, trains, etc.                                         */
    PyModule_AddIntConstant(mod, "xplm_Phase_Vectors", xplm_Phase_Vectors);
    /* 3-d objects (houses", smokestacks, etc.                                      */
    PyModule_AddIntConstant(mod, "xplm_Phase_Objects", xplm_Phase_Objects);
    /* External views of airplanes", both yours and the AI aircraft.                */
    PyModule_AddIntConstant(mod, "xplm_Phase_Airplanes", xplm_Phase_Airplanes);
    /* This is the last point at which you can draw in 3-d.                        */
    PyModule_AddIntConstant(mod, "xplm_Phase_LastScene", xplm_Phase_LastScene);
#endif /* XPLM_DEPRECATED */
#if defined(XPLM302)
     /* A chance to do modern 3D drawing.                                          */
    PyModule_AddIntConstant(mod, "xplm_Phase_Modern3D", xplm_Phase_Modern3D);
#endif
    /* This is the first phase where you can draw in 2-d.                          */
    PyModule_AddIntConstant(mod, "xplm_Phase_FirstCockpit", xplm_Phase_FirstCockpit);
    /* The non-moving parts of the aircraft panel.                                 */
    PyModule_AddIntConstant(mod, "xplm_Phase_Panel", xplm_Phase_Panel);
    /* The moving parts of the aircraft panel.                                     */
    PyModule_AddIntConstant(mod, "xplm_Phase_Gauges", xplm_Phase_Gauges);
    /* Floating windows from plugins.                                              */
    PyModule_AddIntConstant(mod, "xplm_Phase_Window", xplm_Phase_Window);
    /* The last change to draw in 2d.                                              */
    PyModule_AddIntConstant(mod, "xplm_Phase_LastCockpit", xplm_Phase_LastCockpit);
#if defined(XPLM200)
     /* Removed as of XPLM300; Use the full-blown XPLMMap API instead.             */
    /* 3-d Drawing for the local map.  Use regular OpenGL coordinates to draw in   *
     * this phase.                                                                 */
    PyModule_AddIntConstant(mod, "xplm_Phase_LocalMap3D", xplm_Phase_LocalMap3D);
#endif
#if defined(XPLM200)
     /* Removed as of XPLM300; Use the full-blown XPLMMap API instead.             */
    /* 2-d Drawing of text over the local map.                                     */
    PyModule_AddIntConstant(mod, "xplm_Phase_LocalMap2D", xplm_Phase_LocalMap2D);
#endif
#if defined(XPLM200)
     /* Removed as of XPLM300; Use the full-blown XPLMMap API instead.             */
    /* Drawing of the side-profile view in the local map screen.                   */
    PyModule_AddIntConstant(mod, "xplm_Phase_LocalMapProfile", xplm_Phase_LocalMapProfile);
#endif

    PyModule_AddIntConstant(mod, "xplm_MouseDown", xplm_MouseDown);
    PyModule_AddIntConstant(mod, "xplm_MouseDrag", xplm_MouseDrag);
    PyModule_AddIntConstant(mod, "xplm_MouseUp",   xplm_MouseUp);

    PyModule_AddIntConstant(mod, "xplm_CursorDefault", xplm_CursorDefault);
    PyModule_AddIntConstant(mod, "xplm_CursorHidden",  xplm_CursorHidden);
    PyModule_AddIntConstant(mod, "xplm_CursorArrow",   xplm_CursorArrow);
    PyModule_AddIntConstant(mod, "xplm_CursorCustom",  xplm_CursorCustom);

    PyModule_AddIntConstant(mod, "xplm_WindowLayerFlightOverlay", xplm_WindowLayerFlightOverlay);
    PyModule_AddIntConstant(mod, "xplm_WindowLayerFloatingWindows", xplm_WindowLayerFloatingWindows);
    PyModule_AddIntConstant(mod, "xplm_WindowLayerModal", xplm_WindowLayerModal);
    PyModule_AddIntConstant(mod, "xplm_WindowLayerGrowlNotifications", xplm_WindowLayerGrowlNotifications);
    
    PyModule_AddIntConstant(mod, "xplm_WindowPositionFree", xplm_WindowPositionFree);
    PyModule_AddIntConstant(mod, "xplm_WindowCenterOnMonitor", xplm_WindowCenterOnMonitor);
    PyModule_AddIntConstant(mod, "xplm_WindowFullScreenOnMonitor", xplm_WindowFullScreenOnMonitor);
    PyModule_AddIntConstant(mod, "xplm_WindowFullScreenOnAllMonitors", xplm_WindowFullScreenOnAllMonitors);
    PyModule_AddIntConstant(mod, "xplm_WindowPopOut", xplm_WindowPopOut);
    PyModule_AddIntConstant(mod, "xplm_WindowVR", xplm_WindowVR);

    PyModule_AddIntConstant(mod, "xplm_WindowDecorationNone", xplm_WindowDecorationNone);
    PyModule_AddIntConstant(mod, "xplm_WindowDecorationRoundRectangle", xplm_WindowDecorationRoundRectangle);
    PyModule_AddIntConstant(mod, "xplm_WindowDecorationSelfDecorated", xplm_WindowDecorationSelfDecorated);
    PyModule_AddIntConstant(mod, "xplm_WindowDecorationSelfDecoratedResizable", xplm_WindowDecorationSelfDecoratedResizable);
  }

  return mod;
}

int XPLMDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon)
{
  PyObject *pl = NULL, *fun = NULL, *refcon = NULL, *pRes = NULL, *err = NULL;
  PyObject *tup;
  int res = 1;
  pl = PyLong_FromVoidPtr(inRefcon);
  if(pl == NULL){
    printf("Can't create PyLong.");
    goto cleanup;
  }else{
    tup = PyDict_GetItem(drawCallbackDict, pl);
    Py_DECREF(pl);
  }

  if(!tup){
    printf("Got unknown inRefcon (%p)!", inRefcon);
    goto cleanup;
  }
  fun =    PyTuple_GetItem(tup, 1);
  refcon = PyTuple_GetItem(tup, 4);
  PyObject *inPhaseObj = PyLong_FromLong(inPhase);
  PyObject *inIsBeforeObj = PyLong_FromLong(inIsBefore);
  pRes = PyObject_CallFunctionObjArgs(fun, inPhaseObj, inIsBeforeObj, refcon, NULL);
  Py_DECREF(inPhaseObj);
  Py_DECREF(inIsBeforeObj);
  if(!pRes){
    printf("Draw callback failed.\n");
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    printf("Draw callback returned a wrong type.\n");
    goto cleanup;
  }
  res = (int)PyLong_AsLong(pRes);

 cleanup:
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }

  Py_XDECREF(pRes);
  return res;
}

int XPLMKeySnifferCallback(char inChar, XPLMKeyFlags inFlags, char inVirtualKey, void *inRefcon)
{
  PyObject *pl = NULL, *fun = NULL, *refcon = NULL, *pRes = NULL, *err = NULL;
  PyObject *tup;
  int res = 1;

  pl = PyLong_FromVoidPtr(inRefcon);
  if(pl == NULL){
    printf("Can't create PyLong.");
    goto cleanup;
  }else{
    tup = PyDict_GetItem(keySniffCallbackDict, pl);
    Py_DECREF(pl);
  }

  if(!tup){
    printf("Got unknown inRefcon (%p)!", inRefcon);
    goto cleanup;
  }
  fun =    PyTuple_GetItem(tup, 1);
  PyObject *inCharObj = PyLong_FromLong(inChar);
  PyObject *inFlagsObj = PyLong_FromLong(inFlags);
  PyObject *inVirtualKeyObj = PyLong_FromLong((unsigned int)inVirtualKey);
  refcon = PyTuple_GetItem(tup, 3);
  pRes = PyObject_CallFunctionObjArgs(fun, inCharObj, inFlagsObj, inVirtualKeyObj, refcon, NULL);
  Py_DECREF(inCharObj);
  Py_DECREF(inFlagsObj);
  Py_DECREF(inVirtualKeyObj);
  if(!pRes){
    printf("Key sniffer callback failed.\n");
    goto cleanup;
  }
  if(!PyLong_Check(pRes)){
    printf("Key sniffer callback returned a wrong type.\n");
    goto cleanup;
  }
  res = (int)PyLong_AsLong(pRes);

 cleanup:
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }

  Py_XDECREF(pRes);
  return res;
}



