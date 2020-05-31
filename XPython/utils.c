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
  if(ref == Py_None){
    return NULL;
  }else{
    return PyCapsule_GetPointer(ref, refName);
  }
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

char *get_module(PyThreadState *tstate) {
  /* returns filename of top most frame -- this will be the Plugin's file */
  char *last_filename = "[unknown]";
  if (NULL != tstate && NULL != tstate->frame) {
    PyFrameObject *frame = tstate->frame;
    
    while (NULL != frame) {
      // int line = frame->f_lineno;
      /*
        frame->f_lineno will not always return the correct line number
        you need to call PyCode_Addr2Line().
      */
      // int line = PyCode_Addr2Line(frame->f_code, frame->f_lasti);
      PyObject *temp_bytes = PyUnicode_AsEncodedString(frame->f_code->co_filename, "utf-8", "replace");
      const char *filename = PyBytes_AsString(temp_bytes);
      filename = strdup(filename);
      last_filename = strdup(filename);
      Py_DECREF(temp_bytes);

      temp_bytes = PyUnicode_AsEncodedString(frame->f_code->co_name, "utf-8", "replace");
      const char *funcname = PyBytes_AsString(temp_bytes);
      funcname = strdup(funcname);
      Py_DECREF(temp_bytes);
      frame = frame->f_back;
    }
  }

  char *token = strrchr(last_filename, '/');
  if (token == NULL) {
    token = strrchr(last_filename, '\\');
    if (token == NULL) {
      token = strrchr(last_filename, ':');
    }
  }
  if (token) {
    return (++token);
  }
  return "Unknown";
}


