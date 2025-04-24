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
  
    // Percorre as structs e grava cada campo de acordo com o descrito
    //testando para 1 struct
    //transformando o tipo da struct
    unsigned char* base = (unsigned char*)valores;
    int offset = 0;
    base+=4;//avançando 4 byts, pois a string é o segundo elemento. foi usado apenas para teste
    for (int j = 0; j < numTokens; j++) {
        if (tokens[j].tipo == 's') {
            //escrevendo o cabeçalho
            unsigned char temp = 0;
            int tamanho = tokens[j].tamanho;
            //confere se é o ultimo elemento da struct
            if(numTokens ==j){
              temp = temp | (1 << 7);
            }
            //ligando o bit 6
            temp = temp | (1 << 6);
            //ligando os bit(5-0) de acordo com o tamanho da string
            for (int i = 0; i < tamanho; i++) {
              temp = temp | (1 << (5 - i));  // Ligar os bits de 5 a 0 conforme o tamanho
            }
            //add o cabeçalho no arquivo
            fputc(temp, arquivo);
            printf("imprimindo o byte cabeçalho\n");
            unsigned char a = temp;
            for (int b = 7; b >= 0; b--) {
                    printf("%d", (a >> b) & 1);
                }
            printf("\n");
            
            
            printf("Campo string (tamanho %d) em bits:\n", tamanho);
            for (int i = 0; i < tamanho; i++) {
                unsigned char c = base[offset + i];  // acessa diretamente a posição correta
                fputc(c, arquivo);
                for (int b = 7; b >= 0; b--) {
                    printf("%d", (c >> b) & 1);
                }
                printf(" ");
            }
            offset += tamanho;  // avança o offset manualmente
            printf("\n");
        }
        // se tiver outros tipos depois, você soma ao offset o tamanho correspondente também
    }

    
    
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
