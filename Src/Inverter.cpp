//
// Created by matej on 19/07/2020.
//

#include "Inverter.h"


Inverter::Inverter() {

}



void Inverter::begin() {
  //start all pwm channels (set to 0 by default)
  for (uint8_t i = 0; i < 4; ++i) { //goes through all motor channels
    for (uint8_t j = 0; j < 3; ++j) { //goes to all phases for given channel
      HAL_TIM_PWM_Start(htim_list[i], tim_ch_list[i][j]);
      set_float(i,j); //set float, to be sure
    }
  }
  //todo: init analog stuff
}

void Inverter::test() {
  bool direction = false;
  uint16_t pwm = 1000;

  static uint8_t halpos = 0;
  static uint8_t oldpos = 0;
  read_hall(CH2, halpos);
  if(halpos != oldpos){
    debug_print("%d\n", halpos);
  }
  oldpos = halpos;
  //set_commutation_step(0, CH2, direction, pwm)

  set_commutation_step(hall_mapping[CH2][halpos], CH2, direction, pwm);

//  int n = 0;
//  while(1){
//    n++;
//    for(uint8_t i = 0; i<6; i++){
//      set_commutation_step(i, CH2, direction, pwm);
//      HAL_Delay(50);
//    }
//  }



}

//this function needs pointer to an array with size of 6 bytes!! (currently only returns array of correct mapping)
bool Inverter::hall_auto_map(uint8_t motor_ch, uint8_t *array_ptr) {
  uint16_t map_pwm = 300; //raw timer pwm value with which to drive motor during mapping
  uint8_t pos = 0;
  uint8_t oldPos = 0;

  for (uint8_t j = 0; j < 6; ++j) {
    set_commutation_step(j, motor_ch, 1, map_pwm); //set commutation to fixed step
    //debug_print("%d", j);
    HAL_Delay(30);
  }

  for (uint8_t i = 0; i < 6; ++i) {
    set_commutation_step(i, motor_ch, 1, map_pwm); //set commutation to fixed step
    HAL_Delay(15);
    read_hall(motor_ch, pos);
    debug_print("got hall pos: %d at commutation: %d \n", pos, i);
    *(array_ptr+pos)= i;
    HAL_Delay(15);

  }
}


void Inverter::interrupt_handler() {
  static uint8_t hall_pos;
   HAL_GPIO_WritePin(GPIO1_TP_GPIO_Port, GPIO1_TP_Pin, GPIO_PIN_SET);
  for (uint8_t i = CH1; i <= CH4; ++i) {  //do the same for all motor channels
    if (enable_cmd_list[i]){ //if motor enabled
      read_hall(i, hall_pos); //read hall position
      set_commutation_step(hall_mapping[i][hall_pos], i, dir_cmd_list[i], pwm_cmd_list[i]); //set commutation according to hall position and commands
    }
    else{ //set all phases to float if motor disabled
      set_float(i, PH_U);
      set_float(i, PH_U);
      set_float(i, PH_U);
    }
  }
  HAL_GPIO_WritePin(GPIO1_TP_GPIO_Port, GPIO1_TP_Pin, GPIO_PIN_RESET);
  //todo: implement encoder counter
  //todo: analog stuff
}

bool Inverter::set_motor_pwm(uint8_t channel, uint16_t pwm) { //call this with CHx defines
  if(pwm >= MIN_PWM_CMD && pwm <= MAX_PWM_CMD){
    pwm_cmd_list[channel] = pwm;
    return true;
  }
  else{
    return false;
  }
}


void Inverter::enable_motor(uint8_t channel) {
  pwm_cmd_list[channel] = 0; //set pwm to 0 before enabling motor
  enable_cmd_list[channel] = true; //enable motor
}

void Inverter::disable_motor(uint8_t channel) {
  enable_cmd_list[channel] = false; //disable motor
  pwm_cmd_list[channel] = 0; //set pwm to 0 after disabling motor
}














