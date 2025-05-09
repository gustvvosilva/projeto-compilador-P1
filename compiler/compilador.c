#include "compilador.h"

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
    char valor[8];
} TOKEN;

const char *fonte;
int pos = 0;
TOKEN token_atual;
FILE *saida;

void ignora_espaco() {
    while (fonte[pos] == ' ' || fonte[pos] == '\t' || fonte[pos] == '\n') {
        pos++;
    }
}

// Função para identificar palavras reservadas ou identificadores
TOKEN obter_token() {
    TOKEN token;
    int inicio = pos;
    while (isalnum(fonte[pos])) { // permite letras e dígitos, se desejar, ou só letras
        pos++;
    }
    int tamanho = pos - inicio;
    strncpy(token.valor, fonte + inicio, tamanho);
    token.valor[tamanho] = '\0';
    
    // Checa palavras reservadas
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

// Função para ler números inteiros
TOKEN obter_numero() {
    TOKEN token;
    int inicio = pos;
    while (isdigit(fonte[pos])) {
        pos++;
    }
    int tamanho = pos - inicio;
    strncpy(token.valor, fonte + inicio, tamanho);
    token.valor[tamanho] = '\0';
    token.tipo = TOKEN_NUM;
    return token;
}

// Função principal do lexer para obter o próximo token
void proximo_token() {
    ignora_espaco();
    char c = fonte[pos];
    
    if (c == '\0') {
        token_atual.tipo = TOKEN_FINAL;
        strcpy(token_atual.valor, "FINAL");
        return;
    }

    // Identifica letras (palavras reservadas ou identificadores)
    if (isalpha(c)) {
        token_atual = obter_token();
        return;
    }
    
    // Identifica números
    if (isdigit(c)) {
        token_atual = obter_numero();
        return;
    }
    
    // Símbolos simples
    switch (c) {
        case '=':
            token_atual.tipo = TOKEN_DECLA;
            strcpy(token_atual.valor, "=");
            pos++;
            break;
        case '+':
            token_atual.tipo = TOKEN_SOMA;
            strcpy(token_atual.valor, "+");
            pos++;
            break;
        case '-':
            token_atual.tipo = TOKEN_SUB;
            strcpy(token_atual.valor, "-");
            pos++;
            break;
        case '\"':
            token_atual.tipo = TOKEN_ASPAS;
            strcpy(token_atual.valor, "\"");
            pos++;
            break;
        case ':':
            token_atual.tipo = TOKEN_DP;
            strcpy(token_atual.valor, ":");
            pos++;
            break;
        default:
            token_atual.tipo = TOKEN_DESCONHECIDO;
            token_atual.valor[0] = c;
            token_atual.valor[1] = '\0';
            pos++;
            break;
    }
}

// Consome o token atual e verifica se ele é do tipo esperado
void consumir_token(token_tipos tipo_esperado) {
    if (token_atual.tipo == tipo_esperado) {
        proximo_token();
    } else {
        printf("ERRO!\n");
        exit(EXIT_FAILURE);
    }
}

// Protótipos para as funções de parsing de expressões (respectiva à precedência)
void parse_adicao();
void parse_valor();
// void parse_factor();

// <program> ::= <label> "\n" <start> <statement> "\n" <res_statement> "\n" <end>
void parse_programa() {
    // <label> ::= "PROGRAMA " "\"" <var> "\":"
    consumir_token(TOKEN_PROGRAMA);
    // Espera a aspa de abertura:
    consumir_token(TOKEN_ASPAS);
    // O <var> (identificador) – assume que o token atual é um ID
    if (token_atual.tipo != TOKEN_VAR)
        printf("Esperava identificador no label do programa.\n");
    printf("; Definindo o programa: %s\n", token_atual.valor);
    consumir_token(TOKEN_VAR);
    // Espera a aspa de fechamento
    consumir_token(TOKEN_ASPAS);
    consumir_token(TOKEN_DP);
    
    // Consome a quebra de linha (opcional – você pode tratar \n como token ou ignorar)
    // Aqui, assumo que o lexer já ignora \n com skip_whitespace()

    // <start> ::= "INICIO"
    consumir_token(TOKEN_INICIO);
    fprintf(saida, ".DATA\n");
    
    // <statement> – pode ser uma ou várias atribuições
    while (token_atual.tipo == TOKEN_VAR) {
        // <ass_statement> ::= <ws> <var> <ws> "=" <ws> <exp>
        // Aqui, o token atual é um identificador (variável)
        char variavel[8];
        strcpy(variavel, token_atual.valor);
        consumir_token(TOKEN_VAR);
        consumir_token(TOKEN_DECLA);
        printf("; Processando atribuição para %s\n", variavel);
        fprintf(saida, "%s DB ", variavel);
        // Para a geração de código, você pode chamar uma função que gera o código para a expressão
        parse_adicao();
        // Após calcular a expressão, emita código para armazenar o resultado na variável (ex.: STA varName)
        printf("STA %s\n", variavel);
        // Suporta a quebra de linha entre statements
        // nextToken();   // se necessário
    }
    
    fprintf(saida, "R DB ?\n");
    fprintf(saida, ".CODE\n");
    fprintf(saida, ".ORG 0\n");

    // <res_statement> ::= <ws> "RES" <ws> "=" <ws> <exp>
    consumir_token(TOKEN_RES);
    consumir_token(TOKEN_DECLA);
    printf("; Processando instrução RES\n");
    parse_adicao();
    // Aqui, a geração pode ser, por exemplo, armazenar o resultado em uma área especial
    fprintf(saida, "STA R\n");
    printf("STA RES\n");  // ou a instrução apropriada

    // <end> ::= "FIM"
    consumir_token(TOKEN_FIM);
    fprintf(saida, "HLT\n");
}

// <exp> ::= <term> ( <ws> <addop> <ws> <term> )*
void parse_adicao() {
    // Começa interpretando o termo
    fprintf(saida, "LDA ");
    parse_valor();
    // Enquanto o token for um operador de adição ou subtração
    while (token_atual.tipo == TOKEN_SOMA || token_atual.tipo == TOKEN_SUB) {
        token_tipos op = token_atual.tipo;
        consumir_token(op);
        // Emite o código correspondente à operação:
        if (op == TOKEN_SOMA) {
            fprintf(saida, "ADD ");
            printf("ADD\n");
        } else {
            printf("SUB\n");
        }
        // Para geração de código, pode ser necessário empilhar o valor anterior, ou utilizar registradores.
        // Exemplo de comentário: // push acumulator (dependendo da estratégia)
        parse_valor();
    }
}

// <factor> ::= <num> | <var> | "(" <ws> <exp> <ws> ")"
void parse_valor() {
    if (token_atual.tipo == TOKEN_NUM) {
        // Emite código para carregar o número
        printf("LDC %s\n", token_atual.valor);
        fprintf(saida, "%s\n", token_atual.valor);
        consumir_token(TOKEN_NUM);
    } else if (token_atual.tipo == TOKEN_VAR) {
        // Emite código para carregar a variável
        printf("LDA %s\n", token_atual.valor);
        fprintf(saida, "%s\n", token_atual.valor);
        consumir_token(TOKEN_VAR);
    } else {
        printf("Erro no factor: token inesperado.\n");
    }
}

int main() {

    FILE *entrada = fopen("programa.lpn", "r");
    if(entrada == NULL) return -1;

    char buffer[1024] = {0};
    fread(buffer, 1, 1024, entrada);
    fclose(entrada);

    for(int i = 0; buffer[i] != 0; i++) {
        printf("%c", buffer[i]);
    }
    printf("\n");

    saida = fopen("programa.asm", "w");
    if(saida == NULL) return -1;

    // Inicializa o lexer
    fonte = buffer;
    pos = 0;
    proximo_token();
    
    // Inicia o parsing do programa
    parse_programa();

    fclose(saida);
    return 0;
}