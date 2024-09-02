#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "utils.h"

bool running = true;
bool error = false;
uint32_t registers[32] = {0};
uint32_t pc = 0, pbrk = 0;
uint32_t cycle = 0;
char mnemonic[32];

static inline uint32_t regr(uint8_t i) {
  return i ? registers[i] : 0;
}
static inline void regw(uint32_t i, uint32_t val) {
  registers[i] = val;
}

// TODO: Allocate memory dynamically
#define MEMORY_LIMIT 4 * 1024 * 1024
uint8_t mem[MEMORY_LIMIT] = { 0 };

static inline uint32_t mr(uint32_t address) {
  return (address < (MEMORY_LIMIT)) ? *((uint32_t*) &mem[address]) : 0;
}
static inline void mw(uint32_t address, uint32_t val) {
  *((uint32_t*) &mem[address]) = val;
}

void print_log(uint32_t instruction, uint32_t rd_value, char* mnemonic) {
  fprintf(stderr, "PC=%08X [%08x] x%02u=%08X x%02u=%08X x%02u=%08X %s\n", 
         pc, instruction, RD(instruction), rd_value, 
         RS1(instruction), regr(RS1(instruction)), 
         RS2(instruction), regr(RS2(instruction)),
         mnemonic);
}

static inline void syscall() {
  uint32_t number = regr(17);
  uint32_t a0 = regr(10);
  uint32_t a1 = regr(11);
  uint32_t a2 = regr(12);
  uint32_t a3 = regr(13);
  uint32_t a4 = regr(14);
  uint32_t a5 = regr(15);
  uint32_t a6 = regr(16);
  switch (number)
  {
  case 64: // write
    // printf("write(%i, %x, %u)\n", a0, a1, a2);
    if (a0 == 1) {
      fwrite(mem + a1, 1, a2, stdout);
      regw(10, a2);
    } else {
      fprintf(stderr, "Error: Unimplemented write behavior.\n");
      running = false;
      error = true;
    }
    break;
  case 80: // fstat
    if(a0 == 1) {
      memcpy(mem + a1, &stdout_stat, 88);
    } else {
      fprintf(stderr, "Error: Unimplemented fstat behavior.\n");
      running = false;
    }
    break;
  case 93: // exit
    running = false;
    break;
  case 214: // brk
    if (a0 == 0) {
      regw(10, pbrk);
    } else {
      regw(10, pbrk + a0);
      // fprintf(stderr, "Error: Unimplemented brk behavior.\n");
    }
    break;
  case 403:
    exit(1);
    break;
  default:
    fprintf(stderr, "Error: Unimplemented syscall number %u.\n", number);
    printf("%u %u %u %u %u %u %u\n", a0, a1, a2, a3, a4, a5, a6);
    running = false;
    break;
  } 
}

static inline void addi(uint32_t i) {
  sprintf(mnemonic, "addi    %s,%s,%i", reg_names[RD(i)], reg_names[RS1(i)], sext(IMMI(i), 12));
  print_log(i, (int32_t) regr(RS1(i)) + (int32_t) sext(IMMI(i), 12), mnemonic);
  regw(RD(i), (int32_t) regr(RS1(i)) + (int32_t) sext(IMMI(i), 12));
}
static inline void add(uint32_t i) {
  sprintf(mnemonic, "add     %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, (int32_t) regr(RS1(i)) + (int32_t) regr(RS2(i)), mnemonic);
  regw(RD(i), (int32_t) regr(RS1(i)) + (int32_t) regr(RS2(i)));
}
static inline void sub(uint32_t i) {
  sprintf(mnemonic, "sub     %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, (int32_t) regr(RS1(i)) - (int32_t) regr(RS2(i)), mnemonic);
  regw(RD(i), (int32_t) regr(RS1(i)) - (int32_t) regr(RS2(i)));
}
static inline void sra(uint32_t i) {
  sprintf(mnemonic, "sra     %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, ((int32_t) regr(RS1(i))) >> RS2(i), mnemonic);
  regw(RD(i), ((int32_t) regr(RS1(i))) >> RS2(i));
}
static inline void srl(uint32_t i) {
  sprintf(mnemonic, "srl     %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, regr(RS1(i)) >> RS2(i), mnemonic);
  regw(RD(i), regr(RS1(i)) >> RS2(i));
}
static inline void lb(uint32_t i) {
  uint32_t word = mr(regr(RS1(i)) + sext(IMMI(i), 12));
  sprintf(mnemonic, "lb      %s,%i(%s)", reg_names[RD(i)], sext(IMMI(i), 12), reg_names[RS1(i)]);
  print_log(i, sext(word & 0xFF, 8), mnemonic);
  regw(RD(i), sext(word & 0xFF, 8));
}
static inline void lh(uint32_t i) {
  uint32_t word = mr(regr(RS1(i)) + sext(IMMI(i), 12));
  sprintf(mnemonic, "lh      %s,%i(%s)", reg_names[RD(i)], sext(IMMI(i), 12), reg_names[RS1(i)]);
  print_log(i, sext(word & 0xFFFF, 16), mnemonic);
  regw(RD(i), sext(word & 0xFFFF, 16));
}
static inline void lbu(uint32_t i) {
  uint32_t word = mr(regr(RS1(i)) + sext(IMMI(i), 12));
  sprintf(mnemonic, "lbu     %s,%i(%s)", reg_names[RD(i)], sext(IMMI(i), 12), reg_names[RS1(i)]);
  print_log(i, word & 0xFF, mnemonic);
  regw(RD(i), word & 0xFF);
}
static inline void lhu(uint32_t i) {
  uint32_t word = mr(regr(RS1(i)) + (int32_t) sext(IMMI(i), 12));
  sprintf(mnemonic, "lhu     %s,%i(%s)", reg_names[RD(i)], sext(IMMI(i), 12), reg_names[RS1(i)]);
  print_log(i, word & 0xFFFF, mnemonic);
  regw(RD(i), word & 0xFFFF);
}
static inline void lw(uint32_t i) {
  sprintf(mnemonic, "lw      %s,%i(%s)", reg_names[RD(i)], sext(IMMI(i), 12), reg_names[RS1(i)]);
  print_log(i, mr(regr(RS1(i)) + (int32_t) sext(IMMI(i), 12)), mnemonic);
  // fprintf(stderr, "read %i from %x \n", mr(regr(RS1(i)) + (int32_t) sext(IMMI(i), 12)), regr(RS1(i)) + (int32_t) sext(IMMI(i), 12));
  regw(RD(i), mr(regr(RS1(i)) + (int32_t) sext(IMMI(i), 12)));
}
static inline void sb(uint32_t i) {
  uint32_t addr = regr(RS1(i)) + (int32_t) sext(IMMS(i), 12);
  uint32_t data = (regr(RS2(i)) & 0xFF) | (mr(addr) & 0xFFFFFF00);
  sprintf(mnemonic, "sb      %s,%i(%s)", reg_names[RS2(i)], sext(IMMS(i), 12), reg_names[RS1(i)]);
  print_log(i, regr(RD(i)), mnemonic);
  mw(addr, data);
}
static inline void sh(uint32_t i) {
  uint32_t addr = regr(RS1(i)) + (int32_t) sext(IMMS(i), 12);
  uint32_t data = (regr(RS2(i)) & 0xFFFF) | (mr(addr) & 0xFFFF0000);
  sprintf(mnemonic, "sh      %s,%i(%s)", reg_names[RS2(i)], sext(IMMS(i), 12), reg_names[RS1(i)]);
  print_log(i, regr(RD(i)), mnemonic);
  mw(addr, data);
}
static inline void sw(uint32_t i) {
  sprintf(mnemonic, "sw      %s,%i(%s)", reg_names[RS2(i)], sext(IMMS(i), 12), reg_names[RS1(i)]);
  print_log(i, regr(RD(i)), mnemonic);
  // fprintf(stderr, "[%x] = %i \n", regr(RS1(i)) + (int32_t) sext(IMMS(i), 12), regr(RS2(i)));
  mw(regr(RS1(i)) + (int32_t) sext(IMMS(i), 12), regr(RS2(i)));
}
static inline void lui(uint32_t i) {
  sprintf(mnemonic, "lui     %s,0x%X", reg_names[RD(i)], IMMU(i) >> 12);
  print_log(i, IMMU(i), mnemonic);
  regw(RD(i), IMMU(i));
}
static inline void auipc(uint32_t i) {
  sprintf(mnemonic, "auipc   %s,0x%X", reg_names[RD(i)], IMMU(i));
  print_log(i, pc + (int32_t) IMMU(i), mnemonic);
  regw(RD(i), pc + (int32_t) IMMU(i));
}
static inline void slti(uint32_t i) {
  sprintf(mnemonic, "slti    %s,%s,%i", reg_names[RD(i)], reg_names[RS1(i)], sext(IMMI(i), 12));
  print_log(i, (int32_t) regr(RS1(i)) < (int32_t) sext(IMMI(i), 12), mnemonic);
  regw(RD(i), (int32_t) regr(RS1(i)) < (int32_t) sext(IMMI(i), 12));
}
static inline void sltiu(uint32_t i) {
  sprintf(mnemonic, "sltiu   %s,%s,%i", reg_names[RD(i)], reg_names[RS1(i)], sext(IMMI(i), 12));
  print_log(i, regr(RS1(i)) < sext(IMMI(i), 12), mnemonic);
  regw(RD(i), regr(RS1(i)) < sext(IMMI(i), 12));
}
static inline void xori(uint32_t i) {
  sprintf(mnemonic, "xori    %s,%s,%i", reg_names[RD(i)], reg_names[RS1(i)], sext(IMMI(i), 12));
  print_log(i, regr(RS1(i)) ^ sext(IMMI(i), 12), mnemonic);
  regw(RD(i), regr(RS1(i)) ^ sext(IMMI(i), 12));
}
static inline void ori(uint32_t i) {
  sprintf(mnemonic, "ori     %s,%s,%i", reg_names[RD(i)], reg_names[RS1(i)], sext(IMMI(i), 12));
  print_log(i, regr(RS1(i)) | sext(IMMI(i), 12), mnemonic);
  regw(RD(i), regr(RS1(i)) | sext(IMMI(i), 12));
}
static inline void andi(uint32_t i) {
  sprintf(mnemonic, "andi    %s,%s,%i", reg_names[RD(i)], reg_names[RS1(i)], sext(IMMI(i), 12));
  print_log(i, regr(RS1(i)) & sext(IMMI(i), 12), mnemonic);
  regw(RD(i), regr(RS1(i)) & sext(IMMI(i), 12));
}
static inline void slli(uint32_t i) {
  sprintf(mnemonic, "slli    %s,%s,0x%X", reg_names[RD(i)], reg_names[RS1(i)], sext(IMMI(i), 12));
  print_log(i, regr(RS1(i)) << SHAMT(i), mnemonic);
  regw(RD(i), regr(RS1(i)) << SHAMT(i));
}
static inline void srli(uint32_t i) {
  sprintf(mnemonic, "srli    %s,%s,0x%X", reg_names[RD(i)], reg_names[RS1(i)], sext(IMMI(i), 12));
  print_log(i, regr(RS1(i)) >> SHAMT(i), mnemonic);
  regw(RD(i), regr(RS1(i)) >> SHAMT(i));
}
static inline void srai(uint32_t i) {
  sprintf(mnemonic, "srai    %s,%s,0x%X", reg_names[RD(i)], reg_names[RS1(i)], sext(IMMI(i), 12));
  print_log(i, ((int32_t) regr(RS1(i))) >> SHAMT(i), mnemonic);
  regw(RD(i), ((int32_t) regr(RS1(i))) >> SHAMT(i));
}
static inline void sll(uint32_t i) {
  sprintf(mnemonic, "sll     %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, regr(RS1(i)) << regr(RS2(i)), mnemonic);
  regw(RD(i), regr(RS1(i)) << regr(RS2(i)));
}
static inline void slt(uint32_t i) {
  sprintf(mnemonic, "slt     %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, (int32_t) regr(RS1(i)) < (int32_t) regr(RS2(i)), mnemonic);
  regw(RD(i), (int32_t) regr(RS1(i)) < (int32_t) regr(RS2(i)));
}
static inline void sltu(uint32_t i) {
  sprintf(mnemonic, "sltu    %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, regr(RS1(i)) < regr(RS2(i)), mnemonic);
  regw(RD(i), regr(RS1(i)) < regr(RS2(i)));
}
static inline void xor(uint32_t i) {
  sprintf(mnemonic, "xor     %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, regr(RS1(i)) ^ regr(RS2(i)), mnemonic);
  regw(RD(i), regr(RS1(i)) ^ regr(RS2(i)));
}
static inline void or(uint32_t i) {
  sprintf(mnemonic, "or      %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, regr(RS1(i)) | regr(RS2(i)), mnemonic);
  regw(RD(i), regr(RS1(i)) | regr(RS2(i)));
}
static inline void and(uint32_t i) {
  sprintf(mnemonic, "and     %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, regr(RS1(i)) & regr(RS2(i)), mnemonic);
  regw(RD(i), regr(RS1(i)) & regr(RS2(i)));
}
static inline void beq(uint32_t i) {
  sprintf(mnemonic, "beq     %s,%s,%X", reg_names[RS1(i)], reg_names[RS2(i)], pc + (int32_t) sext(IMMB(i), 13));
  print_log(i, regr(RD(i)), mnemonic);
  if (regr(RS1(i)) == regr(RS2(i)))
    pc += (int32_t) sext(IMMB(i), 13) - 4;
}
static inline void bne(uint32_t i) {
  sprintf(mnemonic, "bne     %s,%s,%X", reg_names[RS1(i)], reg_names[RS2(i)], pc + (int32_t) sext(IMMB(i), 13));
  print_log(i, regr(RD(i)), mnemonic);
  if (regr(RS1(i)) != regr(RS2(i)))
    pc += (int32_t) sext(IMMB(i), 13) - 4;
}
static inline void blt(uint32_t i) {
  sprintf(mnemonic, "blt     %s,%s,%X", reg_names[RS1(i)], reg_names[RS2(i)], pc + (int32_t) sext(IMMB(i), 13));
  print_log(i, regr(RD(i)), mnemonic);
  if (((int32_t) regr(RS1(i))) < ((int32_t) regr(RS2(i))))
    pc += (int32_t) sext(IMMB(i), 13) - 4;
}
static inline void bge(uint32_t i) {
  sprintf(mnemonic, "bge     %s,%s,%X", reg_names[RS1(i)], reg_names[RS2(i)], pc + (int32_t) sext(IMMB(i), 13));
  print_log(i, regr(RD(i)), mnemonic);
  if (((int32_t) regr(RS1(i))) >= ((int32_t) regr(RS2(i))))
    pc += (int32_t) sext(IMMB(i), 13) - 4;
}
static inline void bltu(uint32_t i) {
  sprintf(mnemonic, "bltu    %s,%s,%X", reg_names[RS1(i)], reg_names[RS2(i)], pc + (int32_t) sext(IMMB(i), 13));
  print_log(i, regr(RD(i)), mnemonic);
  if (regr(RS1(i)) < regr(RS2(i)))
    pc += (int32_t) sext(IMMB(i), 13) - 4;
}
static inline void bgeu(uint32_t i) {
  sprintf(mnemonic, "bgeu    %s,%s,%X", reg_names[RS1(i)], reg_names[RS2(i)], pc + (int32_t) sext(IMMB(i), 13));
  print_log(i, regr(RD(i)), mnemonic);
  if (regr(RS1(i)) >= regr(RS2(i)))
    pc += (int32_t) sext(IMMB(i), 13) - 4;
}
static inline void fence(uint32_t i) {
  sprintf(mnemonic, "fence   ");
  print_log(i, regr(RD(i)), mnemonic);
}
static inline void jal(uint32_t i) {
  sprintf(mnemonic, "jal     %s,%X", reg_names[RD(i)], pc + (int32_t) sext(IMMJ(i), 21));
  print_log(i, pc + 4, mnemonic);
  regw(RD(i), pc + 4);
  pc += (int32_t) sext(IMMJ(i), 21) - 4;
}
static inline void jalr(uint32_t i) {
  sprintf(mnemonic, "jalr    %s,%s,%i", reg_names[RD(i)], reg_names[RS1(i)], sext(IMMI(i), 12));
  print_log(i, pc + 4, mnemonic);
  uint32_t t = pc + 4;
  pc = (regr(RS1(i)) + (uint32_t) sext(IMMI(i), 12)) - 4;
  regw(RD(i), t);
}
static inline void ecall(uint32_t i) {
  sprintf(mnemonic, "ecall   ");
  print_log(i, regr(RD(i)), mnemonic);
  syscall();
}
static inline void ebreak(uint32_t i) {
  sprintf(mnemonic, "ebreak  ");
  print_log(i, regr(RD(i)), mnemonic);
  running = 0;
}
static inline void illegal(uint32_t i) {
  fprintf(stderr, "Illegal instruction %X at %u\n", i, pc);
}
static inline void mul(uint32_t i) {
  sprintf(mnemonic, "mul     %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, ((int64_t) regr(RS1(i)) * (int64_t) regr(RS2(i))), mnemonic);
  regw(RD(i), ((int64_t) regr(RS1(i)) * (int64_t) regr(RS2(i))));
}
static inline void mulh(uint32_t i) {
  sprintf(mnemonic, "mulh    %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, ((int64_t) regr(RS1(i)) * (int64_t) regr(RS2(i))) >> 32, mnemonic);
  regw(RD(i), ((int64_t) regr(RS1(i)) * (int64_t) regr(RS2(i))) >> 32);
}
static inline void mulhsu(uint32_t i) {
  sprintf(mnemonic, "mulhsu  %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, ((int64_t) regr(RS1(i)) * (uint64_t) regr(RS2(i))) >> 32, mnemonic);
  regw(RD(i), ((int64_t) regr(RS1(i)) * (uint64_t) regr(RS2(i))) >> 32);
}
static inline void mulhu(uint32_t i) {
  sprintf(mnemonic, "mulhu   %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, ((uint64_t) regr(RS1(i)) * (uint64_t) regr(RS2(i))) >> 32, mnemonic);
  regw(RD(i), ((uint64_t) regr(RS1(i)) * (uint64_t) regr(RS2(i))) >> 32);
}
static inline void i_div(uint32_t i) {
  sprintf(mnemonic, "div     %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, ((int32_t) regr(RS1(i)) / (int32_t) regr(RS2(i))), mnemonic);
  regw(RD(i), ((int32_t) regr(RS1(i)) / (int32_t) regr(RS2(i))));
}
static inline void divu(uint32_t i) {
  sprintf(mnemonic, "divu    %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, (regr(RS1(i)) / regr(RS2(i))), mnemonic);
  regw(RD(i), (regr(RS1(i)) / regr(RS2(i))));
}
static inline void rem(uint32_t i) {
  sprintf(mnemonic, "rem     %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, ((int32_t) regr(RS1(i)) % (int32_t) regr(RS2(i))), mnemonic);
  regw(RD(i), ((int32_t) regr(RS1(i)) % (int32_t) regr(RS2(i))));
}
static inline void remu(uint32_t i) {
  sprintf(mnemonic, "remu    %s,%s,%s", reg_names[RD(i)], reg_names[RS1(i)], reg_names[RS2(i)]);
  print_log(i, (regr(RS1(i)) % regr(RS2(i))), mnemonic);
  regw(RD(i), (regr(RS1(i)) % regr(RS2(i))));
}

static inline void op_ld_f(uint32_t i) {
  switch (FUNCT3(i))
  {
  case 0b000:
    lb(i);
    break;
  case 0b001:
    lh(i);
    break;
  case 0b010:
    lw(i);
    break;
  case 0b100:
    lbu(i);
    break;
  case 0b101:
    lhu(i);
    break;
  default:
    illegal(i);
    break;
  }
}
static inline void op_misc_f(uint32_t i) {
  if (FUNCT3(i) == 0) {
    fence(i);
  } else {
    illegal(i) ;
  }
}
static inline void op_imm_f(uint32_t i) {
  switch (FUNCT3(i))
  {
  case 0b000:
    addi(i);
    break;
  case 0b001:
    slli(i);
    break;
  case 0b010:
    slti(i);
    break;
  case 0b011:
    sltiu(i);
    break;
  case 0b100:
    xori(i);
    break;
  case 0b101:
    if (FUNCT5(i) == 0) {
      srli(i);
    } else if(FUNCT5(i) == 0b01000) {
      srai(i);
    } else {
      illegal(i);
    }
    break;
  case 0b110:
    ori(i);
    break;
  case 0b111:
    andi(i);
    break;
  default:
    illegal(i);
    break;
  }
}
static inline void op_auipc_f(uint32_t i) {
  auipc(i);
}
static inline void op_st_f(uint32_t i) {
  switch (FUNCT3(i))
  {
  case 0b000:
    sb(i);
    break;
  case 0b001:
    sh(i);
    break;
  case 0b010:
    sw(i);
    break;
  default:
    illegal(i);
    break;
  }
}
static inline void op_op_f(uint32_t i) {
  switch (FUNCT7(i))
  {
  case 0:
    switch (FUNCT3(i))
    {
    case 0b000:
      add(i);
      break;
    case 0b001:
      sll(i);
      break;
    case 0b010:
      slt(i);
      break;
    case 0b011:
      sltu(i);
      break;
    case 0b100:
      xor(i);
      break;
    case 0b101:
      srl(i);
      break;
    case 0b110:
      or(i);
      break;
    case 0b111:
      and(i);
      break;
    default:
      illegal(i);
      break;
    }
    break;
  case 1:
    switch (FUNCT3(i))
    {
    case 0b000:
      mul(i);
      break;
    case 0b001:
      mulh(i);
      break;
    case 0b010:
      mulhsu(i);
      break;
    case 0b011:
      mulhu(i);
      break;
    case 0b100:
      i_div(i);
      break;
    case 0b101:
      divu(i);
      break;
    case 0b110:
      rem(i);
      break;
    case 0b111:
      remu(i);
      break;
    default:
      illegal(i);
      break;
    }
    break;
  case 0b0100000:
    switch (FUNCT3(i))
    {
    case 0b000:
      sub(i);
      break;
    case 0b101:
      sra(i);
      break;
    default:
      illegal(i);
      break;
    }
    break;
  default:
    illegal(i);
    break;
  }
}
static inline void op_lui_f(uint32_t i) {
  lui(i);
}
static inline void op_branch_f(uint32_t i) {
  switch (FUNCT3(i))
  {
  case 0b000:
    beq(i);
    break;
  case 0b001:
    bne(i);
    break;
  case 0b100:
    blt(i);
    break;
  case 0b101:
    bge(i);
    break;
  case 0b110:
    bltu(i);
    break;
  case 0b111:
    bgeu(i);
    break;
  default:
    illegal(i);
    break;
  }
}
static inline void op_jalr_f(uint32_t i) {
  if (FUNCT3(i) == 0)
    jalr(i);
  else
    illegal(i);
}
static inline void op_jal_f(uint32_t i) {
  jal(i);
}
static inline void op_sys_f(uint32_t i) {
  switch (FUNCT3(i))
  {
  case 0b000:
    if (i == 115)
      ecall(i);
    else if (i == 1048691)
      ebreak(i);
    else
      illegal(i);
    break;
  default:
    illegal(i);
    break;
  }
}

// Execute instruction
typedef void (*op_ex_f)(uint32_t instruction);
// Make all equal to illegal
// Then set the defined instruction opcodes
op_ex_f op_ex[32] = {
      op_ld_f,   illegal, illegal, op_misc_f, op_imm_f, op_auipc_f, illegal, illegal,
      op_st_f,   illegal, illegal,   illegal,  op_op_f,   op_lui_f, illegal, illegal,
      illegal,   illegal, illegal,   illegal,  illegal,    illegal, illegal, illegal,
  op_branch_f, op_jalr_f, illegal,  op_jal_f, op_sys_f,    illegal, illegal, illegal
};
