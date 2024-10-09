#ifndef BRANCH_PREDICTOR_H
#define BRANCH_PREDICTOR_H

#include <stdint.h>

#define BHT_SIZE 32

typedef struct {
    uint8_t contador; // (0, 1, 2, 3) vulgo 2 bit
    uint16_t alvo;
} EntradaBHT;

typedef struct {
    EntradaBHT tabela[BHT_SIZE];
} PreditorDoDesvio;

void inicializa_preditor(PreditorDoDesvio *preditor);
int preve_desvio(PreditorDoDesvio *preditor, uint16_t pc);
void atualiza_preditor(PreditorDoDesvio *preditor, uint16_t pc, int tomado, uint16_t alvo);

#endif