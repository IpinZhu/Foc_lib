#ifndef __TASKS_H_
#define __TASKS_H_

#include "foc.h"
#include "svpwm.h"
#include "main.h"
#include "tim.h"
#include "adc.h"

/// @brief 定义任务断言
typedef int8_t err;

typedef struct
{
    int16_t value;
    int16_t error;
    int16_t ki;
    int16_t kp;
    int32_t errSum;
    int16_t result;
} PI_Type;

typedef struct
{
    ab_t Iaib;
    qd_t Iqid;
    qd_t Uqud;
    alphabeta_t Ialphabeta;
    alphabeta_t Ualphabeta;

    int16_t Omegac;
    uint16_t Udc;
    uint16_t Tpwm;

    int16_t ess; // 误差
    int16_t KP;
    int16_t KI;
    int16_t A;
    int16_t B;

} PMSM_Handle;

err Foc_Open_Loop(TIM_HandleTypeDef *htim, int16_t Iq_ref, int16_t Id_ref, int16_t Speed, int16_t Udc_ref);

#endif // !__TASKS_H_