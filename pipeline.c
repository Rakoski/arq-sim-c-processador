#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "lib.h"

#define MEMORY_SIZE 0X0000FF
#include "preditor.h"
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
    uint16_t pc;
    uint16_t registradores[8];
    PreditorDoDesvio previsao;
    struct {
        uint16_t busca;
        uint16_t decodifica;
        uint16_t executa;
        uint16_t writeback;
    } mini_pcs;
} EstagioDaPipe;

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

void atualiza_mini_pcs(EstagioDaPipe *estado) {
    estado->mini_pcs.writeback = estado->mini_pcs.executa;
    estado->mini_pcs.executa = estado->mini_pcs.decodifica;
    estado->mini_pcs.decodifica = estado->mini_pcs.busca;
    estado->mini_pcs.busca = estado->busca.pc;
}

void print_mini_pcs(const EstagioDaPipe *estado) {
    printf("Mini-PCs:\n");
    printf("  Busca: %d\n", estado->mini_pcs.busca);
    printf("  Decodifica: %d\n", estado->mini_pcs.decodifica);
    printf("  Executa: %d\n", estado->mini_pcs.executa);
    printf("  Writeback: %d\n", estado->mini_pcs.writeback);
}

void print_pc(const EstagioDaPipe *estado) {
//    printf("PC: %d\n", estado->busca.pc);
    for (int i = 0; i < 8; i++) {
        printf("%s: %d ", get_reg_name_str(i), estado->registradores[i]);
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

void busca(EstagioDaPipe *estado, const uint16_t *memoria) {
    printf("PC: %d\n", estado->pc);
    estado->busca.instrucao = memoria[estado->pc];
    estado->mini_pcs.busca = estado->pc;
    estado->pc++;
}

void decodifica(EstagioDaPipe *estado_da_pipeline) {
    uint16_t instrucao = estado_da_pipeline->busca.instrucao;
    Decodifica *decodifica = &estado_da_pipeline->decodifica;

    decodifica->tipo = extract_bits(instrucao, 15, 1) ? 'I' : 'R';
    if (decodifica->tipo == 'R') {
        decodifica->opcode = extract_bits(instrucao, 9, 6);
        decodifica->reg_dest = extract_bits(instrucao, 6, 3);
        decodifica->reg1 = extract_bits(instrucao, 3, 3);
        decodifica->reg2 = extract_bits(instrucao, 0, 3);
        decodifica->imediato = 0;

        printf("Tipo: R\n");
        printf("Opcode: %s ", nomes_do_opcode_r[estado_da_pipeline->decodifica.opcode]);
        printf("%d: \n", estado_da_pipeline->decodifica.opcode);

//        printf("tipo: R ");
//        printf("opcode: ", decodifica->opcode);
        printf("reg_dest: r%d\n", decodifica->reg_dest);
        printf("reg1: r%d\n", decodifica->reg1);
        printf("reg2: r%d\n", decodifica->reg2);
    } else {
        decodifica->opcode = extract_bits(instrucao, 13, 2);
        decodifica->reg_dest = extract_bits(instrucao, 10, 3);
        decodifica->imediato = extract_bits(instrucao, 0, 10);
        decodifica->reg1 = 0;
        decodifica->reg2 = 0;

        printf("Tipo: I\n");
        printf("Opcode: %s ", nomes_do_opcode_i[estado_da_pipeline->decodifica.opcode]);
        printf("%d: \n", estado_da_pipeline->decodifica.opcode);

//        printf("tipo: I ");
//        printf("opcode: ", decodifica->opcode);
        printf("reg_dest: r%d\n", decodifica->reg_dest);
        printf("imediato: %d\n", decodifica->imediato);
    }
}

void executa(EstagioDaPipe *estado_da_pipeline) {
    Decodifica *dec = &estado_da_pipeline->decodifica;
    Executa *exe = &estado_da_pipeline->executa;

    if (dec->tipo == 'R') {
        switch (dec->opcode) {
            case add:
            case sub:
            case mul:
            case div2:
                exe->resultado = ULA(estado_da_pipeline->registradores[dec->reg1],
                                     estado_da_pipeline->registradores[dec->reg2],
                                     dec->opcode);
                exe->reg_dest = dec->reg_dest;
                break;

        }
    } else if (dec->tipo == 'I') {
        switch (dec->opcode) {
            case mov:
                exe->resultado = dec->imediato;
                exe->reg_dest = dec->reg_dest;
                break;
        }
    }
}

void writeback(EstagioDaPipe *estado) {
    Executa *exe = &estado->executa;
    Writeback *wb = &estado->writeback;

    wb->reg_dest = exe->reg_dest;
    wb->resultado = exe->resultado;
    estado->registradores[wb->reg_dest] = wb->resultado;
}

void inicializa_pipeline(EstagioDaPipe *estado) {
    memset(estado, 0, sizeof(EstagioDaPipe));
    estado->pc = 0;
}

int main(const int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s [bin_name]\n", argv[0]);
        exit(1);
    }

    load_binary_to_memory(argv[1], memoria, sizeof(memoria));

    EstagioDaPipe estado = {0};
    PreditorDoDesvio preditor;
    inicializa_preditor(&preditor);

    estado.busca.pc = 0;

    while (1) {
        atualiza_mini_pcs(&estado);

        writeback(&estado);
        executa(&estado);
        decodifica(&estado);

        int previsao = preve_desvio(&preditor, estado.busca.pc);
        if (previsao) {
            estado.busca.pc = preditor.tabela[estado.busca.pc % BHT_SIZE].alvo;
        }
        busca(&estado, memoria);

        if (estado.decodifica.tipo == 'I' &&
            (estado.decodifica.opcode == jump || estado.decodifica.opcode == jump_cond)) {
            int tomado = (estado.decodifica.opcode == jump) ||
                         (estado.decodifica.opcode == jump_cond &&
                          estado.registradores[estado.decodifica.reg_dest] != 0);
            atualiza_preditor(&preditor, estado.busca.pc - 1, tomado,
                              estado.decodifica.imediato);
        }

        print_pc(&estado);
        print_mini_pcs(&estado);

        getchar();
    }
    return 0;
}
