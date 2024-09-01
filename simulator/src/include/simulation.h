#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void ld_elf(char *fname);

void ld_img(char *fname, uint32_t offset);

int start(unsigned cycles);
