#include "tokens.h"

DATACODIGO *init_datacodigo() {
    DATACODIGO *nova = (DATACODIGO *) malloc(sizeof(DATACODIGO));
    nova->instrucao = 0;
    nova->variavel = 0;
    nova->tem2bytes = false;
    nova->endereco = 0;
    nova->prox = NULL;
    return nova;
}

void inserir_cod(DATACODIGO *datacodigo, __uint8_t instrucao, __uint8_t variavel, bool tem2bytes, int linha_entrada) {

    if(datacodigo->prox != NULL) {
        inserir_cod(datacodigo->prox, instrucao, variavel, tem2bytes, linha_entrada);
        return;
    }

    DATACODIGO *nova = (DATACODIGO *) malloc(sizeof(DATACODIGO));
    nova->instrucao = instrucao;
    nova->variavel = variavel;
    nova->tem2bytes = tem2bytes;
    nova->endereco = linha_entrada;
    nova->prox = NULL;
    datacodigo->prox = nova;
    return;
}

void imprimir_cod(DATACODIGO *datacodigo) {

    DATACODIGO *auxiliar = datacodigo;

    printf("Datacodigo:\n");
    while (auxiliar != NULL) {
        printf("> %02x - %02x\n", auxiliar->instrucao, auxiliar->variavel);
        auxiliar = auxiliar->prox;
    }
    printf("\n");
    return;
}

void excluir_cod(DATACODIGO *datacodigo) {

    DATACODIGO *apagar;
    DATACODIGO *auxiliar = datacodigo;

    while (auxiliar != NULL) {
        apagar = auxiliar;
        auxiliar = auxiliar->prox;
        free(apagar);
    }

    printf("Datacodigo apagado com sucesso.\n");
    return;
}

__uint8_t obter_dado_cod(FILE *file) {

    char leitura[1];
    __uint8_t variavel[1];

    for(;;) {

        fread(leitura, 1, 1, file);

        // printf("C %c\n", *leitura);

        if(*leitura != 32 && *leitura != 10)
        {
            variavel[0] = *leitura;
        }
        else
        {
            printf(">L %c = %02x\n", variavel[0], variavel[0]);
            // printf("kkkkkkk %d\n", strcmp(linha, "ADD"));
            break;
        }
    }
    return variavel[0];
}

__uint8_t obter_endereco_cod(FILE *file) {

    char caractere[1];
    char linha[8] = {0};
    int i = 0;
    __uint8_t endereco;

    for(;;) {

        fread(caractere, 1, 1, file);
        // printf("C %c\n", *caractere);

        if(*caractere != 32 && *caractere != 10)
        {
            linha[i] = *caractere;
            i++;
        }
        else
        {
            printf(">L %s\n", linha);
            // printf("kkkkkkk %d\n", strcmp(linha, "ADD"));

            endereco = atoi(linha);
            break;
        }
    }
    return endereco;
}

__uint8_t procurar_linha(DATACODIGO *datacodigo, __uint8_t linha) {

    DATACODIGO *auxiliar = datacodigo;
    __uint8_t endereco = 0;

    while(auxiliar != NULL && auxiliar->endereco != linha)
    {
        if(auxiliar->tem2bytes)
            endereco += 2;
        else
            endereco += 1;

        auxiliar = auxiliar->prox;
    }

    return endereco;
}

DATAVALOR *init_datavalor() {
    DATAVALOR *nova = (DATAVALOR *) malloc(sizeof(DATAVALOR));
    nova->variavel = 0;
    nova->valor = 0;
    nova->endereco = 0;
    nova->prox = NULL;
    return nova;
}

void inserir_val(DATAVALOR *datavalor, __uint8_t variavel, __uint8_t valor) {

    if(datavalor->prox != NULL) {
        inserir_val(datavalor->prox, variavel, valor);
        return;
    }

    DATAVALOR *nova = (DATAVALOR *) malloc(sizeof(DATAVALOR));
    nova->variavel = variavel;
    nova->valor = valor;
    nova->endereco = 0;
    nova->prox = NULL;
    datavalor->prox = nova;
    return;
}

void imprimir_val(DATAVALOR *datavalor) {

    DATAVALOR *auxiliar = datavalor;

    printf("Datavalor:\n");
    while (auxiliar != NULL) {
        printf("> %02x - %02x %02x\n", auxiliar->variavel, auxiliar->valor, auxiliar->endereco);
        auxiliar = auxiliar->prox;
    }
    printf("\n");
    return;
}

void excluir_val(DATAVALOR *datavalor) {

    DATAVALOR *apagar;
    DATAVALOR *auxiliar = datavalor;

    while (auxiliar != NULL) {
        apagar = auxiliar;
        auxiliar = auxiliar->prox;
        free(apagar);
    }

    printf("Datavalor apagado com sucesso.\n");
    return;
}

__uint8_t obter_dado_val(FILE *file2) {

    char caractere[1];
    char linha[8] = {0};
    int i = 0;
    __uint8_t valor;

    for(;;) {

        fread(caractere, 1, 1, file2);
        // printf("C %c\n", *caractere);

        if(*caractere != 32 && *caractere != 10)
        {
            linha[i] = *caractere;
            i++;
        }
        else
        {
            printf(">L %s\n", linha);
            // printf("kkkkkkk %d\n", strcmp(linha, "ADD"));

            if(strcmp(linha, "DB") != 0) {
                if(strcmp(linha, "?") == 0){
                    valor = 0;
                    break;
                }
                else
                {
                    // Converte valores negativos implicitamente.
                    valor = atoi(linha);
                    break;
                }
            }

            limpa_linha(linha);
            i = 0;
        }
    }
    return valor;
}

__uint8_t procurar_variavel(DATAVALOR *datavalor, __uint8_t var) {

    DATAVALOR *auxiliar = datavalor;
    __uint8_t endereco = 0;

    while(auxiliar != NULL) {

        if(auxiliar->variavel == var) {
            endereco = auxiliar->endereco;
            break;
        }
        auxiliar = auxiliar->prox;
    }

    return endereco;
}

void limpa_linha(char *linha) {
    for (int i = 0; linha[i] != 0; i++)
        linha[i] = 0;
    return;
}