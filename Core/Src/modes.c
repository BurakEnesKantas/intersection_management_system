#include "modes.h"
#include "main.h"

// Global değişkenler (main.h'den extern)
volatile SystemMode_t current_mode = MODE_IDLE;

void SendLedStatus(char* led, char* state) {
    char msg[50];
    sprintf(msg, "\r\n%s %s\r\n", led, state);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

// LED yakma fonksiyonu
void SetLed(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, char* ledName, GPIO_PinState state) {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, state);
    if(state == GPIO_PIN_SET) {
        SendLedStatus(ledName, "ON");
    } else {
        SendLedStatus(ledName, "OFF");
    }
}

// Sequences_Mode implementasyonu
void Sequences_Mode(void) {
    while(current_mode == MODE_SEQUENCE) {

    	        north_green();
    	        south_red();
    	        east_red();
    	        west_red();
    	        if(1)
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
    	        if(1)
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
    	        SetLed(SR_PORT, SR_PIN, "South Red", GPIO_PIN_SET);
    	        SetLed(SY_PORT, SY_PIN, "South Yellow",GPIO_PIN_SET);
    	       	east_red();
    	       	west_red();
    	        if(1)
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
    	        if(1)
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
    	        if(1)
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
    	        SetLed(ER_PORT, ER_PIN, "East Red",GPIO_PIN_SET);
    	        SetLed(EY_PORT, EY_PIN, "East Yellow",GPIO_PIN_SET);
    	       	west_red();
    	       	if(1)
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
    	       	if(1)
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
    	   		if(1)
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
    	   		SetLed(WR_PORT, WR_PIN, "West Red",GPIO_PIN_SET);
    	   		SetLed(WY_PORT, WY_PIN, "West Yellow",GPIO_PIN_SET);
    	   		if(1)
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
    	   		if(1)
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
    	   		if(1)
    	   			for(uint32_t i = 0; i < yellow_delay; i += 10) {
    	   			    HAL_Delay(10);
    	   			    if(cmd_ready) {
    	   			        cmd_ready = 0;
    	   			        ProcessCommand((char*)rx_buffer);
    	   			        if(current_mode != MODE_SEQUENCE) return;
    	   			    }
    	   			}

    	   		if(current_mode != MODE_SEQUENCE) return;

    	   		SetLed(NR_PORT, NR_PIN, "North Yellow",GPIO_PIN_SET);
    	   		SetLed(NY_PORT, NY_PIN, "North Yellow",GPIO_PIN_SET);
    	   		south_red();
    	   		east_red();
    	 		west_red();
    	 		if(1)
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

// Flash_Mode
void Flash_Mode(uint16_t duration_ms) {
    uint32_t start_time = HAL_GetTick();
    while (HAL_GetTick() - start_time < duration_ms) {

    	SetLed(NY_PORT, NY_PIN, "North Yellow",GPIO_PIN_SET);
    	SetLed(WY_PORT, WY_PIN, "West Yellow",GPIO_PIN_SET);
    	SetLed(EY_PORT, EY_PIN, "East Yellow",GPIO_PIN_SET);
    	SetLed(SY_PORT, SY_PIN, "South Yellow",GPIO_PIN_SET);
        HAL_Delay(flash_delay);


        All_LEDs_Off();
        HAL_Delay(flash_delay);
    }
}

// RunPhaseWithTransition (main.c'den kopyalayın)
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

// RunMultiPhase
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

// Reset_Mode
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
