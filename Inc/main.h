/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define M1H1_Pin GPIO_PIN_2
#define M1H1_GPIO_Port GPIOE
#define M1H2_Pin GPIO_PIN_3
#define M1H2_GPIO_Port GPIOE
#define M1H3_Pin GPIO_PIN_4
#define M1H3_GPIO_Port GPIOE
#define PWM_IN_2_Pin GPIO_PIN_5
#define PWM_IN_2_GPIO_Port GPIOE
#define PWM_IN_1_Pin GPIO_PIN_6
#define PWM_IN_1_GPIO_Port GPIOE
#define M2H1_Pin GPIO_PIN_13
#define M2H1_GPIO_Port GPIOC
#define M2H2_Pin GPIO_PIN_14
#define M2H2_GPIO_Port GPIOC
#define M2H3_Pin GPIO_PIN_15
#define M2H3_GPIO_Port GPIOC
#define CS_1_Pin GPIO_PIN_0
#define CS_1_GPIO_Port GPIOC
#define CS_2_Pin GPIO_PIN_1
#define CS_2_GPIO_Port GPIOC
#define CS_3_Pin GPIO_PIN_2
#define CS_3_GPIO_Port GPIOC
#define CS_4_Pin GPIO_PIN_3
#define CS_4_GPIO_Port GPIOC
#define W1_PWM_Pin GPIO_PIN_0
#define W1_PWM_GPIO_Port GPIOA
#define U1_PWM_Pin GPIO_PIN_1
#define U1_PWM_GPIO_Port GPIOA
#define V1_PWM_Pin GPIO_PIN_2
#define V1_PWM_GPIO_Port GPIOA
#define GPIO2_TP_Pin GPIO_PIN_3
#define GPIO2_TP_GPIO_Port GPIOA
#define VBAT_SNS_Pin GPIO_PIN_7
#define VBAT_SNS_GPIO_Port GPIOA
#define W2_PWM_Pin GPIO_PIN_9
#define W2_PWM_GPIO_Port GPIOE
#define V2_PWM_Pin GPIO_PIN_11
#define V2_PWM_GPIO_Port GPIOE
#define GPIO_OUT_3_Pin GPIO_PIN_12
#define GPIO_OUT_3_GPIO_Port GPIOE
#define U2_PWM_Pin GPIO_PIN_13
#define U2_PWM_GPIO_Port GPIOE
#define GPIO_OUT_2_Pin GPIO_PIN_14
#define GPIO_OUT_2_GPIO_Port GPIOE
#define GPIO_OUT_1_Pin GPIO_PIN_15
#define GPIO_OUT_1_GPIO_Port GPIOE
#define U2_EN_Pin GPIO_PIN_12
#define U2_EN_GPIO_Port GPIOB
#define V2_EN_Pin GPIO_PIN_13
#define V2_EN_GPIO_Port GPIOB
#define W2_EN_Pin GPIO_PIN_14
#define W2_EN_GPIO_Port GPIOB
#define U3_EN_Pin GPIO_PIN_15
#define U3_EN_GPIO_Port GPIOB
#define V3_EN_Pin GPIO_PIN_8
#define V3_EN_GPIO_Port GPIOD
#define W3_EN_Pin GPIO_PIN_9
#define W3_EN_GPIO_Port GPIOD
#define W4_PWM_Pin GPIO_PIN_12
#define W4_PWM_GPIO_Port GPIOD
#define V4_PWM_Pin GPIO_PIN_13
#define V4_PWM_GPIO_Port GPIOD
#define U4_PWM_Pin GPIO_PIN_14
#define U4_PWM_GPIO_Port GPIOD
#define W3_PWM_Pin GPIO_PIN_6
#define W3_PWM_GPIO_Port GPIOC
#define V3_PWM_Pin GPIO_PIN_7
#define V3_PWM_GPIO_Port GPIOC
#define U3_PWM_Pin GPIO_PIN_8
#define U3_PWM_GPIO_Port GPIOC
#define LED_EXT_3_Pin GPIO_PIN_9
#define LED_EXT_3_GPIO_Port GPIOA
#define LED_EXT_2_Pin GPIO_PIN_10
#define LED_EXT_2_GPIO_Port GPIOA
#define LED_EXT_1_Pin GPIO_PIN_11
#define LED_EXT_1_GPIO_Port GPIOA
#define GPIO1_TP_Pin GPIO_PIN_12
#define GPIO1_TP_GPIO_Port GPIOA
#define W4_EN_Pin GPIO_PIN_5
#define W4_EN_GPIO_Port GPIOD
#define V4_EN_Pin GPIO_PIN_6
#define V4_EN_GPIO_Port GPIOD
#define U4_EN_Pin GPIO_PIN_7
#define U4_EN_GPIO_Port GPIOD
#define W1_EN_Pin GPIO_PIN_3
#define W1_EN_GPIO_Port GPIOB
#define V1_EN_Pin GPIO_PIN_4
#define V1_EN_GPIO_Port GPIOB
#define U1_EN_Pin GPIO_PIN_5
#define U1_EN_GPIO_Port GPIOB
#define M3H1_Pin GPIO_PIN_6
#define M3H1_GPIO_Port GPIOB
#define M3H2_Pin GPIO_PIN_7
#define M3H2_GPIO_Port GPIOB
#define M3H3_Pin GPIO_PIN_8
#define M3H3_GPIO_Port GPIOB
#define M4H1_Pin GPIO_PIN_9
#define M4H1_GPIO_Port GPIOB
#define M4H2_Pin GPIO_PIN_0
#define M4H2_GPIO_Port GPIOE
#define M4H3_Pin GPIO_PIN_1
#define M4H3_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
