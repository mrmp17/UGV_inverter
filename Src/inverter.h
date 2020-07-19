//
// Created by matej on 19/07/2020.
//

#ifndef INVERTER_4CH_INVERTER_H
#define INVERTER_4CH_INVERTER_H


#include <stm32f4xx_hal_tim.h>
#include "tim.h"
#include "gpio.h"

class inverter {

public:
    inverter();


private:



    //tim2: ch1
    //tim1: ch2
    //tim3: ch3
    //tim4: ch4

    //ch1: 2,3,1 (U;V;W)
    //ch2: 3,2,1
    //ch3: 3,2,1
    //ch4: 3,2,1

    //timer, timer_ch, en_port, en_pin arrays
    const TIM_HandleTypeDef htim_list [4] = {htim2, htim1, htim3, htim4}; //timer handlers
    const uint16_t timch_U [4] = {TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_3, TIM_CHANNEL_3}; //phase U timer channels
    const uint16_t timch_V [4] = {TIM_CHANNEL_3, TIM_CHANNEL_2, TIM_CHANNEL_2, TIM_CHANNEL_2}; //phase V timer channels
    const uint16_t timch_W [4] = {TIM_CHANNEL_1, TIM_CHANNEL_1, TIM_CHANNEL_1, TIM_CHANNEL_1}; //phase W timer channels
    const GPIO_TypeDef* enport_U [4] = {U1_EN_GPIO_Port, U2_EN_GPIO_Port, U3_EN_GPIO_Port, U4_EN_GPIO_Port}; //phase U enable ports
    const GPIO_TypeDef* enport_V [4] = {V1_EN_GPIO_Port, V2_EN_GPIO_Port, V3_EN_GPIO_Port, V4_EN_GPIO_Port}; //phase V enable ports
    const GPIO_TypeDef* enport_W [4] = {W1_EN_GPIO_Port, W2_EN_GPIO_Port, W3_EN_GPIO_Port, W4_EN_GPIO_Port}; //phase W enable ports
    const uint16_t enpin_U [4] = {U1_EN_Pin, U2_EN_Pin, U3_EN_Pin, U4_EN_Pin}; //phase U enable pins
    const uint16_t enpin_V [4] = {V1_EN_Pin, V2_EN_Pin, V3_EN_Pin, V4_EN_Pin}; //phase V enable pins
    const uint16_t enpin_W [4] = {W1_EN_Pin, W2_EN_Pin, W3_EN_Pin, W4_EN_Pin}; //phase U enable pins


};


#endif //INVERTER_4CH_INVERTER_H
