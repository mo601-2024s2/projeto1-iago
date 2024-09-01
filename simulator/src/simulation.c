#include <stdbool.h>
#include "utils.h"
#include "simulation.h"
#include "instructions.h"

// Load program
void ld_img(char *fname, uint32_t offset) {
  FILE *in = fopen(fname, "rb");
  if (NULL == in) {
    fprintf(stderr, "Cannot open file %s.\n", fname);
    exit(1);
  }
  uint32_t *p = (uint32_t *) (mem + offset);
  while (fread(p++, sizeof(uint32_t), 1, in)) {}
  fclose(in);
}

typedef struct {
  unsigned char e_ident[16];
  uint16_t      e_type;
  uint16_t      e_machine;
  uint32_t      e_version;
  uint32_t      e_entry;
  uint32_t      e_phoff;
  uint32_t      e_shoff;
  uint32_t      e_flags;
  uint16_t      e_ehsize;
  uint16_t      e_phentsize;
  uint16_t      e_phnum;
  uint16_t      e_shentsize;
  uint16_t      e_shnum;
  uint16_t      e_shstrndx;
} elf32_eh;

typedef struct {
	uint32_t	p_type;
	uint32_t	p_offset;
	uint32_t	p_vaddr;
	uint32_t	p_paddr;
	uint32_t	p_filesz;
	uint32_t	p_memsz;
	uint32_t	p_flags;
	uint32_t	p_align;
} elf32_ph;

typedef struct {
  uint32_t   sh_name;
  uint32_t   sh_type;
  uint32_t   sh_flags;
  uint32_t   sh_addr;
  uint32_t   sh_offset;
  uint32_t   sh_size;
  uint32_t   sh_link;
  uint32_t   sh_info;
  uint32_t   sh_addralign;
  uint32_t   sh_entsize;
} elf32_sh;

#define ET_EXEC 0x02
#define RISCV   0xF3 	
#define LENDIAN 0x01
#define BENDIAN 0x02
#define F32BIT  0x01
#define PH_LOAD 0x01

bool isElf(elf32_eh *e_header) {
  return e_header->e_ident[0] == 0x7F &&
         e_header->e_ident[1] == 'E' &&
         e_header->e_ident[2] == 'L' &&
         e_header->e_ident[3] == 'F';
}

bool isValidElf(elf32_eh *e_header) {
  return e_header->e_ident[4] == F32BIT &&
         e_header->e_ident[5] == LENDIAN &&
         e_header->e_type == ET_EXEC &&
         e_header->e_machine == RISCV;
}

void ld_elf(char *fname) {
  FILE *in = fopen(fname, "rb");
  if (NULL == in) {
    fprintf(stderr, "Cannot open file %s.\n", fname);
    exit(1);
  }
  elf32_eh e_header;
  fread(&e_header, sizeof(e_header), 1, in);
  
  if (!isElf(&e_header)) {
    fprintf(stderr, "File %s is not an ELF.\n", fname);
    fclose(in);
    exit(1);
  }

  if (!isValidElf(&e_header)) {
    fprintf(stderr, "File %s is not a valid ELF.\n", fname);
    if (e_header.e_machine != RISCV)
      fprintf(stderr, "Machine type not RISC-V.\n");
    if (e_header.e_type != ET_EXEC)
      fprintf(stderr, "Not an executable ELF.\n");
    if (e_header.e_ident[4] != F32BIT)
      fprintf(stderr, "Not RV32.\n");
    if (e_header.e_ident[5] != LENDIAN)
      fprintf(stderr, "Not little endian.\n");
    fclose(in);
    exit(1);
  }

  for (int i = 0; i < (sizeof(mem) / 4); i++) {
    mem[i] = 0;
  }

  for (unsigned i = 0; i < e_header.e_phnum; i++) {
    elf32_ph ph_header;
    fseek(in, e_header.e_phoff + e_header.e_phentsize * i, SEEK_SET);
    fread(&ph_header, sizeof(ph_header), 1, in);

    if (ph_header.p_type == PH_LOAD) {
      fseek(in, ph_header.p_offset, SEEK_SET);
      // printf("Reading %x bytes of data from %x to %x.\n", ph_header.p_filesz, ph_header.p_offset, ph_header.p_paddr);
      if (fread(mem + ph_header.p_paddr, sizeof(uint8_t), ph_header.p_filesz, in) != ph_header.p_filesz) {
        fprintf(stderr, "Error copying data.\n");
        exit(1);
      }
    }
    // TODO: Apply memory flags
    // printf(" p_flags: %d\n", ph_header.p_flags);
  }

  elf32_sh sh_header;
  fseek(in, e_header.e_shoff + e_header.e_shentsize * e_header.e_shstrndx, SEEK_SET);
  fread(&sh_header, sizeof(sh_header), 1, in);
  unsigned shrtrtab =  sh_header.sh_offset;

  for (unsigned i = 0; i < e_header.e_shnum; i++) {
    fseek(in, e_header.e_shoff + e_header.e_shentsize * i, SEEK_SET);
    fread(&sh_header, sizeof(sh_header), 1, in);
        
    char buffer[32];
    fseek(in, shrtrtab + sh_header.sh_name, SEEK_SET);
    fgets(buffer, 31, in);

    if (strcmp(buffer, ".bss") == 0) {
      pbrk = sh_header.sh_addr;
    }
  }
  pc = e_header.e_entry;
  fclose(in);
}

// Program loop
int start(unsigned total_cycles) {
  // printf("Starting at 0x%x\n", pc);
  unsigned count = 0;
  while (running) {
    // printf("At 0x%x\n", pc);
    uint32_t i = mr(pc);
    if (i == 0) {
      printf("Stopping at: %x.\n", pc);
      break;
    }
    if (NOT_COMPACT(i))
      op_ex[OPCODE(i)](i);
    pc += 4;
    cycle += 1;
    if (total_cycles > 0) {
      if (count > total_cycles)
        exit(0);
      count++;
    }
  }
  return error;
}