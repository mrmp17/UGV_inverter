//
// Created by matej on 05/07/2020.
//

#include "BLDC_driver.h"





BLDC_driver::BLDC_driver(TIM_HandleTypeDef timer_used) {
  timer_htim = timer_used;
}
