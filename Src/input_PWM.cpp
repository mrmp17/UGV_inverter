//
// Created by matej on 22/07/2020.
//

#include "input_PWM.h"

input_PWM input;


input_PWM::input_PWM() {
}

uint16_t input_PWM::get_pulse(uint8_t channel) {
  return pwm_width[channel];
}





void input_PWM::begin() {
  HAL_TIM_Base_Start_IT(&htim13); //start timer
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin == PWM_IN_1_Pin){
    if(HAL_GPIO_ReadPin(PWM_IN_1_GPIO_Port, PWM_IN_1_Pin)==GPIO_PIN_SET){ //rising edge
      //HAL_GPIO_WritePin(GPIO1_TP_GPIO_Port, GPIO1_TP_Pin, GPIO_PIN_SET);
      input.pwm_rising_time[PWM1] = __HAL_TIM_GET_COUNTER(&htim13);
    }
    if(HAL_GPIO_ReadPin(PWM_IN_1_GPIO_Port, PWM_IN_1_Pin)==GPIO_PIN_RESET){ //falling edge
      //HAL_GPIO_WritePin(GPIO1_TP_GPIO_Port, GPIO1_TP_Pin, GPIO_PIN_RESET);
      input.pwm_falling_time[PWM1] = __HAL_TIM_GET_COUNTER(&htim13);
      input.pwm_width[PWM1] = input.pwm_falling_time[PWM1]-input.pwm_rising_time[PWM1];
    }
  }

}