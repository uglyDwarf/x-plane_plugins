#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPLMScenery import *
from XPLMInstance import *
from XPLMUtilities import *

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'Instance');
   
   def XPluginStart(self):
      self.Name = "Instance regression test"
      self.Sig = "InstanceRT"
      self.Desc = "Regression test for XPLMInstance module"
      self.versions = XPLMGetVersions()

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      if self.versions[1] < 300:
         return None

      self.str0Dref = XPLMFindDataRef('instance.str')
      self.int0Dref = XPLMFindDataRef('instance.int0')
      self.float0Dref = XPLMFindDataRef('instance.float0')
      self.xDref = XPLMFindDataRef('instance.x')
      self.yDref = XPLMFindDataRef('instance.y')
      self.zDref = XPLMFindDataRef('instance.z')
      self.pitchDref = XPLMFindDataRef('instance.pitch')
      self.headingDref = XPLMFindDataRef('instance.heading')
      self.rollDref = XPLMFindDataRef('instance.roll')

      obj = XPLMLoadObject('obj')
      datarefs = ('dref1', 'dref2')
      inst = XPLMCreateInstance(obj, datarefs)
      self.checkVal('XPLMCreateInstance:datagefs', self.getString(self.str0Dref), 'obj{}'.format(''.join(datarefs)))

      pos = (11, 22, 33, 44, 55, 66)
      data = (123.456, 789.012)
      XPLMInstanceSetPosition(inst, pos, data)
      self.checkVal('XPLMInstanceSetPosition:pos', (XPLMGetDataf(self.xDref), XPLMGetDataf(self.yDref),
                                                    XPLMGetDataf(self.zDref), XPLMGetDataf(self.pitchDref),
                                                    XPLMGetDataf(self.headingDref), XPLMGetDataf(self.rollDref)),
                                                    pos)
      self.checkVal('XPLMInstanceSetPosition:data', XPLMGetDataf(self.float0Dref), sum(data))

      XPLMDestroyInstance(inst)
      self.checkVal('XPLMDestroyInstance', XPLMGetDatai(self.int0Dref), 365)
      XPLMUnloadObject(obj)
      return None

 
