//Python comes first!
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <XPLM/XPLMDefs.h>

#include <sys/types.h>
#include <regex.h>

#include "utils.h"
#include "plugin_dl.h"

PyMODINIT_FUNC PyInit_XPLMDefs(void);
PyMODINIT_FUNC PyInit_XPLMDisplay(void);
PyMODINIT_FUNC PyInit_XPLMGraphics(void);
PyMODINIT_FUNC PyInit_XPLMDataAccess(void);
PyMODINIT_FUNC PyInit_XPLMUtilities(void);
PyMODINIT_FUNC PyInit_XPLMScenery(void);
PyMODINIT_FUNC PyInit_XPLMMenus(void);
PyMODINIT_FUNC PyInit_XPLMNavigation(void);
PyMODINIT_FUNC PyInit_XPLMPlugin(void);
PyMODINIT_FUNC PyInit_XPLMPlanes(void);
PyMODINIT_FUNC PyInit_XPLMProcessing(void);
PyMODINIT_FUNC PyInit_XPLMCamera(void);
PyMODINIT_FUNC PyInit_XPWidgetDefs(void);
PyMODINIT_FUNC PyInit_XPWidgets(void);
PyMODINIT_FUNC PyInit_XPStandardWidgets(void);
PyMODINIT_FUNC PyInit_XPUIGraphics(void);
PyMODINIT_FUNC PyInit_XPWidgetUtils(void);
PyMODINIT_FUNC PyInit_XPLMInstance(void);
PyMODINIT_FUNC PyInit_XPLMMap(void);
PyMODINIT_FUNC PyInit_SBU(void);

static FILE *logFile;
static char *logFileName;

static const char *pluginsPath = "./Resources/plugins/PythonPlugins";
static const char *internalPluginsPath = "./Resources/plugins/XPythonRevival";

static bool stopped;

static PyObject *logWriterWrite(PyObject *self, PyObject *args)
{
  (void) self;
  char *msg;
  if(!PyArg_ParseTuple(args, "s", &msg)){
    return NULL;
  }
  //printf("%s", msg);
  fprintf(logFile, "%s", msg);
  fflush(logFile);
  Py_RETURN_NONE;
}

static PyObject *logWriterFlush(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  fflush(logFile);
  Py_RETURN_NONE;
}

static PyMethodDef logWriterMethods[] = {
  {"write", logWriterWrite, METH_VARARGS, ""},
  {"flush", logWriterFlush, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPythonLogWriterModule = {
  PyModuleDef_HEAD_INIT,
  "XPythonLogWriter",
  NULL,
  -1,
  logWriterMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPythonLogWriter(void)
{
  PyObject *mod = PyModule_Create(&XPythonLogWriterModule);
  if(mod){
    PySys_SetObject("stdout", mod);
    PySys_SetObject("stderr", mod);
  }

  return mod;
};

//should be static, don't change after Py_SetProgramName is called 
// should be freed by PyMem_RawFree()
static wchar_t *program = NULL;

static PyObject *moduleDict;
static PyObject *loggerObj;

int initPython(const char *programName){
  program = Py_DecodeLocale(programName, NULL);
  if(program == NULL){
    fprintf(logFile, "Can't decode programName.\n");
    return -1;
  }
  Py_SetProgramName(program);

  PyImport_AppendInittab("XPLMDefs", PyInit_XPLMDefs);
  PyImport_AppendInittab("XPLMDisplay", PyInit_XPLMDisplay);
  PyImport_AppendInittab("XPLMGraphics", PyInit_XPLMGraphics);
  PyImport_AppendInittab("XPLMDataAccess", PyInit_XPLMDataAccess);
  PyImport_AppendInittab("XPLMUtilities", PyInit_XPLMUtilities);
  PyImport_AppendInittab("XPLMScenery", PyInit_XPLMScenery);
  PyImport_AppendInittab("XPLMMenus", PyInit_XPLMMenus);
  PyImport_AppendInittab("XPLMNavigation", PyInit_XPLMNavigation);
  PyImport_AppendInittab("XPLMPlugin", PyInit_XPLMPlugin);
  PyImport_AppendInittab("XPLMPlanes", PyInit_XPLMPlanes);
  PyImport_AppendInittab("XPLMProcessing", PyInit_XPLMProcessing);
  PyImport_AppendInittab("XPLMCamera", PyInit_XPLMCamera);
  PyImport_AppendInittab("XPWidgetDefs", PyInit_XPWidgetDefs);
  PyImport_AppendInittab("XPWidgets", PyInit_XPWidgets);
  PyImport_AppendInittab("XPStandardWidgets", PyInit_XPStandardWidgets);
  PyImport_AppendInittab("XPUIGraphics", PyInit_XPUIGraphics);
  PyImport_AppendInittab("XPWidgetUtils", PyInit_XPWidgetUtils);
  PyImport_AppendInittab("XPLMInstance", PyInit_XPLMInstance);
  PyImport_AppendInittab("XPLMMap", PyInit_XPLMMap);
  PyImport_AppendInittab("XPythonLogger", PyInit_XPythonLogWriter);
  PyImport_AppendInittab("SandyBarbourUtilities", PyInit_SBU);

  Py_Initialize();
  if(!Py_IsInitialized()){
    fprintf(logFile, "Failed to initialize Python.\n");
    return -1;
  }

  //get the plugin directory into the python's path
  loggerObj = PyImport_ImportModule("XPythonLogger");
  PyObject *path = PySys_GetObject("path"); //Borrowed!
  const char pathStr[] = "./Resources/plugins/PythonPlugins";
  PyObject *pathStrObj = PyUnicode_DecodeUTF8(pathStr, sizeof(pathStr) - 1, NULL);
  PyList_Append(path, pathStrObj);
  Py_DECREF(pathStrObj);
  //PySys_SetPath(L"");
  moduleDict = PyDict_New();
  return 0;
}


bool loadPIClass(const char *fname)
{
  PyObject *pName = NULL, *pModule = NULL, *pClass = NULL,
           *pObj = NULL, *pRes = NULL, *err = NULL;
  //printf("Decoding the filename '%s'.\n", fname);

  pName = PyUnicode_DecodeFSDefault(fname);
  if(pName == NULL){
    fprintf(logFile, "Problem decoding the filename.\n");
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
    fprintf(logFile, "%s initialized.\n", fname);
    fprintf(logFile, "  Name: %s\n", PyBytes_AsString(u1));
    fprintf(logFile, "  Sig:  %s\n", PyBytes_AsString(u2));
    fprintf(logFile, "  Desc: %s\n", PyBytes_AsString(u3));
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

void loadModules(const char *path, const char *pattern)
{
  //Scan current directory for the plugin modules
  DIR *dir = opendir(path);
  if(dir == NULL){
    fprintf(logFile, "Can't open '%s' to scan for plugins.\n", path);
    return;
  }
  struct dirent *de;
  regex_t rex;
  if(regcomp(&rex, pattern, REG_NOSUB) == 0){
    while((de = readdir(dir))){
      if(regexec(&rex, de->d_name, 0, NULL, 0) == 0){
        char *modName = strdup(de->d_name);
        if(modName){
          modName[strlen(de->d_name) - 3] = '\0';
        }
        loadPIClass(modName);
        free(modName);
      }
    }
    regfree(&rex);
    closedir(dir);
  }
}

static bool pythonStarted;

static int startPython(void)
{
  if(pythonStarted){
    return 0;
  }
  loadAllFunctions();
  initPython("X-Plane");

  // Load internal stuff
  loadModules(internalPluginsPath, "^I_PI_.*\\.py$");
  // Load modules
  loadModules(pluginsPath, "^PI_.*\\.py$");
  pythonStarted = true;
  return 1;
}

static int stopPython(void)
{
  if(!pythonStarted){
    return 0;
  }
  PyDict_Clear(moduleDict);

  // Invoke cleanup method of all built-in modules
  char *mods[] = {"XPLMDefs", "XPLMDisplay", "XPLMGraphics", "XPLMUtilities", "XPLMScenery", "XPLMMenus",
                  "XPLMNavigation", "XPLMPlugin", "XPLMPlanes", "XPLMProcessing", "XPLMCamera", "XPWidgetDefs",
                  "XPWidgets", "XPStandardWidgets", "XPUIGraphics", "XPWidgetUtils", "XPLMInstance",
                  "XPLMMap", "XPLMDataAccess", "SandyBarbourUtilities", NULL};
  char **mod_ptr = mods;

  while(*mod_ptr != NULL){
    PyObject *mod = PyImport_ImportModule(*mod_ptr);
    if(mod){
      PyObject *res = PyObject_CallMethod(mod, "cleanup", "");
      Py_DECREF(res);
      Py_DECREF(mod);
    }
    ++mod_ptr;
  }
  Py_DECREF(loggerObj);
  Py_Finalize();
  PyMem_RawFree(program);
  pythonStarted = false;
  return 0;
}

static XPLMCommandRef stopScripts;
static XPLMCommandRef startScripts;
static XPLMCommandRef reloadScripts;

static int commandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon)
{
  (void) inRefcon;
  if(inPhase != xplm_CommandBegin){
    return 0;
  }
  if(inCommand == stopScripts){
    stopped = true;
  }else if(inCommand == startScripts){
    stopped = false;
  }else if(inCommand == reloadScripts){
    stopPython();
    startPython();
  }
  return 0;
}

static void menuHandler(void *inMenuRef, void *inItemRef)
{
  (void) inMenuRef;
  (void) commandHandler(inItemRef, xplm_CommandBegin, NULL);
}

PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
  logFileName = "XPython.log";
  char *log;
  log = getenv("XPYTHON_LOG");
  if(log != NULL){
    logFileName = log;
  }
  logFile = fopen(logFileName, "w");
  if(logFile == NULL){
    logFile = stdout;
  }

  fprintf(logFile, "X-PluginStart called.\n");
  strcpy(outName, "XPython3");
  strcpy(outSig, "XPython3.0.0");
  strcpy(outDesc, "X-Plane interface for Python 3.");

  stopScripts = XPLMCreateCommand("XPython3/stopScripts", "Stop all running scripts");
  startScripts = XPLMCreateCommand("XPython3/startScripts", "Start all scripts");
  reloadScripts = XPLMCreateCommand("XPython3/reloadScripts", "Reload all scripts");

  XPLMRegisterCommandHandler(stopScripts, commandHandler, 1, (void *)0);
  XPLMRegisterCommandHandler(startScripts, commandHandler, 1, (void *)1);
  XPLMRegisterCommandHandler(reloadScripts, commandHandler, 1, (void *)2);

  int menuIndex = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "XPython3", NULL, 1);

  XPLMMenuID setupMenu = XPLMCreateMenu("XPython3", XPLMFindPluginsMenu(), menuIndex, 
                         menuHandler, NULL);
  if(XPLMAppendMenuItemWithCommand_ptr){
    XPLMAppendMenuItemWithCommand_ptr(setupMenu, "Stop scripts", stopScripts);
    XPLMAppendMenuItemWithCommand_ptr(setupMenu, "Start scripts", startScripts);
    XPLMAppendMenuItemWithCommand_ptr(setupMenu, "Reload scripts", reloadScripts);
  }else{
    XPLMAppendMenuItem(setupMenu, "Stop scripts", (void *)stopScripts, 0);
    XPLMAppendMenuItem(setupMenu, "Sart scripts", (void *)startScripts, 0);
    XPLMAppendMenuItem(setupMenu, "Reload scripts", (void *)reloadScripts, 0);
  }

  startPython();

  return 1;
}


PLUGIN_API void XPluginStop(void)
{
  PyObject *pKey, *pVal;
  Py_ssize_t pos = 0;

  if(stopped){
    return;
  }

  while(PyDict_Next(moduleDict, &pos, &pKey, &pVal)){
    PyObject *res = PyObject_CallMethod(pVal, "XPluginStop", NULL);
    PyObject *err = PyErr_Occurred();
    Py_DECREF(res);
    if(err){
      fprintf(logFile, "Error occured during the XPluginStop call:\n");
      PyErr_Print();
    }
  }
  stopPython();
  //printf("XPluginStop finished.\n");
}

PLUGIN_API int XPluginEnable(void)
{
  PyObject *pKey, *pVal, *pRes;
  Py_ssize_t pos = 0;
  if(stopped){
    return 1;
  }

  while(PyDict_Next(moduleDict, &pos, &pKey, &pVal)){
    pRes = PyObject_CallMethod(pVal, "XPluginEnable", NULL);
    if(!(pRes && PyLong_Check(pRes))){
      fprintf(logFile, "XPluginEnable didn't return integer.\n");
    }else{
      //printf("XPluginEnable returned %ld\n", PyLong_AsLong(pRes));
    }
    PyObject *err = PyErr_Occurred();
    if(err){
      fprintf(logFile, "Error occured during the XPluginEnable call:\n");
      PyErr_Print();
    }else{
      Py_DECREF(pRes);
    }
  }

  return 1;
}

PLUGIN_API void XPluginDisable(void)
{
  PyObject *pKey, *pVal, *pRes;
  Py_ssize_t pos = 0;
  if(stopped){
    return;
  }

  while(PyDict_Next(moduleDict, &pos, &pKey, &pVal)){
    pRes = PyObject_CallMethod(pVal, "XPluginDisable", NULL);
    PyObject *err = PyErr_Occurred();
    if(err){
      fprintf(logFile, "Error occured during the XPluginDisable call:\n");
      PyErr_Print();
    }else{
      Py_DECREF(pRes);
    }
  }

}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, long inMessage, void *inParam)
{
  PyObject *pKey, *pVal, *pRes;
  Py_ssize_t pos = 0;
  PyObject *param;
  if(stopped){
    return;
  }
  if(inParam != NULL){
    param = PyLong_FromLong((long)inParam);
  }else{
    param = Py_None;
    Py_INCREF(param);
  }
  while(PyDict_Next(moduleDict, &pos, &pKey, &pVal)){
    pRes = PyObject_CallMethod(pVal, "XPluginReceiveMessage", "ilO", inFromWho, inMessage, param);
    PyObject *err = PyErr_Occurred();
    if(err){
      fprintf(logFile, "Error occured during the XPluginReceiveMessage call:\n");
      PyErr_Print();
    }else{
      Py_DECREF(pRes);
    }
  }
  Py_DECREF(param);
}



