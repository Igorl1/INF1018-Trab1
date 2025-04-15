/* Igor Soares Lemos 2011287 3WC */
/* Nome_do_Aluno2 Matricula Turma */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gravacomp.h"

// Escreve um valor em big endian no arquivo
void escrever_big_endian(FILE *arquivo, uint32_t val, int nbytes) {

}

// Lê valor big endian
uint32_t ler_big_endian(FILE *arquivo, int nbytes) {
    return 0;
}

int gravacomp(int nstructs, void* valores, char* descritor, FILE* arquivo) {
       int len = strlen(descritor);
    char* type[10]; // até 10 elementos
    int cont = -1;

    for (int i = 0; i < len; i++) {
        if (isalpha(descritor[i])) {
            // nova letra = novo tipo
            cont++;
            type[cont] = (char*)malloc(10 * sizeof(char)); // tamanho suficiente
            type[cont][0] = descritor[i];
            type[cont][1] = '\0';
        } else if (isdigit(descritor[i]) && cont >= 0) {
            // número = concatena ao último tipo
            int n = strlen(type[cont]);
            type[cont][n] = descritor[i];
            type[cont][n + 1] = '\0';
        }
    }

    // printa resultado
    printf("Resultado:\n");
    for (int i = 0; i <= cont; i++) {
        printf("type[%d] = %s\n", i, type[i]);
        free(type[i]);
    }

    return 0;
}

void mostracomp(FILE* arquivo) {

}
