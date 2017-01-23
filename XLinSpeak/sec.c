/******************************************************************************
Searches current executable for given functions 
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "sec.h"
#include "utils.h"

struct commonELF_header{
  uint32_t magic; //0x464C457F ("\07fELF")
  uint8_t bitness; //1 - 32bit, 2 - 64bit
  uint8_t endianness; //1 - little, 2 - big
  uint8_t elf_version ;
  uint8_t os_abi; //usually 0 for SysV
  uint8_t padding[8];
  uint16_t type; //1 - relocatable, 2 - executable, 3 - shared, 4 - core
  uint16_t instr_set; //0 - unknown, 2 - sparc, 3 - x86, 8 - MIPS, 0x14 PowerPC
                      // 0x28 - ARM, 0x2A - SuperH, 0x32 - IA-64, 0x32 - x86-64
                      // 0xB7 - AArch64
}__attribute__((packed));

struct ELF32_header{
  uint32_t version;
  uint32_t entry_point;
  uint32_t program_header_table_pos;
  uint32_t section_header_table_pos;
  uint32_t flags;
  uint16_t header_size;
  uint16_t pht_entry_size;
  uint16_t pht_entries;
  uint16_t sht_entry_size;
  uint16_t sht_entries;
  uint16_t sht_names_index;
}__attribute__((packed));

struct ELF64_header{
  uint32_t version;
  uint64_t entry_point;
  uint64_t program_header_table_pos;
  uint64_t section_header_table_pos;
  uint32_t flags;
  uint16_t header_size;
  uint16_t pht_entry_size;
  uint16_t pht_entries;
  uint16_t sht_entry_size;
  uint16_t sht_entries;
  uint16_t sht_names_index;
}__attribute__((packed));


struct PROGRAM32_header{
  uint32_t type;
  uint32_t offset;
  uint32_t vaaddr;
  uint32_t paddr;
  uint32_t filesz;
  uint32_t memsz;
  uint32_t flags;
  uint32_t align;
}__attribute__((packed));

struct PROGRAM64_header{
  uint32_t type;
  uint32_t offset;
  uint32_t vaaddr;
  uint32_t paddr;
  uint32_t filesz;
  uint32_t memsz;
  uint32_t flags;
  uint32_t align;
}__attribute__((packed));

struct SECTION32_header{
  uint32_t name; //offset to string in .shstrtab section
  uint32_t type; //0x1 program data, 0x2 symbol table, 0x3 string table, 0xb dynamic symbol table
  uint32_t flags;
  uint32_t addr; //va address for loaded sections
  uint32_t offset; // offset of the section in the file image
  uint32_t size; // size in the file image (can be 0)
  uint32_t link; //section index? 
  uint32_t info; //extra info
  uint32_t addralign; //must be power of two
  uint32_t entsize;   //size of entries when fixed size otherwise 0
}__attribute__((packed));

struct SECTION64_header{
  uint32_t name;
  uint32_t type;
  uint64_t flags;
  uint64_t addr;
  uint64_t offset;
  uint64_t size;
  uint32_t link;
  uint32_t info;
  uint64_t addralign;
  uint64_t entsize;
}__attribute__((packed));

#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define ST_FUNC 2

struct symbols_info{
  uint8_t *symbol_table;
  char    *strings;
  size_t   size;
  int      bits;
}symbols_info;

static char exe[2048];

static uint8_t *read_section(FILE *f, size_t offset, size_t size)
{
  uint8_t *ptr = (uint8_t *)malloc(size);
  if(ptr == NULL){
    xcDebug("XLinSpeak: Couldn't malloc %lud bytes.\n", (long unsigned int)size);
    return NULL;
  }
  if(fseek(f, offset, SEEK_SET) != 0){
    xcDebug("XLinSpeak: Problem seeking to the section pos (%luX\n)\n", (long unsigned int)offset);
    free(ptr);
    return NULL;
  }
  if(fread(ptr, 1, size, f) != size){
    xcDebug("XLinSpeak: Couldn't read the whole section(offset: %luX, length: %lud\n).\n", (long unsigned int)offset, (long unsigned int)size);
    free(ptr);
    return NULL;
  }
  return ptr;
}


static bool parse_elf(FILE *f)
{
  struct commonELF_header eh;
  struct ELF32_header eh32;
  struct ELF64_header eh64;
  struct SECTION32_header sh32;
  struct SECTION64_header sh64;
  int i;

  if(fread(&eh, sizeof(eh), 1, f) != 1){
    xcDebug("XLinSpeak: Can't read common ELF header!\n");
    return false;
  }

  if(eh.bitness == 1){ //32 bit
    symbols_info.bits = 32;
    if(fread(&eh32, sizeof(eh32), 1, f) != 1){
      xcDebug("XLinSpeak: Can't read 32bit ELF header!\n");
      return false;
    }
    xcDebug("XLinSpeak: Entry point: 0x%08X\n", eh32.entry_point);
    xcDebug("XLinSpeak: Flags: %08X\n", eh32.flags);
    xcDebug("XLinSpeak: Section headers entries: %d\n", eh32.sht_entries);

    //skip over program header tables
    long anchor = eh32.section_header_table_pos;
    for(i = 0; i < eh32.sht_entries; ++i){
      fseek(f, anchor + i * eh32.sht_entry_size, SEEK_SET);
      if(fread(&sh32, sizeof(sh32), 1, f) != 1){
        xcDebug("XLinSpeak: Problem reading section header\n");
        return false;
      }
      //xcDebug("XLinSpeak: Section offset: %08X\n", sh32.offset);
      switch(sh32.type){
        case SHT_SYMTAB:
          symbols_info.symbol_table = read_section(f, sh32.offset, sh32.size);
          symbols_info.size = sh32.size;
          break;
        case SHT_STRTAB:
          symbols_info.strings = (char *)read_section(f, sh32.offset, sh32.size);
        default:
          break;
      }
    }
  }else{ //64 bit
    if(fread(&eh64, sizeof(eh64), 1, f) != 1){
      xcDebug("XLinSpeak: Can't read 64bit ELF header!\n");
      return false;
    }
    symbols_info.bits = 64;
    xcDebug("XLinSpeak: Entry point: 0x%08luX\n", (long unsigned int)eh64.entry_point);
    xcDebug("XLinSpeak: Flags: %08X\n", eh64.flags);
    xcDebug("XLinSpeak: Section headers entries: %d\n", eh64.sht_entries);

    //skip over program header tables
    long anchor = eh64.section_header_table_pos;
    for(i = 0; i < eh64.sht_entries; ++i){
      fseek(f, anchor + i * eh64.sht_entry_size, SEEK_SET);
      if(fread(&sh64, sizeof(sh64), 1, f) != 1){
        xcDebug("XLinSpeak: Problem reading section header\n");
        return false;
      }
      //xcDebug("XLinSpeak: Section offset: %08lX\n", (long unsigned int)sh64.offset);
      switch(sh64.type){
        case SHT_SYMTAB:
          symbols_info.symbol_table = read_section(f, sh64.offset, sh64.size);
          symbols_info.size = sh64.size;
          xcDebug("XLinSpeak: Symtab @ %lX\n", (long unsigned int)sh64.offset);
          break;
        case SHT_STRTAB:
          symbols_info.strings = (char *)read_section(f, sh64.offset, sh64.size);
        default:
          break;
      }
    }
  }
  
  return true;
}

bool locate_tables(void)
{
  pid_t pid = getpid();
  snprintf(exe, sizeof(exe), "/proc/%d/exe", (int)pid);
  FILE *f = fopen(exe, "r");
  if(f == NULL){
    perror("Open: ");
    return false;
  }
  if(!parse_elf(f)){
    xcDebug("XLinSpeak: Can't parse elf.\n");
    return false;
  }
  if((symbols_info.symbol_table== NULL) || (symbols_info.strings == NULL)){
    xcDebug("XLinSpeak: Problem loading tables.\n");
    return false;
  }
  return true;
}


struct symbol32{
  uint32_t name;
  uint32_t value;
  uint32_t size;
  uint8_t info;
  uint8_t other;
  uint16_t sec_index;
}__attribute__((packed));

struct symbol64{
  uint32_t name;
  uint8_t info;
  uint8_t other;
  uint16_t index;
  uint64_t value;
  uint64_t size;
}__attribute__((packed));


bool find_functions(struct function_ptrs *ptrs, int funcs)
{
  xcDebug("XLinSpeak: %d functions to check.\n", funcs);
  if((symbols_info.symbol_table == NULL) || (symbols_info.strings == NULL)){
    xcDebug("XLinSpeak: Load tables first.\n");
    return false;
  }
  long i;
  int j;
  if(symbols_info.bits == 32){
    struct symbol32 *sym_ptr = (struct symbol32 *) (symbols_info.symbol_table);
    long records = symbols_info.size / sizeof(struct symbol32);
    //xcDebug("XLinSpeak: Size: %lud, len: %lud, rec: %ld\n", (long unsigned int)symbols_info.size, (long unsigned int)sizeof(struct symbol32), records);
    for(i = 0; i < records; ++i){
      if((sym_ptr[i].name != 0) && ((sym_ptr[i].info & 0x0f) == ST_FUNC) && (sym_ptr[i].size > 0)){
        char *name = sym_ptr[i].name + symbols_info.strings;
        for(j = 0; j < funcs; ++j){
          if((ptrs[j].address == 0) && (strcmp(name, ptrs[j].name) == 0)){
            ptrs[j].address = (uint64_t)sym_ptr[i].value;
            xcDebug("XLinSpeak: Symbol %s -> %08X\n", name, sym_ptr[i].value);
          }
        }
      }
    }
  }else{
    struct symbol64 *sym_ptr = (struct symbol64 *) (symbols_info.symbol_table);
    long records = symbols_info.size / sizeof(struct symbol64);
    //xcDebug("XLinSpeak: Size: %lud, len: %lud, rec: %ld\n", (long unsigned int)symbols_info.size, (long unsigned int)sizeof(struct symbol64), records);
    for(i = 0; i < records; ++i){
      //xcDebug("XLinSpeak: addr: %lX, str: %X\n", sym_ptr[i].value, sym_ptr[i].name);
      if((sym_ptr[i].name != 0) && ((sym_ptr[i].info & 0x0f) == ST_FUNC) && (sym_ptr[i].size > 0)){
        char *name = sym_ptr[i].name + symbols_info.strings;
        for(j = 0; j < funcs; ++j){
          if((ptrs[j].address == 0) && (strcmp(name, ptrs[j].name) == 0)){
            ptrs[j].address = (uint64_t)sym_ptr[i].value;
            xcDebug("XLinSpeak: Symbol %s -> %lX\n", name, (long unsigned int)sym_ptr[i].value);
          }
        }
      }
    }
  }
  return true;
}

/*
int main(int argc, char *argv[])
{
  xcDebug("XLinSpeak: Hello World!\n");

  if(argc != 2){
    xcDebug("XLinSpeak: Provide a name of file to check.\n");
    return 0;
  }
  char *fname = argv[1];
  xcDebug("XLinSpeak: Opening '%s'.\n", fname);
  if((f = fopen(fname, "rb")) == NULL){
    xcDebug("XLinSpeak: Can't open\n");
  }
  parse_elf();
  return 0;
}
*/
