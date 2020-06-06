#ifndef PLUGIN_DL__H
#define PLUGIN_DL__H

#include <sys/time.h>
#include <stdbool.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMScenery.h>
#include <XPLM/XPLMMenus.h>
#include <XPLM/XPLMInstance.h>
#include <XPLM/XPLMMap.h>
#include <XPLM/XPLMDisplay.h>
#include <XPLM/XPLMPlanes.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPWidgets.h>

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
extern typeof(XPLMHasKeyboardFocus) *XPLMHasKeyboardFocus_ptr;
extern typeof(XPLMCreateInstance) *XPLMCreateInstance_ptr;
extern typeof(XPLMDestroyInstance) *XPLMDestroyInstance_ptr;
extern typeof(XPLMInstanceSetPosition) *XPLMInstanceSetPosition_ptr;
//XPLM_301
extern typeof(XPGetWidgetUnderlyingWindow) *XPGetWidgetUnderlyingWindow_ptr;
extern typeof(XPLMGetWindowGeometryVR) *XPLMGetWindowGeometryVR_ptr;
extern typeof(XPLMSetWindowGeometryVR) *XPLMSetWindowGeometryVR_ptr;
extern typeof(XPLMWindowIsInVR) *XPLMWindowIsInVR_ptr;


bool loadAllFunctions(void);

#endif
