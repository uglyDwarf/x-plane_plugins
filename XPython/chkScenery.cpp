#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMScenery.h>

#include "chk_helper.h"


#if defined(XPLM200)
static XPLMProbeInfo_t probe;
#endif
static std::string path;
static int lighting;
static int earth_relative;
static double lat;
static double lon;
static float hdg;
static std::vector<XPLMDrawInfo_t> locations;
static std::list<XPLMDataRef> d;

static int getVecData(void *inRefcon, float *outValues, int inOffset, int inMax)
{
  if(inMax < 7){
    return 0;
  }
  XPLMDrawInfo_t &di = static_cast<std::vector<XPLMDrawInfo_t>*>(inRefcon)->at(inOffset);
  outValues[0] = di.structSize;
  outValues[1] = di.x;
  outValues[2] = di.y;
  outValues[3] = di.z;
  outValues[4] = di.pitch;
  outValues[5] = di.heading;
  outValues[6] = di.roll;
  return 7;
}


static XPLMDataRef registerROAccessor(const char *name, std::vector<XPLMDrawInfo_t> &locations)
{
  return XPLMRegisterDataAccessor(name, xplmType_FloatArray, 0, 
    NULL, NULL, 
    NULL, NULL, 
    NULL, NULL, 
    NULL, NULL, 
    getVecData, NULL,
    NULL, NULL,
    &locations, NULL);
}



void initSceneryModule()
{
  d.push_back(registerROAccessor("probe.structSize", probe.structSize));
  d.push_back(registerROAccessor("probe.locationX", probe.locationX));
  d.push_back(registerROAccessor("probe.locationY", probe.locationY));
  d.push_back(registerROAccessor("probe.locationZ", probe.locationZ));
  d.push_back(registerROAccessor("probe.normalX", probe.normalX));
  d.push_back(registerROAccessor("probe.normalY", probe.normalY));
  d.push_back(registerROAccessor("probe.normalZ", probe.normalZ));
  d.push_back(registerROAccessor("probe.velocityX", probe.velocityX));
  d.push_back(registerROAccessor("probe.velocityY", probe.velocityY));
  d.push_back(registerROAccessor("probe.velocityZ", probe.velocityZ));
  d.push_back(registerROAccessor("probe.is_wet", probe.is_wet));

  d.push_back(registerROAccessor("obj.path", path));
  d.push_back(registerROAccessor("obj.lighting", lighting));
  d.push_back(registerROAccessor("obj.earth_relative", earth_relative));
  d.push_back(registerROAccessor("obj.locations", locations));
  d.push_back(registerROAccessor("obj.latitude", lat));
  d.push_back(registerROAccessor("obj.longitude", lon));
  d.push_back(registerROAccessor("obj.heading", hdg));
}

void cleanupSceneryModule()
{
  for(std::list<XPLMDataRef>::iterator i = d.begin(); i != d.end(); ++i){
    XPLMUnregisterDataAccessor(*i);
  }
  d.empty();
}


#if defined(XPLM200)
XPLMProbeRef XPLMCreateProbe(XPLMProbeType inProbeType)
{
  lighting = inProbeType;
  probe.structSize = sizeof(XPLMProbeInfo_t);
  probe.normalX = rand();
  probe.normalY = rand();
  probe.normalZ = rand();
  probe.velocityX = rand();
  probe.velocityY = rand();
  probe.velocityZ = rand();
  probe.is_wet = rand() & 1;
  return (void*)&probe;
}

void XPLMDestroyProbe(XPLMProbeRef inProbe)
{
  XPLMProbeInfo_t *probePtr = static_cast<XPLMProbeInfo_t *>(inProbe);
  if(probePtr != &probe){
    std::cout << "*** ERROR *** Probe address mismatch; got " << inProbe << ", while expecting " << &probe << "." << std::endl;
  }
  probe.structSize = 0;
}

XPLMProbeResult XPLMProbeTerrainXYZ(XPLMProbeRef inProbe, float inX, float inY, float inZ, XPLMProbeInfo_t *outInfo)
{
  
  outInfo->locationX = inX;
  outInfo->locationY = inY;
  outInfo->locationZ = inZ;

  XPLMProbeInfo_t *probe = static_cast<XPLMProbeInfo_t *>(inProbe);
  outInfo->structSize = sizeof(XPLMProbeInfo_t);
  outInfo->normalX = probe->normalX;
  outInfo->normalY = probe->normalY;
  outInfo->normalZ = probe->normalZ;
  outInfo->velocityX = probe->velocityX;
  outInfo->velocityY = probe->velocityY;
  outInfo->velocityZ = probe->velocityZ;
  outInfo->is_wet = probe->is_wet;
  return xplm_ProbeHitTerrain;
}
#endif

#if defined(XPLM300)
float XPLMGetMagneticVariation(double latitude,
                               double longitude)
{
  lat = latitude;
  lon = longitude;
  return lat + lon * lon;
}

float XPLMDegTrueToDegMagnetic(float headingDegreesTrue)
{
  hdg = headingDegreesTrue;
  return hdg * hdg * hdg;
}

float XPLMDegMagneticToDegTrue(float headingDegreesMagnetic)
{
  hdg = headingDegreesMagnetic;
  return 1 / hdg;
}
#endif

#if defined(XPLM200)
XPLMObjectRef XPLMLoadObject(const char *inPath)
{
  path = inPath;
  return static_cast<XPLMObjectRef>(new std::string(inPath));
}
#endif

#if defined(XPLM210)
void XPLMLoadObjectAsync(const char *inPath, XPLMObjectLoaded_f inCallback, void *inRefcon)
{
  path = inPath;
  inCallback(new std::string(inPath), inRefcon);
}
#endif

#if defined(XPLM200)
void XPLMDrawObjects(XPLMObjectRef inObject, int inCount, XPLMDrawInfo_t *inLocations, int inLighting, int inEarth_relative)
{
  path = *static_cast<std::string *>(inObject);
  locations.clear();
  for(int i = 0; i < inCount; ++i){
    locations.push_back(inLocations[i]);
  }
  lighting = inLighting;
  earth_relative = inEarth_relative;
}



void XPLMUnloadObject(XPLMObjectRef inObject)
{
  std::string *obj = static_cast<std::string *>(inObject);
  path = *obj + " deleted";
  delete obj;
}

int XPLMLookupObjects(const char *inPath, float inLatitude, float inLongitude, 
                      XPLMLibraryEnumerator_f enumerator, void *ref)
{
  path = inPath;
  probe.locationX = inLatitude;
  probe.locationY = inLongitude;
  enumerator((path + "1").c_str(), ref);
  enumerator((path + "2").c_str(), ref);
  return 2;
}
#endif


