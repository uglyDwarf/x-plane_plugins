#include <iostream>
#include <map>
#include <vector>
#include <string.h>

#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDisplay.h>

#include "chk_helper.h"
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
  drawCallbacks.push_back(new drawCallback(inCallback, inPhase, inWantsBefore, inRefcon));
  return 1;
}


int XPLMRegisterKeySniffer(XPLMKeySniffer_f inCallback,
                           int              inBeforeWindows,
                           void            *inRefcon)
{
  keySniffCallbacks.push_back(new keySniffCallback(inCallback, inBeforeWindows, inRefcon));
  return 1;
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
      return 1;
    }
  }
  return 0;
}

class windowInfo{
  XPLMCreateWindow_t info;
  XPLMWindowID id;
  static intptr_t idCntr;
 public:
  windowInfo(XPLMCreateWindow_t *inParams);
  ~windowInfo();
  void draw();
  void handleKey(char inKey, XPLMKeyFlags inFlags, char inVirtualKey, int losingFocus);
  int handleMouseClick(int x, int y, XPLMMouseStatus inMouse);
  XPLMCursorStatus handleCursor(int x, int y);
  int handleMouseWheel(int x, int y, int wheel, int clicks);
  bool isEx();
  XPLMWindowID getID();
  int getLeft()const{return info.left;};
  int getTop()const{return info.top;};
  int getRight()const{return info.right;};
  int getBottom()const{return info.bottom;};
  int getVisible()const{return info.visible;};
  void *getRefcon()const{return info.refcon;};
  void setLeft(int left){info.left = left;};
  void setTop(int top){info.top = top;};
  void setRight(int right){info.right = right;};
  void setBottom(int bottom){info.bottom = bottom;};
  void setVisible(int visible){info.visible = visible;};
  void setRefcon(void *refcon){info.refcon = refcon;};
};

intptr_t windowInfo::idCntr = 0;
std::map<void *, windowInfo*> windowInfoList;
XPLMWindowID winInFront = NULL;
XPLMWindowID winInFocus = NULL;

windowInfo::windowInfo(XPLMCreateWindow_t *inParams) : info(*inParams), id((void *)++idCntr)
{
  winInFront = id;
  winInFocus = id;
}

windowInfo::~windowInfo()
{
}

void windowInfo::draw()
{
  info.drawWindowFunc(id, info.refcon);
}

void windowInfo::handleKey(char inKey, XPLMKeyFlags inFlags, char inVirtualKey, int losingFocus)
{
  info.handleKeyFunc(id, inKey, inFlags, inVirtualKey, info.refcon, losingFocus);
}

int windowInfo::handleMouseClick(int x, int y, XPLMMouseStatus inMouse)
{
  return info.handleMouseClickFunc(id, x, y, inMouse, info.refcon);
}

XPLMCursorStatus windowInfo::handleCursor(int x, int y)
{
  return info.handleCursorFunc(id, x, y, info.refcon);
}

int windowInfo::handleMouseWheel(int x, int y, int wheel, int clicks)
{
  return info.handleMouseWheelFunc(id, x, y, wheel, clicks, info.refcon);
}

XPLMWindowID windowInfo::getID()
{
  return id;
}

bool windowInfo::isEx()
{
  if((info.handleCursorFunc == NULL) || (info.handleMouseWheelFunc == NULL)){
    return false;
  }
  return true;
}

XPLMWindowID XPLMCreateWindowEx(XPLMCreateWindow_t *inParams)
{
  windowInfo *wi = new windowInfo(inParams);
  windowInfoList.insert(std::pair<void *, windowInfo *>(wi->getID(), wi));
  return wi->getID();
}

XPLMWindowID XPLMCreateWindow(int inLeft,
                              int inTop,
                              int inRight,
                              int inBottom,
                              int inIsVisible,
                              XPLMDrawWindow_f       inDrawCallback,
                              XPLMHandleKey_f        inKeyCallback,
                              XPLMHandleMouseClick_f inMouseCallback,
                              void                  *inRefcon){
  XPLMCreateWindow_t p = {.structSize = sizeof(XPLMCreateWindow_t),
                          .left = inLeft,
                          .top = inTop,
                          .right = inRight,
                          .bottom = inBottom,
                          .visible = inIsVisible,
                          .drawWindowFunc = inDrawCallback,
                          .handleMouseClickFunc = inMouseCallback,
                          .handleKeyFunc = inKeyCallback,
                          .handleCursorFunc = NULL,
                          .handleMouseWheelFunc = NULL,
                          .refcon = inRefcon
                         };
  return XPLMCreateWindowEx(&p);
}

void XPLMDestroyWindow(XPLMWindowID inWindowID)
{
  for(std::map<void *, windowInfo*>::iterator i = windowInfoList.begin(); i != windowInfoList.end(); ++i){
    if((i->second)->getID() == inWindowID){
      windowInfo *ii = (i->second);
      windowInfoList.erase(i);
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

void XPLMGetMouseLocation(int *outX, int *outY)
{
  if(outX){
    *outX = 1024;
  }
  if(outY){
    *outY = 768;
  }
}

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
}

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

void *XPLMGetWindowRefCon(XPLMWindowID inWindowID)
{
  windowInfo *wi;
  if(!(wi = findWinInfo(inWindowID))){
    return 0;
  }
  return wi->getRefcon();
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
  std::cout << "Focus set to window " << inWindowID << std::endl;
  add_int_value("XPLMTakeKeyboardFocus:inWindowID", (intptr_t)inWindowID);
}

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
  if((inIndex < 0) || (inIndex >= hotkeyVector.size())){
    return NULL;
  }
  //std::cout << "Hotkey index " << inIndex << " requested" << std::endl;
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

void checkDisplayModule()
{
  std::vector<drawCallback*>::iterator i;
  for(i = drawCallbacks.begin(); i != drawCallbacks.end(); ++i){
    (*i)->call();
  }

  std::vector<keySniffCallback*>::iterator j;
  for(j = keySniffCallbacks.begin(); j != keySniffCallbacks.end(); ++j){
    (*j)->call('Q', xplm_ShiftFlag | xplm_DownFlag, XPLM_VK_Q );
  }

  std::map<void *, windowInfo*>::iterator k;
  for(k = windowInfoList.begin(); k != windowInfoList.end(); ++k){
    (k->second)->draw();
    (k->second)->handleKey('P', xplm_ControlFlag | xplm_UpFlag, XPLM_VK_P, 1);
    std::cout << "handleMouseClick returned " << (k->second)->handleMouseClick(222, 333, xplm_MouseUp) << std::endl;
    if((k->second)->isEx()){
      std::cout << "handleCursor returned " << (k->second)->handleCursor(444, 555) << std::endl;
      std::cout << "handleMouseWheel returned " << (k->second)->handleMouseWheel(666, 777, 8, 42) << std::endl;
    }
  }
  std::vector<hotkey>::iterator l;
  for(l = hotkeyVector.begin(); l != hotkeyVector.end(); ++l){
    l->press();
  }
}


