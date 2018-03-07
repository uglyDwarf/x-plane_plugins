#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPLMPlanes import *

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'Planes');
   
   def XPluginStart(self):
      self.Name = "Planes regression test"
      self.Sig = "PlanesRT"
      self.Desc = "Regression test for XPLMPlanes module"

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      str0Dref = XPLMFindDataRef("planes/Str0")
      str1Dref = XPLMFindDataRef("planes/Str1")
      int0Dref = XPLMFindDataRef("planes/Int0")
      int1Dref = XPLMFindDataRef("planes/Int1")
      inXValDref = XPLMFindDataRef("planes/inXVal")
      inYValDref = XPLMFindDataRef("planes/inYVal")
      inZValDref = XPLMFindDataRef("planes/inZVal")
      inPitchValDref = XPLMFindDataRef("planes/inPitchVal")
      inRollValDref = XPLMFindDataRef("planes/inRollVal")
      inYawValDref = XPLMFindDataRef("planes/inYawVal")
      structSizeDref = XPLMFindDataRef("planes/structSize")
      gearPositionDref = XPLMFindDataRef("planes/gearPosition")
      flapRatioDref = XPLMFindDataRef("planes/flapRatio")
      spoilerRatioDref = XPLMFindDataRef("planes/spoilerRatio")
      speedBrakeRatioDref = XPLMFindDataRef("planes/speedBrakeRatio")
      slatRatioDref = XPLMFindDataRef("planes/slatRatio")
      wingSweepDref = XPLMFindDataRef("planes/wingSweep")
      thrustDref = XPLMFindDataRef("planes/thrust")
      yokePitchDref = XPLMFindDataRef("planes/yokePitch")
      yokeHeadingDref = XPLMFindDataRef("planes/yokeHeading")
      yokeRollDref = XPLMFindDataRef("planes/yokeRoll")
      latitudeDref = XPLMFindDataRef("planes/latitude")
      longitudeDref = XPLMFindDataRef("planes/longitude")


      acf = 'F4 Phantom'
      XPLMSetUsersAircraft(acf)
      self.checkVal('XPLMSetUsersAircraft didn\'t pass the inAircraftPath parameter correctly',
                    self.getString(str0Dref), acf)
      airport = 'LKCM'
      XPLMPlaceUserAtAirport(airport)
      self.checkVal('XPLMPlaceUserAtAirport didn\'t pass the inAirportCode parameter correctly',
                    self.getString(str1Dref), airport)
      (lat, lon) = (3.14, 2.72)
      (alt, hdg, spd) = (10.2, 361.0, 660.2)
      XPLMPlaceUserAtLocation(lat, lon, alt, hdg, spd)
      self.checkVal('XPLMPlaceUserAtLocation:latitude', XPLMGetDatad(latitudeDref), lat)
      self.checkVal('XPLMPlaceUserAtLocation:longitude', XPLMGetDatad(longitudeDref), lat)
      self.checkVal('XPLMPlaceUserAtLocation:elevationMetersMSL', XPLMGetDatad(inXVal, alt)
      self.checkVal('XPLMPlaceUserAtLocation:headingDegreesTrue', XPLMGetDatad(inYVal, hdg)
      self.checkVal('XPLMPlaceUserAtLocation:speedMetersPerSecond', XPLMGetDatad(inZVal), spd)
      
      index = 7777;
      (outFileName, outPath) = XPLMGetNthAircraftModel(index)
      self.checkVal('XPLMGetNthAircraftModel returned wrong outFileName', outFileName, acf)
      self.checkVal('XPLMGetNthAircraftModel returned wrong outPath', outPath, airport)

      (outTotalAircraft, outActiveAircraft, outController) = XPLMCountAircraft()
      self.checkVal('XPLMCountAircraft didn\'t return the expected outTotalAircraft', outTotalAircraft, index + 42)
      self.checkVal('XPLMCountAircraft didn\'t return the expected outActiveAircraft', outActiveAircraft, index + 43)
      self.checkVal('XPLMCountAircraft didn\'t return the expected outTotalAircraft', outController, index + 44)

      self.acquireCallback = self.acquired
      self.acquiredRefcon = []
      airplanes = ['F16 Falcon', 'Su 27 Flanker', '']
      res = XPLMAcquirePlanes(self, airplanes, self.acquireCallback, self.acquiredRefcon)
      self.checkVal('XPLMAcquirePlanes returned wrong value', res, len(airplanes) - 1)
     # self.checkVal('XPLMAcquirePlanes didn\'t receive inAircraft correctly', self.getString(str0Dref), )
      self.checkVal('XPLMAcquirePlanes didn\'t call the callback', self.acquiredRefcon, ['Called!'])

      XPLMReleasePlanes()
      self.checkVal('Problem calling XPLMReleasePlanes', XPLMGetDatai(int0Dref), -1)

      active = 567
      XPLMSetActiveAircraftCount(active)
      self.checkVal('XPLMSetActiveAircraft didn\'t pass the inCount correctly', XPLMGetDatai(int0Dref), active)

      index = 678
      path = 'path/to/plane'
      XPLMSetAircraftModel(index, path)
      self.checkVal('XPLMSetAircraftModel didn\'t pass the inIndex correctly', XPLMGetDatai(int0Dref), index)
      self.checkVal('XPLMSetAircraftModel didn\'t pass the inAircraftPath correctly', self.getString(str0Dref), path)

      index = 789
      XPLMDisableAIForPlane(index)
      self.checkVal('XPLMDisableAIForPlan didn\'t pass the inPlaneIndex correctly', XPLMGetDatai(int0Dref), index)

      index = 890
      x = 1.234
      y = 2.345
      z = 3.456
      pitch = 4.567
      roll = 5.678
      yaw = 6.789
      fullDraw = 321
      dsSize = 44
      dsGearPosition = 9.87
      dsFlapRatio = 8.76
      dsSpoilerRatio = 7.65
      dsSpeedBrakeRatio = 6.54
      dsSlatRatio = 5.43
      dsWingSweep = 4.32
      dsThrust = 3.21
      dsYokePitch = 2.10
      dsYokeHeading = 1.09
      dsYokeRoll = 0.98
      XPLMDrawAircraft(index, x, y, z, pitch, roll, yaw, fullDraw, 
                       (dsSize, dsGearPosition, dsFlapRatio, dsSpoilerRatio, dsSpeedBrakeRatio, dsSlatRatio,
                        dsWingSweep, dsThrust, dsYokePitch, dsYokeHeading, dsYokeRoll))
      self.checkVal('XPLMDrawAircraft didn\'t pass the inPlaneIndex correctly', XPLMGetDatai(int0Dref), index)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inX correctly', XPLMGetDataf(inXValDref), x)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inY correctly', XPLMGetDataf(inYValDref), y)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inZ correctly', XPLMGetDataf(inZValDref), z)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inPitch correctly', XPLMGetDataf(inPitchValDref), pitch)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inRoll correctly', XPLMGetDataf(inRollValDref), roll)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inYaw correctly', XPLMGetDataf(inYawValDref), yaw)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inFullDraw correctly', XPLMGetDatai(int1Dref), fullDraw)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inDrawStateInfo.structSize correctly',
                    XPLMGetDatai(structSizeDref), dsSize)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inDrawStateInfo.gearPosition correctly',
                    XPLMGetDataf(gearPositionDref), dsGearPosition)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inDrawStateInfo.flapRatio correctly',
                    XPLMGetDataf(flapRatioDref), dsFlapRatio)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inDrawStateInfo.spoilerRatio correctly',
                    XPLMGetDataf(spoilerRatioDref), dsSpoilerRatio)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inDrawStateInfo.speedBrakeRatio correctly',
                    XPLMGetDataf(speedBrakeRatioDref), dsSpeedBrakeRatio)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inDrawStateInfo.slatRatio correctly',
                    XPLMGetDataf(slatRatioDref), dsSlatRatio)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inDrawStateInfo.wingSweep correctly',
                    XPLMGetDataf(wingSweepDref), dsWingSweep)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inDrawStateInfo.thrust correctly',
                    XPLMGetDataf(thrustDref), dsThrust)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inDrawStateInfo.yokePitch correctly',
                    XPLMGetDataf(yokePitchDref), dsYokePitch)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inDrawStateInfo.yokeHeading correctly',
                    XPLMGetDataf(yokeHeadingDref), dsYokeHeading)
      self.checkVal('XPLMDrawAircraft didn\'t pass the inDrawStateInfo.yokeRoll correctly',
                    XPLMGetDataf(yokeRollDref), dsYokeRoll)


      XPLMReinitUsersPlane()
      self.checkVal('Problem calling XPLMReinitUsersPlane', XPLMGetDatai(int0Dref), -2)

   def acquired(self, inRefcon):
      inRefcon.append('Called!')

