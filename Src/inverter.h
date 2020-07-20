//
// Created by matej on 19/07/2020.
//

#ifndef INVERTER_4CH_INVERTER_H
#define INVERTER_4CH_INVERTER_H


#include "tim.h"
#include "gpio.h"
#include "stm32f4xx_hal_gpio.h"

//use these defines with macros, to define BLDC channel
#define CH1 0
#define CH2 1
#define CH3 2
#define CH4 3
#define PH_U 0
#define PH_V 1
#define PH_W 2

#define WVF_LOW 0
#define WVF_FLT 1
#define WVF_PWM 2

//set enable pin to low and pwm to 0 (low) (floats phase)
#define set_float(ch, phase) { \
  HAL_GPIO_WritePin(enport_list[ch][phase], enpin_list[ch][phase], GPIO_PIN_RESET); \
  __HAL_TIM_SET_COMPARE(&htim_list[ch], tim_ch_list[ch][phase], 0); \
}

//set enable pin to high and pwm to 0 (low) (pulls phase down)
#define set_low(ch, phase) { \
  __HAL_TIM_SET_COMPARE(&htim_list[ch], tim_ch_list[ch][phase], 0); \
  HAL_GPIO_WritePin(enport_list[ch][phase], enpin_list[ch][phase], GPIO_PIN_SET); \
}

//set enable pin to high and pwm to #pwm# (pwming) (pulls phase "high" - pwm)
#define set_pwm(ch, phase, pwm_val) { \
  __HAL_TIM_SET_COMPARE(&htim_list[ch], tim_ch_list[ch][phase], pwm_val); \
  HAL_GPIO_WritePin(enport_list[ch][phase], enpin_list[ch][phase], GPIO_PIN_SET); \
}

//macro sets commutation step to selected motor channel with given pwm and direction
#define set_commutation_step(step, ch, dir, pwm_val) { \
  switch (phase_wvf_U[dir][step]){ \
    case WVF_FLT: \
      set_float(ch, PH_U); \
    break; \
    case WVF_LOW: \
      set_low(ch, PH_U); \
    break; \
    case WVF_PWM: \
      set_pwm(ch, PH_U, pwm_val); \
    break; \
  } \
  switch (phase_wvf_V[dir][step]){ \
    case WVF_FLT: \
      set_float(ch, PH_V); \
    break; \
    case WVF_LOW: \
      set_low(ch, PH_V); \
    break; \
    case WVF_PWM: \
      set_pwm(ch, PH_V, pwm_val); \
    break; \
  } \
  switch (phase_wvf_W[dir][step]){ \
    case WVF_FLT: \
      set_float(ch, PH_W); \
    break; \
    case WVF_LOW: \
      set_low(ch, PH_W); \
    break; \
    case WVF_PWM: \
      set_pwm(ch, PH_W, pwm_val); \
    break; \
  } \
}




//tim2: ch1
//tim1: ch2
//tim3: ch3
//tim4: ch4

//ch1: 2,3,1 (U;V;W)
//ch2: 3,2,1
//ch3: 3,2,1
//ch4: 3,2,1

//timer, timer_ch, en_port, en_pin arrays







class inverter {

public:
    inverter();
    void begin();
    void test();


private:


    TIM_HandleTypeDef htim_list [4] = {htim2, htim1, htim3, htim4}; //timer handlers

    uint16_t tim_ch_list [4][3] = {{TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_1}, //timer channels for BLDC channels and phases
                                   {TIM_CHANNEL_3, TIM_CHANNEL_2, TIM_CHANNEL_1},
                                   {TIM_CHANNEL_3, TIM_CHANNEL_2, TIM_CHANNEL_1},
                                   {TIM_CHANNEL_3, TIM_CHANNEL_2, TIM_CHANNEL_1}}; //outer: BLDC channel (1;2;3;4), Inner: phase (U;V;W)

    GPIO_TypeDef* enport_list [4][3] = {{U1_EN_GPIO_Port, V1_EN_GPIO_Port, W1_EN_GPIO_Port}, //enable ports for BLDC channels and phases
                                        {U2_EN_GPIO_Port, V2_EN_GPIO_Port, W2_EN_GPIO_Port},
                                        {U3_EN_GPIO_Port, V3_EN_GPIO_Port, W3_EN_GPIO_Port},
                                        {U4_EN_GPIO_Port, V4_EN_GPIO_Port, W4_EN_GPIO_Port} }; //outer: BLDC channel (1;2;3;4), Inner: phase (U;V;W)

    uint16_t enpin_list [4][3] = {{U1_EN_Pin, V1_EN_Pin, W1_EN_Pin}, //enable pins for BLDC channels and phases
                                  {U2_EN_Pin, V2_EN_Pin, W2_EN_Pin},
                                  {U3_EN_Pin, V3_EN_Pin, W3_EN_Pin},
                                  {U4_EN_Pin, V4_EN_Pin, W4_EN_Pin}}; //outer: BLDC channel (1;2;3;4), Inner: phase (U;V;W)

    GPIO_TypeDef* hallport_list [4][3] = {{M1H1_GPIO_Port, M1H2_GPIO_Port, M1H3_GPIO_Port}, //enable ports for BLDC channels and halls
                                          {M2H1_GPIO_Port, M2H2_GPIO_Port, M2H3_GPIO_Port},
                                          {M3H1_GPIO_Port, M3H2_GPIO_Port, M3H3_GPIO_Port},
                                          {M4H1_GPIO_Port, M4H2_GPIO_Port, M4H3_GPIO_Port} }; //outer: BLDC channel (1;2;3;4), Inner: halls (1;2;3)

    uint16_t hallpin_list [4][3] = {{M1H1_Pin, M1H2_Pin, M1H3_Pin}, //enable pins for BLDC channels and halls
                                    {M2H1_Pin, M2H2_Pin, M2H3_Pin},
                                    {M3H1_Pin, M3H2_Pin, M3H3_Pin},
                                    {M4H1_Pin, M4H2_Pin, M4H3_Pin}}; //outer: BLDC channel (1;2;3;4), Inner: phase (1;2;3)

    //use defines WVF_LOW ,WVF_PWM, WVF_FLT to access these arrays
    uint8_t phase_wvf_U [2][6] = {{WVF_PWM, WVF_PWM, WVF_FLT, WVF_LOW, WVF_LOW, WVF_FLT},{WVF_LOW, WVF_LOW, WVF_FLT, WVF_PWM, WVF_PWM, WVF_FLT}}; //defines commutation waveform. contains 6 commutation steps for forward/reverse direction (phase U)
    uint8_t phase_wvf_V [2][6] = {{WVF_LOW, WVF_FLT, WVF_PWM, WVF_PWM, WVF_FLT, WVF_LOW},{WVF_PWM, WVF_FLT, WVF_LOW, WVF_LOW, WVF_FLT, WVF_PWM}}; //defines commutation waveform. contains 6 commutation steps for forward/reverse direction (phase V)
    uint8_t phase_wvf_W [2][6] = {{WVF_FLT, WVF_LOW, WVF_LOW, WVF_FLT, WVF_PWM, WVF_PWM},{WVF_FLT, WVF_PWM, WVF_PWM, WVF_FLT, WVF_LOW, WVF_LOW}}; //defines commutation waveform. contains 6 commutation steps for forward/reverse direction (phase W)







};


#endif //INVERTER_4CH_INVERTER_H
