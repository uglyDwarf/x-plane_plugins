#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <string.h>

#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDisplay.h>

#include "chk_helper.h"

static std::string str0;
static int int0, int1, int2, int3;
static float f0, f1, f2, f3;
static std::list<XPLMDataRef> d;

void initDisplayModule(void)
{
  d.push_back(registerROAccessor("display/int0", int0));
  d.push_back(registerROAccessor("display/int1", int1));
  d.push_back(registerROAccessor("display/int2", int2));
  d.push_back(registerROAccessor("display/int3", int3));
  d.push_back(registerROAccessor("display/float0", f0));
  d.push_back(registerROAccessor("display/float1", f1));
  d.push_back(registerROAccessor("display/float2", f2));
  d.push_back(registerROAccessor("display/float3", f3));
  d.push_back(registerROAccessor("display/str0", str0));
}

void cleanupDisplayModule(void)
{
  for(std::list<XPLMDataRef>::iterator i = d.begin(); i != d.end(); ++i){
    XPLMUnregisterDataAccessor(*i);
  }
  d.empty();
}

class drawCallback{
  XPLMDrawCallback_f callback;
  XPLMDrawingPhase   phase;
  int                wantsBefore;
  void              *refcon;

 public:
  drawCallback(XPLMDrawCallback_f inCallback, XPLMDrawingPhase inPhase, int inWantsBefore, void *inRefcon);
  ~drawCallback();
  int call();
  bool is(XPLMDrawCallback_f inCallback, XPLMDrawingPhase inPhase, int inWantsBefore, void *inRefcon);
};

class keySniffCallback{
  XPLMKeySniffer_f callback;
  int              beforeWindows;
  void            *refcon;
 public:
  keySniffCallback(XPLMKeySniffer_f inCallback, int inBeforeWindows, void *inRefcon);
  ~keySniffCallback();
  int call(char inChar, XPLMKeyFlags inFlags, char inVirtualKey);
  bool is(XPLMKeySniffer_f inCallback, int inBeforeWindows, void *inRefcon);
};

std::vector<drawCallback *> drawCallbacks;
std::vector<keySniffCallback *> keySniffCallbacks;

drawCallback::drawCallback(XPLMDrawCallback_f inCallback, XPLMDrawingPhase inPhase, int inWantsBefore, void *inRefcon):
  callback(inCallback), phase(inPhase), wantsBefore(inWantsBefore), refcon(inRefcon)
{
}

drawCallback::~drawCallback()
{
}

int drawCallback::call()
{
  return callback(phase, wantsBefore, refcon);
}

bool drawCallback::is(XPLMDrawCallback_f inCallback, XPLMDrawingPhase inPhase, int inWantsBefore, void *inRefcon)
{
  if(inCallback != callback) return false;
  if(inPhase != phase) return false;
  if(inWantsBefore != wantsBefore) return false;
  if(inRefcon != refcon) return false;
  return true;
}

keySniffCallback::keySniffCallback(XPLMKeySniffer_f inCallback, int inBeforeWindows, void *inRefcon):
  callback(inCallback), beforeWindows(inBeforeWindows), refcon(inRefcon)
{
}

keySniffCallback::~keySniffCallback()
{
}

int keySniffCallback::call(char inChar, XPLMKeyFlags inFlags, char inVirtualKey)
{
  return callback(inChar, inFlags, inVirtualKey, refcon);
}

bool keySniffCallback::is(XPLMKeySniffer_f inCallback, int inBeforeWindows, void *inRefcon)
{
  if(inCallback != callback) return false;
  if(inBeforeWindows != beforeWindows) return false;
  if(inRefcon != refcon) return false;
  return true;
}


int XPLMRegisterDrawCallback(XPLMDrawCallback_f   inCallback,
                             XPLMDrawingPhase     inPhase,
                             int                  inWantsBefore,
                             void *               inRefcon)
{
  drawCallback *cbk = new drawCallback(inCallback, inPhase, inWantsBefore, inRefcon);
  drawCallbacks.push_back(cbk);
  return cbk->call();
}


int XPLMRegisterKeySniffer(XPLMKeySniffer_f inCallback,
                           int              inBeforeWindows,
                           void            *inRefcon)
{
  
  keySniffCallback *cbk = new keySniffCallback(inCallback, inBeforeWindows, inRefcon);
  keySniffCallbacks.push_back(cbk);
  return cbk->call('Q', xplm_ShiftFlag | xplm_DownFlag, XPLM_VK_Q );
}

int XPLMUnregisterDrawCallback(XPLMDrawCallback_f   inCallback,
                             XPLMDrawingPhase     inPhase,
                             int                  inWantsBefore,
                             void *               inRefcon)
{
  std::vector<drawCallback*>::iterator i;
  for(i = drawCallbacks.begin(); i != drawCallbacks.end(); ++i){
    if((*i)->is(inCallback, inPhase, inWantsBefore, inRefcon)){
      drawCallbacks.erase(i);
      delete *i;
      return 1;
    }
  }
  return 0;
}

int XPLMUnregisterKeySniffer(XPLMKeySniffer_f inCallback,
                           int              inBeforeWindows,
                           void            *inRefcon)
{
  std::vector<keySniffCallback *>::iterator i;
  for(i = keySniffCallbacks.begin(); i != keySniffCallbacks.end(); ++i){
    if((*i)->is(inCallback, inBeforeWindows, inRefcon)){
      keySniffCallbacks.erase(i);
      delete *i;
      return 1;
    }
  }
  return 0;
}

class windowInfo{
  int structSize, left, top, right, bottom, visible;
  XPLMDrawWindow_f drawWindowFunc;
  XPLMHandleMouseClick_f handleMouseClickFunc;
  XPLMHandleKey_f handleKeyFunc;
#if defined(XPLM200)
  XPLMHandleCursor_f handleCursorFunc;
  XPLMHandleMouseWheel_f handleMouseWheelFunc;
#endif
  void *refcon;
#if defined(XPLM300)
  int decorateAsFloatingWindow, layer; 
  XPLMHandleMouseClick_f    handleRightClickFunc;
#endif
  XPLMWindowID id;
  static intptr_t idCntr;
 public:
  windowInfo(
    int size,
    int l, int t, int r, int b, int vis,
    XPLMDrawWindow_f drawFunc,
    XPLMHandleMouseClick_f mouseClickFunc,
    XPLMHandleKey_f keyFunc,
#if defined(XPLM200)
    XPLMHandleCursor_f cursorFunc,
    XPLMHandleMouseWheel_f wheelFunc,
#endif
    void *rfc
#if defined(XPLM300)
    ,int dFW, int layer, 
    XPLMHandleMouseClick_f rightClickFunc
#endif
  );
  ~windowInfo();
  void draw();
  void handleKey(char inKey, XPLMKeyFlags inFlags, char inVirtualKey, int losingFocus);
  int handleMouseClick(int x, int y, XPLMMouseStatus inMouse);
#if defined(XPLM300)
  int handleRightClick(int x, int y, XPLMMouseStatus inMouse);
#endif
#if defined(XPLM200)
  XPLMCursorStatus handleCursor(int x, int y);
  int handleMouseWheel(int x, int y, int wheel, int clicks);
#endif
  bool isEx();
  XPLMWindowID getID();
  int getLeft()const{return left;};
  int getTop()const{return top;};
  int getRight()const{return right;};
  int getBottom()const{return bottom;};
  int getVisible()const{return visible;};
  void *getRefcon()const{return refcon;};
  void setLeft(int l){left = l;};
  void setTop(int t){top = t;};
  void setRight(int r){right = r;};
  void setBottom(int b){bottom = b;};
  void setVisible(int vis){visible = vis;};
  void setRefcon(void *rfc){refcon = rfc;};
};

intptr_t windowInfo::idCntr = 0;
std::map<void *, windowInfo*> windowInfoList;
XPLMWindowID winInFront = NULL;
XPLMWindowID winInFocus = NULL;

windowInfo::windowInfo(int size,
    int l, int t, int r, int b, int vis,
    XPLMDrawWindow_f drawFunc,
    XPLMHandleMouseClick_f mouseClickFunc,
    XPLMHandleKey_f keyFunc,
#if defined(XPLM200)
    XPLMHandleCursor_f cursorFunc,
    XPLMHandleMouseWheel_f wheelFunc,
#endif
    void *rfc
#if defined(XPLM300)
    ,int dFW, int lyr, 
    XPLMHandleMouseClick_f rightClickFunc
#endif
                        ) : id((void *)++idCntr)
{
  structSize = size;
  left = l; top = t; right = r; bottom = b; visible = vis;
  drawWindowFunc = drawFunc; handleMouseClickFunc = mouseClickFunc;
  handleKeyFunc = keyFunc; 
#if defined(XPLM200)
  handleCursorFunc = cursorFunc;
  handleMouseWheelFunc = wheelFunc;
#endif
  refcon = rfc;
#if defined(XPLM300)
  decorateAsFloatingWindow = dFW; layer = lyr;
  handleRightClickFunc = rightClickFunc;
#endif
  winInFront = id;
  winInFocus = id;
}

windowInfo::~windowInfo()
{
}

void windowInfo::draw()
{
  drawWindowFunc(id, refcon);
}

void windowInfo::handleKey(char inKey, XPLMKeyFlags inFlags, char inVirtualKey, int losingFocus)
{
  handleKeyFunc(id, inKey, inFlags, inVirtualKey, refcon, losingFocus);
}

int windowInfo::handleMouseClick(int x, int y, XPLMMouseStatus inMouse)
{
  return handleMouseClickFunc(id, x, y, inMouse, refcon);
}

#if defined(XPLM300)
int windowInfo::handleRightClick(int x, int y, XPLMMouseStatus inMouse)
{
  return handleRightClickFunc(id, x, y, inMouse, refcon);
}
#endif
#if defined(XPLM200)
XPLMCursorStatus windowInfo::handleCursor(int x, int y)
{
  return handleCursorFunc(id, x, y, refcon);
}

int windowInfo::handleMouseWheel(int x, int y, int wheel, int clicks)
{
  return handleMouseWheelFunc(id, x, y, wheel, clicks, refcon);
}
#endif

XPLMWindowID windowInfo::getID()
{
  return id;
}

bool windowInfo::isEx()
{
#if defined(XPLM200)
  if((handleCursorFunc == NULL) || (handleMouseWheelFunc == NULL)){
    return false;
  }
  return true;
#else
  return false;
#endif
}

#if defined(XPLM200)
XPLMWindowID XPLMCreateWindowEx(XPLMCreateWindow_t *inParams)
{
  windowInfo *wi = new windowInfo(inParams->structSize, inParams->left, inParams->top,
    inParams->right, inParams->bottom, inParams->visible, inParams->drawWindowFunc,
    inParams->handleMouseClickFunc, inParams->handleKeyFunc, inParams->handleCursorFunc,
    inParams->handleMouseWheelFunc, inParams->refcon
  #if defined(XPLM300)
    ,inParams->decorateAsFloatingWindow, inParams->layer, inParams->handleRightClickFunc
  #endif
  );
  windowInfoList.insert(std::pair<void *, windowInfo *>(wi->getID(), wi));
  #if defined(XPLM300)
  int0 = inParams->decorateAsFloatingWindow;
  int1 = inParams->layer;
  #endif
  return wi->getID();
}
#endif

XPLMWindowID XPLMCreateWindow(int inLeft,
                              int inTop,
                              int inRight,
                              int inBottom,
                              int inIsVisible,
                              XPLMDrawWindow_f       inDrawCallback,
                              XPLMHandleKey_f        inKeyCallback,
                              XPLMHandleMouseClick_f inMouseCallback,
                              void                  *inRefcon){
  windowInfo *wi = new windowInfo(
#if defined(XPLM200)
    sizeof(XPLMCreateWindow_t),
#else
    0,
#endif
    inLeft, inTop,
    inRight, inBottom, inIsVisible, inDrawCallback,
    inMouseCallback, inKeyCallback, 
  #if defined(XPLM200)
    NULL, NULL,
  #endif
    inRefcon
  #if defined(XPLM300)
    ,0, 0, NULL
  #endif
  );
  windowInfoList.insert(std::pair<void *, windowInfo *>(wi->getID(), wi));
  return wi->getID();
}

void XPLMDestroyWindow(XPLMWindowID inWindowID)
{
  for(std::map<void *, windowInfo*>::iterator i = windowInfoList.begin(); i != windowInfoList.end(); ++i){
    if((i->second)->getID() == inWindowID){
      windowInfo *ii = (i->second);
      windowInfoList.erase(i);
      ii->draw();
      ii->handleKey('P', xplm_ControlFlag | xplm_UpFlag, XPLM_VK_P, 1);
      int0 = ii->handleMouseClick(222, 333, xplm_MouseUp);
      if(ii->isEx()){
#if defined(XPLM200)
        int1 = ii->handleCursor(444, 555);
        int2 = ii->handleMouseWheel(666, 777, 8, 42);
#endif
#if defined(XPLM300)
        int3 = ii->handleRightClick(888, 999, xplm_MouseUp);
#endif
      }
      delete ii;
      //std::cout << "XPLMDestroyWindow removed window ID " << inWindowID << std::endl;
      return;
    }
  }
  std::cout << "XPLMDestroyWindow Error: Window ID " << inWindowID << " not found!" << std::endl;
}

void XPLMGetScreenSize(int *outWidth, int *outHeight)
{
  if(outWidth){
    *outWidth = 1920;
  }
  if(outHeight){
    *outHeight = 1080;
  }
}


#if defined(XPLM300)
void XPLMGetScreenBoundsGlobal(int *outLeft, int *outTop, int *outRight, int *outBottom)
{
  if(outLeft){
    *outLeft = 320;
  }
  if(outTop){
    *outTop = 480;
  }
  if(outRight){
    *outRight = 640;
  }
  if(outBottom){
    *outBottom = 240;
  }
}

void XPLMGetAllMonitorBoundsGlobal(XPLMReceiveMonitorBoundsGlobal_f inMonitorBoundsCallback, void *refcon)
{
  inMonitorBoundsCallback(0, 5, 1030, 1285, 6, refcon);
  inMonitorBoundsCallback(1, 1285, 1087, 3205, 7, refcon);
}

void XPLMGetAllMonitorBoundsOS(XPLMReceiveMonitorBoundsGlobal_f inMonitorBoundsCallback, void *refcon)
{
  inMonitorBoundsCallback(0, 9, 1208, 1609, 8, refcon);
  inMonitorBoundsCallback(1, 1610, 491, 2250, 11, refcon);
}
#endif

void XPLMGetMouseLocation(int *outX, int *outY)
{
  if(outX){
    *outX = 1024;
  }
  if(outY){
    *outY = 768;
  }
}

#if defined(XPLM300)
void XPLMGetMouseLocationGlobal(int *outX, int *outY)
{
  if(outX){
    *outX = 1600;
  }
  if(outY){
    *outY = 1200;
  }
}
#endif

windowInfo *findWinInfo(XPLMWindowID inWindowID)
{
  std::map<void *, windowInfo *>::iterator i = windowInfoList.find(inWindowID);
  if(i == windowInfoList.end()){
    std::cout << "Can't find window ID " << inWindowID << std::endl;
    return NULL;
  }
  return i->second;
}

void XPLMGetWindowGeometry(XPLMWindowID inWindowID,
                           int *        outLeft,
                           int *        outTop,
                           int *        outRight,
                           int *        outBottom)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  
  if(outLeft){
    *outLeft = wi->getLeft();
  }
  if(outTop){
    *outTop = wi->getTop();
  }
  if(outRight){
    *outRight = wi->getRight();
  }
  if(outBottom){
    *outBottom = wi->getBottom();
  }
}

void XPLMSetWindowGeometry(XPLMWindowID inWindowID,
                           int          inLeft,
                           int          inTop,
                           int          inRight,
                           int          inBottom)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  wi->setLeft(inLeft);
  wi->setTop(inTop);
  wi->setRight(inRight);
  wi->setBottom(inBottom);
  wi->draw();
  wi->handleKey('P', xplm_ControlFlag | xplm_UpFlag, XPLM_VK_P, 1);
  int0 = wi->handleMouseClick(222, 333, xplm_MouseUp);
  if(wi->isEx()){
#if defined(XPLM200)
    int1 = wi->handleCursor(444, 555);
    int2 = wi->handleMouseWheel(666, 777, 8, 42);
#endif
#if defined(XPLM300)
    int3 = wi->handleRightClick(888, 999, xplm_MouseUp);
#endif
  }
}

#if defined(XPLM300)
void XPLMGetWindowGeometryOS(XPLMWindowID inWindowID,
                             int *        outLeft,
                             int *        outTop,
                             int *        outRight,
                             int *        outBottom)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  
  if(outLeft){
    *outLeft = wi->getLeft() + 1024;
  }
  if(outTop){
    *outTop = wi->getTop();
  }
  if(outRight){
    *outRight = wi->getRight() + 1024;
  }
  if(outBottom){
    *outBottom = wi->getBottom();
  }
}

void XPLMSetWindowGeometryOS(XPLMWindowID inWindowID,
                             int          inLeft,
                             int          inTop,
                             int          inRight,
                             int          inBottom)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  wi->setLeft(inLeft - 1024);
  wi->setTop(inTop);
  wi->setRight(inRight - 1024);
  wi->setBottom(inBottom);
}
#endif

#if defined(XPLM301)
void XPLMGetWindowGeometryVR(XPLMWindowID inWindowID,
                             int *        outWidthBoxels,
                             int *        outHeightBoxels)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  
  if(outWidthBoxels){
    *outWidthBoxels = wi->getRight() - wi->getLeft();
  }
  if(outHeightBoxels){
    *outHeightBoxels = wi->getBottom() - wi->getTop();
  }
}

void XPLMSetWindowGeometryVR(XPLMWindowID inWindowID,
                             int          widthBoxels,
                             int          heightBoxels)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  wi->setLeft(widthBoxels - 512);
  wi->setTop(heightBoxels - 256);
  wi->setRight(widthBoxels + 512);
  wi->setBottom(heightBoxels + 256);
}
#endif

int XPLMGetWindowIsVisible(XPLMWindowID inWindowID)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return 0;
  }
  return wi->getVisible();
}

void XPLMSetWindowIsVisible(XPLMWindowID inWindowID,
                            int          inIsVisible)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  wi->setVisible(inIsVisible);
}

#if defined(XPLM301)
int XPLMWindowIsInVR(XPLMWindowID inWindowID)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return -1;
  }
  return wi->getVisible() * 8;
}
#endif

#if defined(XPLM300)
int XPLMWindowIsPoppedOut(XPLMWindowID inWindowID)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return -1;
  }
  return wi->getVisible() * 2;
}

void XPLMSetWindowGravity(XPLMWindowID inWindowID, float inLeftGravity, float inTopGravity, 
                          float inRightGravity, float inBottomGravity)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  int0 = wi->getVisible() * 3;
  f0 = inLeftGravity;
  f1 = inTopGravity;
  f2 = inRightGravity;
  f3 = inBottomGravity;
}

void XPLMSetWindowResizingLimits(XPLMWindowID inWindowID, int inMinWidthBoxels, int inMinHeightBoxels,
                                 int inMaxWidthBoxels, int inMaxHeightBoxels)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  int0 = inMinWidthBoxels + wi->getVisible() * 5;
  int1 = inMinHeightBoxels;
  int2 = inMaxWidthBoxels;
  int3 = inMaxHeightBoxels;
}

void XPLMSetWindowPositioningMode(XPLMWindowID inWindowID, XPLMWindowPositioningMode inPositioningMode, int inMonitorIndex)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  int0 = wi->getVisible() * 7;
  int1 = inPositioningMode;
  int2 = inMonitorIndex;
}

void XPLMSetWindowTitle(XPLMWindowID inWindowID, const char *inWindowTitle)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  int0 = wi->getVisible() * 9;
  str0 = inWindowTitle;
}
#endif

void *XPLMGetWindowRefCon(XPLMWindowID inWindowID)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return 0;
  }
  void *res = wi->getRefcon();
  return res;
}

void XPLMSetWindowRefCon(XPLMWindowID inWindowID,
                         void *       inRefcon)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  wi->setRefcon(inRefcon);
}

void XPLMTakeKeyboardFocus(XPLMWindowID inWindowID)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  winInFocus = inWindowID;
}

#if defined(XPLM300)
int XPLMHasKeyboardFocus(XPLMWindowID inWindowID)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return 0;
  }
  if(winInFocus == inWindowID){
    return 1;
  }
  return 0;
}
#endif

void XPLMBringWindowToFront(XPLMWindowID inWindowID)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return;
  }
  winInFront = inWindowID;
  //std::cout << "Window " << inWindowID << " brought to front" << std::endl;
}

int XPLMIsWindowInFront(XPLMWindowID inWindowID)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return 0;
  }
  return winInFront == inWindowID;
}

class hotkey{
  static intptr_t hotkeyCounter;
  char virtualKey;
  XPLMKeyFlags flags;
  const char *description;
  XPLMHotKey_f callback;
  void *refcon;
  XPLMHotKeyID id;
 public:
  hotkey(char         inVirtualKey,
         XPLMKeyFlags inFlags,
         const char * inDescription,
         XPLMHotKey_f inCallback,
         void *       inRefcon);
  ~hotkey(){};
  XPLMHotKeyID getID()const{return id;};
  void getInfo(char *        outVirtualKey,
               XPLMKeyFlags *outFlags,
               char *        outDescription,
               XPLMPluginID *outPlugin);
  void setKey(char         inVirtualKey,
              XPLMKeyFlags inFlags){virtualKey = inVirtualKey; flags = inFlags;};
  void press()const{callback(refcon);};
};

intptr_t hotkey::hotkeyCounter = 0;
std::vector<hotkey> hotkeyVector;

hotkey::hotkey(char         inVirtualKey,
               XPLMKeyFlags inFlags,
               const char * inDescription,
               XPLMHotKey_f inCallback,
               void *       inRefcon): virtualKey(inVirtualKey), flags(inFlags), 
                                       description(inDescription), callback(inCallback),
                                       refcon(inRefcon), id((XPLMHotKeyID)++hotkeyCounter)
{
}

void hotkey::getInfo(char *        outVirtualKey,
                     XPLMKeyFlags *outFlags,
                     char *        outDescription,
                     XPLMPluginID *outPlugin)
{
  if(outVirtualKey){
    *outVirtualKey = virtualKey;
  }
  if(outFlags){
    *outFlags = flags;
  }
  if(outDescription){
    strcpy(outDescription, description);
  }
  if(outPlugin){
    //TODO!!! Do we differentiate between Python plugins somehow?
    *outPlugin = 42;
  }
}




XPLMHotKeyID XPLMRegisterHotKey(char         inVirtualKey,
         XPLMKeyFlags inFlags,
         const char * inDescription,
         XPLMHotKey_f inCallback,
         void *       inRefcon)
{
  hotkey hk(inVirtualKey, inFlags, inDescription, inCallback, inRefcon);
  hotkeyVector.push_back(hk);
  //std::cout << "Hotkey Desc: '" << inDescription << "'" << std::endl;
  return hk.getID();
}

void XPLMUnregisterHotKey(XPLMHotKeyID inHotKey)
{
  for(std::vector<hotkey>::iterator i = hotkeyVector.begin(); i != hotkeyVector.end(); ++i){
    if(i->getID() == inHotKey){
      hotkeyVector.erase(i);
      return;
    }
  }
  std::cout << "XPLMUnregisterHotKey Error: couldn't find hotkey ID " << inHotKey << std::endl;
}

int XPLMCountHotKeys(void){
  return hotkeyVector.size();
}

XPLMHotKeyID XPLMGetNthHotKey(int inIndex)
{
  std::vector<hotkey>::size_type index = inIndex;
  if(index >= hotkeyVector.size()){
    return NULL;
  }
  //std::cout << "Hotkey index " << inIndex << " requested" << std::endl;
  hotkeyVector[inIndex].press();
  return hotkeyVector[inIndex].getID();
}

void XPLMGetHotKeyInfo(XPLMHotKeyID  inHotKey,
                       char *        outVirtualKey,
                       XPLMKeyFlags *outFlags,
                       char *        outDescription,
                       XPLMPluginID *outPlugin)
{
  for(std::vector<hotkey>::iterator i = hotkeyVector.begin(); i != hotkeyVector.end(); ++i){
    if(i->getID() == inHotKey){
      i->getInfo(outVirtualKey, outFlags, outDescription, outPlugin);
      return;
    }
  }
  std::cout << "XPLMGetHotKeyInfo Error: couldn't find hotkey ID " << inHotKey << std::endl;
}

void XPLMSetHotKeyCombination(XPLMHotKeyID inHotKey,
                              char         inVirtualKey,
                              XPLMKeyFlags inFlags)
{
  for(std::vector<hotkey>::iterator i = hotkeyVector.begin(); i != hotkeyVector.end(); ++i){
    if(i->getID() == inHotKey){
      i->setKey(inVirtualKey, inFlags);
      return;
    }
  }
  std::cout << "XPLMSetHotKeyCombination Error: couldn't find hotkey ID " << inHotKey << std::endl;
}


