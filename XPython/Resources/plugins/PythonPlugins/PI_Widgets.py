#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPWidgetDefs import *
from XPWidgets import *
from XPLMUtilities import *

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'Widgets');
   
   def XPluginStart(self):
      self.Name = "Widgets regression test"
      self.Sig = "WidgetsRT"
      self.Desc = "Regression test for the XPWidgets module"
      self.versions = XPLMGetVersions()

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
      self.int4Dref = XPLMFindDataRef('widgets/int4')

      self.widgetLeft = 300
      self.widgetTop = 450
      self.widgetRight = 400
      self.widgetBottom = 350
      self.widgetVisible = 107
      self.widgetDesc = 'Awesome widget'
      self.widgetIsRoot = 108
      self.widgetContainer = None
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
      self.checkVal('widget visible', XPIsWidgetVisible(self.widget), self.widgetVisible * self.widgetClass)
      desc = []
      res = XPGetWidgetDescriptor(self.widget, desc, 1024)
      self.checkVal('widget descriptor length', res, len(self.widgetDesc))
      self.checkVal('widget descriptor', desc[0], self.widgetDesc)
      self.checkVal('widget isRoot', XPLMGetDatai(self.int4Dref), self.widgetIsRoot)
      self.checkVal('widget container', XPGetParentWidget(self.widget), self.widgetContainer)
      self.checkVal('widget class', XPLMGetDatai(self.int1Dref), self.widgetClass)

      self.c_widgetLeft = 285
      self.c_widgetTop = 465
      self.c_widgetRight = 349
      self.c_widgetBottom = 401
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
      self.checkVal('widget visible', XPIsWidgetVisible(self.cust_widget), self.c_widgetVisible * 11) # default widgetClass
      desc = []
      res = XPGetWidgetDescriptor(self.cust_widget, desc, 1024)
      self.checkVal('widget descriptor length', res, len(self.c_widgetDesc))
      self.checkVal('widget descriptor', desc[0], self.c_widgetDesc)
      self.checkVal('widget isRoot', XPLMGetDatai(self.int4Dref), self.c_widgetIsRoot)
      self.checkVal('widget container', XPGetParentWidget(self.cust_widget), self.c_widgetContainer)
      self.message = 3456
      self.mode = 4567
      self.params = (1234, 2345)
      self.returnRes = 5678
      self.customCallbackCalled = 0
      self.target_widget = self.cust_widget
      res = XPSendMessageToWidget(self.cust_widget, self.message, self.mode, self.params[0], self.params[1])
      self.checkVal('customWidget callback called', self.customCallbackCalled, 1)
      self.checkVal('customWidget callback retval', res, self.returnRes)
      self.checkVal('customWidget callback method', XPLMGetDatai(self.int0Dref), self.mode)

      self.c_widget1Left = 351
      self.c_widget1Top = 399
      self.c_widget1Right = 415
      self.c_widget1Bottom = 335
      self.cust_widget1 = XPCreateCustomWidget(self, self.c_widget1Left, self.c_widget1Top,
                                               self.c_widget1Right, self.c_widget1Bottom,
                                               self.c_widgetVisible, self.c_widgetDesc,
                                               self.c_widgetIsRoot, 0,
                                               self.c_widgetCallback)
      XPPlaceWidgetWithin(self.cust_widget1, self.widget)
      self.checkVal('XPCountChildWidgets', XPCountChildWidgets(self.widget), 2)
      
      self.checkVal('XPGetNthChildWidget', XPGetNthChildWidget(self.widget, 0), self.cust_widget)
      self.checkVal('XPGetNthChildWidget', XPGetNthChildWidget(self.widget, 1), self.cust_widget1)

      self.checkVal('XPGetParentWidget', XPGetParentWidget(self.cust_widget), self.widget)
      
      self.checkVal('XPIsWidgetVisible', XPIsWidgetVisible(self.cust_widget), self.c_widgetVisible * 11)
      XPHideWidget(self.cust_widget)
      self.checkVal('XPIsWidgetVisible', XPIsWidgetVisible(self.cust_widget), 0 * 11)
      XPShowWidget(self.cust_widget)
      self.checkVal('XPIsWidgetVisible', XPIsWidgetVisible(self.cust_widget), 1 * 11)
      
      self.checkVal('XPFindRootWidget', XPFindRootWidget(self.cust_widget), self.widget)

      left = []; top = []; right = []; bottom = []
      XPGetWidgetExposedGeometry(self.cust_widget, left, top, right, bottom)
      self.checkVal('XPGetWidgetExposedGeometry', (left[0], top[0], right[0], bottom[0]),
                    (self.widgetLeft, self.widgetTop, self.c_widgetRight, self.c_widgetBottom))


      self.xOffset = 320
      self.yOffset = 430
      self.recursive = 555
      self.visibleOnly = 666
      res = XPGetWidgetForLocation(self.widget, self.xOffset, self.yOffset, self.recursive, self.visibleOnly)
      self.checkVal('XPGetWidgetForLocation result', res, self.cust_widget)
      self.checkVal('XPGetWidgetForLocation X offset', XPLMGetDatai(self.int0Dref), self.xOffset)
      self.checkVal('XPGetWidgetForLocation Y offset', XPLMGetDatai(self.int1Dref), self.yOffset)
      self.checkVal('XPGetWidgetForLocation recursive', XPLMGetDatai(self.int2Dref), self.recursive)
      self.checkVal('XPGetWidgetForLocation visibleOnly', XPLMGetDatai(self.int3Dref), self.visibleOnly)




      self.c_widgetLeft = 1212
      self.c_widgetTop = 1213
      self.c_widgetRight = 1214
      self.c_widgetBottom = 1215
      XPSetWidgetGeometry(self.cust_widget, self.c_widgetLeft, self.c_widgetTop, self.c_widgetRight, self.c_widgetBottom)

      left = []; top = []; right = []; bottom = []
      XPGetWidgetGeometry(self.cust_widget, left, top, right, bottom)
      self.checkVal('widget left', left[0], self.c_widgetLeft)
      self.checkVal('widget top', top[0], self.c_widgetTop)
      self.checkVal('widget right', right[0], self.c_widgetRight)
      self.checkVal('widget bottom', bottom[0], self.c_widgetBottom)

      self.checkVal('XPIsWidgetInFront', XPIsWidgetInFront(self.widget), 0)
      XPBringRootWidgetToFront(self.cust_widget)
      self.checkVal('XPIsWidgetInFront', XPIsWidgetInFront(self.widget), 1)

      desc = []
      XPGetWidgetDescriptor(self.cust_widget, desc, 4096)
      self.checkVal('XPGetWidgetDescriptor', desc[0], self.c_widgetDesc)
      self.c_widgetDesc = 'Cool custom widget'
      XPSetWidgetDescriptor(self.cust_widget, self.c_widgetDesc)
      desc = []
      XPGetWidgetDescriptor(self.cust_widget, desc, 4096)
      self.checkVal('XPSetWidgetDescriptor', desc[0], self.c_widgetDesc)

      self.checkVal('XPSetKeyboardFocus', XPSetKeyboardFocus(self.cust_widget), self.cust_widget)
      self.checkVal('XPGetWidgetWithFocus', XPGetWidgetWithFocus(), self.cust_widget)
      XPLoseKeyboardFocus(self.cust_widget)
      self.checkVal('XPLoseKeyboardFocus', XPGetWidgetWithFocus(), self.widget)

      try:
        self.checkVal('XPGetWidgetUnderlyingWindow', XPGetWidgetUnderlyingWindow(self.widget), None)
      except RuntimeError as re:
         if (self.versions[1] >= 301) or (str(re) != 'XPGetWidgetUnderlyingWindow is available only in XPLM301 and up.'):
            raise

      exists = []
      self.prop = 12345
      self.propVal = 23456
      self.checkVal('XPGetWidgetProperty', XPGetWidgetProperty(self.widget, self.prop, exists), 0)
      self.checkVal('XPGetWidgetProperty exists', exists[0], 0)
      XPSetWidgetProperty(self.widget, self.prop, self.propVal)
      exists = []
      self.checkVal('XPGetWidgetProperty', XPGetWidgetProperty(self.widget, self.prop, exists), self.propVal)
      self.checkVal('XPGetWidgetProperty exists', exists[0], 1)

      self.w_class = 4356
      cbk = XPGetWidgetClassFunc(self.w_class)
      self.checkVal('XPGetWidgetClassFunc', XPLMGetDatai(self.int0Dref), self.w_class)
      XPAddWidgetCallback(self, self.widget, cbk)
      XPAddWidgetCallback(self, self.widget, self.c_widgetCallback)
      
      self.message = 4560
      self.mode = 4561
      self.params = (12345, 23456)
      self.returnRes = 5678
      self.customCallbackCalled = 0
      self.target_widget = self.widget
      res = XPSendMessageToWidget(self.widget, self.message, self.mode, self.params[0], self.params[1])
      self.checkVal('widget callback called', self.customCallbackCalled, 1)
      self.checkVal('widget callback retval', res, self.returnRes)

      self.returnRes = 0
      self.customCallbackCalled = 0
      res = XPSendMessageToWidget(self.widget, self.message, self.mode, self.params[0], self.params[1])
      self.checkVal('widget callback called', self.customCallbackCalled, 1)
      self.checkVal('widget callback retval', res, self.message + 2 * self.params[0] + 3 * self.params[1])
      self.checkVal('widget callback message', XPLMGetDatai(self.int0Dref), self.message)
      self.checkVal('widget callback params', (XPLMGetDatai(self.int1Dref), XPLMGetDatai(self.int2Dref)), self.params)


      self.destroyChildren = 6789
      XPDestroyWidget(self, self.cust_widget1, self.destroyChildren)
      self.checkVal('XPDestroyWidget destroyChildren', XPLMGetDatai(self.int0Dref), self.destroyChildren)
      self.checkVal('XPCountChildWidgets', XPCountChildWidgets(self.widget), 1)

      self.destroyChildren = 1
      XPDestroyWidget(self, self.widget, self.destroyChildren)
      self.checkVal('XPDestroyWidget destroyChildren', XPLMGetDatai(self.int0Dref), self.destroyChildren)
      self.checkVal('XPDestroyWidget remaining', XPLMGetDatai(self.int1Dref), 0)

      return None

   def customCallback(self, inMessage, inWidget, inParam1, inParam2):
      if (inMessage == xpMsg_Destroy) or (inMessage == xpMsg_Create):
         return 0
      self.checkVal('callback\'s message', inMessage, self.message)
      self.checkVal('callback\'s params', (inParam1, inParam2), self.params)
      self.checkVal('callback\'s widget class', inWidget, self.target_widget)
      self.customCallbackCalled = True
      return self.returnRes;

   


