#ifndef TIMER_CONFIG_H
#define TIMER_CONFIG_H

#include "main.h"

// Fonksiyon prototipleri
void MX_TIM1_Init(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif // TIMER_CONFIG_H
