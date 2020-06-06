# TODO
* Currently, always use instance method for callbacks (self.foo)
  a) can I use @classmethod
  b) @staticmethod
  c) standalone functions?
  
* Currently, we use "global" values for refcon, since we do rich compare (don't we?)
  can instead we just use regular values?, Strings, ints, floats,.. Perhaps even arrays or dicts,
  as long as they'd match?
     >>> a = {'a': 1, 'b': 2}
     >>> b = {'b': 2, 'a': 1}
     >>> a == b
     True
  
## BUILD
Separate out "sample.c" build info -- move to different directory with
(equivalent) makefiles

## CODE
Consider receipt of message XPLM_MSG_AIRPORT_LOADED, it's from XPLM_PLUGIN_XPLANE, which is
  defined as 0, but we're really returning None -- what should we do?

Clean out unnecessary comments
Consider Log.txt vs XPPython3.txt messages
  Keep Log.txt to a minimum
  Regularize output messages: include particular PI plugin, where useful

Provide better samples for testing

Support (some?) of the Sandy Barbour convenience functions

Better debug reporting rather than crashing on bad input.

These appear to be 'internal', yet available interfaces from Python2 -- I don't see usefulness of these
  XPLMPlugin.PI_RemoveEnumerateFeatures
  XPLMScenery.PI_RemoveLookupObjects
  XPLMScenery.PI_RemoveObjectLoadedCallback
  XPLMUtilities.PI_RemoveErrorCallback


## DOCUMENTATION

### BUILD:
More details on windows build
Describe how to build user-facing package (gzip)

### SDK:
Include updated docs on SDK, pointers to Laminar resources, etc.

### API
Test or remove old version of pluginSelf

Change from "out" params to real returns. This is a partial set:
display:
  XPLMGetScreenSize(outWidth, outHeight)
  XPLMGetMouseLocationGlobal(outX, outY)
  XPLMGetWindowGeometry(inWindowID, outLeft, outTop, outRight, outBottom)
  XPLMGetWindowGeometryOS(inWindowID, outLeft, outTop, outRight, outBottom)
  XPLMGetWindowGeometryVR(inWindowID, outWidthBoxels, outHeightBoxels)
  XPLMGetScreenBoundsGlobal(outLeft, outTop, outRight, outBottom)
  XPLMGetHotKeyInfo(inHotKey, outVirtualKey, outFlags, outDescription, outPlugin)

Support PythonScriptMessaging functions
* number = PI_CountScripts()
* scriptID = PI_GetNthScript(index)
* scriptID = PI_FindScriptBySignature(signature)
* PI_GetScriptInfo(scriptID, &name, &signature, &description)
* boolean = PI_IsScriptEnabled(scriptID)
* boolean = PI_EnableScript(scriptID)
* PI_DisableScript(scriptID)
* PI_SendMessageToScript(<self>, scriptID | None, message, param)

### TEST
PI_SeeAndAvoid
PI_NOAA -- PORTED!!
        Runs on Mac and Windows, not test Linux (likely weatherServer won't starup correctly yet).
