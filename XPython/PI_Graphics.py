#!/usr/bin/env python3
#

from XPLMCHKHelper import *

from XPLMDefs import *
from XPLMGraphics import *

class PythonInterface:
   def __init__(self):
      self.errors = 0
      return
   def check(self):
      if self.errors == 0:
         print('Graphics module check OK.')
      else:
         print('Graphics module check: {0} errors found.'.format(self.errors))

   def checkVal(self, prompt, got, expected):
      if got != None:
         if abs(got - expected) > 1e-6:
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
      self.Name = "XPLMGraphics module test"
      self.Sig = "Graph.Sig"
      self.Desc = "Module testing the XPLMGraphics interface."
      
      XPLMSetGraphicsState(1, 2, 3, 4, 5, 6, 7)
      self.checkVal('XPLMSetGraphicsState:inEnableFog', None, 1)
      self.checkVal('XPLMSetGraphicsState:inNumberTexUnits', None, 2)
      self.checkVal('XPLMSetGraphicsState:inEnableLighting', None, 3)
      self.checkVal('XPLMSetGraphicsState:inEnableAlphaTesting', None, 4)
      self.checkVal('XPLMSetGraphicsState:inEnableAlphaBlending', None, 5)
      self.checkVal('XPLMSetGraphicsState:inEnableDepthTesting', None, 6)
      self.checkVal('XPLMSetGraphicsState:inEnableDepthWriting', None, 7)

      XPLMBindTexture2d(8, 9)
      self.checkVal('XPLMBindTexture2d:inTextureNum', None, 8)
      self.checkVal('XPLMBindTexture2d:inTextureUnit', None, 9)

      self.textures = []
      XPLMGenerateTextureNumbers(self.textures, 10)
      for i, tex in enumerate(self.textures):
         #print('{0}, {1}'.format(i, tex))
         self.checkVal('XPLMGenerateTextureNumbers', tex, i + 42)
            

      newTex = XPLMGetTexture(11)
      self.checkVal('XPLMGetTexture: ', newTex, 53)

      x, y, z = XPLMWorldToLocal(123, 456, 789)
      self.checkVal('XPLMWorldToLocal.X: ', x, 124.23)
      self.checkVal('XPLMWorldToLocal.Y: ', y, 458.34)
      self.checkVal('XPLMWorldToLocal.Z: ', z, 792.45)
     
      lat, lon, alt = XPLMLocalToWorld(123, 456, 789)
      self.checkVal('XPLMLocalToWorld.lat: ', lat, 127.56)
      self.checkVal('XPLMLocalToWorld.lon: ', lon, 461.67)
      self.checkVal('XPLMLocalToWorld.alt: ', alt, 795.78)

      XPLMDrawTranslucentDarkBox(111, 222, 333, 444)
      self.checkVal('XPLMDrawTranslucentDarkBox:inLeft', None, 111)
      self.checkVal('XPLMDrawTranslucentDarkBox:inTop', None, 222)
      self.checkVal('XPLMDrawTranslucentDarkBox:inRight', None, 333)
      self.checkVal('XPLMDrawTranslucentDarkBox:inBottom', None, 444)


      color = 0.4, 0.5, 0.6
      XPLMDrawString(color, 555, 666, 'XPLMDrawString', 14, xplmFont_PanelEFIS)
      self.checkVal('XPLMDrawString:inColorRGB[0]', None, 0.4)
      self.checkVal('XPLMDrawString:inColorRGB[1]', None, 0.5)
      self.checkVal('XPLMDrawString:inColorRGB[2]', None, 0.6)
      self.checkVal('XPLMDrawString:inXOffset', None, 555)
      self.checkVal('XPLMDrawString:inYOffset', None, 666)
      self.checkVal('XPLMDrawString:inChar', None, 'XPLMDrawString')
      self.checkVal('XPLMDrawString:inWordWrapWidth', None, 14)
      self.checkVal('XPLMDrawString:inFontID', None, xplmFont_PanelEFIS)
      
      XPLMDrawNumber(color, 777, 888, 3.141592654, 10, 4, 5, xplmFont_PanelGPS)
      self.checkVal('XPLMDrawNumber:inColorRGB[0]', None, 0.4)
      self.checkVal('XPLMDrawNumber:inColorRGB[1]', None, 0.5)
      self.checkVal('XPLMDrawNumber:inColorRGB[2]', None, 0.6)
      self.checkVal('XPLMDrawNumber:inXOffset', None, 777)
      self.checkVal('XPLMDrawNumber:inYOffset', None, 888)
      self.checkVal('XPLMDrawNumber:inValue', None, 3.141592654)
      self.checkVal('XPLMDrawNumber:inDigits', None, 10)
      self.checkVal('XPLMDrawNumber:inDecimals', None, 4)
      self.checkVal('XPLMDrawNumber:inShowSign', None, 5)
      self.checkVal('XPLMDrawNumber:inFontID', None, xplmFont_PanelGPS)
 
      w = []; h = []; d = []
      XPLMGetFontDimensions(xplmFont_PanelEFIS, w, h, d)
      self.checkVal('XPLMGetFontDimensions.w: ',  w[0], 339)
      self.checkVal('XPLMGetFontDimensions.h: ',  h[0], 450)
      self.checkVal('XPLMGetFontDimensions.d: ',  d[0], 561)

      ms = XPLMMeasureString(xplmFont_PanelGPS, "abc", 16)
      self.checkVal('XPLMMeasureString: ', ms, 29.14)

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return
   
   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      self.checkVal('XPluginReceiveMessage: Unexpected inFromWho', inFromWho, 5)
      self.checkVal('XPluginReceiveMessage: Unexpected inMessage', inMessage, 103)
      self.checkVal('XPluginReceiveMessage: Unexpected inParam', inParam, 333)
   
#
