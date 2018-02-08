#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPWidgetDefs import *
from XPWidgets import *
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
      

      return 1.0
