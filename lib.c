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
    for (uint32_t i = 0; i < tamanho; i++) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (memoria[i] >> j) & 1);
        }
        printf(" ");
        if ((i + 1) % 8 ==0) {
            printf("\n");
        }
    }
}