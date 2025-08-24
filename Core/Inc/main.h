#ifndef MAIN_H
#define MAIN_H

#include "stm32g0xx_hal.h"  // STM32 HAL kütüphanesi (IDE'nize göre ayarlayın)
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "gpio_config.h"
#include "uart_config.h"
#include "rtc.h"
#include "timer_config.h"
#include "traffic_lights.h"
#include "modes.h"
#include "commands.h"
#include "types.h"



// Global değişkenler
extern volatile SystemMode_t current_mode;
extern SystemMode_t Reset_Mode(void);
extern volatile uint32_t timerCounter;
extern uint16_t green_delay;
extern uint16_t red_delay;
extern uint16_t north_delay;
extern uint16_t south_delay;
extern uint16_t east_delay;
extern uint16_t west_delay;
extern uint16_t yellow_delay;
extern uint16_t transition_delay;
extern uint16_t flash_delay;
extern char rx_data[50];
extern int8_t time_zone_offset;

// Extern handler'lar
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim1;

// Fonksiyon prototipleri (diğer modüllerden)
void SystemClock_Config(void);
void Error_Handler(void);


#endif // MAIN_H
