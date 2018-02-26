#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMNavigation.h>

#include "chk_helper.h"

static int navAid;
static std::string name;
static std::string id;
static float lat;
static float lon;
static int freq;
static int type;
static int fmsEntry;
static int destFmsEntry;
static int ref;
static int altitude;

void initNavigationModule()
{
}

void cleanupNavigationModule()
{
}


XPLMNavRef XPLMGetFirstNavAid(void)
{
  return static_cast<XPLMNavRef>(333);
}

XPLMNavRef XPLMGetNextNavAid(XPLMNavRef inNavAid)
{
  navAid = inNavAid;
  return navAid + 1;
}

XPLMNavRef XPLMFindFirstNavAidOfType(XPLMNavType inType)
{
  type = inType;
  return inType + 480;
}

XPLMNavRef XPLMFindLastNavAidOfType(XPLMNavType inType)
{
  type = inType;
  return inType + 490;
}

XPLMNavRef XPLMFindNavAid(const char *inNameFragment, const char *inIDFragment, float *inLat, float *inLon,
                          int *inFrequency, XPLMNavType inType)
{
  name = inNameFragment;
  id = inIDFragment;
  lat = *inLat;
  lon = *inLon;
  freq = *inFrequency;
 
  return inType + 500;
}

void XPLMGetNavAidInfo(XPLMNavRef inRef, XPLMNavType *outType, float *outLatitude, float *outLongitude, float *outHeight,
                       int *outFrequency, float *outHeading, char *outID, char *outName, char *outReg)
{
  *outType = type;
  *outLatitude = lat;
  *outLongitude = lon;
  *outHeight = inRef;
  *outFrequency = freq;
  *outHeading = lat * lon;
  strcpy(outID, id.c_str());
  strcpy(outName, name.c_str());
  strcpy(outReg, "LKTB");
}

int XPLMCountFMSEntries(void)
{
  return fmsEntry + 2;
}

int XPLMGetDisplayedFMSEntry(void)
{
  return fmsEntry;
}

int XPLMGetDestinationFMSEntry(void)
{
  return destFmsEntry;
}

void XPLMSetDisplayedFMSEntry(int inIndex)
{
  fmsEntry = inIndex;
}

void XPLMSetDestinationFMSEntry(int inIndex)
{
  destFmsEntry = inIndex;
}

void XPLMGetFMSEntryInfo(int inIndex, XPLMNavType *outType, char *outID, XPLMNavRef *outRef, int *outAltitude,
                         float *outLat, float *outLon)
{
  *outType = inIndex;
  strcpy(outID, id.c_str());
  *outRef = ref;
  *outAltitude = altitude;
  *outLat = lat;
  *outLon = lon;
}

void XPLMSetFMSEntryInfo(int inIndex, XPLMNavRef inRef, int inAltitude)
{
  fmsEntry = inIndex;
  ref = inRef;
  altitude = inAltitude;
}

void XPLMSetFMSEntryLatLon(int inIndex, float inLat, float inLon, int inAltitude)
{
  fmsEntry = inIndex;
  lat = inLat;
  lon = inLon;
  altitude = inAltitude;
}

void XPLMClearFMSEntry(int inIndex)
{
  fmsEntry = inIndex;
}

XPLMNavType XPLMGetGPSDestinationType(void)
{
  return type;
}

XPLMNavRef XPLMGetGPSDestination(void)
{
  return ref;
}

