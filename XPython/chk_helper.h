#ifndef CHK_HELPER__H
#define CHK_HELPER__H

#include <XPLM/XPLMDataAccess.h>

  XPLMDataRef registerROAccessor(const char*name, int &val);
  XPLMDataRef registerROAccessor(const char*name, float &val);
  XPLMDataRef registerROAccessor(const char*name, double &val);
  XPLMDataRef registerROAccessor(const char*name, std::string &val);

#endif


