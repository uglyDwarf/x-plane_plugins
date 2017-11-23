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


void initPlanesModule()
{
  registerROAccessor("planes/Str0", str0);
  registerROAccessor("planes/Str1", str1);
  registerROAccessor("planes/Int0", int0);
  registerROAccessor("planes/Int1", int1);
  registerROAccessor("planes/inXVal", inXVal);
  registerROAccessor("planes/inYVal", inYVal);
  registerROAccessor("planes/inZVal", inZVal);
  registerROAccessor("planes/inPitchVal", inPitchVal);
  registerROAccessor("planes/inRollVal", inRollVal);
  registerROAccessor("planes/inYawVal", inYawVal);
  registerROAccessor("planes/structSize", structSize);
  registerROAccessor("planes/gearPosition", gearPosition);
  registerROAccessor("planes/flapRatio", flapRatio);
  registerROAccessor("planes/spoilerRatio", spoilerRatio);
  registerROAccessor("planes/speedBrakeRatio", speedBrakeRatio);
  registerROAccessor("planes/slatRatio", slatRatio);
  registerROAccessor("planes/wingSweep", wingSweep);
  registerROAccessor("planes/thrust", thrust);
  registerROAccessor("planes/yokePitch", yokePitch);
  registerROAccessor("planes/yokeHeading", yokeHeading);
  registerROAccessor("planes/yokeRoll", yokeRoll);
}

void XPLMSetUsersAircraft(const char *inAircraftPath)
{
  str0 = inAircraftPath;
}

void XPLMPlaceUserAtAirport(const char *inAirportCode)
{
  str1 = inAirportCode;
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

