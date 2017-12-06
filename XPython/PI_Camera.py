#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPLMCamera import *

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'Camera');
   
   def XPluginStart(self):
      self.Name = "Camera regression test"
      self.Sig = "CameraRT"
      self.Desc = "Regression test for XPLMCamera module"

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      self.int0Dref = XPLMFindDataRef('camera/int0')

      self.cbkCalled = 0
      self.howLong = 1516
      self.camCbk = self.cameraCallback
      self.pos = [1517, 1518, 1519, 1520, 1521, 1522, 1523]
      self.cbkRetVal = 1524
      XPLMControlCamera(self, self.howLong, self.camCbk, self.pos)
      self.checkVal('Camera callback not called', self.cbkCalled, 1)
      self.checkVal('Camera callback return value incorrect', XPLMGetDatai(self.int0Dref), self.cbkRetVal)

      XPLMDontControlCamera()
      self.checkVal('XPLMDontControlCamera was not called properly', XPLMGetDatai(self.int0Dref), -1)

      res, duration = XPLMIsCameraBeingControlled()
      self.checkVal('XPLMIsCameraBeingControlled return value incorrect', res, XPLMGetDatai(self.int0Dref))
      self.checkVal('XPLMIsCameraBeingControlled outCameraControlDuration value incorrect',
                    duration, self.howLong)

      testPos = []
      XPLMReadCameraPosition(testPos)
      self.checkVal('XPLMReadCameraPosition outCameraPosition incorrect', testPos, self.pos)

      return 1.0

   def cameraCallback(self, outCameraPosition, inIsLosingControl, inRefcon):
      self.cbkCalled = 1;
      outCameraPosition[:] = inRefcon;
      self.checkVal('cameraCallback\'s inIsLosingControl is not passed correctly', inIsLosingControl, self.howLong)
      return self.cbkRetVal

 
