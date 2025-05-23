CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all:
	$(CC) $(CFLAGS) ./neander/neander.c -o ./neander/executor
	$(CC) $(CFLAGS) ./assembler/assembler.c ./assembler/tokens.c -o ./assembler/assembler
	$(CC) $(CFLAGS) ./compiler/compilador.c -o ./compiler/compilador

asm: all
	./assembler/assembler ./exemplo02.asm
	./neander/executor ./assembler/programa.bin

run: all
	./compiler/compilador ./programa.lpn
	./assembler/assembler ./compiler/programa.asm
	./neander/executor ./assembler/programa.bin

clean:
	rm -f ./neander/executor
	rm -f ./assembler/assembler ./assembler/programa.bin
	rm -f ./compiler/compilador ./compiler/programa.asm
