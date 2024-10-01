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

void print_pc(const Busca *ondeEleTa, const EstadoDaPipeline *estado_da_pipeline) {
    printf("onde o processador ta: %p \n", ondeEleTa);
    printf("PC: %d\n", ondeEleTa->pc);
    for (int i = 0; i < 8; i++) {
        printf("%s: %d ", get_reg_name_str(i), estado_da_pipeline->registradores[i]);
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

void busca(EstadoDaPipeline *estado_da_pipeline, const uint16_t *memoria) {
    estado_da_pipeline->busca.instrucao = memoria[estado_da_pipeline->busca.pc];
    estado_da_pipeline->busca.pc++;
}

void decodifica(EstadoDaPipeline *estado) {
  uint16_t instrucao = estado->busca.instrucao;

  //determinar tipo de instrução(r ou i)
  estado->decodifica.tipo = (instrucao >> 15) ? 'I' : 'R';

  if (estado->decodifica.tipo == 'R') {
    estado->decodifica.opcode = (instrucao >> 9) & 0x3F; // 6 bits do opcode
    estado->decodifica.reg_dest = (instrucao >> 6) & 0x7; // 3 bits do reg_dest
    estado->decodifica.reg1 = (instrucao >> 3) & 0x7; // 3 bits do reg1
    estado->decodifica.reg2 = instrucao & 0x7; // 3 bits do reg2
    estado->decodifica.imediato = 0; // não se usa imediato em instruções 'R'
  } else {
    estado->decodifica.opcode = (instrucao >> 13) & 0x3; // 2 bits do opcode
    estado->decodifica.reg_dest = (instrucao >> 10) & 0x7; // 3 bits do reg_dest
    estado->decodifica.imediato = instrucao & 0x3FF; // 10 bits do imediato
    estado->decodifica.reg1 = 0; // não se usa reg1 em instruções 'I'
    estado->decodifica.reg2 = 0; // o mesmo se aplica para reg 2
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
        busca(&estado_da_pipeline, memoria);
        decodifica(&estado_da_pipeline);

        //impressões de depuracao
        printf("Instrução: 0x%04X\n", estado_da_pipeline.busca.instrucao);
        printf("Tipo: %c\n", estado_da_pipeline.decodifica.tipo);
        printf("Opcode: %d\n", estado_da_pipeline.decodifica.opcode);
        printf("Reg Dest: %d\n Reg1: %d\n Reg2: %d\n Imediato: %d\n",
               estado_da_pipeline.decodifica.reg_dest,
               estado_da_pipeline.decodifica.reg1,
               estado_da_pipeline.decodifica.reg2,
               estado_da_pipeline.decodifica.imediato);

        getchar();
    }

    return 0;
}