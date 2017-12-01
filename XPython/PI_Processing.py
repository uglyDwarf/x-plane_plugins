#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPLMProcessing import *

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'Processing');
   
   def XPluginStart(self):
      self.Name = "Processing regression test"
      self.Sig = "ProcessingRT"
      self.Desc = "Regression test for XPLMProcessing module"

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      self.int0Dref = XPLMFindDataRef('processing/int0')
      self.int1Dref = XPLMFindDataRef('processing/int1')
      self.flt0Dref = XPLMFindDataRef('processing/flt0')
      self.flt1Dref = XPLMFindDataRef('processing/flt1')
      self.flt2Dref = XPLMFindDataRef('processing/flt2')

      tmp = XPLMGetElapsedTime()
      self.checkVal('XPLMGetElapsedTime returned wrong value', tmp, XPLMGetDataf(self.flt0Dref))

      tmp = XPLMGetCycleNumber()
      self.checkVal('XPLMGetCycleNumber returned wrong value', tmp, XPLMGetDatai(self.int0Dref))

      self.fl0 = self.flightLoop1
      self.fl0Ref = []
      self.fl1 = self.flightLoop2
      self.fl1Ref = []

      interval = 16384.8192
      XPLMRegisterFlightLoopCallback(self, self.fl0, interval, self.fl0Ref)
      self.checkVal('XPLMRegisterFlightLoopCallback interval passed incorrectly', 
                    XPLMGetDataf(self.flt0Dref), 1000 * interval)
      self.checkVal('XPLMRegisterFlightLoopCallback interval passed incorrectly', XPLMGetDataf(self.flt1Dref), 2 * interval)
      self.checkVal('XPLMRegisterFlightLoopCallback loopback wasn\'t called', self.fl0Ref, ["FlightLoop1 called."])
      self.checkVal('FlightLoopCallback1 return value incorrect', XPLMGetDataf(self.flt2Dref), 1.0)

      interval = 33.44
      relative = 32768
      XPLMSetFlightLoopCallbackInterval(self, self.fl0, interval, relative, self.fl0Ref)
      self.checkVal('XPLMSetFlightLoopCallbackInterval callback check failed', XPLMGetDatai(self.int0Dref), 1);
      self.checkVal('XPLMSetFlightLoopCallbackInterval inInterval passed incorrectly',
                    XPLMGetDataf(self.flt0Dref), interval);
      self.checkVal('XPLMSetFlightLoopCallbackInterval inRelativeToNow passed incorrectly',
                    XPLMGetDatai(self.int1Dref), relative);
      
      phase = 65535
      params = [phase, self.fl1, self.fl1Ref]
      flid = XPLMCreateFlightLoop(self, params)
      self.checkVal('XPLMCreateFlightLoop phase passed incorrectly',
                    XPLMGetDatai(self.int0Dref), phase);

      interval = 55.77
      relative = 4096
      XPLMScheduleFlightLoop(self, flid, interval, relative)
      self.checkVal('XPLMScheduleFlightLoop inInterval passed incorrectly',
                    XPLMGetDataf(self.flt0Dref), interval);
      self.checkVal('XPLMScheduleFlightLoop inRelativeToNow passed incorrectly',
                    XPLMGetDatai(self.int0Dref), relative);
      self.checkVal('XPLMCreateFlightLoop loopback wasn\'t called', self.fl1Ref, ["FlightLoop2 called."])
      self.checkVal('FlightLoopCallback2 return value incorrect', XPLMGetDataf(self.flt2Dref), 3.0)
 


      XPLMUnregisterFlightLoopCallback(self, self.fl0, self.fl0Ref)
      self.checkVal('XPLMUnregisterFlightLoopCallback failed to remove flightloop', XPLMGetDatai(self.int0Dref), 1)
      XPLMDestroyFlightLoop(self, flid)
      self.checkVal('XPLMDestroyFlightLoop failed', XPLMGetDatai(self.int0Dref), 123456)
      
      return 

   def flightLoop1(self, inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter, inRefcon):
      self.checkVal('FlightLoopCallback1 inElapsedSinceLastCall passed incorrectly', inElapsedSinceLastCall,
                    XPLMGetDataf(self.flt0Dref))
      self.checkVal('FlightLoopCallback1 inElapsedTimeSinceLastFlightLoop passed incorrectly',
                    inElapsedTimeSinceLastFlightLoop, XPLMGetDataf(self.flt1Dref))
      self.checkVal('FlightLoopCallback1 inCounter passed incorrectly', inCounter, XPLMGetDatai(self.int0Dref))
      inRefcon.append('FlightLoop1 called.')
      return 1.0

   def flightLoop2(self, inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter, inRefcon):
      print('flightLoop2 called!')
      self.checkVal('FlightLoopCallback2 inElapsedSinceLastCall passed incorrectly', inElapsedSinceLastCall,
                    XPLMGetDataf(self.flt0Dref) * 1000)
      self.checkVal('FlightLoopCallback2 inElapsedTimeSinceLastFlightLoop passed incorrectly',
                    inElapsedTimeSinceLastFlightLoop, XPLMGetDataf(self.flt0Dref) * 2)
      self.checkVal('FlightLoopCallback2 inCounter passed incorrectly', inCounter, XPLMGetDatai(self.int0Dref))
      inRefcon.append('FlightLoop2 called.')
      return 2.0

