//
// Created by matej on 05/07/2020.
//

#ifndef INVERTER_4CH_BLDC_DRIVER_H
#define INVERTER_4CH_BLDC_DRIVER_H

#include "tim.h"



class BLDC_driver {

public:
    BLDC_driver(TIM_HandleTypeDef timer_used);

private:
    TIM_HandleTypeDef timer_htim;


};


#endif //INVERTER_4CH_BLDC_DRIVER_H
