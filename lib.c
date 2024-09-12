#include <stdio.h>
#include <stdlib.h>
#include "lib.h" //lembra do extract_bits

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

typedef struct {
    uint16_t pc;
    uint16_t registers[8];
} aondeOProcessadorEstaAgora;

char *get_reg_name_str(int i) {
    static char *reg_names[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    return reg_names[i];
}
