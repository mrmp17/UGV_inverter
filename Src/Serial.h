//
// Created by matej on 21/07/2020.
//

#ifndef INVERTER_4CH_SERIAL_H
#define INVERTER_4CH_SERIAL_H






#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "stm32f4xx_hal_gpio.h"

extern DMA_HandleTypeDef hdma_usart3_rx;
extern UART_HandleTypeDef huart3;



#define DMA_BUFF_LEN 256
#define SERIAL_BUFF_LEN 512

extern  "C" {
void USART3_IRQHandler(void);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
}

class Serial {
public:
    Serial(UART_HandleTypeDef *uartHandle, DMA_HandleTypeDef *dmaHandle);
    void begin();
    bool available();
    int8_t read();
    int8_t write(uint8_t singleByte);
    int8_t write(uint8_t buffer[], uint16_t len);
    void ILHandler();
    uint16_t getErrorCount();
    void clearError();  //counter for serial error events (calling write too soon, buffer full, HW error)
    bool txOngoing = false;
private:
    void cpyToBuffer (int16_t);
    UART_HandleTypeDef *uartHandle;
    DMA_HandleTypeDef *dmaHandle;
    uint8_t DMABuffer [DMA_BUFF_LEN];
    uint8_t serialBuffer [SERIAL_BUFF_LEN];
    uint16_t head = 1;
    uint16_t tail = 0;
    uint16_t errorCount = 0;
    bool serialBufferFull = false;
    uint8_t txBuffer [SERIAL_BUFF_LEN];

};

extern Serial serial_01;




#endif //INVERTER_4CH_SERIAL_H
