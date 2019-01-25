#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPLMUtilities import *

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'Utilities');
      checkBase.addRef()
   
   def XPluginStart(self):
      self.Name = "Utilities regression test"
      self.Sig = "UtilitiesRT"
      self.Desc = "Regression test for XPLMUtilities module"

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
      checkBase.remRef()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      self.checkVal('XPluginReceiveMessage: Unexpected inFromWho', inFromWho, 5)
      self.checkVal('XPluginReceiveMessage: Unexpected inMessage', inMessage, 103)
      self.checkVal('XPluginReceiveMessage: Unexpected inParam', inParam[0], 42)

      keyType = XPLMFindDataRef("keyType")
      key = XPLMFindDataRef("key")
      string = XPLMFindDataRef("string")
      button = XPLMFindDataRef("button")
      state = XPLMFindDataRef("state")

      XPLMSimulateKeyPress(335, 446)
      self.checkVal('XPLMSimulateKeyPress keyType doesn\'t seem be passed correctly.', XPLMGetDatai(keyType), 335)
      self.checkVal('XPLMSimulateKeyPress key doesn\'t seem be passed correctly.', XPLMGetDatai(key), 446)

      tmp = "Very elaborate string."
      XPLMSpeakString(tmp)
      self.checkVal('XPLMSpeakString doesn\'t pass the string correctly.', self.getString(string), tmp)

      XPLMCommandKeyStroke(xplm_key_jettison)
      self.checkVal('XPLMCommandKeyStroke key doesn\'t seem be passed correctly.', XPLMGetDatai(key), xplm_key_jettison)

      XPLMCommandButtonPress(xplm_joy_start_7)
      self.checkVal('XPLMCommandButtonPress button doesn\'t seem be passed correctly.', XPLMGetDatai(button), xplm_joy_start_7)
      self.checkVal('XPLMCommandButtonPress state doesn\'t seem be passed correctly.', XPLMGetDatai(state), 1)
      XPLMCommandButtonRelease(xplm_joy_brakesMAX)
      self.checkVal('XPLMCommandButtonRelease button doesn\'t seem be passed correctly.', XPLMGetDatai(button), xplm_joy_brakesMAX)
      self.checkVal('XPLMCommandButtonRelease state doesn\'t seem be passed correctly.', XPLMGetDatai(state), 0)

      tmp = "C"
      res = XPLMGetVirtualKeyDescription(ord(tmp))
      self.checkVal('XPLMGetVirtualKeyDescription doesn\'t pass the string correctly.',
                    self.getString(string), "VK{0}".format(ord(tmp[0])))

      XPLMReloadScenery();
      # 0xDEADBEEF is negative on int resolution, so we need to convert it to long...
      self.checkVal('XPLMReloadScenery doesn\'t seem be work.', XPLMGetDatai(key), -((0xDEADBEEF ^ 0xFFFFFFFF) + 1))

      tmp = XPLMGetSystemPath()
      self.checkVal('XPLMGetSystemPath doesn\'t seem be return the right thing.', tmp, "/X-System/folder/")

      tmp = XPLMGetPrefsPath()
      self.checkVal('XPLMGetPrefsPath doesn\'t seem be return the right thing.', tmp, "/X-System/prefs/")

      tmp = XPLMGetDirectorySeparator()
      self.checkVal('XPLMGetDirectorySeparator doesn\'t seem be return the right thing.', tmp, "/")

      tmp = XPLMExtractFileAndPath("/home/user/X-Plane/Resources/plugins/lin.xpl")
      self.checkVal('XPLMExtract doesn\'t seem be return the right thing.', tmp[0], 
      "lin.xpl")
      self.checkVal('XPLMExtract doesn\'t seem be return the right thing.', tmp[1], 
      "/home/user/X-Plane/Resources/plugins")

      base = 5
      indices = 3
      path = "/test/path"
      (tmp, files, total) = XPLMGetDirectoryContents(path, base, 1024, indices)
      #check results
      self.checkVal('XPLMGetDirectoryContents returned wrong result', tmp, 0)
      #check the path made it through
      self.checkVal('XPLMGetDirectoryContents didn\'t pass the path correctly.', self.getString(string), path)

      #check the returned filenames
      outStr = ""
      tmp = ""
      for i,x in enumerate(files):
         outStr += "{0},".format(x)
         tmp += "File{0},".format(i+base)
      self.checkVal('XPLMGetDirectoryContents didn\'t return expected filenames.', outStr, tmp)
      self.checkVal('XPLMGetDirectoryContents returned wrong total file count', total, base + indices)

      
      self.checkVal('XPLMInitialized returned unexpected value.', XPLMInitialized(), 1)


      XPlaneVersion, XPLMVersion, HostID = XPLMGetVersions()
      if XPlaneVersion == 1120:
         self.checkVal('XPLMGetVersions returned unexpected XPLM version.', XPLMVersion, 301)
      elif XPlaneVersion == 1110:
         self.checkVal('XPLMGetVersions returned unexpected XPLM version.', XPLMVersion, 300)
      elif XPlaneVersion == 1060:
         self.checkVal('XPLMGetVersions returned unexpected XPLM version.', XPLMVersion, 210)
      else:
         self.checkVal('XPLMGetVersions returned unexpected XPLM version.', XPLMVersion, 200)
      self.checkVal('XPLMGetVersions returned unexpected host ID.', HostID, xplm_Host_YoungsMod)


      self.checkVal('XPLMGetLanguage returned unexpected language ID.', XPLMGetLanguage(), xplm_Language_Greek)


      tmp = 'Highly complicated debug string cones here'
      XPLMDebugString(tmp)
      self.checkVal('XPLMDebugString didn\'t pass the string correctly.', self.getString(string), tmp)

      self.dbgString = ''
      self.errCbk = self.errorCallback
      XPLMSetErrorCallback(self, self.errCbk)
      self.checkVal('Error callback called unexpectedly.', self.dbgString, '')
      tmp = XPLMFindSymbol('Abraca Dabra')
      self.checkVal('Error callback should have triggered by now.', self.dbgString,
                    "Roses are red, violets are blue and this code desperately needs some love...")


      dftype = xplm_DataFile_Situation
      path = '/tmp/sit.txt'
      tmp = XPLMLoadDataFile(dftype, path)
      self.checkVal('XPLMLoadDataFile returned wrong value.', tmp, 1)
      self.checkVal('XPLMLoadDataFile file type doesn\'t match.', XPLMGetDatai(key), dftype)
      self.checkVal('XPLMLoadDataFile returned wrong value.', self.getString(string), path)

      dftype = xplm_DataFile_ReplayMovie
      path = '/tmp/rep.txt'
      tmp = XPLMSaveDataFile(dftype, path)
      self.checkVal('XPLMSaveDataFile returned wrong value.', tmp, 1)
      self.checkVal('XPLMSaveDataFile file type doesn\'t match.', XPLMGetDatai(key), dftype)
      self.checkVal('XPLMSaveDataFile returned wrong value.', self.getString(string), path)
      
      path = 'test_command'
      self.cmdRef = XPLMCreateCommand(path, 'test command')
      self.checkVal('XPLMFindCommand didn\'t find my new command.', XPLMFindCommand(path), self.cmdRef)
      
      self.cmdHandler = self.commandCallback
      self.cmdStatus = [0, 0]
      XPLMRegisterCommandHandler(self, self.cmdRef, self.cmdHandler, 1, self.cmdStatus)
      XPLMCommandOnce(self.cmdRef)
      self.checkVal('XPLMCommandOnce behaviour incorrect (count).', self.cmdStatus[0], 2)
      self.checkVal('XPLMCommandOnce behaviour incorrect (stages).', self.cmdStatus[1], 5)
      self.cmdStatus[0] = 0; self.cmdStatus[1] = 0
      XPLMCommandBegin(self.cmdRef)
      self.checkVal('XPLMCommandBegin behaviour incorrect (count).', self.cmdStatus[0], 2)
      self.checkVal('XPLMCommandBegin behaviour incorrect (stages).', self.cmdStatus[1], 3)
      XPLMCommandEnd(self.cmdRef)
      self.checkVal('XPLMCommandEnd behaviour incorrect (count).', self.cmdStatus[0], 3)
      self.checkVal('XPLMCommandEnd behaviour incorrect (stages).', self.cmdStatus[1], 7)

      XPLMUnregisterCommandHandler(self, self.cmdRef, self.cmdHandler, 1, self.cmdStatus)
      self.cmdStatus[0] = 0; self.cmdStatus[1] = 0
      XPLMCommandOnce(self.cmdRef)
      XPLMCommandBegin(self.cmdRef)
      XPLMCommandEnd(self.cmdRef)
      self.checkVal('XPLMUnregisterCommandHandler behaviour incorrect (count).', self.cmdStatus[0], 0)
      self.checkVal('XPLMUnregisterCommandHandler behaviour incorrect (stages).', self.cmdStatus[1], 0)

   def errorCallback(self, inMessage):
      self.dbgString = inMessage

   def commandCallback(self, inCommand, inPhase, inRefcon):
      self.checkVal('CommandCallback given unknown command reference.', inCommand, self.cmdRef)
      inRefcon[0] += 1
      inRefcon[1] |= 1 << inPhase
      return 1

#
