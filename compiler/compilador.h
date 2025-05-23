#ifndef COMPILADOR_H
#define COMPILADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

typedef enum {
    TOKEN_PROGRAMA,
    TOKEN_INICIO,
    TOKEN_FIM,
    TOKEN_RES,
    TOKEN_VAR,
    TOKEN_NUM,
    TOKEN_DECLA,   // =
    TOKEN_SOMA,    // +
    TOKEN_SUB,     // -
    TOKEN_ASPAS,   // "
    TOKEN_DP,      // :
    TOKEN_FINAL,
    TOKEN_DESCONHECIDO
} token_tipos;

typedef struct {
    token_tipos tipo;
    char valor[16];
} TOKEN;

void proximo_token();
void consumir_token(token_tipos tipo_esperado);

void parse_programa();
void parse_adicao();
void parse_valor();

int eval_valor();
int eval_exp();

#endif // COMPILADOR_H