#ifndef CHK_HELPER__H
#define CHK_HELPER__H

#ifdef __cplusplus
extern "C" {
#endif

  bool compare_str_value(char *name, char *str);
  bool compare_int_value(char *name, int val);
  bool compare_double_value(char *name, double val);

#ifdef __cplusplus
  void add_str_value(std::string name, char *str);
  void add_int_value(std::string name, int val);
  void add_double_value(std::string name, double val);
} //extern "C"

  void registerROAccessor(const char*name, int &val);
  void registerROAccessor(const char*name, float &val);
  void registerROAccessor(const char*name, double &val);
  void registerROAccessor(const char*name, std::string &val);

#endif

#endif


