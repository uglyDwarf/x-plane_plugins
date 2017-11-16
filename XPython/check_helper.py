#!/usr/bin/env python3
#

from XPLMDataAccess import *

class checkBase(object):
   def __init__(self, moduleName):
      self._modName = moduleName
      self._errors = 0

   def check(self):
      if self._errors == 0:
         print('{0} module check OK.'.format(self._modName))
      else:
         print('{0} module check: {1} errors found.'.format(self._modName, self._errors))

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
      if not self.compare(got, expected):
         self.error('{0}: got {1}, expected {2}'.format(prompt, got, expected))

   def getString(self, dataref):
      out = []
      res = XPLMGetDatab(dataref, out, 0, 256)
      outStr = ""
      for x in out:
         outStr += chr(x)
      return outStr

   

