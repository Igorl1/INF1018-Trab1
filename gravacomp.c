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
}Token; // Ex: Descritor "ius10" possui 3 tokens: int, unsigned int e string de 10 char

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

int qnt_de_bytes(int valor, int is_unsigned) {
    // Determina quantos bytes vão ser gravados no arquivo
    int ret;
    if (is_unsigned) {
        unsigned int uvalor = (unsigned int) valor;
        if (uvalor <= 0xFF) // Até o 255
            ret = 1;
        else if (uvalor <= 0xFFFF) // Até 2^16
            ret = 2;
        else if (uvalor <= 0xFFFFFF) // Até 2^32
            ret = 3;
        else // Até 2^64
            ret = 4;
    } else { // Se signed
        if (valor >= -128 && valor <= 127)
            ret = 1;
        else if (valor >= -32768 && valor <= 32767)
            ret = 2;
        else if (valor >= -8388608 && valor <= 8388607)
            ret = 3;
        else
            ret = 4;
    }
    return ret;
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
    
    // Escreve o número de structs no primeiro byte do arquivo
    fputc((unsigned char)nstructs, arquivo);
  
    // Percorre as structs e grava cada campo de acordo com o descritor
    unsigned char* base = (unsigned char*)valores; // Ponteiro base pra percorrer memória do vetor valores
    for (int i = 0; i < nstructs; i++) {
        for (int j = 0; j < numTokens; j++) {
            if (tokens[j].tipo == 's') {
                char* str = (char*) base;
                int len = strlen(str);
                unsigned char cabecalho = 0;
                if (j == numTokens - 1) // É o último campo?
                    cabecalho = cabecalho | (1 << 7); // Se sim, coloca 1 no bit 7 do cabeçalho
                cabecalho = cabecalho | (1 << 6); // Coloca 1 no bit 6
                for (int k = 0; k < tokens[j].tamanho; k++) {
                    cabecalho = cabecalho | (1 << (5 - k)); // Liga os bits de 5 a 0 conforme o tamanho
                }
                fputc(cabecalho, arquivo);
                fwrite(str, 1, len, arquivo);
                base += tokens[j].tamanho; // Avança o ponteiro respeitando o array original
            }
            else if (tokens[j].tipo == 'i' || tokens[j].tipo == 'u') {
                unsigned int val = 0;
                memcpy(&val, base, sizeof(int)); // Copia os dados de base para val
                int nbytes = qnt_de_bytes(val, tokens[j].tipo == 'i'); // Calcula o número de bytes necessários para armazenar o valor de val
                unsigned char cabecalho = 0;
                if (j == numTokens - 1)
                    cabecalho = cabecalho | (1 << 7);
                if (tokens[j].tipo == 'i')
                    cabecalho = cabecalho | (1 << 5); // 01 para signed
                if (tokens[j].tipo == 'u')
                    cabecalho = cabecalho | (0 << 5); // 00 para unsigned
                for (int k = 0; k < 5; k++) {
                    cabecalho = cabecalho | ((nbytes & (1 << k)) << (k)); // Liga os bits de 4 a 0 conforme nbytes
                }
                fputc(cabecalho, arquivo);
                escrever_big_endian(arquivo, val, nbytes);
                base += sizeof(int); // Aqui tem que pular 4 bytes
            }
        }
    }
    return 0;
}

void mostracomp(FILE* arquivo) {
    unsigned char nstructs;
    fread(&nstructs, 1, 1, arquivo);
    printf("Estruturas: %d\n", nstructs);
}
