/*
 * buzzer.c
 *
 *  Created on: Apr 3, 2025
 *      Author: HOME
 */


#include "buzzer.h"
#include "tim.h"

void Buzzer_PlayAlert()
{
//    __HAL_TIM_SET_AUTORELOAD(&htimY, 4000 - 1); // Period (based on clock)
//    __HAL_TIM_SET_COMPARE(&htimY, TIM_CHANNEL_1, 2000); // 50% duty
//    HAL_TIM_PWM_Start(&htimY, TIM_CHANNEL_1);
}

void Buzzer_Stop()
{
//    HAL_TIM_PWM_Stop(&htimY, TIM_CHANNEL_1);
}
