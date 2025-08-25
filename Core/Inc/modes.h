#ifndef MODES_H
#define MODES_H

#include "main.h"
#include "types.h"

#define NUM_LEDS 12

// LED dizileri ve durumları
extern GPIO_PinState lastLedState[NUM_LEDS];
extern GPIO_TypeDef* ledPorts[NUM_LEDS];
extern uint16_t ledPins[NUM_LEDS];
extern char* ledNames[NUM_LEDS];
extern uint32_t lastLedLogTime;

// Fonksiyon prototipleri
void Sequences_Mode(void);
void Flash_Mode(uint16_t duration_ms);
void RunPhaseWithTransition(char* target_phase);
void RunMultiPhase(char* input);
SystemMode_t Reset_Mode(void);  // Eğer implementasyon varsa
void initLeds(void);
void UpdateLedStatusLog(void);






#endif // MODES_H




