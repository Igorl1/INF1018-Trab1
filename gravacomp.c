/* Igor Soares Lemos 2011287 3WC */
/* Bruno Kubudi Cardeman 2132924 3WC */

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
}Campo; // Ex: Descritor "is05u" possui 3 campos: int, string de 5 char, unsigned int

void escrever_big_endian(FILE *arquivo, unsigned int valor, int nbytes) {
    // Ex: Valor 0x12345678, 4 nbytes, se escreve primeiro 0x12
    for (int i = nbytes - 1; i >= 0; i--) {
        int deslocamento = i*8;
        unsigned char byte = valor >> deslocamento; // Desloca para pegar só o byte desejado
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
        valor = valor << 8; // Desloca para abrir espaço pro novo byte
        valor = valor | byte; // Adiciona o byte lido na parte menos significativa
    }
    return valor;
}

int qnt_de_bytes_unsigned(int valor) {
    // Determina quantos bytes vão ser gravados no arquivo (Caso de unsigned)
    int ret;
    unsigned int uvalor = (unsigned int) valor;
    if (uvalor <= 0xFF) // Até o 255
        ret = 1;
    else if (uvalor <= 0xFFFF) // Até 2^16
        ret = 2;
    else if (uvalor <= 0xFFFFFF) // Até 2^32
        ret = 3;
    else // Até 2^64
        ret = 4;
    return ret;
}

int qnt_de_bytes_signed(int valor) {
    // (Caso de signed)
    int ret;
    if (valor >= -128 && valor <= 127)
        ret = 1;
    else if (valor >= -32768 && valor <= 32767)
        ret = 2;
    else if (valor >= -8388608 && valor <= 8388607)
        ret = 3;
    else
        ret = 4;
    return ret;
}

unsigned int alinha_offset(unsigned int offset, unsigned int alinhamento) {
    // Retorna qtd necessária para arredondar o offset para o próximo múltiplo do alinhamento
    unsigned int ajuste = alinhamento - 1;
    return (offset + ajuste) & ~ajuste;
}

void calc_paddings_do_descritor(const char *descritor, unsigned int paddings[], int *numCampos) {
    // Calcula o padding necessário entre os campos da struct, respeitando o alinhamento de cada tipo, e armazena os valores de padding em um vetor.
    unsigned int offset = 0;
    int indice = 0; // indice de preenhimento

    for (unsigned int i = 1; i < strlen(descritor); ) {
        char type = descritor[i];
        unsigned int tam = 0, alinhamento = 0;
        if (type == 'i' || type == 'u') {
            tam = 4;
            alinhamento = 4;
            i += 1;
        } 
        else if (type == 's') {
            if (isdigit(descritor[i + 1]) && isdigit(descritor[i + 2])) {
                int len = (descritor[i + 1] - '0') * 10 + (descritor[i + 2] - '0'); // Estilo LAB2
                tam = len;
                alinhamento = 1;
                i += 3;
            }
        }
        unsigned int offsetAlinhado = alinha_offset(offset, alinhamento);
        paddings[indice++] = offsetAlinhado - offset;
        offset = offsetAlinhado + tam;
    }

    // Padding final (fim da struct)
    unsigned int final_align = 4; // alinhamento da struct
    unsigned int final_size = alinha_offset(offset, final_align);
    paddings[indice++] = final_size - offset;

    *numCampos = indice;
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
    
    // Escreve o número de structs no primeiro byte do arquivo
    fputc((unsigned char)nstructs, arquivo);
  
    // Percorre as structs e grava cada campo de acordo com o descritor
    unsigned char* base = (unsigned char*)valores; // Ponteiro base pra percorrer memória do vetor valores
    
    unsigned int paddings[MAX_CAMPOS];
    
    int n;
    calc_paddings_do_descritor(descritor, paddings, &n);
    for (int i = 0; i < nstructs; i++) {
        for (int j = 0; j < numCampos; j++) {
            int ultimo_campo = (j == numCampos - 1);
            if (campos[j].tipo == 's') {
                char* str = (char*) base;
                int len = strlen(str);
                if (len > 63)
                    len = 63;
                
                unsigned char cabecalho = 0;
                if (ultimo_campo)
                    cabecalho = cabecalho | (1 << 7); // Se sim, coloca 1 no bit 7 do cabeçalho
                cabecalho = cabecalho | (1 << 6); // Tipo = string
                cabecalho = cabecalho | (len & 0x3F); // Len limitado a 6 bits, usando máscara de 6 bits (00111111). Tamanho até 63
                
                fputc(cabecalho, arquivo);
                fwrite(str, 1, len, arquivo);
                base += campos[j].tamanho; // Avança o ponteiro respeitando o array original
            }
            else if (campos[j].tipo == 'i' || campos[j].tipo == 'u') {
                unsigned int val = 0;
                memcpy(&val, base, sizeof(int)); // Copia os dados de base para val
                int nbytes = 0;
                if (campos[j].tipo == 'i')
                    nbytes = qnt_de_bytes_signed(val); // bytes necessários para armazenar o valor de val
                else if (campos[j].tipo == 'u')
                    nbytes = qnt_de_bytes_unsigned(val);
                unsigned char cabecalho = 0;
                if (ultimo_campo)
                    cabecalho = cabecalho | (1 << 7);
                if (campos[j].tipo == 'i')
                    cabecalho = cabecalho | (1 << 5); // 01 para signed
                if (campos[j].tipo == 'u')
                    cabecalho = cabecalho | (0 << 5); // 00 para unsigned
                cabecalho |= (nbytes & 0x1F); // Nbytes limitado a 5 bits. (00011111)
                fputc(cabecalho, arquivo);
                escrever_big_endian(arquivo, val, nbytes);
                base += sizeof(int); // Aqui tem que pular 4 bytes
            }
            base += paddings[j];
        }
    }
    return 0;
}

void mostracomp(FILE* arquivo) {
    // Lê o número de structs no primeiro byte
    unsigned char nstructs;
    fread(&nstructs, 1, 1, arquivo);
    printf("Estruturas: %d\n\n", nstructs);

    for (int i = 0; i < nstructs; i++) {
        int fim_struct = 0;
        while (!fim_struct) {
            unsigned char cabecalho;
            fread(&cabecalho, 1, 1, arquivo); // Lê o cabeçalho de 1 byte
            
            fim_struct = (cabecalho >> 7) & 1; // Bit 7 indica se é o último campo
            int tipo = (cabecalho >> 6) & 1;   // Bit 6: 1 = string, 0 = inteiro
            int subtipo = (cabecalho >> 5) & 1; // Bit 5 (só para int): 0 = unsigned, 1 = signed
            int tam = cabecalho & 0x1F; // Bits 0–4 (ou 0–5 se string): tamanho (máx 31 ou 63)
            
            if (tipo == 1) { // Campo string
                char buffer[64];
                fread(buffer, 1, tam, arquivo); // Lê 'tam' caracteres
                buffer[tam] = '\0'; // Termina com \0
                printf("(str): %s\n", buffer);
            } 
            else { // Campo signed ou unsigned
                unsigned int val = ler_big_endian(arquivo, tam);
                
                if (subtipo == 0) {
                    // unsigned
                    printf("(uns): %u (%08x)\n", val, val);
                } else {
                    // signed
                    int svalor = val;
                    
                    // Se o bit mais significativo do valor estiver ligado, então é número negativo
                    if (tam == 1 && (val & 0x80))
                        svalor |= 0xFFFFFF00;
                    else if (tam == 2 && (val & 0x8000))
                        svalor |= 0xFFFF0000;
                    else if (tam == 3 && (val & 0x800000))
                        svalor |= 0xFF000000;
                    // tam == 4 não precisa ajustar, pq já é int
                    
                    printf("(int): %d (%08x)\n", svalor, svalor);
                }
            }
        }
        printf("\n");
    }
}
