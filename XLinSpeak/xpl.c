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
  {.name = "_ZN10spch_class22SPEECH_synth_non_radioENSt3__112basic_stringIcNS0_11char_traitsIcEENS0_9allocatorIcEEEE11speech_typei", .address = 0, .hook = 1},
  {.name = "_ZN10spch_class18SPEECH_speakstringENSt3__112basic_stringIcNS0_11char_traitsIcEENS0_9allocatorIcEEEE11speech_typei", .address = 0, .hook = 1},
  {.name = "_ZN10soun_class18SPEECH_speakstringESs11speech_typei", .address = 0, .hook = 0},
  {.name = "_ZN10soun_class18SPEECH_speakstringESsi", .address = 0, .hook = 2}
};


PLUGIN_API int XPluginStart(
						char *		outName,
						char *		outSig,
						char *		outDesc)
{
  strcpy(outName, "XLinSpeak");
  strcpy(outSig, "XLinSpeak v04");
  strcpy(outDesc, "Speak up now");

  xcDebug("XLinSpeak going to init tables...\n");
  if(!locate_tables()){
    xcDebug("Couldn't init tables!\n");
    return 1;
  }
  xcDebug("XLinSpeak going to search for functions...\n");
  if(!find_functions(ptrs, sizeof(ptrs) / sizeof(ptrs[0]))){
    xcDebug("XLinSpeak Search for functions unsuccessful!\n");
    return 1;
  }

  unsigned int i;
  for(i = 0; i < sizeof(ptrs) / sizeof(ptrs[0]); ++i){
    if(ptrs[i].address != 0){
      xcDebug("XLinSpeak Addr: %p\n", (void *)(intptr_t)ptrs[i].address);
    }
  }

  xcDebug("XLinSpeak Going to init speech.\n");
  if(!speech_init()){
    xcDebug("XLinSpeak Speech not ready!\n");
    //speech_test();
    return 1;
  }
  xcDebug("XLinSpeak Speech OK, now hooks.\n");
  for(i = 0; i < sizeof(ptrs)/sizeof(ptrs[0]); ++i){
    if(ptrs[i].address == 0){
      continue;
    }
    xcDebug("XLinSpeak Hook%ud: Addr 0x%llX Hook proc %d.\n", i, ptrs[i].address, ptrs[i].hook);
    if(hook((void *)((intptr_t)ptrs[i].address), ptrs[i].hook)){
      xcDebug("XLinSpeak Hook %d initialized.\n", i);
    }else{
      xcDebug("XLinSpeak Hook %d unsuccessful.\n", i);
    }
  }
  //XPLMSpeakString("XLinSpeak is working! Enjoy the ride.");

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

