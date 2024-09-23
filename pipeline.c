#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lib.h"

#define MEMORY_SIZE 0X0000FF
uint16_t memoria[MEMORY_SIZE];

typedef struct {
    uint16_t pc;
    uint16_t registradores[8];
} Busca;

typedef struct {
    char tipo;
    uint16_t opcode;
    uint16_t reg_dest;
    uint16_t reg1;
    uint16_t reg2;
    uint16_t imediato;
} Decodifica;

typedef struct {
    uint16_t reg_dest;
    uint16_t resultado;
} Executa;

typedef struct {
    uint16_t reg_dest;
    uint16_t resultado;
} Writeback;

typedef struct {
    Busca busca;
    Decodifica decodifica;
    Executa executa;
    Writeback writeback;

} EstadoDaPipeline;

typedef enum {
    add = 0, sub = 1, mul = 2, div2 = 3, cmp_equal = 4, cmp_neq = 5,
    load = 15, store = 16, syscall = 63
} OpcodesR;

typedef enum {
    jump = 0, jump_cond = 1, mov = 3
} OpcodesI;


const char* nomes_do_opcode_r[] = {
        [add] = "add", [sub] = "sub", [mul] = "mul", [div2] = "div",
        [cmp_equal] = "cmp_equal", [cmp_neq] = "cmp_neq",
        [load] = "load", [store] = "store", [syscall] = "syscall"
};

const char* nomes_do_opcode_i[] = {
        [jump] = "jump", [jump_cond] = "jump_cond", [mov] = "mov"
};

void print_pc(const Busca *ondeEleTa) {
    printf("onde o processador ta: %p \n", ondeEleTa);
    printf("PC: %d\n", ondeEleTa->pc);
    for (int i = 0; i < 8; i++) {
        printf("%s: %d ", get_reg_name_str(i), ondeEleTa->registradores[i]);
        if (i % 4 == 3) printf("\n");
    }
    printf("\n");
}

uint16_t ULA(uint16_t reg1, uint16_t reg2, uint16_t opcode ) {
    switch (opcode) {
        case add:
            return reg1 + reg2;
        case sub:
            return reg1 - reg2;
        case mul:
            return reg1 * reg2;
        case div2:
            return reg1 / reg2;
        default:
            return 0;
    }
}

int main(const int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s [bin_name]\n", argv[0]);
        exit(1);
    }

    load_binary_to_memory(argv[1], memoria, sizeof(memoria));

    EstadoDaPipeline estado_da_pipeline = {0};

    while (1) {

        getchar();
    }

    return 0;
}