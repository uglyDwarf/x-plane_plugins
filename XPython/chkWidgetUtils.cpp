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
#include <Widgets/XPWidgets.h>
#include <Widgets/XPWidgetUtils.h>

#include "chk_helper.h"


static int int0, int1, int2, int3, int4, int5, int6;
static std::list<XPLMDataRef> d;


void initWidgetUtilsModule()
{
  d.push_back(registerROAccessor("widgetUtils/int0", int0));
  d.push_back(registerROAccessor("widgetUtils/int1", int1));
  d.push_back(registerROAccessor("widgetUtils/int2", int2));
  d.push_back(registerROAccessor("widgetUtils/int3", int3));
  d.push_back(registerROAccessor("widgetUtils/int4", int4));
  d.push_back(registerROAccessor("widgetUtils/int5", int5));
  d.push_back(registerROAccessor("widgetUtils/int6", int6));
}

void cleanupWidgetUtilsModule()
{
  for(std::list<XPLMDataRef>::iterator i = d.begin(); i != d.end(); ++i){
    XPLMUnregisterDataAccessor(*i);
  }
  d.empty();
}

void XPUCreateWidgets(const XPWidgetCreate_t *inWidgetDefs, int inCount, XPWidgetID inParamParent, XPWidgetID *ioWidgets)
{
  int i;
  for(i = 0; i < inCount; ++i){
    ioWidgets[i] = NULL;
  }
  XPWidgetID container;
  const XPWidgetCreate_t *defs;
  for(i = 0; i < inCount; ++i){
    defs = &(inWidgetDefs[i]);
    switch(defs->containerIndex){
      case NO_PARENT:
        container = NULL;
        break;
      case PARAM_PARENT:
        container = inParamParent;
        break;
      default:
        if((0 <= defs->containerIndex) && (defs->containerIndex < i)){
          container = ioWidgets[defs->containerIndex];
        }else{
          container = NULL;
        }
        break;
    }
    ioWidgets[i] = XPCreateWidget(defs->left, defs->top, defs->right, defs->bottom,
                                  defs->visible, defs->descriptor, defs->isRoot, container,
                                  defs->widgetClass);
  }
}


void XPUMoveWidgetBy(XPWidgetID inWidget, int inDeltaX, int inDeltaY)
{
  int left, top, right, bottom;
  XPGetWidgetGeometry(inWidget, &left, &top, &right, &bottom);
  left += inDeltaX;
  right += inDeltaX;
  top += inDeltaY;
  bottom += inDeltaY;
  XPSetWidgetGeometry(inWidget, left, top, right, bottom);
}

int XPUFixedLayout(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
  int0 = inMessage;
  int1 = inParam1;
  int2 = inParam2;
  return XPIsWidgetVisible(inWidget) + inMessage;
}


int XPUSelectIfNeeded(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2, int inEatClick)
{
  int0 = inEatClick;
  int1 = inMessage;
  int2 = inParam1;
  int3 = inParam2;
  return XPIsWidgetVisible(inWidget) + inParam1;
}

int XPUDefocusKeyboard(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2, int inEatClick)
{
  int0 = inMessage;
  int1 = inEatClick;
  int2 = inParam1;
  int3 = inParam2;
  return XPIsWidgetVisible(inWidget) + inParam2;
}


int XPUDragWidget(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2,
                  int inLeft, int inTop, int inRight, int inBottom)
{
  int0 = inMessage;
  int1 = inParam1;
  int2 = inParam2;
  int3 = inLeft;
  int4 = inTop;
  int5 = inRight;
  int6 = inBottom;
  return XPIsWidgetVisible(inWidget) + inLeft;
}



