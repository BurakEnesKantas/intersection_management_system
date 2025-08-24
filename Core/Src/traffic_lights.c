#include "traffic_lights.h"

Direction directions[] = {
    {"north", NG_PORT, NG_PIN, NY_PORT, NY_PIN, NR_PORT, NR_PIN},
    {"south", SG_PORT, SG_PIN, SY_PORT, SY_PIN, SR_PORT, SR_PIN},
    {"east",  EG_PORT, EG_PIN, EY_PORT, EY_PIN, ER_PORT, ER_PIN},
    {"west",  WG_PORT, WG_PIN, WY_PORT, WY_PIN, WR_PORT, WR_PIN},
};

const int direction_count = sizeof(directions) / sizeof(Direction);
void SetLed(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, const char* name, GPIO_PinState state);

// Işık kontrol fonksiyonları
void west_red (void) // Batı - West Red
{
    SetLed(WG_PORT, WG_PIN, "West Green", GPIO_PIN_RESET);
    SetLed(WY_PORT, WY_PIN, "West Yellow", GPIO_PIN_RESET);
    SetLed(WR_PORT, WR_PIN, "West Red", GPIO_PIN_SET);
}

void south_red (void) // Güney - South Red
{
    SetLed(SG_PORT, SG_PIN, "South Green", GPIO_PIN_RESET);
    SetLed(SY_PORT, SY_PIN, "South Yellow", GPIO_PIN_RESET);
    SetLed(SR_PORT, SR_PIN, "South Red", GPIO_PIN_SET);
}

void north_red (void) // Kuzey - North Red
{
    SetLed(NG_PORT, NG_PIN, "North Green", GPIO_PIN_RESET);
    SetLed(NY_PORT, NY_PIN, "North Yellow", GPIO_PIN_RESET);
    SetLed(NR_PORT, NR_PIN, "North Red", GPIO_PIN_SET);
}

void east_red (void) // Doğu - East Red
{
    SetLed(EG_PORT, EG_PIN, "East Green", GPIO_PIN_RESET);
    SetLed(EY_PORT, EY_PIN, "East Yellow", GPIO_PIN_RESET);
    SetLed(ER_PORT, ER_PIN, "East Red", GPIO_PIN_SET);
}

void west_green (void) // Batı - West Green
{
    SetLed(WG_PORT, WG_PIN, "West Green", GPIO_PIN_SET);
    SetLed(WY_PORT, WY_PIN, "West Yellow", GPIO_PIN_RESET);
    SetLed(WR_PORT, WR_PIN, "West Red", GPIO_PIN_RESET);
}

void south_green (void) // Güney - South Green
{
    SetLed(SG_PORT, SG_PIN, "South Green", GPIO_PIN_SET);
    SetLed(SY_PORT, SY_PIN, "South Yellow", GPIO_PIN_RESET);
    SetLed(SR_PORT, SR_PIN, "South Red", GPIO_PIN_RESET);
}

void north_green (void) // Kuzey - North Green
{
    SetLed(NG_PORT, NG_PIN, "North Green", GPIO_PIN_SET);
    SetLed(NY_PORT, NY_PIN, "North Yellow", GPIO_PIN_RESET);
    SetLed(NR_PORT, NR_PIN, "North Red", GPIO_PIN_RESET);
}

void east_green (void) // Doğu - East Green
{
    SetLed(EG_PORT, EG_PIN, "East Green", GPIO_PIN_SET);
    SetLed(EY_PORT, EY_PIN, "East Yellow", GPIO_PIN_RESET);
    SetLed(ER_PORT, ER_PIN, "East Red", GPIO_PIN_RESET);
}

void west_yellow (void) // Batı - West Yellow
{
    SetLed(WG_PORT, WG_PIN, "West Green", GPIO_PIN_RESET);
    SetLed(WY_PORT, WY_PIN, "West Yellow", GPIO_PIN_SET);
    SetLed(WR_PORT, WR_PIN, "West Red", GPIO_PIN_RESET);
}

void south_yellow (void) // Güney - South Yellow
{
    SetLed(SG_PORT, SG_PIN, "South Green", GPIO_PIN_RESET);
    SetLed(SY_PORT, SY_PIN, "South Yellow", GPIO_PIN_SET);
    SetLed(SR_PORT, SR_PIN, "South Red", GPIO_PIN_RESET);
}

void north_yellow (void) // Kuzey - North Yellow
{
    SetLed(NG_PORT, NG_PIN, "North Green", GPIO_PIN_RESET);
    SetLed(NY_PORT, NY_PIN, "North Yellow", GPIO_PIN_SET);
    SetLed(NR_PORT, NR_PIN, "North Red", GPIO_PIN_RESET);
}

void east_yellow (void) // Doğu - East Yellow
{
    SetLed(EG_PORT, EG_PIN, "East Green", GPIO_PIN_RESET);
    SetLed(EY_PORT, EY_PIN, "East Yellow", GPIO_PIN_SET);
    SetLed(ER_PORT, ER_PIN, "East Red", GPIO_PIN_RESET);
}

void All_LEDs_Off(void) {
    SetLed(NR_PORT, NR_PIN, "North Red", GPIO_PIN_RESET);
    SetLed(NY_PORT, NY_PIN, "North Yellow", GPIO_PIN_RESET);
    SetLed(NG_PORT, NG_PIN, "North Green", GPIO_PIN_RESET);

    SetLed(WR_PORT, WR_PIN, "West Red", GPIO_PIN_RESET);
    SetLed(WY_PORT, WY_PIN, "West Yellow", GPIO_PIN_RESET);
    SetLed(WG_PORT, WG_PIN, "West Green", GPIO_PIN_RESET);

    SetLed(SR_PORT, SR_PIN, "South Red", GPIO_PIN_RESET);
    SetLed(SY_PORT, SY_PIN, "South Yellow", GPIO_PIN_RESET);
    SetLed(SG_PORT, SG_PIN, "South Green", GPIO_PIN_RESET);

    SetLed(ER_PORT, ER_PIN, "East Red", GPIO_PIN_RESET);
    SetLed(EY_PORT, EY_PIN, "East Yellow", GPIO_PIN_RESET);
    SetLed(EG_PORT, EG_PIN, "East Green", GPIO_PIN_RESET);
}
