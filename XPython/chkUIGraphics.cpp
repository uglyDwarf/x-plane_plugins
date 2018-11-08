#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPUIGraphics.h>

#include "chk_helper.h"


static int int0, int1, int2, int3, int4, int5, int6, int7, int8;
static std::list<XPLMDataRef> d;

void initUIGraphicsModule()
{
  d.push_back(registerROAccessor("uiGraphics/int0", int0));
  d.push_back(registerROAccessor("uiGraphics/int1", int1));
  d.push_back(registerROAccessor("uiGraphics/int2", int2));
  d.push_back(registerROAccessor("uiGraphics/int3", int3));
  d.push_back(registerROAccessor("uiGraphics/int4", int4));
  d.push_back(registerROAccessor("uiGraphics/int5", int5));
  d.push_back(registerROAccessor("uiGraphics/int6", int6));
  d.push_back(registerROAccessor("uiGraphics/int7", int7));
  d.push_back(registerROAccessor("uiGraphics/int8", int8));
}

void cleanupUIGraphicsModule()
{
  for(std::list<XPLMDataRef>::iterator i = d.begin(); i != d.end(); ++i){
    XPLMUnregisterDataAccessor(*i);
  }
  d.empty();
}

void XPDrawWindow(int inX1, int inY1, int inX2, int inY2, XPWindowStyle inStyle)
{
  int0 = inX1;
  int1 = inY1;
  int2 = inX2;
  int3 = inY2;
  int4 = inStyle;
}

void XPGetWindowDefaultDimensions(XPWindowStyle inStyle, int *outWidth, int *outHeight)
{
  *outWidth = int2 - int0;
  *outHeight = int3 - int1;
  int0 = inStyle;
}

void XPDrawElement(int inX1, int inY1, int inX2, int inY2, XPElementStyle inStyle, int inLit)
{
  int0 = inX1;
  int1 = inY1;
  int2 = inX2;
  int3 = inY2;
  int4 = inStyle;
  int5 = inLit;
}

void XPGetElementDefaultDimensions(XPElementStyle inStyle, int *outWidth, int *outHeight, int*outCanBeLit)
{
  *outWidth = int2 - int0;
  *outHeight = int3 - int1;
  *outCanBeLit = int5;
  int0 = inStyle;
}

void XPDrawTrack(int inX1, int inY1, int inX2, int inY2, int inMin, int inMax, int inValue, 
                 XPTrackStyle inStyle, int inLit)
{
  int0 = inX1;
  int1 = inY1;
  int2 = inX2;
  int3 = inY2;
  int4 = inMin;
  int5 = inMax;
  int6 = inValue;
  int7 = inStyle;
  int8 = inLit;
}

void XPGetTrackDefaultDimensions(XPTrackStyle inStyle, int *outWidth, int *outCanBeLit)
{
  *outWidth = int2 - int0;
  *outCanBeLit = int8;
  int0 = inStyle;
}

void XPGetTrackMetrics(int inX1, int inY1, int inX2, int inY2, int inMin, int inMax, int inValue,
                       XPTrackStyle inTrackStyle, int *outIsVertical, int *outDownBtnSize,
                       int *outDownPageSize, int *outThumbSize, int *outUpPageSize, int *outUpBtnSize)
{
  *outIsVertical = inX1;
  *outDownBtnSize = inY1;
  *outDownPageSize = inX2;
  *outThumbSize = inY2;
  *outUpPageSize = inMin;
  *outUpBtnSize = inMax;
  int0 = inValue;
  int1 = inTrackStyle;
}



