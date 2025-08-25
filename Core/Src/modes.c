#include "modes.h"
#include "main.h"

// Global değişkenler
GPIO_PinState lastLedState[NUM_LEDS] = {0};
GPIO_TypeDef* ledPorts[NUM_LEDS];
uint16_t ledPins[NUM_LEDS];
char* ledNames[NUM_LEDS];
uint32_t lastLedLogTime = 0;
uint8_t allOffLogged = 0; // tüm LEDler off mesajı için

typedef enum {
    PHASE_NORTH_GREEN,
    PHASE_NORTH_YELLOW,
    PHASE_TRANSITION_N_S,
    PHASE_SOUTH_GREEN,
    PHASE_SOUTH_YELLOW,
    PHASE_TRANSITION_S_E,
    PHASE_EAST_GREEN,
    PHASE_EAST_YELLOW,
    PHASE_TRANSITION_E_W,
    PHASE_WEST_GREEN,
    PHASE_WEST_YELLOW,
    PHASE_TRANSITION_W_N
} TrafficPhase_t;

TrafficPhase_t current_phase = PHASE_NORTH_GREEN;
uint32_t phase_start_tick = 0;

void initLeds(void) {
    ledPorts[0] = NG_PORT; ledPins[0] = NG_PIN; ledNames[0] = "North Green";
    ledPorts[1] = NY_PORT; ledPins[1] = NY_PIN; ledNames[1] = "North Yellow";
    ledPorts[2] = NR_PORT; ledPins[2] = NR_PIN; ledNames[2] = "North Red";
    ledPorts[3] = SG_PORT; ledPins[3] = SG_PIN; ledNames[3] = "South Green";
    ledPorts[4] = SY_PORT; ledPins[4] = SY_PIN; ledNames[4] = "South Yellow";
    ledPorts[5] = SR_PORT; ledPins[5] = SR_PIN; ledNames[5] = "South Red";
    ledPorts[6] = EG_PORT; ledPins[6] = EG_PIN; ledNames[6] = "East Green";
    ledPorts[7] = EY_PORT; ledPins[7] = EY_PIN; ledNames[7] = "East Yellow";
    ledPorts[8] = ER_PORT; ledPins[8] = ER_PIN; ledNames[8] = "East Red";
    ledPorts[9] = WG_PORT; ledPins[9] = WG_PIN; ledNames[9] = "West Green";
    ledPorts[10] = WY_PORT; ledPins[10] = WY_PIN; ledNames[10] = "West Yellow";
    ledPorts[11] = WR_PORT; ledPins[11] = WR_PIN; ledNames[11] = "West Red";
    // Başlangıçta tüm LED durumlarını OFF olarak ayarla
    for(int i=0; i<NUM_LEDS; i++) lastLedState[i] = GPIO_PIN_RESET;
}


// Global değişkenler (main.h'den extern)
volatile SystemMode_t current_mode = MODE_IDLE;

void SendLedStatus(char* led, char* state) {
    char msg[50];
    sprintf(msg, "\r\n%s %s\r\n", led, state);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

// LED yakma fonksiyonu, değişiklik olduğunda logla
void SetLed(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, char* ledName, GPIO_PinState state) {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, state);

    // Durum değiştiyse logla
    for(int i=0; i<NUM_LEDS; i++) {
        if(ledPorts[i] == GPIOx && ledPins[i] == GPIO_Pin) {
            if(lastLedState[i] != state) {
                lastLedState[i] = state;
                SendLedStatus(ledName, state == GPIO_PIN_SET ? "ON" : "OFF");
                allOffLogged = 0; // LED açıldıysa OFF mesajını resetle
            }
            break;
        }
    }
}

// LED durumlarını kontrol et ve tümü OFF ise 1 kere logla
void CheckAllOffLog(void) {
    uint8_t anyOn = 0;
    for(int i=0; i<NUM_LEDS; i++) {
        if(HAL_GPIO_ReadPin(ledPorts[i], ledPins[i]) == GPIO_PIN_SET) {
            anyOn = 1;
            break;
        }
    }

    if(!anyOn && !allOffLogged) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"All LEDs OFF\r\n", 14, HAL_MAX_DELAY);
        allOffLogged = 1; // tekrar yazmasın
    }
}
void UpdateLedStatusLog(void)
{
    static uint32_t lastTick = 0;
    static uint8_t allOffLogged = 0;

    uint32_t now = HAL_GetTick();
    if(now - lastTick >= 1000) // her saniye
    {
        lastTick = now;

        uint8_t allOff = 1;
        char msg[100];

        // LED durumlarını sırayla kontrol et
        for(int i = 0; i < NUM_LEDS; i++)
        {
            GPIO_PinState state = HAL_GPIO_ReadPin(ledPorts[i], ledPins[i]);
            if(state == GPIO_PIN_SET)
                allOff = 0;

            // Sadece ON olanları hemen yaz
            if(state == GPIO_PIN_SET)
            {
                sprintf(msg, "%s ON\r\n", ledNames[i]);
                HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
            }
        }

        // Eğer tüm LED’ler OFF ve daha önce yazılmadıysa mesajı 1 kere yaz
        if(allOff && !allOffLogged)
        {
            for(int i = 0; i < NUM_LEDS; i++)
            {
                sprintf(msg, "%s OFF\r\n", ledNames[i]);
                HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
            }
            allOffLogged = 1;
        }
        else if(!allOff)
        {
            allOffLogged = 0;
        }

        // Saat/dakika/saniye logu
        uint32_t t = now / 1000;
        uint32_t h = (t / 3600) % 24;
        uint32_t m = (t / 60) % 60;
        uint32_t s = t % 60;
        sprintf(msg, "Time: %02lu:%02lu:%02lu\r\n", h, m, s);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
}




// Sequences_Mode implementasyonu
void Sequences_Mode(void) {
    uint32_t now = HAL_GetTick();

    // 1. LED logunu güncelle
    UpdateLedStatusLog();

    // 2. Komut varsa işle
    if(cmd_ready) {
        cmd_ready = 0;
        ProcessCommand((char*)rx_buffer);
        if(current_mode != MODE_SEQUENCE) return; // mod değiştiyse çık
    }

    // 3. Faz yönetimi
    switch(current_phase) {

        case PHASE_NORTH_GREEN:
            if(phase_start_tick == 0) {
                north_green(); south_red(); east_red(); west_red();
                phase_start_tick = now;
            }
            if(now - phase_start_tick >= north_delay) {
                current_phase = PHASE_NORTH_YELLOW;
                phase_start_tick = 0;
            }
            break;

        case PHASE_NORTH_YELLOW:
            if(phase_start_tick == 0) {
                north_yellow(); south_red(); east_red(); west_red();
                phase_start_tick = now;
            }
            if(now - phase_start_tick >= yellow_delay) {
                current_phase = PHASE_TRANSITION_N_S;
                phase_start_tick = 0;
            }
            break;

        case PHASE_TRANSITION_N_S:
            if(phase_start_tick == 0) {
                north_red();
                SetLed(SR_PORT, SR_PIN, "South Red", GPIO_PIN_SET);
                SetLed(SY_PORT, SY_PIN, "South Yellow", GPIO_PIN_SET);
                east_red(); west_red();
                phase_start_tick = now;
            }
            if(now - phase_start_tick >= transition_delay) {
                current_phase = PHASE_SOUTH_GREEN;
                phase_start_tick = 0;
            }
            break;

        case PHASE_SOUTH_GREEN:
            if(phase_start_tick == 0) {
                south_green(); north_red(); east_red(); west_red();
                phase_start_tick = now;
            }
            if(now - phase_start_tick >= south_delay) {
                current_phase = PHASE_SOUTH_YELLOW;
                phase_start_tick = 0;
            }
            break;

        case PHASE_SOUTH_YELLOW:
            if(phase_start_tick == 0) {
                south_yellow(); north_red(); east_red(); west_red();
                phase_start_tick = now;
            }
            if(now - phase_start_tick >= yellow_delay) {
                current_phase = PHASE_TRANSITION_S_E;
                phase_start_tick = 0;
            }
            break;

        case PHASE_TRANSITION_S_E:
            if(phase_start_tick == 0) {
                north_red(); south_red();
                SetLed(ER_PORT, ER_PIN, "East Red", GPIO_PIN_SET);
                SetLed(EY_PORT, EY_PIN, "East Yellow", GPIO_PIN_SET);
                west_red();
                phase_start_tick = now;
            }
            if(now - phase_start_tick >= transition_delay) {
                current_phase = PHASE_EAST_GREEN;
                phase_start_tick = 0;
            }
            break;

        case PHASE_EAST_GREEN:
            if(phase_start_tick == 0) {
                east_green(); north_red(); south_red(); west_red();
                phase_start_tick = now;
            }
            if(now - phase_start_tick >= east_delay) {
                current_phase = PHASE_EAST_YELLOW;
                phase_start_tick = 0;
            }
            break;

        case PHASE_EAST_YELLOW:
            if(phase_start_tick == 0) {
                east_yellow(); north_red(); south_red(); west_red();
                phase_start_tick = now;
            }
            if(now - phase_start_tick >= yellow_delay) {
                current_phase = PHASE_TRANSITION_E_W;
                phase_start_tick = 0;
            }
            break;

        case PHASE_TRANSITION_E_W:
            if(phase_start_tick == 0) {
                north_red(); south_red(); east_red();
                SetLed(WR_PORT, WR_PIN, "West Red", GPIO_PIN_SET);
                SetLed(WY_PORT, WY_PIN, "West Yellow", GPIO_PIN_SET);
                phase_start_tick = now;
            }
            if(now - phase_start_tick >= transition_delay) {
                current_phase = PHASE_WEST_GREEN;
                phase_start_tick = 0;
            }
            break;

        case PHASE_WEST_GREEN:
            if(phase_start_tick == 0) {
                west_green(); north_red(); south_red(); east_red();
                phase_start_tick = now;
            }
            if(now - phase_start_tick >= west_delay) {
                current_phase = PHASE_WEST_YELLOW;
                phase_start_tick = 0;
            }
            break;

        case PHASE_WEST_YELLOW:
            if(phase_start_tick == 0) {
                west_yellow(); north_red(); south_red(); east_red();
                phase_start_tick = now;
            }
            if(now - phase_start_tick >= yellow_delay) {
                current_phase = PHASE_TRANSITION_W_N;
                phase_start_tick = 0;
            }
            break;

        case PHASE_TRANSITION_W_N:
            if(phase_start_tick == 0) {
                north_red(); south_red(); east_red();
                SetLed(NR_PORT, NR_PIN, "North Red", GPIO_PIN_SET);
                SetLed(NY_PORT, NY_PIN, "North Yellow", GPIO_PIN_SET);
                west_red(); south_red(); east_red();
                phase_start_tick = now;
            }
            if(now - phase_start_tick >= transition_delay) {
                current_phase = PHASE_NORTH_GREEN; // Döngü başa dönüyor
                phase_start_tick = 0;
            }
            break;
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
