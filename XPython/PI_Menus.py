#!/usr/bin/env python3
#
from XPLMCHKHelper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPLMMenus import *

import random

class PythonInterface:
   def __init__(self):
      self.errors = 0
   
   def check(self):
      if self.errors == 0:
         print('Menus module check OK.')
      else:
         print('Menus module check: {0} errors found.'.format(self.errors))

   def checkVal(self, prompt, got, expected):
      #print("Going to check ", prompt)
      if got != None:
         if isinstance(expected, float):
            if abs(got - expected) > 4e-5:
               print(' ** ERROR ** {0}: got {1}, expected {2}'.format(prompt, got, expected))
               self.errors += 1
         elif isinstance(expected, list) or isinstance(expected, tuple):
            if len(got) != len(expected):
               print(' ** ERROR ** {0}: got {1}, expected {2}(bad length)'.format(prompt, got, expected))
               self.errors += 1
               return
            for v1, v2 in zip(got, expected):
               if abs(v1 - v2) > 4e-5:
                  print(' ** ERROR ** {0}: got {1}, expected {2} (|{3} - {4}| = {5})'.format(prompt, got, expected,
                        v1, v2, abs(v1-v2)))
                  self.errors += 1
                  return
                  
         else:
            if got != expected:
               print(' ** ERROR ** {0}: got {1}, expected {2}'.format(prompt, got, expected))
               self.errors += 1
         return
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
      self.Name = "Menus regression test"
      self.Sig = "MenusRT"
      self.Desc = "Regression test for XPLMMenus module"

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      name = XPLMFindDataRef("name")
      item = XPLMFindDataRef("item")
      lang = XPLMFindDataRef("lang")
      check = XPLMFindDataRef("check")

      menuName = "MyLittleMenu"
      parentItem = 42
      self.handler = self.menuHandler
      self.menuRef = [0]
      menu = XPLMCreateMenu(self, menuName, XPLMFindPluginsMenu(), parentItem, self.handler, self.menuRef)
      self.checkVal('XPLMCreateMenu didn\'t pass the name.', self.getString(name), menuName)
      self.checkVal('XPLMCreateMenu didn\'t pass the parentItem.', XPLMGetDatai(item), parentItem)
      XPLMClearAllMenuItems(menu)
      self.checkVal('XPLMClearAllMenuItems wasn\'t called.', XPLMGetDatai(item), 0)
      newMenuName = 'Pink fluffy unicorn' # Oh no, they got me brainwashed too!!!
      self.itemRef = [14]
      forEng = 6463
      res = XPLMAppendMenuItem(menu, newMenuName, self.itemRef, forEng)
      self.checkVal('XPLMAppendMenuItem dirn\'t return the right value', res, 1)
      self.checkVal('XPLMAppendMenuItem didn\'t pass name.', self.getString(name), newMenuName)
      self.checkVal('XPLMAppendMenuItem didn\'t pass the forceEnglish.', XPLMGetDatai(lang), forEng)
      self.checkVal('XPLMAppendMenuItem didn\'t call the menuHandler.', self.menuRef, [1])
      self.checkVal('XPLMAppendMenuItem didn\'t call the menuHandler.', self.itemRef, [16])
      XPLMAppendMenuSeparator(menu)
      self.checkVal('XPLMAppendMenuSeparator wasn\'t called.', self.getString(name), "separator")
      itemIndex = 36
      forEng = 888
      XPLMSetMenuItemName(menu, itemIndex, newMenuName, forEng)
      self.checkVal('XPLMAppendMenuItem didn\'t pass the inIndex.', XPLMGetDatai(item), itemIndex)
      self.checkVal('XPLMAppendMenuItem didn\'t pass name.', self.getString(name), newMenuName)
      self.checkVal('XPLMAppendMenuItem didn\'t pass the forceEnglish.', XPLMGetDatai(lang), forEng)
      
      itemIndex = 48
      checkVal = 445
      XPLMCheckMenuItem(menu, itemIndex, checkVal)
      self.checkVal('XPLMCheckMenuItem didn\'t pass the inIndex.', XPLMGetDatai(item), itemIndex)
      self.checkVal('XPLMCheckMenuItem didn\'t pass the inCheck.', XPLMGetDatai(check), checkVal)

      itemIndex = 446
      res = XPLMCheckMenuItemState(menu, itemIndex)
      self.checkVal('XPLMCheckMenuItemState didn\'t pass the inIndex.', XPLMGetDatai(item), itemIndex)
      self.checkVal('XPLMCheckMenuItemState didn\'t pass the inCheck.', res, checkVal)

      itemIndex = 447
      enabled = 556
      XPLMEnableMenuItem(menu, itemIndex, enabled)
      self.checkVal('XPLMEnableMenuItem didn\'t pass the inIndex.', XPLMGetDatai(item), itemIndex)
      self.checkVal('XPLMEnableMenuItem didn\'t pass the enabled.', XPLMGetDatai(lang), enabled)

      itemIndex = 448
      XPLMRemoveMenuItem(menu, itemIndex)
      self.checkVal('XPLMRemoveMenuItem wasn\'t called.', self.getString(name), "removed")
      self.checkVal('XPLMRemoveMenuItem didn\'t pass the inIndex.', XPLMGetDatai(item), itemIndex)

      XPLMDestroyMenu(menu)
      self.checkVal('XPLMDestroyMenu wasn\'t called.', self.getString(name), "destroyed")


      return

   def getString(self, dataref):
      out = []
      res = XPLMGetDatab(dataref, out, 0, 256)
      outStr = ""
      for x in out:
         outStr += chr(x)
      return outStr

   def menuHandler(self, inMenuRef, inItemRef):
      inMenuRef[0] += 1
      inItemRef[0] += 2
      return

