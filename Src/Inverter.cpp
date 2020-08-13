//
// Created by matej on 19/07/2020.
//

#include "Inverter.h"

Inverter inverter;


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
  HAL_ADC_Start_DMA(ADC1_HANDLE, ADC1_buffer, ADC1_CH_NUM); //start adc in DMA mode, cont conversion
  HAL_ADC_Start_DMA(ADC2_HANDLE, ADC2_buffer, ADC2_CH_NUM); //start adc in DMA mode, cont conversion
  HAL_Delay(1); //wait for a few adc conversions
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

int32_t Inverter::motor_rpm(uint8_t channel) {
  return (60*INTERRUPTS_PER_SEC)/(MOTOR_TICKS_PER_REV*vel_dts_per_tick[channel]);
}

float Inverter::motor_vel(uint8_t channel) {
  return (float)(motor_rpm(channel)*MOTOR_CIRC)/60;
}


void Inverter::interrupt_handler() {
  static uint8_t hall_pos;
  static uint8_t commutation_step;
  static int16_t current;
  static uint16_t pwm_lower;
  static int32_t prev_pwm_dir_combined [4] = {0}; //previous pwm/dir command. This is before current limit!
  static uint8_t dt_counter [4] = {0,2,4,6}; //counts to PWM_RAMP_dt - dt takes a number of interrupt cycles. staged a few cycles (for shorter intterupr)
  static uint16_t actual_pwm [4] = {0};
  static bool actual_dir [4] = {0};
  //HAL_GPIO_WritePin(GPIO_OUT_1_GPIO_Port, GPIO_OUT_1_Pin, GPIO_PIN_SET);
  for (uint8_t i = CH1; i <= CH4; ++i) {  //do the same for all motor channels
    static uint8_t raw_hall_pos;
    read_hall(i, raw_hall_pos); //read hall position (even if disabled, to have encoder always working
    if(raw_hall_pos > 1 && raw_hall_pos < 7 && 1){ //valid numbers only 1,2,3,4,5,6
      hall_pos = raw_hall_pos;
    }
    //else: if non valid hall position is read, hall_pos remains at the previous value
    //this avoids invalid position glitch between correct positions
    if (enable_cmd_list[i]){ //if motor enabled

      //pwm change ramping: limits dPWM/dt to protect the motor from current spikes.
      //this happens BEFORE current limiting. Current limit must be immediate, not ramped!!
      //monitors direction change too - prevents direction change at high pwms
      if(dt_counter[i] < PWM_RAMP_dt){
        dt_counter[i]++; //increment dt counter if not yet at threshold
      }
      else{ //this is a dt event! pwm and direction is checked and modified here
        static int32_t pwm_dir_combined; //combined pwm and direction in a single signed variable
        static int32_t pwm_dir_comb_mod; //modified pwm/dir combined (max allowable change)

        if(dir_cmd_list[i]) pwm_dir_combined = pwm_cmd_list[i];
        else pwm_dir_combined = -pwm_cmd_list[i];
        pwm_commanded[i] = pwm_dir_combined; //TEST
        diff[i] = abs(pwm_dir_combined - prev_pwm_dir_combined[i]); //TEST
        if(abs(pwm_dir_combined - prev_pwm_dir_combined[i]) <= MAX_dPWMdt){ //small, allowable change in pwm command
          actual_pwm[i] = pwm_cmd_list[i]; // no changes needed for pwm/dir commands
          actual_dir[i] = dir_cmd_list[i];
          prev_pwm_dir_combined[i] = pwm_dir_combined; //save as previous value to use in next dt cycle
          pwm_set[i] = pwm_dir_combined; //TEST
        }
        else{ //pwm/dir change too big, reduction needed

          if(pwm_dir_combined > prev_pwm_dir_combined[i]){
            pwm_dir_comb_mod = prev_pwm_dir_combined[i]+MAX_dPWMdt;
          }
          else{
            pwm_dir_comb_mod = prev_pwm_dir_combined[i]-MAX_dPWMdt;
          }
          //convert combined value back into pwm and dir
          if(pwm_dir_comb_mod >= 0){
            actual_pwm[i] = pwm_dir_comb_mod;
            actual_dir[i] = true;
          }
          else{
            actual_pwm[i] = -pwm_dir_comb_mod;
            actual_dir[i] = false;
          }
          prev_pwm_dir_combined[i] = pwm_dir_comb_mod; //save as previous value to use in next dt cycle
          pwm_set[i] = pwm_dir_comb_mod; //TEST
        }
        dt_counter[i] = 0;
      }



      //current limiting
      current = get_current(i);
      if(current > MAX_CHANNEL_CURRENT){
        pwm_lower = (uint16_t)((float)(current - MAX_CHANNEL_CURRENT) * CURRENT_LIMIT_KP);
        if(pwm_lower > actual_pwm[i]) actual_pwm[i] = 0;
        actual_pwm[i] = actual_pwm[i]- pwm_lower;
      }

      //commutation
      commutation_step = hall_mapping[i][hall_pos];
      set_commutation_step(commutation_step, i, actual_dir[i], actual_pwm[i]); //set commutation according to hall position and commands
    }
    else{ //set all phases to float if motor disabled
      set_float(i, PH_U);
      set_float(i, PH_U);
      set_float(i, PH_U);
    }

    //encoder (runs even if motor is not enabled)
    //commutation steps should be going in order, using this for step counting (commutation_step)
    //also includes velocity calculations
    static uint8_t prevStep [4] = {0};
    static bool prevDir [4] = {0}; //true: positive dir, false: negative dir
    static uint32_t directionCount[4] = {0}; //this counts how many steps happened after direction change (capped at 10k to prevent ovrflw)

    if((commutation_step > prevStep[i] || (commutation_step == 0 && prevStep[i] == 5)) && !(commutation_step == 5 && prevStep[i] == 0)){ //step in positive direction
      encoder_list[i]++;
      if(prevDir[i]){
        if(directionCount[i] < 10000) directionCount[i]++;
      }
      else{
        directionCount[i] = 0;
      }
      if(directionCount[i] >= MOTOR_DIR_CHG_THR){
        vel_dts_per_tick[i] = vel_dts_list[i];
      }
      vel_dts_list[i] = 0;
      prevDir[i] = true;
    }
    else if(commutation_step < prevStep[i] || (commutation_step == 5 && prevStep[i] == 0)){ //step in negative direction
      encoder_list[i]--;
      if(!prevDir[i]){
        if(directionCount[i] < 10000) directionCount[i]++;
      }
      else{
        directionCount[i] = 0;
      }
      if(directionCount[i] >= MOTOR_DIR_CHG_THR){
        vel_dts_per_tick[i] = -vel_dts_list[i]; //negative dt/tick if going backwards
      }
      vel_dts_list[i] = 0;
      prevDir[i] = false;
    }
    if(vel_dts_list[i] > MOTOR_MAX_DTS_PER_TICK){
      vel_dts_per_tick[i] = 0;
    }
    else{
      vel_dts_list[i]++;
    }

    prevStep[i] = commutation_step;
    vel_dts_list[i]++;





  }

  //HAL_GPIO_WritePin(GPIO_OUT_1_GPIO_Port, GPIO_OUT_1_Pin, GPIO_PIN_RESET);
  //todo: implement encoder counter
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


void Inverter::set_motor_float(uint8_t channel, float throttle) {
  if(throttle < 0){
    set_motor_direction(channel, false);
    throttle = -throttle;
  }
  else if(throttle > 0) set_motor_direction(channel, true);
  if (throttle > 1) throttle = 1;
  else if (throttle < -1) throttle = -1;

  uint16_t pwm_val = mapf(throttle, 0, 1, (float)MIN_PWM_CMD, (float)MAX_PWM_CMD);
  //debug_print("setting to: %d\n", pwm_val);
  set_motor_pwm(channel, pwm_val);
}


void Inverter::set_motor_direction(uint8_t channel, bool dir) {
  dir_cmd_list[channel] = dir;
}


float Inverter::mapf(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint32_t Inverter::get_ADC_voltage(uint8_t adc, uint8_t channel) {
  if(adc == ADC_CONV_1) return (ADC1_buffer[channel]*ADC_REF)/ADC_MAX_VAL; //mV
  else if (adc == ADC_CONV_2) return (ADC2_buffer[channel]*ADC_REF)/ADC_MAX_VAL; //mV
}

int16_t Inverter::get_current(uint8_t channel) {
  return get_channel_current(channel);
}

uint16_t Inverter::battery_voltage() {
    return (uint16_t)((float)get_ADC_voltage(ADC_CONV_1, ADC_VBAT)*ADC_VBAT_COEF);
}

uint32_t Inverter::encoder(uint8_t channel) {
  return encoder_list[channel];
}

void Inverter::reset_encoder(uint8_t channel) {
  encoder_list[channel] = 0;
}


float Inverter::MCU_temp() {
  return ((float)(get_ADC_voltage(ADC_CONV_1, ADC_TEMP) - MCU_TEMP_25_COEF)/MCU_TEMP_SLOPE) + 25;
}







