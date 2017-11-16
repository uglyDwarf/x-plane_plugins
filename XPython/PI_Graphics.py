#!/usr/bin/env python3
#
#from XPLMCHKHelper import *
from check_helper import *

from XPLMDefs import *
from XPLMGraphics import *

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'Graphics');
      return

   def XPluginStart(self):
      self.Name = "XPLMGraphics module test"
      self.Sig = "Graph.Sig"
      self.Desc = "Module testing the XPLMGraphics interface."
      
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
      self.checkVal('XPluginReceiveMessage: Unexpected inParam', inParam[0], 42)
   
      enableFogDref = XPLMFindDataRef("enableFog")
      numTexUnitsDref = XPLMFindDataRef("numTexUnits")
      enableLightingDref = XPLMFindDataRef("enableLighting")
      enableAlphaTestDref = XPLMFindDataRef("enableAlphaTest")
      enableAlphaBlendDref = XPLMFindDataRef("enableAlphaBlend")
      enableDepthTestDref = XPLMFindDataRef("enableDepthTest")
      enableDepthWriteDref = XPLMFindDataRef("enableDepthWrite")
      texNumDref = XPLMFindDataRef("texNum")
      texUnitNumDref = XPLMFindDataRef("texUnitNum")
      int0Dref = XPLMFindDataRef("int0")
      int1Dref = XPLMFindDataRef("int1")
      int2Dref = XPLMFindDataRef("int2")
      int3Dref = XPLMFindDataRef("int3")
      int4Dref = XPLMFindDataRef("int4")
      int5Dref = XPLMFindDataRef("int5")
      float0Dref = XPLMFindDataRef("float0")
      float1Dref = XPLMFindDataRef("float1")
      float2Dref = XPLMFindDataRef("float2")
      str0Dref = XPLMFindDataRef("str0")
      double0Dref = XPLMFindDataRef("double0")

      fog = 1
      texUnits = 2
      enaLight = 3
      enaAlphaTest = 4
      enaAlphaBlend = 5
      enaDepthTest = 6
      enaDepthWrite = 7
      XPLMSetGraphicsState(fog, texUnits, enaLight, enaAlphaTest, enaAlphaBlend, enaDepthTest, enaDepthWrite)
      self.checkVal('XPLMSetGraphicsState:inEnableFog', XPLMGetDatai(enableFogDref), fog)
      self.checkVal('XPLMSetGraphicsState:inNumberTexUnits', XPLMGetDatai(numTexUnitsDref), texUnits)
      self.checkVal('XPLMSetGraphicsState:inEnableLighting', XPLMGetDatai(enableLightingDref), enaLight)
      self.checkVal('XPLMSetGraphicsState:inEnableAlphaTesting', XPLMGetDatai(enableAlphaTestDref), enaAlphaTest)
      self.checkVal('XPLMSetGraphicsState:inEnableAlphaBlending', XPLMGetDatai(enableAlphaBlendDref), enaAlphaBlend)
      self.checkVal('XPLMSetGraphicsState:inEnableDepthTesting', XPLMGetDatai(enableDepthTestDref), enaDepthTest)
      self.checkVal('XPLMSetGraphicsState:inEnableDepthWriting', XPLMGetDatai(enableDepthWriteDref), enaDepthWrite)

      texNum = 8
      texUnit = 9
      XPLMBindTexture2d(texNum, texUnit)
      self.checkVal('XPLMBindTexture2d:inTextureNum', XPLMGetDatai(texNumDref), texNum)
      self.checkVal('XPLMBindTexture2d:inTextureUnit', XPLMGetDatai(texUnitNumDref), texUnit)

      self.textures = []
      XPLMGenerateTextureNumbers(self.textures, 10)
      for i, tex in enumerate(self.textures):
         self.checkVal('XPLMGenerateTextureNumbers', tex, i + 42)
            
      texNum = 11
      newTex = XPLMGetTexture(texNum)
      self.checkVal('XPLMGetTexture: ', newTex, texNum + 43)

      lat, lon, alt = 123, 456, 789
      x, y, z = XPLMWorldToLocal(lat, lon, alt)
      self.checkVal('XPLMWorldToLocal.X: ', x, lat + 1.23)
      self.checkVal('XPLMWorldToLocal.Y: ', y, lon + 2.34)
      self.checkVal('XPLMWorldToLocal.Z: ', z, alt + 3.45)
      x, y, z = 123, 456, 789
      lat, lon, alt = XPLMLocalToWorld(x, y, z)
      self.checkVal('XPLMLocalToWorld.lat: ', lat, x + 4.56)
      self.checkVal('XPLMLocalToWorld.lon: ', lon, y + 5.67)
      self.checkVal('XPLMLocalToWorld.alt: ', alt, z + 6.78)

      left, top, right, bottom =  12, 13, 14, 15
      XPLMDrawTranslucentDarkBox(left, top, right, bottom)
      self.checkVal('XPLMDrawTranslucentDarkBox:inLeft', XPLMGetDatai(int0Dref), left)
      self.checkVal('XPLMDrawTranslucentDarkBox:inTop', XPLMGetDatai(int1Dref), top)
      self.checkVal('XPLMDrawTranslucentDarkBox:inRight', XPLMGetDatai(int2Dref), right)
      self.checkVal('XPLMDrawTranslucentDarkBox:inBottom', XPLMGetDatai(int3Dref), bottom)


      color = 0.4, 0.5, 0.6
      xOffset = 555
      yOffset = 666
      string = 'XPLMDrawString'
      wrap = 14
      font = xplmFont_PanelEFIS
      XPLMDrawString(color, xOffset, yOffset, string, wrap, font)
      self.checkVal('XPLMDrawString:inColorRGB[0]', XPLMGetDataf(float0Dref), color[0])
      self.checkVal('XPLMDrawString:inColorRGB[1]', XPLMGetDataf(float1Dref), color[1])
      self.checkVal('XPLMDrawString:inColorRGB[2]', XPLMGetDataf(float2Dref), color[2])
      self.checkVal('XPLMDrawString:inXOffset', XPLMGetDatai(int0Dref), xOffset)
      self.checkVal('XPLMDrawString:inYOffset', XPLMGetDatai(int1Dref), yOffset)
      self.checkVal('XPLMDrawString:inChar', self.getString(str0Dref), string)
      self.checkVal('XPLMDrawString:inWordWrapWidth', XPLMGetDatai(int2Dref), wrap)
      self.checkVal('XPLMDrawString:inFontID', XPLMGetDatai(int3Dref), font)
      
      color = 0.4, 0.5, 0.6
      xOffset = 555
      yOffset = 666
      val = 3.1415926
      digits = 15
      decimals = 16
      sign = 18
      font = xplmFont_PanelGPS
      XPLMDrawNumber(color, xOffset, yOffset, val, digits, decimals, sign, font)
      self.checkVal('XPLMDrawNumber:inColorRGB[0]', XPLMGetDataf(float0Dref), color[0])
      self.checkVal('XPLMDrawNumber:inColorRGB[1]', XPLMGetDataf(float1Dref), color[1])
      self.checkVal('XPLMDrawNumber:inColorRGB[2]', XPLMGetDataf(float2Dref), color[2])
      self.checkVal('XPLMDrawNumber:inXOffset', XPLMGetDatai(int0Dref), xOffset)
      self.checkVal('XPLMDrawNumber:inYOffset', XPLMGetDatai(int1Dref), yOffset)
      self.checkVal('XPLMDrawNumber:inValue', XPLMGetDatad(double0Dref), val)
      self.checkVal('XPLMDrawNumber:inDigits', XPLMGetDatai(int2Dref), digits)
      self.checkVal('XPLMDrawNumber:inDecimals', XPLMGetDatai(int3Dref), decimals)
      self.checkVal('XPLMDrawNumber:inShowSign', XPLMGetDatai(int4Dref), sign)
      self.checkVal('XPLMDrawNumber:inFontID', XPLMGetDatai(int5Dref), font)
 
      w = []; h = []; d = []
      font = xplmFont_PanelEFIS
      XPLMGetFontDimensions(font, w, h, d)
      self.checkVal('XPLMGetFontDimensions.w: ',  w[0], font + 333)
      self.checkVal('XPLMGetFontDimensions.h: ',  h[0], font + 444)
      self.checkVal('XPLMGetFontDimensions.d: ',  d[0], font + 555)

      font = xplmFont_PanelGPS
      string = "supercalifragilistic"
      chars = len(string)
      ms = XPLMMeasureString(font, string, chars)
      self.checkVal('XPLMMeasureString:retval/font', ms, font + 3.14)
      self.checkVal('XPLMMeasureString:inChar', self.getString(str0Dref), string)
      self.checkVal('XPLMMeasureString:retval/font', XPLMGetDatai(int0Dref), chars)

#
