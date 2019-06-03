#define _GNU_SOURCE 1
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#define XPLM210

#include <XPLM/XPLMDefs.h>
#include <Widgets/XPWidgetDefs.h>

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyMethodDef XPWidgetDefsMethods[] = {
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPWidgetDefsModule = {
  PyModuleDef_HEAD_INIT,
  "XPWidgetDefs",
  NULL,
  -1,
  XPWidgetDefsMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPWidgetDefs(void)
{
  PyObject *mod = PyModule_Create(&XPWidgetDefsModule);
  if(mod){
    PyModule_AddIntConstant(mod, "xpProperty_Refcon", xpProperty_Refcon);
    PyModule_AddIntConstant(mod, "xpProperty_Dragging", xpProperty_Dragging);
    PyModule_AddIntConstant(mod, "xpProperty_DragXOff", xpProperty_DragXOff);
    PyModule_AddIntConstant(mod, "xpProperty_DragYOff", xpProperty_DragYOff);
    PyModule_AddIntConstant(mod, "xpProperty_Hilited", xpProperty_Hilited);
    PyModule_AddIntConstant(mod, "xpProperty_Object", xpProperty_Object);
    PyModule_AddIntConstant(mod, "xpProperty_Clip", xpProperty_Clip);
    PyModule_AddIntConstant(mod, "xpProperty_Enabled", xpProperty_Enabled);
    PyModule_AddIntConstant(mod, "xpProperty_UserStart", xpProperty_UserStart);

    PyModule_AddIntConstant(mod, "xpMode_Direct", xpMode_Direct);
    PyModule_AddIntConstant(mod, "xpMode_UpChain", xpMode_UpChain);
    PyModule_AddIntConstant(mod, "xpMode_Recursive", xpMode_Recursive);
    PyModule_AddIntConstant(mod, "xpMode_DirectAllCallbacks", xpMode_DirectAllCallbacks);
    PyModule_AddIntConstant(mod, "xpMode_Once", xpMode_Once);

    PyModule_AddIntConstant(mod, "xpWidgetClass_None", xpWidgetClass_None);

    PyModule_AddIntConstant(mod, "xpMsg_None", xpMsg_None);
    PyModule_AddIntConstant(mod, "xpMsg_Create", xpMsg_Create);
    PyModule_AddIntConstant(mod, "xpMsg_Destroy", xpMsg_Destroy);
    PyModule_AddIntConstant(mod, "xpMsg_Paint", xpMsg_Paint);
    PyModule_AddIntConstant(mod, "xpMsg_Draw", xpMsg_Draw);
    PyModule_AddIntConstant(mod, "xpMsg_KeyPress", xpMsg_KeyPress);
    PyModule_AddIntConstant(mod, "xpMsg_KeyTakeFocus", xpMsg_KeyTakeFocus);
    PyModule_AddIntConstant(mod, "xpMsg_KeyLoseFocus", xpMsg_KeyLoseFocus);
    PyModule_AddIntConstant(mod, "xpMsg_MouseDown", xpMsg_MouseDown);
    PyModule_AddIntConstant(mod, "xpMsg_MouseDrag", xpMsg_MouseDrag);
    PyModule_AddIntConstant(mod, "xpMsg_MouseUp", xpMsg_MouseUp);
    PyModule_AddIntConstant(mod, "xpMsg_Reshape", xpMsg_Reshape);
    PyModule_AddIntConstant(mod, "xpMsg_ExposedChanged", xpMsg_ExposedChanged);
    PyModule_AddIntConstant(mod, "xpMsg_AcceptChild", xpMsg_AcceptChild);
    PyModule_AddIntConstant(mod, "xpMsg_LoseChild", xpMsg_LoseChild);
    PyModule_AddIntConstant(mod, "xpMsg_AcceptParent", xpMsg_AcceptParent);
    PyModule_AddIntConstant(mod, "xpMsg_Shown", xpMsg_Shown);
    PyModule_AddIntConstant(mod, "xpMsg_Hidden", xpMsg_Hidden);
    PyModule_AddIntConstant(mod, "xpMsg_DescriptorChanged", xpMsg_DescriptorChanged);
    PyModule_AddIntConstant(mod, "xpMsg_PropertyChanged", xpMsg_PropertyChanged);
    PyModule_AddIntConstant(mod, "xpMsg_MouseWheel", xpMsg_MouseWheel);
    PyModule_AddIntConstant(mod, "xpMsg_CursorAdjust", xpMsg_CursorAdjust);
    PyModule_AddIntConstant(mod, "xpMsg_UserStart", xpMsg_UserStart);
  }
  return mod;
}

