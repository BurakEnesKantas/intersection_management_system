#ifndef MODES_H
#define MODES_H

#include "main.h"
#include "types.h"

// Fonksiyon prototipleri
void Sequences_Mode(void);
void Flash_Mode(uint16_t duration_ms);
void RunPhaseWithTransition(char* target_phase);
void RunMultiPhase(char* input);
SystemMode_t Reset_Mode(void);  // Eğer implementasyon varsa

#endif // MODES_H




