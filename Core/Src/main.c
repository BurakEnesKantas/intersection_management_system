#ifndef MODES_H
#define MODES_H

#include "main.h"

// Fonksiyon prototipleri
void Sequences_Mode(void);
void Flash_Mode(uint16_t duration_ms);
void RunPhaseWithTransition(char* target_phase);
void RunMultiPhase(char* input);
SystemMode_t Reset_Mode(void);  // Eğer implementasyon varsa

#endif // MODES_H



#include "main.h"

// Global değişkenler (başlangıç değerleri)
volatile uint32_t timerCounter = 0;
uint16_t green_delay = 4000;
uint16_t red_delay = 4000;
uint16_t north_delay = 4000;
uint16_t south_delay = 4000;
uint16_t east_delay = 4000;
uint16_t west_delay = 4000;
uint16_t yellow_delay = 2000;
uint16_t transition_delay = 1500;
uint16_t flash_delay = 250;
char rx_data[50] = {0};
int8_t time_zone_offset = 3;

// Handler'lar
UART_HandleTypeDef huart2;
RTC_HandleTypeDef hrtc;
TIM_HandleTypeDef htim1;



// --- Zaman yayınlama yardımcıları ---

static void format_time_with_offset(RTC_TimeTypeDef *t, int8_t tz, int *hh, int *mm, int *ss)
{
    int h;

    if (tz == 3) {
        h = t->Hours;   // RTC zaten UTC+3'te çalışıyor
    } else {
        h = t->Hours + (tz - 3);  // Farkı uygula
    }

    if (h < 0)   h += 24;
    if (h >= 24) h -= 24;

    *hh = h;
    *mm = t->Minutes;
    *ss = t->Seconds;
}



void BroadcastTimeIfSecondChanged(void)
{
    static uint8_t lastSecond = 255;
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    if (sTime.Seconds != lastSecond)
    {
        lastSecond = sTime.Seconds;

        int hh, mm, ss;
        format_time_with_offset(&sTime, time_zone_offset, &hh, &mm, &ss);

        char msg[64];
        snprintf(msg, sizeof(msg),
                 "Current Time (UTC %+d): %02d:%02d:%02d\r\n",
                 time_zone_offset, hh, mm, ss);

        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
}







int main(void) {
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_RTC_Init();
    MX_TIM1_Init();
    HAL_TIM_Base_Start_IT(&htim1);

    HAL_UART_Transmit(&huart2, (uint8_t*)"Hazir\r\n", strlen("Hazir\r\n"), HAL_MAX_DELAY);
    HAL_UART_Receive_IT(&huart2, (uint8_t*)&uart_rx_byte, 1);







    while (1) {

    	BroadcastTimeIfSecondChanged();
        // Zaman aşımı kontrolü
        if (current_mode != MODE_SEQUENCE && (HAL_GetTick() - timerCounter) > 80000) {
            current_mode = MODE_SEQUENCE;
            HAL_UART_Transmit(&huart2, (uint8_t*)"Timeout, returning to SEQUENCE MODE\r\n", strlen("Timeout, returning to SEQUENCE MODE\r\n"), HAL_MAX_DELAY);
            timerCounter = HAL_GetTick();
        }

        if (cmd_ready) {
            cmd_ready = 0;
            ProcessCommand((char*)rx_buffer);
        }

        switch (current_mode) {
            case MODE_SEQUENCE:
                Sequences_Mode();
                break;
            case MODE_FLASH:
                Flash_Mode(1);
                break;
            case MODE_MANUAL:
                // Manuel mod
                break;
            case MODE_IDLE:
                break;
            case MODE_OFF:
                All_LEDs_Off();
                break;
            case MODE_RESET:
                Reset_Mode();
                break;
            case MODE_PHASE:
                if (strchr((char*)rx_data, '+')) {
                    RunMultiPhase((char*)rx_data);
                    for (uint32_t i = 0; i < 1000; i += 10) {
                        HAL_Delay(10);
                        BroadcastTimeIfSecondChanged();
                        if (cmd_ready) {
                            cmd_ready = 0;
                            ProcessCommand((char*)rx_buffer);
                            if (current_mode != MODE_PHASE) break;
                        }
                    }
                } else {
                    RunPhaseWithTransition((char*)rx_data);
                }
                break;
        }
    }
}



void SystemClock_Config(void)
 {
   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

   /** Configure the main internal regulator output voltage
   */
   HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

   /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
   RCC_OscInitStruct.HSIState = RCC_HSI_ON;
   RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
   RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
   RCC_OscInitStruct.LSIState = RCC_LSI_ON;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
     Error_Handler();
   }

   /** Initializes the CPU, AHB and APB buses clocks
   */
   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                               |RCC_CLOCKTYPE_PCLK1;
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
   {
     Error_Handler();
   }
 }

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

