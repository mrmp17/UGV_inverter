//
// Created by matej on 22/07/2020.
//

#ifndef INVERTER_4CH_INPUT_PWM_H
#define INVERTER_4CH_INPUT_PWM_H


//#include <stm32f4xx_hal_tim.h>
#include "tim.h"

#define TIMER_USED &htim13
#define PWM1 0
#define PWM2 1

extern "C" {
  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
  void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
};


class input_PWM {
public:
    input_PWM();

    void begin();
    uint16_t get_pulse(uint8_t channel);


    uint16_t pwm_width [2] = {0}; //array for storing pwm width
    uint16_t pwm_rising_time [2] = {0}; //array for storing rising edge timer count
    uint16_t pwm_falling_time [2] = {0}; //array for storing rising edge timer count
private:





};

extern input_PWM input;

#endif //INVERTER_4CH_INPUT_PWM_H
