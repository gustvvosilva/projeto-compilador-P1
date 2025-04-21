#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

int main() {

    FILE *entrada = fopen("programa.lpn", "r");
    if(entrada == NULL) return -1;

    char buffer[1024];
    fread(buffer, 1, 1024, entrada);
    fclose(entrada);

    for(int i = 0; buffer[i] != 0; i++) {
        printf("%c ", buffer[i]);
    }
    printf("\n");

    return 0;
}