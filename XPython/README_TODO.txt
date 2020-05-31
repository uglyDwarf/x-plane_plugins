BUILD
-----
Separate out "sample.c" build info -- move to different directory with (equivalent) makefiles

CODE
----
Consider receipt of message XPLM_MSG_AIRPORT_LOADED, it's from XPLM_PLUGIN_XPLANE, which is
  defined as 0, but we're really returning None -- what should we do?

Clean out unnecessary comments
Consider Log.txt vs XPPython3.txt messages
  Keep Log.txt to a minimum
  Regularize output messages: include particular PI plugin, where useful
Push to github
Add python version number(?) to XPPython.log, initial output.. any other configuration/debug info?
Provide better samples for testing
Support (some?) of the Sandy Barbour convenience functions
Support PythonScriptMessaging functions
* number = PI_CountScripts()
* scriptID = PI_GetNthScript(index)
* scriptID = PI_FindScriptBySignature(signature)
* PI_GetScriptInfo(scriptID, &name, &signature, &description)
* boolean = PI_IsScriptEnabled(scriptID)
* boolean = PI_EnableScript(scriptID)
* PI_DisableScript(scriptID)
* PI_SendMessageToScript(<self>, scriptID | None, message, param)

Better debug reporting rather than crashing on bad input.

These appear to be 'internal', yet available interfaces from Python2 -- I don't see usefulness of these
  XPLMPlugin.PI_RemoveEnumerateFeatures
  XPLMScenery.PI_RemoveLookupObjects
  XPLMScenery.PI_RemoveObjectLoadedCallback
  XPLMUtilities.PI_RemoveErrorCallback


DOCUMENTATION
-------------
* For people attempting to build xppython3.xpl, see README_BUILD.txt
* For people attempting to build python plugins, see README_SDK.txt
* For people attempting to merely install / use, see README_INSTALL.txt

Includw with README_SDK, updated docs on SDK, Laminar resources, etc.

Build website with all this available.

API
---
Test or remove old version of pluginSelf
dynamically support alternative API for results: rather than passing in empty [], return tuple(?)

TEST
----
PI_SeeAndAvoid
PI_NOAA -- PORTED!! Cleanup -- run ONLY on mac (likely weatherServer won't starup correctly on lin or win, yet)
           ALSO, note it's still using the 'self' type callbacks (but as it's using them consistently, it
           still works.
