#include "traffic_lights.h"

Direction directions[] = {
    {"north", NG_PORT, NG_PIN, NY_PORT, NY_PIN, NR_PORT, NR_PIN},
    {"south", SG_PORT, SG_PIN, SY_PORT, SY_PIN, SR_PORT, SR_PIN},
    {"east",  EG_PORT, EG_PIN, EY_PORT, EY_PIN, ER_PORT, ER_PIN},
    {"west",  WG_PORT, WG_PIN, WY_PORT, WY_PIN, WR_PORT, WR_PIN},
};

const int direction_count = sizeof(directions) / sizeof(Direction);

// Işık kontrol fonksiyonları
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
