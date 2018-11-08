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
#include <XPLM/XPLMProcessing.h>

#include "chk_helper.h"

class callback;

static std::map<void *, class callback *> callbacks;


class callback{
  XPLMFlightLoop_f cbk;
  void *refcon;
  float interval;
 public:
  callback(XPLMFlightLoop_f loopCallback, void *ref, float _interval):cbk(loopCallback), refcon(ref), interval(_interval){};
  bool check(XPLMFlightLoop_f loopCallback, void *ref)
    {if(cbk == loopCallback && ref == refcon) return true; else return false;};
  float call(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter)
    {return cbk(inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop + interval, inCounter, refcon);}
};



static int int0, int1;
static float flt0, flt1, flt2;
#if defined(XPLM210)
static callback *id;
#endif
static std::list<XPLMDataRef> d;

void initProcessingModule()
{
  d.push_back(registerROAccessor("processing/int0", int0));
  d.push_back(registerROAccessor("processing/int1", int1));
  d.push_back(registerROAccessor("processing/flt0", flt0));
  d.push_back(registerROAccessor("processing/flt1", flt1));
  d.push_back(registerROAccessor("processing/flt2", flt2));
}

void cleanupProcessingModule()
{
  for(std::list<XPLMDataRef>::iterator i = d.begin(); i != d.end(); ++i){
    XPLMUnregisterDataAccessor(*i);
  }
  d.empty();
}

float XPLMGetElapsedTime()
{
  flt0 = rand() / 10.0;
  return flt0;
}

int XPLMGetCycleNumber()
{
  int0 = rand();
  return int0;
}

void XPLMRegisterFlightLoopCallback(XPLMFlightLoop_f inFlightLoop, float inInterval, void *inRefcon)
{
  callback *cbk = new callback(inFlightLoop, inRefcon, inInterval);
  callbacks[inRefcon] = cbk;
  flt0 = 1000 * inInterval;
  flt1 = 2 * inInterval;
  int0 = rand();
  flt2 += cbk->call(flt0, flt1, int0);
}

void XPLMUnregisterFlightLoopCallback(XPLMFlightLoop_f inFlightLoop, void *inRefcon)
{
  callback *cbk = callbacks[inRefcon];
  if(cbk == NULL){
    int0 = -1;
    return;
  }
  int0 = cbk->check(inFlightLoop, inRefcon);
  callbacks.erase(inRefcon);
  delete cbk;
  return;
}

void XPLMSetFlightLoopCallbackInterval(XPLMFlightLoop_f inFlightLoop, float inInterval, int inRelativeToNow, void *inRefcon)
{
  callback *cbk = callbacks[inRefcon];
  if(cbk == NULL){
    int0 = -1;
    return;
  }
  int0 = cbk->check(inFlightLoop, inRefcon);
  flt0 = inInterval;
  int1 = inRelativeToNow;
}

#if defined(XPLM210)
XPLMFlightLoopID XPLMCreateFlightLoop(XPLMCreateFlightLoop_t *inParams)
{
  if(inParams->structSize != sizeof(XPLMCreateFlightLoop_t)){
    return NULL;
  }
  id = new callback(inParams->callbackFunc, inParams->refcon, -42);
  int0 = inParams->phase;
  return id;
}

void XPLMDestroyFlightLoop(XPLMFlightLoopID inFlightLoopID)
{
  assert(inFlightLoopID == id);
  delete id;
  id = NULL;
  int0 = 123456;
}

void XPLMScheduleFlightLoop(XPLMFlightLoopID inFlightLoopID, float inInterval, int inRelativeToNow)
{
  assert(inFlightLoopID == id);
  flt0 = inInterval;
  int0 = inRelativeToNow;
  flt2 += static_cast<callback*>(inFlightLoopID)->call(1000 * flt0, 2 * flt0, int0);
}
#endif

