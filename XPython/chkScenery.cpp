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
#include <XPLM/XPLMScenery.h>

#include "chk_helper.h"

static XPLMProbeInfo_t probe;
static std::string path;
static int lighting;
static int earth_relative;
static std::vector<XPLMDrawInfo_t> locations;

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

static void registerROAccessor(const char *name, std::vector<XPLMDrawInfo_t> &locations)
{
  XPLMRegisterDataAccessor(name, xplmType_FloatArray, 0, 
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
  registerROAccessor("probe.structSize", probe.structSize);
  registerROAccessor("probe.locationX", probe.locationX);
  registerROAccessor("probe.locationY", probe.locationY);
  registerROAccessor("probe.locationZ", probe.locationZ);
  registerROAccessor("probe.normalX", probe.normalX);
  registerROAccessor("probe.normalY", probe.normalY);
  registerROAccessor("probe.normalZ", probe.normalZ);
  registerROAccessor("probe.velocityX", probe.velocityX);
  registerROAccessor("probe.velocityY", probe.velocityY);
  registerROAccessor("probe.velocityZ", probe.velocityZ);
  registerROAccessor("probe.is_wet", probe.is_wet);

  registerROAccessor("obj.path", path);
  registerROAccessor("obj.lighting", lighting);
  registerROAccessor("obj.earth_relative", earth_relative);
  registerROAccessor("obj.locations", locations);

}


XPLMProbeRef XPLMCreateProbe(XPLMProbeType inProbeType)
{
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

XPLMObjectRef XPLMLoadObject(const char *inPath)
{
  path = inPath;
  return static_cast<XPLMObjectRef>(new std::string(inPath));
}

void XPLMLoadObjectAsync(const char *inPath, XPLMObjectLoaded_f inCallback, void *inRefcon)
{
  path = inPath;
  inCallback(new std::string(inPath), inRefcon);
}

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


