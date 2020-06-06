======= XPPython3 Plugin Development ==========
See also README_INSTALL.txt.

Plugin Development
------------------
Python coding errors / Exceptions will appear in XPPython3.Log. Error in some calls
to XP interface don't cause exceptions but rather just terminate the plugin's method
(e.g., terminate the flightloop) with no further message.

Note we set NativePaths ON (i.e.,  XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1))
Python2 plugin allowed you to be selected. XPPython3 doesn't. You should be using
native paths rather than legacy paths. (You can use python to convert from one
to the other if it becomes necessary.)

CHANGES from Python2 SDK
* 303 is base version (XP 11.50). There is no attempt to implement deprecated API
  or test on earlier versions
* XPLM_USE_NATIVE_PATHS is automatically set on startup. We never use legacy paths,
  so XPLMGetSystemPath() will return '/' separated results. If you need legacy paths,
  use python within your plugin to convert.
* REMOVAL of passing 'self' as first parameter in callback registrations
  We're using stack manipulation to do that for you instead.
  [Presently (5/2020), you can either always pass self (as with Python2 interface),
   or never pass self -- mixing styles will result in errors.
   The plan is to completely remove the option, and NEVER require passing self.]
      XPLMLoadObjectAsync(path, callback, refcon) not XPLMLoadObjectAsync(self, path, callback, refcon)

* XPGetWidgetWithFocus() returns None, instead of 0, if no widgets have focus

  All routines return "PyCapsules" rather than ints for XPLMWindowIDRef, XPLMHotkeyIDRef, XPLMMenuIDRef, XPLMWidgetID

* PI_GetMouseState() no longer supported: you'll now get tuple (x, y, button, delta) as param1 in widget callbacks
  (for xpMsg_MouseDown, xpMsg_MouseDrag, xpMsg_MouseUp, xpMsp_CursorAdjust)
* PI_GetKeyState() no longer supported: you'll now get tuple (key, flags, vkey) as param1 in widget callbacks
  (for xpMsg_KeyPress)
* PI_GetWidgetGeometry() no longer supported: you'll now get tuple (dx, dy, dwidth, dheight) as param1 in widget callbacks
  (for xpMsg_Reshape)




NEW
  XPLMUtilities.XPLMPythonGetDicts()
    returns dictionary of internal plugin python dictionaries. The Plugin already stores lists of
    items registered by each (XPython3) plugin. You may be able to use these, read-only,
    rather than maintaining your own list of things you've registered.
    Documentation of these dicts are TBD.


