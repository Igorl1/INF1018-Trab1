/* Igor Soares Lemos 2011287 3WC */
/* Nome_do_Aluno2 Matricula Turma */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "gravacomp.h"

#define MAX_TOKENS 50 // Arbitrário

typedef struct{ 
    char tipo;
    int tamanho;
}Token; // Ex: Descritor "ius10" possui 3 tokens: int, unsigned int e string de 10 chars

void escrever_big_endian(FILE *arquivo, unsigned int valor, int nbytes) {
    // Ex: Valor 0x12345678, 4 nbytes, se escreve primeiro 0x12
    for (int i = nbytes - 1; i >= 0; i--) {
        int deslocamento = i*8;
        unsigned char byte = valor >> deslocamento; // Desloca pra direita para pegar só o byte desejado
        byte = byte & 0xFF; // Zera tudo menos o byte desejado
        fputc(byte, arquivo);
    }
}

unsigned int ler_big_endian(FILE* arquivo, int nbytes) {
    // Ex: Se tiver 0x12 0x34 0x56 0x78 no arquivo, retorna 0x12345678
    unsigned int valor = 0;
    for (int i = 0; i < nbytes; i++) {
        unsigned char byte;
        fread(&byte, 1, 1, arquivo); // Lê 1 byte do arquivo
        valor = valor << 8; // Desloca para esquerda pra abrir espaço pro novo byte
        valor = valor | byte; // Adiciona o byte lido na parte menos significativa
    }
    return valor;
}

int gravacomp(int nstructs, void* valores, char* descritor, FILE* arquivo) {
    int len = strlen(descritor);
    Token tokens[MAX_TOKENS];
    int numTokens = 0;
    
    // Percorre o descritor e preenche "tokens"
    for (int i = 0; i < len; i++) {
        if (isalpha(descritor[i])) {
            tokens[numTokens].tipo = descritor[i];
            tokens[numTokens].tamanho = 0;
        if (descritor[i] == 's') {
            int tamanho = (descritor[i+1] - '0') * 10 + (descritor[i+2] - '0'); // Estilo LAB2
            tokens[numTokens].tamanho = tamanho;
            i += 2; // Avança dois dígitos
        }
            numTokens++;
        }
    }
    
    // Printa resultado
    printf("Resultado:\n");
    for (int i = 0; i < numTokens; i++) {
        if (tokens[i].tipo == 's') {
            printf("Campo %d: tipo %c, tamanho = %d\n", i, tokens[i].tipo, tokens[i].tamanho);
        } else {
            printf("Campo %d: tipo %c\n", i, tokens[i].tipo);
        }
    }
    
    // Escreve o número de structs no primeiro byte do arquivo
    fputc((unsigned char)nstructs, arquivo);
    
    // Percorre as structs e grava cada campo de acordo com o descritor
    /*
    for (int i = 0; i < nstructs; i++) {
    }
    */

    return 0;
}

void mostracomp(FILE* arquivo) {
    unsigned char nstructs;
    fread(&nstructs, 1, 1, arquivo);
    printf("Estruturas: %d\n", nstructs);
}
