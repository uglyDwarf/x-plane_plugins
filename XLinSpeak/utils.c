#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h> 
#include <string.h>

#include "utils.h"

#include "libconfig/lib/libconfig.h"

#define XPLM200
#define APL 0
#define IBM 0
#define LIN 1
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"

#include <speech-dispatcher/libspeechd.h>

static SPDConnection *conn = NULL;
static config_t cfg;
static config_setting_t *voices;
static char *default_voice = NULL;
static signed int voice_rate = 0;
static int   Radio1_now = 0;
static int   Radio1_old = 0;
static XPLMDataRef   XPRADIO= NULL;
static int count,enable_random_voices = 0;

bool speech_init(void)
{
  conn = spd_open("XLinSpeak", "Main", "player", SPD_MODE_SINGLE);
  if(conn == NULL){
    xcDebug("XLinSpeak: Couldn't init speech!\n");

    return false;
  }

  //const char *str;

  config_init(&cfg);
  char *DirPatch;

  // Определяем путь к  текущей директории 
  DirPatch = get_current_dir_name ();

  const char* PluginPatch = "/Resources/plugins/xlinspeak/";
  const char* ConfigName = "xlinspeak.cfg";

  char* PluginConfigPatch = calloc(strlen(DirPatch) + strlen(PluginPatch) + strlen(ConfigName) + 1, 1);
  strcat(PluginConfigPatch, DirPatch);
  strcat(PluginConfigPatch, PluginPatch);
  strcat(PluginConfigPatch, ConfigName);
  

  /* Read the file. If there is an error, report it and exit. */
  if(! config_read_file(&cfg, PluginConfigPatch))
  {
    xcDebug("XLinSpeak: Error read config file!\n");
    config_destroy(&cfg);
    //return false;
  }
  else 
  {
    xcDebug("XLinSpeak: Config loaded...\n");
    voices = config_lookup(&cfg, "random_voices.r_voice");

    if(config_lookup_int(&cfg, "enable_random_voices", &enable_random_voices)){
      if(enable_random_voices == 1){
        xcDebug("XLinSpeak: Random voices ENABLED.\n");
        count = config_setting_length(voices);
        xcDebug("XLinSpeak: Total voices count = %d\n",count);
      }else
        xcDebug("XLinSpeak: Random voices DISABLED.\n");

    }

    
    XPRADIO = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_frequency_hz_833");        

    if(config_lookup_string(&cfg, "default_voice", &default_voice)){
      xcDebug("XLinSpeak: the synthesis voice set (%s)\n",default_voice);
      spd_set_synthesis_voice(conn, default_voice);
    }

    if(config_lookup_int(&cfg, "rate", &voice_rate)){
      xcDebug("XLinSpeak: the rate of the speech set (%d)\n",voice_rate);
      spd_set_voice_rate(conn, voice_rate);
    }



  }
  free(PluginConfigPatch);
  return true;
}

void speech_say(char *str)
{
  if(conn != NULL){
    if(enable_random_voices == 1){  
      if (XPRADIO != NULL){
        if(Radio1_old == 0)
          Radio1_old = XPLMGetDatai(XPRADIO);

        Radio1_now = XPLMGetDatai(XPRADIO);

        xcDebug("XLinSpeak: Radio1_now (%d)\n",Radio1_now);
        xcDebug("XLinSpeak: Radio1_old (%d)\n",Radio1_old);

        if(Radio1_now > 0 ){
          if(Radio1_now != Radio1_old){
              
              
              if(voices != NULL)
              {
                time_t t;
                srand((unsigned) time(&t));
                int r_voice = rand() % count-1;
                int i;
                xcDebug("XLinSpeak: r_voice (%d)\n",r_voice);
                
                for(i = 0; i < count; ++i)
                {
                  char *random_voice;
                  config_setting_t *voice_t = config_setting_get_elem(voices, i);

                  if(!(config_setting_lookup_string(voice_t, "title", &random_voice)))
                    xcDebug("XLinSpeak: Error read r_voice!\n");

                  if(i == r_voice){
                    xcDebug("XLinSpeak: Set random voice (%s)\n",random_voice);
                    spd_set_synthesis_voice(conn, random_voice);
                  }
                }
                Radio1_old = XPLMGetDatai(XPRADIO);
                
              }
            }
        }
    }
  }
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



