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

#define MIN_PWM 950
#define MAX_PWM 2050
#define PMM_MID 1500
#define FAILSAFE_THR 1000
#define PWM_H 1950
#define PWM_L 1050

extern "C" {
  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
  void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
};


class input_PWM {
public:
    input_PWM();

    void begin();
    uint16_t get_pulse(uint8_t channel);
    bool is_failsafe(uint8_t channel);

    float skidSteer_throttle();
    float skidSteer_steer();


    uint16_t pwm_width [2] = {0}; //array for storing pwm width
    uint16_t pwm_rising_time [2] = {0}; //array for storing rising edge timer count
    uint16_t pwm_falling_time [2] = {0}; //array for storing rising edge timer count
    bool rise_detected [2] = {0}; //checked every timer period to see if signal is being received
    bool pwm_recvd [2] = {0}; //true if at least one rising edge detected in each timer period

private:

    float mapf(float x, float in_min, float in_max, float out_min, float out_max);




};

extern input_PWM input;

#endif //INVERTER_4CH_INPUT_PWM_H
