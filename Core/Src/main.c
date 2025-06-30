/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


typedef enum
{
    MODE_IDLE,		//  0: Boşta
    MODE_SEQUENCE,	//  1: Trafik ışığı sequence modu
    MODE_FLASH,		//  2: Yanıp sönme modu
    MODE_MANUAL,	//  3: Manuel kontrol modu
	MODE_RESET,		//  5: Sistem yeniden başlatılıyor
	MODE_OFF,		//  4: Bütün ıl-şıklar kapalı
	MODE_PHASE
} SystemMode_t;










/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/*
kuzey - north
NR,NY,NG
güney - south
doğu - east
batı - west
*/

//NORTH GROUP
#define NR_PIN GPIO_PIN_8
#define NR_PORT GPIOC
#define NY_PIN GPIO_PIN_6
#define NY_PORT GPIOC
#define NG_PIN GPIO_PIN_9
#define NG_PORT GPIOC

//SOUTH GROUP
#define SR_PIN GPIO_PIN_6
#define SR_PORT GPIOD
#define SY_PIN GPIO_PIN_12
#define SY_PORT GPIOA
#define SG_PIN GPIO_PIN_11
#define SG_PORT GPIOA

//EAST GROUP
#define ER_PIN GPIO_PIN_1
#define ER_PORT GPIOC
#define EY_PIN GPIO_PIN_0
#define EY_PORT GPIOC
#define EG_PIN GPIO_PIN_2
#define EG_PORT GPIOB

//WEST GROUP
#define WR_PIN GPIO_PIN_6
#define WR_PORT GPIOB
#define WY_PIN GPIO_PIN_15
#define WY_PORT GPIOB
#define WG_PIN GPIO_PIN_10
#define WG_PORT GPIOB


#define RX_BUFFER_SIZE 50

volatile SystemMode_t current_mode = MODE_IDLE;
SystemMode_t Reset_Mode(void);
volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_index = 0;
volatile uint8_t cmd_ready = 0;

//UART handler extern tanımı
extern UART_HandleTypeDef huart2;

volatile uint32_t last_command_time = 0;
#define COMMAND_TIMEOUT 80000 // 80 saniye (ms cinsinden)
uint16_t north_delay = 4000;
uint16_t south_delay = 4000;
uint16_t east_delay = 4000;
uint16_t west_delay = 4000;
uint16_t yellow_delay = 2000;
uint16_t transition_delay = 1500;
uint16_t flash_delay = 250;
char rx_data[50] = {0};

uint8_t uart_rx_byte;

typedef struct {
    const char* name;
    GPIO_TypeDef* green_port;
    uint16_t green_pin;
    GPIO_TypeDef* yellow_port;
    uint16_t yellow_pin;
    GPIO_TypeDef* red_port;
    uint16_t red_pin;
} Direction;

Direction directions[] = {
    {"north", NG_PORT, NG_PIN, NY_PORT, NY_PIN, NR_PORT, NR_PIN},
    {"south", SG_PORT, SG_PIN, SY_PORT, SY_PIN, SR_PORT, SR_PIN},
    {"east",  EG_PORT, EG_PIN, EY_PORT, EY_PIN, ER_PORT, ER_PIN},
    {"west",  WG_PORT, WG_PIN, WY_PORT, WY_PIN, WR_PORT, WR_PIN},
};

const int direction_count = sizeof(directions) / sizeof(Direction);

// Receive callback fonksiyonu
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART2)
  {
    // Yeni veri al
    if(uart_rx_byte == '\r' || uart_rx_byte == '\n')
    {
      if(rx_index > 0)
      {
        rx_buffer[rx_index] = '\0';
        cmd_ready = 1;
        rx_index = 0;
      }
    }
    else if(rx_index < RX_BUFFER_SIZE-1)
    {
      rx_buffer[rx_index++] = uart_rx_byte;
    }

    // Yeni veri beklemeye devam et
    HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1);
  }
}










void ProcessCommand(char* cmd);
void Sequences_Mode(void);
void Flash_Mode(uint16_t duration_ms);
void All_LEDs_Off(void);
uint8_t CheckModeChange(void);
void north_red(void);
void north_green(void);
void north_yellow(void);
void south_red(void);
void south_green(void);
void south_yellow(void);
void east_red(void);
void east_green(void);
void east_yellow(void);
void west_red(void);
void west_green(void);
void west_yellow(void);






/* Includes ------------------------------------------------------------------*/


void ProcessCommand(char* cmd) {
	char msg[128];  // UART'a yazılacak mesajlar için buffer
	last_command_time = HAL_GetTick();
	if (strcmp(cmd, "info") == 0) {
		HAL_UART_Transmit(&huart2, (uint8_t*)"Sequence Mode = SM\r\nFlash Mode = FM\r\nPhase Mode = phase=north/south/east/west\r\n"
				"Restart = reset\r\nSignal Group Info = SIG?\r\nSignal Info = SIG=X(1-2-3-4)\r\nStop All System = stop\r\nClose All LEDs = off" ,
				strlen("Sequence Mode = SM\r\nFlash Mode = FM\r\nPhase Mode = phase=north/south/east/west\r\nRestart = reset\r\n"
						"Signal Group Info = SIG?\r\nSignal Info = SIG=X(1-2-3-4)\r\nStop All System = stop\r\nClose All LEDs = off" ), HAL_MAX_DELAY);
		}
	else if (strcmp(cmd, "SM") == 0) {
        current_mode = MODE_SEQUENCE;
        HAL_UART_Transmit(&huart2, (uint8_t*)"SEQUENCE MODE STARTED\r\n", 30, HAL_MAX_DELAY);
    }

	else if (strcmp(cmd, "SIG?") == 0) {  // Tam eşleşme kontrolü
	    // Tüm grupların LED durumlarını döndür
	    for (int i = 0; i < direction_count; i++) {
	        sprintf(msg, "Group %d (%s):\r\n", i + 1, directions[i].name);
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	        sprintf(msg, "R = %s\r\n", HAL_GPIO_ReadPin(directions[i].red_port, directions[i].red_pin) == GPIO_PIN_SET ? "High" : "Low");
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	        sprintf(msg, "Y = %s\r\n", HAL_GPIO_ReadPin(directions[i].yellow_port, directions[i].yellow_pin) == GPIO_PIN_SET ? "High" : "Low");
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	        sprintf(msg, "G = %s\r\n", HAL_GPIO_ReadPin(directions[i].green_port, directions[i].green_pin) == GPIO_PIN_SET ? "High" : "Low");
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	        HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
	    }
	}
	else if (strncmp(cmd, "SIG=", 4) == 0 && strlen(cmd) > 4) {  // Uzunluk kontrolü ekleyin
	    int groupIndex = atoi(&cmd[4]) - 1;

	    if (groupIndex >= 0 && groupIndex < direction_count) {
	        sprintf(msg, "Group %d (%s):\r\n", groupIndex + 1, directions[groupIndex].name);
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	        sprintf(msg, "R = %s\r\n", HAL_GPIO_ReadPin(directions[groupIndex].red_port, directions[groupIndex].red_pin) == GPIO_PIN_SET ? "High" : "Low");
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	        sprintf(msg, "Y = %s\r\n", HAL_GPIO_ReadPin(directions[groupIndex].yellow_port, directions[groupIndex].yellow_pin) == GPIO_PIN_SET ? "High" : "Low");
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	        sprintf(msg, "G = %s\r\n", HAL_GPIO_ReadPin(directions[groupIndex].green_port, directions[groupIndex].green_pin) == GPIO_PIN_SET ? "High" : "Low");
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	    } else {
	        sprintf(msg, "Invalid group index!\r\n");
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	    }
	}


    else if(strcmp(cmd, "MODE?") == 0 ){
    	if (current_mode == MODE_SEQUENCE) {
    		HAL_UART_Transmit(&huart2, (uint8_t*)"SEQUENCE MODE\r\n", strlen("SEQUENCE MODE\r\n"), HAL_MAX_DELAY);
		}
   	else if(current_mode == MODE_FLASH) {
    		HAL_UART_Transmit(&huart2, (uint8_t*)"FLASH MODE\r\n", strlen("FLASH MODE\r\n"), HAL_MAX_DELAY);
    	}
  	else if(current_mode == MODE_RESET) {
       		HAL_UART_Transmit(&huart2, (uint8_t*)"RESET MODE\r\n", strlen("RESET MODE\r\n"), HAL_MAX_DELAY);
    	}
    else if(current_mode == MODE_PHASE) {
       		HAL_UART_Transmit(&huart2, (uint8_t*)"PHASE MODE\r\n", strlen("PHASE MODE\r\n"), HAL_MAX_DELAY);
  	  	}
    }
    else if(strcmp(cmd, "active?") == 0){
        char actives[64];
        GetActiveGreenDirections(actives, sizeof(actives));
        HAL_UART_Transmit(&huart2, (uint8_t*)actives, strlen(actives), HAL_MAX_DELAY);
    }

    else if (strcmp(cmd, "FM") == 0) {
        current_mode = MODE_FLASH;
        HAL_UART_Transmit(&huart2, (uint8_t*)"FLASH MODE STARTED\r\n", 30, HAL_MAX_DELAY);
    }
    else if (strncmp(cmd, "phase=", 6) == 0) {
        current_mode = MODE_PHASE;
        strncpy((char*)rx_data, cmd + 6, sizeof(rx_data) - 1); // yön adını sakla
        rx_data[sizeof(rx_data)-1] = '\0';
        sprintf(msg, "Phase: %s\r\n", rx_data);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
    else if (strcmp(cmd, "reset") == 0) {
    	HAL_UART_Transmit(&huart2, (uint8_t*)"WAIT FOR RESTART...\r\n", strlen("WAIT FOR RESTART...\r\n"), HAL_MAX_DELAY);
    	current_mode = MODE_RESET;
        HAL_UART_Transmit(&huart2, (uint8_t*)"Sequence Mode Continues\r\n", strlen("Sequence Mode Continues\r\n"), HAL_MAX_DELAY);
    }
    else if (strcmp(cmd, "stop") == 0) {
        current_mode = MODE_IDLE;
        HAL_UART_Transmit(&huart2, (uint8_t*)"ALL MODES STOPPED\r\n", strlen("ALL MODES STOPPED\r\n"), HAL_MAX_DELAY);
    }
    else if (strcmp(cmd, "off") == 0)
    {
    	current_mode = MODE_OFF;
    	HAL_UART_Transmit(&huart2, (uint8_t*)"ALL LEDs OFF\r\n", strlen("ALL LEDs OFF\r\n"), HAL_MAX_DELAY);
    }
    else if (strncmp(cmd, "south=", 6) == 0) {
        south_delay = atoi(cmd + 6);
        sprintf(msg, "South delay set: %dms\r\n", south_delay);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
    else if (strncmp(cmd, "north=", 6) == 0) {
        north_delay = atoi(cmd + 6);
        sprintf(msg, "North delay set: %dms\r\n", north_delay);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
    else if (strncmp(cmd, "east=", 5) == 0) {
        east_delay = atoi(cmd + 5);
        sprintf(msg, "East delay set: %dms\r\n", east_delay);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
    else if (strncmp(cmd, "west=", 5) == 0) {
        west_delay = atoi(cmd + 5);
        sprintf(msg, "West delay set: %dms\r\n", west_delay);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
    else if (strncmp(cmd, "yellow=", 7) == 0) {
        yellow_delay = atoi(cmd + 7);
        sprintf(msg, "Yellow delay set: %dms\r\n", yellow_delay);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
    else if (strncmp(cmd, "transition=", 11) == 0) {
        transition_delay = atoi(cmd + 11);
        sprintf(msg, "Transition delay set: %dms\r\n", transition_delay);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }

  	  char debug_msg[50];
      sprintf(debug_msg, "\r\nCMD: %s\r\n", cmd);
      HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);

}




void west_red (void)//Batı - West Red
{
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
}

void south_red (void)// Güney - South red
{
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
}

void north_red (void)// Kuzey - North red
{
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
}

void east_red (void)// Doğu - East red
{
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_SET);
}


void west_green (void)//Batı - West Green
{
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_RESET);
}

void south_green (void)// Güney - South Green
{
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_RESET);
}

void north_green (void)// Kuzey - North Green
{
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_RESET);
}

void east_green (void)// Doğu - East Green
{
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_RESET);
}

void west_yellow (void)//Batı - West Green
{
	HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_RESET);
}

void south_yellow (void)// Güney - South Green
{
	HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_RESET);
}

void north_yellow (void)// Kuzey - North Green
{
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_RESET);
}

void east_yellow (void)// Doğu - East Green
{
	HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_RESET);
}

void All_LEDs_Off(void) {
	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(NG_PORT, NG_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(WG_PORT, WG_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SG_PORT, SG_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(EG_PORT, EG_PIN, GPIO_PIN_RESET);
}


void RunPhaseWithTransition(char* target_phase) {
    // 1. Önce mevcut yeşil hangi yönse, onu sarıya al
    if (HAL_GPIO_ReadPin(NG_PORT, NG_PIN) == GPIO_PIN_SET) {
        north_yellow();
        HAL_Delay(yellow_delay);
        north_red();
    }
    else if (HAL_GPIO_ReadPin(SG_PORT, SG_PIN) == GPIO_PIN_SET) {
        south_yellow();
        HAL_Delay(yellow_delay);
        south_red();
    }
    else if (HAL_GPIO_ReadPin(EG_PORT, EG_PIN) == GPIO_PIN_SET) {
        east_yellow();
        HAL_Delay(yellow_delay);
        east_red();
    }
    else if (HAL_GPIO_ReadPin(WG_PORT, WG_PIN) == GPIO_PIN_SET) {
        west_yellow();
        HAL_Delay(yellow_delay);
        west_red();
    }

    // 2. Her ihtimale karşı tüm yönleri kırmızı yap (temizlik)
    north_red(); south_red(); east_red(); west_red();
    HAL_Delay(transition_delay); // kırmızıya geçiş beklemesi

    // 3. Hedef yön için kırmızı + sarı yak
    if (strcmp(target_phase, "north") == 0) {
    	HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
    	HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_SET);
    }
    else if (strcmp(target_phase, "south") == 0) {
    	HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
    	HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_SET);
    }
    else if (strcmp(target_phase, "east") == 0) {
    	HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_SET);
    	HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_SET);
    }
    else if (strcmp(target_phase, "west") == 0) {
    	HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
    	HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_SET);
    }

    HAL_Delay(transition_delay); // kırmızı + sarı beklemesi

    // 4. Sadece yeşil yak
    if (strcmp(target_phase, "north") == 0) {
        north_green();
    }
    else if (strcmp(target_phase, "south") == 0) {
        south_green();
    }
    else if (strcmp(target_phase, "east") == 0) {
        east_green();
    }
    else if (strcmp(target_phase, "west") == 0) {
        west_green();
    }
}



const char* GetCurrentGreenPhase(void) {
    if (HAL_GPIO_ReadPin(NG_PORT, NG_PIN) == GPIO_PIN_SET) return "north";
    if (HAL_GPIO_ReadPin(SG_PORT, SG_PIN) == GPIO_PIN_SET) return "south";
    if (HAL_GPIO_ReadPin(EG_PORT, EG_PIN) == GPIO_PIN_SET) return "east";
    if (HAL_GPIO_ReadPin(WG_PORT, WG_PIN) == GPIO_PIN_SET) return "west";
    return "none";
}





SystemMode_t Reset_Mode(void)

{
	// Flash efekti göster

	  	Flash_Mode(3000);
	    All_LEDs_Off();
	    HAL_Delay(1000);
	    Flash_Mode(5000);
	    HAL_Delay(1000);

	    HAL_UART_Transmit(&huart2, (uint8_t*)"System Reset Complete\r\n", 22, HAL_MAX_DELAY);
	    current_mode = MODE_SEQUENCE; // Varsayılan moda dön
	    return current_mode;
}


void RunPhase(char* phase, uint16_t green_time) {
    // 1. YEŞİL AÇ (Sadece verilen fazda)
    if (strcmp(phase, "north") == 0) {
        north_green(); south_red(); east_red(); west_red();
    } else if (strcmp(phase, "south") == 0) {
        north_red(); south_green(); east_red(); west_red();
    } else if (strcmp(phase, "east") == 0) {
        north_red(); south_red(); east_green(); west_red();
    } else if (strcmp(phase, "west") == 0) {
        north_red(); south_red(); east_red(); west_green();
    }

    if(!CheckModeChange()) HAL_Delay(green_time);
    if(current_mode != MODE_SEQUENCE) return;

    // 2. SARI + KIRMIZI (Önceki fazın kırmızısı ile birlikte)
    if (strcmp(phase, "north") == 0) {
        north_yellow(); south_red(); east_red(); west_red();
    } else if (strcmp(phase, "south") == 0) {
        north_red(); south_yellow(); east_red(); west_red();
    } else if (strcmp(phase, "east") == 0) {
        north_red(); south_red(); east_yellow(); west_red();
    } else if (strcmp(phase, "west") == 0) {
        north_red(); south_red(); east_red(); west_yellow();
    }

    if(!CheckModeChange()) HAL_Delay(yellow_delay);
    if(current_mode != MODE_SEQUENCE) return;

    // 3. TÜM KIRMIZI
    north_red(); south_red(); east_red(); west_red();
    if(!CheckModeChange()) HAL_Delay(transition_delay);
    if(current_mode != MODE_SEQUENCE) return;

    // 4. SONRAKİ FAZ İÇİN KIRMIZI + SARI
    if (strcmp(phase, "north") == 0) {
        // Sonraki faz güney olduğu için güney sarı
        south_yellow();
    } else if (strcmp(phase, "south") == 0) {
        // Sonraki faz doğu olduğu için doğu sarı
        east_yellow();
    } else if (strcmp(phase, "east") == 0) {
        // Sonraki faz batı olduğu için batı sarı
        west_yellow();
    } else if (strcmp(phase, "west") == 0) {
        // Sonraki faz kuzey olduğu için kuzey sarı
        north_yellow();
    }

    if(!CheckModeChange()) HAL_Delay(transition_delay);
    if(current_mode != MODE_SEQUENCE) return;
}



void Sequences_Mode(void) {
    while(current_mode == MODE_SEQUENCE) {
    	 /* KUZEY YEŞİL */
    	        north_green();
    	        south_red();
    	        east_red();
    	        west_red();
    	        if(!CheckModeChange())
    	        	for(uint32_t i = 0; i < north_delay; i += 10) {
    	        	    HAL_Delay(10);
    	        	    if(cmd_ready) {
    	        	        cmd_ready = 0;
    	        	        ProcessCommand((char*)rx_buffer);
    	        	        if(current_mode != MODE_SEQUENCE) return;
    	        	    }
    	        	}

    	        if(current_mode != MODE_SEQUENCE) return;

    	        /* KUZEY SARI */
    	        north_yellow();
    	       	south_red();
    	       	east_red();
    	        west_red();
    	        if(!CheckModeChange())
    	        	for(uint32_t i = 0; i < yellow_delay; i += 10) {
    	        	    HAL_Delay(10);
    	        	    if(cmd_ready) {
    	        	        cmd_ready = 0;
    	        	        ProcessCommand((char*)rx_buffer);
    	        	        if(current_mode != MODE_SEQUENCE) return;
    	        	    }
    	        	}

    	        if(current_mode != MODE_SEQUENCE) return;

    	        /* KUZEY KIRMIZI + GÜNEY SARI */
    	        north_red();
    	        HAL_GPIO_WritePin(SR_PORT, SR_PIN, GPIO_PIN_SET);
    	        HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_SET);
    	       	east_red();
    	       	west_red();
    	        if(!CheckModeChange())
    	        	for(uint32_t i = 0; i < transition_delay; i += 10) {
    	        	    HAL_Delay(10);
    	        	    if(cmd_ready) {
    	        	        cmd_ready = 0;
    	        	        ProcessCommand((char*)rx_buffer);
    	        	        if(current_mode != MODE_SEQUENCE) return;
    	        	    }
    	        	}

    	        if(current_mode != MODE_SEQUENCE) return;

    	        /* GÜNEY YEŞİL */
    	        north_red();
    	       	south_green();
    	      	east_red();
    	        west_red();
    	        if(!CheckModeChange())
    	        	for(uint32_t i = 0; i < south_delay; i += 10) {
    	        	    HAL_Delay(10);
    	        	    if(cmd_ready) {
    	        	        cmd_ready = 0;
    	        	        ProcessCommand((char*)rx_buffer);
    	        	        if(current_mode != MODE_SEQUENCE) return;
    	        	    }
    	        	}

    	        if(current_mode != MODE_SEQUENCE) return;

    	        //Güney Sarı
    	        north_red();
    	       	south_yellow();
    	       	east_red();
    	        west_red();
    	        if(!CheckModeChange())
    	        	for(uint32_t i = 0; i < yellow_delay; i += 10) {
    	        	    HAL_Delay(10);
    	        	    if(cmd_ready) {
    	        	        cmd_ready = 0;
    	        	        ProcessCommand((char*)rx_buffer);
    	        	        if(current_mode != MODE_SEQUENCE) return;
    	        	    }
    	        	}

    	        if(current_mode != MODE_SEQUENCE) return;


    	        north_red();
    	        south_red();
    	        HAL_GPIO_WritePin(ER_PORT, ER_PIN, GPIO_PIN_SET);
    	        HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_SET);
    	       	west_red();
    	       	if(!CheckModeChange())
    	       		for(uint32_t i = 0; i < transition_delay; i += 10) {
    	       		    HAL_Delay(10);
    	       		    if(cmd_ready) {
    	       		        cmd_ready = 0;
    	       		        ProcessCommand((char*)rx_buffer);
    	       		        if(current_mode != MODE_SEQUENCE) return;
    	       		    }
    	       		}

    	       	if(current_mode != MODE_SEQUENCE) return;

    	       	north_red();
    	       	south_red();
    	       	east_green();
    	  		west_red();
    	       	if(!CheckModeChange())
    	       		for(uint32_t i = 0; i < east_delay; i += 10) {
    	       		    HAL_Delay(10);
    	       		    if(cmd_ready) {
    	       		        cmd_ready = 0;
    	       		        ProcessCommand((char*)rx_buffer);
    	       		        if(current_mode != MODE_SEQUENCE) return;
    	       		    }
    	       		}

    	       	if(current_mode != MODE_SEQUENCE) return;

    	       	north_red();
    	       	south_red();
    	       	east_yellow();
    	   		west_red();
    	   		if(!CheckModeChange())
    	   			for(uint32_t i = 0; i < yellow_delay; i += 10) {
    	   			    HAL_Delay(10);
    	   			    if(cmd_ready) {
    	   			        cmd_ready = 0;
    	   			        ProcessCommand((char*)rx_buffer);
    	   			        if(current_mode != MODE_SEQUENCE) return;
    	   			    }
    	   			}

    	   		if(current_mode != MODE_SEQUENCE) return;

    	   		north_red();
    			south_red();
    	   		east_red();
    	   		HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
    	   		HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_SET);
    	   		if(!CheckModeChange())
    	   			for(uint32_t i = 0; i < transition_delay; i += 10) {
    	   			    HAL_Delay(10);
    	   			    if(cmd_ready) {
    	   			        cmd_ready = 0;
    	   			        ProcessCommand((char*)rx_buffer);
    	   			        if(current_mode != MODE_SEQUENCE) return;
    	   			    }
    	   			}

    	   		if(current_mode != MODE_SEQUENCE) return;

    	   		north_red();
    	   		south_red();
    	   		east_red();
    	   		west_green();
    	   		if(!CheckModeChange())
    	   			for(uint32_t i = 0; i < west_delay; i += 10) {
    	   			    HAL_Delay(10);
    	   			    if(cmd_ready) {
    	   			        cmd_ready = 0;
    	   			        ProcessCommand((char*)rx_buffer);
    	   			        if(current_mode != MODE_SEQUENCE) return;
    	   			    }
    	   			}

    	   		if(current_mode != MODE_SEQUENCE) return;

    	   		north_red();
    	   		south_red();
    	   		east_red();
    	   		west_yellow();
    	   		if(!CheckModeChange())
    	   			for(uint32_t i = 0; i < yellow_delay; i += 10) {
    	   			    HAL_Delay(10);
    	   			    if(cmd_ready) {
    	   			        cmd_ready = 0;
    	   			        ProcessCommand((char*)rx_buffer);
    	   			        if(current_mode != MODE_SEQUENCE) return;
    	   			    }
    	   			}

    	   		if(current_mode != MODE_SEQUENCE) return;

    	   		HAL_GPIO_WritePin(NR_PORT, NR_PIN, GPIO_PIN_SET);
    	   		HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_SET);
    	   		south_red();
    	   		east_red();
    	 		west_red();
    	 		if(!CheckModeChange())
    	 			for(uint32_t i = 0; i < transition_delay; i += 10) {
    	 			    HAL_Delay(10);
    	 			    if(cmd_ready) {
    	 			        cmd_ready = 0;
    	 			        ProcessCommand((char*)rx_buffer);
    	 			        if(current_mode != MODE_SEQUENCE) return;
    	 			    }
    	 			}

    	 		if(current_mode != MODE_SEQUENCE) return;
    }
}




/* RunMultiPhase fonksiyonu */
void RunMultiPhase(char* input) {
    // 1. Önce aktif yeşil olan yönleri sarıya çevir
    for (int i = 0; i < direction_count; i++) {
        if (HAL_GPIO_ReadPin(directions[i].green_port, directions[i].green_pin) == GPIO_PIN_SET) {
            HAL_GPIO_WritePin(directions[i].green_port, directions[i].green_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(directions[i].yellow_port, directions[i].yellow_pin, GPIO_PIN_SET);
        }
    }
    HAL_Delay(yellow_delay);

    // 2. Tüm yönleri kırmızıya çevir
    for (int i = 0; i < direction_count; i++) {
        HAL_GPIO_WritePin(directions[i].yellow_port, directions[i].yellow_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(directions[i].red_port, directions[i].red_pin, GPIO_PIN_SET);
    }
    HAL_Delay(transition_delay);

    // 3. Girişte belirtilen yönleri yeşil yap
    char* token = strtok(input, "+");
    while (token != NULL) {
        for (int i = 0; i < direction_count; i++) {
            if (strcmp(token, directions[i].name) == 0) {
                HAL_GPIO_WritePin(directions[i].red_port, directions[i].red_pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(directions[i].green_port, directions[i].green_pin, GPIO_PIN_SET);
                break;
            }
        }
        token = strtok(NULL, "+");
    }
}





uint8_t CheckModeChange(void) {
    uint8_t temp;
    if(HAL_UART_Receive(&huart2, &temp, 1, 10) == HAL_OK) {
        rx_data[rx_index++] = temp;
        if(temp == '\r' || temp == '\n') {
            rx_data[rx_index-1] = '\0';
            ProcessCommand(rx_data);
            rx_index = 0;
            return 1;
        }
    }
    return 0;
}


void Flash_Mode(uint16_t duration_ms) {
    uint32_t start_time = HAL_GetTick();
    while (HAL_GetTick() - start_time < duration_ms) {
        // LED'leri yak
        HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_SET);
        HAL_Delay(flash_delay);

        // LED'leri söndür
        All_LEDs_Off();
        HAL_Delay(flash_delay);
    }
}


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE BEGIN PV */

/* USER CODE BEGIN PV */
/* Yön yapısı ve global değişkenler */


/* USER CODE END PV */


/* USER CODE BEGIN PV */





/* USER CODE END PV */
/* USER CODE END PV */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */


void GetActiveGreenDirections(char* result, size_t result_size);
void RunMultiPhase(char* input);


/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* GetActiveGreenDirections fonksiyonu */
void GetActiveGreenDirections(char* result, size_t result_size) {
    result[0] = '\0'; // Boşalt
    for (int i = 0; i < direction_count; i++) {
        if (HAL_GPIO_ReadPin(directions[i].green_port, directions[i].green_pin) == GPIO_PIN_SET) {
            strncat(result, directions[i].name, result_size - strlen(result) - 1);
            strncat(result, " ", result_size - strlen(result) - 1);
        }
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */





  HAL_UART_Transmit(&huart2, (uint8_t*)"Hazir\r\n", strlen("Hazir\r\n"), HAL_MAX_DELAY);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


  /* main fonksiyonundaki çağrıyı değiştirin */
  HAL_UART_Receive_IT(&huart2, (uint8_t*)&uart_rx_byte, 1);


  const char* activePhase = GetCurrentGreenPhase();
  char msg[32];
  sprintf(msg, "Aktif faz: %s\r\n", activePhase);
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

  while (1) {

	   // Zaman aşımı kontrolü
	    if (current_mode != MODE_SEQUENCE &&
	        (HAL_GetTick() - last_command_time) > COMMAND_TIMEOUT) {
	        current_mode = MODE_SEQUENCE;
	        HAL_UART_Transmit(&huart2, (uint8_t*)"Timeout, returning to SEQUENCE MODE\r\n", 35, HAL_MAX_DELAY);
	        last_command_time = HAL_GetTick(); // Zamanı sıfırla
	    }
      if(cmd_ready)
              {
                  cmd_ready = 0;
                  ProcessCommand((char*)rx_buffer);
              }

  /* USER CODE END 3 */


      switch(current_mode) {
          case MODE_SEQUENCE:
              Sequences_Mode();
              break;
          case MODE_FLASH:
              Flash_Mode(1);
              break;
          case MODE_MANUAL:
              // Manuel modda sadece komut bekler
              break;
          case MODE_IDLE:
        	  break;
          case MODE_OFF:
        	  All_LEDs_Off();
        	  break;
          case MODE_RESET:
              Reset_Mode();  // yeni fonksiyon
              break;
          case MODE_PHASE:
              if (strchr((char*)rx_data, '+')) {
                  RunMultiPhase((char*)rx_data);
              } else {
                  RunPhaseWithTransition((char*)rx_data);
              }
              for (uint32_t i = 0; i < 10000; i += 10) {
                  HAL_Delay(10);
                  if (cmd_ready) {
                      cmd_ready = 0;
                      ProcessCommand((char*)rx_buffer);
                      if (current_mode != MODE_PHASE) break;
                  }
              }
              break;






      }

  }

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  //Interrup ayarları
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);

  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
 void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_6|GPIO_PIN_8
                          |GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_15|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC0 PC1 PC6 PC8
                           PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_6|GPIO_PIN_8
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB10 PB15 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_15|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
