//Python comes first!
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <XPLM/XPLMDefs.h>

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyMethodDef XPLMDefsMethods[] = {
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};



static struct PyModuleDef XPLMDefsModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMDefs",
  NULL,
  -1,
  XPLMDefsMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMDefs(void)
{
  PyObject *mod = PyModule_Create(&XPLMDefsModule);
  if(mod != NULL){
    PyModule_AddIntConstant(mod, "XPLM_NO_PLUGIN_ID", -1);
    PyModule_AddIntConstant(mod, "XPLM_PLUGIN_XPLANE", 0);
    PyModule_AddIntConstant(mod, "kXPLM_Version", 210);

    PyModule_AddIntConstant(mod, "xplm_ShiftFlag", 1);
    PyModule_AddIntConstant(mod, "xplm_OptionAltFlag", 2);
    PyModule_AddIntConstant(mod, "xplm_ControlFlag", 4);
    PyModule_AddIntConstant(mod, "xplm_DownFlag", 8);
    PyModule_AddIntConstant(mod, "xplm_UpFlag", 16);

    PyModule_AddIntConstant(mod, "XPLM_KEY_RETURN", 13);
    PyModule_AddIntConstant(mod, "XPLM_KEY_ESCAPE", 27);
    PyModule_AddIntConstant(mod, "XPLM_KEY_TAB", 9);
    PyModule_AddIntConstant(mod, "XPLM_KEY_DELETE", 8);
    PyModule_AddIntConstant(mod, "XPLM_KEY_LEFT", 28);
    PyModule_AddIntConstant(mod, "XPLM_KEY_RIGHT", 29);
    PyModule_AddIntConstant(mod, "XPLM_KEY_UP", 30);
    PyModule_AddIntConstant(mod, "XPLM_KEY_DOWN", 31);
    PyModule_AddIntConstant(mod, "XPLM_KEY_0", 48);
    PyModule_AddIntConstant(mod, "XPLM_KEY_1", 49);
    PyModule_AddIntConstant(mod, "XPLM_KEY_2", 50);
    PyModule_AddIntConstant(mod, "XPLM_KEY_3", 51);
    PyModule_AddIntConstant(mod, "XPLM_KEY_4", 52);
    PyModule_AddIntConstant(mod, "XPLM_KEY_5", 53);
    PyModule_AddIntConstant(mod, "XPLM_KEY_6", 54);
    PyModule_AddIntConstant(mod, "XPLM_KEY_7", 55);
    PyModule_AddIntConstant(mod, "XPLM_KEY_8", 56);
    PyModule_AddIntConstant(mod, "XPLM_KEY_9", 57);
    PyModule_AddIntConstant(mod, "XPLM_KEY_DECIMAL", 46);

    PyModule_AddIntConstant(mod, "XPLM_VK_BACK", 0x08);
    PyModule_AddIntConstant(mod, "XPLM_VK_TAB", 0x09);
    PyModule_AddIntConstant(mod, "XPLM_VK_CLEAR", 0x0C);
    PyModule_AddIntConstant(mod, "XPLM_VK_RETURN", 0x0D);
    PyModule_AddIntConstant(mod, "XPLM_VK_ESCAPE", 0x1B);
    PyModule_AddIntConstant(mod, "XPLM_VK_SPACE", 0x20);
    PyModule_AddIntConstant(mod, "XPLM_VK_PRIOR", 0x21);
    PyModule_AddIntConstant(mod, "XPLM_VK_NEXT", 0x22);
    PyModule_AddIntConstant(mod, "XPLM_VK_END", 0x23);
    PyModule_AddIntConstant(mod, "XPLM_VK_HOME", 0x24);
    PyModule_AddIntConstant(mod, "XPLM_VK_LEFT", 0x25);
    PyModule_AddIntConstant(mod, "XPLM_VK_UP", 0x26);
    PyModule_AddIntConstant(mod, "XPLM_VK_RIGHT", 0x27);
    PyModule_AddIntConstant(mod, "XPLM_VK_DOWN", 0x28);
    PyModule_AddIntConstant(mod, "XPLM_VK_SELECT", 0x29);
    PyModule_AddIntConstant(mod, "XPLM_VK_PRINT", 0x2A);
    PyModule_AddIntConstant(mod, "XPLM_VK_EXECUTE", 0x2B);
    PyModule_AddIntConstant(mod, "XPLM_VK_SNAPSHOT", 0x2C);
    PyModule_AddIntConstant(mod, "XPLM_VK_INSERT", 0x2D);
    PyModule_AddIntConstant(mod, "XPLM_VK_DELETE", 0x2E);
    PyModule_AddIntConstant(mod, "XPLM_VK_HELP", 0x2F);
/* XPLM_VK_0 thru XPLM_VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39)   */
    PyModule_AddIntConstant(mod, "XPLM_VK_0", 0x30);
    PyModule_AddIntConstant(mod, "XPLM_VK_1", 0x31);
    PyModule_AddIntConstant(mod, "XPLM_VK_2", 0x32);
    PyModule_AddIntConstant(mod, "XPLM_VK_3", 0x33);
    PyModule_AddIntConstant(mod, "XPLM_VK_4", 0x34);
    PyModule_AddIntConstant(mod, "XPLM_VK_5", 0x35);
    PyModule_AddIntConstant(mod, "XPLM_VK_6", 0x36);
    PyModule_AddIntConstant(mod, "XPLM_VK_7", 0x37);
    PyModule_AddIntConstant(mod, "XPLM_VK_8", 0x38);
    PyModule_AddIntConstant(mod, "XPLM_VK_9", 0x39);
/* XPLM_VK_A thru XPLM_VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A)   */
    PyModule_AddIntConstant(mod, "XPLM_VK_A", 0x41);
    PyModule_AddIntConstant(mod, "XPLM_VK_B", 0x42);
    PyModule_AddIntConstant(mod, "XPLM_VK_C", 0x43);
    PyModule_AddIntConstant(mod, "XPLM_VK_D", 0x44);
    PyModule_AddIntConstant(mod, "XPLM_VK_E", 0x45);
    PyModule_AddIntConstant(mod, "XPLM_VK_F", 0x46);
    PyModule_AddIntConstant(mod, "XPLM_VK_G", 0x47);
    PyModule_AddIntConstant(mod, "XPLM_VK_H", 0x48);
    PyModule_AddIntConstant(mod, "XPLM_VK_I", 0x49);
    PyModule_AddIntConstant(mod, "XPLM_VK_J", 0x4A);
    PyModule_AddIntConstant(mod, "XPLM_VK_K", 0x4B);
    PyModule_AddIntConstant(mod, "XPLM_VK_L", 0x4C);
    PyModule_AddIntConstant(mod, "XPLM_VK_M", 0x4D);
    PyModule_AddIntConstant(mod, "XPLM_VK_N", 0x4E);
    PyModule_AddIntConstant(mod, "XPLM_VK_O", 0x4F);
    PyModule_AddIntConstant(mod, "XPLM_VK_P", 0x50);
    PyModule_AddIntConstant(mod, "XPLM_VK_Q", 0x51);
    PyModule_AddIntConstant(mod, "XPLM_VK_R", 0x52);
    PyModule_AddIntConstant(mod, "XPLM_VK_S", 0x53);
    PyModule_AddIntConstant(mod, "XPLM_VK_T", 0x54);
    PyModule_AddIntConstant(mod, "XPLM_VK_U", 0x55);
    PyModule_AddIntConstant(mod, "XPLM_VK_V", 0x56);
    PyModule_AddIntConstant(mod, "XPLM_VK_W", 0x57);
    PyModule_AddIntConstant(mod, "XPLM_VK_X", 0x58);
    PyModule_AddIntConstant(mod, "XPLM_VK_Y", 0x59);
    PyModule_AddIntConstant(mod, "XPLM_VK_Z", 0x5A);
    PyModule_AddIntConstant(mod, "XPLM_VK_NUMPAD0", 0x60);
    PyModule_AddIntConstant(mod, "XPLM_VK_NUMPAD1", 0x61);
    PyModule_AddIntConstant(mod, "XPLM_VK_NUMPAD2", 0x62);
    PyModule_AddIntConstant(mod, "XPLM_VK_NUMPAD3", 0x63);
    PyModule_AddIntConstant(mod, "XPLM_VK_NUMPAD4", 0x64);
    PyModule_AddIntConstant(mod, "XPLM_VK_NUMPAD5", 0x65);
    PyModule_AddIntConstant(mod, "XPLM_VK_NUMPAD6", 0x66);
    PyModule_AddIntConstant(mod, "XPLM_VK_NUMPAD7", 0x67);
    PyModule_AddIntConstant(mod, "XPLM_VK_NUMPAD8", 0x68);
    PyModule_AddIntConstant(mod, "XPLM_VK_NUMPAD9", 0x69);
    PyModule_AddIntConstant(mod, "XPLM_VK_MULTIPLY", 0x6A);
    PyModule_AddIntConstant(mod, "XPLM_VK_ADD", 0x6B);
    PyModule_AddIntConstant(mod, "XPLM_VK_SEPARATOR", 0x6C);
    PyModule_AddIntConstant(mod, "XPLM_VK_SUBTRACT", 0x6D);
    PyModule_AddIntConstant(mod, "XPLM_VK_DECIMAL", 0x6E);
    PyModule_AddIntConstant(mod, "XPLM_VK_DIVIDE", 0x6F);
    PyModule_AddIntConstant(mod, "XPLM_VK_F1", 0x70);
    PyModule_AddIntConstant(mod, "XPLM_VK_F2", 0x71);
    PyModule_AddIntConstant(mod, "XPLM_VK_F3", 0x72);
    PyModule_AddIntConstant(mod, "XPLM_VK_F4", 0x73);
    PyModule_AddIntConstant(mod, "XPLM_VK_F5", 0x74);
    PyModule_AddIntConstant(mod, "XPLM_VK_F6", 0x75);
    PyModule_AddIntConstant(mod, "XPLM_VK_F7", 0x76);
    PyModule_AddIntConstant(mod, "XPLM_VK_F8", 0x77);
    PyModule_AddIntConstant(mod, "XPLM_VK_F9", 0x78);
    PyModule_AddIntConstant(mod, "XPLM_VK_F10", 0x79);
    PyModule_AddIntConstant(mod, "XPLM_VK_F11", 0x7A);
    PyModule_AddIntConstant(mod, "XPLM_VK_F12", 0x7B);
    PyModule_AddIntConstant(mod, "XPLM_VK_F13", 0x7C);
    PyModule_AddIntConstant(mod, "XPLM_VK_F14", 0x7D);
    PyModule_AddIntConstant(mod, "XPLM_VK_F15", 0x7E);
    PyModule_AddIntConstant(mod, "XPLM_VK_F16", 0x7F);
    PyModule_AddIntConstant(mod, "XPLM_VK_F17", 0x80);
    PyModule_AddIntConstant(mod, "XPLM_VK_F18", 0x81);
    PyModule_AddIntConstant(mod, "XPLM_VK_F19", 0x82);
    PyModule_AddIntConstant(mod, "XPLM_VK_F20", 0x83);
    PyModule_AddIntConstant(mod, "XPLM_VK_F21", 0x84);
    PyModule_AddIntConstant(mod, "XPLM_VK_F22", 0x85);
    PyModule_AddIntConstant(mod, "XPLM_VK_F23", 0x86);
    PyModule_AddIntConstant(mod, "XPLM_VK_F24", 0x87);
/* The following definitions are extended and are not based on the Microsoft   *
 * key set.                                                                    */
    PyModule_AddIntConstant(mod, "XPLM_VK_EQUAL", 0xB0);
    PyModule_AddIntConstant(mod, "XPLM_VK_MINUS", 0xB1);
    PyModule_AddIntConstant(mod, "XPLM_VK_RBRACE", 0xB2);
    PyModule_AddIntConstant(mod, "XPLM_VK_LBRACE", 0xB3);
    PyModule_AddIntConstant(mod, "XPLM_VK_QUOTE", 0xB4);
    PyModule_AddIntConstant(mod, "XPLM_VK_SEMICOLON", 0xB5);
    PyModule_AddIntConstant(mod, "XPLM_VK_BACKSLASH", 0xB6);
    PyModule_AddIntConstant(mod, "XPLM_VK_COMMA", 0xB7);
    PyModule_AddIntConstant(mod, "XPLM_VK_SLASH", 0xB8);
    PyModule_AddIntConstant(mod, "XPLM_VK_PERIOD", 0xB9);
    PyModule_AddIntConstant(mod, "XPLM_VK_BACKQUOTE", 0xBA);
    PyModule_AddIntConstant(mod, "XPLM_VK_ENTER", 0xBB);
    PyModule_AddIntConstant(mod, "XPLM_VK_NUMPAD_ENT", 0xBC);
    PyModule_AddIntConstant(mod, "XPLM_VK_NUMPAD_EQ", 0xBD);
  }
  return mod;
}




