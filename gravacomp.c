/* Igor Soares Lemos 2011287 3WC */
/* Nome_do_Aluno2 Matricula Turma */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "gravacomp.h"

#define MAX_CAMPOS 50 // Arbitrário

typedef struct{ 
    char tipo;
    int tamanho;
}Campo; // Ex: Descritor "ius10" possui 3 campos -> 'i' [int], 'u' [unsigned int] e 's10' [string de 10 chars]

void escrever_big_endian(FILE *arquivo, uint32_t valor, int nbytes) {
    // Ex: valor = 0x12345678, com 4 nbytes, se deve escrever primeiro o 0x12 (mais significativo)
    for (int i = nbytes - 1; i >= 0; i--) {
        // Desloca os bits para a direita pra pegar só o byte desejado
        int deslocamento = i * 8;
        uint32_t byte = valor >> deslocamento;
        // Zera tudo menos o byte desejado
        byte = byte & 0xFF;
        // Escreve esse byte no arquivo
        fputc(byte, arquivo);
    }
}

uint32_t ler_big_endian(FILE *arquivo, int nbytes) {
    return 0;
}

int gravacomp(int nstructs, void* valores, char* descritor, FILE* arquivo) {
    int len = strlen(descritor);
    Campo campos[MAX_CAMPOS];
    int numCampos = 0;
    
    // Percorre o descritor e preenche "campos"
    for (int i = 0; i < len; i++) {
        if (isalpha(descritor[i])) {
            campos[numCampos].tipo = descritor[i];
            campos[numCampos].tamanho = 0;
        if (descritor[i] == 's') {
            int tamanho = (descritor[i+1] - '0') * 10 + (descritor[i+2] - '0'); // Estilo LAB2
            campos[numCampos].tamanho = tamanho;
            i += 2; // Avança dois dígitos
        }
            numCampos++;
        }
    }

    // Printa resultado
    printf("Resultado:\n");
    for (int i = 0; i < numCampos; i++) {
        if (campos[i].tipo == 's') {
            printf("Campo %d: tipo %c, tamanho = %d\n", i, campos[i].tipo, campos[i].tamanho);
        } else {
            printf("Campo %d: tipo %c\n", i, campos[i].tipo);
        }
    }
    
    // Escreve o número de structs no primeiro byte do arquivo
    fputc((unsigned char)nstructs, arquivo);

    return 0;
}

void mostracomp(FILE* arquivo) {

}
