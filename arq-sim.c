#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "lib.h"


#include <stdio.h>
#include <stdint.h>

// Opcode enums and name arrays (copied from lib.c)
typedef enum {
    add = 0, sub = 1, mul = 2, div2 = 3, cmp_equal = 4, cmp_neq = 5,
    load = 15, store = 16, syscall = 63
} OpcodeR;

typedef enum {
    jump = 0, jump_cond = 1, mov = 3
} OpcodeI;

const char* nomes_do_opcode_r[] = {
        [add] = "add", [sub] = "sub", [mul] = "mul", [div2] = "div",
        [cmp_equal] = "cmp_equal", [cmp_neq] = "cmp_neq",
        [load] = "load", [store] = "store", [syscall] = "syscall"
};

const char* nomes_do_opcode_i[] = {
        [jump] = "jump", [jump_cond] = "jump_cond", [mov] = "mov"
};

int binario_pra_decimal(const int binarios[], int tamanho_do_vetor) {
    int decimal = 0;
    for (int i = 0; i < tamanho_do_vetor; i++) {
        decimal += binarios[i] * (1 << (tamanho_do_vetor - 1 - i));
    }
    return decimal;
}

void print_binary(uint16_t instrucao) {
    printf("Instrução: ");
    for (int j = 15; j >= 0; j--) {
        printf("%d", (instrucao & (1 << j)) ? 1 : 0);
        if (j == 15 || j == 9 || j == 6 || j == 3) {
            printf(" ");
        }
    }
    printf("\n");

    char tipo = (instrucao & (1 << 15)) ? 'I' : 'R';
    printf("Formato: %c\n", tipo);

    if (tipo == 'R') {
        int opcode_bits[6];
        for (int j = 14; j >= 9; j--) {
            opcode_bits[14 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        int opcode = binario_pra_decimal(opcode_bits, 6);
        printf("Opcode: %d (%s)\n", opcode, nomes_do_opcode_r[opcode]);

        int dest_bits[3];
        for (int j = 8; j >= 6; j--) {
            dest_bits[8 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        printf("Destino: r%d\n", binario_pra_decimal(dest_bits, 3));

        int op1_bits[3];
        for (int j = 5; j >= 3; j--) {
            op1_bits[5 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        printf("Operando 1: r%d\n", binario_pra_decimal(op1_bits, 3));

        int op2_bits[3];
        for (int j = 2; j >= 0; j--) {
            op2_bits[2 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        printf("Operando 2: r%d\n", binario_pra_decimal(op2_bits, 3));
    } else {
        int opcode_bits[2];
        for (int j = 14; j >= 13; j--) {
            opcode_bits[14 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        int opcode = binario_pra_decimal(opcode_bits, 2);
        printf("Opcode: %d (%s)\n", opcode, nomes_do_opcode_i[opcode]);

        int reg_bits[3];
        for (int j = 12; j >= 10; j--) {
            reg_bits[12 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        printf("Registrador: r%d\n", binario_pra_decimal(reg_bits, 3));

        int imm_bits[10];
        for (int j = 9; j >= 0; j--) {
            imm_bits[9 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        printf("Imediato: %d\n", binario_pra_decimal(imm_bits, 10));
    }

    printf("\n");
}

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

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s [bin_name]\n", argv[0]);
        exit(1);
    }

    const uint32_t tamanho_da_memoria = 0x0000FFF;
    uint16_t *memory = calloc(tamanho_da_memoria, sizeof(uint16_t));
    if (!memory) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    load_binary_to_memory(argv[1], memory, tamanho_da_memoria);

    for(int i = 0; i < tamanho_da_memoria; i++) {
        printf("memory[%d] = ", i);
        print_binary(memory[i]);
    }



    // print_binarios_pra_debug(memory, tamanho_da_memoria);

    free(memory);  // Don't forget to free the allocated memory
    return 0;
}