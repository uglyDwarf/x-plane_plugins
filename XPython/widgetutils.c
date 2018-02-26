#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#define XPLM210

#include <XPLM/XPLMDefs.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPWidgetUtils.h>

static PyObject *XPUCreateWidgetsFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *widgetDefs = NULL;
  int inCount;
  PyObject *paramParent, *widgets;
  if(!PyArg_ParseTuple(args, "OiOO", &widgetDefs, &inCount, &paramParent, &widgets)){
    return NULL;
  }
  XPWidgetID inParamParent = PyLong_AsVoidPtr(paramParent);
  XPWidgetID *ioWidgets = malloc(sizeof(XPWidgetID) * inCount);
  XPWidgetCreate_t *defs = malloc(sizeof(XPWidgetCreate_t) * inCount);
  if((defs == NULL) || (ioWidgets == NULL)){
    printf("Out of memory");
    return NULL;
  }
  int i;
  for(i = 0; i < inCount; ++i){
    PyObject *defListItem = PyList_GetItem(widgetDefs, i);
    defs[i].left = PyLong_AsLong(PyList_GetItem(defListItem, 0));
    defs[i].top = PyLong_AsLong(PyList_GetItem(defListItem, 1));
    defs[i].right = PyLong_AsLong(PyList_GetItem(defListItem, 2));
    defs[i].bottom = PyLong_AsLong(PyList_GetItem(defListItem, 3));
    defs[i].visible = PyLong_AsLong(PyList_GetItem(defListItem, 4));
    defs[i].descriptor = PyUnicode_AsUTF8(PyList_GetItem(defListItem, 5));
    defs[i].isRoot = PyLong_AsLong(PyList_GetItem(defListItem, 6));
    defs[i].containerIndex = PyLong_AsLong(PyList_GetItem(defListItem, 7));
    defs[i].widgetClass = PyLong_AsLong(PyList_GetItem(defListItem, 8));
  }
  XPUCreateWidgets(defs, inCount, inParamParent, ioWidgets);

  for(i = 0; i < inCount; ++i){
    PyList_Append(widgets, PyLong_FromVoidPtr(ioWidgets[i]));
  }
  free(ioWidgets);
  free(defs);
  Py_RETURN_NONE;
}

static PyObject *XPUMoveWidgetByFun(PyObject *self, PyObject *args)
{
  (void) self;
  XPWidgetID inWidget;
  int inDeltaX, inDeltaY;
  PyObject *widget = NULL;
  if(!PyArg_ParseTuple(args, "Oii", &widget, &inDeltaX, &inDeltaY)){
    return NULL;
  }
  inWidget = PyLong_AsVoidPtr(widget);
  XPUMoveWidgetBy(inWidget, inDeltaX, inDeltaY);
  Py_RETURN_NONE;
}

static PyObject *XPUFixedLayoutFun(PyObject *self, PyObject *args)
{
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  PyObject *widget = NULL, *param1 = NULL, *param2 = NULL;
  if(!PyArg_ParseTuple(args, "iOOO", &inMessage, &widget, &param1, &param2)){
    return NULL;
  }
  inWidget = PyLong_AsVoidPtr(widget);
  inParam1 = PyLong_AsLong(param1);
  inParam2 = PyLong_AsLong(param2);
 
  int res = XPUFixedLayout(inMessage, inWidget, inParam1, inParam2);
  return PyLong_FromLong(res);
}

static PyObject *XPUSelectIfNeededFun(PyObject *self, PyObject *args)
{
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  int inEatClick;
  PyObject *widget = NULL, *param1 = NULL, *param2 = NULL;
  if(!PyArg_ParseTuple(args, "iOOOi", &inMessage, &widget, &param1, &param2, &inEatClick)){
    return NULL;
  }
  inWidget = PyLong_AsVoidPtr(widget);
  inParam1 = PyLong_AsLong(param1);
  inParam2 = PyLong_AsLong(param2);
 
  int res = XPUSelectIfNeeded(inMessage, inWidget, inParam1, inParam2, inEatClick);
  return PyLong_FromLong(res);
}

static PyObject *XPUDefocusKeyboardFun(PyObject *self, PyObject *args)
{
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  int inEatClick;
  PyObject *widget = NULL, *param1 = NULL, *param2 = NULL;
  if(!PyArg_ParseTuple(args, "iOOOi", &inMessage, &widget, &param1, &param2, &inEatClick)){
    return NULL;
  }
  inWidget = PyLong_AsVoidPtr(widget);
  inParam1 = PyLong_AsLong(param1);
  inParam2 = PyLong_AsLong(param2);
 
  int res = XPUDefocusKeyboard(inMessage, inWidget, inParam1, inParam2, inEatClick);
  return PyLong_FromLong(res);
}

static PyObject *XPUDragWidgetFun(PyObject *self, PyObject *args)
{
  (void) self;
  XPWidgetMessage inMessage;
  XPWidgetID inWidget;
  intptr_t inParam1, inParam2;
  int inLeft, inTop, inRight, inBottom;
  PyObject *widget = NULL, *param1 = NULL, *param2 = NULL;
  if(!PyArg_ParseTuple(args, "iOOOiiii", &inMessage, &widget, &param1, &param2, &inLeft, &inTop, &inRight, &inBottom)){
    return NULL;
  }
  inWidget = PyLong_AsVoidPtr(widget);
  inParam1 = PyLong_AsLong(param1);
  inParam2 = PyLong_AsLong(param2);
 
  int res = XPUDragWidget(inMessage, inWidget, inParam1, inParam2, inLeft, inTop, inRight, inBottom);
  return PyLong_FromLong(res);
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyMethodDef XPWidgetUtilsMethods[] = {
  {"XPUCreateWidgets", XPUCreateWidgetsFun, METH_VARARGS, ""},
  {"XPUMoveWidgetBy", XPUMoveWidgetByFun, METH_VARARGS, ""},
  {"XPUFixedLayout", XPUFixedLayoutFun, METH_VARARGS, ""},
  {"XPUSelectIfNeeded", XPUSelectIfNeededFun, METH_VARARGS, ""},
  {"XPUDefocusKeyboard", XPUDefocusKeyboardFun, METH_VARARGS, ""},
  {"XPUDragWidget", XPUDragWidgetFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};


static struct PyModuleDef XPWidgetUtilsModule = {
  PyModuleDef_HEAD_INIT,
  "XPWidgetUtils",
  NULL,
  -1,
  XPWidgetUtilsMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPWidgetUtils(void)
{
  PyObject *mod = PyModule_Create(&XPWidgetUtilsModule);
  if(mod){
    PyModule_AddIntConstant(mod, "NO_PARENT", NO_PARENT);
    PyModule_AddIntConstant(mod, "PARAM_PARENT", PARAM_PARENT);
  }

  return mod;
}
  
