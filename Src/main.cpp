/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Inverter.h"
#include "Serial.h"
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "input_PWM.h"

#include "simple_serial_arduino/src/SimpleSerial.h"
#include "simple_serial_arduino/src/transmission_test.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
SimpleSerial sser(&serial_01, 40, 8);
enum class SS_ID : uint8_t {
    STATUS = 10,
    MOTOR_PWR = 20,
    MOVE = 22,
    MOVE_TANK = 23
};
uint32_t status_send_interval = 500; // ms
uint32_t last_status_send = 0;
uint32_t move_rec_interval = 1000; // ms
uint32_t last_move_rec = 0;

enum class ControlMode : uint8_t {rc, serial};
ControlMode control_mode = ControlMode::serial;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void debug_print(const char *format, ...){
  return;
  char formatedString[128] = {0};
  va_list arglist;
  va_start(arglist, format);
  int len = vsnprintf(formatedString, sizeof(formatedString), format, arglist);
  uint16_t n = 0;
  //comment out this block if carriage return not needed (change serial_01.write len!!!)
  while(formatedString[n] != 0) n++;
  formatedString[n] = 0xD;
  //####
  va_end(arglist);
  while(serial_01.txOngoing);
  serial_01.write(reinterpret_cast<uint8_t*>(formatedString), len+1);
}


/**
  * @brief This function handles TIM8 trigger and commutation interrupts and TIM14 global interrupt.
  */
void TIM8_TRG_COM_TIM14_IRQHandler(void){
  /* USER CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 0 */
  inverter.interrupt_handler();
  /* USER CODE END TIM8_TRG_COM_TIM14_IRQn 0 */
  HAL_TIM_IRQHandler(&htim14);
  /* USER CODE BEGIN TIM8_TRG_COM_TIM14_IRQn 1 */

  /* USER CODE END TIM8_TRG_COM_TIM14_IRQn 1 */
}

void diff_steer(float trans, float rot, float &left, float &right) {
  float sum = abs(trans) + abs(rot);
  float scale;
  if(sum > 1.) scale = 1. - (sum - 1.) / 2.;
  else scale = 1.;
  left = scale * (trans - rot);
  right = scale * (trans + rot);
}

void move(float trans, float rot) {
    float left;
    float right;
    diff_steer(trans, rot, left, right);
    inverter.set_motor_float(CH1, left);
    inverter.set_motor_float(CH2, left);
    inverter.set_motor_float(CH3, -right);
    inverter.set_motor_float(CH4, -right);
}

void move_tank(float left, float right) {
    inverter.set_motor_float(CH1, left);
    inverter.set_motor_float(CH2, left);
    inverter.set_motor_float(CH3, -right);
    inverter.set_motor_float(CH4, -right);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_I2C3_Init();
  MX_TIM14_Init();
  MX_USART3_UART_Init();
  MX_TIM13_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim14); //start Inverter interrupt timer

  serial_01.begin();
  inverter.begin();
  input.begin();
  HAL_Delay(3500);
  HAL_GPIO_TogglePin(GPIO1_TP_GPIO_Port, GPIO1_TP_Pin);

  inverter.enable_motor(CH1);
  inverter.enable_motor(CH2);
  inverter.enable_motor(CH3);
  inverter.enable_motor(CH4);
  debug_print("motors active...\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {

    if (control_mode == ControlMode::rc) {
        float thr = input.skidSteer_throttle();
        float str = input.skidSteer_steer();
        move(thr, str);
    }

//    int16_t thrli = thr_left*1000;
//    int16_t thrri = thr_right*1000;
//    debug_print("left: %d, right: %d\n", thrli, thrri);
//
//    debug_print("RPM_1: %d\n", inverter.motor_rpm(CH1));
//    debug_print("SPD_1: %d\n", (int32_t)(inverter.motor_vel(CH1)*10));
//
//    debug_print("batvolt: %d\n", inverter.battery_voltage());
//
//    debug_print("mcu temp: %d\n", (int32_t)inverter.MCU_temp());
//
//    battery current output on serial
//    debug_print("batt cur: %d\n", inverter.get_current(CH1)+inverter.get_current(CH2)+inverter.get_current(CH3)+inverter.get_current(CH4));
//
//    debug_print("rpm CH4: %d\n", inverter.motor_rpm(CH4));
//    debug_print("enc CH4: %d\n", inverter.encoder(CH4));
//
//
//    ####### slip control
//
//    CH1,CH3: front, CH2,CH4 back
//    float slipCtrlKp = 0.001;
//    float lf = thr_left;
//    float lb = thr_left;
//    float rf = thr_right;
//    float rb = thr_right;
//    int32_t dif_left = inverter.motor_rpm(CH1)-inverter.motor_rpm(CH2);
//    int32_t dif_right = inverter.motor_rpm(CH3)-inverter.motor_rpm(CH4);
//    if(abs(dif_left)>10){
//      if(dif_left > 0){ //CH1 faster
//        if(lf>0) lf  = thr_left - abs(dif_left)*slipCtrlKp;
//        else lf  = thr_left + abs(dif_left)*slipCtrlKp;
//
//      }
//      else{ //CH2 faster
//        if(lb>0) lb = thr_left - abs(dif_left)*slipCtrlKp;
//        else lb = thr_left + abs(dif_left)*slipCtrlKp;
//
//      }
//    }
//    if(abs(dif_right)>10){
//      if(dif_right > 0){ //CH1 faster
//        if(rf>0) rf  = thr_right - abs(dif_right)*slipCtrlKp;
//        else rf  = thr_right + abs(dif_right)*slipCtrlKp;
//
//      }
//      else{ //CH2 faster
//        if(rb>0) rb = thr_right - abs(dif_right)*slipCtrlKp;
//        else rb = thr_right + abs(dif_right)*slipCtrlKp;
//
//      }
//    }
//
//
//
//    inverter.set_motor_float(CH1, 0.1);
//    inverter.set_motor_float(CH2, 0.1);
//    inverter.set_motor_float(CH3, -0.1);
//    inverter.set_motor_float(CH4, -0.1);

    //uint32_t adcval = inverter.get_ADC_voltage(ADC_CONV_1, ADC_VBAT)*(float)ADC_VBAT_COEF;
    //debug_print("bat adc voltage: %d\n", adcval);

    //uint32_t csval = inverter.get_ADC_voltage(ADC_CONV_2, ADC_CS1);
    //debug_print("cs1 adc voltage: %d\n", csval);



//    HAL_Delay(20);



//    int16_t thrf = (float)thr*1000;
//    int16_t strf = (float)str*1000;
//    debug_print("thr: %d, str: %d\n", thrf, strf);

//    uint16_t val = 0;
//    val = input.get_pulse(PWM1);
//    bool pwm_det = input.pwm_recvd[PWM1];
//    bool flsf = input.is_failsafe(PWM1);
//    debug_print("PWM_1 input val: %d, is detected?: %d, flsf: %d\n", val, pwm_det, flsf);
//
//    val = input.get_pulse(PWM2);
//    pwm_det = input.pwm_recvd[PWM2];
//    flsf = input.is_failsafe(PWM2);
//    debug_print("PWM_2 input val: %d, is detected?: %d, flsf: %d\n\n", val, pwm_det, flsf);
//    HAL_Delay(50);



//    uint16_t t1 = __HAL_TIM_GET_COUNTER(&htim13);
//    HAL_Delay(40);
//    uint16_t t2 = __HAL_TIM_GET_COUNTER(&htim13);
//    uint16_t time = t2-t1;





//    uint8_t result[7] = {0};
//    debug_print("new test: ###\n");
//    Inverter.hall_auto_map(CH2, result);
//    debug_print("commutation array: [%d,%d,%d,%d,%d,%d,%d]\n", result[0],result[1],result[2],result[3],result[4],result[5],result[6]);
//    HAL_Delay(5000);


    // Simple serial command loop
    sser.loop();
    if (sser.available()) {
        SimpleSerial::Packet packet = sser.read();
        switch (static_cast<SS_ID >(packet.id)) {
            case SS_ID::MOTOR_PWR: {
                int16_t pwr = byte_conversion::bytes_2_int(packet.payload);
                if (pwr > 0) {
                    inverter.enable_motors();
                }
                else {
                    inverter.disable_motors();
                }}
                break;
            case SS_ID::MOVE: {
                float trans = byte_conversion::bytes_2_float(packet.payload);
                float rot = byte_conversion::bytes_2_float(packet.payload + 4);
                if (control_mode == ControlMode::serial) {
                    last_move_rec = HAL_GetTick();
                    move(trans, rot);
                }}
                break;
            case SS_ID::MOVE_TANK: {
                float left = byte_conversion::bytes_2_float(packet.payload);
                float right = byte_conversion::bytes_2_float(packet.payload + 4);
                if (control_mode == ControlMode::serial) {
                    last_move_rec = HAL_GetTick();
                    move_tank(left, right);
                }}
                break;
            default:
                break;
        }
    }

    // Check last received move time
    if (HAL_GetTick() - last_move_rec > move_rec_interval) {
        move(0, 0);
    }

    // Send status
    if (HAL_GetTick() - last_status_send > status_send_interval) {
        uint8_t payload[37];
        uint8_t bts[4];

        byte_conversion::float_2_bytes(inverter.motor_vel(CH1), bts);
        memcpy(payload, bts, 4);

        byte_conversion::float_2_bytes(inverter.motor_vel(CH2), bts);
        memcpy(payload+4, bts, 4);

        byte_conversion::float_2_bytes(inverter.motor_vel(CH3), bts);
        memcpy(payload+8, bts, 4);

        byte_conversion::float_2_bytes(inverter.motor_vel(CH4), bts);
        memcpy(payload+12, bts, 4);

        byte_conversion::float_2_bytes(inverter.get_current(CH1), bts);
        memcpy(payload+16, bts, 4);

        byte_conversion::float_2_bytes(inverter.get_current(CH2), bts);
        memcpy(payload+20, bts, 4);

        byte_conversion::float_2_bytes(inverter.get_current(CH3), bts);
        memcpy(payload+24, bts, 4);

        byte_conversion::float_2_bytes(inverter.get_current(CH4), bts);
        memcpy(payload+28, bts, 4);

        byte_conversion::float_2_bytes(inverter.battery_voltage(), bts);
        memcpy(payload+32, bts, 4);

        payload[36] = static_cast<uint8_t>(inverter.motors_enabled());

        sser.send(static_cast<uint8_t>(SS_ID::STATUS), 37, payload);
        last_status_send = HAL_GetTick();
    }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 128;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
