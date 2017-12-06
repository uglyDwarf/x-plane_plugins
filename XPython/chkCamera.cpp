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
#include <XPLM/XPLMCamera.h>

#include "chk_helper.h"

static float x, y, z, pitch, heading, roll, zoom;
static int int0;
static int duration;

void initCameraModule()
{
  registerROAccessor("camera/int0", int0);
}

void XPLMControlCamera(XPLMCameraControlDuration inHowLong, XPLMCameraControl_f inControlFunc, void *inRefcon)
{
  XPLMCameraPosition_t pos;
  duration = inHowLong;
  int0 = inControlFunc(&pos, inHowLong, inRefcon);
  x = pos.x;
  y = pos.y;
  z = pos.z;
  pitch = pos.pitch;
  heading = pos.heading;
  roll = pos.roll;
  zoom = pos.zoom;
}

void XPLMDontControlCamera(void)
{
  int0 = -1;
}

int XPLMIsCameraBeingControlled(XPLMCameraControlDuration *outCameraControlDuration)
{
  *outCameraControlDuration = duration;
  int0 = rand();
  return int0;
}

void XPLMReadCameraPosition(XPLMCameraPosition_t *outCameraPosition)
{
  outCameraPosition->x = x;
  outCameraPosition->y = y;
  outCameraPosition->z = z;
  outCameraPosition->pitch = pitch;
  outCameraPosition->heading = heading;
  outCameraPosition->roll = roll;
  outCameraPosition->zoom = zoom;
}

