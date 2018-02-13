#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPWidgetDefs import *
from XPWidgets import *
from XPStandardWidgets import *
from XPWidgetUtils import *

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'WidgetUtils');
   
   def XPluginStart(self):
      self.Name = "WidgetUtils regression test"
      self.Sig = "WidgetUtilsRT"
      self.Desc = "Regression test for the Widget Utils module"

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      self.int0Dref = XPLMFindDataRef('widgetUtils/int0')
      self.int1Dref = XPLMFindDataRef('widgetUtils/int1')
      self.int2Dref = XPLMFindDataRef('widgetUtils/int2')
      self.int3Dref = XPLMFindDataRef('widgetUtils/int3')
      self.int4Dref = XPLMFindDataRef('widgetUtils/int4')
      self.int5Dref = XPLMFindDataRef('widgetUtils/int5')
      self.int6Dref = XPLMFindDataRef('widgetUtils/int6')
      
      self.parentWidget = XPCreateWidget(0, 480, 640, 0, 1, "Parent", 1, 0, xpWidgetClass_MainWindow)

      (self.child1L, self.child1T, self.child1R, self.child1B) = (10, 230, 310, 10)
      self.child1Visible = 750
      self.child1Descriptor = "Child1"
      self.child1IsRoot = 0
      self.child1Class = xpWidgetClass_SubWindow
      (self.child11L, self.child11T, self.child11R, self.child11B) = (20, 220, 25, 215)
      self.child11Visible = 751
      self.child11Descriptor = "Child11"
      self.child11IsRoot = 0
      self.child11Class = xpWidgetClass_SubWindow
      (self.staL, self.staT, self.staR, self.staB) = (650, 490, 1280, 50)
      self.staVisible = 752
      self.staDescriptor = "Standalone"
      self.staIsRoot = 1
      self.staClass = xpWidgetClass_MainWindow

      template = [
        [self.child1L, self.child1T, self.child1R, self.child1B, self.child1Visible, 
         self.child1Descriptor, self.child1IsRoot, PARAM_PARENT, self.child1Class],
        [self.child11L, self.child11T, self.child11R, self.child11B, self.child11Visible,
         self.child11Descriptor, self.child11IsRoot, 0, self.child11Class],
        [self.staL, self.staT, self.staR, self.staB, self.staVisible, 
         self.staDescriptor, self.staIsRoot, NO_PARENT, self.staClass]
      ];

      widgets = []
      XPUCreateWidgets(template, len(template), self.parentWidget, widgets)
      (self.child1, self.child11, self.standalone) = widgets

      
      descriptor = [];
      coords = XPGetWidgetGeometry(self.child1)
      XPGetWidgetDescriptor(self.child1, descriptor, 1024)
      visible = XPIsWidgetVisible(self.child1)
      parent = XPGetParentWidget(self.child1)
      self.checkVal('Child1 coordinates', coords, [self.child1L, self.child1T, self.child1R, self.child1B])
      self.checkVal('Child1 descriptor', descriptor[0], self.child1Descriptor)
      self.checkVal('Child1 parent', parent, self.parentWidget)
      self.checkVal('Child1 isRoot', XPGetWidgetProperty(self.child1, xpProperty_UserStart, None), self.child1IsRoot)
      self.checkVal('Child1 class', XPGetWidgetProperty(self.child1, xpProperty_UserStart + 1, None), self.child1Class)

      descriptor = [];
      coords = XPGetWidgetGeometry(self.child11)
      XPGetWidgetDescriptor(self.child11, descriptor, 1024)
      visible = XPIsWidgetVisible(self.child11)
      parent = XPGetParentWidget(self.child11)
      self.checkVal('Child11 coordinates', coords, [self.child11L, self.child11T, self.child11R, self.child11B])
      self.checkVal('Child11 descriptor', descriptor[0], self.child11Descriptor)
      self.checkVal('Child11 parent', parent, self.child1)
      self.checkVal('Child11 isRoot', XPGetWidgetProperty(self.child11, xpProperty_UserStart, None), self.child11IsRoot)
      self.checkVal('Child11 class', XPGetWidgetProperty(self.child11, xpProperty_UserStart + 1, None), self.child11Class)

      descriptor = [];
      coords = XPGetWidgetGeometry(self.standalone)
      XPGetWidgetDescriptor(self.standalone, descriptor, 1024)
      visible = XPIsWidgetVisible(self.standalone)
      parent = XPGetParentWidget(self.standalone)
      self.checkVal('Sta coordinates', coords, [self.staL, self.staT, self.staR, self.staB])
      self.checkVal('Sta descriptor', descriptor[0], self.staDescriptor)
      self.checkVal('Sta parent', parent, 0)
      self.checkVal('Sta isRoot', XPGetWidgetProperty(self.standalone, xpProperty_UserStart, None), self.staIsRoot)
      self.checkVal('Sta class', XPGetWidgetProperty(self.standalone, xpProperty_UserStart + 1, None), self.staClass)

      (self.dX, self.dY) = (1, 2)
      XPUMoveWidgetBy(self.standalone, self.dX, self.dY)
      self.checkVal('Move widget', XPGetWidgetGeometry(self.standalone), [self.staL + self.dX, self.staT + self.dY,
                                                                         self.staR + self.dX, self.staB + self.dY])

      (self.msg, self.inParam1, self.inParam2) = (4567, 4568, 4569)
      res = XPUFixedLayout(self.msg, self.child1, self.inParam1, self.inParam2)
      self.checkVal('XPUFixedLayout res, inWidget', res, self.child1Visible + self.msg)
      self.checkVal('XPUFixedLayout inMessage', XPLMGetDatai(self.int0Dref),  self.msg)
      self.checkVal('XPUFixedLayout inParam1', XPLMGetDatai(self.int1Dref),  self.inParam1)
      self.checkVal('XPUFixedLayout inParam2', XPLMGetDatai(self.int2Dref),  self.inParam2)

      (self.msg, self.inParam1, self.inParam2, self.eatClick) = (4570, 4571, 4572, 4573)
      res = XPUSelectIfNeeded(self.msg, self.child11, self.inParam1, self.inParam2, self.eatClick)
      self.checkVal('XPUSelectIfNeeded res, inWidget', res, self.child11Visible + self.inParam1)
      self.checkVal('XPUSelectIfNeeded inMessage', XPLMGetDatai(self.int1Dref),  self.msg)
      self.checkVal('XPUSelectIfNeeded inParam1', XPLMGetDatai(self.int2Dref),  self.inParam1)
      self.checkVal('XPUSelectIfNeeded inParam2', XPLMGetDatai(self.int3Dref),  self.inParam2)
      self.checkVal('XPUSelectIfNeeded inEatClick', XPLMGetDatai(self.int0Dref),  self.eatClick)

      (self.msg, self.inParam1, self.inParam2, self.eatClick) = (4574, 4575, 4576, 4577)
      res = XPUDefocusKeyboard(self.msg, self.standalone, self.inParam1, self.inParam2, self.eatClick)
      self.checkVal('XPUDefocusKeyboard res, inWidget', res, self.staVisible + self.inParam2)
      self.checkVal('XPUDefocusKeyboard inMessage', XPLMGetDatai(self.int0Dref),  self.msg)
      self.checkVal('XPUDefocusKeyboard inParam1', XPLMGetDatai(self.int2Dref),  self.inParam1)
      self.checkVal('XPUDefocusKeyboard', XPLMGetDatai(self.int3Dref),  self.inParam2)
      self.checkVal('XPUDefocusKeyboard inEatClick', XPLMGetDatai(self.int1Dref),  self.eatClick)

      (self.msg, self.inParam1, self.inParam2, self.left, self.top, self.right, self.bottom) = \
      (4578,     4579,          4580,          4581,      4582,     4583,       4584)
      res = XPUDragWidget(self.msg, self.standalone, self.inParam1, self.inParam2,
                          self.left, self.top, self.right, self.bottom)
      self.checkVal('XPUDragWidget res, inWidget', res, self.staVisible + self.left)
      self.checkVal('XPUDragWidget inMessage', XPLMGetDatai(self.int0Dref),  self.msg)
      self.checkVal('XPUDragWidget inParam1', XPLMGetDatai(self.int1Dref),  self.inParam1)
      self.checkVal('XPUDragWidget inParam2', XPLMGetDatai(self.int2Dref),  self.inParam2)
      self.checkVal('XPUDragWidget inLeft', XPLMGetDatai(self.int3Dref),  self.left)
      self.checkVal('XPUDragWidget inTop', XPLMGetDatai(self.int4Dref),  self.top)
      self.checkVal('XPUDragWidget inRight', XPLMGetDatai(self.int5Dref),  self.right)
      self.checkVal('XPUDragWidget inBottom', XPLMGetDatai(self.int6Dref),  self.bottom)

      XPDestroyWidget(self, self.standalone, 1)
      XPDestroyWidget(self, self.parentWidget, 1)

      return 1.0

