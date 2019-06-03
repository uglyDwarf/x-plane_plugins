#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPWidgetDefs import *
from XPUIGraphics import *

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'UIGraphics');
      checkBase.addRef()
   
   def XPluginStart(self):
      self.Name = "UI Graphics regression test"
      self.Sig = "UIGraphicsRT"
      self.Desc = "Regression test for the UIGraphics module"

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
      checkBase.remRef()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      self.int0Dref = XPLMFindDataRef('uiGraphics/int0')
      self.int1Dref = XPLMFindDataRef('uiGraphics/int1')
      self.int2Dref = XPLMFindDataRef('uiGraphics/int2')
      self.int3Dref = XPLMFindDataRef('uiGraphics/int3')
      self.int4Dref = XPLMFindDataRef('uiGraphics/int4')
      self.int5Dref = XPLMFindDataRef('uiGraphics/int5')
      self.int6Dref = XPLMFindDataRef('uiGraphics/int6')
      self.int7Dref = XPLMFindDataRef('uiGraphics/int7')
      self.int8Dref = XPLMFindDataRef('uiGraphics/int8')

      x1 = 589
      y1 = 590
      x2 = 591
      y2 = 593
      style = 592
      XPDrawWindow(x1, y1, x2, y2, style)
      self.checkVal('XPDrawWindow inX1', XPLMGetDatai(self.int0Dref), x1)
      self.checkVal('XPDrawWindow inY1', XPLMGetDatai(self.int1Dref), y1)
      self.checkVal('XPDrawWindow inX2', XPLMGetDatai(self.int2Dref), x2)
      self.checkVal('XPDrawWindow inY2', XPLMGetDatai(self.int3Dref), y2)
      self.checkVal('XPDrawWindow inStyle', XPLMGetDatai(self.int4Dref), style)

      style = 594
      width = []
      height = []
      XPGetWindowDefaultDimensions(style, width, height)
      self.checkVal('XPGetWindowDefaultDimensions style', XPLMGetDatai(self.int0Dref), style)
      self.checkVal('XPGetWindowDefaultDimensions outWidth', width[0], x2 - x1)
      self.checkVal('XPGetWindowDefaultDimensions outHeight', height[0], y2 - y1)

      x1 = 595
      y1 = 596
      x2 = 597
      y2 = 599
      style = 598
      lit = 600
      XPDrawElement(x1, y1, x2, y2, style, lit)
      self.checkVal('XPDrawElement inX1', XPLMGetDatai(self.int0Dref), x1)
      self.checkVal('XPDrawElement inY1', XPLMGetDatai(self.int1Dref), y1)
      self.checkVal('XPDrawElement inX2', XPLMGetDatai(self.int2Dref), x2)
      self.checkVal('XPDrawElement inY2', XPLMGetDatai(self.int3Dref), y2)
      self.checkVal('XPDrawElement inStyle', XPLMGetDatai(self.int4Dref), style)
      self.checkVal('XPDrawElement inLit', XPLMGetDatai(self.int5Dref), lit)

      style = 601
      width = []
      height = []
      canBeLit = []
      XPGetElementDefaultDimensions(style, width, height, canBeLit)
      self.checkVal('XPGetElementDefaultDimensions style', XPLMGetDatai(self.int0Dref), style)
      self.checkVal('XPGetElementDefaultDimensions outWidth', width[0], x2 - x1)
      self.checkVal('XPGetElementDefaultDimensions outHeight', height[0], y2 - y1)
      self.checkVal('XPGetElementDefaultDimensions outCanBeLit', canBeLit[0], lit)

      x1 = 602
      y1 = 603
      x2 = 605
      y2 = 604
      minVal = 606 
      maxVal = 607
      val = 608
      style = 609
      lit = 610
      XPDrawTrack(x1, y1, x2, y2, minVal, maxVal, val, style, lit)
      self.checkVal('XPDrawTrack inX1', XPLMGetDatai(self.int0Dref), x1)
      self.checkVal('XPDrawTrack inY1', XPLMGetDatai(self.int1Dref), y1)
      self.checkVal('XPDrawTrack inX2', XPLMGetDatai(self.int2Dref), x2)
      self.checkVal('XPDrawTrack inY2', XPLMGetDatai(self.int3Dref), y2)
      self.checkVal('XPDrawTrack inMin', XPLMGetDatai(self.int4Dref), minVal)
      self.checkVal('XPDrawTrack inMax', XPLMGetDatai(self.int5Dref), maxVal)
      self.checkVal('XPDrawTrack inValue', XPLMGetDatai(self.int6Dref), val)
      self.checkVal('XPDrawTrack inStyle', XPLMGetDatai(self.int7Dref), style)
      self.checkVal('XPDrawTrack inLit', XPLMGetDatai(self.int8Dref), lit)

      style = 611
      width = []
      canBeLit = []
      XPGetTrackDefaultDimensions(style, width, canBeLit)
      self.checkVal('XPGetTrackDefaultDimensions style', XPLMGetDatai(self.int0Dref), style)
      self.checkVal('XPGetTrackDefaultDimensions outWidth', width[0], x2 - x1)
      self.checkVal('XPGetTrackDefaultDimensions outCanBeLit', canBeLit[0], lit)

      x1 = 612
      y1 = 613
      x2 = 615
      y2 = 614
      minVal = 616 
      maxVal = 617
      val = 618
      style = 619
      isVertical = []
      downBtnSize = []
      downPageSize = []
      thumbSize = []
      upPageSize = []
      upBtnSize = []
      XPGetTrackMetrics(x1, y1, x2, y2, minVal, maxVal, val, style, 
                        isVertical, downBtnSize, downPageSize, thumbSize, upPageSize, upBtnSize)
      self.checkVal('XPDrawTrack inX1', isVertical[0], x1)
      self.checkVal('XPDrawTrack inY1', downBtnSize[0], y1)
      self.checkVal('XPDrawTrack inX2', downPageSize[0], x2)
      self.checkVal('XPDrawTrack inY2', thumbSize[0], y2)
      self.checkVal('XPDrawTrack inMin', upPageSize[0], minVal)
      self.checkVal('XPDrawTrack inMax', upBtnSize[0], maxVal)
      self.checkVal('XPDrawTrack inValue', XPLMGetDatai(self.int0Dref), val)
      self.checkVal('XPDrawTrack inStyle', XPLMGetDatai(self.int1Dref), style)

      return 1.0


