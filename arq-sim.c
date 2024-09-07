#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lib.h"

typedef struct {
    uint16_t pc;
    uint16_t registers[8];
} aondeOProcessadorEstaAgora;

void print_pc(const aondeOProcessadorEstaAgora *ondeEleTa) {
    printf("PC: 0x%04X\n", ondeEleTa->pc);
    for (int i = 0; i < 8; i++) {
        printf("%s: 0x%04X ", get_reg_name_str(i), ondeEleTa->registers[i]);
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

const char* nomes_do_opcode_r[] = {
        [add] = "add", [sub] = "sub", [mul] = "mul", [div2] = "div",
        [cmp_equal] = "cmp_equal", [cmp_neq] = "cmp_neq",
        [load] = "load", [store] = "store", [syscall] = "syscall"
};

const char* nomes_do_opcode_i[] = {
        [jump] = "jump", [jump_cond] = "jump_cond", [mov] = "mov"
};

void printzaoDebug(const uint16_t instrucao) {
    printf("instrução: ");
    for (int j = 15; j >= 0; j--) {
        printf("%d", (instrucao & (1 << j)) ? 1 : 0);
        if (j == 15 || j == 9 || j == 6 || j == 3) {
            printf(" ");
        }
    }
    printf("\n");

    char tipo = (instrucao & (1 << 15)) ? 'I' : 'R';
    printf("formato: %c\n", tipo);

    if (tipo == 'R') {
        int opcode_bits[6];
        for (int j = 14; j >= 9; j--) {
            opcode_bits[14 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        int opcode = binario_pra_decimal(opcode_bits, 6);
        printf("opcode: %d (%s)\n", opcode, nomes_do_opcode_r[opcode]);

        int dest_bits[3];
        for (int j = 8; j >= 6; j--) {
            dest_bits[8 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        printf("reg dest: r%d\n", binario_pra_decimal(dest_bits, 3));

        int op1_bits[3];
        for (int j = 5; j >= 3; j--) {
            op1_bits[5 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        printf("reg 1: r%d\n", binario_pra_decimal(op1_bits, 3));

        int op2_bits[3];
        for (int j = 2; j >= 0; j--) {
            op2_bits[2 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        printf("reg 2: r%d\n", binario_pra_decimal(op2_bits, 3));
    } else {
        int opcode_bits[2];
        for (int j = 14; j >= 13; j--) {
            opcode_bits[14 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        int opcode = binario_pra_decimal(opcode_bits, 2);
        printf("opcode: %d (%s)\n", opcode, nomes_do_opcode_i[opcode]);

        int reg_bits[3];
        for (int j = 12; j >= 10; j--) {
            reg_bits[12 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        printf("reg dest: r%d\n", binario_pra_decimal(reg_bits, 3));

        int imm_bits[10];
        for (int j = 9; j >= 0; j--) {
            imm_bits[9 - j] = (instrucao & (1 << j)) ? 1 : 0;
        }
        printf("imediato: %d\n", binario_pra_decimal(imm_bits, 10));
    }

    printf("\n");
}

void decodifica(uint16_t instrucao) {
    char tipo = (instrucao & (1 << 15)) ? 'I' : 'R';
    printf("tipo da instrução: %c\n", tipo);

    if (tipo == 'R') {
        uint16_t const opcode = extract_bits(instrucao, 9, 6);
        uint16_t const dest = extract_bits(instrucao, 6, 3);
        uint16_t const op1 = extract_bits(instrucao, 3, 3);
        uint16_t const op2 = extract_bits(instrucao, 0, 3);

        printf("opcode: %d (%s)\n", opcode, nomes_do_opcode_r[opcode]);
        printf("reg dest: r%d\n", dest);
        printf("reg 1: r%d\n", op1);
        printf("reg 2: r%d\n", op2);
    } else {
        uint16_t const opcode = extract_bits(instrucao, 13, 2);
        uint16_t const reg = extract_bits(instrucao, 10, 3);
        uint16_t const immediate = extract_bits(instrucao, 0, 10);

        printf("opdoce: %d (%s)\n", opcode, nomes_do_opcode_i[opcode]);
        printf("reg 1: r%d\n", reg);
        printf("imediato: %d\n", immediate);
    }
}

int main(const int argc, char **argv)
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
        printzaoDebug(memory[i]);
    }

    // podia bem começar no 40 pois é mais ou menos aonde começam as instruções do assembly de verdade (jump 40)
    aondeOProcessadorEstaAgora ondeEleTa = {0};

    while (1) {
        const uint16_t instrucao = fetch_instrucao(memory, ondeEleTa.pc);

        //print_pc(&ondeEleTa);
       // printzaoDebug(instrucao);

        decodifica(instrucao);

        ondeEleTa.pc++;

        // perto de onde o programa começa a colocar só instruções com 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 (eu acho)
        if (instrucao == 0x1111) {
            break;
        }

        getchar();
    }

    free(memory);
    return 0;
}