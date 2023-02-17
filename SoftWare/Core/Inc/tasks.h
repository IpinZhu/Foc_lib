#ifndef __TASKS_H_
#define __TASKS_H_

#include "foc.h"
#include "svpwm.h"
#include "main.h"
#include "tim.h"
#include "adc.h"

/// @brief 定义任务断言，非Rtos使用
typedef int8_t err ;
err Foc_Open_Loop(TIM_HandleTypeDef *htim, int16_t Iq_ref, int16_t Id_ref, int16_t Speed, int16_t Udc_ref);

#endif // !__TASKS_H_