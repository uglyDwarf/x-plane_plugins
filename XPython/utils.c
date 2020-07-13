#define _GNU_SOURCE 1
#include <Python.h>
#include <frameobject.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils.h"

const char *objRefName = "XPLMObjectRef";
const char *commandRefName = "XPLMCommandRef";
const char *widgetRefName = "XPLMWidgetID";

void dbg(const char *msg){
  printf("Going to check %s\n", msg);
  PyObject *err = PyErr_Occurred();
  if(err){
    printf("Error occured during the %s call:\n", msg);
    PyErr_Print();
  }
}

char * objToStr(PyObject *item) {
  // returns char * pointer to something in heap
  PyObject *pyAsStr = PyObject_Str(item); // new object
  PyObject *pyBytes = PyUnicode_AsEncodedString(pyAsStr, "utf-8", "replace"); // new object
  char *res = PyBytes_AS_STRING(pyBytes);  //borrowed (from pyBytes)
  res = strdup(res); // allocated on heap
  Py_DECREF(pyAsStr);
  Py_DECREF(pyBytes);
  return res;
}
  
bool objToList(PyObject *item, PyObject *list)
{
  if(!PyList_Check(list)){
    return false;
  }
  // because this is used to set return values, we're handed a python list [], we return
  // the result as the FIRST list item. Now, if the user was careless & reused this
  // result list and we merely Appended, then user would get ever-growing results list.
  if(PyList_Size(list) > 0){
    PyList_SetItem(list, 0, item);
  }else{
    PyList_Append(list, item);
    Py_DECREF(item);
  }
  return true;
}

float getFloatFromTuple(PyObject *seq, Py_ssize_t i)
{
  return PyFloat_AsDouble(PyTuple_GetItem(seq, i));
}

long getLongFromTuple(PyObject *seq, Py_ssize_t i)
{
  return PyLong_AsLong(PyTuple_GetItem(seq, i));
}

// To avoid Python code messing with raw pointers (when passed
//   in using PyLong_FromVoidPtr), these are hidden in the capsules.

PyObject *get_pluginSelf() {
  // returns heap-allocated PyObject (or Py_RETURN_NONE)
  PyGILState_STATE gilState = PyGILState_Ensure();
  PyThreadState *tstate = PyThreadState_GET();
  PyObject *last_filenameObj = Py_None;
  if (NULL != tstate && NULL != tstate->frame) {
    PyFrameObject *frame = tstate->frame;
    while (NULL != frame) {
      // creates new PyObject
      last_filenameObj = frame->f_code->co_filename;
      frame = frame->f_back;
    }
  }

  char *last_filename = objToStr(last_filenameObj); // allocates new string on heap
  char *token = strrchr(last_filename, '/');
  if (token == NULL) {
    token = strrchr(last_filename, '\\');
    if (token == NULL) {
      token = strrchr(last_filename, ':');
    }
  }
  PyGILState_Release(gilState);
  if (token) {
    PyObject *ret = PyUnicode_FromString(++token); // return new item, we then free the char*
    free(last_filename);
    return ret;
  }
  free(last_filename);
  Py_RETURN_NONE;
}

// To avoid Python code messing with raw pointers (when passed
//   in using PyLong_FromVoidPtr), these are hidden in the capsules.

// Can be used where no callbacks are involved in passing the capsule
PyObject *getPtrRefOneshot(void *ptr, const char *refName)
{
  if(ptr){
    return PyCapsule_New(ptr, refName, NULL);
  }else{
    Py_RETURN_NONE;
  }
}

PyObject *getPtrRef(void *ptr, PyObject *dict, const char *refName)
{
  if(!ptr){
    Py_RETURN_NONE;
  }
  // Check if the refernece is known
  PyObject *key = PyLong_FromVoidPtr(ptr);
  PyObject *res = PyDict_GetItem(dict, key);
  if(res == NULL){
    // New ref, register it
    res = getPtrRefOneshot(ptr, refName);
    PyDict_SetItem(dict, key, res);
  }
  Py_INCREF(res);
  return res;
}

void *refToPtr(PyObject *ref, const char *refName)
{
  if(PyCapsule_CheckExact(ref)){
    return PyCapsule_GetPointer(ref, refName);
  }else if(ref == Py_None){
    return NULL;
  }else if(PyLong_Check(ref) && (PyLong_AsLong(ref) == 0)){
    return NULL;
  }
  // If we got here, then ref is something we didn't expect.
  PyErr_Format(PyExc_RuntimeError, "Error extracting %s reference from %S", refName, ref);
  return NULL;
}

void removePtrRef(void *ptr, PyObject *dict)
{
  if(!ptr){
    return;
  }
  PyObject *key = PyLong_FromVoidPtr(ptr);
  PyDict_DelItem(dict, key);
  Py_DECREF(key);
}


/*
  The following code works tries to go through the pains
  of (possible) multiply parsing of the path only once.
  The parsing function is called indirectly through the
  get_fname pointer; the default function does the initial
  check and sets the get_fname pointer to the appropriate
  version...

  Beware: Might it be fooled by a crefted path?
  Maybe the decision should be somehow tied to the platform,
  as the only problem is Mac - if the colon or slash is used.
*/
typedef const char *(*get_fname_t)(const char *fullName);
static const char *get_fname_generic(const char *fullName);
static get_fname_t get_fname = get_fname_generic; 

static const char *get_fname_lin(const char *fullName)
{
  return strrchr(fullName, '/');
}

static const char *get_fname_win(const char *fullName)
{
  return strrchr(fullName, '\\');
}

static const char *get_fname_mac(const char *fullName)
{
  return strrchr(fullName, '\\');
}

static const char *get_fname_generic(const char *fullName)
{
  char *token = NULL;
  token = strrchr(fullName, '/');
  if(token){
    get_fname = get_fname_lin;
    return token;
  }
  token = strrchr(fullName, '\\');
  if(token){
    get_fname = get_fname_win;
    return token;
  }
  token = strrchr(fullName, ':');
  if(token){
    get_fname = get_fname_mac;
    return token;
  }
  return NULL;
}


char *get_module(PyThreadState *tstate) {
  /* returns filename of top most frame -- this will be the Plugin's file */
  /*   Result must be freed after use!                                    */
  char *res = NULL;
  if (NULL != tstate && NULL != tstate->frame) {
    PyFrameObject *frame = tstate->frame;
    
    while (NULL != frame) {
      // int line = frame->f_lineno;
      /*
        frame->f_lineno will not always return the correct line number
        you need to call PyCode_Addr2Line().
      */
      // int line = PyCode_Addr2Line(frame->f_code, frame->f_lasti);
      frame = frame->f_back;
    }
    PyObject *fnameObj = PyUnicode_AsEncodedString(frame->f_code->co_filename, "utf-8", "replace");
    const char *filename = PyBytes_AsString(fnameObj);
    /* Unused...
      PyObject *funcNameObj = PyUnicode_AsEncodedString(frame->f_code->co_name, "utf-8", "replace");
      const char *funcname = PyBytes_AsString(funcNameObj);
      funcname = strdup(funcname);
      Py_DECREF(funcNameObj);
    */
    const char *last_path_separator = get_fname(filename);
    Py_DECREF(fnameObj);
    if(last_path_separator){
      ++last_path_separator;
      res = strdup(last_path_separator);
    }
  }

  if(!res){
    res = strdup("unknown file");
  }
  return res;
}

// Motivation:
// On Windows we are limited to the restricted API, that doesn't have
//   the PyUnicode_AsUTF8 available; instead we are forced to use the
//   PyUnicode_AsUTF8String/PyBytes_AsString combo, which creates
//   a Unicode object, that must be released when no longer needed
//   (e.g. after the XPLM... call).
// The implementation puts the object to the stringStash and the cleanup
//   must be called to release it; when more than a signle string is
//   needed, the previous content of the stringStash is appended to the
//   stringList, that will be destroyed once the cleanup is called,
//   taking all the allocated object with itself.
// This way, single string creation doesn't involve any list operations,
//   which should be more optimal, than putting everything to the list.
//
// TODO: Should this all be guarded by a gil or a mutex of some kind?
// For now, since XPLM functions should be called only from the main
//   thread, I'll leave it as it is...

// Last UTF8 string created
static PyObject *stringStash = NULL;
// List of previous 
static PyObject *stringList = NULL;

const char *asString(PyObject *obj)
{
  PyObject *str = PyObject_Str(obj);
  if(!str){
    return NULL;
  }
  PyObject *utfStr = PyUnicode_AsUTF8String(obj);
  Py_DECREF(str);
  if(!utfStr){
    return NULL;
  }
  if(stringStash){
    // Something is already in a stash
    if(stringList == NULL){
      stringList = PyList_New(1);
      if(!stringList){
        return NULL;
      }
      PyList_SET_ITEM(stringList, 0, stringStash);
    }else{
      PyList_Append(stringList, stringStash);
      // If this fails, we leak the object!
      // TODO: is there anything we can do about it?
    }
  }
  stringStash = utfStr;

  return PyBytes_AsString(stringStash);
}

void stringCleanup(void)
{
  Py_XDECREF(stringStash);
  stringStash = NULL;
  Py_XDECREF(stringList);
  stringList = NULL;
}

