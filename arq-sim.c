#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "lib.h"

const char* get_reg_name_str (uint16_t reg)
{
    static const char *str[] = {
        "r0",
        "r1",
        "r2",
        "r3",
        "r4",
        "r5",
        "r6",
        "r7"
    };

    return str[reg];
}

int main (int argc, char **argv /* vulgo arquivo q vai ser compilado */)
{
    if (argc != 2) {
        printf("usage: %s [bin_name]\n", argv[0]);
        exit(1);
    }

    const uint32_t tamanho_da_memoria = 0x0000FFF;
    uint32_t *memoria = malloc(tamanho_da_memoria);
    if (!memoria) {
        printf("não alocou");
        exit(1);
    }

    printf(argv[1]);
    load_binary_to_memory(argv[1], memoria, tamanho_da_memoria);

    print_binarios_pra_debug(memoria, tamanho_da_memoria);
    free(memoria);

    return 0;
}