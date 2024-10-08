#ifndef BRANCH_PREDICTOR_H
#define BRANCH_PREDICTOR_H

#include <stdint.h>

#define BHT_SIZE 256

typedef struct {
    int contador; // (-1, 0, 1, 2)
    uint16_t alvo;
} EntradaBHT;

typedef struct {
    EntradaBHT tabela[BHT_SIZE];
} PreditorDoDesvio;

void inicializa_preditor(PreditorDoDesvio *preditor);
int preve_desvio(PreditorDoDesvio *preditor, uint16_t pc);
void atualiza_preditor(PreditorDoDesvio *preditor, uint16_t pc, int tomado, uint16_t alvo);

#endif