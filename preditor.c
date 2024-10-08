#include "preditor.h"

void inicializa_preditor(PreditorDoDesvio *preditor) {
    for (int i = 0; i < BHT_SIZE; i++) {
        preditor->tabela[i].contador = 1; //primeiro começa como tomado, se nn tomou ai dps corrige
        preditor->tabela[i].alvo = 0;
    }
}

int preve_desvio(PreditorDoDesvio *preditor, uint16_t pc) {
    int indice = pc % BHT_SIZE;
    return preditor->tabela[indice].contador > 0;
}

void atualiza_preditor(PreditorDoDesvio *preditor, uint16_t pc, int tomado, uint16_t alvo) {
    int indice = pc % BHT_SIZE;
    EntradaBHT *entrada = &preditor->tabela[indice];

    if (tomado) {
        if (entrada->contador < 2) entrada->contador++;
    } else {
        if (entrada->contador > -1) entrada->contador--;
    }

    entrada->alvo = alvo;
}
