//Python comes first!
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <XPLM/XPLMDefs.h>

#include "utils.h"
#include "chk_helper.h"

PyMODINIT_FUNC PyInit_XPLMDefs(void);
PyMODINIT_FUNC PyInit_XPLMDisplay(void);
PyMODINIT_FUNC PyInit_XPLMGraphics(void);
PyMODINIT_FUNC PyInit_XPLMDataAccess(void);
PyMODINIT_FUNC PyInit_XPLMUtilities(void);
PyMODINIT_FUNC PyInit_XPLMScenery(void);
PyMODINIT_FUNC PyInit_XPLMMenus(void);
PyMODINIT_FUNC PyInit_XPLMNavigation(void);


static PyObject *XPLMCHKHelperCheckIntFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 's', "id"},
    {1, 'i', "expected"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 2, paramChecks)){
    return NULL;
  }
  char *id = PyUnicode_AsUTF8(PySequence_GetItem(args, 0));
  int expected = PyLong_AsLong(PySequence_GetItem(args, 1));
  if(compare_int_value(id, expected)){
    Py_RETURN_TRUE;
  }else{
    Py_RETURN_FALSE;
  }
}



static PyObject *XPLMCHKHelperCheckDoubleFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 's', "id"},
    {1, 'n', "expected"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 2, paramChecks)){
    return NULL;
  }
  char *id = PyUnicode_AsUTF8(PySequence_GetItem(args, 0));
  double expected = PyFloat_AsDouble(PyNumber_Float(PySequence_GetItem(args, 1)));
  if(compare_double_value(id, expected)){
    Py_RETURN_TRUE;
  }else{
    Py_RETURN_FALSE;
  }
}



static PyObject *XPLMCHKHelperCheckStrFun(PyObject *self, PyObject *args)
{
  (void) self;
  paramCheck_t paramChecks[] = {
    {0, 's', "id"},
    {1, 's', "expected"},
    {-1, '\0', NULL}
  };
  if(!checkParams(args, 2, paramChecks)){
    return NULL;
  }
  char *id = PyUnicode_AsUTF8(PySequence_GetItem(args, 0));
  char *expected = PyUnicode_AsUTF8(PySequence_GetItem(args, 1));
  if(compare_str_value(id, expected)){
    Py_RETURN_TRUE;
  }else{
    Py_RETURN_FALSE;
  }
}


static PyMethodDef XPLMCHKHelperMethods[] = {
  {"XPLMCHKHelperCheckInt", XPLMCHKHelperCheckIntFun, METH_VARARGS, ""},
  {"XPLMCHKHelperCheckDouble", XPLMCHKHelperCheckDoubleFun, METH_VARARGS, ""},
  {"XPLMCHKHelperCheckStr", XPLMCHKHelperCheckStrFun, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMCHKHelperModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMCHKHelper",
  NULL,
  -1,
  XPLMCHKHelperMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC PyInit_XPLMCHKHelper(void)
{
  return PyModule_Create(&XPLMCHKHelperModule);
}

//should be static, don't change after Py_SetProgramName is called 
// should be freed by PyMem_RawFree()
static wchar_t *program = NULL;

static PyObject *moduleDict;

int initPython(const char *programName){
  program = Py_DecodeLocale(programName, NULL);
  if(program == NULL){
    printf("Can't decode programName.\n");
    return -1;
  }
  Py_SetProgramName(program);
  PyImport_AppendInittab("XPLMCHKHelper", PyInit_XPLMCHKHelper);
  PyImport_AppendInittab("XPLMDefs", PyInit_XPLMDefs);
  PyImport_AppendInittab("XPLMDisplay", PyInit_XPLMDisplay);
  PyImport_AppendInittab("XPLMGraphics", PyInit_XPLMGraphics);
  PyImport_AppendInittab("XPLMDataAccess", PyInit_XPLMDataAccess);
  PyImport_AppendInittab("XPLMUtilities", PyInit_XPLMUtilities);
  PyImport_AppendInittab("XPLMScenery", PyInit_XPLMScenery);
  PyImport_AppendInittab("XPLMMenus", PyInit_XPLMMenus);
  PyImport_AppendInittab("XPLMNavigation", PyInit_XPLMNavigation);

  Py_Initialize();
  if(!Py_IsInitialized()){
    printf("Failed to initialize Python.\n");
    return -1;
  }
  //get "." into the python's path 
  PyRun_SimpleString("print('Adding the \".\" to path')\n"
                     "import sys\n"
                     "sys.path.append('.')");
  moduleDict = PyDict_New();
  return 0;
}

bool loadPIClass(const char *fname)
{
  PyObject *pName = NULL, *pModule = NULL, *pClass = NULL,
           *pObj = NULL, *pRes = NULL, *err = NULL;

  pName = PyUnicode_DecodeFSDefault(fname);
  if(pName == NULL){
    printf("Problem decoding the filename.\n");
    goto cleanup;
  }
  pModule = PyImport_Import(pName);
  Py_DECREF(pName);
  if(pModule == NULL){
    goto cleanup;
  }

  pClass = PyObject_GetAttrString(pModule, "PythonInterface");
  if(pClass == NULL){
    goto cleanup;
  }
  if(!PyCallable_Check(pClass)){
    goto cleanup;
  }
  //trying to get an object constructed
  pObj = PyObject_CallObject(pClass, NULL);

  if(pObj == NULL){
    goto cleanup;
  }
  pRes = PyObject_CallMethod(pObj, "XPluginStart", NULL);
  if(pRes == NULL){
    goto cleanup;
  }
  if(!(PyTuple_Check(pRes) && (PyTuple_Size(pRes) == 3) &&
      PyUnicode_Check(PyTuple_GetItem(pRes, 0)) &&
      PyUnicode_Check(PyTuple_GetItem(pRes, 1)) &&
      PyUnicode_Check(PyTuple_GetItem(pRes, 2)))){
    goto cleanup;
  }
  
  PyObject *u1 = NULL, *u2 = NULL, *u3 = NULL;

  u1 = PyUnicode_AsUTF8String(PyTuple_GetItem(pRes, 0));
  u2 = PyUnicode_AsUTF8String(PyTuple_GetItem(pRes, 1));
  u3 = PyUnicode_AsUTF8String(PyTuple_GetItem(pRes, 2));
  if(u1 && u2 && u3){
    printf("%s initialized:", fname);
    printf(" Name: %s", PyBytes_AsString(u1));
    printf(" Sig: %s", PyBytes_AsString(u2));
    printf(" Desc: %s\n", PyBytes_AsString(u3));
  }
  Py_XDECREF(u1);
  Py_XDECREF(u2);
  Py_XDECREF(u3);

  PyDict_SetItem(moduleDict, pRes, pObj);
 cleanup:
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_XDECREF(pRes);
  Py_XDECREF(pModule);
  Py_XDECREF(pClass);
  return pObj;
}


int XPluginStart(char *outName, char *outSig, char *outDesc)
{
  printf("X-PluginStart called.\n");
  strcpy(outName, "Python Interface revival");
  strcpy(outSig, "x.y.z");
  strcpy(outDesc, "X-Plane interface for Python 3.");
  initPython("X-Plane");

  //Scan current directory for the plugin modules
  DIR *dir = opendir(".");
  if(dir == NULL){
    printf("Can't open '.' to scan for plugins.\n");
    return 0;
  }
  struct dirent *de;
  while((de = readdir(dir))){
    //Check that it strts by 'PI_' and ends with '.py'
    //  Well, regexp ould be more suitable... TODO?
    if((strncmp(de->d_name, "PI_", 3) == 0) && (strncmp(de->d_name + strlen(de->d_name) - 3, ".py", 3) == 0)){
      char *modName = strdup(de->d_name);
      if(modName){
        modName[strlen(de->d_name) - 3] = '\0';
      }
      loadPIClass(modName);
    }
  }

  return 1;
}


void XPluginStop(void)
{
  PyObject *pKey, *pVal;
  Py_ssize_t pos = 0;

  while(PyDict_Next(moduleDict, &pos, &pKey, &pVal)){
    PyObject_CallMethod(pVal, "XPluginStop", NULL);
    PyObject *err = PyErr_Occurred();
    if(err){
      printf("Error occured during the XPluginStop call:\n");
      PyErr_Print();
    }
  }
  PyDict_Clear(moduleDict);

  Py_Finalize();
  PyMem_RawFree(program);
  //printf("XPluginStop finished.\n");
}

int XPluginEnable(void)
{
  PyObject *pKey, *pVal, *pRes;
  Py_ssize_t pos = 0;

  while(PyDict_Next(moduleDict, &pos, &pKey, &pVal)){
    pRes = PyObject_CallMethod(pVal, "XPluginEnable", NULL);
    if(!(pRes && PyLong_Check(pRes))){
      printf("XPluginEnable didn't return integer.\n");
    }else{
      //printf("XPluginEnable returned %ld\n", PyLong_AsLong(pRes));
    }
    Py_XDECREF(pRes);
    PyObject *err = PyErr_Occurred();
    if(err){
      printf("Error occured during the XPluginEnable call:\n");
      PyErr_Print();
    }
  }

  return 1;
}

void XPluginDisable(void)
{
  PyObject *pKey, *pVal;
  Py_ssize_t pos = 0;

  while(PyDict_Next(moduleDict, &pos, &pKey, &pVal)){
    PyObject_CallMethod(pVal, "XPluginDisable", NULL);
    PyObject *err = PyErr_Occurred();
    if(err){
      printf("Error occured during the XPluginDisable call:\n");
      PyErr_Print();
    }
  }

}

void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void *inParam)
{
  PyObject *pKey, *pVal;
  Py_ssize_t pos = 0;

  while(PyDict_Next(moduleDict, &pos, &pKey, &pVal)){
    PyObject_CallMethod(pVal, "XPluginReceiveMessage", "ill", inFromWho, inMessage, inParam);
    PyObject *err = PyErr_Occurred();
    if(err){
      printf("Error occured during the XPluginReceiveMessage call:\n");
      PyErr_Print();
    }
  }
}



