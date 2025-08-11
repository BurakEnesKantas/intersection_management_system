#ifndef TRAFFIC_LIGHTS_H
#define TRAFFIC_LIGHTS_H

#include "main.h"

// Struct tanımları
typedef struct {
    const char* name;
    GPIO_TypeDef* green_port;
    uint16_t green_pin;
    GPIO_TypeDef* yellow_port;
    uint16_t yellow_pin;
    GPIO_TypeDef* red_port;
    uint16_t red_pin;
} Direction;

extern Direction directions[];
extern const int direction_count;

// Fonksiyon prototipleri
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
void All_LEDs_Off(void);

#endif // TRAFFIC_LIGHT_H
