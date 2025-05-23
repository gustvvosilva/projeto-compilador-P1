#include "compilador.h"

const char *fonte;
int pos = 0;
TOKEN token_atual;
FILE *saida;

void ignora_espaco() {
    while (fonte[pos] == ' ' || fonte[pos] == '\t' || fonte[pos] == '\n')
        pos++;
}

TOKEN obter_token() {
    TOKEN token;
    int inicio = pos;
    while (isalnum((unsigned char)fonte[pos])) pos++;
    int tamanho = pos - inicio;
    strncpy(token.valor, fonte + inicio, tamanho);
    token.valor[tamanho] = '\0';
    if (strcmp(token.valor, "PROGRAMA") == 0)
        token.tipo = TOKEN_PROGRAMA;
    else if (strcmp(token.valor, "INICIO") == 0)
        token.tipo = TOKEN_INICIO;
    else if (strcmp(token.valor, "FIM") == 0)
        token.tipo = TOKEN_FIM;
    else if (strcmp(token.valor, "RES") == 0)
        token.tipo = TOKEN_RES;
    else
        token.tipo = TOKEN_VAR;
    return token;
}

TOKEN obter_numero() {
    TOKEN token;
    int inicio = pos;
    while (isdigit((unsigned char)fonte[pos])) pos++;
    int tamanho = pos - inicio;
    strncpy(token.valor, fonte + inicio, tamanho);
    token.valor[tamanho] = '\0';
    token.tipo = TOKEN_NUM;
    return token;
}

void proximo_token() {
    ignora_espaco();
    char c = fonte[pos];
    if (c == '\0') {
        token_atual.tipo = TOKEN_FINAL;
        strcpy(token_atual.valor, "FINAL");
        return;
    }
    if (isalpha((unsigned char)c)) {
        token_atual = obter_token();
        return;
    }
    if (isdigit((unsigned char)c)) {
        token_atual = obter_numero();
        return;
    }
    switch (c) {
        case '=': token_atual.tipo = TOKEN_DECLA; strcpy(token_atual.valor, "="); pos++; break;
        case '+': token_atual.tipo = TOKEN_SOMA; strcpy(token_atual.valor, "+"); pos++; break;
        case '-': token_atual.tipo = TOKEN_SUB; strcpy(token_atual.valor, "-"); pos++; break;
        case '"': token_atual.tipo = TOKEN_ASPAS; strcpy(token_atual.valor, "\""); pos++; break;
        case ':': token_atual.tipo = TOKEN_DP; strcpy(token_atual.valor, ":"); pos++; break;
        default:
            token_atual.tipo = TOKEN_DESCONHECIDO;
            token_atual.valor[0] = c;
            token_atual.valor[1] = '\0';
            pos++;
            break;
    }
}

void consumir_token(token_tipos tipo_esperado) {
    if (token_atual.tipo == tipo_esperado) {
        proximo_token();
    } else {
        fprintf(stderr, "Erro: token inesperado '%s'\n", token_atual.valor);
        exit(EXIT_FAILURE);
    }
}

int eval_valor() {
    if (token_atual.tipo == TOKEN_NUM) {
        int v = atoi(token_atual.valor);
        consumir_token(TOKEN_NUM);
        return v;
    }
    fprintf(stderr, "Erro: esperado número na inicialização de variável, encontrado '%s'\n", token_atual.valor);
    exit(EXIT_FAILURE);
}

int eval_exp() {
    int v = eval_valor();
    while (token_atual.tipo == TOKEN_SOMA || token_atual.tipo == TOKEN_SUB) {
        token_tipos op = token_atual.tipo;
        consumir_token(op);
        int rhs = eval_valor();
        if (op == TOKEN_SOMA) v += rhs;
        else v -= rhs;
    }
    return v;
}

void parse_programa() {
    consumir_token(TOKEN_PROGRAMA);
    consumir_token(TOKEN_ASPAS);
    if (token_atual.tipo != TOKEN_VAR) {
        fprintf(stderr, "Erro: esperado identificador do programa\n");
        exit(EXIT_FAILURE);
    }
    consumir_token(TOKEN_VAR);
    consumir_token(TOKEN_ASPAS);
    consumir_token(TOKEN_DP);

    consumir_token(TOKEN_INICIO);
    fprintf(saida, ".DATA\n");
    while (token_atual.tipo == TOKEN_VAR) {
        char variavel[16];
        strcpy(variavel, token_atual.valor);
        consumir_token(TOKEN_VAR);
        consumir_token(TOKEN_DECLA);
        int val = eval_exp();
        fprintf(saida, "%s DB %d\n", variavel, val);
    }
    fprintf(saida, "R DB ?\n");
    fprintf(saida, ".CODE\n");
    fprintf(saida, ".ORG 0\n");

    consumir_token(TOKEN_RES);
    consumir_token(TOKEN_DECLA);
    parse_adicao();
    fprintf(saida, "STA R\n");

    consumir_token(TOKEN_FIM);
    fprintf(saida, "HLT\n");
}

void parse_adicao() {
    fprintf(saida, "LDA ");
    parse_valor();
    while (token_atual.tipo == TOKEN_SOMA || token_atual.tipo == TOKEN_SUB) {
        token_tipos op = token_atual.tipo;
        consumir_token(op);
        if (op == TOKEN_SOMA) fprintf(saida, "ADD ");
        else fprintf(saida, "SUB ");
        parse_valor();
    }
}

void parse_valor() {
    if (token_atual.tipo == TOKEN_NUM) {
        fprintf(saida, "%s\n", token_atual.valor);
        consumir_token(TOKEN_NUM);
    } else if (token_atual.tipo == TOKEN_VAR) {
        fprintf(saida, "%s\n", token_atual.valor);
        consumir_token(TOKEN_VAR);
    } else {
        fprintf(stderr, "Erro no factor: token inesperado '%s'\n", token_atual.valor);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv) {
    FILE *entrada = fopen(argv[argc - 1], "r");
    if (!entrada) return -1;
    char buffer[4096] = {0};
    fread(buffer, 1, sizeof(buffer)-1, entrada);
    fclose(entrada);

    saida = fopen("./compiler/programa.asm", "w");
    if (!saida) return -1;

    fonte = buffer;
    proximo_token();
    parse_programa();
    fclose(saida);
    return 0;
}