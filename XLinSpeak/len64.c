/**************************************************************
Read instruction (AMD64) to determine its length
**************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define IS_PREFIX 0x01
#define X66       (0x02 | IS_PREFIX)
#define X67       (0x04 | IS_PREFIX)
#define REX       (0x08 | IS_PREFIX)
#define REX_B     (0x10 | IS_PREFIX)
#define REX_X     (0x20 | IS_PREFIX)
#define REX_R     (0x40 | IS_PREFIX)
#define REX_W     (0x80 | IS_PREFIX)
#define WRONG     0x80000000

typedef struct{
  uint8_t modrm_mask;
  uint8_t modrm_val;
  int imm[2];
  int addr[2];
} t_modrm_variants;

typedef struct{
  //bit field
  int prefix;
  bool last;
  bool modrm;
  t_modrm_variants *variants;
} t_instr_info;

t_modrm_variants no_modrm[] = {{0x00, 0x00, {0, 0}, {0, 0}}};
t_modrm_variants modrm[] = {{0x00, 0x00, {0, 0}, {0, 0}}};
t_modrm_variants modrm_byte[] = {{0x00, 0x00, {1, 1}, {0, 0}}};


t_instr_info byte1[256] = {
  {WRONG, true, false, no_modrm}, //0x00
  {WRONG, true, false, no_modrm}, //0x01
  {WRONG, true, false, no_modrm}, //0x02
  {WRONG, true, false, no_modrm}, //0x03
  {WRONG, true, false, no_modrm}, //0x04
  {WRONG, true, false, no_modrm}, //0x05
  {WRONG, true, false, no_modrm}, //0x06
  {WRONG, true, false, no_modrm}, //0x07
  {WRONG, true, false, no_modrm}, //0x08
  {WRONG, true, false, no_modrm}, //0x09
  {WRONG, true, false, no_modrm}, //0x0A
  {WRONG, true, false, no_modrm}, //0x0B
  {WRONG, true, false, no_modrm}, //0x0C
  {WRONG, true, false, no_modrm}, //0x0D
  {WRONG, true, false, no_modrm}, //0x0E
  {WRONG, true, false, no_modrm}, //0x0F

  {WRONG, true, false, no_modrm}, //0x10
  {WRONG, true, false, no_modrm}, //0x11
  {WRONG, true, false, no_modrm}, //0x12
  {WRONG, true, false, no_modrm}, //0x13
  {WRONG, true, false, no_modrm}, //0x14
  {WRONG, true, false, no_modrm}, //0x15
  {WRONG, true, false, no_modrm}, //0x16
  {WRONG, true, false, no_modrm}, //0x17
  {WRONG, true, false, no_modrm}, //0x18
  {WRONG, true, false, no_modrm}, //0x19
  {WRONG, true, false, no_modrm}, //0x1A
  {WRONG, true, false, no_modrm}, //0x1B
  {WRONG, true, false, no_modrm}, //0x1C
  {WRONG, true, false, no_modrm}, //0x1D
  {WRONG, true, false, no_modrm}, //0x1E
  {WRONG, true, false, no_modrm}, //0x1F


  {WRONG, true, false, no_modrm}, //0x20
  {WRONG, true, false, no_modrm}, //0x21
  {WRONG, true, false, no_modrm}, //0x22
  {WRONG, true, false, no_modrm}, //0x23
  {WRONG, true, false, no_modrm}, //0x24
  {WRONG, true, false, no_modrm}, //0x25
  {WRONG, true, false, no_modrm}, //0x26
  {WRONG, true, false, no_modrm}, //0x27
  {WRONG, true, false, no_modrm}, //0x28
  {WRONG, true, false, no_modrm}, //0x29
  {WRONG, true, false, no_modrm}, //0x2A
  {WRONG, true, false, no_modrm}, //0x2B
  {WRONG, true, false, no_modrm}, //0x2C
  {WRONG, true, false, no_modrm}, //0x2D
  {WRONG, true, false, no_modrm}, //0x2E
  {WRONG, true, false, no_modrm}, //0x2F


  {WRONG, true, false, no_modrm}, //0x30
  {WRONG, true, false, no_modrm}, //0x31
  {WRONG, true, false, no_modrm}, //0x32
  {WRONG, true, false, no_modrm}, //0x33
  {WRONG, true, false, no_modrm}, //0x34
  {WRONG, true, false, no_modrm}, //0x35
  {WRONG, true, false, no_modrm}, //0x36
  {WRONG, true, false, no_modrm}, //0x37
  {WRONG, true, false, no_modrm}, //0x38
  {WRONG, true, false, no_modrm}, //0x39
  {WRONG, true, false, no_modrm}, //0x3A
  {WRONG, true, false, no_modrm}, //0x3B
  {WRONG, true, false, no_modrm}, //0x3C
  {WRONG, true, false, no_modrm}, //0x3D
  {WRONG, true, false, no_modrm}, //0x3E
  {WRONG, true, false, no_modrm}, //0x3F


 {REX, false, false, no_modrm}, //0x40
 {REX_B, false, false, no_modrm}, //0x41
 {REX_X, false, false, no_modrm}, //0x42
 {REX_X | REX_B, false, false, no_modrm}, //0x43
 {REX_R, false, false, no_modrm}, //0x44
 {REX_R | REX_B, false, false, no_modrm}, //0x45
 {REX_R | REX_X, false, false, no_modrm}, //0x46
 {REX_R | REX_X | REX_B, false, false, no_modrm}, //0x47
 {REX_W, false, false, no_modrm}, //0x48
 {REX_W | REX_B, false, false, no_modrm}, //0x49
 {REX_W | REX_X, false, false, no_modrm}, //0x4A
 {REX_W | REX_X | REX_B, false, false, no_modrm}, //0x4B
 {REX_W | REX_R, false, false, no_modrm}, //0x4C
 {REX_W | REX_R | REX_B, false, false, no_modrm}, //0x4D
 {REX_W | REX_R | REX_X, false, false, no_modrm}, //0x4E
 {REX_W | REX_R | REX_X | REX_B, false, false, no_modrm}, //0x4F


 {0x00, true, false, no_modrm}, //0x50
 {0x00, true, false, no_modrm}, //0x51
 {0x00, true, false, no_modrm}, //0x52
 {0x00, true, false, no_modrm}, //0x53
 {0x00, true, false, no_modrm}, //0x54
 {0x00, true, false, no_modrm}, //0x55
 {0x00, true, false, no_modrm}, //0x56
 {0x00, true, false, no_modrm}, //0x57
 {0x00, true, false, no_modrm}, //0x58
 {0x00, true, false, no_modrm}, //0x59
 {0x00, true, false, no_modrm}, //0x5A
 {0x00, true, false, no_modrm}, //0x5B
 {0x00, true, false, no_modrm}, //0x5C
 {0x00, true, false, no_modrm}, //0x5D
 {0x00, true, false, no_modrm}, //0x5E
 {0x00, true, false, no_modrm}, //0x5F


  {WRONG, true, false, no_modrm}, //0x60
  {WRONG, true, false, no_modrm}, //0x61
  {WRONG, true, false, no_modrm}, //0x62
  {WRONG, true, false, no_modrm}, //0x63
  {WRONG, true, false, no_modrm}, //0x64
  {WRONG, true, false, no_modrm}, //0x65
  {WRONG, true, false, no_modrm}, //0x66
  {WRONG, true, false, no_modrm}, //0x67
  {WRONG, true, false, no_modrm}, //0x68
  {WRONG, true, false, no_modrm}, //0x69
  {WRONG, true, false, no_modrm}, //0x6A
  {WRONG, true, false, no_modrm}, //0x6B
  {WRONG, true, false, no_modrm}, //0x6C
  {WRONG, true, false, no_modrm}, //0x6D
  {WRONG, true, false, no_modrm}, //0x6E
  {WRONG, true, false, no_modrm}, //0x6F


  {WRONG, true, false, no_modrm}, //0x70
  {WRONG, true, false, no_modrm}, //0x71
  {WRONG, true, false, no_modrm}, //0x72
  {WRONG, true, false, no_modrm}, //0x73
  {WRONG, true, false, no_modrm}, //0x74
  {WRONG, true, false, no_modrm}, //0x75
  {WRONG, true, false, no_modrm}, //0x76
  {WRONG, true, false, no_modrm}, //0x77
  {WRONG, true, false, no_modrm}, //0x78
  {WRONG, true, false, no_modrm}, //0x79
  {WRONG, true, false, no_modrm}, //0x7A
  {WRONG, true, false, no_modrm}, //0x7B
  {WRONG, true, false, no_modrm}, //0x7C
  {WRONG, true, false, no_modrm}, //0x7D
  {WRONG, true, false, no_modrm}, //0x7E
  {WRONG, true, false, no_modrm}, //0x7F


  {WRONG, true, false, no_modrm}, //0x80
  {WRONG, true, false, no_modrm}, //0x81
  {WRONG, true, false, no_modrm}, //0x82
 {0x00, true, true, modrm_byte}, //0x83
  {WRONG, true, false, no_modrm}, //0x84
  {WRONG, true, false, no_modrm}, //0x85
  {WRONG, true, false, no_modrm}, //0x86
  {WRONG, true, false, no_modrm}, //0x87
  {WRONG, true, false, no_modrm}, //0x88
 {0x00, true, true, modrm}, //0x89
  {WRONG, true, false, no_modrm}, //0x8A
  {WRONG, true, false, no_modrm}, //0x8B
  {WRONG, true, false, no_modrm}, //0x8C
 {0x00, true, true, modrm}, //0x8D
  {WRONG, true, false, no_modrm}, //0x8E
  {WRONG, true, false, no_modrm}, //0x8F


 {0x00, true, false, no_modrm}, //0x90
  {WRONG, true, false, no_modrm}, //0x91
  {WRONG, true, false, no_modrm}, //0x92
  {WRONG, true, false, no_modrm}, //0x93
  {WRONG, true, false, no_modrm}, //0x94
  {WRONG, true, false, no_modrm}, //0x95
  {WRONG, true, false, no_modrm}, //0x96
  {WRONG, true, false, no_modrm}, //0x97
  {WRONG, true, false, no_modrm}, //0x98
  {WRONG, true, false, no_modrm}, //0x99
  {WRONG, true, false, no_modrm}, //0x9A
  {WRONG, true, false, no_modrm}, //0x9B
  {WRONG, true, false, no_modrm}, //0x9C
  {WRONG, true, false, no_modrm}, //0x9D
  {WRONG, true, false, no_modrm}, //0x9E
  {WRONG, true, false, no_modrm}, //0x9F


  {WRONG, true, false, no_modrm}, //0xA0
  {WRONG, true, false, no_modrm}, //0xA1
  {WRONG, true, false, no_modrm}, //0xA2
  {WRONG, true, false, no_modrm}, //0xA3
  {WRONG, true, false, no_modrm}, //0xA4
  {WRONG, true, false, no_modrm}, //0xA5
  {WRONG, true, false, no_modrm}, //0xA6
  {WRONG, true, false, no_modrm}, //0xA7
  {WRONG, true, false, no_modrm}, //0xA8
  {WRONG, true, false, no_modrm}, //0xA9
  {WRONG, true, false, no_modrm}, //0xAA
  {WRONG, true, false, no_modrm}, //0xAB
  {WRONG, true, false, no_modrm}, //0xAC
  {WRONG, true, false, no_modrm}, //0xAD
  {WRONG, true, false, no_modrm}, //0xAE
  {WRONG, true, false, no_modrm}, //0xAF


  {WRONG, true, false, no_modrm}, //0xB0
  {WRONG, true, false, no_modrm}, //0xB1
  {WRONG, true, false, no_modrm}, //0xB2
  {WRONG, true, false, no_modrm}, //0xB3
  {WRONG, true, false, no_modrm}, //0xB4
  {WRONG, true, false, no_modrm}, //0xB5
  {WRONG, true, false, no_modrm}, //0xB6
  {WRONG, true, false, no_modrm}, //0xB7
  {WRONG, true, false, no_modrm}, //0xB8
  {WRONG, true, false, no_modrm}, //0xB9
  {WRONG, true, false, no_modrm}, //0xBA
  {WRONG, true, false, no_modrm}, //0xBB
  {WRONG, true, false, no_modrm}, //0xBC
  {WRONG, true, false, no_modrm}, //0xBD
  {WRONG, true, false, no_modrm}, //0xBE
  {WRONG, true, false, no_modrm}, //0xBF


  {WRONG, true, false, no_modrm}, //0xC0
  {WRONG, true, false, no_modrm}, //0xC1
  {WRONG, true, false, no_modrm}, //0xC2
  {WRONG, true, false, no_modrm}, //0xC3
  {WRONG, true, false, no_modrm}, //0xC4
  {WRONG, true, false, no_modrm}, //0xC5
  {WRONG, true, false, no_modrm}, //0xC6
  {WRONG, true, false, no_modrm}, //0xC7
  {WRONG, true, false, no_modrm}, //0xC8
  {WRONG, true, false, no_modrm}, //0xC9
  {WRONG, true, false, no_modrm}, //0xCA
  {WRONG, true, false, no_modrm}, //0xCB
  {WRONG, true, false, no_modrm}, //0xCC
  {WRONG, true, false, no_modrm}, //0xCD
  {WRONG, true, false, no_modrm}, //0xCE
  {WRONG, true, false, no_modrm}, //0xCF


  {WRONG, true, false, no_modrm}, //0xD0
  {WRONG, true, false, no_modrm}, //0xD1
  {WRONG, true, false, no_modrm}, //0xD2
  {WRONG, true, false, no_modrm}, //0xD3
  {WRONG, true, false, no_modrm}, //0xD4
  {WRONG, true, false, no_modrm}, //0xD5
  {WRONG, true, false, no_modrm}, //0xD6
  {WRONG, true, false, no_modrm}, //0xD7
  {WRONG, true, false, no_modrm}, //0xD8
  {WRONG, true, false, no_modrm}, //0xD9
  {WRONG, true, false, no_modrm}, //0xDA
  {WRONG, true, false, no_modrm}, //0xDB
  {WRONG, true, false, no_modrm}, //0xDC
  {WRONG, true, false, no_modrm}, //0xDD
  {WRONG, true, false, no_modrm}, //0xDE
  {WRONG, true, false, no_modrm}, //0xDF


  {WRONG, true, false, no_modrm}, //0xE0
  {WRONG, true, false, no_modrm}, //0xE1
  {WRONG, true, false, no_modrm}, //0xE2
  {WRONG, true, false, no_modrm}, //0xE3
  {WRONG, true, false, no_modrm}, //0xE4
  {WRONG, true, false, no_modrm}, //0xE5
  {WRONG, true, false, no_modrm}, //0xE6
  {WRONG, true, false, no_modrm}, //0xE7
  {WRONG, true, false, no_modrm}, //0xE8
  {WRONG, true, false, no_modrm}, //0xE9
  {WRONG, true, false, no_modrm}, //0xEA
  {WRONG, true, false, no_modrm}, //0xEB
  {WRONG, true, false, no_modrm}, //0xEC
  {WRONG, true, false, no_modrm}, //0xED
  {WRONG, true, false, no_modrm}, //0xEE
  {WRONG, true, false, no_modrm}, //0xEF


  {WRONG, true, false, no_modrm}, //0xF0
  {WRONG, true, false, no_modrm}, //0xF1
  {WRONG, true, false, no_modrm}, //0xF2
  {WRONG, true, false, no_modrm}, //0xF3
  {WRONG, true, false, no_modrm}, //0xF4
  {WRONG, true, false, no_modrm}, //0xF5
  {WRONG, true, false, no_modrm}, //0xF6
  {WRONG, true, false, no_modrm}, //0xF7
  {WRONG, true, false, no_modrm}, //0xF8
  {WRONG, true, false, no_modrm}, //0xF9
  {WRONG, true, false, no_modrm}, //0xFA
  {WRONG, true, false, no_modrm}, //0xFB
  {WRONG, true, false, no_modrm}, //0xFC
  {WRONG, true, false, no_modrm}, //0xFD
  {WRONG, true, false, no_modrm}, //0xFE
  {WRONG, true, false, no_modrm}, //0xFF
};

t_instr_info *opcodes[] = {
  byte1
};



int read_instruction64(uint8_t *ptr)
{
  uint8_t *cur = ptr;
  bool op_size_override = false;
  bool addr_size_override = false;
  bool get_modrm = false;
  bool get_sib = false;
  int displacement = 0;
  int imm = 0;
  int addr = 0;
  int mod = 0, rm = 0;
  int opcode_byte = 0;
  t_instr_info *info;
  t_modrm_variants *var_info;

  while(1){
    info = &(opcodes[opcode_byte][*cur]);
    if(info->prefix & WRONG){
      return -1;
    }
    ++cur;
    if(!(info -> prefix & IS_PREFIX)){
      opcode_byte += 1;
    }else{
      if(info->prefix & X67){
        addr_size_override = true;
      }else if(info->prefix & X66){
        op_size_override = true;
      }
    }
    if(info->last){
      get_modrm = info->modrm;

      var_info = info->variants;
      if(get_modrm){
        while((*cur & var_info->modrm_mask) != var_info->modrm_val){
          ++var_info;
        }
      }

      imm += op_size_override ? var_info->imm[1] : var_info->imm[0];
      addr += addr_size_override ? var_info->addr[1] : var_info->addr[0];
      break;
    }
  }

  // mod reg  rm
  //  xx xxx xxx
  if(get_modrm){
    //not interested in the reg operand
    mod = *cur >> 6;
    rm  = *cur & 0x07;
    printf("    Modrm: %02X\n", *cur);
    ++cur;
    if(addr_size_override){
      //16 bit ModR/M
      if((mod == 0x00) && (rm == 0x06)){
        displacement = 2;
      }else if(mod == 0x01){
        displacement = 1;
      }else if(mod == 0x02){
        displacement = 2;
      }
    }else{
      //32/64 bit ModR/M
      if((rm == 0x04) && (mod != 0x03)){
        get_sib = true;
      }
      if((mod == 0) && (rm == 0x05)){
        displacement = 4;
      }
      if(mod == 0x01){
        displacement = 1;
      }else if(mod == 0x02){
        displacement = 4;
      }
    }
  }
  if(get_sib){
    uint8_t sib = *cur;
    ++cur;
    if(((sib & 0x07) == 0x05) && (mod == 0x00)){
      displacement += 4;
    }
  }
  printf("    Extra: %d (displacement), %d (immediate)\n", displacement, imm);
  cur += displacement + imm + addr;
  return cur - ptr;
}

/*
int main(int argc, char *argv[])
{
  printf("Hello World!\n");
  uint8_t pokus[] = {0x55, 
                     0x89, 0xd5,
                     0x53,
                     0x48, 0x89, 0xfb,
                     0x48, 0x8d, 0x7f, 0x38,
                     0x48, 0x83, 0xec, 0x08,

                     0x55,
                     0x41, 0x56,
                     0x53,
                     0x41, 0x89, 0xd6,
                     0x48, 0x89, 0xfb,
                     0x48, 0x8d, 0x6b, 0x10,
                     0x90, 0x90, 0x90, 0x90};
                     
  uint8_t *ptr = pokus;
  int tmp;
  while(ptr < pokus + sizeof(pokus)){
    tmp = read_instruction64(ptr);
    if(tmp < 0){
      printf("Problem!\n");
      return 1;
    }
    printf("%llX -> %d\n", (intptr_t)ptr, tmp);
    ptr += tmp;
  } 
  return 0;
}
*/



