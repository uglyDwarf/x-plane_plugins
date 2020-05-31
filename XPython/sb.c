#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>

static PyObject *SandyBarbourDisplayFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *msg;
  if(!PyArg_ParseTuple(args, "s", &msg)){
    return NULL;
  }
  //TODO
  printf("SBD: %s\n", msg);
  Py_RETURN_NONE;
}

static PyObject *SandyBarbourClearDisplayFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  //TODO
  printf("SBD:======================================\n");
  Py_RETURN_NONE;
}

static PyObject *SandyBarbourPrintFun(PyObject *self, PyObject *args)
{
  (void) self;
  const char *msg;
  if(!PyArg_ParseTuple(args, "s", &msg)){
    return NULL;
  }
  //TODO
  printf("SBP: %s\n", msg);
  Py_RETURN_NONE;
}

static PyObject *SandyBarbourClearPrintFun(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  //TODO
  printf("SBP:======================================\n");
  Py_RETURN_NONE;
}

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}

static PyMethodDef SBUMethods[] = {
  {"SandyBarbourDisplay", SandyBarbourDisplayFun, METH_VARARGS, ""},
  {"SandyBarbourClearDisplay", SandyBarbourClearDisplayFun, METH_VARARGS, ""},
  {"SandyBarbourPrint", SandyBarbourPrintFun, METH_VARARGS, ""},
  {"SandyBarbourClear", SandyBarbourClearPrintFun, METH_VARARGS, ""},
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef SBUModule = {
  PyModuleDef_HEAD_INIT,
  "SandyBarbourUtilities",
  NULL,
  -1,
  SBUMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_SBU(void)
{
  PyObject *mod = PyModule_Create(&SBUModule);
  return mod;
}




