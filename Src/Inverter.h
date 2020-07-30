//
// Created by matej on 19/07/2020.
//

#ifndef INVERTER_4CH_INVERTER_H
#define INVERTER_4CH_INVERTER_H


#include "tim.h"
#include "gpio.h"
#include "stm32f4xx_hal_gpio.h"
#include "adc.h"

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

#define ADC1_HANDLE &hadc1
#define ADC2_HANDLE &hadc2
#define ADC1_CH_NUM 2
#define ADC2_CH_NUM 4
#define ADC_REF 3300
#define ADC_MAX_VAL 4095

#define ADC_VBAT 0 //adc1
#define ADC_CS1 0 //adc2
#define ADC_CS2 1 //adc2
#define ADC_CS3 2 //adc2
#define ADC_CS4 3 //adc2
#define ADC_TEMP 2 //adc1

#define ADC_CONV_1 0
#define ADC_CONV_2 1

#define ADC_VBAT_COEF 15.33333333 //volts per adc voltage (divider ratio)
#define ADC_CURRENT_COEF 8.0586086 //mA per adc count
#define ADC_CURRENT_MIDVAL 2048 //todo: tune this value
#define CURRENT_LIMIT_KP 0.006

#define MOTOR_TICKS_PER_REV 90 //encoder ticks per revolution
#define MOTOR_MAX_DTS_PER_TICK 2000 //rpm is 0 if no enocder change detected in 10000 ticks
#define INTERRUPTS_PER_SEC 10000 //number of inverter handler interrupts per second
#define MOTOR_CIRC 0.644 //motor circumfence in m todo: measure exactly

#define MAX_CHANNEL_CURRENT 10000 //mA

#define MCU_TEMP_25_COEF 760 //mV at 25degc
#define MCU_TEMP_SLOPE 2.5 //mV/degc


//#define MAX_PWM_CMD 3800
#define MAX_PWM_CMD 3500 //safety
#define MIN_PWM_CMD 0

//set enable pin to low and pwm to 0 (low) (floats phase)
#define set_float(ch, phase) { \
  HAL_GPIO_WritePin(enport_list[ch][phase], enpin_list[ch][phase], GPIO_PIN_RESET); \
  __HAL_TIM_SET_COMPARE(htim_list[ch], tim_ch_list[ch][phase], 0); \
}

//set enable pin to high and pwm to 0 (low) (pulls phase down)
#define set_low(ch, phase) { \
  __HAL_TIM_SET_COMPARE(htim_list[ch], tim_ch_list[ch][phase], 0); \
  HAL_GPIO_WritePin(enport_list[ch][phase], enpin_list[ch][phase], GPIO_PIN_SET); \
}

//set enable pin to high and pwm to #pwm# (pwming) (pulls phase "high" - pwm)
#define set_pwm(ch, phase, pwm_val) { \
  __HAL_TIM_SET_COMPARE(htim_list[ch], tim_ch_list[ch][phase], pwm_val); \
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

//this macro reads hall signals for specified channel and calculates a position number (which needs to be mapped to commutation waveform step
//values "0" and "7" are non-valid!! todo: implement validity check somewhere
#define read_hall(ch, pos_byte){ \
  pos_byte = 0u; \
  pos_byte = uint8_t(HAL_GPIO_ReadPin(hallport_list[ch][0], hallpin_list[ch][0])== GPIO_PIN_SET); \
  pos_byte |= uint8_t(HAL_GPIO_ReadPin(hallport_list[ch][1], hallpin_list[ch][1])== GPIO_PIN_SET)<<1; \
  pos_byte |= uint8_t(HAL_GPIO_ReadPin(hallport_list[ch][2], hallpin_list[ch][2])== GPIO_PIN_SET)<<2; \
}


#define get_channel_current(ch){ \
  (int16_t)((float)((int16_t)inverter.ADC2_buffer[ch]-ADC_CURRENT_MIDVAL) *ADC_CURRENT_COEF) \
}



class Inverter {

public:
    Inverter();
    void begin();
    void test(); //only for testing
    bool hall_auto_map(uint8_t motor_ch, uint8_t *array_ptr);

    void interrupt_handler(); //call this at 10kHz. Handles fast stuff

    bool set_motor_pwm(uint8_t channel, uint16_t pwm); // 0 to 3800
    void enable_motor(uint8_t channel);
    void disable_motor(uint8_t channel);
    void set_motor_float (uint8_t channel, float throttle); // -1 to 1
    void set_motor_direction (uint8_t channel, bool dir);


    uint32_t get_ADC_voltage (uint8_t adc, uint8_t channel);
    int16_t get_current (uint8_t channel);
    uint16_t battery_voltage();
    uint32_t encoder(uint8_t channel);
    void reset_encoder(uint8_t channel);

    float MCU_temp();

    int32_t motor_rpm(uint8_t channel);
    float motor_vel(uint8_t channel); //motor velocity in m/s



private:


    uint16_t pwm_cmd_list [4] = {0};  //pwm commands for motor channels. call this with CHx defines
    bool dir_cmd_list [4] = {0};  //direction commands for motor channels. call this with CHx defines
    bool enable_cmd_list [4] = {0}; //motor enable command list. call this with CHx defines

    uint32_t ADC1_buffer [2]; //adc1 buffer array (battery voltage and stm32 temperature)
    uint32_t ADC2_buffer [4]; //adc2 buffer array (current sensing for 4 channels)

    int32_t encoder_list [4] = {0}; //encoder array for 4 channels

    uint32_t vel_dts_list[4] = {0}; //internal array for counting dts
    int32_t vel_dts_per_tick[4] = {0}; //current motor velocity in dt/encoderTick








    float mapf(float x, float in_min, float in_max, float out_min, float out_max);


    TIM_HandleTypeDef *htim_list [4] = {&htim2, &htim1, &htim3, &htim4}; //timer handlers

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
                                          {M4H1_GPIO_Port, M4H2_GPIO_Port, M4H3_GPIO_Port} }; //outer: BLDC channel (1;2;3;4), Inner: hall sensor (1;2;3)

    uint16_t hallpin_list [4][3] = {{M1H1_Pin, M1H2_Pin, M1H3_Pin}, //enable pins for BLDC channels and halls
                                    {M2H1_Pin, M2H2_Pin, M2H3_Pin},
                                    {M3H1_Pin, M3H2_Pin, M3H3_Pin},
                                    {M4H1_Pin, M4H2_Pin, M4H3_Pin}}; //outer: BLDC channel (1;2;3;4), Inner: hall sensor (1;2;3)

    //use defines WVF_LOW ,WVF_PWM, WVF_FLT to access these arrays
    uint8_t phase_wvf_U [2][6] = {{WVF_PWM, WVF_PWM, WVF_FLT, WVF_LOW, WVF_LOW, WVF_FLT},{WVF_LOW, WVF_LOW, WVF_FLT, WVF_PWM, WVF_PWM, WVF_FLT}}; //defines commutation waveform. contains 6 commutation steps for forward/reverse direction (phase U)
    uint8_t phase_wvf_V [2][6] = {{WVF_LOW, WVF_FLT, WVF_PWM, WVF_PWM, WVF_FLT, WVF_LOW},{WVF_PWM, WVF_FLT, WVF_LOW, WVF_LOW, WVF_FLT, WVF_PWM}}; //defines commutation waveform. contains 6 commutation steps for forward/reverse direction (phase V)
    uint8_t phase_wvf_W [2][6] = {{WVF_FLT, WVF_LOW, WVF_LOW, WVF_FLT, WVF_PWM, WVF_PWM},{WVF_FLT, WVF_PWM, WVF_PWM, WVF_FLT, WVF_LOW, WVF_LOW}}; //defines commutation waveform. contains 6 commutation steps for forward/reverse direction (phase W)


    //hall mapping for every channel. index: hall position  value at index: corresponding commutation step
    uint8_t hall_mapping [4][7] = {{0,1,3,2,5,0,4},
                                   {0,1,3,2,5,0,4},
                                   {0,1,3,2,5,0,4},
                                   {0,1,3,2,5,0,4}};







};

extern Inverter inverter;



#endif //INVERTER_4CH_INVERTER_H
