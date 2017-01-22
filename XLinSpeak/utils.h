#ifndef UTILS__H
#define UTILS__H

#include <stdbool.h>
#include <stdarg.h>

bool speech_init(void);
void speech_say(char *str);
void speech_close(void);
void xcDebug(const char *format, ...);

#endif







