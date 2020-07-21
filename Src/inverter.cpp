//
// Created by matej on 19/07/2020.
//

#include "inverter.h"





inverter::inverter() {

}


void inverter::begin() {
  //start all pwm channels (set to 0 by default)
  for (uint8_t i = 0; i < 4; ++i) { //goes through all motor channels
    for (uint8_t j = 0; j < 3; ++j) { //goes to all phases for given channel
      HAL_TIM_PWM_Start(&htim_list[i], tim_ch_list[i][j]);
      set_float(i,j); //set float, to be sure
    }
  }
  //todo: init analog stuff
}

void inverter::test() {
  bool direction = false;
  uint16_t pwm = 200;

  int n = 0;
  while(1){
    n++;
    for(uint8_t i = 0; i<6; i++){
      set_commutation_step(i, CH2, direction, pwm);
      HAL_Delay(50);
    }
  }

}

//this function needs pointer to an array with size of 6 bytes!! (currently only returns array of correct mapping)
bool inverter::hall_auto_map(uint8_t motor_ch, uint8_t *array_ptr) {
  uint16_t map_pwm = 200; //raw timer pwm value with which to drive motor during mapping
  uint8_t pos = 0;
  uint8_t oldPos = 0;

  for (uint8_t i = 0; i < 6; ++i) {
    set_commutation_step(i, motor_ch, 0, map_pwm); //set commutation to fixed step
    HAL_Delay(10);
    read_hall(motor_ch, pos);
    oldPos = pos;
    *(array_ptr+i)= pos;
    HAL_Delay(200);

  }
}