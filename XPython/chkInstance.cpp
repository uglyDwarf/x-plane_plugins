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
#include <XPLM/XPLMInstance.h>

#include "chk_helper.h"

static std::string str0;
static int int0;
static float x, y, z, pitch, heading, roll, float0;

static std::list<XPLMDataRef> d;

void initInstanceModule()
{
  d.push_back(registerROAccessor("instance.str", str0));
  d.push_back(registerROAccessor("instance.int0", int0));
  d.push_back(registerROAccessor("instance.float0", float0));
  d.push_back(registerROAccessor("instance.x", x));
  d.push_back(registerROAccessor("instance.y", y));
  d.push_back(registerROAccessor("instance.z", z));
  d.push_back(registerROAccessor("instance.pitch", pitch));
  d.push_back(registerROAccessor("instance.heading", heading));
  d.push_back(registerROAccessor("instance.roll", roll));
}

void cleanupInstanceModule()
{
  for(std::list<XPLMDataRef>::iterator i = d.begin(); i != d.end(); ++i){
    XPLMUnregisterDataAccessor(*i);
  }
  d.empty();
}

#if defined(XPLM300)
static int inst;
XPLMInstanceRef XPLMCreateInstance(XPLMObjectRef obj, const char **datarefs)
{
  int0 = (intptr_t)obj;
  str0 = "";
  const char **ptr = datarefs;
  inst = 0;
  while(*ptr != NULL){
    str0 += *ptr;
    ++ptr;
    ++inst;
  }
  return &inst;
}

void XPLMDestroyInstance(XPLMInstanceRef instance)
{
  assert(instance == &inst);
  int0 = 365;
}

void XPLMInstanceSetPosition(XPLMInstanceRef instance, const XPLMDrawInfo_t *new_position, const float *data)
{
  assert(instance == &inst);
  assert(new_position->structSize == sizeof(XPLMDrawInfo_t));
  x = new_position->x;
  y = new_position->y;
  z = new_position->z;
  pitch = new_position->pitch;
  heading = new_position->heading;
  roll = new_position->roll;
  float0 = 0.0f;
  for(int i = 0; i < *(int *)instance; ++i){
    float0 += data[i];
  }
}
#endif

