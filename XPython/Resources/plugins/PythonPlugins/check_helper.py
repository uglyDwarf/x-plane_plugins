#!/usr/bin/env python3
#

from XPLMDataAccess import *

class checkBase(object):
   _all_errors = 0
   _refs = 0

   @classmethod
   def addErrors(cls, err):
      cls._all_errors += err

   @classmethod
   def addRef(cls):
      cls._refs += 1

   @classmethod
   def remRef(cls):
      cls._refs -= 1
      if(cls._refs == 0):
        if cls._all_errors == 0:
           print("No errors found.")
        else:
           print("{} errors encountered!".format(cls._all_errors))


   def __init__(self, moduleName, expectedChecks = None):
      self._modName = moduleName
      self._errors = 0
      self._checks = 0
      self._expected = expectedChecks

   def check(self):
      if self._errors == 0:
         if(self._expected == None) or (self._checks == self._expected):
            print('{0} module check OK ({1} checks passed).'.format(self._modName, self._checks))
         else:
            print('{0} module check found no errors, but only {1} of {2} checks were executed.'.format(
               self._modName, self._checks, self._expected))
      else:
         print('{0} module check: {1} errors found.'.format(self._modName, self._errors))
         self.addErrors(self._errors)

   def floatEq(self, a, b):
      if (a == None) or (b == None):
         return False
      #needed to be able to compare higher values...
      if abs(1 - float(a) / float(b)) < 1e-5:
         return True
      return False

   def error(self, message):
      print(' ** ERROR ** {0}'.format(message))
      self._errors += 1
      self._all_errors += 1

   def listOrTuple(self, v):
      return isinstance(v, list) or isinstance(v, tuple)

   def compare(self, v1, v2):
      if self.listOrTuple(v1) and self.listOrTuple(v2):
         if len(v1) != len(v2):
            return False
         for e1, e2 in zip(v1, v2):
            if not self.compare(e1, e2):
               return False
         return True
      elif isinstance(v1, float) or isinstance(v2, float):
         return self.floatEq(v1, v2)
      else:
         return v1 == v2

   def checkVal(self, prompt, got, expected):
      self._checks += 1
      if not self.compare(got, expected):
         self.error('{0}: got {1}, expected {2}'.format(prompt, got, expected))

   def getString(self, dataref):
      out = []
      res = XPLMGetDatab(dataref, out, 0, 256)
      outStr = ""
      for x in out:
         outStr += chr(x)
      return outStr

   

