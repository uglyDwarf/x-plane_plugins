#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPLMMenus import *
from XPLMUtilities import *

import random

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'Menus');
   
   def XPluginStart(self):
      self.Name = "Menus regression test"
      self.Sig = "MenusRT"
      self.Desc = "Regression test for XPLMMenus module"
      self.versions = XPLMGetVersions()

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

      try:
         self.checkVal('XPLMFindAircraftMenu', XPLMFindAircraftMenu(), 0x11223344)
      except RuntimeError as re:
         if (self.versions[1] >= 300) or (str(re) != 'XPLMFindAircraftMenu is available only in XPLM300 and up.'):
            raise
      path = 'menu_item_test_command'
      self.cmdRef = XPLMCreateCommand(path, 'menu item command test')
      self.checkVal('XPLMFindCommand didn\'t find my new command.', XPLMFindCommand(path), self.cmdRef)
      
      self.cmdHandler = self.commandCallback
      self.cmdStatus = [0, 0]
      XPLMRegisterCommandHandler(self, self.cmdRef, self.cmdHandler, 1, self.cmdStatus)
      commandMenuName = 'Cmd Menu'
      try:
         res = XPLMAppendMenuItemWithCommand(menu, commandMenuName, self.cmdRef)
      except RuntimeError as re:
         if (self.versions[1] >= 300) or (str(re) != 'XPLMAppendMenuItemWithCommand is available only in XPLM300 and up.'):
            raise
      else:
         self.checkVal('XPLMAppendMenuItemWithCommand:res', res, 2)
         self.checkVal('XPLMAppendMenuItemWithCommand:name', self.getString(name), commandMenuName)
         self.checkVal('XPLMAppendMenuItemWithCommand:command', self.cmdStatus, [2, 5])
      XPLMUnregisterCommandHandler(self, self.cmdRef, self.cmdHandler, 1, self.cmdStatus)

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
      try:
         XPLMRemoveMenuItem(menu, itemIndex)
      except RuntimeError as re:
         if (self.versions[1] >= 210) or (str(re) != 'XPLMRemoveMenuItem is available only in XPLM210 and up.'):
            raise
      else:
         self.checkVal('XPLMRemoveMenuItem wasn\'t called.', self.getString(name), "removed")
         self.checkVal('XPLMRemoveMenuItem didn\'t pass the inIndex.', XPLMGetDatai(item), itemIndex)

      XPLMDestroyMenu(menu)
      self.checkVal('XPLMDestroyMenu wasn\'t called.', self.getString(name), "destroyed")


      return

   def menuHandler(self, inMenuRef, inItemRef):
      inMenuRef[0] += 1
      inItemRef[0] += 2
      return

   def commandCallback(self, inCommand, inPhase, inRefcon):
      self.checkVal('CommandCallback given unknown command reference.', inCommand, self.cmdRef)
      inRefcon[0] += 1
      inRefcon[1] |= 1 << inPhase
      return 1


