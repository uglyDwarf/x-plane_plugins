#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPLMNavigation import *

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'Navigation');

   def XPluginStart(self):
      self.Name = "Navigation regression test"
      self.Sig = "NavigationRT"
      self.Desc = "Regression test for XPLMNavigation module"

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      nr = 333
      self.checkVal('XPLMGetNextNavAid returned unexpected value', XPLMGetNextNavAid(nr), nr + 1)
      self.checkVal('XPLMGetFirstNavAid returned unexpected value', XPLMGetFirstNavAid(), nr)
      inType = 123
      self.checkVal('XPLMFindFirstNavAidOfType returned unexpected value', XPLMFindFirstNavAidOfType(inType), inType + 480)
      self.checkVal('XPLMFindLastNavAidOfType returned unexpected value', XPLMFindLastNavAidOfType(inType), inType + 490)
      
      name = "nameFrag"
      idStr = "idFrag"
      lat = 28.5
      lon = 44.8
      freq = 108500
      nr = 888

      self.checkVal('XPLMFindNavAid returned unexpected value', XPLMFindNavAid(name, idStr, lat, lon, freq, nr), nr + 500)
      nr = 999
      outType = []
      outLatitude = []
      outLongitude = []
      outHeight = []
      outFrequency = []
      outHeading = []
      outID = []
      outName = []
      outReg = []
      XPLMGetNavAidInfo(nr, outType, outLatitude, outLongitude, outHeight, outFrequency, outHeading, outID, outName, outReg)
      self.checkVal('XPLMGetNavAidInfo outType doesn\'t match', outType[0], inType)
      self.checkVal('XPLMGetNavAidInfo outLatitude doesn\'t match', outLatitude[0], lat)
      self.checkVal('XPLMGetNavAidInfo outLongitude doesn\'t match', outLongitude[0], lon)
      self.checkVal('XPLMGetNavAidInfo outHeight doesn\'t match', outHeight[0], nr)
      self.checkVal('XPLMGetNavAidInfo outFrequency doesn\'t match', outFrequency[0], freq)
      self.checkVal('XPLMGetNavAidInfo outHeading doesn\'t match', outHeading[0], lat * lon)
      self.checkVal('XPLMGetNavAidInfo outID doesn\'t match', outID[0], idStr)
      self.checkVal('XPLMGetNavAidInfo outName doesn\'t match', outName[0], name)
      self.checkVal('XPLMGetNavAidInfo outReg doesn\'t match', outReg[0], 1)

      fmsEntry = 4321
      destFmsEntry = 5432
      XPLMSetDisplayedFMSEntry(fmsEntry)
      XPLMSetDestinationFMSEntry(destFmsEntry)
      self.checkVal('XPLMCountFMSEntries returned unexpected value', XPLMCountFMSEntries(), fmsEntry + 2)
      self.checkVal('XPLMGetDisplayedFMSEntry returned unexpected value', XPLMGetDisplayedFMSEntry(), fmsEntry)
      self.checkVal('XPLMGetDestinationFMSEntry returned unexpected value', XPLMGetDestinationFMSEntry(), destFmsEntry)

      index = 777
      ref = 888
      altitude = 1200
      lat = 10.3
      lon = 15.5
      XPLMSetFMSEntryInfo(index, ref, altitude)
      outType = []
      outID = []
      outRef = []
      outAltitude = []
      outLatitude = []
      outLongitude = []
      XPLMGetFMSEntryInfo(index, outType, outID, outRef, outAltitude, outLatitude, outLongitude)
      self.checkVal('XPLMGetFMSEntryInfo outType doesn\'t match', outType[0], index)
      self.checkVal('XPLMGetFMSEntryInfo outID doesn\'t match', outID[0], idStr)
      self.checkVal('XPLMGetFMSEntryInfo outRef doesn\'t match', outRef[0], ref)
      self.checkVal('XPLMGetFMSEntryInfo outAltitude doesn\'t match', outAltitude[0], altitude)

      index += 1
      altitude += 1
      XPLMSetFMSEntryLatLon(index, lat, lon, altitude)
      outType = []
      outID = []
      outRef = []
      outAltitude = []
      outLatitude = []
      outLongitude = []
      XPLMGetFMSEntryInfo(index, outType, outID, outRef, outAltitude, outLatitude, outLongitude)
      self.checkVal('XPLMGetFMSEntryInfo outType doesn\'t match', outType[0], index)
      self.checkVal('XPLMGetFMSEntryInfo outID doesn\'t match', outID[0], idStr)
      self.checkVal('XPLMGetFMSEntryInfo outRef doesn\'t match', outRef[0], ref)
      self.checkVal('XPLMGetFMSEntryInfo outAltitude doesn\'t match', outAltitude[0], altitude)
      self.checkVal('XPLMGetFMSEntryInfo outLatitude doesn\'t match', outLatitude[0], lat)
      self.checkVal('XPLMGetFMSEntryInfo outLongitude doesn\'t match', outLongitude[0], lon)

      index += 1
      XPLMClearFMSEntry(index)
      self.checkVal('XPLMClearFMSEntry didn\'t passed the inIndex', XPLMGetDisplayedFMSEntry(), index)

      index += 1
      XPLMFindFirstNavAidOfType(index)
      self.checkVal('XPLMGetGPSDestinationType returned wrong value', XPLMGetGPSDestinationType(), index)

      ref += 1
      XPLMSetFMSEntryInfo(0, ref, 0)
      self.checkVal('XPLMGetGPSDestination returned wrong value', XPLMGetGPSDestination(), ref)

      return


