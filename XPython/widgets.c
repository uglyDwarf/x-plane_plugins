#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>

#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDisplay.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPWidgets.h>
#include <Widgets/XPStandardWidgets.h>
#include "plugin_dl.h"
#include "utils.h"

PyObject *widgetCallbackDict;
PyObject *widgetIDCapsules;

int widgetCallback(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
  PyObject *widget = getPtrRef(inWidget, widgetIDCapsules, widgetRefName);
  PyObject *param1, *param2;
  XPKeyState_t *keyState;
  XPMouseState_t *mouseState;
  XPWidgetGeometryChange_t *wChange;
  param1 = PyLong_FromLong(inParam1);
  param2 = PyLong_FromLong(inParam2);
  switch(inMessage){
  case xpMsg_KeyPress:
    keyState = (XPKeyState_t *)inParam1;
    param1 = Py_BuildValue("(iii)", (int)keyState->key, (int)keyState->flags,
                           (int)keyState->vkey);
    break;
  case xpMsg_MouseDown:
  case xpMsg_MouseDrag:
  case xpMsg_MouseUp:
  case xpMsg_MouseWheel:
  case xpMsg_CursorAdjust:
    mouseState = (XPMouseState_t *)inParam1;
    param1 = Py_BuildValue("(iiii)", mouseState->x, mouseState->y,
                           mouseState->button, mouseState->delta);
    break;
  case xpMsg_Reshape:
    wChange = (XPWidgetGeometryChange_t *)inParam1;
    param1 = Py_BuildValue("(iiii)", wChange->dx, wChange->dy,
                           wChange->dwidth, wChange->dheight);
    break;
  case xpMsg_AcceptChild:
  case xpMsg_LoseChild:
  case xpMsg_AcceptParent:
  case xpMsg_Shown:
  case xpMsg_Hidden:
  case xpMsg_TextFieldChanged:
    param1 =  getPtrRef((void *)inParam1, widgetIDCapsules, widgetRefName);
    break;
    
  default: // intentionally empty
    break;
  }

  PyObject *callbackList = PyDict_GetItem(widgetCallbackDict, widget);
  if(callbackList == NULL){
    /* we'll get an xpMsg_Create that we can't handle from a CustomWidget (because the widgetCallbackDict
       isn't populated yet). Ignore the message (CreateCustomWidget() below will send it again!)
       If not xpMsg_Create, write error.
     */
    if (inMessage != xpMsg_Create) {
      printf("Couldn't find the callback list for widget ID %p. for message %d\n", inWidget, inMessage);
    }
    Py_DECREF(widget);
    Py_DECREF(param1);
    Py_DECREF(param2);
    return 0;
  }

  Py_ssize_t i;
  int res;
  PyObject *callback;
  for(i = 0; i < PyList_Size(callbackList); ++i){
    callback = PyList_GetItem(callbackList, i);
    //Have to differentiate between python callbacks and "binary" function callbacks
    // (like the ones returned by XPGetWidgetClassFunc)
    if(PyLong_Check(callback)){
      XPWidgetFunc_t cFunc = (XPWidgetFunc_t)PyLong_AsVoidPtr(callback);
      res = cFunc(inMessage, inWidget, inParam1, inParam2);
    }else{
      PyObject *inMessageObj = PyLong_FromLong(inMessage);
      PyObject *resObj = PyObject_CallFunctionObjArgs(callback, inMessageObj, widget, param1, param2, NULL);
      Py_DECREF(inMessageObj);
      if(!resObj){
        PyErr_Print();
        break;
      }
      res = PyLong_AsLong(resObj);
      Py_DECREF(resObj);
    }
    if(res != 0){
      if(inMessage == xpMsg_CursorAdjust){
        *(XPLMCursorStatus *)inParam2 = (int)PyLong_AsLong(param2);
      }
      break;
    }
  }

  if(inMessage == xpMsg_Destroy){
    PyDict_DelItem(widgetCallbackDict, widget);
  }

  Py_DECREF(widget);
  Py_DECREF(param1);
  Py_DECREF(param2);
  return res;
}



static PyObject *XPCreateWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inLeft, inTop, inRight, inBottom, inVisible, inIsRoot;
  const char *inDescriptor;
  PyObject *container;
  XPWidgetClass inClass;
  if(!PyArg_ParseTuple(args, "iiiiisiOi", &inLeft, &inTop, &inRight, &inBottom, &inVisible, &inDescriptor, &inIsRoot,
                                         &container, &inClass)){
    return NULL;
  }
  // use inContainer 0, if passed in value of 0
  XPWidgetID inContainer;
  if ((PyLong_Check(container) && PyLong_AsLong(container) == 0) || container == Py_None) {
    inContainer = 0;
  } else {
    inContainer = refToPtr(container, widgetRefName);
  }

  XPWidgetID res = XPCreateWidget(inLeft, inTop, inRight, inBottom, inVisible, inDescriptor, inIsRoot, inContainer, inClass);
  return getPtrRef(res, widgetIDCapsules, widgetRefName);
}

static PyObject *XPCreateCustomWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf;
  int inLeft, inTop, inRight, inBottom, inVisible, inIsRoot;
  const char *inDescriptor;
  PyObject *container;
  PyObject *inCallback;
  if(!PyArg_ParseTuple(args, "OiiiiisiOO", &pluginSelf, &inLeft, &inTop, &inRight, &inBottom, &inVisible, &inDescriptor,
                       &inIsRoot, &container, &inCallback)){
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "iiiiisiOO", &inLeft, &inTop, &inRight, &inBottom, &inVisible, &inDescriptor,
                         &inIsRoot, &container, &inCallback)){
      return NULL;
    }
  }
  // use inContainer 0, if passed in value of 0
  XPWidgetID inContainer;
  if ((PyLong_Check(container) && PyLong_AsLong(container) == 0) || container == Py_None) {
    inContainer = 0;
  } else {
    inContainer = refToPtr(container, widgetRefName);
  }

  /* vvvvvvvvvvvvvvvvvv widgetCallback will be immediately called with Create msg BUT
       widgetCallbackDict does not yet have entry for this widget, so the create msg
       will not be received by this function. 
       So... we populate the dict and then call SendMessageToWidget directly!
   */
  XPWidgetID res = XPCreateCustomWidget(inLeft, inTop, inRight, inBottom, inVisible, inDescriptor, inIsRoot,
                                        inContainer, widgetCallback);
  PyObject *resObj = getPtrRef(res, widgetIDCapsules, widgetRefName);
  PyObject *callbackList = PyList_New(0);
  PyList_Insert(callbackList, 0, inCallback);
  PyDict_SetItem(widgetCallbackDict, resObj, callbackList);
  XPSendMessageToWidget(res, xpMsg_Create, xpMode_Direct, 0, 0);
  return resObj;
}

static PyObject *XPDestroyWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget, *pluginSelf;
  int inDestroyChildren;
  if(!PyArg_ParseTuple(args, "OOi", &pluginSelf, &widget, &inDestroyChildren)){
    PyErr_Clear();
    if(!PyArg_ParseTuple(args, "Oi", &widget, &inDestroyChildren)){
      return NULL;
    }
  }
  XPWidgetID wid = refToPtr(widget, widgetRefName);
  XPDestroyWidget(wid, inDestroyChildren);
  PyObject *w = PyDict_GetItem(widgetCallbackDict, widget);
  if(w){
    PyDict_DelItem(widgetCallbackDict, widget);
  }
  removePtrRef(wid, widgetIDCapsules);
  Py_RETURN_NONE;
}

static PyObject *XPSendMessageToWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget, *param1, *param2;
  int inMessage, inMode;
  if(!PyArg_ParseTuple(args, "OiiOO", &widget, &inMessage, &inMode, &param1, &param2)){
    return NULL;
  }
  XPWidgetID inWidget = refToPtr(widget, widgetRefName);
  intptr_t inParam1;
  if (PyCapsule_CheckExact(param1)) {
    printf("param one is a capsule with name %s\n", PyCapsule_GetName(param1));
    inParam1 = (intptr_t) PyCapsule_GetPointer(param1, PyCapsule_GetName(param1));
  } else {
    inParam1 = PyLong_AsLong(param1);
  }

  intptr_t inParam2;
  if (PyCapsule_CheckExact(param2)) {
    printf("param one is a capsule with name %s\n", PyCapsule_GetName(param2));
    inParam2 = (intptr_t) PyCapsule_GetPointer(param1, PyCapsule_GetName(param2));
  } else {
    inParam2 = PyLong_AsLong(param2);
  }

  int res = XPSendMessageToWidget(inWidget, inMessage, inMode, inParam1, inParam2);
  return PyLong_FromLong(res);
}

static PyObject *XPPlaceWidgetWithinFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *subWidget, *container;
  if(!PyArg_ParseTuple(args, "OO", &subWidget, &container)){
    return NULL;
  }
  XPPlaceWidgetWithin(refToPtr(subWidget, widgetRefName), refToPtr(container, widgetRefName));
  Py_RETURN_NONE;
}

static PyObject *XPCountChildWidgetsFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  int res = XPCountChildWidgets(refToPtr(widget, widgetRefName));
  return PyLong_FromLong(res);
}

static PyObject *XPGetNthChildWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  int inIndex;
  if(!PyArg_ParseTuple(args, "Oi", &widget, &inIndex)){
    return NULL;
  }
  XPWidgetID res = XPGetNthChildWidget(refToPtr(widget, widgetRefName), inIndex);
  return getPtrRef(res, widgetIDCapsules, widgetRefName);
}

static PyObject *XPGetParentWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPWidgetID res = XPGetParentWidget(refToPtr(widget, widgetRefName));
  return getPtrRef(res, widgetIDCapsules, widgetRefName);
}

static PyObject *XPShowWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPShowWidget(refToPtr(widget, widgetRefName));
  Py_RETURN_NONE;
}

static PyObject *XPHideWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPHideWidget(refToPtr(widget, widgetRefName));
  Py_RETURN_NONE;
}

static PyObject *XPIsWidgetVisibleFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  int res = XPIsWidgetVisible(refToPtr(widget, widgetRefName));
  return(PyLong_FromLong(res));
}

static PyObject *XPFindRootWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPWidgetID res = XPFindRootWidget(refToPtr(widget, widgetRefName));
  return(getPtrRef(res, widgetIDCapsules, widgetRefName));
}

static PyObject *XPBringRootWidgetToFrontFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPBringRootWidgetToFront(refToPtr(widget, widgetRefName));
  Py_RETURN_NONE;
}

static PyObject *XPIsWidgetInFrontFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  int res = XPIsWidgetInFront(refToPtr(widget, widgetRefName));
  return(PyLong_FromLong(res));
}

static PyObject *XPGetWidgetGeometryFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget, *left, *top, *right, *bottom;
  bool lists;
  if(PyTuple_Size(args) > 1){
    lists = true;
    if(!PyArg_ParseTuple(args, "OOOOO", &widget, &left, &top, &right, &bottom)){
      return NULL;
    }
  }else{
    lists = false;
    if(!PyArg_ParseTuple(args, "O", &widget)){
      return NULL;
    }
  }
  int outLeft, outTop, outRight, outBottom;
  XPGetWidgetGeometry(refToPtr(widget, widgetRefName), &outLeft, &outTop, &outRight, &outBottom);
  if(lists){
      objToList(PyLong_FromLong(outLeft), left);
      objToList(PyLong_FromLong(outTop), top);
      objToList(PyLong_FromLong(outRight), right);
      objToList(PyLong_FromLong(outBottom), bottom);
      Py_RETURN_NONE;
  }else{
    PyObject *res = PyList_New(4);
    PyList_SetItem(res, 0, PyLong_FromLong(outLeft));
    PyList_SetItem(res, 1, PyLong_FromLong(outTop));
    PyList_SetItem(res, 2, PyLong_FromLong(outRight));
    PyList_SetItem(res, 3, PyLong_FromLong(outBottom));
    return res;
  }
}

static PyObject *XPSetWidgetGeometryFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  int inLeft, inTop, inRight, inBottom;
  if(!PyArg_ParseTuple(args, "Oiiii", &widget, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  XPSetWidgetGeometry(refToPtr(widget, widgetRefName), inLeft, inTop, inRight, inBottom);
  Py_RETURN_NONE;
}

static PyObject *XPGetWidgetForLocationFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  int inLeft, inTop, inRight, inBottom;
  if(!PyArg_ParseTuple(args, "Oiiii", &widget, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  XPWidgetID res = XPGetWidgetForLocation(refToPtr(widget, widgetRefName), inLeft, inTop, inRight, inBottom);
  return getPtrRef(res, widgetIDCapsules, widgetRefName);
}

static PyObject *XPGetWidgetExposedGeometryFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget, *left, *top, *right, *bottom;
  if(!PyArg_ParseTuple(args, "OOOOO", &widget, &left, &top, &right, &bottom)){
    return NULL;
  }
  int outLeft, outTop, outRight, outBottom;
  XPGetWidgetExposedGeometry(refToPtr(widget, widgetRefName), &outLeft, &outTop, &outRight, &outBottom);
  objToList(PyLong_FromLong(outLeft), left);
  objToList(PyLong_FromLong(outTop), top);
  objToList(PyLong_FromLong(outRight), right);
  objToList(PyLong_FromLong(outBottom), bottom);
  Py_RETURN_NONE;
}

static PyObject *XPSetWidgetDescriptorFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  const char *inDescriptor;
  if(!PyArg_ParseTuple(args, "Os", &widget, &inDescriptor)){
    return NULL;
  }
  XPSetWidgetDescriptor(refToPtr(widget, widgetRefName), inDescriptor);
  Py_RETURN_NONE;
}

static PyObject *XPGetWidgetDescriptorFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget, *descriptor;
  int inMaxDescLength;
  if(!PyArg_ParseTuple(args, "OOi", &widget, &descriptor, &inMaxDescLength)){
    return NULL;
  }
  int res;
  if(descriptor != Py_None){
    char *outDescriptor = (char *)malloc(inMaxDescLength + 1);
    res = XPGetWidgetDescriptor(refToPtr(widget, widgetRefName), outDescriptor, inMaxDescLength);
    outDescriptor[inMaxDescLength] = '\0';
    PyObject *str = PyUnicode_DecodeUTF8(outDescriptor, strlen(outDescriptor), NULL);
    PyList_Append(descriptor, str);
    Py_DECREF(str);
    free(outDescriptor);
  }else{
    res = XPGetWidgetDescriptor(refToPtr(widget, widgetRefName), NULL, inMaxDescLength);
  }
  return PyLong_FromLong(res);
}

static PyObject *XPGetWidgetUnderlyingWindowFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *widget;
  if(!XPGetWidgetUnderlyingWindow_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPGetWidgetUnderlyingWindow is available only in XPLM301 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPLMWindowID res = XPGetWidgetUnderlyingWindow_ptr(refToPtr(widget, widgetRefName));
  return getPtrRef(res, windowIDCapsules, windowIDRef);
}

static PyObject *XPSetWidgetPropertyFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget, *value;
  int property;
  if(!PyArg_ParseTuple(args, "OiO", &widget, &property, &value)){
    return NULL;
  }
  XPWidgetPropertyID inProperty = property;
  XPSetWidgetProperty(refToPtr(widget, widgetRefName), inProperty, PyLong_AsLong(value));
  Py_RETURN_NONE;
}

static PyObject *XPGetWidgetPropertyFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *widget, *exists;
  int property;
  if(!PyArg_ParseTuple(args, "OiO", &widget, &property, &exists)){
    return NULL;
  }
  XPWidgetPropertyID inProperty = property;
  int inExists;
  intptr_t res = XPGetWidgetProperty(refToPtr(widget, widgetRefName), inProperty, &inExists);
  if(exists != Py_None){
    PyObject *e = PyLong_FromLong(inExists);
    PyList_Append(exists, e);
    Py_DECREF(e);
  }
  PyObject *resObj = PyLong_FromLong(res);
  return resObj;
}

static PyObject *XPSetKeyboardFocusFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPWidgetID res = XPSetKeyboardFocus(refToPtr(widget, widgetRefName));
  PyObject *resObj = getPtrRef(res, widgetIDCapsules, widgetRefName);
  return resObj;
}

static PyObject *XPLoseKeyboardFocusFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPLoseKeyboardFocus(refToPtr(widget, widgetRefName));
  Py_RETURN_NONE;
}

static PyObject *XPGetWidgetWithFocusFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  XPWidgetID res = XPGetWidgetWithFocus();
  PyObject *resObj = getPtrRef(res, widgetIDCapsules, widgetRefName);
  return resObj;
}

//Since we have only one callback available, we'll have to handle this
//  ourselves...

static PyObject *XPAddWidgetCallbackFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf, *widget, *callback;
  if(!PyArg_ParseTuple(args, "OOO", &pluginSelf, &widget, &callback)){
    PyErr_Clear();
    if (!PyArg_ParseTuple(args, "OO", &widget, &callback)){
      return NULL;
    }
  }
  PyObject *current = PyDict_GetItem(widgetCallbackDict, widget);
  if(current == NULL){
    current = PyList_New(0);
    PyList_Append(current, callback);
    PyDict_SetItem(widgetCallbackDict, widget, current);
    //register only the first time
    XPAddWidgetCallback(refToPtr(widget, widgetRefName), widgetCallback);
  }else{
    PyList_Insert(current, 0, callback);
  }
  Py_RETURN_NONE;
}

static PyObject *XPGetWidgetClassFuncFun(PyObject *self, PyObject *args)
{
  (void)self;
  int inWidgetClass; 
  if(!PyArg_ParseTuple(args, "i", &inWidgetClass)){
    return NULL;
  }
  XPWidgetFunc_t res = XPGetWidgetClassFunc(inWidgetClass);
  return PyLong_FromVoidPtr(res);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(widgetCallbackDict);
  Py_DECREF(widgetCallbackDict);
  PyDict_Clear(widgetIDCapsules);
  Py_DECREF(widgetIDCapsules);
  Py_RETURN_NONE;
}

static PyMethodDef XPWidgetsMethods[] = {
  {"XPCreateWidget", XPCreateWidgetFun, METH_VARARGS, ""},
  {"XPCreateCustomWidget", XPCreateCustomWidgetFun, METH_VARARGS, ""},
  {"XPDestroyWidget", XPDestroyWidgetFun, METH_VARARGS, ""},
  {"XPSendMessageToWidget", XPSendMessageToWidgetFun, METH_VARARGS, ""},
  {"XPPlaceWidgetWithin", XPPlaceWidgetWithinFun, METH_VARARGS, ""},
  {"XPCountChildWidgets", XPCountChildWidgetsFun, METH_VARARGS, ""},
  {"XPGetNthChildWidget", XPGetNthChildWidgetFun, METH_VARARGS, ""},
  {"XPGetParentWidget", XPGetParentWidgetFun, METH_VARARGS, ""},
  {"XPShowWidget", XPShowWidgetFun, METH_VARARGS, ""},
  {"XPHideWidget", XPHideWidgetFun, METH_VARARGS, ""},
  {"XPIsWidgetVisible", XPIsWidgetVisibleFun, METH_VARARGS, ""},
  {"XPFindRootWidget", XPFindRootWidgetFun, METH_VARARGS, ""},
  {"XPBringRootWidgetToFront", XPBringRootWidgetToFrontFun, METH_VARARGS, ""},
  {"XPIsWidgetInFront", XPIsWidgetInFrontFun, METH_VARARGS, ""},
  {"XPGetWidgetGeometry", XPGetWidgetGeometryFun, METH_VARARGS, ""},
  {"XPSetWidgetGeometry", XPSetWidgetGeometryFun, METH_VARARGS, ""},
  {"XPGetWidgetForLocation", XPGetWidgetForLocationFun, METH_VARARGS, ""},
  {"XPGetWidgetExposedGeometry", XPGetWidgetExposedGeometryFun, METH_VARARGS, ""},
  {"XPSetWidgetDescriptor", XPSetWidgetDescriptorFun, METH_VARARGS, ""},
  {"XPGetWidgetDescriptor", XPGetWidgetDescriptorFun, METH_VARARGS, ""},
  {"XPGetWidgetUnderlyingWindow", XPGetWidgetUnderlyingWindowFun, METH_VARARGS, ""},
  {"XPSetWidgetProperty", XPSetWidgetPropertyFun, METH_VARARGS, ""},
  {"XPGetWidgetProperty", XPGetWidgetPropertyFun, METH_VARARGS, ""},
  {"XPSetKeyboardFocus", XPSetKeyboardFocusFun, METH_VARARGS, ""},
  {"XPLoseKeyboardFocus", XPLoseKeyboardFocusFun, METH_VARARGS, ""},
  {"XPGetWidgetWithFocus", XPGetWidgetWithFocusFun, METH_VARARGS, ""},
  {"XPAddWidgetCallback", XPAddWidgetCallbackFun, METH_VARARGS, ""},
  {"XPGetWidgetClassFunc", XPGetWidgetClassFuncFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPWidgetsModule = {
  PyModuleDef_HEAD_INIT,
  "XPWidgets",
  NULL,
  -1,
  XPWidgetsMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPWidgets(void)
{
  if(!(widgetCallbackDict = PyDict_New())){
    return NULL;
  }
  if(!(widgetIDCapsules = PyDict_New())){
    return NULL;
  }
  PyObject *mod = PyModule_Create(&XPWidgetsModule);
  if(mod){
    //PyModule_AddIntConstant(mod, "", );
  }

  return mod;
}



