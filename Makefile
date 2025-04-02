all:
	gcc ./neander/neander.c -o ./neander/neander
	gcc ./assembler/assembler.c ./assembler/tokens.c -o ./assembler/assembler

run: all
	./assembler/assembler
	./neander/neander ./assembler/memoria.mem

clean:
	rm -f ./neander/neander
	rm -f ./assembler/assembler
	rm -f ./assembler/memoria.mem
