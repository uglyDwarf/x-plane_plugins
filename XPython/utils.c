#define _GNU_SOURCE
#include <Python.h>
#include <stdio.h>
#include <stdbool.h>
#include "utils.h"


bool checkParams(PyObject *args, Py_ssize_t len, paramCheck_t *checks){
  bool paramsOK = true;
  paramCheck_t *check = checks;

  PyObject *obj = NULL;
  char *message = NULL;
  if(PySequence_Size(args) != len){
    asprintf(&message, "Sequence should have %d items.", (int)len);
    PyErr_SetString(PyExc_TypeError , message);
    return false;
  }
  while(check->index >= 0){
    obj = PySequence_GetItem(args, check->index);
    if(!obj){
      paramsOK = false;
      asprintf(&message, "Argument should have at least %d items.", check->index + 1);
      break;
    }
    switch(check->type){
      case 'n':
        if(!PyNumber_Check(obj)){
          asprintf(&message, "Argument number %d (%s) should be a number.", check->index, check->name);
          paramsOK = false;
        }
        break;
      case 'd':
        if(!PyLong_Check(obj)){
          asprintf(&message, "Argument number %d (%s) should be Floating point.", check->index, check->name);
          paramsOK = false;
        }
        break;
      case 'i':
        if(!PyLong_Check(obj)){
          asprintf(&message, "Argument number %d (%s) should be Long.", check->index, check->name);
          paramsOK = false;
        }
        break;
      case 'l':
        if(!PyList_Check(obj)){
          asprintf(&message, "Argument number %d (%s) should be list.", check->index, check->name);
          paramsOK = false;
        }
        break;
      case 'q':
        if(!PySequence_Check(obj)){
          asprintf(&message, "Argument number %d (%s) should be a sequence.", check->index, check->name);
          paramsOK = false;
        }
        break;
      case 'f':
        if(!PyCallable_Check(obj)){
          asprintf(&message, "Argument number %d (%s) should be callable.", check->index, check->name);
          paramsOK = false;
        }
        break;
      case 't':
        if(!PyTuple_Check(obj)){
          asprintf(&message, "Argument number %d (%s) should be Tuple.", check->index, check->name);
          paramsOK = false;
        }
        break;
      case 's':
        if(!PyUnicode_Check(obj)){
          asprintf(&message, "Argument number %d (%s) should be String.", check->index, check->name);
          paramsOK = false;
        }
      default:
        break;
    }
    if(!paramsOK){
      PyErr_SetString(PyExc_TypeError, message);
      break;
    }
    ++check;
  }
  return paramsOK;
}


