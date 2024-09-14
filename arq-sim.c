#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lib.h"

typedef struct {
    uint16_t pc;
    uint16_t registradores[8];
} aondeOProcessadorEstaAgora;

// acessa e pega o que ta naquele endereço de memória (do ponteiro do struct aondeOProcessadorEstaAgora que o pc e o  ta)
void print_pc(const aondeOProcessadorEstaAgora *ondeEleTa) {
    printf("onde o processador ta: %p \n", ondeEleTa);
    printf("PC: %X\n", ondeEleTa->pc);
    for (int i = 0; i < 8; i++) {
        printf("%s: %X ", get_reg_name_str(i), ondeEleTa->registradores[i]);
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

void decodifica(uint16_t instrucao) {
    uint16_t bit_formato = extract_bits(instrucao, 15, 1);
    char tipo = bit_formato ? 'I' : 'R';
    printf("formato: %c\n", tipo);

    if (tipo == 'R') {
        const uint16_t opcode = extract_bits(instrucao, 9, 6);
        const uint16_t dest = extract_bits(instrucao, 6, 3);
        const uint16_t op1 = extract_bits(instrucao, 3, 3);
        const uint16_t op2 = extract_bits(instrucao, 0, 3);

        printf("opcode: %d (%s)\n", opcode, nomes_do_opcode_r[opcode]);
        printf("reg dest: r%d\n", dest);
        printf("reg 1: r%d\n", op1);
        printf("reg 2: r%d\n", op2);
    } else {
        const uint16_t opcode = extract_bits(instrucao, 13, 2);
        const uint16_t reg = extract_bits(instrucao, 10, 3);
        const uint16_t immediate = extract_bits(instrucao, 0, 10);

        printf("opdoce: %d (%s)\n", opcode, nomes_do_opcode_i[opcode]);
        printf("reg 1: r%d\n", reg);
        printf("imediato: %d\n", immediate);
    }
}

void handle_syscall(aondeOProcessadorEstaAgora * estado_pc, uint16_t *memoria) {
    switch (estado_pc->registradores[0]) { // assuma que r0 contém o código do syscall
        case 0: // imprime o inteiro
            printf("Programa encerrado. \n");
            exit(0);
        case 1: // imprime uma string
            {
                uint16_t endereco = estado_pc->registradores[1]; // assume que o endereço da string está em r1
                while (memoria[endereco] != 0) {
                    printf("%c", (char)memoria[endereco]);
                    endereco++;
                }
            }
            break;
        case 2: // imprimir nova linha
            printf("\n");
            break;
        case 3: // imprimir inteiro
            printf("%d", estado_pc->registradores[1]);
            break;
        case 4: // alocar memoria (malloc)
            {
                uint16_t tamanho = estado_pc->registradores[1];
                // Aqui você precisaria implementar a alocação de memória
                // Por exemplo:
                // void* novo_espaco = malloc(tamanho);
                // estado_pc->registradores[1] = (uint16_t)((uintptr_t)novo_espaco & 0xFFFF);
                // estado_pc->registradores[2] = (uint16_t)(((uintptr_t)novo_espaco >> 16) & 0xFFFF);
                printf("Alocação de memória simulada: %d bytes\n", tamanho);
            }
            break;
        case 5: // desalocar memória (free)
            {
                uint16_t endereco = estado_pc->registradores[1];
                // precisa implementar desalocação de memória
                // ex.: free((void*)endereco);
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
void banco_registradores(aondeOProcessadorEstaAgora *estado_pc, const uint16_t instrucao, uint16_t *memoria) {
    uint16_t bit_formato = extract_bits(instrucao, 15, 1);
    char tipo = bit_formato ? 'I' : 'R';

    if (tipo == 'R') {
        const uint16_t opcode = extract_bits(instrucao, 9, 6);
        const uint16_t reg_dest = extract_bits(instrucao, 6, 3);
        const uint16_t reg1 = extract_bits(instrucao, 3, 3);
        const uint16_t reg2 = extract_bits(instrucao, 0, 3);

        switch(opcode) {
            case add:
            case sub:
            case mul:
            case div2:
                estado_pc->registradores[reg_dest] = ULA(reg1, reg2, opcode);
                break;
            case cmp_equal:
                estado_pc->registradores[reg_dest] = estado_pc->registradores[reg1] == estado_pc->registradores[reg2];
                break;
            case cmp_neq:
                estado_pc->registradores[reg_dest] = estado_pc->registradores[reg1] != estado_pc->registradores[reg2];
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
        const uint16_t opcode = extract_bits(instrucao, 13, 2);
        const uint16_t reg = extract_bits(instrucao, 10, 3);
        const uint16_t imediato = extract_bits(instrucao, 0, 10);

        switch(opcode) {
            case mov:
                estado_pc->registradores[reg] = imediato;
                break;
            case jump:
                estado_pc->pc = imediato -1;
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
    aondeOProcessadorEstaAgora onde_pc_ta = {0};

    while (1) {
        // busca da instrução é feita quando eu aperto o enter
        const uint16_t instrucao = memoria[onde_pc_ta.pc];

        print_pc(&onde_pc_ta);
       // printzaoDebug(instrucao);

        decodifica(instrucao);
        banco_registradores(&onde_pc_ta, instrucao, memoria);

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