#!/bin/bash

SIMULATOR="./simulator/build/src/vm"

for file in test/ACStone/*.elf ; do
  filename="${file%.elf}"
  echo "Running ${file}"
  $SIMULATOR "${file}" 2> "${filename}.log"
done
cd ../..

echo "Running dhrystone.elf"
$SIMULATOR "test/dhrystone/dhrystone.elf" 2> "test/dhrystone/dhrystone.log"

echo "Running coremark.elf"
$SIMULATOR "test/coremark/coremark.elf" 2> "test/coremark/coremark.log"
