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
#define COMMAND_TIMEOUT 80000
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


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) //her byte(veri) geldiğinde kesme tetiklenir
{
  if(huart->Instance == USART2)
  {
    // Yeni veri alır
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


    HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1); //1byte almak için kesme ayarıdır
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
		HAL_UART_Transmit(&huart2, (uint8_t*)"Sequence Mode = SM\r\nFlash Mode = FM\r\nPhase Mode = phase=1/2/3/4\r\n"
				"Restart = RST\r\nSignal Group Info = SIG?\r\nSignal Info = SIG=X(1-2-3-4)\r\nStop All System = stop\r\nClose All LEDs = off\r\n"
				"Mode Info = MODE?\r\nPhase Info = PHASE?" ,
				strlen("Sequence Mode = SM\r\nFlash Mode = FM\r\nPhase Mode = phase=1/2/3/4\r\nRestart = RST\r\n"
						"Signal Group Info = SIG?\r\nSignal Info = SIG=X(1-2-3-4)\r\nStop All System = stop\r\nClose All LEDs = off\r\n"
						"Mode Info = MODE?\r\nPhase Info = PHASE?" ), HAL_MAX_DELAY);
		}
	else if (strcmp(cmd, "SM") == 0) {
        current_mode = MODE_SEQUENCE;
        HAL_UART_Transmit(&huart2, (uint8_t*)"SEQUENCE MODE STARTED\r\n", strlen("SEQUENCE MODE STARTED\r\n"), HAL_MAX_DELAY);
    }

	else if (strcmp(cmd, "SIG?") == 0) {
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
	else if (strncmp(cmd, "SIG=", 4) == 0 && strlen(cmd) > 4) {
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
    else if(strcmp(cmd, "PHASE?") == 0)
    {
        const char* directionNames[] = {"North", "East", "West", "South"};
        GPIO_TypeDef* ports[] = {NG_PORT, EG_PORT, WG_PORT, SG_PORT};
        uint16_t pins[] = {NG_PIN, EG_PIN, WG_PIN, SG_PIN};

        char msg[128];
        msg[0] = '\0'; // Boş string ile başlat
        strcat(msg, "Active Phase(s): ");

        int found = 0;
        for (int i = 0; i < 4; i++) {
            if (HAL_GPIO_ReadPin(ports[i], pins[i]) == GPIO_PIN_SET) {
                if (found > 0) {
                    strcat(msg, ", ");
                }
                strcat(msg, directionNames[i]);
                found++;
            }
        }

        if (found == 0) {
            strcat(msg, "None");
        }

        strcat(msg, "\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }


    else if (strcmp(cmd, "FM") == 0) {
        current_mode = MODE_FLASH;
        HAL_UART_Transmit(&huart2, (uint8_t*)"FLASH MODE STARTED\r\n", strlen("FLASH MODE STARTED\r\n"), HAL_MAX_DELAY);
    }
    else if (strncmp(cmd, "phase=", 6) == 0) {
        current_mode = MODE_PHASE;
        strncpy((char*)rx_data, cmd + 6, sizeof(rx_data) - 1);
        rx_data[sizeof(rx_data)-1] = '\0';
        sprintf(msg, "Phase: %s\r\n", rx_data);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
    else if (strcmp(cmd, "RST") == 0) {
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


    north_red(); south_red(); east_red(); west_red();
    HAL_Delay(red_delay); // kırmızıya geçiş beklemesi


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

    HAL_Delay(transition_delay);


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

    uint32_t phase_duration = 0;
    if (strcmp(target_phase, "north") == 0) {
            phase_duration = north_delay;
        }
    else if (strcmp(target_phase, "south") == 0) {
            phase_duration = south_delay;
        }
    else if (strcmp(target_phase, "east") == 0) {
            phase_duration = east_delay;
        }
    else if (strcmp(target_phase, "west") == 0) {
            phase_duration = west_delay;
        }

    for(uint32_t i = 0; i < phase_duration; i += 10) {
            HAL_Delay(10);
            if(cmd_ready) {
                cmd_ready = 0;
                ProcessCommand((char*)rx_buffer);
                if(current_mode != MODE_PHASE) return; // Mod değişirse çık
            }
        }
    // Fazı bitir - sarıya çevir
        if (strcmp(target_phase, "north") == 0) {
            north_yellow();
            HAL_Delay(yellow_delay);
            north_red();
        }
        else if (strcmp(target_phase, "south") == 0) {
            south_yellow();
            HAL_Delay(yellow_delay);
            south_red();
        }
        else if (strcmp(target_phase, "east") == 0) {
            east_yellow();
            HAL_Delay(yellow_delay);
            east_red();
        }
        else if (strcmp(target_phase, "west") == 0) {
            west_yellow();
            HAL_Delay(yellow_delay);
            west_red();
        }

        HAL_Delay(red_delay);

        // Sequence moduna geç
        current_mode = MODE_SEQUENCE;
        HAL_UART_Transmit(&huart2, (uint8_t*)"Phase completed, returning to SEQUENCE MODE\r\n",
                          strlen("Phase completed, returning to SEQUENCE MODE\r\n"), HAL_MAX_DELAY);
}



/*const char* GetCurrentGreenPhase(void) {
    if (HAL_GPIO_ReadPin(NG_PORT, NG_PIN) == GPIO_PIN_SET) return "north";
    if (HAL_GPIO_ReadPin(SG_PORT, SG_PIN) == GPIO_PIN_SET) return "south";
    if (HAL_GPIO_ReadPin(EG_PORT, EG_PIN) == GPIO_PIN_SET) return "east";
    if (HAL_GPIO_ReadPin(WG_PORT, WG_PIN) == GPIO_PIN_SET) return "west";
    return 1;
}*/





SystemMode_t Reset_Mode(void)

{


	  	Flash_Mode(3000);
	    All_LEDs_Off();
	    HAL_Delay(1000);
	    Flash_Mode(5000);
	    HAL_Delay(1000);

	    HAL_UART_Transmit(&huart2, (uint8_t*)"System Reset Complete\r\n", 22, HAL_MAX_DELAY);
	    current_mode = MODE_SEQUENCE;
	    return current_mode;
}


void RunPhase(char* phase, uint16_t green_time) {

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


    north_red(); south_red(); east_red(); west_red();
    if(!CheckModeChange()) HAL_Delay(transition_delay);
    if(current_mode != MODE_SEQUENCE) return;


    if (strcmp(phase, "north") == 0) {

        south_yellow();
    } else if (strcmp(phase, "south") == 0) {

        east_yellow();
    } else if (strcmp(phase, "east") == 0) {

        west_yellow();
    } else if (strcmp(phase, "west") == 0) {

        north_yellow();
    }

    if(!CheckModeChange()) HAL_Delay(transition_delay);
    if(current_mode != MODE_SEQUENCE) return;
}



void Sequences_Mode(void) {
    while(current_mode == MODE_SEQUENCE) {

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





void RunMultiPhase(char* input) {
    //Mevcutta yeşil olan yönleri sarıya çevirdik
    for (int i = 0; i < direction_count; i++) {
        if (HAL_GPIO_ReadPin(directions[i].green_port, directions[i].green_pin) == GPIO_PIN_SET) {
            HAL_GPIO_WritePin(directions[i].green_port, directions[i].green_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(directions[i].yellow_port, directions[i].yellow_pin, GPIO_PIN_SET);
        }
    }
    HAL_Delay(yellow_delay);

    // Tüm yönleri kırmızıya çevirdik
    for (int i = 0; i < direction_count; i++) {
        HAL_GPIO_WritePin(directions[i].yellow_port, directions[i].yellow_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(directions[i].red_port, directions[i].red_pin, GPIO_PIN_SET);
    }
    HAL_Delay(transition_delay);

    //Belirtilen yönleri yeşil yaptık
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

        HAL_GPIO_WritePin(NY_PORT, NY_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(WY_PORT, WY_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(EY_PORT, EY_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(SY_PORT, SY_PIN, GPIO_PIN_SET);
        HAL_Delay(flash_delay);


        All_LEDs_Off();
        HAL_Delay(flash_delay);
    }
}


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define ESC "\033"
#define CLEAR_SCREEN ESC "[2J"
#define CURSOR_HOME ESC "[2H"
/* USER CODE BEGIN PV */

/* USER CODE BEGIN PV */



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
RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE BEGIN PV */
/* Yön yapısı ve global değişkenler */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

void Set_RTC_Time(void);
void Set_RTC_Date(void);
void Set_RTC_Alarm(void);
void Show_Menu(void);
void Print_Time(void);
void Print_Date(void);
void Print_Alarm(void);
void HAL_RTC_AlarmEventCallback(RTC_HandleTypeDef *hrtc);
void UART_Receive(uint8_t *data, uint16_t size);
void Clear_Screen(void);

//void GetActiveGreenDirections(char* result, size_t result_size);
void RunMultiPhase(char* input);


/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Clear_Screen(void)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)CLEAR_SCREEN, strlen(CLEAR_SCREEN), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)CURSOR_HOME, strlen(CURSOR_HOME), HAL_MAX_DELAY);
}




void UART_Receive(uint8_t *buffer, uint16_t size)
{
	HAL_UART_Receive(&huart2, buffer, size, HAL_MAX_DELAY);
	buffer[size] = '\0';
}

void Set_RTC_Time(void)
{
	RTC_TimeTypeDef sTime = {0};
	uint8_t buffer[3];


	HAL_UART_Transmit(&huart2, (uint8_t*)"Enter Hours (00-23):", 21, HAL_MAX_DELAY);
	UART_Receive(buffer, 2);
	sTime.Hours = (buffer[0] - '0') * 10 + (buffer[1] - '0');

	HAL_UART_Transmit(&huart2, (uint8_t*)"Enter Minutes (00-59):", 24, HAL_MAX_DELAY);
	UART_Receive(buffer, 2);
	sTime.Minutes = (buffer[0] - '0') * 10 + (buffer[1] - '0');


	HAL_UART_Transmit(&huart2, (uint8_t*)"Enter Seconds (00-59):", 24, HAL_MAX_DELAY);
	UART_Receive(buffer, 2);
	sTime.Seconds = (buffer[0] - '0') * 10 + (buffer[1] - '0');

	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	if(HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_UART_Transmit(&huart2, (uint8_t*)"\n\rTime is set!\n\r", 16, HAL_MAX_DELAY);
}

void Set_RTC_Date(void)
{
	RTC_DateTypeDef sDate = {0};
	uint8_t buffer[3];



	HAL_UART_Transmit(&huart2, (uint8_t*)"Enter Year (00-99):", 20, HAL_MAX_DELAY);
	UART_Receive(buffer, 2);
	sDate.Year = (buffer[0] - '0') * 10 + (buffer[1] - '0');

	HAL_UART_Transmit(&huart2, (uint8_t*)"Enter Month (01-12):", 21, HAL_MAX_DELAY);
	UART_Receive(buffer, 2);
	sDate.Month = (buffer[0] - '0') * 10 + (buffer[1] - '0');

	HAL_UART_Transmit(&huart2, (uint8_t*)"Enter Date (01-31):", 21, HAL_MAX_DELAY);
	UART_Receive(buffer, 2);
	sDate.Date = (buffer[0] - '0') * 10 + (buffer[1] - '0');

	HAL_UART_Transmit(&huart2, (uint8_t*)"Enter WeekDay (1=Mon, 7=Sun):", 33, HAL_MAX_DELAY);
	UART_Receive(buffer, 2);
	sDate.WeekDay = (buffer[0] - '0') * 10 + (buffer[1] - '0');

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_UART_Transmit(&huart2, (uint8_t*)"\n\rDate is set!\n\r", 16, HAL_MAX_DELAY);


}

void Print_Time(void)
{
	RTC_TimeTypeDef sTime;
	char buffer[50];

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	sprintf(buffer, "Current Time: %02d:%02d:%02d\n\r", sTime.Hours, sTime.Minutes, sTime.Seconds);
	HAL_UART_Transmit(&huart2, (uint8_t*)buffer,strlen(buffer), HAL_MAX_DELAY);
}

void Print_Date(void)
{
	RTC_DateTypeDef sDate;
	char buffer[50];

	if (HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
	        Error_Handler(); // Hata durumunda işlem yap
	    }

	HAL_RTC_GetTime(&hrtc, &sDate, RTC_FORMAT_BIN);
	sprintf(buffer, "Current Date: %02d-%02d-20%02d\n\r", sDate.Date, sDate.Month, sDate.Year);
	HAL_UART_Transmit(&huart2, (uint8_t*)buffer,strlen(buffer), HAL_MAX_DELAY);
}

void Show_Menu(void)
{
	Clear_Screen();
	const char *menu =	"\r\nMenu:\n\r"
						"0. Show Menu:\n\r"
						"1. Show Time:\n\r"
						"2. Show Date:\n\r"
						"3. Set Time:\n\r"
						"4. Set Date:\n\r";

	HAL_UART_Transmit(&huart2, (uint8_t*)menu, strlen(menu), HAL_MAX_DELAY);
}
/* GetActiveGreenDirections fonksiyonu */
/*void GetActiveGreenDirections(char* result, size_t result_size) {
    result[0] = '\0'; // Boşalt
    for (int i = 0; i < direction_count; i++) {
        if (HAL_GPIO_ReadPin(directions[i].green_port, directions[i].green_pin) == GPIO_PIN_SET) {
            strncat(result, directions[i].name, result_size - strlen(result) - 1);
            strncat(result, " ", result_size - strlen(result) - 1);
        }

    }
}*/
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
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  Show_Menu();




  HAL_UART_Transmit(&huart2, (uint8_t*)"Hazir\r\n", strlen("Hazir\r\n"), HAL_MAX_DELAY);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */



  HAL_UART_Receive_IT(&huart2, (uint8_t*)&uart_rx_byte, 1);


  /*const char* activePhase = GetCurrentGreenPhase();
  char msg[32];
  sprintf(msg, "Aktif faz: %s\r\n", activePhase);
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);*/

  while (1)
  {

	  uint8_t userChoice = 0;

	  UART_Receive(&userChoice, 1);

	  	  switch(userChoice)
	  	  {
	  	  case '0':
	  	  	  Show_Menu();
	  	  	  break;
	  	  case '1':
	  		  Print_Time();
	  		  break;
	  	  case '2':
	  		  Print_Date();
	  		  break;
	  	  case '3':
	  		  Set_RTC_Time();
	  		  break;
	  	  case '4':
	  		  Set_RTC_Date();
	  		  break;
	  	  default:
	  		  HAL_UART_Transmit(&huart2, (uint8_t*)"Invalid Choice.\n\r", 36, HAL_MAX_DELAY);
	  		  break;

	  	  }




	   // Zaman aşımı kontrolünü sağlar
	    if (current_mode != MODE_SEQUENCE &&
	        (HAL_GetTick() - last_command_time) > COMMAND_TIMEOUT) {
	        current_mode = MODE_SEQUENCE;
	        HAL_UART_Transmit(&huart2, (uint8_t*)"Timeout, returning to SEQUENCE MODE\r\n", strlen("Timeout, returning to SEQUENCE MODE\r\n"), HAL_MAX_DELAY);
	        last_command_time = HAL_GetTick(); // Zamanı sıfırlar
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
                    Reset_Mode();
                    break;
                case MODE_PHASE:
                    if (strchr((char*)rx_data, '+')) {
                        // Çoklu faz modu
                        RunMultiPhase((char*)rx_data);
                        // Çoklu faz için bekleme süresi
                        for (uint32_t i = 0; i < 10000; i += 10) {
                            HAL_Delay(10);
                            if (cmd_ready) {
                                cmd_ready = 0;
                                ProcessCommand((char*)rx_buffer);
                                if (current_mode != MODE_PHASE) break;
                            }
                        }
                    } else {
                        // Tek faz modu - geçiş ile
                        RunPhaseWithTransition((char*)rx_data);
                        // RunPhaseWithTransition fonksiyonu kendi içinde mode değişimini yapacak
                        // Bu yüzden buraya ekstra kod eklemeye gerek yok
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.SubSeconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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

  /*Interrup ayarlarını burda yaptık
   HAL_UART_Receive_IT(...) olan kısım 1byte almak için kesme ayarıdır



    */
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

