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
#include <XPLM/XPLMProcessing.h>

#include "chk_helper.h"

class callback;

static std::map<void *, class callback *> callbacks;


class callback{
  XPLMFlightLoop_f cbk;
  void *refcon;
  float interval;
 public:
  callback(XPLMFlightLoop_f loopCallback, void *ref):cbk(loopCallback), refcon(ref), interval(-1.0){};
  bool check(XPLMFlightLoop_f loopCallback, void *ref)
    {if(cbk == loopCallback && ref == refcon) return true; else return false;};
  float call(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter)
  {return cbk(inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter, refcon);}
};



static int int0, int1;
static float flt0, flt1, flt2;
static callback *id;

void initProcessingModule()
{
  registerROAccessor("processing/int0", int0);
  registerROAccessor("processing/int1", int1);
  registerROAccessor("processing/flt0", flt0);
  registerROAccessor("processing/flt1", flt1);
  registerROAccessor("processing/flt2", flt2);
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
  callbacks[inRefcon] = new callback(inFlightLoop, inRefcon);
  flt0 = 1000 * inInterval;
  flt1 = 2 * inInterval;
  int0 = rand();
  flt2 += callbacks[inRefcon]->call(flt0, flt1, int0);
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

XPLMFlightLoopID XPLMCreateFlightLoop(XPLMCreateFlightLoop_t *inParams)
{
  if(inParams->structSize != sizeof(XPLMCreateFlightLoop_t)){
    return NULL;
  }
  id = new callback(inParams->callbackFunc, inParams->refcon);
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

