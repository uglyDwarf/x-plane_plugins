#define XPLM200
#define APL 0
#define IBM 0
#define LIN 1

#include <stdio.h>
#include <string.h>
#include <XPLMDefs.h>
#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>
#include "hook.h"
#include "sec.h"

struct function_ptrs ptrs[] = {
  {.name = "_ZN10spch_class18SPEECH_speakstringENSt3__112basic_stringIcNS0_11char_traitsIcEENS0_9allocatorIcEEEE11speech_typei", .address = 0, .hook = 1},
  {.name = "_ZN10soun_class18SPEECH_speakstringESs11speech_typei", .address = 0, .hook = 0},
  {.name = "_ZN10soun_class18SPEECH_speakstringESsi", .address = 0, .hook = 2}
};


PLUGIN_API int XPluginStart(
						char *		outName,
						char *		outSig,
						char *		outDesc)
{
  strcpy(outName, "Kecal");
  strcpy(outSig, "kec03");
  strcpy(outDesc, "Speak up now");

  xcDebug("Kecal: going to init tables...\n");
  if(!locate_tables()){
    xcDebug("Couldn't init tables!\n");
    return 1;
  }
  xcDebug("Kecal: going to search for functions...\n");
  if(!find_functions(ptrs, sizeof(ptrs) / sizeof(ptrs[0]))){
    xcDebug("Search for functions unsuccessful!\n");
  }

  printf("Addr: %p\n", (void *)(intptr_t)ptrs[1].address);

  xcDebug("Kecal: Going to init speech.\n");
  if(!speech_init()){
    xcDebug("Kecal: Speech not ready!\n");
    //speech_test();
    return 1;
  }
  xcDebug("Kecal: Speech OK, now hooks.\n");
  unsigned int i;
  for(i = 0; i < sizeof(ptrs)/sizeof(ptrs[0]); ++i){
    if(ptrs[i].address == 0){
      continue;
    }
    xcDebug("Kecal: Hook%ud: Addr 0x%llX Hook proc %d.\n", i, ptrs[i].address, ptrs[i].hook);
    if(hook((void *)((intptr_t)ptrs[i].address), ptrs[i].hook)){
      xcDebug("Kecal: Hook %d initialized.\n", i);
    }else{
      xcDebug("Kecal: Hook %d unsuccessful.\n", i);
    }
  }  


  return 1;
}

PLUGIN_API void	XPluginStop(void)
{
  //speech_close();
}

PLUGIN_API int XPluginEnable(void)
{
  return 1;
}

PLUGIN_API void XPluginDisable(void)
{
}


PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam)
{
  (void) inFrom;
  (void) inMsg;
  (void) inParam;
}

