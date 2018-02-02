#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#define XPLM210

#include <XPLM/XPLMDefs.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPWidgets.h>

PyObject *widgetCallbackDict;


int widgetCallback(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
  PyObject *widget = PyLong_FromVoidPtr(inWidget);
  PyObject *param1 = PyLong_FromVoidPtr((void *)inParam1);
  PyObject *param2 = PyLong_FromVoidPtr((void *)inParam2);

  PyObject *callbackList = PyDict_GetItem(widgetCallbackDict, widget);
  if(callbackList == NULL){
    printf("Couldn't find the callback list for widget ID %p.\n", inWidget);
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
      PyObject *resObj = PyObject_CallFunction(callback, "iOOO", inMessage, widget, param1, param2);
      if(!resObj){
        PyErr_Print();
        break;
      }
      res = PyLong_AsLong(PyNumber_Long(resObj));
      Py_DECREF(resObj);
    }
    if(res != 0){
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
  XPWidgetID inContainer = PyLong_AsVoidPtr(container);
  XPWidgetID res = XPCreateWidget(inLeft, inTop, inRight, inBottom, inVisible, inDescriptor, inIsRoot, inContainer, inClass);
  return PyLong_FromVoidPtr(res);
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
    return NULL;
  }
  XPWidgetID inContainer = PyLong_AsVoidPtr(container);
  XPWidgetID res = XPCreateCustomWidget(inLeft, inTop, inRight, inBottom, inVisible, inDescriptor, inIsRoot,
                                        inContainer, widgetCallback);
  PyObject *resObj = PyLong_FromVoidPtr(res);
  PyObject *callbackList = PyList_New(0);
  PyList_Insert(callbackList, 0, inCallback);
  PyDict_SetItem(widgetCallbackDict, resObj, callbackList);
  return resObj;
}

static PyObject *XPDestroyWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget, *pluginSelf;
  int inDestroyChildren;
  if(!PyArg_ParseTuple(args, "OOi", &pluginSelf, &widget, &inDestroyChildren)){
    return NULL;
  }
  XPDestroyWidget(PyLong_AsVoidPtr(widget), inDestroyChildren);
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
  XPWidgetID inWidget = PyLong_AsVoidPtr(widget);
  intptr_t inParam1 = (intptr_t)PyLong_AsVoidPtr(param1);
  intptr_t inParam2 = (intptr_t)PyLong_AsVoidPtr(param2);

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
  XPPlaceWidgetWithin(PyLong_AsVoidPtr(subWidget), PyLong_AsVoidPtr(container));
  Py_RETURN_NONE;
}

static PyObject *XPCountChildWidgetsFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  int res = XPCountChildWidgets(PyLong_AsVoidPtr(widget));
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
  XPWidgetID res = XPGetNthChildWidget(PyLong_AsVoidPtr(widget), inIndex);
  return PyLong_FromVoidPtr(res);
}

static PyObject *XPGetParentWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPWidgetID res = XPGetParentWidget(PyLong_AsVoidPtr(widget));
  return PyLong_FromVoidPtr(res);
}

static PyObject *XPShowWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPShowWidget(PyLong_AsVoidPtr(widget));
  Py_RETURN_NONE;
}

static PyObject *XPHideWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPHideWidget(PyLong_AsVoidPtr(widget));
  Py_RETURN_NONE;
}

static PyObject *XPIsWidgetVisibleFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  int res = XPIsWidgetVisible(PyLong_AsVoidPtr(widget));
  return(PyLong_FromLong(res));
}

static PyObject *XPFindRootWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPWidgetID res = XPFindRootWidget(PyLong_AsVoidPtr(widget));
  return(PyLong_FromVoidPtr(res));
}

static PyObject *XPBringRootWidgetToFrontFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPBringRootWidgetToFront(PyLong_AsVoidPtr(widget));
  Py_RETURN_NONE;
}

static PyObject *XPIsWidgetInFrontFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  int res = XPIsWidgetInFront(PyLong_AsVoidPtr(widget));
  return(PyLong_FromLong(res));
}

static PyObject *XPGetWidgetGeometryFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget, *left, *top, *right, *bottom;
  if(!PyArg_ParseTuple(args, "OOOOO", &widget, &left, &top, &right, &bottom)){
    return NULL;
  }
  int outLeft, outTop, outRight, outBottom;
  XPGetWidgetGeometry(PyLong_AsVoidPtr(widget), &outLeft, &outTop, &outRight, &outBottom);
  if(left != Py_None){
    PyList_Append(left, PyLong_FromLong(outLeft));
  }
  if(top != Py_None){
    PyList_Append(top, PyLong_FromLong(outTop));
  }
  if(right != Py_None){
    PyList_Append(right, PyLong_FromLong(outRight));
  }
  if(bottom != Py_None){
    PyList_Append(bottom, PyLong_FromLong(outBottom));
  }
  Py_RETURN_NONE;
}

static PyObject *XPSetWidgetGeometryFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  int inLeft, inTop, inRight, inBottom;
  if(!PyArg_ParseTuple(args, "Oiiii", &widget, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  XPSetWidgetGeometry(PyLong_AsVoidPtr(widget), inLeft, inTop, inRight, inBottom);
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
  XPWidgetID res = XPGetWidgetForLocation(PyLong_AsVoidPtr(widget), inLeft, inTop, inRight, inBottom);
  return PyLong_FromVoidPtr(res);
}

static PyObject *XPGetWidgetExposedGeometryFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget, *left, *top, *right, *bottom;
  if(!PyArg_ParseTuple(args, "OOOOO", &widget, &left, &top, &right, &bottom)){
    return NULL;
  }
  int outLeft, outTop, outRight, outBottom;
  XPGetWidgetExposedGeometry(PyLong_AsVoidPtr(widget), &outLeft, &outTop, &outRight, &outBottom);
  if(left != Py_None){
    PyList_Append(left, PyLong_FromLong(outLeft));
  }
  if(top != Py_None){
    PyList_Append(top, PyLong_FromLong(outTop));
  }
  if(right != Py_None){
    PyList_Append(right, PyLong_FromLong(outRight));
  }
  if(bottom != Py_None){
    PyList_Append(bottom, PyLong_FromLong(outBottom));
  }
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
  XPSetWidgetDescriptor(PyLong_AsVoidPtr(widget), inDescriptor);
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
  char *outDescriptor = (char *)malloc(inMaxDescLength + 1);
  int res = XPGetWidgetDescriptor(PyLong_AsVoidPtr(widget), outDescriptor, inMaxDescLength);
  outDescriptor[inMaxDescLength] = '\0';
  PyObject *str = PyUnicode_DecodeUTF8(outDescriptor, strlen(outDescriptor), NULL);
  PyList_Append(descriptor, str);
  Py_DECREF(str);
  free(outDescriptor);
  return PyLong_FromLong(res);
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
  XPSetWidgetProperty(PyLong_AsVoidPtr(widget), inProperty, (intptr_t)PyLong_AsVoidPtr(value));
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
  intptr_t res = XPGetWidgetProperty(PyLong_AsVoidPtr(widget), inProperty, &inExists);
  if(exists != Py_None){
    PyObject *e = PyLong_FromLong(inExists);
    PyList_Append(exists, e);
    Py_DECREF(e);
  }
  PyObject *resObj = PyLong_FromVoidPtr((void *)res);
  return resObj;
}

static PyObject *XPSetKeyboardFocusFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPWidgetID res = XPSetKeyboardFocus(PyLong_AsVoidPtr(widget));
  PyObject *resObj = PyLong_FromVoidPtr((void*)res);
  return resObj;
}

static PyObject *XPLoseKeyboardFocusFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widget;
  if(!PyArg_ParseTuple(args, "O", &widget)){
    return NULL;
  }
  XPLoseKeyboardFocus(PyLong_AsVoidPtr(widget));
  Py_RETURN_NONE;
}

static PyObject *XPGetWidgetWithFocusFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  XPWidgetID res = XPGetWidgetWithFocus();
  PyObject *resObj = PyLong_FromVoidPtr((void*)res);
  return resObj;
}

//Since we have only one callback available, we'll have to handle this
//  ourselves...

static PyObject *XPAddWidgetCallbackFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *pluginSelf, *widget, *callback;
  if(!PyArg_ParseTuple(args, "OOO", &pluginSelf, &widget, &callback)){
    return NULL;
  }
  PyObject *current = PyDict_GetItem(widgetCallbackDict, widget);
  if(current == NULL){
    current = PyList_New(0);
    PyList_Insert(current, 0, callback);
    PyDict_SetItem(widgetCallbackDict, widget, current);
    //register only the first time
    XPAddWidgetCallback(PyLong_AsVoidPtr(widget), widgetCallback);
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
  {"XPSetWidgetProperty", XPSetWidgetPropertyFun, METH_VARARGS, ""},
  {"XPGetWidgetProperty", XPGetWidgetPropertyFun, METH_VARARGS, ""},
  {"XPSetKeyboardFocus", XPSetKeyboardFocusFun, METH_VARARGS, ""},
  {"XPLoseKeyboardFocus", XPLoseKeyboardFocusFun, METH_VARARGS, ""},
  {"XPGetWidgetWithFocus", XPGetWidgetWithFocusFun, METH_VARARGS, ""},
  {"XPAddWidgetCallback", XPAddWidgetCallbackFun, METH_VARARGS, ""},
  {"XPGetWidgetClassFunc", XPGetWidgetClassFuncFun, METH_VARARGS, ""},
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
  PyObject *mod = PyModule_Create(&XPWidgetsModule);
  if(mod){
    //PyModule_AddIntConstant(mod, "", );
  }

  return mod;
}



