//
// Created by matej on 19/07/2020.
//

#include "inverter.h"





inverter::inverter() {

}


void inverter::begin() {
  //start all pwm channels (set to 0 by default)
  for (int i = 0; i < 4; ++i) { //goes through all motor channels
    for (int j = 0; j < 3; ++j) { //goes to all phases for given channel
      HAL_TIM_PWM_Start(&htim_list[i], tim_ch_list[i][j]);
      set_float(i,j); //set float, to be sure
    }
  }
  //todo: init analog stuff
}

void inverter::test() {

}