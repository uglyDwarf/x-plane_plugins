"""
The XPLMPlanes APIs allow you to control the various aircraft in X-Plane,
both the user's and the sim's.

Constants and enumerations
**************************
XPLM_USER_AIRCRAFT = 0
"""


def XPLMSetUsersAircraft(inAircraftPath):
    """
    This routine changes the user's aircraft.  Note that this will reinitialize
    the user to be on the nearest airport's first runway.  Pass in a full path
    (hard drive and everything including the .acf extension) to the .acf file.
    """
    pass


def XPLMPlaceUserAtAirport(inAirportCode):
    """
    This routine places the user at a given airport.  Specify the airport by
    its ICAO code (e.g. 'KBOS').
    """
    pass


def XPLMPlaceUserAtLocation(latitudeDegrees,
                            longitudeDegrees,
                            elevationMetersMSL,
                            headingDegreesTrue,
                            speedMetersPerSecond):
    """
    Places the user at a specific location after performing any necessary
    scenery loads.

    As with in-air starts initiated from the X-Plane user interface, the
    aircraft will always start with its engines running, regardless of the
    user's preferences (i.e., regardless of what the dataref
    sim/operation/prefs/startup_running says).
    """
    pass


def XPLMCountAircraft(outTotalAircraft, outActiveAircraft, outController):
    """
    This function returns the number of aircraft X-Plane is capable of having,
    as well as the number of aircraft that are currently active.  These numbers
    count the user's aircraft.  It can also return the plugin that is currently
    controlling aircraft.  In X-Plane 7, this routine reflects the number of
    aircraft the user has enabled in the rendering options window.

    outTotalAircraft : list, will contain number of planes X-Plane can have
    outActiveAircraft : list, will contain number of active planes
    outController : list, will return Id of plugin controlling the aircraft
    """
    pass


def XPLMGetNthAircraftModel(inIndex, outFileName, outPath):
    """
    This function returns the aircraft model for the Nth aircraft.  Indices are
    zero based, with zero being the user's aircraft.  The file name should be
    at least 256 chars in length; the path should be at least 512 chars in
    length.

    inIndex     : model index (integer)
    outFileName : list, will contain model filename
    outPath     : list, will contain path to the model
    """
    pass


def XPLMPlanesAvailable_f(self, inRefcon):
    """
    Your airplanes available callback is called when another plugin gives up
    access to the multiplayer planes.  Use this to wait for access to
    multiplayer.
    """
    pass


def XPLMAcquirePlanes(self, inAircraft, inCallback, inRefcon):
    """
    XPLMAcquirePlanes grants your plugin exclusive access to the aircraft.  It
    returns 1 if you gain access, 0 if you do not. inAircraft - pass in a list
    of strings specifying the planes you want loaded.  For any plane index you
    do not want loaded, pass an empty string.  Other strings should be full
    paths with the .acf extension.  Pass None if there are no planes you want
    loaded. If you pass in a callback and do not receive access to the planes
    your callback will be called when the airplanes are available. If you do
    receive airplane access, your callback will not be called.

    inAircraft : none or list planes to load (list of strings)
    inCallback : XPLMPlanesAvailable_f
    inRefcon   : object passed to the callback (any object)
    """
    pass


def XPLMReleasePlanes():
    """
    Call this function to release access to the planes.  Note that if you are
    disabled, access to planes is released for you and you must reacquire it.
    """
    pass


def XPLMSetActiveAircraftCount(inCount):
    """
    This routine sets the number of active planes.  If you pass in a number
    higher than the total number of planes availables, only the total number of
    planes available is actually used.
    """
    pass


def XPLMSetAircraftModel(inIndex, inAircraftPath):
    """
    This routine loads an aircraft model.  It may only be called if you  have
    exclusive access to the airplane APIs.  Pass in the path of the  model with
    the .acf extension.  The index is zero based, but you  may not pass in 0
    (use XPLMSetUsersAircraft to load the user's aircracft).
    """
    pass


def XPLMDisableAIForPlane(inPlaneIndex):
    """
    This routine turns off X-Plane's AI for a given plane.  The plane will
    continue to draw and be a real plane in X-Plane, but will not  move itself.
    """
    pass


def XPLMDrawAircraft(inPlaneIndex,
                     inX, inY, inZ,
                     inPitch, inRoll, inYaw,
                     inFullDraw,
                     inDrawStateInfo):
    """
    This routine draws an aircraft.  It can only be called from a 3-d drawing
    callback.  Pass in the position of the plane in OpenGL local coordinates
    and the orientation of the plane.  A 1 for full drawing indicates that the
    whole plane must be drawn; a 0 indicates you only need the nav lights
    drawn. (This saves rendering time when planes are far away.)
    """
    pass


def XPLMReinitUsersPlane():
    """
    This function recomputes the derived flight model data from the aircraft
    structure in memory.  If you have used the data access layer to modify the
    aircraft structure, use this routine to resynchronize X-Plane; since
    X-Plane works at least partly from derived values, the sim will not behave
    properly until this is called.

    WARNING: this routine does not necessarily place the airplane at the
    airport; use XPLMSetUsersAircraft to be compatible.  This routine is
    provided to do special experimentation with flight models without resetting
    flight.
    """
    pass

