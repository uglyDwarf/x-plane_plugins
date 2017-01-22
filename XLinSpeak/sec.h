#ifndef SEC__H
#define SEC__H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool locate_tables(void);

struct function_ptrs{
  const char *name;
  uint64_t address;
  int hook;
};

bool find_functions(struct function_ptrs *ptrs, int funcs);

#ifdef __cplusplus
}
#endif


#endif
