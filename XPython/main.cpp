#include <iostream>
#include <map>
#include <vector>
#include <dlfcn.h>
#include <string.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMUtilities.h>

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
#include "chkInstance.h"
#include "chkMap.h"

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

#if LIN
#if __x86_64__
  const char plugin_path[] =  "./Resources/plugins/XPPython3/lin_x64/xppython3.xpl";
#else
  const char plugin_path[] =  "./Resources/plugins/XPPython3/lin_i686/xppython3.xpl";
#endif
#elif APL
  const char plugin_path[] =  "./Resources/plugins/XPPython3/mac_x64/xppython3.xpl";
#elif IBM
  const char plugin_path[] =  "./Resources/plugins/XPPython3/win_x64/xppython3.xpl";
#endif

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
  dlerror();
  void *plugin = NULL;
  plugin = dlopen(plugin_path, RTLD_NOW | RTLD_GLOBAL);
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
  initInstanceModule();
  initMapModule();

  int res = XPluginStart(outName, outSig, outDesc);
  XPLMCommandRef reloadCmd = XPLMFindCommand("XPPython3/reloadScripts");
  std::cout << "Y-Plane loaded plugin " << outName << "(" << res << ")" << std::endl;
  std::cout << "  Signature: " << outSig << std::endl;
  std::cout << "  Description:" << outDesc << std::endl;
  
  for(int c = 0; c < 1000; ++c){
    if(c % 50 == 0){
      std::cout << c << std::endl;
    }
    if((c > 0) && ((c % 100) == 0)){
      std::cout << "Reloading..." << std::endl;
      XPLMCommandOnce(reloadCmd);
    }
    XPluginReceiveMessage(5, 103, (void*)333);
    XPluginDisable();
    XPluginEnable();
  }
  XPluginStop();
  std::cout << "===============================================" << std::endl;

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
  cleanupInstanceModule();
  cleanupMapModule();
  dlclose(plugin);
  return 0;
}

