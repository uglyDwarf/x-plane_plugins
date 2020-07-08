#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMMenus.h>
#include "utils.h"
#include "plugin_dl.h"

static intptr_t menuCntr;
static PyObject *menuDict;
static PyObject *menuRefDict;
static PyObject *menuIDCapsules;

static const char menuIDRef[] = "XPLMMenuIDRef"; 

static void menuHandler(void * inMenuRef, void * inItemRef)
{
  PyObject *pID = PyLong_FromVoidPtr(inMenuRef);
  PyObject *menuCallbackInfo = PyDict_GetItem(menuDict, pID);
  Py_DECREF(pID);
  if(menuCallbackInfo == NULL){
    printf("Unknown callback requested in menuHandler(%p).\n", inMenuRef);
    return;
  }
  PyObject *res = PyObject_CallFunctionObjArgs(PyTuple_GetItem(menuCallbackInfo, 4),
                                        PyTuple_GetItem(menuCallbackInfo, 5), (PyObject*)inItemRef, NULL);
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
  return getPtrRef(XPLMFindPluginsMenu(), menuIDCapsules, menuIDRef);
}

static PyObject *XPLMFindAircraftMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  if(!XPLMFindAircraftMenu_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMFindAircraftMenu is available only in XPLM300 and up.");
    return NULL;
  }
  return getPtrRef(XPLMFindAircraftMenu_ptr(), menuIDCapsules, menuIDRef);
}

static PyObject *XPLMCreateMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *parentMenu = NULL, *pythonHandler = NULL, *menuRef = NULL;
  PyObject *pluginSelf;
  int inParentItem;
  const char *inName;
  if(PyTuple_Size(args) == 6){
    if(!PyArg_ParseTuple(args, "OsOiOO", &pluginSelf, &inName, &parentMenu, &inParentItem, &pythonHandler, &menuRef)){
      return NULL;
    }
  }else{
    if(!PyArg_ParseTuple(args, "sOiOO", &inName, &parentMenu, &inParentItem, &pythonHandler, &menuRef)){
      return NULL;
    }
    pluginSelf = get_pluginSelf(/*PyThreadState_GET()*/);
  }
  PyObject *argsObj = Py_BuildValue( "(OsOiOO)", pluginSelf, inName, parentMenu, inParentItem, pythonHandler, menuRef);

  void *inMenuRef = (void *)++menuCntr;
  menuRef = PyLong_FromVoidPtr(inMenuRef);

  XPLMMenuHandler_f handler = (pythonHandler != Py_None) ? menuHandler : NULL;
  XPLMMenuID rawMenuID = XPLMCreateMenu(inName, refToPtr(parentMenu, menuIDRef),
                                        inParentItem, handler, inMenuRef);
  if(!rawMenuID){
    Py_DECREF(menuRef);
    Py_RETURN_NONE;
  }
  PyObject *menuID = getPtrRef(rawMenuID, menuIDCapsules, menuIDRef);
  PyDict_SetItem(menuDict, menuRef, argsObj);
  Py_DECREF(argsObj);
  PyDict_SetItem(menuRefDict, menuID, menuRef);
  Py_DECREF(menuRef);
  return menuID;
}

static PyObject *XPLMDestroyMenuFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID, *pluginSelf;
  if(PyTuple_Size(args) == 2){
    if(!PyArg_ParseTuple(args, "OO", &pluginSelf, &menuID)){
      return NULL;
    }
  }else{
    if(!PyArg_ParseTuple(args, "O", &menuID)){
      return NULL;
    }
  }
  PyObject *menuRef = PyDict_GetItem(menuRefDict, menuID);
  if(!menuRef){
    printf("Unknown menuID passed to XPLMDestroyMenu.\n");
  }
  PyDict_DelItem(menuDict, menuRef);
  PyDict_DelItem(menuRefDict, menuID);
  XPLMMenuID id = refToPtr(menuID, menuIDRef);
  
  XPLMDestroyMenu(id);
  
  removePtrRef(id, menuIDCapsules);
  
  Py_RETURN_NONE;
}

static PyObject *XPLMClearAllMenuItemsFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  if(!PyArg_ParseTuple(args, "O", &menuID)){
    return NULL;
  }
  XPLMClearAllMenuItems(refToPtr(menuID, menuIDRef));
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
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
  int res = XPLMAppendMenuItem(inMenu, inItemName, inItemRef, inForceEnglish);
  return PyLong_FromLong(res);
}

static PyObject *XPLMAppendMenuItemWithCommandFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  const char *inItemName;
  PyObject *commandToExecute;
  if(!XPLMAppendMenuItemWithCommand_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMAppendMenuItemWithCommand is available only in XPLM300 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "OsO", &menuID, &inItemName, &commandToExecute)){
    return NULL;
  }
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
  XPLMCommandRef inCommandToExecute = (XPLMCommandRef)refToPtr(commandToExecute, commandRefName);
  int res = XPLMAppendMenuItemWithCommand_ptr(inMenu, inItemName, inCommandToExecute);
  return PyLong_FromLong(res);
}

static PyObject *XPLMAppendMenuSeparatorFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  if(!PyArg_ParseTuple(args, "O", &menuID)){
    return NULL;
  }
  XPLMAppendMenuSeparator(refToPtr(menuID, menuIDRef));
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
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
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
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
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
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
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
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
  XPLMEnableMenuItem(inMenu, index, enabled);
  Py_RETURN_NONE;
}

static PyObject *XPLMRemoveMenuItemFun(PyObject *self, PyObject *args)
{
  (void)self;
  PyObject *menuID;
  int inIndex;
  if(!XPLMRemoveMenuItem_ptr){
    PyErr_SetString(PyExc_RuntimeError , "XPLMRemoveMenuItem is available only in XPLM210 and up.");
    return NULL;
  }
  if(!PyArg_ParseTuple(args, "Oi", &menuID, &inIndex)){
    return NULL;
  }
  XPLMMenuID inMenu = refToPtr(menuID, menuIDRef);
  XPLMRemoveMenuItem_ptr(inMenu, inIndex);
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(menuDict);
  Py_DECREF(menuDict);
  PyDict_Clear(menuRefDict);
  Py_DECREF(menuRefDict);
  PyDict_Clear(menuIDCapsules);
  Py_DECREF(menuIDCapsules);
  Py_RETURN_NONE;
}

static PyMethodDef XPLMMenusMethods[] = {
  {"XPLMFindPluginsMenu", XPLMFindPluginsMenuFun, METH_VARARGS, ""},
  {"XPLMFindAircraftMenu", XPLMFindAircraftMenuFun, METH_VARARGS, ""},
  {"XPLMCreateMenu", XPLMCreateMenuFun, METH_VARARGS, ""},
  {"XPLMDestroyMenu", XPLMDestroyMenuFun, METH_VARARGS, ""},
  {"XPLMClearAllMenuItems", XPLMClearAllMenuItemsFun, METH_VARARGS, ""},
  {"XPLMAppendMenuItem", XPLMAppendMenuItemFun, METH_VARARGS, ""},
  {"XPLMAppendMenuItemWithCommand", XPLMAppendMenuItemWithCommandFun, METH_VARARGS, ""},
  {"XPLMAppendMenuSeparator", XPLMAppendMenuSeparatorFun, METH_VARARGS, ""},
  {"XPLMSetMenuItemName", XPLMSetMenuItemNameFun, METH_VARARGS, ""},
  {"XPLMCheckMenuItem", XPLMCheckMenuItemFun, METH_VARARGS, ""},
  {"XPLMCheckMenuItemState", XPLMCheckMenuItemStateFun, METH_VARARGS, ""},
  {"XPLMEnableMenuItem", XPLMEnableMenuItemFun, METH_VARARGS, ""},
  {"XPLMRemoveMenuItem", XPLMRemoveMenuItemFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
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
  if(!(menuIDCapsules = PyDict_New())){
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


