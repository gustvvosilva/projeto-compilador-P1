#include "neander.h"

int main(int argc, char **argv){

    FILE *file = fopen(argv[argc - 1], "rb");

    if(!file) {
        printf("Erro ao abrir arquivo.\n");
        return 1;
    }
    
    __uint8_t bytes[256];
    __uint8_t byte[1];
    int getByte = 0;
    __uint32_t memID[1];
    __uint8_t ac = 0, pc = 0;
    bool neg, zer;

    fread(memID, 1, 4, file);
    // Little Endian.
    if(*memID != 0x52444e03) {
        printf("O arquivo fornecido não corresponde á um formato conhecido do neander.\n");
        fclose(file);
        return 1;
    }

    // Separando apenas os 256 Bytes que importam para nosso programa. Fica muito mais fácil trabalhar com esse formato.
    for(int i = 0; i < 512; i++) {

        if(i % 2 == 1)
            fread(byte, 1, 1, file);
        else {
            fread(byte, 1, 1, file);
            bytes[getByte] = *byte;
            getByte++;
        }
    }
    fclose(file);

    // Mude esse valores para trocar os caracteres da operação.
    // bytes[0x80] = 0xF7;
    // bytes[0x81] = 0xFB;

    while(bytes[pc] != HLT) {

        // Flags.
        if(ac >= 0x80)
            neg = true;
        else
            neg = false;

        if(ac == 0)
            zer = true;
        else
            zer = false;
            
        switch(bytes[pc]) {

            case STA:
            pc++;
            bytes[bytes[pc]] = ac;
            break;

            case LDA:
            pc++;
            ac = bytes[bytes[pc]];
            break;

            case ADD:
            pc++;
            ac += bytes[bytes[pc]];
            break;

            case NOT:
            ac = ~ac;
            break;

            case JMP:
            pc++;
            pc = bytes[pc] - 0x01;
            break;

            case JN:
            pc++;
            if(neg) {
                pc = bytes[pc] - 0x01;
            }
            break;

            case JZ:
            pc++;
            if(zer) {
                pc = bytes[pc] - 0x01;
            }
            break;
        }

        pc++;
    }

    if(ac >= 0x80)
        neg = true;
    else
        neg = false;

    if(ac == 0)
        zer = true;
    else
        zer = false;

    printf("\nAC: %02x - PC: %02x\n"
        "flags: N(%c) - Z(%c)\n", ac, pc, neg?'*':' ', zer?'*':' ');
    printf("\nMemória final =========================================");
    for(int i = 0; i < 256; i++){
        if(i % 16 == 0)
            printf("\n%07x ", i);
        printf("%02x ", bytes[i]);
    }
    printf("\n=======================================================\n");
    printf("Finalizou com sucesso!\n\n");
    return 0;
}
