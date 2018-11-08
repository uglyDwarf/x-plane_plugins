
#include <map>
#include <string>
#include <iostream>
#include "chk_helper.h"
#include <math.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>

class val_c{
 public:
  val_c(){};
  virtual bool check_int(int &i)const{(void) i; return false;};
  virtual bool check_double(double &d)const{(void) d; return false;} 
  virtual bool check_string(std::string &s)const{(void) s; std::cout << "Wrong call" << std::endl; return false;};
};

class int_val : public val_c{
  int v;
 public:
  int_val(int i):v(i){};
  virtual bool check_int(int &i)const{return i == v;};
};

class double_val : public val_c{
  double v;
 public:
  double_val(double d):v(d){};
  virtual bool check_double(double &d)const{return fabs(d - v) < 1e-6;};
};

class str_val : public val_c{
  std::string v;
 public:
  str_val(std::string s):v(s){};
  virtual bool check_string(std::string &s)const{std::cout << v << " vs " << s << std::endl; return s == v;};
};

typedef std::map<std::string, val_c*> t_value_map;
static t_value_map values;


bool compare_str_value(char *name, char *str)
{
  std::string key(name);
  std::string val(str);
  std::cout << "Going to compare " << key << " to " << val << std::endl;
  t_value_map::const_iterator i = values.find(key);
  if(i == values.end()){
    std::cout << "Key not existing!" << std::endl;
    return false;
  }else{
    return i->second->check_string(val);
  }
}

bool compare_int_value(char *name, int val)
{
  std::string key(name);
  t_value_map::const_iterator i = values.find(key);
  if(i == values.end()){
    return false;
  }else{
    return i->second->check_int(val);
  }
}

bool compare_double_value(char *name, double val)
{
  std::string key(name);
  t_value_map::const_iterator i = values.find(key);
  if(i == values.end()){
    return false;
  }else{
    return i->second->check_double(val);
  }
}

void add_int_value(std::string name, int val)
{
  int_val *tmp = new int_val(val);
  values.insert(std::pair<std::string, val_c*>(name, tmp));
}

void add_str_value(std::string name, char *str)
{
  str_val *tmp = new str_val(std::string(str));
  values.insert(std::pair<std::string, val_c*>(name, tmp));
}

void add_double_value(std::string name, double val)
{
  double_val *tmp = new double_val(val);
  values.insert(std::pair<std::string, val_c*>(name, tmp));
}


/*
static int readInt(void *inRefcon)
{
  return *(static_cast<int *>(inRefcon));
}
*/

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



