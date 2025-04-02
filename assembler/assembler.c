#include "assembler.h"

void obter_tokens(FILE *entrada, DATACODIGO *datacodigo, DATAVALOR *datavalor) {

    char caractere[1];
    char linha[8] = {0};
    int i = 0;
    bool lendo_data = false;
    bool lendo_code = false;
    int linha_entrada = 0;

    while(fread(caractere, 1, 1, entrada)) {
        
        // printf("-C %c\n", *caractere);

        if(*caractere != 0x20 && *caractere != 0x0a)
        {
            linha[i] = *caractere;
            i++;
        }
        else
        {
            printf(">L %s\n", linha);

            if(linha[0] == 0x3b)
            {
                while(*caractere != 0x0a)
                {
                    if(!fread(caractere, 1, 1, entrada))
                        break;
                }
            }
            else if(strcmp(linha, ".DATA") == 0)
            {
                lendo_data = true;
                lendo_code = false;
            }
            else if (strcmp(linha, ".CODE") == 0)
            {
                lendo_data = false;
                lendo_code = true;
            } 
            else if(lendo_data)
            {
                if((linha[0] >= 0x41 && linha[0] <= 0x5a) || (linha[0] >= 0x61 && linha[0] <= 0x7a)) {
                    inserir_val(datavalor, linha[0], obter_dado_val(entrada));
                }
            }
            else if(lendo_code)
            {
                if(strcmp(linha, ".ORG") == 0)
                {
                    while(*caractere != 0x0a)
                    {
                        if(!fread(caractere, 1, 1, entrada))
                            break;
                    }
                    linha_entrada--;
                }
                else if(strcmp(linha, "STA") == 0) {
                    inserir_cod(datacodigo, STA, obter_dado_cod(entrada), true, linha_entrada);
                }
                else if (strcmp(linha, "LDA") == 0) {
                    inserir_cod(datacodigo, LDA, obter_dado_cod(entrada), true, linha_entrada);               
                }
                else if (strcmp(linha, "ADD") == 0) {
                    inserir_cod(datacodigo, ADD, obter_dado_cod(entrada), true, linha_entrada);
                }
                else if (strcmp(linha, "NOT") == 0) {
                    inserir_cod(datacodigo, NOT, 0, false, linha_entrada);
                }
                else if (strcmp(linha, "JMP") == 0) {
                    inserir_cod(datacodigo, JMP, obter_endereco_cod(entrada), true, linha_entrada);
                }
                else if (strcmp(linha, "JN") == 0) {
                    inserir_cod(datacodigo, JN, obter_endereco_cod(entrada), true, linha_entrada);
                }
                else if (strcmp(linha, "JZ") == 0) {
                    inserir_cod(datacodigo, JZ, obter_endereco_cod(entrada), true, linha_entrada);
                }
                else if (strcmp(linha, "HLT") == 0) {
                    inserir_cod(datacodigo, HLT, 0, false, linha_entrada);
                }
                linha_entrada++;
            }

            limpa_linha(linha);
            i = 0;
        }
    }
    return;
}

bool criar_memoria() {

    FILE *memoria = fopen("./assembler/memoria.mem", "wb");
    if(memoria == NULL) return false;

    __uint32_t memoriaID[] = {0x52444e03};  // Little Endian
    __uint8_t byteZerado[512] = {0x00};

    fwrite(memoriaID, 4, 1, memoria);
    fwrite(byteZerado, 1, 512, memoria);
    fclose(memoria);

    return true;
}

bool inserir_tokens(DATACODIGO *datacodigo, DATAVALOR *datavalor) {

    FILE *memoria = fopen("./assembler/memoria.mem", "r+b");
    if(memoria == NULL) return false;

    DATACODIGO *datacodigo_aux = datacodigo->prox;
    DATAVALOR *datavalor_aux = datavalor->prox;
    __uint8_t endereco;

    int i = 4;

    while(datacodigo_aux != NULL) {

        if(datacodigo_aux->tem2bytes)
            i += 4;
        else
            i += 2;

        datacodigo_aux = datacodigo_aux->prox;
    }
    while(datavalor_aux != NULL) {

        fseek(memoria, i, SEEK_SET);
        fwrite(&datavalor_aux->valor, 1, 1, memoria);
        datavalor_aux->endereco = i;
        datavalor_aux = datavalor_aux->prox;
        i += 2;
    }
    
    i = 4;
    datacodigo_aux = datacodigo->prox;

    while(datacodigo_aux != NULL) {

        fseek(memoria, i, SEEK_SET);
        fwrite(&datacodigo_aux->instrucao, 1, 1, memoria);
        if(datacodigo_aux->tem2bytes)
        {
            i += 2;
            fseek(memoria, i, SEEK_SET);
            if(datacodigo_aux->instrucao >= 0x10 && datacodigo_aux->instrucao <= 0x30)
            {
                endereco = (procurar_variavel(datavalor->prox, datacodigo_aux->variavel) - 4) / 2;
            }
            else
            {
                endereco = procurar_linha(datacodigo->prox, datacodigo_aux->variavel);
            }
            fwrite(&endereco, 1, 1, memoria);
        }
        datacodigo_aux = datacodigo_aux->prox;
        i += 2;
    }

    fclose(memoria);
    return true;
}

void imprimir_memoria() {

    FILE *memoria = fopen("./assembler/memoria.mem", "rb");
    if(memoria == NULL) {
        printf("ERRO ao imprimir memoria.\n");
        return;
    }

    __uint8_t bytes[516];

    fread(bytes, 1, 516, memoria);
    fclose(memoria);
    
    printf("\nHEXDUMP:");
    for(int i = 0; i < 516; i++) {

        if (i % 2 == 0)
            printf(" ");
        if (i % 16 == 0)
            printf("\n%07x ", i);

        printf("%02x", bytes[i]);
    }
    printf("\n");

    return;
}

int main() {

    FILE *entrada = fopen("./assembler/entrada.asm", "r");
    if(entrada == NULL) return -1;

    DATACODIGO *datacodigo = init_datacodigo();
    DATAVALOR *datavalor = init_datavalor();

    obter_tokens(entrada, datacodigo, datavalor);
    fclose(entrada);

    imprimir_cod(datacodigo);
    imprimir_val(datavalor);

    if(!criar_memoria())
        return -1;

    if(!inserir_tokens(datacodigo, datavalor))
        return -1;

    imprimir_val(datavalor);
    excluir_cod(datacodigo);
    excluir_val(datavalor);

    imprimir_memoria();
    
    return 0;
}