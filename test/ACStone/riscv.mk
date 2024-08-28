CROSS_CFLAGS = -g -march=rv32im -std=gnu99 -mabi=ilp32
LINK_OPTS = -nostartfiles -nostdinc -nostdlib
LIB_DIR	:=	-L../riscv/tests/libac_sysc
LIBS	:=	-lc -lac_sysc
HAL		:=	../riscv/tests/rv_hal/get_id.S

# Compile programs
$(TESTS):
	# $(CROSS_COMPILER) -c crt.S $(CROSS_CFLAGS)
	$(CROSS_COMPILER) $(CROSS_CFLAGS) crt.S $@.c -o $@$(SUFFIX)  $(CROSS_CFLAGS) $(LINK_OPTS)
	
