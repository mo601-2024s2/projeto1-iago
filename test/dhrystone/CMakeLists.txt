add_executable(dhrystone.elf dhry_1.c dhry_2.c dhry.h ../common/crt.S)
target_compile_options(dhrystone.elf PUBLIC -O3 -DTIME -DNOENUM -save-temps -Wno-implicit)
