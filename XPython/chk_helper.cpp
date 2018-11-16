
#include <map>
#include <string>
#include <iostream>
#include "chk_helper.h"
#include <math.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>

template <class T> static T readVal(void *inRefcon)
{
  return *(static_cast<T*>(inRefcon));
}

static int readData(void *inRefcon, void *outValue, int inOffset, int inLength)
{
  std::string &str = *(static_cast<std::string *>(inRefcon));
  size_t len = str.size();
  if(outValue == NULL){
    return static_cast<int>(len);
  }
  char *tgt = static_cast<char *>(outValue);
  const char *src = str.c_str();
  size_t i;
  for(i = 0; i < (size_t)inLength; ++i){
    if((size_t)inOffset + i >= len){
      break;
    }
    tgt[i] = src[inOffset + i];
  }
  return i; 
}


XPLMDataRef registerROAccessor(const char*name, int &val)
{
  return XPLMRegisterDataAccessor(name, xplmType_Int, 0, 
    readVal<int>, NULL, 
    NULL, NULL, 
    NULL, NULL, 
    NULL, NULL, 
    NULL, NULL,
    NULL, NULL,
    &val, NULL);
}

XPLMDataRef registerROAccessor(const char*name, float &val)
{
  return XPLMRegisterDataAccessor(name, xplmType_Float, 0, 
    NULL, NULL, 
    readVal<float>, NULL, 
    NULL, NULL, 
    NULL, NULL, 
    NULL, NULL,
    NULL, NULL,
    &val, NULL);
}

XPLMDataRef registerROAccessor(const char*name, double &val)
{
  return XPLMRegisterDataAccessor(name, xplmType_Double, 0, 
    NULL, NULL, 
    NULL, NULL, 
    readVal<double>, NULL, 
    NULL, NULL, 
    NULL, NULL,
    NULL, NULL,
    &val, NULL);
}


XPLMDataRef registerROAccessor(const char*name, std::string &val)
{
  return XPLMRegisterDataAccessor(name, xplmType_Data, 0, 
    NULL, NULL, 
    NULL, NULL, 
    NULL, NULL, 
    NULL, NULL, 
    NULL, NULL,
    readData, NULL,
    &val, NULL);
}



