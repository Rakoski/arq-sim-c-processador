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

        printf("  Completo: ");
        for (int j = 15; j >= 0; j--) {
            printf("%d", (instrucao & (1 << j)) ? 1 : 0);
        }
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
        for (int j = 8; j >= 6; j--) {
            printf("%d", (instrucao & (1 << j)) ? 1 : 0);
        }
        printf("\n");

        printf("  Operando 1:   ");
        for (int j = 5; j >= 3; j--) {
            printf("%d", (instrucao & (1 << j)) ? 1 : 0);
        }
        printf("\n");

        printf("  Operando 2:   ");
        for (int j = 2; j >= 0; j--) {
            printf("%d", (instrucao & (1 << j)) ? 1 : 0);
        }
        printf("\n\n");
    }
}