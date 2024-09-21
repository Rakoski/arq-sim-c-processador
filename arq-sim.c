#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lib.h"

typedef struct {
    uint16_t pc;
    uint16_t registradores[8];
} AondeProgramCounterEsta;

typedef struct {
    char tipo;
    uint16_t opcode;
    uint16_t reg_dest;
    uint16_t reg1;
    uint16_t reg2;
    uint16_t imediato;
} InstrucaoDecodada;

// acessa e pega o que ta naquele endereço de memória (do ponteiro do struct AondeProgramCounterEsta que o pc e o  ta)
void print_pc(const AondeProgramCounterEsta *ondeEleTa) {
    printf("onde o processador ta: %p \n", ondeEleTa);
    printf("PC: %d\n", ondeEleTa->pc);
    for (int i = 0; i < 8; i++) {
        printf("%s: %d ", get_reg_name_str(i), ondeEleTa->registradores[i]);
        if (i % 4 == 3) printf("\n");
    }
    printf("\n");
}

// nao da pra colocar div como chave em algum enum por causa do tipo div_t em c, assim opto por colocar div2
// pelo q entendi structs é tipo hashmaps chave e valor
typedef enum {
    add = 0, sub = 1, mul = 2, div2 = 3, cmp_equal = 4, cmp_neq = 5,
    load = 15, store = 16, syscall = 63
} OpcodesR;

typedef enum {
    jump = 0, jump_cond = 1, mov = 3
} OpcodesI;


// pega os nomes a partir das structs definidas em cima
const char* nomes_do_opcode_r[] = {
        [add] = "add", [sub] = "sub", [mul] = "mul", [div2] = "div",
        [cmp_equal] = "cmp_equal", [cmp_neq] = "cmp_neq",
        [load] = "load", [store] = "store", [syscall] = "syscall"
};

const char* nomes_do_opcode_i[] = {
        [jump] = "jump", [jump_cond] = "jump_cond", [mov] = "mov"
};

InstrucaoDecodada decodificador(uint16_t instrucao) {
    InstrucaoDecodada decodificacao;
    const uint16_t bit_formato = extract_bits(instrucao, 15, 1);
    decodificacao.tipo = bit_formato ? 'I' : 'R';
    printf("formato: %c\n", decodificacao.tipo);

    if (decodificacao.tipo == 'R') {
        decodificacao.opcode = extract_bits(instrucao, 9, 6);
        decodificacao.reg_dest = extract_bits(instrucao, 6, 3);
        decodificacao.reg1 = extract_bits(instrucao, 3, 3);
        decodificacao.reg2 = extract_bits(instrucao, 0, 3);

        printf("opcode: %d %s\n", decodificacao.opcode, nomes_do_opcode_r[decodificacao.opcode]);
        printf("reg_dest: r%d\n", decodificacao.reg_dest);
        printf("reg 1: r%d\n", decodificacao.reg1);
        printf("reg 2: r%d\n", decodificacao.reg2);

        decodificacao.imediato = 0;
    } else {
        decodificacao.opcode = extract_bits(instrucao, 13, 2);
        decodificacao.reg_dest = extract_bits(instrucao, 10, 3);
        decodificacao.imediato = extract_bits(instrucao, 0, 10);

        printf("opdoce: %d (%s)\n", decodificacao.opcode, nomes_do_opcode_i[decodificacao.opcode]);
        printf("reg 1: r%d\n", decodificacao.reg_dest);
        printf("imediato: %d\n", decodificacao.imediato);

        decodificacao.reg1 = 0;
        decodificacao.reg2 = 0;
    }
    return decodificacao;
}

void handle_syscall(AondeProgramCounterEsta * estado_pc, uint16_t *memoria) {
    switch (estado_pc->registradores[0]) {
        case 0:
            exit(0);
        case 1:
            {
                uint16_t endereco = estado_pc->registradores[1];
                while (memoria[endereco] != 0) {
                    printf("\n %c \n", (char)memoria[endereco]);
                    endereco++;
                }
            }
            break;
        case 2:
            printf("\n");
            break;
        case 3:
            printf("%d", estado_pc->registradores[1]);
            break;
        case 4:
            {
                uint16_t tamanho = estado_pc->registradores[1];
                void* novo_espaco = malloc(tamanho);
                estado_pc->registradores[1] = (uint16_t)((uintptr_t)novo_espaco & 0xFFFF);
                estado_pc->registradores[2] = (uint16_t)(((uintptr_t)novo_espaco >> 16) & 0xFFFF);
                printf("Alocação de memória simulada: %d bytes\n", tamanho);
            }
            break;
        case 5:
            {
                uint16_t endereco = estado_pc->registradores[1];
                free((void*)endereco);
                printf("Desalocação de memória simulada no endereço: %d\n", endereco);
            }
        default:
            printf("Syscall não reconhecido\n");
    }
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

// estaod_pc tem em qual instrucao nós estamos (pc na struct) e os registradores do processador
void banco_registradores(AondeProgramCounterEsta *estado_pc, const InstrucaoDecodada decodada, uint16_t *memoria) {
    const uint16_t tipo = decodada.tipo;
    const uint16_t opcode = decodada.opcode;
    const uint16_t reg_dest = decodada.reg_dest;
    const uint16_t reg1 = decodada.reg1;
    const uint16_t reg2 = decodada.reg2;
    const uint16_t imediato = decodada.imediato;

    if (tipo == 'R') {
        switch(opcode) {
            case add:
            case sub:
            case mul:
            case div2:
                estado_pc->registradores[reg_dest] = ULA(
                estado_pc->registradores[reg1],
                estado_pc->registradores[reg2],
                opcode
                );
                break;
            case cmp_equal:
                if (estado_pc->registradores[reg1] == estado_pc->registradores[reg2]) {
                    estado_pc->registradores[reg_dest] = 1;
                } else {
                    estado_pc->registradores[reg_dest] = 0;
                }
                break;
            case cmp_neq:
                if (estado_pc->registradores[reg1] != estado_pc->registradores[reg2]) {
                    estado_pc->registradores[reg_dest] = 1;
                } else {
                    estado_pc->registradores[reg_dest] = 0;
                }
                break;
            case load:
                // load r0, [r2] (pega um registrador de destino e coloca o que está alocado na memória no registrador 1 (da instrução))
                estado_pc->registradores[reg_dest] = memoria[estado_pc->registradores[reg1]];
                break;
            case store:
                memoria[estado_pc->registradores[reg1]] = memoria[estado_pc->registradores[reg2]];
                break;
            case syscall:
                handle_syscall(estado_pc, memoria);
                break;
            default:
                break;
        }
    } else {
        switch(opcode) {
            case jump:
                estado_pc->pc = imediato -1;
            case jump_cond:
                if (estado_pc->registradores[reg_dest] != 0) {
                    estado_pc->pc = imediato -1;
                }
                break;
            case mov:
                estado_pc->registradores[reg_dest] = imediato;
                break;
            default:
                break;
        }
    }
}

int main(const int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s [bin_name]\n", argv[0]);
        exit(1);
    }

    const uint32_t tamanho_da_memoria = 0x0000FFF;
    uint16_t *memoria = malloc(tamanho_da_memoria);
    // tava dando um warning na minha ide
    if (memoria == NULL) {
        printf("memory allocation failed\n");
        exit(1);
    }

    load_binary_to_memory(argv[1], memoria, tamanho_da_memoria);

    //for(int i = 0; i < tamanho_da_memoria; i++) {
        //printf("memoria[%d] = ", i);
        //printzaoDebug(memoria[i]);
    //}

    // podia bem começar no 40 pois é mais ou menos aonde começam as instruções do assembly de verdade (jump 40
    // na primeira instrução 0x0001)
    AondeProgramCounterEsta onde_pc_ta = {0};

    while (1) {
        // busca da instrução é feita quando eu aperto o enter
        const uint16_t instrucao = memoria[onde_pc_ta.pc];

        print_pc(&onde_pc_ta);
       // printzaoDebug(instrucao);

        const InstrucaoDecodada decodada = decodificador(instrucao);
        banco_registradores(&onde_pc_ta, decodada, memoria);

        onde_pc_ta.pc++;

        // perto de onde o programa começa a colocar só instruções com 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 (eu acho)
        if (instrucao == 0x029A) {
            break;
        }

        getchar();
    }

    free(memoria);
    return 0;
}