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
#include <XPLM/XPLMPlugin.h>

#include "chk_helper.h"

static int id;
static std::string path;
static std::string signature;
static std::map<std::string, int> features;

void initPluginModule()
{
}

void cleanupPluginModule()
{
}


XPLMPluginID XPLMGetMyID(void)
{
  return id + 1;
}

int XPLMCountPlugins(void)
{
  return id + 2;
}

XPLMPluginID XPLMGetNthPlugin(int inIndex)
{
  return inIndex + 1;
}

XPLMPluginID XPLMFindPluginByPath(const char *inPath)
{
  path = inPath;
  return id + 3;
}

XPLMPluginID XPLMFindPluginBySignature(const char *inSignature)
{
  signature = inSignature;
  return id + 4;
}

void XPLMGetPluginInfo(XPLMPluginID inPlugin, char *outName, char *outFilePath, char *outSignature, char *outDescription)
{
  id = inPlugin;
  strcpy(outName, "plugin name");
  strcpy(outFilePath, path.c_str());
  strcpy(outSignature, signature.c_str());
  strcpy(outDescription, "plugin description");
}

int XPLMIsPluginEnabled(XPLMPluginID inPluginID)
{
  id = inPluginID;
  return inPluginID + 100;
}

int XPLMEnablePlugin(XPLMPluginID inPluginID)
{
  return inPluginID + 101;
}

void XPLMDisablePlugin(XPLMPluginID inPluginID)
{
  id += inPluginID;
}

void XPLMReloadPlugins(void)
{
  id = -1;
}

void sendMessage(XPLMPluginID inFromWho, long inMessage, void* inParam);

void XPLMSendMessageToPlugin(XPLMPluginID inPlugin, int inMessage, void *inParam)
{
  sendMessage(inPlugin, inMessage, inParam);
}

int XPLMHasFeature(const char *inFeature)
{
  std::map<std::string, int>::const_iterator i = features.find(std::string(inFeature));
  if(i != features.end()){
    return 1;
  }
  return 0;
}

int XPLMIsFeatureEnabled(const char *inFeature)
{
  return features[std::string(inFeature)];
}

void XPLMEnableFeature(const char *inFeature, int inEnable)
{
  features[std::string(inFeature)] = inEnable;
}

void XPLMEnumerateFeatures(XPLMFeatureEnumerator_f inEnumerator, void *inRef)
{
  for(std::map<std::string, int>::const_iterator i = features.begin(); i != features.end(); ++i){
    inEnumerator(i->first.c_str(), inRef);
  }
}

