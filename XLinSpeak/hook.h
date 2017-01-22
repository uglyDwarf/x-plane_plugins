#ifndef HOOK__H
#define HOOK__H

  #include <stdbool.h>
  
  bool hook(void *proceduura, int n);
  bool speech_init(void);
  void speech_test(void);
  void speech_close(void);

  void xcDebug(const char *format, ...);

#endif
