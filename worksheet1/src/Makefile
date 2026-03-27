NASM = nasm
GCC = gcc
NASM_FLAGS = -f elf
GCC_FLAGS = -m32

ASM_IO = asm_io.asm
DRIVER = driver.c
TARGETS = task1 task1_2 task2_welcome task2_array task2_range

all: $(TARGETS)

task1: task1.o asm_io.o driver.o
	$(GCC) $(GCC_FLAGS) driver.o task1.o asm_io.o -o task1

task1.o: task1.asm asm_io.inc
	$(NASM) $(NASM_FLAGS) task1.asm -o task1.o

task1_2: task1_2.o asm_io.o driver.o
	$(GCC) $(GCC_FLAGS) driver.o task1_2.o asm_io.o -o task1_2

task1_2.o: task1_2.asm asm_io.inc
	$(NASM) $(NASM_FLAGS) task1_2.asm -o task1_2.o

task2_welcome: task2_welcome.o asm_io.o driver.o
	$(GCC) $(GCC_FLAGS) driver.o task2_welcome.o asm_io.o -o task2_welcome

task2_welcome.o: task2_welcome.asm asm_io.inc
	$(NASM) $(NASM_FLAGS) task2_welcome.asm -o task2_welcome.o

task2_array: task2_array.o asm_io.o driver.o
	$(GCC) $(GCC_FLAGS) driver.o task2_array.o asm_io.o -o task2_array

task2_array.o: task2_array.asm asm_io.inc
	$(NASM) $(NASM_FLAGS) task2_array.asm -o task2_array.o

task2_range: task2_range.o asm_io.o driver.o
	$(GCC) $(GCC_FLAGS) driver.o task2_range.o asm_io.o -o task2_range

task2_range.o: task2_range.asm asm_io.inc
	$(NASM) $(NASM_FLAGS) task2_range.asm -o task2_range.o

asm_io.o: $(ASM_IO) asm_io.inc
	$(NASM) $(NASM_FLAGS) $(ASM_IO) -o asm_io.o

driver.o: $(DRIVER)
	$(GCC) $(GCC_FLAGS) -c $(DRIVER) -o driver.o

clean:
	rm -f *.o $(TARGETS)

.PHONY: all clean
