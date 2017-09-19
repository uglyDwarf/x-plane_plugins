"""XPLM Display APIs (based on CHeaders/XPLM/XPLMDisplay.h)

 This API provides the basic hooks to draw in X-Plane and create user
 interface. All X-Plane drawing is done in OpenGL.  The X-Plane plug-in
 manager takes care of properly setting up the OpenGL context and matrices.
 You do not decide when in your code's  execution to draw; X-Plane tells you
 when it is ready to have your plugin draw.

 X-Plane's drawing strategy is straightforward: every "frame" the screen is
 rendered by drawing the 3-d scene (dome, ground, objects, airplanes, etc.)
 and then drawing the cockpit on top of it.  Alpha blending is used to
 overlay the cockpit over the world (and the gauges over the panel, etc.).

 There are two ways you can draw: directly and in a window.

 Direct drawing involves drawing to the screen before or after X-Plane
 finishes a phase of drawing.  When you draw directly, you can specify
 whether x-plane is to complete this phase or not.  This allows you to do
 three things: draw before x-plane does (under it), draw after x-plane does
 (over it), or draw instead of x-plane.

 To draw directly, you register a callback and specify what phase you want
 to intercept.  The plug-in manager will call you over and over to draw that
 phase.

 Direct drawing allows you to override scenery, panels, or anything.  Note
 that you cannot assume that you are the only plug-in drawing at this
 phase.

 Window drawing provides slightly higher level functionality.  With window
 drawing you create a window that takes up a portion of the screen.  Window
 drawing is always two dimensional.  Window drawing is front-to-back
 controlled; you can  specify that you want your window to be brought on
 top, and other plug-ins may put their window on top of you.  Window drawing
 also allows you to sign up for key presses and receive mouse clicks.

 Note: all 2-d (and thus all window drawing) is done in 'cockpit pixels'.
 Even when the OpenGL window contains more than 1024x768 pixels, the cockpit
 drawing is magnified so that only 1024x768 pixels are available.

 There are three ways to get keystrokes:

 If you create a window, the window can take keyboard focus.  It will then
 receive all keystrokes.  If no window has focus, X-Plane receives
 keystrokes.  Use this to implement typing in dialog boxes, etc.  Only one
 window may have focus at a time; your window will be notified if it loses
 focus.

 If you need to associate key strokes with commands/functions in your
 plug-in, use a hot key.  A hoy is a key-specific callback.  Hotkeys are
 sent based on virtual key strokes, so any key may be distinctly mapped with
 any modifiers.  Hot keys  can be remapped by other plug-ins.  As a plug-in,
 you don't have to worry about  what your hot key ends up mapped to; other
 plug-ins may provide a UI for remapping keystrokes.  So hotkeys allow a
 user to resolve conflicts and customize keystrokes.

 If you need low level access to the keystroke stream, install a key
 sniffer.  Key sniffers can be installed above everything or right in front
 of the sim.

 DRAWING CALLBACKS
 Basic drawing callbacks are for low level intercepting of render loop. The
 purpose of drawing callbacks is to provide targeted additions or
 replacements to x-plane's graphics environment (for example, to add extra
 custom objects, or replace drawing of the AI aircraft).  Do not assume that
 the drawing callbacks will be called in the order implied by the
 enumerations. Also do not assume that each drawing phase ends before
 another begins; they may be nested.

 XPLMDrawingPhase
 This constant indicates which part of drawing we are in.  Drawing is done
 from the back to the front.  We get a callback before or after each item.
 Metaphases provide access to the beginning and end of the 3d (scene) and 2d
 (cockpit) drawing in a manner that is independent of new phases added via
 x-plane implementation.

 WARNING: As X-Plane's scenery evolves, some drawing phases may cease to
 exist and new ones may be invented. If you need a particularly specific
 use of these codes, consult Austin and/or be prepared to revise your code
 as X-Plane evolves.

 xplm_Phase_FirstScene
   - This is the earliest point at which you can draw in 3-d.

 xplm_Phase_Terrain
   - Drawing of land and water.

 xplm_Phase_Airports
   - Drawing runways and other airport detail.

 xplm_Phase_Vectors
   - Drawing roads, trails, trains, etc.

 xplm_Phase_Objects
   - 3-d objects (houses, smokestacks, etc.

 xplm_Phase_Airplanes
   - External views of airplanes, both yours and the AI aircraft.

 xplm_Phase_LastScene
   - This is the last point at which you can draw in 3-d.

 xplm_Phase_FirstCockpit
   - This is the first phase where you can draw in 2-d.

 xplm_Phase_Panel
   - The non-moving parts of the aircraft panel.

 xplm_Phase_Gauges
   - The moving parts of the aircraft panel.

 xplm_Phase_Window
   - Floating windows from plugins.

 xplm_Phase_LastCockpit
   - The last change to draw in 2d.

 xplm_Phase_LocalMap3D
   - 3-d Drawing for the local map.  Use regular OpenGL coordinates to draw in
     this phase.

 xplm_Phase_LocalMap2D
   - 2-d Drawing of text over the local map.

 xplm_Phase_LocalMapProfile
   - Drawing of the side-profile view in the local map screen.
"""

import XPLMDefs

# This is the earliest point at which you can draw in 3-d.
xplm_Phase_FirstScene                    = 0

# Drawing of land and water.
xplm_Phase_Terrain                       = 5

# Drawing runways and other airport detail.
xplm_Phase_Airports                      = 10

# Drawing roads, trails, trains, etc.
xplm_Phase_Vectors                       = 15

# 3-d objects (houses, smokestacks, etc.
xplm_Phase_Objects                       = 20

# External views of airplanes, both yours and the AI aircraft.
xplm_Phase_Airplanes                     = 25

# This is the last point at which you can draw in 3-d.
xplm_Phase_LastScene                     = 30

# This is the first phase where you can draw in 2-d.
xplm_Phase_FirstCockpit                  = 35

# The non-moving parts of the aircraft panel.
xplm_Phase_Panel                         = 40

# The moving parts of the aircraft panel.
xplm_Phase_Gauges                        = 45

# Floating windows from plugins.
xplm_Phase_Window                        = 50

# The last change to draw in 2d.
xplm_Phase_LastCockpit                   = 55

# 3-d Drawing for the local map.  Use regular OpenGL coordinates to draw in
#   this phase.
xplm_Phase_LocalMap3D                    = 100

# 2-d Drawing of text over the local map.
xplm_Phase_LocalMap2D                    = 101

# Drawing of the side-profile view in the local map screen.
xplm_Phase_LocalMapProfile               = 102



###############################################################################
def XPLMDrawCallback_f(self, inPhase, inIsBefore, inRefcon):
   """Prototype for a low level drawing callback.

      inPhase    - integer (xplm_Phase_*)
      inIsBefore - integer
      inRefcon   - integer

      return integer (0/1)

      You are passed in the phase and whether it is before or after. If you are
      before the phase, return 1 to let x-plane draw or 0 to suppress x-plane
      drawing. If you are after the phase the return value is ignored.

      Refcon is a unique value that you specify when registering the callback.

      Upon entry the OpenGL context will be correctly set up for you and OpenGL
      will be in 'local' coordinates for 3d drawing and panel coordinates for 2d
      drawing.  The OpenGL state (texturing, etc.) will be unknown.
   """
   pass

###############################################################################
def XPLMKeySniffer_f(self, inChar, inFlags, inVirtualKey, inRefcon):
   """Prototype for a low level key-sniffing callback.

      inChar       - integer
      inFlags      - integer (xplm_*Flag)
      inVirtualKey - integer (XPLM_VK_*)
      inRefcon     - integer

      return integer (0/1)

      Window-based UI should not use this! The windowing system provides
      high-level mediated keyboard access. By comparison, the key sniffer
      provides low level keyboard access.

      Key sniffers are provided to allow libraries to provide non-windowed user
      interaction. For example, the MUI library uses a key sniffer to do pop-up
      text entry.

      inKey is the character pressed, inRefCon is a value you supply during
      registration. Return 1 to pass the key on to the next sniffer, the window
      mgr, x-plane, or whomever is down stream. Return 0 to consume the key.
   """
   pass

###############################################################################
def XPLMRegisterDrawCallback(self, inCallback, inPhase, inWantsBefore, inRefcon):
   """ Register a low level drawing callback.

      inCallback    - callback reference
      inPhase       - integer (xplm_Phase_*)
      inWantsBefore - integer
      inRefcon      - integer

      returns integer (0/1)

      Pass in the phase you want to be called for and whether you want to be
      called before or after. This routine returns 1 if the registration was
      successful, or 0 if the phase does not exist in this version of x-plane.
      You may register a callback multiple times for the same or different
      phases as long as the refcon is unique for each time.
    """
   pass

###############################################################################
def XPLMUnregisterDrawCallback(self, inCallback, inPhase, inWantsBefore, inRefcon):
   """Unregister a low level drawing callback.

      inCallback    - callback reference
      inPhase       - integer (xplm_Phase_*)
      inWantsBefore - integer
      inRefcon      - integer

      returns integer (0/1)

      You must unregister a callback for each time you register a callback if
      you have registered it multiple times with different refcons.
      The routine returns 1 if it can find the callback to unregister, 0 otherwise.
    """
   pass

###############################################################################
def XPLMRegisterKeySniffer(self, inCallback, inBeforeWindows, inRefcon):
   """Registr a low level key sniffing callback.

      inCallback      - callback reference
      inBeforeWindows - integer
      inRefcon        - integer

      returns integer (0/1)

      Specify whether you want to sniff before the window system, or only sniff
      keys the window system does not consume. You should ALMOST ALWAYS sniff
      non-control keys after the window system. When the window system
      consumes a key, it is because the user has "focused" a window.
      Consuming the key or taking action based on the key will produce very
      weird results.
      Returns 1 if successful.
   """
   pass

###############################################################################
def XPLMUnregisterKeySniffer(self, inCallback, inBeforeWindows, inRefcon):
   """Unregister a low level key sniffing callback.

      inCallback      - callback reference
      inBeforeWindows - integer
      inRefcon        - integer

      returns integer (0/1)

      You must unregister a key sniffer for every time you register one with
      the exact same signature.
      Returns 1 if successful.
    """
   pass

###############################################################################
# WINDOW API
###############################################################################

###############################################################################
# XPLMMouseStatus
#
# When the mouse is clicked, your mouse click routine is called repeatedly.
# It is first called with the mouse down message.  It is then called zero or
# more times with the mouse-drag message, and finally it is called once with
# the mouse up message.  All of these messages will be directed to the same
# window.
xplm_MouseDown       = 1

xplm_MouseDrag       = 2

xplm_MouseUp         = 3


###############################################################################
# XPLMCursorStatus
#
# XPLMCursorStatus describes how you would like X-Plane to manage the cursor.
# See XPLMHandleCursor_f for more info.

# X-Plane manages the cursor normally, plugin does not affect the cusrsor.
xplm_CursorDefault                       = 0

# X-Plane hides the cursor.
xplm_CursorHidden                        = 1

# X-Plane shows the cursor as the default arrow.
xplm_CursorArrow                         = 2

# X-Plane shows the cursor but lets you select an OS cursor.
xplm_CursorCustom                        = 3


###############################################################################
def XPLMDrawWindow_f(self, inWindowID, inRefcon):
   """Window drawing callback prototype.

   inWindowID - integer
   inRefcon   - integer

   This function handles drawing. You are passed in your window and its
   refcon. Draw the window. You can use XPLM functions to find the current
   dimensions of your window, etc.  When this callback is called, the OpenGL
   context will be set properly for cockpit drawing. NOTE: Because you are
   drawing your window over a background, you can make a translucent window
   easily by simply not filling in your entire window's bounds.
   """
   pass

###############################################################################
def XPLMHandleKey_f(self, inWindowID, inKey, inFlags, inVirtualKey, inRefcon, losingFocus):
   """Window keyboard input handling callback prototype.

   inWindowID   - integer
   inKey        - integer
   inFlags      - integer (xplm_*Flag)
   inVirtualKey - integer (XPLM_VK_*)
   inRefcon     - integer
   losingFocus  - integer

   This function is called when a key is pressed or keyboard focus is taken
   away from your window.  If losingFocus is 1, you are losign the keyboard
   focus, otherwise a key was pressed and inKey contains its character.  You
   are also passed your window and a refcon.
   """
   pass

###############################################################################
def XPLMHandleMouseClick_f(self, inWindowID, x, y, inMouse, inRefcon):
   """Mouse handling (except for mouse wheels) callback prototype.

   inWindow - integer
   x        - integer
   y        - integer
   inMouse  - integer (xplm_Mouse*)
   inRefcon - integer

   You receive this call when the mouse button is pressed down or released.
   Between then these two calls is a drag.  You receive the x and y of the
   click, your window,  and a refcon.  Return 1 to consume the click, or 0 to
   pass it through.

   WARNING: passing clicks through windows (as of this writing) causes mouse
   tracking problems in X-Plane; do not use this feature!

   When the mouse is clicked, your mouse click routine is called repeatedly.
   It is first called with the mouse down message.  It is then called zero or
   more times with the mouse-drag message, and finally it is called once with
   the mouse up message.  All of these messages will be directed to the same
   window.

   inMouse parameter values
     xplm_MouseDown

     xplm_MouseDrag

     xplm_MouseUp
   """
   pass

###############################################################################
def XPLMHandleCursor_f(self, inWindowID, x, y, inRefcon):
   """Mouse cursor handling callback prototype.

   inWindow - integer
   x        - integer
   y        - integer
   inRefcon - integer

   return integer (xplm_Cursor*)

   The SDK calls your cursor status callback when the mouse is over your
   plugin window.  Return a cursor status code to indicate how you would like
   X-Plane to manage the cursor.  If you return xplm_CursorDefault, the SDK
   will try lower-Z-order plugin windows, then let the sim manage the cursor.

   Note: you should never show or hide the cursor yourself - these APIs are
   typically reference-counted and thus  cannot safely and predictably be used
   by the SDK.  Instead return one of xplm_CursorHidden to hide the cursor or
   xplm_CursorArrow/xplm_CursorCustom to show the cursor.

   If you want to implement a custom cursor by drawing a cursor in OpenGL, use
   xplm_CursorHidden to hide the OS cursor and draw the cursor using a 2-d
   drawing callback (after xplm_Phase_Window is probably a good choice).  If
   you want to use a custom OS-based cursor, use xplm_CursorCustom to ask
   X-Plane to show the cursor but not affect its image.  You can then use an
   OS specific call like SetThemeCursor (Mac) or SetCursor/LoadCursor
   (Windows).

   Return values details

      xplm_CursorDefault
       - X-Plane manages the cursor normally, plugin does not affect the cusrsor.

      xplm_CursorHidden
       - X-Plane hides the cursor.

      xplm_CursorArrow
       - X-Plane shows the cursor as the default arrow.

      xplm_CursorCustom
       - X-Plane shows the cursor but lets you select an OS cursor.
   """
   pass

###############################################################################
def XPLMHandleMouseWheel_f(self, inWindowID, x, y, wheel, clicks, inRefcon):
   """Mouse wheel handling callback prototype.

   inWindow - integer
   x        - integer
   y        - integer
   wheel    - integer
   clicks   - integer
   inRefcon - integer

   return integer (0/1)

   The SDK calls your mouse wheel callback when one of the mouse wheels is
   turned within your window.  Return 1 to consume the  mouse wheel clicks or
   0 to pass them on to a lower window.  (You should consume mouse wheel
   clicks even if they do nothing if your window appears opaque to the user.)
   The number of clicks indicates how far the wheel was turned since the last
   callback. The wheel is 0 for the vertical axis or 1 for the horizontal axis
   (for OS/mouse combinations that support this).
    """
   pass

###############################################################################
def XPLMGetScreenSize(outWidth, outHeight):
   """Query X-Plane screen size.

   outWidth  - list
   outHeight - list

   This routine returns the size of the size of the X-Plane OpenGL window in
   pixels.  Please note that this is not the size of the screen when  doing
   2-d drawing (the 2-d screen is currently always 1024x768, and  graphics are
   scaled up by OpenGL when doing 2-d drawing for higher-res monitors).  This
   number can be used to get a rough idea of the amount of detail the user
   will be able to see when drawing in 3-d.
   """
   pass

###############################################################################
def XPLMGetMouseLocation(outX, outY):
   """Get the current mouse location in cockpit pixels.

   outX - list or None
   outY - list or None

   The bottom left corner of the display is 0,0.  Pass None to not receive info
   about either parameter. Result will be in outX/Y[0].
   """
   pass

###############################################################################
def XPLMCreateWindow(self, inLeft, inTop, inRight, inBottom, inIsVisible,
                     inDrawCallback, inKeyCallback, inMouseCallback, inRefcon):
   """Creates a new window and return its handle.

   inLeft          - integer
   inTop           - integer
   inRight         - integer
   inBottom        - integer
   inIsVisible     - integer
   inDrawCallback  - function reference (XPLMDrawWindow_f)
   inKeyCallback   - function refernece (XPLMHandleKey_f)
   inMouseCallback - function reference (XPLMHandleMouseClick_f)

   returns integer (window handle)

   Pass in the dimensions and offsets to the window's bottom left corner from
   the bottom left of the screen. You can specify whether the window is
   initially visible or not. Also, you pass in three callbacks to run the
   window and a refcon. This function returns a window ID you can use to
   refer to the new window.

   NOTE: windows do not have "frames"; you are responsible for drawing the
   background and frame of the window.  Higher level libraries have routines
   which make this easy.
   """
   pass

###############################################################################
def XPLMCreateWindowEx(self, inParams):
   """Create a new window and return its handle.

   inParams - list with the following contents:
      left - integer
      top  - integer
      right - integer
      bottom - integer
      visible - integer
      drawWindowFunc - function refernce (XPLMDrawWindow_f)
      handleMouseClickFunc - function refernece (XPLMHandleMouseClick_f)
      handleKeyFunc - function reference (XPLMHandleKeyFunc_f)
      handleCursorFunc - function reference (XPLMHandleCursor_f)
      handleMouseWheelFunc - function reference (XPLMHandleMouseWheel_f)
      refcon - integer

   returns integer (window handle)

   Parameter functions are similar to those of XPLMCreateWindow.
   """
   pass

###############################################################################
def XPLMDestroyWindow(inWindowID):
   """Destroys a window based on the handle passed in.

   inWindowID - integer

   The callbacks are not called after this call. Keyboard focus is removed
   from the window before destroying it.
   """
   pass

###############################################################################
def XPLMGetWindowGeometry(inWindowID, outLeft, outTop, outRight, outBottom):
   """Get the window position and size in cockpit pixels.

   inWindowID - integer
   outLeft    - list or None
   outTop     - list or None
   outRight   - list or None
   outBottom  - list or None

   Pass None if not interested in some of the parameters.
   """
   pass

###############################################################################
def XPLMSetWindowGeometry(inWindowID, inLeft, inTop, inRight, inBottom):
   """Set window position and size.

   inWindowID - integer
   inLeft     - integer
   inTop      - integer
   inRight    - integer
   inBottom   - integer
   """
   pass

###############################################################################
def XPLMGetWindowIsVisible(inWindowID):
   """Get window's isVisible attribute value.

   inWindow - integer

   returns integer (0/1)
   """
   pass

###############################################################################
def XPLMSetWindowIsVisible(inWindowID, inIsVisible):
   """Set window's isVisible attribute value.

   inWindowID  - integer
   inIsVisible - integer
   """
   pass

###############################################################################
def XPLMGetWindowRefCon(inWindowID):
   """Return window's refCon attribute value.

   inWindowID  - integer

   returns integer(refcon)
   """
   pass

###############################################################################
def XPLMSetWindowRefCon(inWindowID, inRefcon):
   """Set window's refcon attribute value.

   inWindowID - integer
   inRefcon   - integer
   """
   pass

###############################################################################
def XPLMTkeKeyboardFocus(inWindowID):
   """Give a specific window keyboard focus.

   inWindowID - integer

   Keystrokes will be sent to that window. Pass a window ID of 0 to pass
   keyboard strokes directly to X-Plane.
   """
   pass

###############################################################################
def XPLMBringWindowToFront(inWindowID):
   """Bring window to the front of the Z-order.

   inWindowID - integer

   Windows are brought to the front when they are created. Beyond that you
   should make sure you are front before handling mouse clicks.
   """
   pass

###############################################################################
def XPLMIsWindowInFront(inWindowID):
   """Returns true if window is the frontmost visible.

   inWindowID - integer

   returns integer (0/1)
   """
   pass

###############################################################################
def XPLMHotKey_f(inRefcon):
   """The hotkey callback

   inRefcon - integer
   """
   pass

###############################################################################
def XPLMRegisterHotKey(self, inVirtualKey, inFlags, inDescription, inCallback,
                       inRefcon):
   """Register a hotkey

   inVirtualKey  - integer (XPLM_VK_*)
   inFlags       - integer (xplm_*Flag)
   inDescription - string
   inCallback    - function reference
   inRefcon      - integer

   return integer (hotkeyID)

   Specify your preferred key stroke virtual key/flag combination,
   a description of what your callback does (so the other plug-ins can
   describe the plug-in to the user for remapping) and a callback function
   and opaque pointer to pass in).  A new hot key ID is returned.
   During execution, the actual key associated with your hot key
   may change, but you are insulated from this.
   """
   pass

###############################################################################
def XPLMUnregisterHotKey(inHotKey):
   """Unregister a hotkey.

   inHotKey - integer

   Only your own hotkeys can be unregistered!
   """
   pass

###############################################################################
def XPLMCountHotKeys():
   """Return number of hotkeys defined.
   """
   pass

###############################################################################
def XPLMGetNthHotKey(inIndex):
   """Returns ID of Nth hotkey.

      inIndex - integer

      returns integer (hotkeyID)
   """
   pass

###############################################################################
def XPLMGetHotKeyInfo(inHotKey, outVirtualKey, outFlags, outDescription,
                      outPlugin):
   """Return information about the hotkey.

      inHotKey       - integer
      outVirtualKey  - integer (XPLM_VK_*)
      outFlags       - integer (xplm_*Flag)
      outDescription - string
      outPlugin      - integer
   """
   pass

###############################################################################
def XPLMSetHotKeyCombination(inHotKey, inVirtualKey, inFlags):
   """Remap a hot key's keystroke.

      inHotKey     - integer
      inVirtualKey - integer (XPLM_VK_*)
      inFlags      - integer (xplm_*Flags)

      You may remap another plugin's keystrokes.
   """
   pass

###############################################################################

