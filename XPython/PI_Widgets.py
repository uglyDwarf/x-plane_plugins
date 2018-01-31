#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPWidgets import *

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'Widgets');
   
   def XPluginStart(self):
      self.Name = "Widgets regression test"
      self.Sig = "WidgetsRT"
      self.Desc = "Regression test for the XPWidgets module"

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      self.int0Dref = XPLMFindDataRef('widgets/int0')
      self.int1Dref = XPLMFindDataRef('widgets/int1')
      self.int2Dref = XPLMFindDataRef('widgets/int2')
      self.int3Dref = XPLMFindDataRef('widgets/int3')
      
      self.widgetLeft = 105
      self.widgetTop = 126
      self.widgetRight = 125
      self.widgetBottom = 106
      self.widgetVisible = 107
      self.widgetDesc = 'Awesome widget'
      self.widgetIsRoot = 108
      self.widgetContainer = 0
      self.widgetClass = 109
      self.widget = XPCreateWidget(self.widgetLeft, self.widgetTop, self.widgetRight, self.widgetBottom,
                                   self.widgetVisible, self.widgetDesc, self.widgetIsRoot,
                                   self.widgetContainer, self.widgetClass)
      left = []; top = []; right = []; bottom = []
      XPGetWidgetGeometry(self.widget, left, top, right, bottom)
      self.checkVal('widget left', left[0], self.widgetLeft)
      self.checkVal('widget top', top[0], self.widgetTop)
      self.checkVal('widget right', right[0], self.widgetRight)
      self.checkVal('widget bottom', bottom[0], self.widgetBottom)
      self.checkVal('widget visible', XPIsWidgetVisible(self.widget), self.widgetVisible)
      desc = []
      res = XPGetWidgetDescriptor(self.widget, desc, 1024)
      self.checkVal('widget descriptor length', res, len(self.widgetDesc))
      self.checkVal('widget descriptor', desc[0], self.widgetDesc)
      self.checkVal('widget isRoot', XPLMGetDatai(self.int0Dref), self.widgetIsRoot)
      self.checkVal('widget container', XPGetParentWidget(self.widget), self.widgetContainer)
      self.checkVal('widget class', XPLMGetDatai(self.int1Dref), self.widgetClass)

      self.c_widgetLeft = 1105
      self.c_widgetTop = 1126
      self.c_widgetRight = 1125
      self.c_widgetBottom = 1106
      self.c_widgetVisible = 1107
      self.c_widgetDesc = 'Awesome custom widget'
      self.c_widgetIsRoot = 1108
      self.c_widgetContainer = self.widget
      self.c_widgetCallback = self.customCallback;
      self.cust_widget = XPCreateCustomWidget(self, self.c_widgetLeft, self.c_widgetTop,
                                              self.c_widgetRight, self.c_widgetBottom,
                                              self.c_widgetVisible, self.c_widgetDesc,
                                              self.c_widgetIsRoot, self.c_widgetContainer,
                                              self.c_widgetCallback)
      
     
      left = []; top = []; right = []; bottom = []
      XPGetWidgetGeometry(self.cust_widget, left, top, right, bottom)
      self.checkVal('widget left', left[0], self.c_widgetLeft)
      self.checkVal('widget top', top[0], self.c_widgetTop)
      self.checkVal('widget right', right[0], self.c_widgetRight)
      self.checkVal('widget bottom', bottom[0], self.c_widgetBottom)
      self.checkVal('widget visible', XPIsWidgetVisible(self.cust_widget), self.c_widgetVisible)
      desc = []
      res = XPGetWidgetDescriptor(self.cust_widget, desc, 1024)
      self.checkVal('widget descriptor length', res, len(self.c_widgetDesc))
      self.checkVal('widget descriptor', desc[0], self.c_widgetDesc)
      self.checkVal('widget isRoot', XPLMGetDatai(self.int0Dref), self.c_widgetIsRoot)
      self.checkVal('widget container', XPGetParentWidget(self.cust_widget), self.c_widgetContainer)
      self.message = 3456
      self.mode = 4567
      self.params = (1234, 2345)
      self.returnRes = 5678
      self.customCallbackCalled = 0
      res = XPSendMessageToWidget(self.cust_widget, self.message, self.mode, self.params[0], self.params[1])
      self.checkVal('customWidget callback called', self.customCallbackCalled, 1)
      self.checkVal('customWidget callback retval', res, self.returnRes)
      self.checkVal('customWidget callback method', XPLMGetDatai(self.int0Dref), self.mode)

      self.cust_widget1 = XPCreateCustomWidget(self, self.c_widgetLeft, self.c_widgetTop,
                                               self.c_widgetRight, self.c_widgetBottom,
                                               self.c_widgetVisible, self.c_widgetDesc,
                                               self.c_widgetIsRoot, 0,
                                               self.c_widgetCallback)
      XPPlaceWidgetWithin(self.cust_widget1, self.widget)
      self.checkVal('XPCountChildWidgets', XPCountChildWidgets(self.widget), 2)
      
      self.checkVal('XPGetNthChildWidget', XPGetNthChildWidget(self.widget, 0), self.cust_widget)
      self.checkVal('XPGetNthChildWidget', XPGetNthChildWidget(self.widget, 1), self.cust_widget1)

      self.checkVal('XPGetParentWidget', XPGetParentWidget(self.cust_widget), self.widget)
      
      self.checkVal('XPIsWidgetVisible', XPIsWidgetVisible(self.cust_widget), self.c_widgetVisible)
      XPHideWidget(self.cust_widget)
      self.checkVal('XPIsWidgetVisible', XPIsWidgetVisible(self.cust_widget), 0)
      XPShowWidget(self.cust_widget)
      self.checkVal('XPIsWidgetVisible', XPIsWidgetVisible(self.cust_widget), 1)
      
      self.checkVal('XPFindRootWidget', XPFindRootWidget(self.cust_widget), self.widget)


      self.destroyChildren = 6789
      XPDestroyWidget(self, self.cust_widget1, self.destroyChildren)
      self.checkVal('XPDestroyWidget destroyChildren', XPLMGetDatai(self.int0Dref), self.destroyChildren)
      self.checkVal('XPCountChildWidgets', XPCountChildWidgets(self.widget), 1)

      return 1.0

   def customCallback(self, inMessage, inWidget, inParam1, inParam2):
      self.checkVal('callback\'s message', inMessage, self.message)
      self.checkVal('callback\'s params', (inParam1, inParam2), self.params)
      self.checkVal('callback\'s widget class', inWidget, self.cust_widget)
      self.customCallbackCalled = True
      return self.returnRes;

   


