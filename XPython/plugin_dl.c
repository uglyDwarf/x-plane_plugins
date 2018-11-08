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
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct{
  const char *name;
  void **fun_ptr;
} t_fcn_info;

// XPLM210 functions
typeof(XPLMCreateFlightLoop) *XPLMCreateFlightLoop_ptr = NULL;
typeof(XPLMDestroyFlightLoop) *XPLMDestroyFlightLoop_ptr = NULL;
typeof(XPLMLoadObjectAsync) *XPLMLoadObjectAsync_ptr = NULL;
typeof(XPLMRemoveMenuItem) *XPLMRemoveMenuItem_ptr = NULL;
typeof(XPLMScheduleFlightLoop) *XPLMScheduleFlightLoop_ptr = NULL;

t_fcn_info funcs210[] = {
  {"XPLMCreateFlightLoop", (void *)&XPLMCreateFlightLoop_ptr},
  {"XPLMDestroyFlightLoop", (void *)&XPLMDestroyFlightLoop_ptr},
  {"XPLMLoadObjectAsync", (void *)&XPLMLoadObjectAsync_ptr},
  {"XPLMRemoveMenuItem", (void *)&XPLMRemoveMenuItem_ptr},
  {"XPLMScheduleFlightLoop", (void *)&XPLMScheduleFlightLoop_ptr},
  {NULL, NULL}
};

// XPLM300 functions
typeof(XPLMAppendMenuItemWithCommand) *XPLMAppendMenuItemWithCommand_ptr = NULL;
typeof(XPLMCreateMapLayer) *XPLMCreateMapLayer_ptr = NULL;
typeof(XPLMDegMagneticToDegTrue) *XPLMDegMagneticToDegTrue_ptr = NULL;
typeof(XPLMDegTrueToDegMagnetic) *XPLMDegTrueToDegMagnetic_ptr = NULL;
typeof(XPLMDestroyMapLayer) *XPLMDestroyMapLayer_ptr = NULL;
typeof(XPLMDrawMapIconFromSheet) *XPLMDrawMapIconFromSheet_ptr = NULL;
typeof(XPLMDrawMapLabel) *XPLMDrawMapLabel_ptr = NULL;
typeof(XPLMFindAircraftMenu) *XPLMFindAircraftMenu_ptr = NULL;
typeof(XPLMGetAllMonitorBoundsGlobal) *XPLMGetAllMonitorBoundsGlobal_ptr = NULL;
typeof(XPLMGetAllMonitorBoundsOS) *XPLMGetAllMonitorBoundsOS_ptr = NULL;
typeof(XPLMGetMagneticVariation) *XPLMGetMagneticVariation_ptr = NULL;
typeof(XPLMGetMouseLocationGlobal) *XPLMGetMouseLocationGlobal_ptr = NULL;
typeof(XPLMGetScreenBoundsGlobal) *XPLMGetScreenBoundsGlobal_ptr = NULL;
typeof(XPLMGetWindowGeometryOS) *XPLMGetWindowGeometryOS_ptr = NULL;
typeof(XPLMMapExists) *XPLMMapExists_ptr = NULL;
typeof(XPLMMapGetNorthHeading) *XPLMMapGetNorthHeading_ptr = NULL;
typeof(XPLMMapProject) *XPLMMapProject_ptr = NULL;
typeof(XPLMMapScaleMeter) *XPLMMapScaleMeter_ptr = NULL;
typeof(XPLMMapUnproject) *XPLMMapUnproject_ptr = NULL;
typeof(XPLMPlaceUserAtLocation) *XPLMPlaceUserAtLocation_ptr = NULL;
typeof(XPLMRegisterMapCreationHook) *XPLMRegisterMapCreationHook_ptr = NULL;
typeof(XPLMSetWindowGeometryOS) *XPLMSetWindowGeometryOS_ptr = NULL;
typeof(XPLMSetWindowGravity) *XPLMSetWindowGravity_ptr = NULL;
typeof(XPLMSetWindowPositioningMode) *XPLMSetWindowPositioningMode_ptr = NULL;
typeof(XPLMSetWindowResizingLimits) *XPLMSetWindowResizingLimits_ptr = NULL;
typeof(XPLMSetWindowTitle) *XPLMSetWindowTitle_ptr = NULL;
typeof(XPLMWindowIsPoppedOut) *XPLMWindowIsPoppedOut_ptr = NULL;

t_fcn_info funcs300[] = {
  {"XPLMAppendMenuItemWithCommand", (void *)&XPLMAppendMenuItemWithCommand_ptr},
  {"XPLMCreateMapLayer", (void *)&XPLMCreateMapLayer_ptr},
  {"XPLMDegMagneticToDegTrue", (void *)&XPLMDegMagneticToDegTrue_ptr},
  {"XPLMDegTrueToDegMagnetic", (void *)&XPLMDegTrueToDegMagnetic_ptr},
  {"XPLMDestroyMapLayer", (void *)&XPLMDestroyMapLayer_ptr},
  {"XPLMDrawMapIconFromSheet", (void *)&XPLMDrawMapIconFromSheet_ptr},
  {"XPLMDrawMapLabel", (void *)&XPLMDrawMapLabel_ptr},
  {"XPLMFindAircraftMenu", (void *)&XPLMFindAircraftMenu_ptr},
  {"XPLMGetAllMonitorBoundsGlobal", (void *)&XPLMGetAllMonitorBoundsGlobal_ptr},
  {"XPLMGetAllMonitorBoundsOS", (void *)&XPLMGetAllMonitorBoundsOS_ptr},
  {"XPLMGetMagneticVariation", (void *)&XPLMGetMagneticVariation_ptr},
  {"XPLMGetMouseLocationGlobal", (void *)&XPLMGetMouseLocationGlobal_ptr},
  {"XPLMGetScreenBoundsGlobal", (void *)&XPLMGetScreenBoundsGlobal_ptr},
  {"XPLMGetWindowGeometryOS", (void *)&XPLMGetWindowGeometryOS_ptr},
  {"XPLMMapExists", (void *)&XPLMMapExists_ptr},
  {"XPLMMapGetNorthHeading", (void *)&XPLMMapGetNorthHeading_ptr},
  {"XPLMMapProject", (void *)&XPLMMapProject_ptr},
  {"XPLMMapScaleMeter", (void *)&XPLMMapScaleMeter_ptr},
  {"XPLMMapUnproject", (void *)&XPLMMapUnproject_ptr},
  {"XPLMPlaceUserAtLocation", (void *)&XPLMPlaceUserAtLocation_ptr},
  {"XPLMRegisterMapCreationHook", (void *)&XPLMRegisterMapCreationHook_ptr},
  {"XPLMSetWindowGeometryOS", (void *)&XPLMSetWindowGeometryOS_ptr},
  {"XPLMSetWindowGravity", (void *)&XPLMSetWindowGravity_ptr},
  {"XPLMSetWindowPositioningMode", (void *)&XPLMSetWindowPositioningMode_ptr},
  {"XPLMSetWindowResizingLimits", (void *)&XPLMSetWindowResizingLimits_ptr},
  {"XPLMSetWindowTitle", (void *)&XPLMSetWindowTitle_ptr},
  {"XPLMWindowIsPoppedOut", (void *)&XPLMWindowIsPoppedOut_ptr},
  {NULL, NULL}
};



bool loadFunctions(t_fcn_info *ptr)
{
  void *fun_ptr;
  void *handle;
  bool res = true;

  handle = dlopen(NULL, RTLD_NOW);
  if(handle == NULL){
    fprintf(stderr, "Problem dlopening executable.\n");
    return false;
  }
  while(ptr->name != NULL){
    fun_ptr = dlsym(handle, ptr->name);
    if(fun_ptr != NULL){
      *(ptr->fun_ptr) = fun_ptr;
    }else{
      fprintf(stderr, "Couldn't get address of function '%s'.", ptr->name);
      res = false;
    }
    ++ptr;
  }
  return res;
}

bool loadAllFunctions(void)
{
  int xp_ver, xplm_ver;
  XPLMHostApplicationID app;
  XPLMGetVersions(&xp_ver, &xplm_ver, &app);

  bool res = true;
  if(xplm_ver >= 300){
    res &= loadFunctions(funcs300);
  }
  if(xplm_ver >= 210){
    res &= loadFunctions(funcs210);
  }
  return res;
}

