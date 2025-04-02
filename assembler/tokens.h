#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STA 0x10
#define LDA 0x20
#define ADD 0x30
#define NOT 0x60
#define JMP 0x80
#define JN 0x90
#define JZ 0xA0
#define HLT 0xF0

typedef struct datacodigo {
    __uint8_t instrucao;
    __uint8_t variavel;
    bool tem2bytes;
    __uint8_t endereco;
    struct datacodigo *prox;
} DATACODIGO;

typedef struct datavalor {
    __uint8_t variavel;
    __uint8_t valor;
    __uint8_t endereco;
    struct datavalor *prox;
} DATAVALOR;

DATACODIGO *init_datacodigo();
void inserir_cod(DATACODIGO *datacodigo, __uint8_t instrucao, __uint8_t variavel, bool tem2bytes, int linha_entrada);
void imprimir_cod(DATACODIGO *datacodigo);
void excluir_cod(DATACODIGO *datacodigo);
__uint8_t obter_dado_cod(FILE *file);
__uint8_t obter_endereco_cod(FILE *file);
__uint8_t procurar_linha(DATACODIGO *datacodigo, __uint8_t linha);

DATAVALOR *init_datavalor();
void inserir_val(DATAVALOR *datavalor, __uint8_t variavel, __uint8_t valor);
void imprimir_val(DATAVALOR *datavalor);
void excluir_val(DATAVALOR *datavalor);
__uint8_t obter_dado_val(FILE *file2);
__uint8_t procurar_variavel(DATAVALOR *datavalor, __uint8_t var);

void limpa_linha(char *linha);