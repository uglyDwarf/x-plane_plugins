#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPLMCamera import *
from XPLMDisplay import *
from XPLMPlanes import *
from XPLMProcessing import *
from XPLMScenery import *
from XPLMUtilities import *

class PythonInterface(checkBase):
   def __init__(self):
      super().__init__('SLC');
   
   def XPluginStart(self):
      self.Name = "Self-less callbacks regression test"
      self.Sig = "SLCRT"
      self.Desc = "Regression test for self-less callbacks"

      self.msgCounter = 452
      self.versions = XPLMGetVersions()
      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      #############################################################################################
      # Camera
      #############################################################################################
      self.int0Dref = XPLMFindDataRef('camera/int0')

      self.cbkCalled = 0
      self.howLong = 1516
      self.camCbk = self.cameraCallback
      self.pos = [1517, 1518, 1519, 1520, 1521, 1522, 1523]
      self.cbkRetVal = 1524
      XPLMControlCamera(self.howLong, self.camCbk, self.pos)
      self.checkVal('Camera callback not called', self.cbkCalled, 3)
      self.checkVal('Camera callback return value incorrect', XPLMGetDatai(self.int0Dref), self.cbkRetVal)

      XPLMDontControlCamera()
      self.checkVal('XPLMDontControlCamera was not called properly', XPLMGetDatai(self.int0Dref), -1)
      #############################################################################################
      # Data access
      #############################################################################################
      self.getInt = self.getDatai; self.getIntRefcon = 44455444; self.intVal = 468;
      self.setInt = self.setDatai; self.setIntRefcon = 55544555;
      self.intDataref = XPLMRegisterDataAccessor('testSlc/int', xplmType_Int, True,
                                                 self.getInt, self.setInt,
                                                 None, None, #float
                                                 None, None, #double
                                                 None, None, #int vec
                                                 None, None, #float vec
                                                 None, None, #byte vec
                                                 self.getIntRefcon, self.setIntRefcon
                                                )

      self.shdfName = 'testSlc/floatShd'
      self.shdfType = xplmType_Float
      self.shdCbk = self.sharedDataCallback
      self.shdfCbkCalled = [self.msgCounter+10]
      self.shdfRefcon = self.shdfCbkCalled
      shdRes = XPLMShareData(self.shdfName, self.shdfType, self.shdCbk, self.shdfRefcon)
      self.checkVal('XPLMShareData failed', shdRes, 1)

      i = XPLMFindDataRef('testSlc/int')
      self.checkVal('Int dataref should be good.', XPLMIsDataRefGood(i), True)
      self.checkVal('Int dataref should be writable.', XPLMCanWriteDataRef(i), 1)
      self.checkVal('Int dataref has unexpected type.', XPLMGetDataRefTypes(i), xplmType_Int)
      newVal = XPLMGetDatai(i)
      self.checkVal('Int dataref value doesn\'t match.', newVal, self.intVal)
      newVal += 1
      XPLMSetDatai(i, newVal)
      self.checkVal('Int dataref new value doesn\'t match.', newVal, self.intVal)

      shdf = XPLMFindDataRef(self.shdfName)
      self.checkVal('Float shared data callback called prematurely.', self.shdfCbkCalled, [self.msgCounter + 10])
      self.msgCounter += 1
      XPLMSetDataf(shdf, 444.444)
      self.checkVal('Float shared data callback call number wrong.', self.shdfCbkCalled, [self.msgCounter +10])
      self.checkVal('Float shared data value doesn\'t match.', XPLMGetDataf(shdf), 444.444)

      XPLMUnregisterDataAccessor(self.intDataref)
      res = XPLMUnshareData(self.shdfName, self.shdfType, self.shdCbk, self.shdfRefcon)
      self.checkVal('XPLMUnshareData returned unexpected value.', res, 1)
      #############################################################################################
      # Display
      #############################################################################################
      #register drawing callback and store values to check
      self.drawCallbackFun = self.drawCallback
      self.drawPhase = xplm_Phase_Gauges
      self.drawBefore = 1
      self.drawRefcon = [432]
      i = XPLMRegisterDrawCallback(self.drawCallbackFun, self.drawPhase, self.drawBefore, self.drawRefcon)
      self.checkVal('Draw callback was not called', self.drawRefcon, [432, 49152])
      self.checkVal('Draw callback return value incorrect', i, self.drawRefcon[0])
      #register key sniffer
      self.keySnifferFun = self.keySniffer
      self.keySnifferRefcon = [433]
      self.keySnifferBeforeWin = 1
      i = XPLMRegisterKeySniffer(self.keySnifferFun, self.keySnifferBeforeWin, self.keySnifferRefcon)
      self.checkVal('Key sniffer callback was not called', self.keySnifferRefcon, [433, 49153])
      self.checkVal('Key sniffer callback return value incorrect', i, self.keySnifferRefcon[0])

      XPLMUnregisterDrawCallback(self.drawCallbackFun, self.drawPhase, self.drawBefore, self.drawRefcon)
      self.checkVal("Key sniffer unregistration", XPLMUnregisterKeySniffer(self.keySnifferFun,
                                                    self.keySnifferBeforeWin, self.keySnifferRefcon), 1);
      #############################################################################################
      # Planes
      #############################################################################################
      int0DrefPlanes = XPLMFindDataRef("planes/Int0")
      str0DrefPlanes = XPLMFindDataRef("planes/Str0")
      self.acquireCallback = self.acquired
      self.acquiredRefcon = []
      airplanes = ['F15 Eagle', 'Mig 21 Fishbed']
      res = XPLMAcquirePlanes(airplanes, self.acquireCallback, self.acquiredRefcon)
      self.checkVal('XPLMAcquirePlanes returned wrong value', res, len(airplanes))
      self.checkVal('XPLMAcquirePlanes didn\'t receive inAircraft correctly',
                    self.getString(str0DrefPlanes), ','.join(airplanes)+',')
      self.checkVal('XPLMAcquirePlanes didn\'t call the callback', self.acquiredRefcon, ['CalledSlc!'])

      res = XPLMGetDatai(int0DrefPlanes)
      XPLMReleasePlanes();
      self.checkVal('Problem calling XPLMReleasePlanes', XPLMGetDatai(int0DrefPlanes), res - 1)

      #############################################################################################
      # Processing
      #############################################################################################
      self.int0DrefProc = XPLMFindDataRef('processing/int0')
      self.int1DrefProc = XPLMFindDataRef('processing/int1')
      self.flt0DrefProc = XPLMFindDataRef('processing/flt0')
      self.flt1DrefProc = XPLMFindDataRef('processing/flt1')
      self.flt2DrefProc = XPLMFindDataRef('processing/flt2')

      self.fl0 = self.flightLoop1
      self.fl0Ref = []
      self.fl1 = self.flightLoop2
      self.fl1Ref = []

      self.fl1Interval = 32768.4096
      current = XPLMGetDataf(self.flt2DrefProc)
      XPLMRegisterFlightLoopCallback(self.fl0, self.fl1Interval, self.fl0Ref)
      self.checkVal('XPLMRegisterFlightLoopCallback interval passed incorrectly', 
                    XPLMGetDataf(self.flt0DrefProc), 1000 * self.fl1Interval)
      self.checkVal('XPLMRegisterFlightLoopCallback interval passed incorrectly', XPLMGetDataf(self.flt1DrefProc), 2 * self.fl1Interval)
      self.checkVal('XPLMRegisterFlightLoopCallback loopback wasn\'t called', self.fl0Ref, ["FlightLoop1 called."])
      self.checkVal('FlightLoopCallback1 return value incorrect', XPLMGetDataf(self.flt2DrefProc), current + 1.5)

      self.fl2Interval = 33.44
      relative = 32768
      XPLMSetFlightLoopCallbackInterval(self.fl0, self.fl2Interval, relative, self.fl0Ref)
      self.checkVal('XPLMSetFlightLoopCallbackInterval callback check failed', XPLMGetDatai(self.int0DrefProc), 1);
      self.checkVal('XPLMSetFlightLoopCallbackInterval inInterval passed incorrectly',
                    XPLMGetDataf(self.flt0DrefProc), self.fl2Interval);
      self.checkVal('XPLMSetFlightLoopCallbackInterval inRelativeToNow passed incorrectly',
                    XPLMGetDatai(self.int1DrefProc), relative);
      
      phase = 65535
      params = [phase, self.fl1, self.fl1Ref]
      try:
         flid = XPLMCreateFlightLoop(self, params)
      except RuntimeError as re:
         if (self.versions[1] >= 210) or (str(re) != 'XPLMCreateFlightLoop is available only in XPLM210 and up.'):
            raise
         flid = -1
      else:
         self.checkVal('XPLMCreateFlightLoop phase passed incorrectly',
                       XPLMGetDatai(self.int0DrefProc), phase);

      XPLMUnregisterFlightLoopCallback(self.fl0, self.fl0Ref)
      self.checkVal('XPLMUnregisterFlightLoopCallback failed to remove flightloop', XPLMGetDatai(self.int0DrefProc), 1)
      try:
         XPLMDestroyFlightLoop(flid)
      except RuntimeError as re:
         if (self.versions[1] >= 210) or (str(re) != 'XPLMDestroyFlightLoop is available only in XPLM210 and up.'):
            raise
      else:
         self.checkVal('XPLMDestroyFlightLoop failed', XPLMGetDatai(self.int0DrefProc), 123456)
      
      #############################################################################################
      # Scenery
      #############################################################################################
      locationXSc = XPLMFindDataRef("probe.locationX")
      locationYSc = XPLMFindDataRef("probe.locationY")
      pathSc = XPLMFindDataRef("obj.path")
      self.objPath = '/test/path/'
      self.objEnumerator = self.enumeratorCallback
      self.objects = []
      self.checkVal('XPLMLookupObjects returned unexpected number', 
         XPLMLookupObjects(self.objPath, 3.1415926, 2.7182818, self.objEnumerator, self.objects), 2)
      self.checkVal('XPLMLookupData inLatitude problem', XPLMGetDataf(locationXSc), 3.1415926)
      self.checkVal('XPLMLookupData inLongitude problem', XPLMGetDataf(locationYSc), 2.71827818)
      self.checkVal('XPLMLookupData inPath problem', self.getString(pathSc), self.objPath)

      for idx, path in enumerate(self.objects):
         self.checkVal('XPLMLookupData return path wrong', path, "{0}{1}".format(self.objPath, idx + 1))
      
      #############################################################################################
      # Utilities
      #############################################################################################
      self.dbgString = ''
      self.errCbk = self.errorCallback
      XPLMSetErrorCallback(self.errCbk)
      self.checkVal('Error callback called unexpectedly.', self.dbgString, '')
      tmp = XPLMFindSymbol('Abraca Dabra')
      self.checkVal('Error callback should have triggered by now.', self.dbgString,
                    "Roses are red, violets are blue and this code desperately needs some love...")

      path = 'Slc_test_command'
      self.cmdRef = XPLMCreateCommand(path, 'Slc_test command')
      self.checkVal('XPLMFindCommand didn\'t find my new command.', XPLMFindCommand(path), self.cmdRef)
      
      self.cmdHandler = self.commandCallback
      self.cmdStatus = [0, 0]
      XPLMRegisterCommandHandler(self.cmdRef, self.cmdHandler, 1, self.cmdStatus)
      XPLMCommandOnce(self.cmdRef)
      self.checkVal('XPLMCommandOnce behaviour incorrect (count).', self.cmdStatus[0], 2)
      self.checkVal('XPLMCommandOnce behaviour incorrect (stages).', self.cmdStatus[1], 5)

      XPLMUnregisterCommandHandler(self, self.cmdRef, self.cmdHandler, 1, self.cmdStatus)
      self.cmdStatus[0] = 0; self.cmdStatus[1] = 0
      XPLMCommandOnce(self.cmdRef)
      XPLMCommandBegin(self.cmdRef)
      XPLMCommandEnd(self.cmdRef)
      self.checkVal('XPLMUnregisterCommandHandler behaviour incorrect (count).', self.cmdStatus[0], 0)
      self.checkVal('XPLMUnregisterCommandHandler behaviour incorrect (stages).', self.cmdStatus[1], 0)
      return None

   def cameraCallback(self, outCameraPosition, inIsLosingControl, inRefcon):
      self.cbkCalled |= 1;
      if not inIsLosingControl:
         self.cbkCalled |= 2;
         outCameraPosition[:] = inRefcon
      else:
         self.checkVal('cameraCallback\'s inIsLosingControl is not passed correctly', inIsLosingControl, self.howLong)
      return self.cbkRetVal

   def getDatai(self, inRefcon):
      self.checkVal('getDatai refcon doesn\'t match.', inRefcon, self.getIntRefcon)
      return self.intVal

   def setDatai(self, inRefcon, inValue):
      self.checkVal('setDatai refcon doesn\'t match.', inRefcon, self.setIntRefcon)
      self.intVal = inValue
      return

   def sharedDataCallback(self, inRefcon):
      inRefcon[0] += 1

   def drawCallback(self, inPhase, inIsBefore, inRefcon):
      self.checkVal('drawCallback: Unexpected inPhase', inPhase, self.drawPhase)
      self.checkVal('drawCallback: Unexpected inIsBefore', inIsBefore, self.drawBefore)
      self.checkVal('drawCallback: Unexpected inRefcon', inRefcon, self.drawRefcon)
      inRefcon.append(49152)
      return inRefcon[0]

   def keySniffer(self, inChar, inFlags, inVirtualKey, inRefcon):
      self.checkVal('keySniffer: Unexpected key', chr(inChar), 'Q')
      self.checkVal('keySniffer: Unexpected flags', inFlags, xplm_ShiftFlag | xplm_DownFlag)
      self.checkVal('keySniffer: Unexpected virtual key', inVirtualKey, XPLM_VK_Q)
      self.checkVal('keySniffer: Unexpected refcon', inRefcon, self.keySnifferRefcon)
      inRefcon.append(49153)
      return inRefcon[0]

   def acquired(self, inRefcon):
      inRefcon.append('CalledSlc!')

   def flightLoop1(self, inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter, inRefcon):
      self.checkVal('FlightLoopCallback1 inElapsedSinceLastCall passed incorrectly', inElapsedSinceLastCall,
                    XPLMGetDataf(self.flt0DrefProc))
      self.checkVal('FlightLoopCallback1 inElapsedTimeSinceLastFlightLoop passed incorrectly',
                    inElapsedTimeSinceLastFlightLoop, XPLMGetDataf(self.flt1DrefProc) + self.fl1Interval)
      self.checkVal('FlightLoopCallback1 inCounter passed incorrectly', inCounter, XPLMGetDatai(self.int0DrefProc))
      inRefcon.append('FlightLoop1 called.')
      return 1.5

   def flightLoop2(self, inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter, inRefcon):
      self.checkVal('FlightLoopCallback2 inElapsedSinceLastCall passed incorrectly', inElapsedSinceLastCall,
                    XPLMGetDataf(self.flt0Dref) * 1000)
      self.checkVal('FlightLoopCallback2 inElapsedTimeSinceLastFlightLoop passed incorrectly',
                    inElapsedTimeSinceLastFlightLoop, XPLMGetDataf(self.flt0DrefProc) * 2 - 42)
      self.checkVal('FlightLoopCallback2 inCounter passed incorrectly', inCounter, XPLMGetDatai(self.int0DrefProc))
      inRefcon.append('FlightLoop2 called.')
      return 2.5

   def enumeratorCallback(self, inFilePath, inRef):
      inRef.append(inFilePath)
      self.checkVal('EnumeratorCallback path inconsistent', self.objects[-1], inRef[-1])

   def errorCallback(self, inMessage):
      self.dbgString = inMessage

   def commandCallback(self, inCommand, inPhase, inRefcon):
      self.checkVal('CommandCallback given unknown command reference.', inCommand, self.cmdRef)
      inRefcon[0] += 1
      inRefcon[1] |= 1 << inPhase
      return 1


