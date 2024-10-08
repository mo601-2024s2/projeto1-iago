#!/bin/bash

## Environment ##

if ! command -v riscv32-unknown-elf-gcc &> /dev/null
then
  export RISCV=/opt/riscv
  export PATH=$PATH:$RISCV/bin
fi

## Simulator ##

mkdir -p simulator/build
cd simulator/build
cmake ..
cmake --build .
cd ../..

## Tests ##

cd test/ACStone
for file in *.c ; do
  filename="${file%.c}"
  echo "Compiling ${file}"
  riscv32-unknown-elf-gcc -march=rv32im "${file}" ../common/crt.S -o "${filename}.elf" -nostartfiles &
  riscv32-unknown-elf-objdump -d -Mno-aliases "${filename}.elf" > "${filename}.dump"
done
cd ../..
wait

cd test/dhrystone
echo "Compiling dhrystone"
riscv32-unknown-elf-gcc -O3 -DTIME -Wno-implicit -march=rv32im dhry_1.c dhry_2.c dhry.h ../common/crt.S -o dhrystone.elf -nostartfiles
riscv32-unknown-elf-objdump -d -Mno-aliases dhrystone.elf > dhrystone.dump

rm -f *.i
rm -f *.o
rm -f *.s
cd ../..

cd test/coremark
echo "Compiling coremark"
riscv32-unknown-elf-gcc -O3 -march=rv32im core_list_join.c core_main.c core_matrix.c core_state.c core_util.c posix/core_portme.c -I. -I"./posix" ../common/crt.S -o coremark.elf -nostartfiles
riscv32-unknown-elf-objdump -d -Mno-aliases coremark.elf > coremark.dump
cd ../..
