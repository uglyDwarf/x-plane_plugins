#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "utils.h"

#define XPLM200
#define APL 0
#define IBM 0
#define LIN 1
#include "XPLMUtilities.h"

#include <speech-dispatcher/libspeechd.h>

static SPDConnection *conn = NULL;

bool speech_init(void)
{
  conn = spd_open("XLinSpeak", "Main", "player", SPD_MODE_SINGLE);
  if(conn == NULL){
    xcDebug("XLinSpeak: Couldn't init speech!\n");
    return false;
  }
  return true;
}

void speech_say(char *str)
{
  if(conn != NULL){
    spd_say(conn, SPD_MESSAGE, str);
  }
}

void speech_close(void)
{
  if(conn != NULL){
    spd_close(conn);
  }
}

static char *msg = NULL;
static size_t msgSize = 0;

static void xcDebugInt(const char *format, va_list va)
{
  va_list vc;
  int res;
  if(msg == NULL){
    msgSize = 2;
    msg = (char *)malloc(msgSize);
  }
  if(msg == NULL){
    XPLMDebugString("XLinSpeak: Couldn't allocate buffer for messages!\n");
    return;
  }
  while(1){ /*looping once, in case of string too big*/
    /*copy, in case we need another go*/
    va_copy(vc, va);
    res = vsnprintf(msg, msgSize, format, vc);
    va_end(vc);
    
    if((res > -1) && ((size_t)res < msgSize)){
      XPLMDebugString(msg);
      return;
    }else{
      void *tmp;
      msgSize *= 2;
      if((tmp = realloc(msg, msgSize)) == NULL){
        break;
      }
      msg = (char *)tmp;
    }
  }
  
  XPLMDebugString("XLinSpeak: Problem with debug message formatting!\n");
  msg = NULL;
  msgSize = 0;
  return;
}

void xcDebug(const char *format, ...)
{
  va_list ap;
  va_start(ap,format);
  xcDebugInt(format, ap);
  va_end(ap);
}



