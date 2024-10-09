#include "preditor.h"

void inicializa_preditor(PreditorDoDesvio *preditor) {
    for (int i = 0; i < BHT_SIZE; i++) {
        preditor->tabela[i].contador = 1; //primeiro começa como tomado, se nn tomou ai dps corrige
        preditor->tabela[i].alvo = 0;
    }
}


void atualiza_preditor(PreditorDoDesvio *preditor, uint16_t pc, int tomado, uint16_t alvo) {
    uint16_t index = pc;

    if (index >= BHT_SIZE) {
        index = index - BHT_SIZE;
    }

    preditor[index].tabela->alvo = alvo; // aonde q a decisão vai afetar
    if (tomado) {
        if (preditor[index].tabela->contador < 3) {
            preditor[index].tabela->contador++;
        }
    } else {
        if (preditor[index].tabela->contador > 0) {
            preditor[index].tabela->contador--;
        }
    }
}
