#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#define XPLM200
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMUtilities.h>
#include "utils.h"

PyObject *errCallbacks;
PyObject *commandCallbacks;
PyObject *commandRefcons;
intptr_t commandCallbackCntr;



static void error_callback(const char *inMessage)
{
  //TODO: send the error only to the active plugin?
  // for now, pass to all registered
  Py_ssize_t cnt = 0;
  PyObject *pKey = NULL, *pVal = NULL;
  PyObject *msg = PyUnicode_DecodeUTF8(inMessage, strlen(inMessage), NULL);
  while(PyDict_Next(errCallbacks, &cnt, &pKey, &pVal)){
    PyObject *oRes = PyObject_CallFunctionObjArgs(pVal, msg, NULL);
    PyObject *err = PyErr_Occurred();
    if(err){
      PyErr_Print();
    }else{
      Py_DECREF(oRes);
    }
  }
  Py_DECREF(msg);
}


static PyObject *XPLMSimulateKeyPressFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inKeyType, inKey;
  if(!PyArg_ParseTuple(args, "ii", &inKeyType, &inKey)){
    return NULL;
  }
  XPLMSimulateKeyPress(inKeyType, inKey);
  Py_RETURN_NONE;
}

static PyObject *XPLMSpeakStringFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inString;
  if(!PyArg_ParseTuple(args, "s", &inString)){
    return NULL;
  }
  XPLMSpeakString(inString);
  Py_RETURN_NONE;
}

static PyObject *XPLMCommandKeyStrokeFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inKey;
  if(!PyArg_ParseTuple(args, "i", &inKey)){
    return NULL;
  }
  XPLMCommandKeyStroke(inKey);
  Py_RETURN_NONE;
}

static PyObject *XPLMCommandButtonPressFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inButton;
  if(!PyArg_ParseTuple(args, "i", &inButton)){
    return NULL;
  }
  XPLMCommandButtonPress(inButton);
  Py_RETURN_NONE;
}

static PyObject *XPLMCommandButtonReleaseFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inButton;
  if(!PyArg_ParseTuple(args, "i", &inButton)){
    return NULL;
  }
  XPLMCommandButtonRelease(inButton);
  Py_RETURN_NONE;
}

static PyObject *XPLMGetVirtualKeyDescriptionFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inVirtualKey;
  if(!PyArg_ParseTuple(args, "i", &inVirtualKey)){
    return NULL;
  }
  const char *res = XPLMGetVirtualKeyDescription(inVirtualKey);
  return PyUnicode_DecodeUTF8(res, strlen(res), NULL);
}

static PyObject *XPLMReloadSceneryFun(PyObject *self, PyObject *args)
{
  (void)self;
  (void)args;
  XPLMReloadScenery();
  Py_RETURN_NONE;
}

static PyObject *XPLMGetSystemPathFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  char outSystemPath[1024];
  XPLMGetSystemPath(outSystemPath);

  return PyUnicode_DecodeUTF8(outSystemPath, strlen(outSystemPath), NULL);
}

static PyObject *XPLMGetPrefsPathFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  char outPrefsPath[1024];
  XPLMGetPrefsPath(outPrefsPath);

  return PyUnicode_DecodeUTF8(outPrefsPath, strlen(outPrefsPath), NULL);
}

static PyObject *XPLMGetDirectorySeparatorFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  
  const char *res = XPLMGetDirectorySeparator();

  return PyUnicode_DecodeUTF8(res, 1, NULL);
}


static PyObject *XPLMExtractFileAndPathFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inFullPathConst;
  if(!PyArg_ParseTuple(args, "s", &inFullPathConst)){
    return NULL;
  }
  char *inFullPath = strdup(inFullPathConst);
  const char *res = XPLMExtractFileAndPath(inFullPath);
  
  PyObject *resObj = Py_BuildValue("(ss)", res, inFullPath);
  free(inFullPath);
  return resObj;
}

static PyObject *XPLMGetDirectoryContentsFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inDirectoryPath;
  int inFirstReturn;
  int inFileNameBufSize;
  int inIndexCount;
  if(!PyArg_ParseTuple(args, "siii", &inDirectoryPath, &inFirstReturn, &inFileNameBufSize, &inIndexCount)){
    return NULL;
  }
  char *outFileNames = (char *)malloc(inFileNameBufSize);
  char **outIndices = (char **)malloc(inIndexCount * sizeof(char *));
  int outTotalFiles;
  int outReturnedFiles;

  int res = XPLMGetDirectoryContents(inDirectoryPath, inFirstReturn, outFileNames, inFileNameBufSize,
                                     outIndices, inIndexCount, &outTotalFiles, &outReturnedFiles);
  
  PyObject *namesList = PyList_New(0);
  PyObject *tmp;
  for(int i = 0; i < outReturnedFiles; ++i){
    if(outIndices[i] != NULL){
      tmp = PyUnicode_DecodeUTF8(outIndices[i], strlen(outIndices[i]), NULL);
      PyList_Append(namesList, tmp);
      Py_DECREF(tmp);
    }else{
      break;
    }
  }

  PyObject *retObj = Py_BuildValue("(iOi)", res, namesList, outTotalFiles);

  free(outFileNames);
  free(outIndices);
  return retObj;
}

static PyObject *XPLMInitializedFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  
  int res = XPLMInitialized();

  return PyLong_FromLong(res);
}

static PyObject *XPLMGetVersionsFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  int outXPlaneVersion;
  int outXPLMVersion;
  XPLMHostApplicationID outHostID;

  XPLMGetVersions(&outXPlaneVersion, &outXPLMVersion, &outHostID);
  return Py_BuildValue("(iii)", outXPlaneVersion, outXPLMVersion, (int)outHostID);
}

static PyObject *XPLMGetLanguageFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  
  XPLMLanguageCode res = XPLMGetLanguage();

  return PyLong_FromLong(res);
}

static PyObject *XPLMDebugStringFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inString;
  if(!PyArg_ParseTuple(args, "s", &inString)){
    return NULL;
  }
  XPLMDebugString(inString);
  Py_RETURN_NONE;
}


//Assumption:
// Single error reporting callback per PI_* plugin!
static PyObject *XPLMSetErrorCallbackFun(PyObject *self, PyObject *args)
{
  (void) self;

  if(PyDict_Size(errCallbacks) == 0){
    XPLMSetErrorCallback(error_callback);
  }

  PyObject *selfObj;
  PyObject *callback;
  if(!PyArg_ParseTuple(args, "OO", &selfObj, &callback)){
    return NULL;
  }
  PyDict_SetItem(errCallbacks, selfObj, callback);
  
  Py_RETURN_NONE;
}

static PyObject *XPLMFindSymbolFun(PyObject *self, PyObject *args)
{
  (void) self;
  
  const char *inString;
  if(!PyArg_ParseTuple(args, "s", &inString)){
    return NULL;
  }
  return PyLong_FromVoidPtr(XPLMFindSymbol(inString));
}

static PyObject *XPLMLoadDataFileFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inFileType;
  const char *inFilePath;
  if(!PyArg_ParseTuple(args, "is", &inFileType, &inFilePath)){
    return NULL;
  }
  int res = XPLMLoadDataFile(inFileType, inFilePath);
  return PyLong_FromLong(res);
}

static PyObject *XPLMSaveDataFileFun(PyObject *self, PyObject *args)
{
  (void) self;
  int inFileType;
  const char *inFilePath;
  if(!PyArg_ParseTuple(args, "is", &inFileType, &inFilePath)){
    return NULL;
  }
  int res = XPLMSaveDataFile(inFileType, inFilePath);
  return PyLong_FromLong(res);
}

static int commandCallback(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon)
{
  PyObject *pID = PyLong_FromVoidPtr(inRefcon);
  PyObject *pCbk = PyDict_GetItem(commandCallbacks, pID);
  if(pCbk == NULL){
    printf("Received unknown commandCallback refCon (%p).\n", inRefcon);
    return -1;
  }
  //0 - self, 1 - callback, 2 - refcon
  PyObject *arg1 = PyLong_FromVoidPtr(inCommand);
  PyObject *arg2 = PyLong_FromLong(inPhase);
  PyObject *oRes = PyObject_CallFunctionObjArgs(PyTuple_GetItem(pCbk, 2), arg1, arg2, PyTuple_GetItem(pCbk, 4), NULL);
  Py_DECREF(arg1);
  Py_DECREF(arg2);
  PyObject *err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(pID);
  PyObject *tmp = PyNumber_Long(oRes);
  int res = PyLong_AsLong(tmp);
  Py_DECREF(tmp);
  err = PyErr_Occurred();
  if(err){
    PyErr_Print();
  }
  Py_DECREF(oRes);
  return res;
}

static PyObject *XPLMFindCommandFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inName;
  if(!PyArg_ParseTuple(args, "s", &inName)){
    return NULL;
  }
  XPLMCommandRef res = XPLMFindCommand(inName);
  return PyLong_FromVoidPtr(res);
}

static PyObject *XPLMCommandBeginFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *inCommand;
  if(!PyArg_ParseTuple(args, "O", &inCommand)){
    return NULL;
  }
  XPLMCommandBegin(PyLong_AsVoidPtr(inCommand));
  Py_RETURN_NONE;
}

static PyObject *XPLMCommandEndFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *inCommand;
  if(!PyArg_ParseTuple(args, "O", &inCommand)){
    return NULL;
  }
  XPLMCommandEnd(PyLong_AsVoidPtr(inCommand));
  Py_RETURN_NONE;
}

static PyObject *XPLMCommandOnceFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *inCommand;
  if(!PyArg_ParseTuple(args, "O", &inCommand)){
    return NULL;
  }
  XPLMCommandOnce(PyLong_AsVoidPtr(inCommand));
  Py_RETURN_NONE;
}

static PyObject *XPLMCreateCommandFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *inName;
  const char *inDescription;
  if(!PyArg_ParseTuple(args, "ss", &inName, &inDescription)){
    return NULL;
  }
  XPLMCommandRef res = XPLMCreateCommand(inName, inDescription);
  return PyLong_FromVoidPtr(res);
}

static PyObject *XPLMRegisterCommandHandlerFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *inCommand;
  PyObject *inHandler;
  int inBefore;
  PyObject *inRefcon;
  if(!PyArg_ParseTuple(args, "OOOiO", &self, &inCommand, &inHandler, &inBefore, &inRefcon)){
    return NULL;
  }
  intptr_t refcon = commandCallbackCntr++;
  XPLMRegisterCommandHandler(PyLong_AsVoidPtr(inCommand), commandCallback, inBefore, (void *)refcon);
  PyObject *rc = PyLong_FromVoidPtr((void *)refcon);
  PyObject *irc = PyLong_FromVoidPtr((void *)inRefcon);
  PyDict_SetItem(commandRefcons, irc, rc);
  PyDict_SetItem(commandCallbacks, rc, args);
  Py_DECREF(rc);
  Py_DECREF(irc);
  Py_RETURN_NONE;
}

static PyObject *XPLMUnregisterCommandHandlerFun(PyObject *self, PyObject *args)
{
  (void) self;
  PyObject *inCommand;
  PyObject *inHandler;
  int inBefore;
  PyObject *inRefcon;
  if(!PyArg_ParseTuple(args, "OOOiO", &self, &inCommand, &inHandler, &inBefore, &inRefcon)){
    return NULL;
  }
  PyObject *key = PyLong_FromVoidPtr((void *)inRefcon);
  PyObject *refcon = PyDict_GetItem(commandRefcons, key);
  XPLMUnregisterCommandHandler(PyLong_AsVoidPtr(inCommand), commandCallback, inBefore, PyLong_AsVoidPtr(refcon));
  if(PyDict_DelItem(commandRefcons, key)){
    printf("XPLMUnregisterCommandHandler: couldn't remove refcon.\n");
  }
  Py_DECREF(key);
  if(PyDict_DelItem(commandCallbacks, refcon)){
    printf("XPLMUnregisterCommandHandler: couldn't remove command handler.\n");
  }
  
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  PyDict_Clear(errCallbacks);
  Py_DECREF(errCallbacks);
  PyDict_Clear(commandCallbacks);
  Py_DECREF(commandCallbacks);
  PyDict_Clear(commandRefcons);
  Py_DECREF(commandRefcons);
  Py_RETURN_NONE;
}

static PyMethodDef XPLMUtilitiesMethods[] = {
  {"XPLMSimulateKeyPress", XPLMSimulateKeyPressFun, METH_VARARGS, ""},
  {"XPLMSpeakString", XPLMSpeakStringFun, METH_VARARGS, ""},
  {"XPLMCommandKeyStroke", XPLMCommandKeyStrokeFun, METH_VARARGS, ""},
  {"XPLMCommandButtonPress", XPLMCommandButtonPressFun, METH_VARARGS, ""},
  {"XPLMCommandButtonRelease", XPLMCommandButtonReleaseFun, METH_VARARGS, ""},
  {"XPLMGetVirtualKeyDescription", XPLMGetVirtualKeyDescriptionFun, METH_VARARGS, ""},
  {"XPLMReloadScenery", XPLMReloadSceneryFun, METH_VARARGS, ""},
  {"XPLMGetSystemPath", XPLMGetSystemPathFun, METH_VARARGS, ""},
  {"XPLMGetPrefsPath", XPLMGetPrefsPathFun, METH_VARARGS, ""},
  {"XPLMGetDirectorySeparator", XPLMGetDirectorySeparatorFun, METH_VARARGS, ""},
  {"XPLMExtractFileAndPath", XPLMExtractFileAndPathFun, METH_VARARGS, ""},
  {"XPLMGetDirectoryContents", XPLMGetDirectoryContentsFun, METH_VARARGS, ""},
  {"XPLMInitialized", XPLMInitializedFun, METH_VARARGS, ""},
  {"XPLMGetVersions", XPLMGetVersionsFun, METH_VARARGS, ""},
  {"XPLMGetLanguage", XPLMGetLanguageFun, METH_VARARGS, ""},
  {"XPLMDebugString", XPLMDebugStringFun, METH_VARARGS, ""},
  {"XPLMSetErrorCallback", XPLMSetErrorCallbackFun, METH_VARARGS, ""},
  {"XPLMFindSymbol", XPLMFindSymbolFun, METH_VARARGS, ""},
  {"XPLMLoadDataFile", XPLMLoadDataFileFun, METH_VARARGS, ""},
  {"XPLMSaveDataFile", XPLMSaveDataFileFun, METH_VARARGS, ""},
  {"XPLMFindCommand", XPLMFindCommandFun, METH_VARARGS, ""},
  {"XPLMCommandBegin", XPLMCommandBeginFun, METH_VARARGS, ""},
  {"XPLMCommandEnd", XPLMCommandEndFun, METH_VARARGS, ""},
  {"XPLMCommandOnce", XPLMCommandOnceFun, METH_VARARGS, ""},
  {"XPLMCreateCommand", XPLMCreateCommandFun, METH_VARARGS, ""},
  {"XPLMRegisterCommandHandler", XPLMRegisterCommandHandlerFun, METH_VARARGS, ""},
  {"XPLMUnregisterCommandHandler", XPLMUnregisterCommandHandlerFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef XPLMUtilitiesModule = {
  PyModuleDef_HEAD_INIT,
  "XPLMUtilities",
  NULL,
  -1,
  XPLMUtilitiesMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPLMUtilities(void)
{
  if(!(errCallbacks = PyDict_New())){
    return NULL;
  }
  if(!(commandCallbacks = PyDict_New())){
    return NULL;
  }
  if(!(commandRefcons = PyDict_New())){
    return NULL;
  }

  PyObject *mod = PyModule_Create(&XPLMUtilitiesModule);
  if(mod){
    /*
     * XPLMCommandKeyID
     * 
     * These enums represent all the keystrokes available within x-plane.  They 
     * can be sent to x-plane directly.  For example, you can reverse thrust using 
     * these  enumerations.                                                        
     *
     */
    PyModule_AddIntConstant(mod, "xplm_key_pause", xplm_key_pause);
    PyModule_AddIntConstant(mod, "xplm_key_revthrust", xplm_key_revthrust);
    PyModule_AddIntConstant(mod, "xplm_key_jettison", xplm_key_jettison);
    PyModule_AddIntConstant(mod, "xplm_key_brakesreg", xplm_key_brakesreg);
    PyModule_AddIntConstant(mod, "xplm_key_brakesmax", xplm_key_brakesmax);
    PyModule_AddIntConstant(mod, "xplm_key_gear", xplm_key_gear);
    PyModule_AddIntConstant(mod, "xplm_key_timedn", xplm_key_timedn);
    PyModule_AddIntConstant(mod, "xplm_key_timeup", xplm_key_timeup);
    PyModule_AddIntConstant(mod, "xplm_key_fadec", xplm_key_fadec);
    PyModule_AddIntConstant(mod, "xplm_key_otto_dis", xplm_key_otto_dis);
    PyModule_AddIntConstant(mod, "xplm_key_otto_atr", xplm_key_otto_atr);
    PyModule_AddIntConstant(mod, "xplm_key_otto_asi", xplm_key_otto_asi);
    PyModule_AddIntConstant(mod, "xplm_key_otto_hdg", xplm_key_otto_hdg);
    PyModule_AddIntConstant(mod, "xplm_key_otto_gps", xplm_key_otto_gps);
    PyModule_AddIntConstant(mod, "xplm_key_otto_lev", xplm_key_otto_lev);
    PyModule_AddIntConstant(mod, "xplm_key_otto_hnav", xplm_key_otto_hnav);
    PyModule_AddIntConstant(mod, "xplm_key_otto_alt", xplm_key_otto_alt);
    PyModule_AddIntConstant(mod, "xplm_key_otto_vvi", xplm_key_otto_vvi);
    PyModule_AddIntConstant(mod, "xplm_key_otto_vnav", xplm_key_otto_vnav);
    PyModule_AddIntConstant(mod, "xplm_key_otto_nav1", xplm_key_otto_nav1);
    PyModule_AddIntConstant(mod, "xplm_key_otto_nav2", xplm_key_otto_nav2);
    PyModule_AddIntConstant(mod, "xplm_key_targ_dn", xplm_key_targ_dn);
    PyModule_AddIntConstant(mod, "xplm_key_targ_up", xplm_key_targ_up);
    PyModule_AddIntConstant(mod, "xplm_key_hdgdn", xplm_key_hdgdn);
    PyModule_AddIntConstant(mod, "xplm_key_hdgup", xplm_key_hdgup);
    PyModule_AddIntConstant(mod, "xplm_key_barodn", xplm_key_barodn);
    PyModule_AddIntConstant(mod, "xplm_key_baroup", xplm_key_baroup);
    PyModule_AddIntConstant(mod, "xplm_key_obs1dn", xplm_key_obs1dn);
    PyModule_AddIntConstant(mod, "xplm_key_obs1up", xplm_key_obs1up);
    PyModule_AddIntConstant(mod, "xplm_key_obs2dn", xplm_key_obs2dn);
    PyModule_AddIntConstant(mod, "xplm_key_obs2up", xplm_key_obs2up);
    PyModule_AddIntConstant(mod, "xplm_key_com1_1", xplm_key_com1_1);
    PyModule_AddIntConstant(mod, "xplm_key_com1_2", xplm_key_com1_2);
    PyModule_AddIntConstant(mod, "xplm_key_com1_3", xplm_key_com1_3);
    PyModule_AddIntConstant(mod, "xplm_key_com1_4", xplm_key_com1_4);
    PyModule_AddIntConstant(mod, "xplm_key_nav1_1", xplm_key_nav1_1);
    PyModule_AddIntConstant(mod, "xplm_key_nav1_2", xplm_key_nav1_2);
    PyModule_AddIntConstant(mod, "xplm_key_nav1_3", xplm_key_nav1_3);
    PyModule_AddIntConstant(mod, "xplm_key_nav1_4", xplm_key_nav1_4);
    PyModule_AddIntConstant(mod, "xplm_key_com2_1", xplm_key_com2_1);
    PyModule_AddIntConstant(mod, "xplm_key_com2_2", xplm_key_com2_2);
    PyModule_AddIntConstant(mod, "xplm_key_com2_3", xplm_key_com2_3);
    PyModule_AddIntConstant(mod, "xplm_key_com2_4", xplm_key_com2_4);
    PyModule_AddIntConstant(mod, "xplm_key_nav2_1", xplm_key_nav2_1);
    PyModule_AddIntConstant(mod, "xplm_key_nav2_2", xplm_key_nav2_2);
    PyModule_AddIntConstant(mod, "xplm_key_nav2_3", xplm_key_nav2_3);
    PyModule_AddIntConstant(mod, "xplm_key_nav2_4", xplm_key_nav2_4);
    PyModule_AddIntConstant(mod, "xplm_key_adf_1", xplm_key_adf_1);
    PyModule_AddIntConstant(mod, "xplm_key_adf_2", xplm_key_adf_2);
    PyModule_AddIntConstant(mod, "xplm_key_adf_3", xplm_key_adf_3);
    PyModule_AddIntConstant(mod, "xplm_key_adf_4", xplm_key_adf_4);
    PyModule_AddIntConstant(mod, "xplm_key_adf_5", xplm_key_adf_5);
    PyModule_AddIntConstant(mod, "xplm_key_adf_6", xplm_key_adf_6);
    PyModule_AddIntConstant(mod, "xplm_key_transpon_1", xplm_key_transpon_1);
    PyModule_AddIntConstant(mod, "xplm_key_transpon_2", xplm_key_transpon_2);
    PyModule_AddIntConstant(mod, "xplm_key_transpon_3", xplm_key_transpon_3);
    PyModule_AddIntConstant(mod, "xplm_key_transpon_4", xplm_key_transpon_4);
    PyModule_AddIntConstant(mod, "xplm_key_transpon_5", xplm_key_transpon_5);
    PyModule_AddIntConstant(mod, "xplm_key_transpon_6", xplm_key_transpon_6);
    PyModule_AddIntConstant(mod, "xplm_key_transpon_7", xplm_key_transpon_7);
    PyModule_AddIntConstant(mod, "xplm_key_transpon_8", xplm_key_transpon_8);
    PyModule_AddIntConstant(mod, "xplm_key_flapsup", xplm_key_flapsup);
    PyModule_AddIntConstant(mod, "xplm_key_flapsdn", xplm_key_flapsdn);
    PyModule_AddIntConstant(mod, "xplm_key_cheatoff", xplm_key_cheatoff);
    PyModule_AddIntConstant(mod, "xplm_key_cheaton", xplm_key_cheaton);
    PyModule_AddIntConstant(mod, "xplm_key_sbrkoff", xplm_key_sbrkoff);
    PyModule_AddIntConstant(mod, "xplm_key_sbrkon", xplm_key_sbrkon);
    PyModule_AddIntConstant(mod, "xplm_key_ailtrimL", xplm_key_ailtrimL);
    PyModule_AddIntConstant(mod, "xplm_key_ailtrimR", xplm_key_ailtrimR);
    PyModule_AddIntConstant(mod, "xplm_key_rudtrimL", xplm_key_rudtrimL);
    PyModule_AddIntConstant(mod, "xplm_key_rudtrimR", xplm_key_rudtrimR);
    PyModule_AddIntConstant(mod, "xplm_key_elvtrimD", xplm_key_elvtrimD);
    PyModule_AddIntConstant(mod, "xplm_key_elvtrimU", xplm_key_elvtrimU);
    PyModule_AddIntConstant(mod, "xplm_key_forward", xplm_key_forward);
    PyModule_AddIntConstant(mod, "xplm_key_down", xplm_key_down);
    PyModule_AddIntConstant(mod, "xplm_key_left", xplm_key_left);
    PyModule_AddIntConstant(mod, "xplm_key_right", xplm_key_right);
    PyModule_AddIntConstant(mod, "xplm_key_back", xplm_key_back);
    PyModule_AddIntConstant(mod, "xplm_key_tower", xplm_key_tower);
    PyModule_AddIntConstant(mod, "xplm_key_runway", xplm_key_runway);
    PyModule_AddIntConstant(mod, "xplm_key_chase", xplm_key_chase);
    PyModule_AddIntConstant(mod, "xplm_key_free1", xplm_key_free1);
    PyModule_AddIntConstant(mod, "xplm_key_free2", xplm_key_free2);
    PyModule_AddIntConstant(mod, "xplm_key_spot", xplm_key_spot);
    PyModule_AddIntConstant(mod, "xplm_key_fullscrn1", xplm_key_fullscrn1);
    PyModule_AddIntConstant(mod, "xplm_key_fullscrn2", xplm_key_fullscrn2);
    PyModule_AddIntConstant(mod, "xplm_key_tanspan", xplm_key_tanspan);
    PyModule_AddIntConstant(mod, "xplm_key_smoke", xplm_key_smoke);
    PyModule_AddIntConstant(mod, "xplm_key_map", xplm_key_map);
    PyModule_AddIntConstant(mod, "xplm_key_zoomin", xplm_key_zoomin);
    PyModule_AddIntConstant(mod, "xplm_key_zoomout", xplm_key_zoomout);
    PyModule_AddIntConstant(mod, "xplm_key_cycledump", xplm_key_cycledump);
    PyModule_AddIntConstant(mod, "xplm_key_replay", xplm_key_replay);
    PyModule_AddIntConstant(mod, "xplm_key_tranID", xplm_key_tranID);
    PyModule_AddIntConstant(mod, "xplm_key_max", xplm_key_max);
    
    /*
     * XPLMCommandButtonID
     * 
     * These are enumerations for all of the things you can do with a joystick 
     * button in X-Plane.  They currently match the buttons menu in the equipment 
     * setup dialog, but these enums will be stable even if they change in 
     * X-Plane.                                                                    
     *
     */
    PyModule_AddIntConstant(mod, "xplm_joy_nothing", xplm_joy_nothing);
    PyModule_AddIntConstant(mod, "xplm_joy_start_all", xplm_joy_start_all);
    PyModule_AddIntConstant(mod, "xplm_joy_start_0", xplm_joy_start_0);
    PyModule_AddIntConstant(mod, "xplm_joy_start_1", xplm_joy_start_1);
    PyModule_AddIntConstant(mod, "xplm_joy_start_2", xplm_joy_start_2);
    PyModule_AddIntConstant(mod, "xplm_joy_start_3", xplm_joy_start_3);
    PyModule_AddIntConstant(mod, "xplm_joy_start_4", xplm_joy_start_4);
    PyModule_AddIntConstant(mod, "xplm_joy_start_5", xplm_joy_start_5);
    PyModule_AddIntConstant(mod, "xplm_joy_start_6", xplm_joy_start_6);
    PyModule_AddIntConstant(mod, "xplm_joy_start_7", xplm_joy_start_7);
    PyModule_AddIntConstant(mod, "xplm_joy_throt_up", xplm_joy_throt_up);
    PyModule_AddIntConstant(mod, "xplm_joy_throt_dn", xplm_joy_throt_dn);
    PyModule_AddIntConstant(mod, "xplm_joy_prop_up", xplm_joy_prop_up);
    PyModule_AddIntConstant(mod, "xplm_joy_prop_dn", xplm_joy_prop_dn);
    PyModule_AddIntConstant(mod, "xplm_joy_mixt_up", xplm_joy_mixt_up);
    PyModule_AddIntConstant(mod, "xplm_joy_mixt_dn", xplm_joy_mixt_dn);
    PyModule_AddIntConstant(mod, "xplm_joy_carb_tog", xplm_joy_carb_tog);
    PyModule_AddIntConstant(mod, "xplm_joy_carb_on", xplm_joy_carb_on);
    PyModule_AddIntConstant(mod, "xplm_joy_carb_off", xplm_joy_carb_off);
    PyModule_AddIntConstant(mod, "xplm_joy_trev", xplm_joy_trev);
    PyModule_AddIntConstant(mod, "xplm_joy_trm_up", xplm_joy_trm_up);
    PyModule_AddIntConstant(mod, "xplm_joy_trm_dn", xplm_joy_trm_dn);
    PyModule_AddIntConstant(mod, "xplm_joy_rot_trm_up", xplm_joy_rot_trm_up);
    PyModule_AddIntConstant(mod, "xplm_joy_rot_trm_dn", xplm_joy_rot_trm_dn);
    PyModule_AddIntConstant(mod, "xplm_joy_rud_lft", xplm_joy_rud_lft);
    PyModule_AddIntConstant(mod, "xplm_joy_rud_cntr", xplm_joy_rud_cntr);
    PyModule_AddIntConstant(mod, "xplm_joy_rud_rgt", xplm_joy_rud_rgt);
    PyModule_AddIntConstant(mod, "xplm_joy_ail_lft", xplm_joy_ail_lft);
    PyModule_AddIntConstant(mod, "xplm_joy_ail_cntr", xplm_joy_ail_cntr);
    PyModule_AddIntConstant(mod, "xplm_joy_ail_rgt", xplm_joy_ail_rgt);
    PyModule_AddIntConstant(mod, "xplm_joy_B_rud_lft", xplm_joy_B_rud_lft);
    PyModule_AddIntConstant(mod, "xplm_joy_B_rud_rgt", xplm_joy_B_rud_rgt);
    PyModule_AddIntConstant(mod, "xplm_joy_look_up", xplm_joy_look_up);
    PyModule_AddIntConstant(mod, "xplm_joy_look_dn", xplm_joy_look_dn);
    PyModule_AddIntConstant(mod, "xplm_joy_look_lft", xplm_joy_look_lft);
    PyModule_AddIntConstant(mod, "xplm_joy_look_rgt", xplm_joy_look_rgt);
    PyModule_AddIntConstant(mod, "xplm_joy_glance_l", xplm_joy_glance_l);
    PyModule_AddIntConstant(mod, "xplm_joy_glance_r", xplm_joy_glance_r);
    PyModule_AddIntConstant(mod, "xplm_joy_v_fnh", xplm_joy_v_fnh);
    PyModule_AddIntConstant(mod, "xplm_joy_v_fwh", xplm_joy_v_fwh);
    PyModule_AddIntConstant(mod, "xplm_joy_v_tra", xplm_joy_v_tra);
    PyModule_AddIntConstant(mod, "xplm_joy_v_twr", xplm_joy_v_twr);
    PyModule_AddIntConstant(mod, "xplm_joy_v_run", xplm_joy_v_run);
    PyModule_AddIntConstant(mod, "xplm_joy_v_cha", xplm_joy_v_cha);
    PyModule_AddIntConstant(mod, "xplm_joy_v_fr1", xplm_joy_v_fr1);
    PyModule_AddIntConstant(mod, "xplm_joy_v_fr2", xplm_joy_v_fr2);
    PyModule_AddIntConstant(mod, "xplm_joy_v_spo", xplm_joy_v_spo);
    PyModule_AddIntConstant(mod, "xplm_joy_flapsup", xplm_joy_flapsup);
    PyModule_AddIntConstant(mod, "xplm_joy_flapsdn", xplm_joy_flapsdn);
    PyModule_AddIntConstant(mod, "xplm_joy_vctswpfwd", xplm_joy_vctswpfwd);
    PyModule_AddIntConstant(mod, "xplm_joy_vctswpaft", xplm_joy_vctswpaft);
    PyModule_AddIntConstant(mod, "xplm_joy_gear_tog", xplm_joy_gear_tog);
    PyModule_AddIntConstant(mod, "xplm_joy_gear_up", xplm_joy_gear_up);
    PyModule_AddIntConstant(mod, "xplm_joy_gear_down", xplm_joy_gear_down);
    PyModule_AddIntConstant(mod, "xplm_joy_lft_brake", xplm_joy_lft_brake);
    PyModule_AddIntConstant(mod, "xplm_joy_rgt_brake", xplm_joy_rgt_brake);
    PyModule_AddIntConstant(mod, "xplm_joy_brakesREG", xplm_joy_brakesREG);
    PyModule_AddIntConstant(mod, "xplm_joy_brakesMAX", xplm_joy_brakesMAX);
    PyModule_AddIntConstant(mod, "xplm_joy_speedbrake", xplm_joy_speedbrake);
    PyModule_AddIntConstant(mod, "xplm_joy_ott_dis", xplm_joy_ott_dis);
    PyModule_AddIntConstant(mod, "xplm_joy_ott_atr", xplm_joy_ott_atr);
    PyModule_AddIntConstant(mod, "xplm_joy_ott_asi", xplm_joy_ott_asi);
    PyModule_AddIntConstant(mod, "xplm_joy_ott_hdg", xplm_joy_ott_hdg);
    PyModule_AddIntConstant(mod, "xplm_joy_ott_alt", xplm_joy_ott_alt);
    PyModule_AddIntConstant(mod, "xplm_joy_ott_vvi", xplm_joy_ott_vvi);
    PyModule_AddIntConstant(mod, "xplm_joy_tim_start", xplm_joy_tim_start);
    PyModule_AddIntConstant(mod, "xplm_joy_tim_reset", xplm_joy_tim_reset);
    PyModule_AddIntConstant(mod, "xplm_joy_ecam_up", xplm_joy_ecam_up);
    PyModule_AddIntConstant(mod, "xplm_joy_ecam_dn", xplm_joy_ecam_dn);
    PyModule_AddIntConstant(mod, "xplm_joy_fadec", xplm_joy_fadec);
    PyModule_AddIntConstant(mod, "xplm_joy_yaw_damp", xplm_joy_yaw_damp);
    PyModule_AddIntConstant(mod, "xplm_joy_art_stab", xplm_joy_art_stab);
    PyModule_AddIntConstant(mod, "xplm_joy_chute", xplm_joy_chute);
    PyModule_AddIntConstant(mod, "xplm_joy_JATO", xplm_joy_JATO);
    PyModule_AddIntConstant(mod, "xplm_joy_arrest", xplm_joy_arrest);
    PyModule_AddIntConstant(mod, "xplm_joy_jettison", xplm_joy_jettison);
    PyModule_AddIntConstant(mod, "xplm_joy_fuel_dump", xplm_joy_fuel_dump);
    PyModule_AddIntConstant(mod, "xplm_joy_puffsmoke", xplm_joy_puffsmoke);
    PyModule_AddIntConstant(mod, "xplm_joy_prerotate", xplm_joy_prerotate);
    PyModule_AddIntConstant(mod, "xplm_joy_UL_prerot", xplm_joy_UL_prerot);
    PyModule_AddIntConstant(mod, "xplm_joy_UL_collec", xplm_joy_UL_collec);
    PyModule_AddIntConstant(mod, "xplm_joy_TOGA", xplm_joy_TOGA);
    PyModule_AddIntConstant(mod, "xplm_joy_shutdown", xplm_joy_shutdown);
    PyModule_AddIntConstant(mod, "xplm_joy_con_atc", xplm_joy_con_atc);
    PyModule_AddIntConstant(mod, "xplm_joy_fail_now", xplm_joy_fail_now);
    PyModule_AddIntConstant(mod, "xplm_joy_pause", xplm_joy_pause);
    PyModule_AddIntConstant(mod, "xplm_joy_rock_up", xplm_joy_rock_up);
    PyModule_AddIntConstant(mod, "xplm_joy_rock_dn", xplm_joy_rock_dn);
    PyModule_AddIntConstant(mod, "xplm_joy_rock_lft", xplm_joy_rock_lft);
    PyModule_AddIntConstant(mod, "xplm_joy_rock_rgt", xplm_joy_rock_rgt);
    PyModule_AddIntConstant(mod, "xplm_joy_rock_for", xplm_joy_rock_for);
    PyModule_AddIntConstant(mod, "xplm_joy_rock_aft", xplm_joy_rock_aft);
    PyModule_AddIntConstant(mod, "xplm_joy_idle_hilo", xplm_joy_idle_hilo);
    PyModule_AddIntConstant(mod, "xplm_joy_lanlights", xplm_joy_lanlights);
    PyModule_AddIntConstant(mod, "xplm_joy_max", xplm_joy_max);

    /*
     * XPLMHostApplicationID
     * 
     * The plug-in system is based on Austin's cross-platform OpenGL framework and 
     * could theoretically be adapted to  run in other apps like WorldMaker.  The 
     * plug-in system also runs against a test harness for internal development 
     * and could be adapted to another flight sim (in theory at least).  So an ID 
     * is providing allowing plug-ins to  indentify what app they are running 
     * under.                                                                      
     *
     */
    PyModule_AddIntConstant(mod, "xplm_Host_Unknown", xplm_Host_Unknown);
    PyModule_AddIntConstant(mod, "xplm_Host_XPlane", xplm_Host_XPlane);
    PyModule_AddIntConstant(mod, "xplm_Host_PlaneMaker", xplm_Host_PlaneMaker);
    PyModule_AddIntConstant(mod, "xplm_Host_WorldMaker", xplm_Host_WorldMaker);
    PyModule_AddIntConstant(mod, "xplm_Host_Briefer", xplm_Host_Briefer);
    PyModule_AddIntConstant(mod, "xplm_Host_PartMaker", xplm_Host_PartMaker);
    PyModule_AddIntConstant(mod, "xplm_Host_YoungsMod", xplm_Host_YoungsMod);
    PyModule_AddIntConstant(mod, "xplm_Host_XAuto", xplm_Host_XAuto);

    /*
     * XPLMLanguageCode
     * 
     * These enums define what language the sim is running in.  These enumerations 
     * do not imply that the sim can or does run in all of these languages; they 
     * simply provide a known encoding in the event that a given sim version is 
     * localized to a certain language.                                            
     *
     */
    PyModule_AddIntConstant(mod, "xplm_Language_Unknown", xplm_Language_Unknown);
    PyModule_AddIntConstant(mod, "xplm_Language_English", xplm_Language_English);
    PyModule_AddIntConstant(mod, "xplm_Language_French", xplm_Language_French);
    PyModule_AddIntConstant(mod, "xplm_Language_German", xplm_Language_German);
    PyModule_AddIntConstant(mod, "xplm_Language_Italian", xplm_Language_Italian);
    PyModule_AddIntConstant(mod, "xplm_Language_Spanish", xplm_Language_Spanish);
    PyModule_AddIntConstant(mod, "xplm_Language_Korean", xplm_Language_Korean);
    PyModule_AddIntConstant(mod, "xplm_Language_Russian", xplm_Language_Russian);
    PyModule_AddIntConstant(mod, "xplm_Language_Greek", xplm_Language_Greek);
    PyModule_AddIntConstant(mod, "xplm_Language_Japanese", xplm_Language_Japanese);
    PyModule_AddIntConstant(mod, "xplm_Language_Chinese", xplm_Language_Chinese);

    /*
     * XPLMDataFileType
     * 
     * These enums define types of data files you can load or unload using the 
     * SDK.                                                                        
     *
     */
    PyModule_AddIntConstant(mod, "xplm_DataFile_Situation", xplm_DataFile_Situation);
    PyModule_AddIntConstant(mod, "xplm_DataFile_ReplayMovie", xplm_DataFile_ReplayMovie);

    
    /*
     * XPLMCommandPhase
     * 
     * The phases of a command.                                                    
     *
     */
    PyModule_AddIntConstant(mod, "xplm_CommandBegin", xplm_CommandBegin);
    PyModule_AddIntConstant(mod, "xplm_CommandContinue", xplm_CommandContinue);
    PyModule_AddIntConstant(mod, "xplm_CommandEnd", xplm_CommandEnd);

  }

  return mod;
}


