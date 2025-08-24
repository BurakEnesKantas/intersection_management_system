#include "uart_config.h"

volatile uint8_t rx_buffer[RX_BUFFER_SIZE] = {0};
volatile uint8_t rx_index = 0;
volatile uint8_t cmd_ready = 0;
uint8_t uart_rx_byte;

void MX_USART2_UART_Init(void) {
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
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK) {
        Error_Handler();
    }
    // Interrupt modunda dinlemeyi başlat:
        HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1); // <-- BU SATIRI EKLEYİN
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        if (uart_rx_byte == '\r' || uart_rx_byte == '\n') {
            if (rx_index > 0) {
                rx_buffer[rx_index] = '\0';
                cmd_ready = 1;
                rx_index = 0;
            }
        } else if (rx_index < RX_BUFFER_SIZE - 1) {
            rx_buffer[rx_index++] = uart_rx_byte;
        }
        HAL_UART_Receive_IT(&huart2, &uart_rx_byte, 1);
    }
}
