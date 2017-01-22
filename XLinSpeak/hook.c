#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <errno.h>
#include "len.h"
#include "utils.h"

extern uint8_t trampoline1;
extern uint8_t trampoline2;

const intptr_t mask = (intptr_t)~(4095L);

void *get_page_start(void *addr)
{
  if((intptr_t)((intptr_t)addr & mask) == (intptr_t)addr){
    return addr;
  }
  return (void *)((intptr_t)addr & mask);
}

bool change_range_prot(void *addr, size_t len, bool wr_prot)
{
  int res = 0;
  int prot = PROT_READ | PROT_EXEC;
  prot |= wr_prot ? PROT_WRITE : 0;
  void *start_page = get_page_start(addr);
  void *end_page = get_page_start(addr + len);
  res = mprotect((void *)start_page, 4096, prot);
  if(start_page != end_page){
    res |= mprotect((void *)end_page, 4096, prot);
  }
  if(res != 0){
    perror("Mprotect");
  }
  return res == 0;
}

extern void *hook1;
extern void *hook2;


int hook_proc(void *proc_addr, int copy_bytes, 
              void *hook_addr, uint8_t *trampoline)
{
  if(proc_addr == NULL){
    return 1;
  }
  uint8_t *proc_start = (uint8_t *)proc_addr;
  int i;
  //Copy enough off the function start, so
  //  we can hook it
  for(i = 0; i < copy_bytes; ++i){
    trampoline[i] = proc_start[i];
  }

  //Make code writeable
  if(!change_range_prot(proc_addr, copy_bytes, true)){
    printf("Can't unprotect proc range.\n");
    return -1;
  }
#if __x86_64__
  // "Jump" to the hook
  // The hook jump sequence looks like this:
  //   push rax ; will be ballanced later!
  //   mov rax, hook_addr
  //   push rax
  //   ret
  proc_start[0] = 0x50; //push rax
  proc_start[1] = 0x48; //mov rax, imm64
  proc_start[2] = 0xb8;
  *(uint64_t *)(&proc_start[3]) = (uint64_t)hook_addr;
  proc_start[11] = 0x50; //push rax
  proc_start[12] = 0xC3; //ret

  intptr_t tmp = (intptr_t)proc_start + i;
  // Trampoline has the following layout:
  //   ... ;instructions copied from the begining
  //       ; of the hooked functions
  //   push low_dword(func_rest) ; but actually 
  //                             ;pushes 8 bytes!
  //   mov [esp + 4] high_dword(func_rest)
  //   ret
  trampoline[i] = 0x68; //push ...
  *(uint32_t *)(&trampoline[i + 1]) = tmp & 0xFFFFFFFF;
  *(uint32_t *)(&trampoline[i + 5]) = 0x042444C7; //mov ...
  *(uint32_t *)(&trampoline[i + 9]) = tmp >> 32;
  trampoline[i + 13] = 0xC3; //ret

  return i + 14;
#else
  // "Jump" to the hook
  // Need 6 bytes...
  proc_start[0] = 0xE9; //jump rel addr32
  *(uint32_t *)(&proc_start[1]) = (uint32_t)hook_addr - (uint32_t)proc_start - 5;
  //Make code read only again
  if(!change_range_prot(proc_addr, 5, false)){
    printf("Can't reprotect proc range.\n");
    return -1;
  }

  trampoline[i] = 0xE9;
  *(uint32_t *)(&trampoline[i + 1]) = ((uint32_t)proc_start + i) - (uint32_t)&trampoline[i + 5];
  return i + 5;
#endif
}


//mimicks XP10's soun_class::SPEECH_speakstring(std::string, speech_type, int)
void kuk1(void *this, char **str, int type, int i)
{
  (void) this;
  (void) str;
  (void) type;
  (void) i;
  //printf("Kecal: %s\n", *str);
  speech_say(*str);
}

//Mimicks XP11's spch_class::SPEECH_speakstring(std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >, speech_type, int)
void kuk2(void *this, char *str, int type, int i)
{
  (void) this;
  (void) str;
  (void) type;
  (void) i;

  char *ptr;
  //Different functions pass different style strings
  if(*(uint8_t*)str & 1){
    ptr = *(char **)(str+16);
  }else{
    ptr = str + 1;
  }
  //printf("Kecal: >>>%s<<<\n", ptr);
  speech_say(ptr);
}


int get_hook_space(void *ptr)
{
  int safe;
#if __x86_64__
  safe = 13;
#else
  safe = 5;
#endif
  uint8_t *current = (uint8_t *)ptr;
  int d = 0;
  int tmp;
  do{
    #if __x86_64__
    tmp = read_instruction64(current);
    #else
    tmp = read_instruction32(current);
    #endif
    //printf("%p: %d\n", (void*)current, tmp);
    if(tmp < 0){
      return -1;
    }
    d += tmp;
    current += tmp;
  }while(d < safe);
  //printf("Safe: %d\n", d);
  return d;
}




void *hooks[2] = {&hook1, &hook2};
uint8_t *trampolines[2] = {&trampoline1, &trampoline2};

bool hook(void *proceduura, int n)
{
  int copy = get_hook_space(proceduura);
  if(copy <= 0){
    return false;
  }

  if(!change_range_prot(trampolines[n], 128, true)){
    printf("Can't unprotect buffer!\n");
    return false;
  }

  if(hook_proc(proceduura, copy, hooks[n], trampolines[n]) != 0){
    return true;
  }
  return false;
}


