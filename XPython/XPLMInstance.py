"""XPLM APIs (based on CHeaders/XPLM/XPLMInstance.h)

 This API provides instanced drawing of X-Plane objects (.obj files). In 
 contrast to old drawing APIs, which required you to draw your own objects 
 per-frame, the instancing API allows you to simply register an OBJ for 
 drawing, then move or manipulate it later (as needed). 
 
 This provides one tremendous benefit: it keeps all dataref operations for 
 your object in one place. Because datarefs are main thread only, allowing 
 dataref access anywhere is a serious performance bottleneck for the 
 simulator---the whole simulator has to pause and wait for each dataref 
 access. This performance penalty will only grow worse as X-Plane moves 
 toward an ever more heavily multithreaded engine. 
 
 The instancing API allows X-Plane to isolate all dataref manipulations for 
 all plugin object drawing to one place, potentially providing huge 
 performance gains. 
 
 Here's how it works: 
 
 When an instance is created, it provides a list of all datarefs you want to 
 manipulate in for the OBJ in the future. This list of datarefs replaces the 
 ad-hoc collections of dataref objects previously used by art assets. Then, 
 per-frame, you can manipulate the instance by passing in a "block" of 
 packed floats representing the current values of the datarefs for your 
 instance. (Note that the ordering of this set of packed floats must exactly 
 match the ordering of the datarefs when you created your instance.)
"""

###############################################################################
def XPLMCreateInstance(obj, datarefs):
    """
    Registers an instance of an X-Plane object.
        
        obj      - integer (handle returned by XPLMLoadObject
                       or XPLMLoadObjectAsync)
        datarefs - sequence of strings (dataref names)

        returns instance handle
    """
    pass


def XPLMDestroyInstance(instance):
    """
    Unregisters an instance.

        instance - integer (handle from XPLMCreateInstance)
    """
    pass


def XPLMInstanceSetPosition(instance, new_position, data):
    """
    Updates both the position of the instance and all datarefs you registered
    for it.

        instance     - integer (handle from XPLMCreateInstance)
        new_position - sequence of floats (x, y, z, pitch, heading, roll)
        data         - sequence of floats (values of datarefs)
    """
    pass

