//Python comes first!
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <XPLM/XPLMDefs.h>

#include <XPLM/XPLMPlugin.h>
#include <sys/types.h>
#include <regex.h>

#include "utils.h"
#include "plugin_dl.h"

/*************************************
 * Python plugin upgrade for Python 3
 *   Michal        f.josef@email.cz (uglyDwarf on x-plane.org)
 *   Peter Buckner pbuck@avnwx.com (pbuckner on x-plane.org) 
 *
 * Upgraded from original Python2 version by
 *   Sandy Barbour (on x-plane.org)
 */

/**********************
 * Plugin configuration
 */
static char *logFileName = "XPPython3.log";
static char *ENV_logFileVar = "XPPYTHON3_LOG";  // set this environment to override logFileName
static char *ENV_logPreserve = "XPPYTHON3_PRESERVE";  // DO NOT truncate XPPython log on startup. If set, we preserve, if unset, we truncate

static const char *pluginsPath = "./Resources/plugins/PythonPlugins";
static const char *internalPluginsPath = "./Resources/plugins/XPPython3";

static const char *pythonPluginName = "XPPython3";
static const char *pythonPluginSig  = "avnwx.xppython3";
static const char *pythonPluginDesc = "X-Plane interface for Python 3: Version 3.0.0";
static const char *pythonStopCommand = "XPPython3/stopScripts";
static const char *pythonStartCommand = "XPPython3/startScripts";
static const char *pythonReloadCommand = "XPPython3/reloadScripts";
/**********************/

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
static bool stopped;
static int allErrorsEncountered;

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

static PyObject *logWriterAddAllErrors(PyObject *self, PyObject *args)
{
  (void) self;
  int errs;
  if(!PyArg_ParseTuple(args, "i", &errs)){
    return NULL;
  }
  printf("Adding %d errors...\n", errs);
  allErrorsEncountered += errs;
  Py_RETURN_NONE;
}


static PyMethodDef logWriterMethods[] = {
  {"write", logWriterWrite, METH_VARARGS, ""},
  {"flush", logWriterFlush, METH_VARARGS, ""},
  {"addAllErrors", logWriterAddAllErrors, METH_VARARGS, ""},
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

PyObject *moduleDict;
static PyObject *loggerObj;

int initPython(void){
  // setbuf(stdout, NULL);  // for debugging, it removes stdout buffering

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
  moduleDict = PyDict_New();
  return 0;
}


bool loadPIClass(const char *fname)
{
  PyObject *pName = NULL, *pModule = NULL, *pClass = NULL,
           *pObj = NULL, *pRes = NULL, *err = NULL;

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
    fprintf(logFile, "XPluginStart returned NULL\n"); // NULL is error, Py_None is void, we're looking for a tuple[3]
    goto cleanup;
  }
  if(!(PyTuple_Check(pRes) && (PyTuple_Size(pRes) == 3) &&
      PyUnicode_Check(PyTuple_GetItem(pRes, 0)) &&
      PyUnicode_Check(PyTuple_GetItem(pRes, 1)) &&
      PyUnicode_Check(PyTuple_GetItem(pRes, 2)))){
    fprintf(logFile, "Unable to start plugin in file %s: XPluginStart did not return Name, Sig, and Desc.", fname);
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
    fflush(logFile);
  }
  Py_DECREF(u1);
  Py_DECREF(u2);
  Py_DECREF(u3);

  PyObject *pKey = PyTuple_New(4);  /* pKey is new reference */

  /* PyTuple_GetItem borrows reference, PyTuple_SetItem steals:pKey now owns pRes[0] */
  PyTuple_SetItem(pKey, 0, PyTuple_GetItem(pRes, 0));
  PyTuple_SetItem(pKey, 1, PyTuple_GetItem(pRes, 1));
  PyTuple_SetItem(pKey, 2, PyTuple_GetItem(pRes, 2));
  PyTuple_SetItem(pKey, 3, PyUnicode_FromString(fname));
  PyDict_SetItem(moduleDict, pKey, pObj); // does not steal reference. We don't need pKey again, so decref
  Py_DECREF(pKey);

 cleanup:
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  // vvvvv this seems to be key: do no decref (pRes) as it crashes Py_Finalize()
  // Py_XDECREF(pRes);
  // ^^^^^
  // use XDECREF rather than DECREF, because we may hit this section via goto cleanup error
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
  initPython();

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
  PyObject *pKey, *pVal;
  Py_ssize_t pos = 0;

  while(PyDict_Next(moduleDict, &pos, &pKey, &pVal)){
    char *moduleName = objToStr(PyTuple_GetItem(pKey, 3));
    PyObject *pRes = PyObject_CallMethod(pVal, "XPluginStop", NULL); // should return void, so we should see Py_None
    if(pRes != Py_None) {
      fprintf(logFile, "%s XPluginStop returned '%s' rather than None.\n", moduleName, objToStr(pRes));
    }
    PyObject *err = PyErr_Occurred();
    if(err){
      fprintf(logFile, "Error occured during the %s XPluginStop call:\n", moduleName);
      PyErr_Print();
    }else{
      Py_DECREF(pRes);
    }
  }

  PyDict_Clear(moduleDict);
  Py_DECREF(moduleDict);
  
  // Invoke cleanup method of all built-in modules
  char *mods[] = {"XPLMDefs", "XPLMDisplay", "XPLMGraphics", "XPLMUtilities", "XPLMScenery", "XPLMMenus",
                  "XPLMNavigation", "XPLMPlugin", "XPLMPlanes", "XPLMProcessing", "XPLMCamera", "XPWidgetDefs",
                  "XPWidgets", "XPStandardWidgets", "XPUIGraphics", "XPWidgetUtils", "XPLMInstance",
                  "XPLMMap", "XPLMDataAccess", "SandyBarbourUtilities", NULL};
  char **mod_ptr = mods;

  while(*mod_ptr != NULL){
    PyObject *mod = PyImport_ImportModule(*mod_ptr);
    fflush(stdout);
    if (PyErr_Occurred()) {
      fprintf(logFile, "XPlugin Failed during stop of internal module %s\n", *mod_ptr);
      PyErr_Print();
      return 1;
    }
      
    if(mod){
      PyObject *pRes = PyObject_CallMethod(mod, "cleanup", NULL);
      if (PyErr_Occurred() ) {
        fprintf(logFile, "XPlugin Failed during cleanup of internal module %s\n", *mod_ptr);
        PyErr_Print();
        return 1;
      }
        
      Py_DECREF(pRes);
      Py_DECREF(mod);
    }
    ++mod_ptr;
  }
  Py_DECREF(loggerObj);
  Py_Finalize();
  pythonStarted = false;
  return 0;
}

static XPLMCommandRef stopScripts;
static XPLMCommandRef startScripts;
static XPLMCommandRef reloadScripts;
static XPLMMenuID setupMenu; 

static int commandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon);
static void menuHandler(void *inMenuRef, void *inItemRef)
{
  (void) inMenuRef;
  (void) commandHandler(inItemRef, xplm_CommandBegin, NULL);
}

PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
  char *log;
  log = getenv(ENV_logFileVar);
  if(log != NULL){
    logFileName = log;
  }
  if (getenv(ENV_logPreserve) != NULL) {
    printf("Preserving log file\n"); fflush(stdout);
    logFile = fopen(logFileName, "a");
  } else {
    logFile = fopen(logFileName, "w");
  }
  if(logFile == NULL){
    logFile = stdout;
  }

  fprintf(logFile, "%s Started.\n", pythonPluginName);
  strcpy(outName, pythonPluginName);
  strcpy(outSig, pythonPluginSig);
  strcpy(outDesc, pythonPluginDesc);

  if (XPLMHasFeature("XPLM_USE_NATIVE_PATHS")) {
    XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
  } else {
    fprintf(logFile, "Warning: XPLM_USE_NATIVE_PATHS not enabled. Using Legacy paths.\n");
  }
  stopScripts = XPLMCreateCommand(pythonStopCommand, "Stop all running scripts");
  startScripts = XPLMCreateCommand(pythonStartCommand, "Start all scripts");
  reloadScripts = XPLMCreateCommand(pythonReloadCommand, "Reload all scripts");

  XPLMRegisterCommandHandler(stopScripts, commandHandler, 1, (void *)0);
  XPLMRegisterCommandHandler(startScripts, commandHandler, 1, (void *)1);
  XPLMRegisterCommandHandler(reloadScripts, commandHandler, 1, (void *)2);

  int menuIndex = XPLMAppendMenuItem(XPLMFindPluginsMenu(), pythonPluginName, NULL, 1);

  XPLMMenuID setupMenu = XPLMCreateMenu(pythonPluginName, XPLMFindPluginsMenu(), menuIndex, 
                         menuHandler, NULL);
  if(XPLMAppendMenuItemWithCommand_ptr){
    XPLMAppendMenuItemWithCommand_ptr(setupMenu, "Stop scripts", stopScripts);
    XPLMAppendMenuItemWithCommand_ptr(setupMenu, "Start scripts", startScripts);
    XPLMAppendMenuItemWithCommand_ptr(setupMenu, "Reload scripts", reloadScripts);
  }else{
    XPLMAppendMenuItem(setupMenu, "Stop scripts", (void *)stopScripts, 0);
    XPLMAppendMenuItem(setupMenu, "Start scripts", (void *)startScripts, 0);
    XPLMAppendMenuItem(setupMenu, "Reload scripts", (void *)reloadScripts, 0);
  }

  startPython();

  return 1;
}


PLUGIN_API void XPluginStop(void)
{
  stopPython();
  XPLMDestroyMenu(setupMenu);
  XPLMUnregisterCommandHandler(stopScripts, commandHandler, 1, (void *)0);
  XPLMUnregisterCommandHandler(startScripts, commandHandler, 1, (void *)1);
  XPLMUnregisterCommandHandler(reloadScripts, commandHandler, 1, (void *)2);
  if(allErrorsEncountered){
    fprintf(logFile, "Total errors encountered: %d\n", allErrorsEncountered);
  }
  fprintf(logFile, "%s Stopped.\n", pythonPluginName);
  fclose(logFile);
}

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


PLUGIN_API int XPluginEnable(void)
{
  PyObject *pKey, *pVal, *pRes;
  Py_ssize_t pos = 0;
  if(stopped){
    return 1;
  }

  while(PyDict_Next(moduleDict, &pos, &pKey, &pVal)){
    char *moduleName = objToStr(PyTuple_GetItem(pKey, 3));
    pRes = PyObject_CallMethod(pVal, "XPluginEnable", NULL);
    if(!(pRes && PyLong_Check(pRes))){
      fprintf(logFile, "%s XPluginEnable returned '%s' rather than an integer.\n", moduleName, objToStr(pRes));
    }else{
      //printf("XPluginEnable returned %ld\n", PyLong_AsLong(pRes));
    }
    PyObject *err = PyErr_Occurred();
    if(err){
      fprintf(logFile, "Error occured during the %s XPluginEnable call:\n", moduleName);
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
    char *moduleName = objToStr(PyTuple_GetItem(pKey, 3));
    pRes = PyObject_CallMethod(pVal, "XPluginDisable", NULL);
    if(pRes != Py_None) {
      fprintf(logFile, "%s XPluginDisable returned '%s' rather than None.\n", moduleName, objToStr(pRes));
    }
    PyObject *err = PyErr_Occurred();
    if(err){
      fprintf(logFile, "Error occured during the %s XPluginDisable call:\n", moduleName);
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
  /* printf("XPPython3 received message, which we'll try to send to all plugins: From: %d, Msg: %ld, inParam: %ld\n", */
  /*        inFromWho, inMessage, (long)inParam); */
  while(PyDict_Next(moduleDict, &pos, &pKey, &pVal)){
    char *moduleName = objToStr(PyTuple_GetItem(pKey, 3));
    pRes = PyObject_CallMethod(pVal, "XPluginReceiveMessage", "ilO", inFromWho, inMessage, param);
    if (pRes != Py_None) {
      fprintf(logFile, "%s XPluginReceiveMessage didn't return None.\n", moduleName);
    }
    PyObject *err = PyErr_Occurred();
    if(err){
      fprintf(logFile, "Error occured during the %s XPluginReceiveMessage call:\n", moduleName);
      PyErr_Print();
    }else{
      Py_DECREF(pRes);
    }
  }
  Py_DECREF(param);
}
