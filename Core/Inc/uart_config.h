#ifndef UART_CONFIG_H
#define UART_CONFIG_H

#include "main.h"

#define RX_BUFFER_SIZE 50

extern volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
extern volatile uint8_t rx_index;
extern volatile uint8_t cmd_ready;
extern uint8_t uart_rx_byte;

// Fonksiyon prototipleri
void MX_USART2_UART_Init(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif // UART_CONFIG_H
