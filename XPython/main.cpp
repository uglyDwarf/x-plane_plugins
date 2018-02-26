#include <iostream>
#include <map>
#include <vector>
#include <dlfcn.h>
#include <string.h>
#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMGraphics.h>

#include "chkDisplay.h"
#include "chkGraphics.h"
#include "chkUtilities.h"
#include "chkScenery.h"
#include "chkMenus.h"
#include "chkNavigation.h"
#include "chkPlugin.h"
#include "chkPlanes.h"
#include "chkProcessing.h"
#include "chkCamera.h"
#include "chkWidgets.h"
#include "chkUIGraphics.h"
#include "chkWidgetUtils.h"

//extern int XPluginStart(char *outName,
//                        char *outSig,
//                        char *outDesc);

char outName[512];
char outSig[512];
char outDesc[512];

typedef int  (*XPluginStart_proto)(char *outName, char *outSig, char *outDesc);
XPluginStart_proto XPluginStart = NULL;
typedef void (*XPluginStop_proto)(void);
XPluginStop_proto XPluginStop = NULL;
typedef int  (*XPluginEnable_proto)(void);
XPluginEnable_proto XPluginEnable = NULL;
typedef void (*XPluginDisable_proto)(void);
XPluginDisable_proto XPluginDisable = NULL;
typedef void (*XPluginReceiveMessage_proto)(XPLMPluginID inFromWho, long inMessage, void* inParam);
XPluginReceiveMessage_proto XPluginReceiveMessage = NULL;

void *loadFunction(void *libHandle, const char *funcName)
{
  void *fun;
  dlerror();
  fun = dlsym(libHandle, funcName);
  if(fun == NULL){
    std::cout<<"Can't find symbol " << funcName << "(" << dlerror() << ")." << std::endl;
  }
  return fun;
}

void sendMessage(XPLMPluginID inFromWho, long inMessage, void* inParam)
{
  XPluginReceiveMessage(inFromWho, inMessage, inParam);
}

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  std::cout << "Hello World!" << std::endl;
  dlerror();
  void *plugin = dlopen("./plugin.so", RTLD_NOW | RTLD_GLOBAL);
  std::cout << "Plugin: " << plugin << std::endl;
  if(plugin == NULL){
    std::cout << "Can't load plugin (" << dlerror() << ")" << std::endl;
    return 1;
  }
  XPluginStart = (XPluginStart_proto)loadFunction(plugin, "XPluginStart");
  XPluginStop = (XPluginStop_proto)loadFunction(plugin, "XPluginStop");
  XPluginEnable = (XPluginEnable_proto)loadFunction(plugin, "XPluginEnable");
  XPluginDisable = (XPluginDisable_proto)loadFunction(plugin, "XPluginDisable");
  XPluginReceiveMessage = (XPluginReceiveMessage_proto)loadFunction(plugin, "XPluginReceiveMessage");
  if(!(XPluginStart && XPluginStop && XPluginEnable && XPluginDisable && XPluginReceiveMessage)){
    dlclose(plugin);
    return 1;
  }

  initUtilitiesModule();
  initSceneryModule();
  initMenusModule();
  initNavigationModule();
  initPluginModule();
  initGraphicsModule();
  initPlanesModule();
  initProcessingModule();
  initCameraModule();
  initDisplayModule();
  initWidgetsModule();
  initUIGraphicsModule();
  initWidgetUtilsModule();

  int res = XPluginStart(outName, outSig, outDesc);
  std::cout << "Y-Plane loaded plugin " << outName << "(" << res << ")" << std::endl;
  std::cout << "  Signature: " << outSig << std::endl;
  std::cout << "  Description:" << outDesc << std::endl;
  //XPluginReceiveMessage(5, 103, (void*)333);

  XPluginDisable();
  XPluginEnable();
  std::cout << "===============================================" << std::endl;
  XPluginStop();

  cleanupUtilitiesModule();
  cleanupSceneryModule();
  cleanupMenusModule();
  cleanupNavigationModule();
  cleanupPluginModule();
  cleanupGraphicsModule();
  cleanupPlanesModule();
  cleanupProcessingModule();
  cleanupCameraModule();
  cleanupDisplayModule();
  cleanupWidgetsModule();
  cleanupUIGraphicsModule();
  cleanupWidgetUtilsModule();

  dlclose(plugin);
  return 0;
}

