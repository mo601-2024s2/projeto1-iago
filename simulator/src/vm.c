#include <stdbool.h>
#include "simulation.h"

int main(int argc, char **argv) {
  int cycles = 0;
  if (argc == 3)
    cycles = atoi(argv[2]);
  ld_elf(argv[1]);
  bool error = start((unsigned) cycles);
  return (int) error;
}
