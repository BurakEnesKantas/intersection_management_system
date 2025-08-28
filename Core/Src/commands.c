#include "commands.h"

// ProcessCommand implementasyonu
void ProcessCommand(char* cmd) {
	char msg[700];  // UART'a yazılacak mesajlar için buffer
	timerCounter = HAL_GetTick();
	// Komut geldiğinde LED blink
	HAL_GPIO_WritePin(UART_PORT, UART_PIN, GPIO_PIN_SET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(UART_PORT, UART_PIN, GPIO_PIN_RESET);

	if (strcmp(cmd, "T?") == 0) {
	    RTC_TimeTypeDef sTime;
	    int hour;

	    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);


	    if(time_zone_offset == 3){
	    	hour = sTime.Hours;
	    	if (hour >= 24) hour -= 24;
	    	else if (hour < 0) hour += 24;
	    }
	    else if(time_zone_offset < 3 || time_zone_offset >3){
	    	hour = sTime.Hours + time_zone_offset -3;
	    	if (hour >= 24) hour -= 24;
	    	else if (hour < 0) hour += 24;
	    }



	    snprintf(msg, sizeof(msg),
	             "Current Time (UTC %+d): %02d:%02d:%02d\r\n",
	             time_zone_offset, hour, sTime.Minutes, sTime.Seconds);


	    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	}
	else if (strcmp(cmd, "D?") == 0){
		RTC_DateTypeDef sDate;

		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		snprintf(msg, sizeof(msg),
				"Current Date: %02d/%02d/%04d\r\n",
				sDate.Date, sDate.Month, sDate.Year + 2000);

		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	}

	 else if (strcmp(cmd, "INFO") == 0) {
		        RTC_TimeTypeDef sTime;
		        RTC_DateTypeDef sDate;

		        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		        snprintf(msg, sizeof(msg),
		            "=== TRAFFIC LIGHT CONTROLLER INFO ===\r\n"
		            "Current Time: %02d:%02d:%02d\r\n"
		            "Current Date: %02d/%02d/%04d\r\n"
		            "=====================================\r\n"
		            "Available Commands:\r\n"
		            "TZ=+-X set time zone\r\n"
		            "T? - Show current time\r\n"
		            "ST=HH:MM:SS - Set time\r\n"
		            "SD=DD/MM/YYYY - Set date\r\n"
		            "SM - Sequence Mode\r\n"
		            "FM - Flash Mode\r\n"
		            "PM=1(north)/2(south)/3(east)/4(west) - Phase Mode\r\n"
		            "RST - Restart\r\n"
		            "SIG? - Signal Group Info\r\n"
		            "SIG=X - Signal Info\r\n"
		        	"MODE? - Mode Info\r\n"
		            "PHASE? - Phase Info\r\n"
		            "CHANGE THE DELAY:\r\n"
		            "south=delay(ms)\r\n"
		            "north=delay(ms)\r\n"
		            "east=delay(ms)\r\n"
		            "west=delay(ms)\r\n"
		            "transition=delay(ms)\r\n"
		            "STOP - Stop All System\r\n"
		            "OFF - Close All LEDs\r\n",
		            sTime.Hours, sTime.Minutes, sTime.Seconds,
		            sDate.Date, sDate.Month, sDate.Year + 2000);

		        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
		    }
	 else if (strcmp(cmd, "TZ?") == 0) {
	     snprintf(msg, sizeof(msg), "Current Time Zone: UTC %+d\r\n", time_zone_offset);
	     HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	 }

	    else if (strncmp(cmd, "ST=", 3) == 0) {
	        // Format: settime=HH:MM:SS
	        int hours, minutes, seconds;
	        if (sscanf(cmd + 3, "%d:%d:%d", &hours, &minutes, &seconds) == 3) {
	            if (hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59 && seconds >= 0 && seconds <= 59) {
	                RTC_TimeTypeDef sTime = {0};
	                sTime.Hours = hours;
	                sTime.Minutes = minutes;
	                sTime.Seconds = seconds;
	                sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	                sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	                if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) == HAL_OK) {
	                    snprintf(msg, sizeof(msg), "Time set to: %02d:%02d:%02d\r\n", hours, minutes, seconds);
	                } else {
	                    snprintf(msg, sizeof(msg), "Error setting time!\r\n");
	                }
	            } else {
	                snprintf(msg, sizeof(msg), "Invalid time format! Use HH:MM:SS (24h format)\r\n");
	            }
	        } else {
	            snprintf(msg, sizeof(msg), "Invalid format! Use: settime=HH:MM:SS\r\n");
	        }
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	    }
	    else if (strncmp(cmd, "SD=", 3) == 0) {
	        // Format: setdate=DD/MM/YYYY
	        int day, month, year;
	        if (sscanf(cmd + 3, "%d.%d.%d", &day, &month, &year) == 3) {
	            if (day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 2000 && year <= 2099) {
	                RTC_DateTypeDef sDate = {0};
	                sDate.Date = day;
	                sDate.Month = month;
	                sDate.Year = year - 2000; // RTC 20xx formatında çalışır
	                sDate.WeekDay = RTC_WEEKDAY_MONDAY; // Basit çözüm, haftanın gününü hesaplayabilirsiniz

	                if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) == HAL_OK) {
	                    snprintf(msg, sizeof(msg), "Date set to: %02d/%02d/%04d\r\n", day, month, year);
	                } else {
	                    snprintf(msg, sizeof(msg), "Error setting date!\r\n");
	                }
	            } else {
	                snprintf(msg, sizeof(msg), "Invalid date values!\r\n");
	            }
	        } else {
	            snprintf(msg, sizeof(msg), "Invalid format! Use: setdate=DD/MM/YYYY\r\n");
	        }
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	    }
	    else if (strncmp(cmd, "TZ=", 3) == 0) {
	        int tz = atoi(cmd + 3);
	        if (tz >= -12 && tz <= 14) {
	            time_zone_offset = tz;
	            snprintf(msg, sizeof(msg), "Time zone set to UTC %+d\r\n", time_zone_offset);
	        } else {
	            snprintf(msg, sizeof(msg), "Invalid time zone! Range: -12 to +14\r\n");
	        }
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
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

        char msg[512];
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
    	All_LEDs_Off();
    	HAL_Delay(250);
        current_mode = MODE_FLASH;
        HAL_UART_Transmit(&huart2, (uint8_t*)"FLASH MODE STARTED\r\n", strlen("FLASH MODE STARTED\r\n"), HAL_MAX_DELAY);
    }
    else if (strcmp(cmd, "RST Delay") == 0){
    	north_delay = 4000;
    	south_delay = 4000;
    	east_delay = 4000;
    	west_delay = 4000;
    	transition_delay = 1500;
    	HAL_UART_Transmit(&huart2, (uint8_t*)"All Delays Reset\r\n", strlen("All Delays Reset\r\n"), HAL_MAX_DELAY);
    }
    else if (strncmp(cmd, "PM=", 3) == 0) {
        current_mode = MODE_PHASE;
        int phase_num = atoi(cmd + 3);

        // Her bir sayıya yön atadık
        const char* phase_name = "";
        switch(phase_num) {
            case 1: phase_name = "north"; break;
            case 2: phase_name = "south"; break;
            case 3: phase_name = "east"; break;
            case 4: phase_name = "west"; break;
            default:
                HAL_UART_Transmit(&huart2, (uint8_t*)"Invalid phase number! Use 1-4\r\n",
                                strlen("Invalid phase number! Use 1-4\r\n"), HAL_MAX_DELAY);
                return;
        }

        strncpy((char*)rx_data, phase_name, sizeof(rx_data) - 1);
        rx_data[sizeof(rx_data)-1] = '\0';

        char msg[50];
        snprintf(msg, sizeof(msg), "Phase %d (%s) activated\r\n", phase_num, phase_name);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
    else if (strcmp(cmd, "RST") == 0) {
    	HAL_UART_Transmit(&huart2, (uint8_t*)"WAIT FOR RESTART...\r\n", strlen("WAIT FOR RESTART...\r\n"), HAL_MAX_DELAY);
    	current_mode = MODE_RESET;
        HAL_UART_Transmit(&huart2, (uint8_t*)"Sequence Mode Continues\r\n", strlen("Sequence Mode Continues\r\n"), HAL_MAX_DELAY);
    }
    else if (strcmp(cmd, "STOP") == 0) {
        current_mode = MODE_IDLE;
        HAL_UART_Transmit(&huart2, (uint8_t*)"ALL MODES STOPPED\r\n", strlen("ALL MODES STOPPED\r\n"), HAL_MAX_DELAY);
    }
    else if (strcmp(cmd, "OFF") == 0)
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
