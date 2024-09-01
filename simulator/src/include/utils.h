#pragma once

#include <stdint.h>

// Macros for Instruction Decode
#define RS1(i)    ((i >> 15) & 0x1F)
#define RS2(i)    ((i >> 20) & 0x1F)
#define RD(i)     ((i >>  7) & 0x1F)
#define IMMI(i)   (i >> 20)
#define IMMS(i)   (((i >> 20) & 0xFE0) | ((i >> 7) & 0x1F))
#define IMMB(i)   (((i >> 19) & 0x1000) | ((i << 4) & 0x800) | ((i >> 20) & 0x7E0) | ((i >> 7) & 0b11110))
#define IMMU(i)   ((i & 0xFFFFF000))
#define IMMJ(i)   (((i >> 11) & 0x100000) | ((i >> 9) & 0x800) | (i & 0xFF000) | ((i >> 20) & 0x7FE))
#define FUNCT3(i) ((i >> 12) & 0x7)
#define FUNCT5(i) ((i >> 27) & 0x1F)
#define FUNCT7(i) (i >> 25)
#define SHAMT(i)  ((i >> 20) & 0x1F)
#define OPCODE(i) ((i >>  2) & 0x1F)
#define NOT_COMPACT(i) (((i) & 0x3) == 0x3)

// Sign-extends the b-th bit value of n to total 32 bits
static inline uint32_t sext(uint32_t n, int b) {
  return ((n >> (b - 1)) & 1) ?
    (n | (UINT32_MAX << b)) :
    n;
}

char* reg_names[32] = {
  "zero",
  "ra",
  "sp",
  "gp",
  "tp",
  "t0",
  "t1",
  "t2",
  "s0",
  "s1",
  "a0",
  "a1",
  "a2",
  "a3",
  "a4",
  "a5",
  "a6",
  "a7",
  "s2",
  "s3",
  "s4",
  "s5",
  "s6",
  "s7",
  "s8",
  "s9",
  "s10",
  "s11",
  "t3",
  "t4",
  "t5",
  "t6",
};

// 88 bytes
typedef struct stat {
  uint16_t st_dev;     /*    ID of device containing file */
  uint16_t st_ino;     /*    inode number */
  uint32_t st_mode;    /*    protection */
  uint16_t st_nlink;   /*    number of hard links */
  uint16_t st_uid;     /*    user ID of owner */
  uint16_t st_gid;     /*    group ID of owner */
  uint16_t st_rdev;    /*    device ID (if special file) */
  uint32_t st_size;    /*    total size, in bytes */
  uint32_t st_blocks;  /*    number of 512B blocks allocated */
  uint64_t st_atime;   /*    time of last access */
  uint64_t pad0;
  uint64_t st_mtime;   /*    time of last modification */
  uint64_t pad1;
  uint64_t st_ctime;   /*    time of last status change */
  uint64_t pad2;
  uint32_t st_blksize; /*    blocksize for file system I/O */
  uint64_t pad3;
} stat;

stat stdout_stat = {
  .st_dev     = 0x40,
  .st_ino     = 0x7,
  .st_mode    = 0x2190,
  .st_nlink   = 0x1,
  .st_uid     = 0x3e8,
  .st_gid     = 0x5,
  .st_rdev    = -30716,
  .st_size    = 0x0,
  .st_blocks  = 0x0,
  .st_atime   = 0x2a008,
  .pad0       = 0,
  .st_mtime   = 0x2a008,
  .pad1       = 0,
  .st_ctime   = 0x2a008,
  .pad2       = 0,
  .st_blksize = 0x400
};
