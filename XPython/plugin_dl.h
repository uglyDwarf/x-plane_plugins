#ifndef PLUGIN_DL__H
#define PLUGIN_DL__H

#include <stdbool.h>
#define XPLM200
#define XPLM210
#define XPLM300
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMScenery.h>
#include <XPLM/XPLMMenus.h>
#include <XPLM/XPLMMap.h>
#include <XPLM/XPLMDisplay.h>
#include <XPLM/XPLMPlanes.h>

// Dynamicaly bound function pointers
// XPLM210
extern typeof(XPLMCreateFlightLoop) *XPLMCreateFlightLoop_ptr;
extern typeof(XPLMDestroyFlightLoop) *XPLMDestroyFlightLoop_ptr;
extern typeof(XPLMLoadObjectAsync) *XPLMLoadObjectAsync_ptr;
extern typeof(XPLMRemoveMenuItem) *XPLMRemoveMenuItem_ptr;
extern typeof(XPLMScheduleFlightLoop) *XPLMScheduleFlightLoop_ptr;

//XPLM300
extern typeof(XPLMAppendMenuItemWithCommand) *XPLMAppendMenuItemWithCommand_ptr;
extern typeof(XPLMCreateMapLayer) *XPLMCreateMapLayer_ptr;
extern typeof(XPLMDegMagneticToDegTrue) *XPLMDegMagneticToDegTrue_ptr;
extern typeof(XPLMDegTrueToDegMagnetic) *XPLMDegTrueToDegMagnetic_ptr;
extern typeof(XPLMDestroyMapLayer) *XPLMDestroyMapLayer_ptr;
extern typeof(XPLMDrawMapIconFromSheet) *XPLMDrawMapIconFromSheet_ptr;
extern typeof(XPLMDrawMapLabel) *XPLMDrawMapLabel_ptr;
extern typeof(XPLMFindAircraftMenu) *XPLMFindAircraftMenu_ptr;
extern typeof(XPLMGetAllMonitorBoundsGlobal) *XPLMGetAllMonitorBoundsGlobal_ptr;
extern typeof(XPLMGetAllMonitorBoundsOS) *XPLMGetAllMonitorBoundsOS_ptr;
extern typeof(XPLMGetMagneticVariation) *XPLMGetMagneticVariation_ptr;
extern typeof(XPLMGetMouseLocationGlobal) *XPLMGetMouseLocationGlobal_ptr;
extern typeof(XPLMGetScreenBoundsGlobal) *XPLMGetScreenBoundsGlobal_ptr;
extern typeof(XPLMGetWindowGeometryOS) *XPLMGetWindowGeometryOS_ptr;
extern typeof(XPLMMapExists) *XPLMMapExists_ptr;
extern typeof(XPLMMapGetNorthHeading) *XPLMMapGetNorthHeading_ptr;
extern typeof(XPLMMapProject) *XPLMMapProject_ptr;
extern typeof(XPLMMapScaleMeter) *XPLMMapScaleMeter_ptr;
extern typeof(XPLMMapUnproject) *XPLMMapUnproject_ptr;
extern typeof(XPLMPlaceUserAtLocation) *XPLMPlaceUserAtLocation_ptr;
extern typeof(XPLMRegisterMapCreationHook) *XPLMRegisterMapCreationHook_ptr;
extern typeof(XPLMSetWindowGeometryOS) *XPLMSetWindowGeometryOS_ptr;
extern typeof(XPLMSetWindowGravity) *XPLMSetWindowGravity_ptr;
extern typeof(XPLMSetWindowPositioningMode) *XPLMSetWindowPositioningMode_ptr;
extern typeof(XPLMSetWindowResizingLimits) *XPLMSetWindowResizingLimits_ptr;
extern typeof(XPLMSetWindowTitle) *XPLMSetWindowTitle_ptr;
extern typeof(XPLMWindowIsPoppedOut) *XPLMWindowIsPoppedOut_ptr;


bool loadAllFunctions(void);

#endif
