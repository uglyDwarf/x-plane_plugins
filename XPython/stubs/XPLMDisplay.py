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

 Direct drawing allows you to override scenery, panels, or anything. Note
 that you cannot assume that you are the only plug-in drawing at this
 phase.

 Window drawing provides slightly higher level functionality. With window
 drawing you create a window that takes up a portion of the screen. Window
 drawing is always two dimensional. Window drawing is front-to-back
 controlled; you can specify that you want your window to be brought on
 top, and other plug-ins may put their window on top of you. Window drawing
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

xplm_Phase_Modern3D = 31

# This is the first phase where you can draw in 2-d.
xplm_Phase_FirstCockpit = 35

# The non-moving parts of the aircraft panel.
xplm_Phase_Panel = 40

# The moving parts of the aircraft panel.
xplm_Phase_Gauges = 45

# Floating windows from plugins.
xplm_Phase_Window = 50

# The last change to draw in 2d.
xplm_Phase_LastCockpit = 55


###############################################################################

def XPLMDrawCallback_f(inPhase, inIsBefore, inRefcon):
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
    return int  # 0= Suppress x-plane drawing; 1=let x-plane draw

###############################################################################


def XPLMKeySniffer_f(inChar, inFlags, inVirtualKey, inRefcon):
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
    return int  # 0= consume the key; 1= pass the key to next sniffer (window mgr, x-plane, whomever)

###############################################################################


def XPLMRegisterDrawCallback(inCallback, inPhase, inWantsBefore, inRefcon):
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
    return int  # 0=phase does not exist; 1=registration successful

###############################################################################


def XPLMUnregisterDrawCallback(inCallback, inPhase, inWantsBefore, inRefcon):
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
    return int  # 1=callback found and unregistered, 0=otherwise


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
xplm_MouseDown = 1
xplm_MouseDrag = 2
xplm_MouseUp = 3


###############################################################################
# XPLMCursorStatus
#
# XPLMCursorStatus describes how you would like X-Plane to manage the cursor.
# See XPLMHandleCursor_f for more info.

# X-Plane manages the cursor normally, plugin does not affect the cusrsor.
xplm_CursorDefault = 0

# X-Plane hides the cursor.
xplm_CursorHidden = 1

# X-Plane shows the cursor as the default arrow.
xplm_CursorArrow = 2

# X-Plane shows the cursor but lets you select an OS cursor.
xplm_CursorCustom = 3


###############################################################################

def XPLMDrawWindow_f(inWindowID, inRefcon):
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


###############################################################################

def XPLMHandleKey_f(inWindowID, inKey, inFlags, inVirtualKey, inRefcon, losingFocus):
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


###############################################################################
def XPLMHandleMouseClick_f(inWindowID, x, y, inMouse, inRefcon):
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
    return int  # 1=consume the click; 0=pass it through

###############################################################################


def XPLMHandleCursor_f(inWindowID, x, y, inRefcon):
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

    XPLMCursortStatus:
      xplm_CursorDefault
       - X-Plane manages the cursor normally, plugin does not affect the cusrsor.
      xplm_CursorHidden
       - X-Plane hides the cursor.
      xplm_CursorArrow
       - X-Plane shows the cursor as the default arrow.
      xplm_CursorCustom
       - X-Plane shows the cursor but lets you select an OS cursor.
    """
    return int  # XPLMCursorStatus

###############################################################################


def XPLMHandleMouseWheel_f(inWindowID, x, y, wheel, clicks, inRefcon):
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
    return int  # 1=consume the mouse wheel click; 0=pass them on to a lower window

###############################################################################
""" XPLMWindowLayer
 XPLMWindowLayer describes where in the ordering of windows X-Plane should place
 a particular window. Windows in higher layers cover windows in lower layers.
 So, a given window might be at the top of its particular layer, but it might
 still be obscured by a window in a higher layer. (This happens frequently when
 floating windows, like X-Plane’s map, are covered by a modal alert.)

 Your window’s layer can only be specified when you create the window (in the
 XPLMCreateWindow_t you pass to XPLMCreateWindowEx()). For this reason, layering
 only applies to windows created with new X-Plane 11 GUI features. (Windows created
 using the older XPLMCreateWindow(), or windows compiled against a pre-XPLM300
 version of the SDK will simply be placed in the flight overlay window layer.)

  xplm_WindowLayerFlightOverlay    The lowest layer, used for HUD-like displays while
                                   flying.

  xplm_WindowLayerFloatingWindows  Windows that "float" over the sim, like the X-Plane
                                   11 map does. If you are not sure which layer to
                                   create your window in, choose floating

  xplm_WindowLayerModel            An interruptive modal that covers the sim with a
                                   transparent black overaly to draw the user's focus
                                   to the alert.

  xplm_WindowLayerGrowlNotifications  "Growl"-style notifications that are visible in
                                   a corner of the screen, even over modals.
"""

xplm_WindowLayerFlightOverlay = 0
xplm_WindowLayerFloatingWindows = 1
xplm_WindowLayerModal = 2
xplm_WindowLayerGrowlNotifications = 3

""" XPLMWindowDecoration
 XPLMWindowDecoration describes how “modern” windows will be displayed. This
 impacts both how X-Plane draws your window as well as certain mouse handlers.

 Your window’s decoration can only be specified when you create the window
 (in the XPLMCreateWindow_t you pass to XPLMCreateWindowEx()).

   xplm_WindowDecorationNone                    X-Plane will draw no decoration for
                                                your window, and apply no automatic
                                                click handlers. The window will not
                                                stop click from passing through its
                                                bounds. This is suitable for "windows"
                                                which request, say, the full screen
                                                bounds, then only draw in a small
                                                portion of the available area.

   xplm_WindowDecorationRoundRectangle          The default decoration for
                                                "native" windows, like the map.
                                                Provides a solid background, as
                                                well as click handlers for resizing
                                                and dragging the window.

   xplm_WindowDecorationSelfDecorated           X-Plane will draw no decoration
                                                for your window, nor will it
                                                provide resize handlers for your
                                                window edges, but it will stop
                                                clicks from passing through your
                                                windows bounds.

   xplm_WindowDecorationSelfDecoratedResizable  Like self-decorated, but with
                                                resizing; X-Plane will draw no
                                                decoration for your window, but
                                                it will stop clicks from passing
                                                through your windows bounds, and
                                                provide automatic mouse handlers
                                                for resizing.
"""
xplm_WindowDecorationNone = 0
xplm_WindowDecorationRoundRectangle = 1
xplm_WindowDecorationSelfDecorated = 2
xplm_WindowDecorationSelfDecoratedResizable = 3

"""XPLMCreateWindow_t

 The XPMCreateWindow_t structure deUnes all of the parameters used to create a modern
 window using XPLMCreateWindowEx(). The structure will be expanded in future SDK APIs
 to include more features. Always set the structSize member to the size of your struct
 in bytes!

 All windows created by this function in the XPLM300 version of the API are created
 with the new X-Plane 11 GUI features. This means your plugin will get to “know” abou
 the existence of X-Plane windows other than the main window. All drawing and mouse
 callbacks for your window will occur in “boxels,” giving your windows automatic
 support for high-DPI scaling in X-Plane. In addition, your windows can opt-in to
 decoration with the X-Plane 11 window styling, and you can use the
 XPLMSetWindowPositioningMode() API to make your window “popped out” into a First-class
 operating system window.

 Note that this requires dealing with your window’s bounds in “global desktop” positioning
 units, rather than the traditional panel coordinate system. In global desktop coordinates,
 the main X-Plane window may not have its origin at coordinate (0, 0), and your own window
 may have negative coordinates. Assuming you don’t implicitly assume (0, 0) as your origin,
 the only API change you should need is to start using XPLMGetMouseLocationGlobal() rather
 than XPLMGetMouseLocation(), and XPLMGetScreenBoundsGlobal() instead of XPLMGetScreenSize().

 If you ask to be decorated as a floating window, you’ll get the blue window control bar
 and blue backing that you see in X-Plane 11’s normal “floating” windows (like the map).

"""


def XPLMCreateWindowEx(inParams):
    """
    This routine creates a new “modern” window. You pass in an XPLMCreateWindow_t tuple
    with all of the Fields set in. Also, you must provide functions for every
    callback—you may not leave them null! (If you do not support the cursor or mouse wheel,
    use functions that return the default values.)

    (int left, int top, int right, int bottom,
     int visible,
     XPLMDrawWindow_f drawWindowFunc,
     XPLMHandleMouseClick_f handleMouseClickFunc,
     XPLMHandleKey_f handleKeyFunc,
     XPLMHandleCursor_f handleCursorFunc,
     XPLMHandleMouseWheel_f handleMouseWheelFunc,
     refcon,
     XPLMWindowDecoration decorateAsFloatingWindow,
     XPLMWindowLayer layer,
     XPLMHandleMouseClick_f handleRightClickFun
    )

    """
    return int  # XPLMWindowID


def XPLMDestroyWindow(inWindowID):
    """Destroys a window based on the handle passed in.

    inWindowID - integer

    The callbacks are not called after this call. Keyboard focus is removed
    from the window before destroying it.
    """


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


def XPLMGetScreenBoundsGlobal(outLeft, outTop, outRight, outBottom):
    """
    This routine returns the bounds of the “global” X-Plane desktop, in boxels.
    Unlike the non-global version XPLMGetScreenSize(), this is multi-monitor
    aware. There are three primary consequences of multimonitor awareness.

    First, if the user is running X-Plane in full-screen on two or more monitors
    (typically configured using one full-screen window per monitor), the global
    desktop will be sized to include all X-Plane windows.

    Second, the origin of the screen coordinates is not guaranteed to be (0, 0).
    Suppose the user has two displays side- by-side, both running at 1080p.
    Suppose further that they’ve configured their OS to make the left display
    their “primary” monitor, and that X-Plane is running in full-screen on their
    right monitor only. In this case, the global desktop bounds would be the
    rectangle from (1920, 0) to (3840, 1080). If the user later asked X-Plane to
    draw on their primary monitor as well, the bounds would change to (0, 0) to
    (3840, 1080).

    Finally, if the usable area of the virtual desktop is not a perfect rectangle
    (for instance, because the monitors have different resolutions or because one
    monitor is configured in the operating system to be above and to the right of
    the other), the global desktop will include any wasted space. Thus, if you have
    two 1080p monitors, and monitor 2 is conUgured to have its bottom left touch
    monitor 1’s upper right, your global desktop area would be the rectangle from
    (0, 0) to (3840, 2160).

    Note that popped-out windows (windows drawn in their own operating system
    windows, rather than “floating” within X-Plane) are not included in these bounds.
    """


def XPLMGetAllMonitorBoundsGlobal(inMonitorBoundsCallback, inRefcon):
    """
    This routine immediately calls you back with the bounds (in boxels) of each
    full-screen X-Plane window within the X- Plane global desktop space. Note that
    if a monitor is not covered by an X-Plane window, you cannot get its bounds this
    way. Likewise, monitors with only an X-Plane window (not in full-screen mode)
    will not be included.

    If X-Plane is running in full-screen and your monitors are of the same size and
    conUgured contiguously in the OS, then the combined global bounds of all full-screen
    monitors will match the total global desktop bounds, as returned by
    XPLMGetScreenBoundsGlobal(). (Of course, if X-Plane is running in windowed mode,
    this will not be the case. Likewise, if you have differently sized monitors, the
    global desktop space will include wasted space.)

    Note that this function’s monitor indices match those provided by
    XPLMGetAllMonitorBoundsOS(), but the coordinates are different (since the X-Plane
    global desktop may not match the operating system’s global desktop, and one X-Plane
    boxel may be larger than one pixel due to 150% or 200% scaling).

    Callback is:

       def inMonitorBoundsCallback(inMonitorIndex,
                                   inLeftBx, inTopBx, inRightBx, inBottomBx, inRefcon):
            pass

    This function is informed of the global bounds (in boxels) of a particular monitor
    within the X-Plane global desktop space. Note that X-Plane must be running in full
    screen on a monitor in order for that monitor to be passed to you in this callback.
    """


def XPLMGetAllMonitorBoundsOS(inMonitorBoundCallback, inRefcon):
    """
    This routine immediately calls you back with the bounds (in pixels) of each monitor
    within the operating system’s global desktop space. Note that unlike
    XPLMGetAllMonitorBoundsGlobal(), this may include monitors that have no X-Plane window
    on them.

    Note that this function’s monitor indices match those provided by
    XPLMGetAllMonitorBoundsGlobal(), but the coordinates are different (since the X-Plane
    global desktop may not match the operating system’s global desktop, and one X-Plane
    boxel may be larger than one pixel).

    Callback is:

       def inMonitorBoundsCallback(inMonitorIndex,
                                   inLeftPx, inTopPx, inRightPx, inBottomPx, inRefcon):
            pass
    This function is informed of the global bounds (in pixels) of a particular monitor
    within the operating system’s global desktop space. Note that a monitor index being
    passed to you here does not indicate that X-Plane is running in full screen on this
    monitor, or even that any X-Plane windows exist on this monitor.
    """


###############################################################################
def XPLMGetMouseLocationGlobal(outX, outY):
    """
    outX - list
    outY - list

    Returns the current mouse location in global desktop boxels. Unlike
    XPLMGetMouseLocation(), the bottom left of the main X-Plane window is not guaranteed
    to be (0, 0)—instead, the origin is the lower left of the entire global desktop space.
    In addition, this routine gives the real mouse location when the mouse goes to X-Plane
    windows other than the primary display. Thus, it can be used with both pop-out windows
    and secondary monitors.

    This is the mouse location function to use with modern windows (i.e., those created by
    XPLMCreateWindowEx()).
    """


###############################################################################
###############################################################################
def XPLMGetWindowGeometry(inWindowID, outLeft, outTop, outRight, outBottom):
    """

    inWindowID - integer
    outLeft    - list
    outTop     - list
    outRight   - list
    outBottom  - list

    This routine returns the position and size of a window. The units and coordinate
    system vary depending on the type of window you have.

    If this is a legacy window (one compiled against a pre-XPLM300 version of the SDK,
    or an XPLM300 window that was not created using XPLMCreateWindowEx()), the units
    are pixels relative to the main X-Plane display.

    If, on the other hand, this is a new X-Plane 11-style window (compiled against the
    XPLM300 SDK and created using XPLMCreateWindowEx()), the units are global desktop boxels.
   """


###############################################################################
def XPLMSetWindowGeometry(inWindowID, inLeft, inTop, inRight, inBottom):
    """Set window position and size.

    inWindowID - integer
    inLeft     - integer
    inTop      - integer
    inRight    - integer
    inBottom   - integer

    This routine allows you to set the position and size of a window.

    The units and coordinate system match those of XPLMGetWindowGeometry(). That is,
    modern windows use global desktop boxel coordinates, while legacy windows use
    pixels relative to the main X-Plane display.

    Note that this only applies to “floating” windows (that is, windows that are drawn
    within the X-Plane simulation windows, rather than being “popped out” into their
    own first-class operating system windows). To set the position of windows whose
    positioning mode is xplm_WindowPopOut, you’ll need to instead use XPLMSetWindowGeometryOS().
   """


def XPLMGetWindowGeometryOS(inWindowID, outLeft, outTop, outRight, outBottom):
    """

    inWindowID - integer
    outLeft    - list
    outTop     - list
    outRight   - list
    outBottom  - list

    This routine returns the position and size of a “popped out” window (i.e., a window
    whose positioning mode is xplm_WindowPopOut), in operating system pixels.
    """


def XPLMSetWindowGeometryOS(inWindowID, inLeft, inTop, inRight, inBottom):
    """

    inWindowID - integer
    inLeft    - list
    inTop     - list
    inRight   - list
    inBottom  - list

    This routine allows you to set the position and size, in operating system pixel
    coordinates, of a popped out window (that is, a window whose positioning mode
    is xplm_WindowPopOut, which exists outside the X-Plane simulation window, in its
    own Urst-class operating system window).

    Note that you are responsible for ensuring both that your window is popped out
    (using XPLMWindowIsPoppedOut()) and that a monitor really exists at the OS coordinates
    you provide (using XPLMGetAllMonitorBoundsOS()).
    """


def XPLMGetWindowGeometryVR(inWindowID, outWidthBoxels, outHeightBoxels):
    """
    Returns the width and height, in boxels, of a window in VR. Note that you are responsible
    for ensuring your window is in VR (using XPLMWindowIsInVR()).
    """


def XPLMSetWindowGeometryVR(inWindowID, widthBoxels, heightBoxels):
    """
    This routine allows you to set the size, in boxels, of a window in VR (that is, a
    window whose positioning mode is xplm_WindowVR).

    Note that you are responsible for ensuring your window is in VR (using XPLMWindowIsInVR()).
    """


###############################################################################
def XPLMGetWindowIsVisible(inWindowID):
    """Get window's isVisible attribute value.

    inWindow - integer
    """
    return int  # 1=visible


###############################################################################
def XPLMSetWindowIsVisible(inWindowID, inIsVisible):
    """Set window's isVisible attribute value.

    inWindowID  - integer
    inIsVisible - integer, 1=visible
    """


def XPLMWindowIsPoppedOut(inWindowID):
    """
    True if this window has been popped out (making it a Urst-class window in the
    operating system), which in turn is true if and only if you have set the
    window’s positioning mode to xplm_WindowPopOut.

    Only applies to modern windows. (Windows created using the deprecated
    XPLMCreateWindow(), or windows compiled against a pre-XPLM300 version of the
    SDK cannot be popped out.)
    """
    return int  # 1=True


def XPLMWindowIsInVR(inWindowID):
    """
    True if this window has been moved to the virtual reality (VR) headset, which
    in turn is true if and only if you have set the window’s positioning mode to xplm_WindowVR.

    Only applies to modern windows. (Windows created using the deprecated XPLMCreateWindow(), or windows
    compiled against a pre-XPLM301 version of the SDK cannot be moved to VR.)
    """
    return int  # 1=True


def XPLMSetWindowGravity(inWindowID, inLeftGravity, inTopGravity, inRightGravity, inBottomGravity):
    """
    A window’s “gravity” controls how the window shifts as the whole X-Plane window
    resizes. A gravity of 1 means the window maintains its positioning relative to the right or top
    edges, 0 the left/bottom, and 0.5 keeps it centered.

    Default gravity is (0.0, 1.0, 0.0, 1.0), meaning your window will maintain its position relative
    to the top left and will not change size as its containing window grows.

    If you wanted, say, a window that sticks to the top of the screen (with a constant height), but
    which grows to take the full width of the window, you would pass (0.0, 1.0, 1.0, 1.0). Because
    your left and right edges would maintain their positioning relative to their respective edges
    of the screen, the whole width of your window would change with the X-Plane window.

    Only applies to modern windows. (Windows created using the deprecated XPLMCreateWindow(), or
    windows compiled against a pre-XPLM300 version of the SDK will simply get the default gravity.)
    """


def XPLMSetWindowResizingLimits(inWindowID, inMinWidthBoxels, inMinHeightBoxels,
                                inMaxWidthBoxels, inMaxHightBoxels):
    """
    Sets the minimum and maximum size of the client rectangle of the given window. (That is,
    it does not include any window styling that you might have asked X-Plane to apply on your
    behalf.) All resizing operations are constrained to these sizes.

    Only applies to modern windows. (Windows created using the deprecated XPLMCreateWindow(),
    or windows compiled against a pre-XPLM300 version of the SDK will have no minimum or maximum size.)
    """


"""
XPLMWindowPositioningMode

XPLMWindowPositionMode describes how X-Plane will position your window on the user’s screen. X-Plane will
maintain this positioning mode even as the user resizes their window or adds/removes full-screen monitors.

Positioning mode can only be set for “modern” windows (that is, windows created using XPLMCreateWindowEx()
and compiled against the XPLM300 SDK). Windows created using the deprecated XPLMCreateWindow(), or windows
compiled against a pre-XPLM300 version of the SDK will simply get the “free” positioning mode.

xplm_WindowPositionFree             The default positioning mode. Set the window geometry and its
                                    future position will be determined by its window gravity, resizing
                                    limits, and user interactions.

xplm_WindowCenterOnMonitor          Keep the window centered on the monitor you specify

xplm_WindowFullScreenOnMonitor      Keep the window full screen on the monitor you specify

xplm_WindowFullScreenOnAllMonitors  Like gui_window_full_screen_on_monitor, but stretches
                                    over *all* monitors and popout windows.
                                    This is an obscure one... unless you have a very good
                                    reason to need it, you probably don't!

xplm_WindowPopOut                   A first-class window in the operating system, completely
                                    separate from the X-Plane window(s)

xplm_WindowVR                       A floating window visible on the VR headset
"""

xplm_WindowPositionFree = 0
xplm_WindowCenterOnMonitor = 1
xplm_WindowFullScreenOnMonitor = 2
xplm_WindowFullScreenOnAllMonitors = 3
xplm_WindowPopOut = 4
xplm_WindowVR = 5


def XPLMSetWindowPositioningMode(inWindowID, inPositioningMode, inMonitorIndex):
    """
    Sets the policy for how X-Plane will position your window.

    Some positioning modes apply to a particular monitor. For those modes, you can pass a negative
    monitor index to position the window on the main X-Plane monitor (the screen with the X-Plane
    menu bar at the top). Or, if you have a speciUc monitor you want to position your window on,
    you can pass a real monitor index as received from, e.g., XPLMGetAllMonitorBoundsOS().

    Only applies to modern windows. (Windows created using the deprecated XPLMCreateWindow(),
    or windows compiled against a pre-XPLM300 version of the SDK will always use xplm_WindowPositionFree.)
    """


def XPLMSetWindowTitle(inWindowID, inWindowTitle):
    """
    Sets the name for a window. This only applies to windows that opted-in to styling as an X-Plane
    11 `oating window (i.e., with styling mode xplm_WindowDecorationRoundRectangle) when they
    were created using XPLMCreateWindowEx().
    """


###############################################################################
def XPLMGetWindowRefCon(inWindowID):
    """
    Return window's refCon attribute value.
    """
    return int  # refcon


###############################################################################
def XPLMSetWindowRefCon(inWindowID, inRefcon):
    """Set window's refcon attribute value.

    inWindowID - integer
    inRefcon   - integer

    Use this to pass data to yourself in the callbacks.
    """


###############################################################################
def XPLMTakeKeyboardFocus(inWindowID):
    """
    Give a specific window keyboard focus.

    inWindowID - integer

    This routine gives a speciUc window keyboard focus. Keystrokes will be sent to that window.
    Pass a window ID of 0 to remove keyboard focus from any plugin-created windows and instead
    pass keyboard strokes directly to X-Plane.
    """


def XPLMHasKeyboardFocus(inWindowID):
    """
    Returns true (1) if the indicated window has keyboard focus. Pass a window ID of 0 to see
    if no plugin window has focus, and all keystrokes will go directly to X-Plane.
    """
    return int  # 1=window has focus;


###############################################################################
def XPLMBringWindowToFront(inWindowID):
    """
    Bring window to the front of the Z-order.

    This routine brings the window to the front of the Z-order for its layer. Windows are brought
    to the front automatically when they are created. Beyond that, you should make sure you are
    front before handling mouse clicks.

    Note that this only brings your window to the front of its layer (XPLMWindowLayer). Thus, if
    you have a window in the `oating window layer (xplm_WindowLayerFloatingWindows), but there
    is a modal window (in layer xplm_WindowLayerModal) above you, you would still not be the true
    frontmost window after calling this. (After all, the window layers are strictly ordered, and
    no window in a lower layer can ever be above any window in a higher one.)   Windows are brought
    to the front when they are created. Beyond that youshould make sure you are front before handling
    mouse clicks.
   """


###############################################################################
def XPLMIsWindowInFront(inWindowID):
    """
    This routine returns true if the window you passed in is the frontmost visible window in
    its layer (XPLMWindowLayer).

    Thus, if you have a window at the front of the `oating window layer
    (xplm_WindowLayerFloatingWindows), this will return true even if there is a modal window (in
    layer xplm_WindowLayerModal) above you. (Not to worry, though: in such a case, X-Plane will not
    pass clicks or keyboard input down to your layer until the window above stops “eating” the input.)

    Note that legacy windows are always placed in layer xplm_WindowLayerFlightOverlay, while
    modern-style windows default to xplm_WindowLayerFloatingWindows. This means it’s perfectly consistent
    to have two different plugin-created windows (one legacy, one modern) both be in the front
    (of their different layers!) at the same time.
    """
    return int  # 1=True


def XPLMRegisterKeySniffer(inCallback, inBeforeWindows, inRefcon):
    """
    This routine registers a key sniffing callback. You specify whether you want to sniff before
    the window system, or only sniff keys the window system does not consume. You should ALMOST
    ALWAYS sniff non-control keys after the window system. When the window system consumes a key, it
    is because the user has “focused” a window. Consuming the key or taking action based on the key
    will produce very weird results. Returns 1 if successful.

    def snifferCallback(inChar, inFlags, inVirtualKey, inRefcon):
        return int  # 1=pass the key to next sinnfer, 0=consume the key

    This is the prototype for a low level key-sniffing function. Window-based UI should not use this!
    The windowing system provides high-level mediated keyboard access, via the callbacks you attach
    to your XPLMCreateWindow_t. By comparison, the key sniffer provides low level keyboard access.

    Key sniffers are provided to allow libraries to provide non-windowed user interaction. For example,
    the MUI library uses a key sniffer to do pop-up text entry. Return 1 to pass the key on to the next sniffer,
    the window manager, X-Plane, or whomever is down stream. Return 0 to consume the key.

    Warning: this API declares virtual keys as a signed character; however the VKEY #define macros in XPLMDefs.h
    define the vkeys using unsigned values (that is 0x80 instead of -0x80). So you may need to cast the incoming
    vkey to an unsigned char to get correct comparisons in C.
    """
    return int  # 1=success


def XPLMUnregisterKeySniffer(inCallback, inBeforeWindows, inRefcon):
    """
    This routine unregisters a key sniffer. You must unregister a key sniffer for every time you register
    one with the exact same signature. Returns 1 if successful.
    """
    return int  # 1=success


###############################################################################
def XPLMHotKey_f(inRefcon):
    """The hotkey callback

    inRefcon - integer
    """
    pass


###############################################################################
def XPLMRegisterHotKey(inVirtualKey, inFlags, inDescription, inCallback, inRefcon):
    """
    Register a hotkey

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
    return int  # XMPLHotKeyID


###############################################################################
def XPLMUnregisterHotKey(inHotKey):
    """
    Unregister a hotkey.

    inHotKey - integer

    Only your own hotkeys can be unregistered!
    """


###############################################################################
def XPLMCountHotKeys():
    """
    Return number of hotkeys defined.
    """
    return int


###############################################################################
def XPLMGetNthHotKey(inIndex):
    """
    Returns ID of Nth hotkey.

      inIndex - integer

      returns integer (hotkeyID)
    """
    return int  # XPLMHotKeyID


###############################################################################
def XPLMGetHotKeyInfo(inHotKey, outVirtualKey, outFlags, outDescription, outPlugin):
    """
    Return information about the hotkey.

      inHotKey       - integer
      outVirtualKey  - integer (XPLM_VK_*)
      outFlags       - integer (xplm_*Flag)
      outDescription - string
      outPlugin      - integer
    """


###############################################################################
def XPLMSetHotKeyCombination(inHotKey, inVirtualKey, inFlags):
    """Remap a hot key's keystroke.

      inHotKey     - integer
      inVirtualKey - integer (XPLM_VK_*)
      inFlags      - integer (xplm_*Flags)

      You may remap another plugin's keystrokes.
    """


###############################################################################
