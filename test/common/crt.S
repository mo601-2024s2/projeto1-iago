#define STACK_SIZE 65536

  .text
  .globl _start
  .equ memory_size, 0x200000

_start:
  lui gp, 0x20000
  lui sp, 0x50
  jal main
  li a7, 93
  li a0, 0
  ecall

.bss
.align 8
.skip 4096
kstacktop:

.section .tbss
tls_start:
