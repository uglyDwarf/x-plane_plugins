#!/usr/bin/env python3
#
from XPLMCHKHelper import *

from XPLMDefs import *
from XPLMDisplay import *

class PythonInterface:
   def __init__(self):
      self.drawCallbackCalled = 0
      self.keySnifferCallbackCalled = 0
      self.drawWindowCalled = 0
      self.handleMouseClickCalled = 0
      self.handleKeyCalled = 0
      self.handleCursorCalled = 0
      self.handleMouseWheelCalled = 0
      self.hotkeyCallbackCalled = 0
      self.xpluginStopCalled = 0
      self.xpluginEnableCalled = 0
      self.xpluginDisableCalled = 0
      self.xpluginMessageReceived = 0
      self.errors = 0
   
   def check(self):
      if not self.drawCallbackCalled:
         print('Error: DrawCallback was not called!')
         self.errors += 1
      if not self.keySnifferCallbackCalled:
         print('Error: KeySnifferCallback was not called!')
         self.errors += 1
      if not self.drawWindowCalled:
         print('Error: DrawWindow was not called!')
         self.errors += 1
      if not self.handleMouseClickCalled:
         print('Error: HandleMouseClick was not called!')
         self.errors += 1
      if not self.handleKeyCalled:
         print('Error: HandleKey was not called!')
         self.errors += 1
      if not self.handleCursorCalled:
         print('Error: HandleCursor was not called!')
         self.errors += 1
      if not self.handleMouseWheelCalled:
         print('Error: HandleMouseWheel was not called!')
         self.errors += 1
      if not self.hotkeyCallbackCalled:
         print('Error: HotkeyCallback was not called!')
         self.errors += 1
      if not self.xpluginEnableCalled:
         print('Error: pluginEnable was not called!')
         self.errors += 1
      if not self.xpluginDisableCalled:
         print('Error: pluginDisable was not called!')
         self.errors += 1
      if not self.xpluginMessageReceived:
         print('Error: xpluginReceiveMessage was not called!')
         self.errors += 1

      if self.errors == 0:
         print('Display module check OK.')
      else:
         print('Display module check: {0} errors found.'.format(self.errors))

   def checkVal(self, prompt, got, expected):
      if got != None:
         if isinstance(expected, float):
            if abs(got - expected) > 1e-6:
               print(' ** ERROR ** {0}: got {1}, expected {2}'.format(prompt, got, expected))
               self.errors += 1
         else:
            if got != expected:
               print(' ** ERROR ** {0}: got {1}, expected {2}'.format(prompt, got, expected))
               self.errors += 1
         return
      valID = prompt      
      if isinstance(expected, int):
         if not XPLMCHKHelperCheckInt(valID, expected):
            print(' ** ERROR ** {0} != {1}'.format(valID, expected))
            self.errors += 1
      elif isinstance(expected, float):
         if not XPLMCHKHelperCheckDouble(valID, expected):
            print(' ** ERROR ** {0} != {1}'.format(valID, expected))
            self.errors += 1
      elif isinstance(expected, str):
         if not XPLMCHKHelperCheckStr(valID, expected):
            print(' ** ERROR ** {0} != {1}'.format(valID, expected))
            self.errors += 1
      else:
         print(' ** ERROR ** Unsupported type passed to checkVal')
         self.errors += 1

   def XPluginStart(self):
      self.Name = "Display regression test"
      self.Sig = "DisplayRT"
      self.Desc = "Regression test of the XPLMDisplay module"

      #register drawing callback and store values to check
      self.drawCallbackFun = self.drawCallback
      self.drawPhase = xplm_Phase_Airplanes
      self.drawBefore = 1
      self.drawRefcon = 12345678
      i = XPLMRegisterDrawCallback(self, self.drawCallbackFun, self.drawPhase, self.drawBefore, self.drawRefcon)
      #register key sniffer
      self.keySnifferFun = self.keySniffer
      self.keySnifferRefcon = 321
      self.keySnifferBeforeWin = 1
      i = XPLMRegisterKeySniffer(self, self.keySnifferFun, self.keySnifferBeforeWin, self.keySnifferRefcon)
      #create windows (one extended, one normal)
      self.drawWindowFunc = self.drawWindow
      self.handleMouseClickFunc = self.handleMouseClick
      self.handleKeyFunc = self.handleKey
      self.handleCursorFunc = self.handleCursor
      self.handleMouseWheelFunc = self.handleMouseWheel
      self.winExRefcon = 8080
      (self.winExL, self.winExT, self.winExR, self.winExB, self.winExVis) = (110, 221, 332, 443, 1)
      pok = [self.winExL, self.winExT, self.winExR, self.winExB, self.winExVis, 
             self.drawWindowFunc, self.handleKeyFunc, self.handleMouseClickFunc, 
             self.handleCursorFunc, self.handleMouseWheelFunc, self.winExRefcon]
      self.winIDEx = XPLMCreateWindowEx(self, pok)
      self.winRefcon = 8086
      (self.winL, self.winT, self.winR, self.winB) = (111, 222, 333, 444)
      self.winVis = 1
      self.winID = XPLMCreateWindow(self, self.winL, self.winT, self.winR, self.winB, self.winVis, self.drawWindowFunc, 
                                    self.handleKeyFunc, self.handleMouseClickFunc, self.winRefcon)
      #check screen size
      wl = []
      wh = []
      XPLMGetScreenSize(wl, wh)
      self.checkVal('XPLMGetScreenSize: ', (wl[0], wh[0]), (1920, 1080))
      #check mouse location
      x = []
      y = []
      XPLMGetMouseLocation(x, y)
      self.checkVal('XPLMGetMouseLocation', (x[0], y[0]), (1024, 768))
      #check initial window geometry
      l = []; t = []; r = []; b = []
      XPLMGetWindowGeometry(self.winID, l, t, r, b)
      self.checkVal('XPLMGetWindowGeometry', (l[0], t[0], r[0], b[0]), (self.winL, self.winT, self.winR, self.winB))
      l = []; t = []; r = []; b = []
      XPLMGetWindowGeometry(self.winIDEx, l, t, r, b)
      self.checkVal('XPLMGetWindowGeometry', (l[0], t[0], r[0], b[0]), (self.winExL, self.winExT, self.winExR, self.winExB))
      #change window's geometry 
      (self.winL, self.winT, self.winR, self.winB) = (160, 240, 320, 120)
      XPLMSetWindowGeometry(self.winID, self.winL, self.winT, self.winR, self.winB)
      #check the change worked
      l = []; t = []; r = []; b = []
      XPLMGetWindowGeometry(self.winID, l, t, r, b)
      self.checkVal('XPLMGetWindowGeometry', (l[0], t[0], r[0], b[0]), (self.winL, self.winT, self.winR, self.winB))

      #window visibility
      self.checkVal('XPLMGetWindowIsVisible(winID)', XPLMGetWindowIsVisible(self.winID), 1)
      self.checkVal('XPLMGetWindowIsVisible(winIDEx)', XPLMGetWindowIsVisible(self.winIDEx), 1)
      XPLMSetWindowIsVisible(self.winID, 0)
      self.checkVal('XPLMGetWindowIsVisible(winID)', XPLMGetWindowIsVisible(self.winID), 0)

      #window refcon
      self.checkVal('XPLMGetWindowRefcon(winID)', XPLMGetWindowRefCon(self.winID), self.winRefcon)
      newRefcon = 31415926
      XPLMSetWindowRefCon(self.winID, newRefcon)
      self.winRefcon = XPLMGetWindowRefCon(self.winID)
      self.checkVal('XPLMGetWindowRefcon(winID)', XPLMGetWindowRefCon(self.winID), self.winRefcon)

      #Keyboard focus grab check
      XPLMTakeKeyboardFocus(self.winID)
      self.checkVal('XPLMTakeKeyboardFocus:inWindowID', None, self.winID)

      #is window in front
      self.checkVal('XPLMIsWindowInFront', XPLMIsWindowInFront(self.winIDEx), 0)
      XPLMBringWindowToFront(self.winIDEx)
      self.checkVal('XPLMIsWindowInFront', XPLMIsWindowInFront(self.winIDEx), 1)

      #check hotkey definitions
      self.hotkeyHandlerFunc = self.hotkeyHandler;
      hotkeyDefs = ((XPLM_VK_X, xplm_DownFlag, 'X hotkey', ord('x')),
                    (XPLM_VK_Y, xplm_DownFlag | xplm_ShiftFlag, 'Shift-Y hotkey', ord('y'))
                   )

      hotkeyIDDict = {}
      #self.hotkeyDict = {}
      self.hotkeys = []
      for hk in hotkeyDefs:
        id = XPLMRegisterHotKey(self, hk[0], hk[1], hk[2], self.hotkeyHandlerFunc, hk[3])
        self.hotkeys.append(id)
        hotkeyIDDict[id] = hk;
        #self.hotkeyDict[] = 

      for hk in range(XPLMCountHotKeys()):
         id = XPLMGetNthHotKey(hk)
         vkey = []; flags = []; description = []; pluginID = []
         XPLMGetHotKeyInfo(id, vkey, flags, description, pluginID)
         if id in hotkeyIDDict:
            desc = hotkeyIDDict[id]
            self.checkVal('HotKey vkey', vkey[0], desc[0])
            self.checkVal('HotKey flags', flags[0], desc[1])
            self.checkVal('HotKey desc', description[0], desc[2])
            self.checkVal('Hotkey pluginID', pluginID[0], 42)
         else:
            self.errors += 1
      
      XPLMSetHotKeyCombination(self.hotkeys[0], XPLM_VK_Z, xplm_UpFlag)
      desc = hotkeyIDDict[self.hotkeys[0]]
      vkey = []; flags = []; description = []; pluginID = []
      XPLMGetHotKeyInfo(self.hotkeys[0], vkey, flags, description, pluginID)
      self.checkVal('HotKey vkey', vkey[0], XPLM_VK_Z)
      self.checkVal('HotKey flags', flags[0], xplm_UpFlag)
      self.checkVal('HotKey desc', description[0], desc[2])
      self.checkVal('Hotkey pluginID', pluginID[0], 42)
 
      return self.Name, self.Sig, self.Desc
   
   def hotkeyHandler(self, inRefcon):
      self.hotkeyCallbackCalled = 1
   
   def XPluginStop(self):
      XPLMUnregisterDrawCallback(self, self.drawCallbackFun, xplm_Phase_Airplanes, 3, 12345678)
      XPLMUnregisterKeySniffer(self, self.keySnifferFun, 1, 321)
      XPLMDestroyWindow(self, self.winID)
      XPLMDestroyWindow(self, self.winIDEx)
      for hk in self.hotkeys:
        XPLMUnregisterHotKey(self, hk)

      self.check()
   
   def XPluginEnable(self):
      self.xpluginEnableCalled = 1
      return 1
   
   def XPluginDisable(self):
      self.xpluginDisableCalled = 1
   
   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      self.checkVal('XPluginReceiveMessage: Unexpected inFromWho', inFromWho, 5)
      self.checkVal('XPluginReceiveMessage: Unexpected inMessage', inMessage, 103)
      self.checkVal('XPluginReceiveMessage: Unexpected inParam', inParam[0], 42)
      self.xpluginMessageReceived = 1
   
   def drawCallback(self, inPhase, inIsBefore, inRefcon):
      self.checkVal('drawCallback: Unexpected inPhase', inPhase, self.drawPhase)
      self.checkVal('drawCallback: Unexpected inIsBefore', inIsBefore, self.drawBefore)
      self.checkVal('drawCallback: Unexpected inRefcon', inRefcon, self.drawRefcon)
      self.drawCallbackCalled = 1
      return 1

   def keySniffer(self, inChar, inFlags, inVirtualKey, inRefcon):
      self.checkVal('keySniffer: Unexpected key', chr(inChar), 'Q')
      self.checkVal('keySniffer: Unexpected flags', inFlags, xplm_ShiftFlag | xplm_DownFlag)
      self.checkVal('keySniffer: Unexpected virtual key', inVirtualKey, XPLM_VK_Q)
      self.checkVal('keySniffer: Unexpected refcon', inRefcon, self.keySnifferRefcon)
      self.keySnifferCallbackCalled = 1
      return 1

   def checkRightWindow(self, prompt, inWindowID, inRefcon):
      if (((inWindowID == self.winID) and (inRefcon == self.winRefcon)) or 
          ((inWindowID == self.winIDEx) and (inRefcon == self.winExRefcon))):
         return True
      else:
         print(' ** Error ** {0} unexpected windowID and refcon combination: ' + 
               'got ({0}, {1}), expected ({2}, {3}) or ({4}, {5})'.format(
                prompt, inWindowID, inRefcon, self.winID, self.winRefcon, self.winIDEx, self.winExRefcon))
         self.errors += 1
         return False

   def drawWindow(self, inWindowID, inRefcon):
      self.checkRightWindow('drawWindow', inWindowID, inRefcon)
      self.drawWindowCalled = 1
   
   def handleKey(self, inWindowID, inKey, inFlags, inVirtualKey, inRefcon, losingFocus):
      self.checkRightWindow('handleKey', inWindowID, inRefcon)
      self.checkVal('handleKey: Unexpected key', chr(inKey), 'P')
      self.checkVal('handleKey: Unexpected flags', inFlags, xplm_ControlFlag | xplm_UpFlag)
      self.checkVal('handleKey: Unexpected virtual key', inVirtualKey, XPLM_VK_P)
      self.checkVal('handleKey: Unexpected losing focus', losingFocus, 1)
      self.handleKeyCalled = 1
   
   def handleMouseClick(self, inWindowID, x, y, inMouse, inRefcon):
      self.checkRightWindow('handleMouseClick', inWindowID, inRefcon)
      self.checkVal('handleMouseClick: Unexpected coordinates', (x, y), (222, 333))
      self.handleMouseClickCalled = 1
      return 1

   def handleCursor(self, inWindowID, x, y, inRefcon):
      self.checkRightWindow('handleCursor', inWindowID, inRefcon)
      self.checkVal('handleCursor:Unexpected coordinates', (x, y), (444, 555))
      self.handleCursorCalled = 1
      return 0

   def handleMouseWheel(self, inWindowID, x, y, wheel, clicks, inRefcon):
      self.checkRightWindow('handleMouseWheel', inWindowID, inRefcon)
      self.checkVal('handleMouseWheel: Unexpected coordinates', (x, y), (666, 777))
      self.checkVal('handleMouseWheel: Unexpected wheel', wheel, 8)
      self.checkVal('handleMouseWheel: Unexpected clicks', clicks, 42)
      self.handleMouseWheelCalled = 1
      return 1
#
