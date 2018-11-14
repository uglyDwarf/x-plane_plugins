#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <dlfcn.h>
#include <string.h>
#include <sstream>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMUtilities.h>

#include "chk_helper.h"


static int keyType;
static int key;
static std::string string;
static int button;
static int pressed;

static std::list<XPLMDataRef> d;


void initUtilitiesModule()
{
  d.push_back(registerROAccessor("keyType", keyType));
  d.push_back(registerROAccessor("key", key));
  d.push_back(registerROAccessor("string", string));

  d.push_back(registerROAccessor("button", button));
  d.push_back(registerROAccessor("state", pressed));
}

void cleanupUtilitiesModule()
{
  for(std::list<XPLMDataRef>::iterator i = d.begin(); i != d.end(); ++i){
    XPLMUnregisterDataAccessor(*i);
  }
  d.empty();
}

void XPLMSimulateKeyPress(int inKeyType, int inKey)
{
  keyType = inKeyType;
  key = inKey;
}

void XPLMSpeakString(const char *inString)
{
  string = inString;
}

void XPLMCommandKeyStroke(XPLMCommandKeyID inKey)
{
  key = inKey;
}

void XPLMCommandButtonPress(XPLMCommandButtonID inButton)
{
  button = inButton;
  pressed = 1;
}

void XPLMCommandButtonRelease(XPLMCommandButtonID inButton)
{
  button = inButton;
  pressed = 0;
}

const char *XPLMGetVirtualKeyDescription(char inVirtualKey)
{
  std::ostringstream tmp;
  tmp << "VK" << static_cast<int>(inVirtualKey);
  string = tmp.str();
  return string.c_str();
}

void XPLMReloadScenery(void)
{
  key = 0xDEADBEEF;
}

void XPLMGetSystemPath(char *outSystemPath)
{
  strcpy(outSystemPath, "/X-System/folder/");
}

void XPLMGetPrefsPath(char *outPrefsPath)
{
  strcpy(outPrefsPath, "/X-System/prefs/");
}

const char *XPLMGetDirectorySeparator(void)
{
  static const char separator[] = "/";
  return separator;
}

char *XPLMExtractFileAndPath(char *inFullPath)
{
  size_t i = strlen(inFullPath);
  const char *sep = XPLMGetDirectorySeparator();

  while(i > 0){
    if(inFullPath[i] == *sep){
      inFullPath[i] = '\0';
      return inFullPath + i + 1;
    }
    --i;
  }
  //how the hell do they handle a file in root?
  return inFullPath;
}

int XPLMGetDirectoryContents(const char *inDirectoryPath, int inFirstReturn, char *outFileNames, int inFileNameBufSize,
                             char **outIndices, int inIndexCount, int *outTotalFiles, int *outReturnedFiles)
{
  string = inDirectoryPath;
  key = inFirstReturn;
  keyType = inFileNameBufSize;
  button = inIndexCount;
  int index = 0;
  char *text = outFileNames;
  size_t remaining = inFileNameBufSize;
  while(index < inIndexCount){
    std::ostringstream fname;
    fname << "File" << index + inFirstReturn;
    std::string str = fname.str();
    if(remaining <= str.length()){ // have to take the closing \0 into account
      break;
    }
    remaining -= str.length() + 1;
    strcpy(text, str.c_str());
    outIndices[index] = text;
    text += str.length() + 1;
    ++index;
  }
  *outTotalFiles = index + inFirstReturn;
  int res = 0;
  if(index < inIndexCount){
    //reuse the last string's \0
    outIndices[index] = text - 1;
    res = 1;
    ++index;
  }
  *outReturnedFiles = index;
  return res;
}


int XPLMInitialized(void)
{
  return 1;
}

void XPLMGetVersions(int *outXPlaneVersion, int *outXPLMVersion, XPLMHostApplicationID *outHostID)
{
  *outXPlaneVersion = 1125;
  *outXPLMVersion = 365;
  #if defined(XPLM301)
  *outXPlaneVersion = 1120;
  *outXPLMVersion = 301;
  #elif defined(XPLM300)
  *outXPlaneVersion = 1110;
  *outXPLMVersion = 300;
  #elif defined(XPLM210)
  *outXPlaneVersion = 1060;
  *outXPLMVersion = 210;
  #else
  *outXPlaneVersion = 970;
  *outXPLMVersion = 200;
  #endif
  *outHostID = xplm_Host_YoungsMod;
}

XPLMLanguageCode XPLMGetLanguage(void)
{
  return xplm_Language_Greek;
}

void XPLMDebugString(const char *inString)
{
  string = inString;
}

static XPLMError_f err_cbk = NULL;

void XPLMSetErrorCallback(XPLMError_f inCallback)
{
  err_cbk = inCallback;
}

void *XPLMFindSymbol(const char *inString)
{
  string = inString;
  if(err_cbk){
    err_cbk("Roses are red, violets are blue and this code desperately needs some love...");
  }
  return (void *)((intptr_t)0xDEADBEEF);
}

int XPLMLoadDataFile(XPLMDataFileType inFileType, const char *inFilePath)
{
  key = inFileType;
  string = inFilePath;
  return 1;
}

int XPLMSaveDataFile(XPLMDataFileType inFileType, const char *inFilePath)
{
  key = inFileType;
  string = inFilePath;
  return 1;
}


class command{
  typedef std::pair<XPLMCommandCallback_f, void *> handlerRecord;
  std::string name;
  std::string description;
 public:
  command(std::string inName, std::string inDescription):name(inName), description(inDescription){};
  std::list<handlerRecord> beforeCallbacks;
  std::list<handlerRecord> afterCallbacks;
  void addHandler(XPLMCommandCallback_f inHandler, int inBefore, void *inRefcon);
  void removeHandler(XPLMCommandCallback_f inHandler, int inBefore, void *inRefcon);
  bool call(XPLMCommandPhase phase);
};

bool command::call(XPLMCommandPhase phase)
{
  //res == true => interrupt processing
  std::list<handlerRecord>::iterator i;
  for(i = beforeCallbacks.begin(); i != beforeCallbacks.end(); ++i){
    if((i->first)(static_cast<void *>(this), phase, i->second) == 0){
      return true;
    }
  }

  for(i = afterCallbacks.begin(); i != afterCallbacks.end(); ++i){
    if((i->first)(static_cast<void *>(this), phase, i->second) == 0){
      return true;
    }
  }
  return false;
}

void command::addHandler(XPLMCommandCallback_f inHandler, int inBefore, void *inRefcon)
{
  ((inBefore == 1) ? beforeCallbacks : afterCallbacks).push_back(std::make_pair(inHandler, inRefcon));
}

void command::removeHandler(XPLMCommandCallback_f inHandler, int inBefore, void *inRefcon)
{
  std::list<handlerRecord> *rec = (inBefore == 1) ? &beforeCallbacks : &afterCallbacks;
  for(std::list<handlerRecord>::iterator i = rec->begin(); i != rec->end(); ++i){
    if((i->first == inHandler) && (i->second == inRefcon)){
      rec->erase(i);
      break;
    }
  }
}

static std::map<std::string, command*> commands;

XPLMCommandRef XPLMFindCommand(const char *name)
{
  std::map<std::string, command*>::iterator i = commands.find(std::string(name));
  if(i != commands.end()){
    return static_cast<void *>(i->second);
  }
  return NULL;
}

void XPLMCommandBegin(XPLMCommandRef inCommand)
{
  command *c = static_cast<command*>(inCommand);
  c->call(xplm_CommandBegin);
  c->call(xplm_CommandContinue);
}

void XPLMCommandEnd(XPLMCommandRef inCommand)
{
  command *c = static_cast<command*>(inCommand);
  c->call(xplm_CommandEnd);
}

void XPLMCommandOnce(XPLMCommandRef inCommand)
{
  command *c = static_cast<command*>(inCommand);
  c->call(xplm_CommandBegin);
  c->call(xplm_CommandEnd);
}

XPLMCommandRef XPLMCreateCommand(const char *inName, const char *inDescription)
{
  XPLMCommandRef ref = XPLMFindCommand(inName);
  if(ref != NULL){
    return ref;
  }
  command *c = new command(inName, inDescription);
  commands.insert(std::pair<std::string, command*>(inName, c));
  return c;
}

void XPLMRegisterCommandHandler(XPLMCommandRef inCommand, XPLMCommandCallback_f inHandler, int inBefore, void *inRefcon)
{
  command *c = static_cast<command*>(inCommand);
  c->addHandler(inHandler, inBefore, inRefcon);
}

void XPLMUnregisterCommandHandler(XPLMCommandRef inCommand, XPLMCommandCallback_f inHandler, int inBefore, void *inRefcon)
{
  command *c = static_cast<command*>(inCommand);
  c->removeHandler(inHandler, inBefore, inRefcon);
}


