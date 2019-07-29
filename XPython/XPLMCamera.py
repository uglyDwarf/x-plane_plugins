"""XPLM Camera APIs (based on CHeaders/XPLM/XPLMCamera.h)

 XPLMCamera - THEORY OF OPERATION The XPLMCamera APIs allow plug-ins to 
 control the camera angle in X-Plane. This has a number of applications, 
 including but not limited to: 
 
 - Creating new views (including dynamic/user-controllable views) for the 
 user. 
 
 - Creating applications that use X-Plane as a renderer of scenery, 
 aircrafts, or both. 
 
 The camera is controlled via six parameters: a location in OpenGL 
 coordinates and pitch, roll and yaw, similar to an airplane's position. 
 OpenGL coordinate info is described in detail in the XPLMGraphics 
 documentation; generally you should use the XPLMGraphics routines to 
 convert from world to local coordinates. The camera's orientation starts 
 facing level with the ground directly up the negative-Z axis (approximately 
 north) with the horizon horizontal. It is then rotated clockwise for yaw, 
 pitched up for positive pitch, and rolled clockwise around the vector it is 
 looking along for roll. 
 
 You control the camera either either until the user selects a new view or 
 permanently (the later being similar to how UDP camera control works). You 
 control the camera by registering a callback per frame from which you 
 calculate the new camera positions. This guarantees smooth camera motion. 
 
 Use the XPLMDataAccess APIs to get information like the position of the 
 aircraft, etc. for complex camera positioning.                              

 Camera position
 It is a list containing a full specification of the camera. X, Y, and Z are 
 the camera's position in OpenGL coordiantes; pitch, roll, and yaw are 
 rotations from a camera facing flat north in degrees. Positive pitch means 
 nose up, positive roll means roll right, and positive yaw means yaw right, 
 all in degrees. Zoom is a zoom factor, with 1.0 meaning normal zoom and 2.0 
 magnifying by 2x (objects appear larger).

"""


# XPLMCameraControlDuration
#
# This enumeration states how long you want to retain control of the camera. 
# You can retain it indefinitely or until the user selects a new view.        

# Control the camera until the user picks a new view.
xplm_ControlCameraUntilViewChanges = 1
# Control the camera until your plugin is disabled or another plugin forcably
#   takes control.
xplm_ControlCameraForever          = 2


def XPLMCameraControl_f(self, outCameraPosition, inIsLosingControl, inRefcon):
    """
    You use an XPLMCameraControl function to provide continuous control over 
    the camera. You are passed in a structure in which to put the new camera 
    position; modify it and return 1 to reposition the camera. Return 0 to 
    surrender control of the camera; camera control will be handled by X-Plane 
    on this draw loop. The contents of the structure as you are called are 
    undefined. 
    
    If X-Plane is taking camera control away from you, this function will be 
    called with inIsLosingControl set to 1 and outCameraPosition None.
    """
    pass

def XPLMControlCamera(inHowLong, inControlFunc, inRefcon):
    """
    This function repositions the camera on the next drawing cycle. You must 
    pass a non-null control function. Specify in inHowLong how long you'd like 
    control (indefinitely or until a key is pressed).
    """
    pass

def XPLMDontControlCamera():
    """
    This function stops you from controlling the camera. If you have a camera 
    control function, it will not be called with an inIsLosingControl flag. 
    X-Plane will control the camera on the next cycle. 
    
    For maximum compatibility you should not use this routine unless you are in 
    posession of the camera.
    """
    pass

def XPLMIsCameraBeingControlled(outCameraControlDuration)
    """
    Return a tuple 
    The first tuple element is 1 if the camera is being controlled, 0 otherwise.
    If the first element is nonzero, the second one contains duration of the
    current camera control.
    """
    pass

def XPLMReadCameraPosition(outCameraPosition)
    """
    Read current camera position.

    If passed a list outCameraPosition, fields x, y, z, pitch, headning,
      roll and zoom are returned in the list.
    Otherwise a list with the same fields is returned.
    """
    pass


