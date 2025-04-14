#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gravacomp.h"

struct s {
    int i;
    char s1[5];
    unsigned int u;
};

int main() {
    struct s exemplo[2] = {
        { -1, "abc", 258 },
        { 1, "ABCD", 65535 }
    };

    FILE *fp = fopen("saida.bin", "wb");
    if (!fp) {
        perror("Erro na escrita.\n");
        return 1;
    }

    int res = gravacomp(2, exemplo, "is05u", fp);
    fclose(fp);

    if (res == -1) {
        printf("Erro na gravação.\n");
        return 1;
    }

    fp = fopen("saida.bin", "rb");
    if (!fp) {
        perror("Erro na leitura.\n");
        return 1;
    }
    
    mostracomp(fp);
    fclose(fp);
    return 0;
}
