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
    static char *reg_names[] = {
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};

    if (i >= 0 && i < 8) {
        return reg_names[i];
    }
    return "registrador inválido";
}

int binario_pra_decimal(const int binarios[], const int tamanho_do_vetor) {
    int decimal = 0;
    for (int i = 0; i < tamanho_do_vetor; i++) {
        decimal += binarios[i] * (1 << (tamanho_do_vetor - 1 - i));
    }
    return decimal;
}

typedef enum {
    add = 0, sub = 1, mul = 2, div2 = 3, cmp_equal = 4, cmp_neq = 5,
    load = 15, store = 16, syscall = 63
} OpcodesR;

typedef enum {
    jump = 0, jump_cond = 1, mov = 3
} OpcodesI;


// pega os nomes a partir das structs definidas em cima
const char* nomes_do_opcode_r2[] = {
        [add] = "add", [sub] = "sub", [mul] = "mul", [div2] = "div",
        [cmp_equal] = "cmp_equal", [cmp_neq] = "cmp_neq",
        [load] = "load", [store] = "store", [syscall] = "syscall"
};

const char* nomes_do_opcode_i2[] = {
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
        printf("opcode: %d (%s)\n", opcode, nomes_do_opcode_r2[opcode]);

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
        printf("opcode: %d (%s)\n", opcode, nomes_do_opcode_i2[opcode]);

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