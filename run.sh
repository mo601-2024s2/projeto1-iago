#!/bin/bash

SIMULATOR="./simulator/build/src/vm"

set -e

for file in test/ACStone/*.elf ; do
  filename="${file%.elf}"
  echo "Running ${file}"
  $SIMULATOR "${file}" > "${filename}.log"
done
cd ../..

set -x
echo "Running dhrystone.elf"
$SIMULATOR "test/dhrystone/dhrystone.elf" 2> "./test/dhrystone/dhrystone.log"

echo "Running coremark.elf"
$SIMULATOR "test/coremark/coremark.elf" # > "./test/coremark/coremark.log"
