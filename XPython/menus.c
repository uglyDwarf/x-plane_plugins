#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMMenus.h>
#include "utils.h"

static intptr_t menuCntr;
static PyObject *menuDict;
static PyObject *menuRefDict;

static void menuHandler(void * inMenuRef, void * inItemRef)
{
  PyObject *pID = PyLong_FromVoidPtr(inMenuRef);
  PyObject *menuCallbackInfo = PyDict_GetItem(menuDict, pID);
  Py_XDECREF(pID);
  if(menuCallbackInfo == NULL){
    printf("Unknown callback requested in menuHandler(%p).\n", inMenuRef);
    return;
  }
  PyObject *res = PyObject_CallFunction(PySequence_GetItem(menuCallbackInfo, 4), "(OO)",
                                        PySequence_GetItem(menuCallbackInfo, 5), (PyObject*)inItemRef);
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the menuHandler callback(inMenuRef = %p):\n", inMenuRef);
    PyErr_Print();
  }
  Py_XDECREF(res);
}

static PyObject *XPLMFindPluginsMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  return PyLong_FromVoidPtr(XPLMFindPluginsMenu());
}

static PyObject *XPLMCreateMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *parentMenu = NULL, *handler = NULL, *menuRef = NULL;
  int inParentItem;
  const char *inName;
  if(!PyArg_ParseTuple(args, "OsOiOO", &self, &inName, &parentMenu, &inParentItem, &handler, &menuRef)){
    return NULL;
  }
  void *inMenuRef = (void *)++menuCntr;
  menuRef = PyLong_FromVoidPtr(inMenuRef);
  PyDict_SetItem(menuDict, menuRef, args);
  PyObject *menuID = PyLong_FromVoidPtr(XPLMCreateMenu(inName, PyLong_AsVoidPtr(parentMenu),
                                        inParentItem, menuHandler, inMenuRef));
  PyDict_SetItem(menuRefDict, menuID, menuRef);
  return menuID;
}

static PyObject *XPLMDestroyMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  if(!PyArg_ParseTuple(args, "O", &menuID)){
    return NULL;
  }
  PyObject *menuRef = PyDict_GetItem(menuRefDict, menuID);
  if(!menuRef){
    printf("Unknown menuID passed to XPLMDestroyMenu.\n");
  }
  PyDict_DelItem(menuDict, menuRef);
  PyDict_DelItem(menuRefDict, menuID);
  XPLMDestroyMenu(PyLong_AsVoidPtr(menuID));
  Py_RETURN_NONE;
}

static PyObject *XPLMClearAllMenuItemsFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  if(!PyArg_ParseTuple(args, "O", &menuID)){
    return NULL;
  }
  XPLMClearAllMenuItems(PyLong_AsVoidPtr(menuID));
  Py_RETURN_NONE;
}

static PyObject *XPLMAppendMenuItemFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  const char *inItemName;
  PyObject *inItemRef;
  int inForceEnglish;
  if(!PyArg_ParseTuple(args, "OsOi", &menuID, &inItemName, &inItemRef, &inForceEnglish)){
    return NULL;
  }
  XPLMMenuID inMenu = PyLong_AsVoidPtr(menuID);
  int res = XPLMAppendMenuItem(inMenu, inItemName, inItemRef, inForceEnglish);
  return PyLong_FromLong(res);
}

static PyObject *XPLMAppendMenuSeparatorFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  if(!PyArg_ParseTuple(args, "O", &menuID)){
    return NULL;
  }
  XPLMAppendMenuSeparator(PyLong_AsVoidPtr(menuID));
  Py_RETURN_NONE;
}

static PyObject *XPLMSetMenuItemNameFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  const char *inItemName;
  int inIndex;
  int inForceEnglish;
  if(!PyArg_ParseTuple(args, "Oisi", &menuID, &inIndex, &inItemName, &inForceEnglish)){
    return NULL;
  }
  XPLMMenuID inMenu = PyLong_AsVoidPtr(menuID);
  XPLMSetMenuItemName(inMenu, inIndex, inItemName, inForceEnglish);
  Py_RETURN_NONE;
}

static PyObject *XPLMCheckMenuItemFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  int inIndex;
  int inCheck;
  if(!PyArg_ParseTuple(args, "Oii", &menuID, &inIndex, &inCheck)){
    return NULL;
  }
  XPLMMenuID inMenu = PyLong_AsVoidPtr(menuID);
  XPLMCheckMenuItem(inMenu, inIndex, inCheck);
  Py_RETURN_NONE;
}

static PyObject *XPLMCheckMenuItemStateFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  int inIndex;
  int outCheck;
  if(!PyArg_ParseTuple(args, "Oi", &menuID, &inIndex)){
    return NULL;
  }
  XPLMMenuID inMenu = PyLong_AsVoidPtr(menuID);
  XPLMCheckMenuItemState(inMenu, inIndex, &outCheck);
  return PyLong_FromLong(outCheck);
}

static PyObject *XPLMEnableMenuItemFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  int index;
  int enabled;
  if(!PyArg_ParseTuple(args, "Oii", &menuID, &index, &enabled)){
    return NULL;
  }
  XPLMMenuID inMenu = PyLong_AsVoidPtr(menuID);
  XPLMEnableMenuItem(inMenu, index, enabled);
  Py_RETURN_NONE;
}

static PyObject *XPLMRemoveMenuItemFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  int inIndex;
  if(!PyArg_ParseTuple(args, "Oi", &menuID, &inIndex)){
    return NULL;
  }
  XPLMMenuID inMenu = PyLong_AsVoidPtr(menuID);
  XPLMRemoveMenuItem(inMenu, inIndex);
  Py_RETURN_NONE;
}



static PyMethodDef XPLMMenusMethods[] = {
  {"XPLMFindPluginsMenu", XPLMFindPluginsMenuFun, METH_VARARGS, ""},
  {"XPLMCreateMenu", XPLMCreateMenuFun, METH_VARARGS, ""},
  {"XPLMDestroyMenu", XPLMDestroyMenuFun, METH_VARARGS, ""},
  {"XPLMClearAllMenuItems", XPLMClearAllMenuItemsFun, METH_VARARGS, ""},
  {"XPLMAppendMenuItem", XPLMAppendMenuItemFun, METH_VARARGS, ""},
  {"XPLMAppendMenuSeparator", XPLMAppendMenuSeparatorFun, METH_VARARGS, ""},
  {"XPLMSetMenuItemName", XPLMSetMenuItemNameFun, METH_VARARGS, ""},
  {"XPLMCheckMenuItem", XPLMCheckMenuItemFun, METH_VARARGS, ""},
  {"XPLMCheckMenuItemState", XPLMCheckMenuItemStateFun, METH_VARARGS, ""},
  {"XPLMEnableMenuItem", XPLMEnableMenuItemFun, METH_VARARGS, ""},
  {"XPLMRemoveMenuItem", XPLMRemoveMenuItemFun, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMMenusModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMMenus",
  NULL,
  -1,
  XPLMMenusMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMMenus(void)
{
  if(!(menuDict = PyDict_New())){
    return NULL;
  }
  if(!(menuRefDict = PyDict_New())){
    return NULL;
  }
  PyObject *mod = PyModule_Create(&XPLMMenusModule);
  if(mod){
    PyModule_AddIntConstant(mod, "xplm_Menu_NoCheck", xplm_Menu_NoCheck);
    PyModule_AddIntConstant(mod, "xplm_Menu_Unchecked", xplm_Menu_Unchecked);
    PyModule_AddIntConstant(mod, "xplm_Menu_Checked", xplm_Menu_Checked);

  }

  return mod;
}


