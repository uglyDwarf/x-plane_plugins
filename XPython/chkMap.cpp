#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <cassert>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMMap.h>

#include "chk_helper.h"

static std::string str0;
static int int0, int1, int2, int3, int4;
static float float0, float1, float2, float3;

#if defined(XPLM300)
static XPLMMapProjectionID commonProj;
#endif

static std::list<XPLMDataRef> d;

void initMapModule()
{
  d.push_back(registerROAccessor("map.str0", str0));
  d.push_back(registerROAccessor("map.int0", int0));
  d.push_back(registerROAccessor("map.int1", int1));
  d.push_back(registerROAccessor("map.int2", int2));
  d.push_back(registerROAccessor("map.int3", int3));
  d.push_back(registerROAccessor("map.int4", int4));
  d.push_back(registerROAccessor("map.float0", float0));
  d.push_back(registerROAccessor("map.float1", float1));
  d.push_back(registerROAccessor("map.float2", float2));
  d.push_back(registerROAccessor("map.float3", float3));
}

void cleanupMapModule()
{
  for(std::list<XPLMDataRef>::iterator i = d.begin(); i != d.end(); ++i){
    XPLMUnregisterDataAccessor(*i);
  }
  d.empty();
}


#if defined(XPLM300)
static XPLMCreateMapLayer_t layer;
static XPLMMapCreatedCallback_f mapCreated;
static void *mapCreatedRefcon;

XPLMMapLayerID XPLMCreateMapLayer(XPLMCreateMapLayer_t *inParams)
{
  layer = *inParams;
  int0 = inParams->structSize;
  int1 = inParams->layerType;
  str0 = inParams->layerName;
  if(mapCreated != NULL){
    mapCreated(inParams->mapToCreateLayerIn, mapCreatedRefcon);
  }
  return &layer;
}

int XPLMDestroyMapLayer(XPLMMapLayerID inLayer)
{
  if(inLayer != &layer){
    return 0;
  }
  layer.willBeDeletedCallback(inLayer, layer.refcon);
  layer.structSize = 0;
  return 65537;
}

void XPLMRegisterMapCreationHook(XPLMMapCreatedCallback_f callback, void *refcon)
{
  mapCreated = callback;
  mapCreatedRefcon = refcon;
}

int XPLMMapExists(const char *mapIdentifier)
{
  str0 = mapIdentifier;
  float bounds[4];
  bounds[0] = layer.showUiToggle + 1;
  bounds[1] = layer.showUiToggle + 2;
  bounds[2] = layer.showUiToggle + 3;
  bounds[3] = layer.showUiToggle + 4;
  commonProj = (void *)(intptr_t)(layer.layerType + 5);
  float rest = layer.layerType + 6;

  layer.prepCacheCallback(&layer, bounds, commonProj, layer.refcon);

  bounds[0] += 11; bounds[1] += 11; bounds[2] += 11; bounds[3] += 11; rest += 11;
  layer.drawCallback(&layer, bounds, rest, rest + 4, rest + 7, commonProj, layer.refcon);

  bounds[0] += 11; bounds[1] += 11; bounds[2] += 11; bounds[3] += 11; rest += 11;
  layer.iconCallback(&layer, bounds, rest, rest + 4, rest + 7, commonProj, layer.refcon);
  
  bounds[0] += 11; bounds[1] += 11; bounds[2] += 11; bounds[3] += 11; rest += 11;
  layer.labelCallback(&layer, bounds, rest, rest + 4, rest + 7, commonProj, layer.refcon);

  str0 = mapIdentifier; //Would be overwritten by the callbacks
  return 32769;
}

void XPLMDrawMapIconFromSheet(XPLMMapLayerID inLayer, const char *inPngPath, int s, int t,
                              int ds, int dt, float mapX, float mapY, XPLMMapOrientation orientation,
                              float rotationDegrees, float mapWidth)
{
  assert(inLayer == &layer);
  str0 = inPngPath;
  int0 = s;
  int1 = t;
  int2 = ds;
  int3 = dt;
  float0 = mapX;
  float1 = mapY;
  int4 = orientation;
  float2 = rotationDegrees;
  float3 = mapWidth;
}

void XPLMDrawMapLabel(XPLMMapLayerID inLayer, const char *inText, float mapX, float mapY,
                      XPLMMapOrientation orientation, float rotationDegrees)
{
  assert(inLayer == &layer);
  str0 = inText;
  float0 = mapX;
  float1 = mapY;
  int0 = orientation;
  float2 = rotationDegrees;
}

void XPLMMapProject(XPLMMapProjectionID projection, double latitude, double longitude, float *outX, float *outY)
{
  assert(projection == commonProj);
  *outX = latitude * 2.5;
  *outY = longitude * 3.6;
}

void XPLMMapUnproject(XPLMMapProjectionID projection, float mapX, float mapY, double *outLatitude, double *outLongitude)
{
  assert(projection == commonProj);
  *outLatitude = mapY * 4.7;
  *outLongitude = mapX * 5.8;
}

float XPLMMapScaleMeter(XPLMMapProjectionID projection, float mapX, float mapY)
{
  assert(projection == commonProj);
  return 2 * mapX - mapY;
}

float XPLMMapGetNorthHeading(XPLMMapProjectionID projection, float mapX, float mapY)
{
  assert(projection == commonProj);
  return 2 * mapY - mapX;
}
#endif


