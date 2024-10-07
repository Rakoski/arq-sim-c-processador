#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lib.h"

#define MEMORY_SIZE 0X0000FF
uint16_t memoria[MEMORY_SIZE];

typedef struct {
    uint16_t pc;
    uint16_t instrucao;
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
    uint16_t registradores[8];
} EstagioDaPipe;

typedef struct {
    int previsor // -1 -> não pula, 0
} Previsao;

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

void print_pc(const Busca *ondeEleTa, const EstagioDaPipe *estado_da_pipeline) {
    printf("onde o processador ta: %p \n", ondeEleTa);
    printf("PC: %d \n", ondeEleTa->pc - 1);
    for (int i = 0; i < 8; i++) {
        printf("%s: %d  ", get_reg_name_str(i), estado_da_pipeline->registradores[i]);
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

void busca(EstagioDaPipe *estado_da_pipeline, const uint16_t *memoria) {
    estado_da_pipeline->busca.instrucao = memoria[estado_da_pipeline->busca.pc];
    estado_da_pipeline->busca.pc++;
}

void decodifica(EstagioDaPipe *estado_da_pipeline) {
    uint16_t instrucao = estado_da_pipeline->busca.instrucao;
    Decodifica *decodifica = &estado_da_pipeline->decodifica;

    printf("Instrução: 0x%04X\n", estado_da_pipeline->busca.instrucao);

    decodifica->tipo = extract_bits(instrucao, 15, 1) ? 'I' : 'R';
    if (decodifica->tipo == 'R') {
        decodifica->opcode = extract_bits(instrucao, 9, 6);
        decodifica->reg_dest = extract_bits(instrucao, 6, 3);
        decodifica->reg1 = extract_bits(instrucao, 3, 3);
        decodifica->reg2 = extract_bits(instrucao, 0, 3);
        decodifica->imediato = 0;

        printf("Tipo: R\n");
        printf("Opcode: %s\n", nomes_do_opcode_r[estado_da_pipeline->decodifica.opcode]);

//        printf("tipo: R ");
//        printf("opcode: ", decodifica->opcode);
//        printf("reg_dest: %d\n", decodifica->reg_dest);
//        printf("reg1: %d\n", decodifica->reg1);
//        printf("reg2: %d\n", decodifica->reg2);
    } else {
        decodifica->opcode = extract_bits(instrucao, 13, 2);
        decodifica->reg_dest = extract_bits(instrucao, 10, 3);
        decodifica->imediato = extract_bits(instrucao, 0, 10);
        decodifica->reg1 = 0;
        decodifica->reg2 = 0;

        printf("Tipo: I\n");
        printf("Opcode: %s\n", nomes_do_opcode_i[estado_da_pipeline->decodifica.opcode]);

//        printf("tipo: I ");
//        printf("opcode: ", decodifica->opcode);
//        printf("reg_dest: %d\n", decodifica->reg_dest);
//        printf("imediato: %d\n", decodifica->imediato);
    }
}

int main(const int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s [bin_name]\n", argv[0]);
        exit(1);
    }

    load_binary_to_memory(argv[1], memoria, sizeof(memoria));

    EstagioDaPipe estado_da_pipeline = {0};

    while (1) {
        decodifica(&estado_da_pipeline);
        busca(&estado_da_pipeline, memoria);
        print_pc(&estado_da_pipeline.busca, &estado_da_pipeline);
        decodifica(&estado_da_pipeline);

        //impressões de depuracao
        printf("Reg Dest: %d\nReg1: %d\nReg2: %d\nImediato: %d\n",
               estado_da_pipeline.decodifica.reg_dest,
               estado_da_pipeline.decodifica.reg1,
               estado_da_pipeline.decodifica.reg2,
               estado_da_pipeline.decodifica.imediato);

        getchar();
    }

    return 0;
}