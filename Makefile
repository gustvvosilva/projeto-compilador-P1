all:
	gcc ./neander/neander.c -o ./neander/executor
	gcc ./assembler/assembler.c ./assembler/tokens.c -o ./assembler/assembler

run: all
	./assembler/assembler
	./neander/executor ./assembler/programa.bin

clean:
	rm -f ./neander/executor
	rm -f ./assembler/assembler
	rm -f ./assembler/programa.bin
