#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sstream>
#define XPLM200
#define XPLM210
#define XPLM300
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMPlanes.h>

#include "chk_helper.h"

static std::string str0;
static std::string str1;
static int int0;
static int int1;
static float inXVal;
static float inYVal;
static float inZVal;
static float inPitchVal;
static float inRollVal;
static float inYawVal;
static int structSize;
static float gearPosition;
static float flapRatio;
static float spoilerRatio;
static float speedBrakeRatio;
static float slatRatio;
static float wingSweep;
static float thrust;
static float yokePitch;
static float yokeHeading;
static float yokeRoll;
static double latitude, longitude;
static std::list<XPLMDataRef> d;


void initPlanesModule()
{
  d.push_back(registerROAccessor("planes/Str0", str0));
  d.push_back(registerROAccessor("planes/Str1", str1));
  d.push_back(registerROAccessor("planes/Int0", int0));
  d.push_back(registerROAccessor("planes/Int1", int1));
  d.push_back(registerROAccessor("planes/inXVal", inXVal));
  d.push_back(registerROAccessor("planes/inYVal", inYVal));
  d.push_back(registerROAccessor("planes/inZVal", inZVal));
  d.push_back(registerROAccessor("planes/inPitchVal", inPitchVal));
  d.push_back(registerROAccessor("planes/inRollVal", inRollVal));
  d.push_back(registerROAccessor("planes/inYawVal", inYawVal));
  d.push_back(registerROAccessor("planes/structSize", structSize));
  d.push_back(registerROAccessor("planes/gearPosition", gearPosition));
  d.push_back(registerROAccessor("planes/flapRatio", flapRatio));
  d.push_back(registerROAccessor("planes/spoilerRatio", spoilerRatio));
  d.push_back(registerROAccessor("planes/speedBrakeRatio", speedBrakeRatio));
  d.push_back(registerROAccessor("planes/slatRatio", slatRatio));
  d.push_back(registerROAccessor("planes/wingSweep", wingSweep));
  d.push_back(registerROAccessor("planes/thrust", thrust));
  d.push_back(registerROAccessor("planes/yokePitch", yokePitch));
  d.push_back(registerROAccessor("planes/yokeHeading", yokeHeading));
  d.push_back(registerROAccessor("planes/yokeRoll", yokeRoll));
  d.push_back(registerROAccessor("planes/latitude", latitude));
  d.push_back(registerROAccessor("planes/longitude", longitude));
}

void cleanupPlanesModule()
{
  for(std::list<XPLMDataRef>::iterator i = d.begin(); i != d.end(); ++i){
    XPLMUnregisterDataAccessor(*i);
  }
  d.empty();
}

void XPLMSetUsersAircraft(const char *inAircraftPath)
{
  str0 = inAircraftPath;
}

void XPLMPlaceUserAtAirport(const char *inAirportCode)
{
  str1 = inAirportCode;
}

void XPLMPlaceUserAtLocation(double latitudeDegrees, double longitudeDegrees, 
                             float elevationMetersMSL, float headingDegreesTrue, float speedMetersPerSecond)
{
  latitude = latitudeDegrees;
  longitude = longitudeDegrees;
  inXVal = elevationMetersMSL;
  inYVal = headingDegreesTrue;
  inZVal = speedMetersPerSecond;
}

void XPLMCountAircraft(int *outTotalAircraft, int *outActiveAircraft, XPLMPluginID *outController)
{
  *outTotalAircraft = int0 + 42;
  *outActiveAircraft = int0 + 43;
  *outController = int0 + 44;
}

void XPLMGetNthAircraftModel(int inIndex, char *outFileName, char *outPath)
{
  int0 = inIndex;
  strncpy(outFileName, str0.c_str(), 255);
  outFileName[255] = '\0';
  strncpy(outPath, str1.c_str(), 511);
  outPath[511] = '\0';
}


int XPLMAcquirePlanes(char **inAircraft, XPLMPlanesAvailable_f inCallback, void *inRefcon)
{
  std::string all;
  int res = 0;
  while(*inAircraft != NULL){
    all += *inAircraft;
    all += ",";
    ++inAircraft;
    ++res;
  }
  str0 = all;
  inCallback(inRefcon);
  return res;
}

void XPLMReleasePlanes(void)
{
  int0 = -1;
}

void XPLMSetActiveAircraftCount(int inCount)
{
  int0 = inCount;
}

void XPLMSetAircraftModel(int inIndex, const char *inAircraftPath)
{
  int0 = inIndex;
  str0 = inAircraftPath;
}

void XPLMDisableAIForPlane(int inPlaneIndex)
{
  int0 = inPlaneIndex;
}

void XPLMDrawAircraft(int inPlaneIndex, float inX, float inY, float inZ, float inPitch, float inRoll, float inYaw,
                      int inFullDraw, XPLMPlaneDrawState_t *inDrawStateInfo)
{
  int0 = inPlaneIndex;
  inXVal = inX;
  inYVal = inY;
  inZVal = inZ;
  inPitchVal = inPitch;
  inRollVal = inRoll;
  inYawVal = inYaw;
  int1 = inFullDraw;
  structSize = inDrawStateInfo->structSize;
  gearPosition = inDrawStateInfo->gearPosition;
  flapRatio = inDrawStateInfo->flapRatio;
  spoilerRatio = inDrawStateInfo->spoilerRatio;
  speedBrakeRatio = inDrawStateInfo->speedBrakeRatio;
  slatRatio = inDrawStateInfo->slatRatio;
  wingSweep = inDrawStateInfo->wingSweep;
  thrust = inDrawStateInfo->thrust;
  yokePitch = inDrawStateInfo->yokePitch;
  yokeHeading = inDrawStateInfo->yokeHeading;
  yokeRoll = inDrawStateInfo->yokeRoll;
}

void XPLMReinitUsersPlane(void)
{
  int0 = -2;
}

