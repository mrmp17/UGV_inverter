//
// Created by matej on 22/07/2020.
//

#include "input_PWM.h"

input_PWM input;


input_PWM::input_PWM() {
}

uint16_t input_PWM::get_pulse(uint8_t channel) {
  //return pwm value only if pwm is detected and measured width is valid
  if (pwm_width[channel] >= MIN_PWM && pwm_width[channel] <= MAX_PWM && pwm_recvd[channel]){
    return pwm_width[channel];
  }
  else return 0; //return 0 if something's wrong with pwm
}

bool input_PWM::is_failsafe(uint8_t channel) {
  return pwm_width[channel]<FAILSAFE_THR;
}

float input_PWM::mapf(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


float input_PWM::skidSteer_throttle() {
  uint16_t pwm = get_pulse(PWM1);
  float temp = mapf((float)pwm, PWM_L, PWM_H, -1, 1); //return throttle mapped to 0-1
  int16_t tempf = temp*1000;
  //debug_print("got pwm_thr: %d, to: %d\n", pwm, tempf);
  if(is_failsafe(PWM1) || !pwm_recvd[PWM1]) return 0;
  if(temp > 1) return 1;
  if(temp < -1) return -1;
  return temp;
}

float input_PWM::skidSteer_steer() {
  uint16_t pwm = get_pulse(PWM2);
  float temp = mapf((float)pwm, PWM_L, PWM_H, -1, 1); //return steer mapped to 0-1
  int16_t tempf = temp*1000;
  //debug_print("got pwm_str: %d, to: %d\n", pwm, tempf);
  if(is_failsafe(PWM2) || !pwm_recvd[PWM2]) return 0;
  if(temp > 1) return 1;
  if(temp < -1) return -1;
  return temp;
}




void input_PWM::begin() {
  HAL_TIM_Base_Start_IT(&htim13); //start timer
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin == PWM_IN_1_Pin){
    if(HAL_GPIO_ReadPin(PWM_IN_1_GPIO_Port, PWM_IN_1_Pin)==GPIO_PIN_SET){ //rising edge
      //HAL_GPIO_WritePin(GPIO1_TP_GPIO_Port, GPIO1_TP_Pin, GPIO_PIN_SET);
      input.pwm_rising_time[PWM1] = __HAL_TIM_GET_COUNTER(&htim13);
      input.rise_detected[PWM1] = true;
    }
    if(HAL_GPIO_ReadPin(PWM_IN_1_GPIO_Port, PWM_IN_1_Pin)==GPIO_PIN_RESET){ //falling edge
      //HAL_GPIO_WritePin(GPIO1_TP_GPIO_Port, GPIO1_TP_Pin, GPIO_PIN_RESET);
      input.pwm_falling_time[PWM1] = __HAL_TIM_GET_COUNTER(&htim13);
      input.pwm_width[PWM1] = input.pwm_falling_time[PWM1]-input.pwm_rising_time[PWM1];
    }
  }

  if(GPIO_Pin == PWM_IN_2_Pin){
    if(HAL_GPIO_ReadPin(PWM_IN_2_GPIO_Port, PWM_IN_2_Pin)==GPIO_PIN_SET){ //rising edge
      //HAL_GPIO_WritePin(GPIO1_TP_GPIO_Port, GPIO1_TP_Pin, GPIO_PIN_SET);
      input.pwm_rising_time[PWM2] = __HAL_TIM_GET_COUNTER(&htim13);
      input.rise_detected[PWM2] = true;
    }
    if(HAL_GPIO_ReadPin(PWM_IN_2_GPIO_Port, PWM_IN_2_Pin)==GPIO_PIN_RESET){ //falling edge
      //HAL_GPIO_WritePin(GPIO1_TP_GPIO_Port, GPIO1_TP_Pin, GPIO_PIN_RESET);
      input.pwm_falling_time[PWM2] = __HAL_TIM_GET_COUNTER(&htim13);
      input.pwm_width[PWM2] = input.pwm_falling_time[PWM2]-input.pwm_rising_time[PWM2];
    }
  }

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if(htim == &htim13){ //if pwm timer period elapsed
    input.pwm_recvd[PWM1] = input.rise_detected[PWM1]; //at least one rising edge must be detected during timer period
    input.rise_detected[PWM1] = false;

    input.pwm_recvd[PWM2] = input.rise_detected[PWM2]; //at least one rising edge must be detected during timer period
    input.rise_detected[PWM2] = false;
  }
}