#include <stdio.h>
#include <stdlib.h>

#include "lib.h"

void load_binary_to_memory (const char *fname, void *memory, const uint32_t mem_size)
{
    FILE *fp;
    uint32_t bsize;

    fp = fopen(fname, "rb");

    if (!fp) {
        printf("cannot open file %s\n", fname);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    bsize = ftell(fp);

    printf("file %s has %u bytes\n", fname, bsize);

    if (bsize > mem_size) {
        printf("error!\nMax file fize must be 0x0000FFFF\n");
        exit(1);
    }

    rewind(fp);

    if (fread(memory, 1, bsize, fp) != bsize) {
        printf("error loading file %s\n", fname);
        exit(1);
    }

    fclose(fp);
}

int decimal_pra_binario(int binario1, int binario2, int binario3) {
    int decimal1 = binario1 * (2 * 2);
    int decimal2 = binario2 * (2 * 1);
    int decimal3 = binario3 * (2 * 0);
    return decimal1 + decimal2 + decimal3;
}

void print_binarios_pra_debug(const uint32_t *memoria, uint32_t tamanho) {
    printf("iniciando print da arquitetura de 16 bits");
    for (uint32_t i = 0; i < tamanho; i++) {
        int cont1 = 0;
        uint16_t instrucao = memoria[i];

        for (int j = 16; j >= 0; j--) {
            if ((instrucao & (1 << j)) == 1) {
                cont1++;
            }
        }

        if (cont1 == 0) {
            continue; //ignorando instruções só com 0s
        }

        printf("instrução: %d", i);
        printf("\n");

        printf("  Formato: ");
        for (int j = 15; j >= 15; j--) {
            printf("%d", (instrucao & (1 << j)) ? 1 : 0);
        }
        printf("\n");

        printf("  Opcode: ");
        for (int j = 14; j >= 9; j--) {
            printf("%d", (instrucao & (1 << j)) ? 1 : 0);
        }
        printf("\n");

        printf("  Destino:  ");
        int binariodest1 = 0;
        int binariodest2 = 0;
        int binariodest3 = 0;
        for (int j = 8; j >= 6; j--) {
            if (j == 8) {
                binariodest1 = (instrucao & (1 << j)) ? 1 : 0;
            } else if (j == 7) {
                binariodest2 = (instrucao & (1 << j)) ? 1 : 0;
            } else {
                binariodest3 = (instrucao & (1 << j)) ? 1 : 0;
            }
        }
        printf("r%d", decimal_pra_binario(binariodest1, binariodest2, binariodest3));
        printf("\n");

        printf("  Operando 1:   ");
        int binarioop11 = 0;
        int binarioop12 = 0;
        int binarioop13 = 0;
        for (int j = 5; j >= 3; j--) {
            if (j == 5) {
                binarioop11 = (instrucao & (1 << j)) ? 1 : 0;
            } else if (j == 4) {
                binarioop12 = (instrucao & (1 << j)) ? 1 : 0;
            } else {
                binarioop13 = (instrucao & (1 << j)) ? 1 : 0;
            }
        }
        printf("r%d", decimal_pra_binario(binarioop11, binarioop12, binarioop13));
        printf("\n");

        int binarioop21 = 0;
        int binarioop22 = 0;
        int binarioop23 = 0;
        printf("  Operando 2:   ");
        for (int j = 2; j >= 0; j--) {
            if (j == 2) {
                binarioop21 = (instrucao & (1 << j)) ? 1 : 0;
            } else if (j == 1) {
                binarioop22 = (instrucao & (1 << j)) ? 1 : 0;
            } else {
                binarioop23 = (instrucao & (1 << j)) ? 1 : 0;
            }
        }
        printf("r%d", decimal_pra_binario(binarioop21, binarioop22, binarioop23));
        printf("\n");

        printf("  Completo: ");
        for (int j = 15; j >= 0; j--) {
            printf("%d", (instrucao & (1 << j)) ? 1 : 0);
            if (j == 15 || j == 9 || j == 6 || j == 3) {
                printf(" ");
            }
        }
        printf("\n\n");
    }
}