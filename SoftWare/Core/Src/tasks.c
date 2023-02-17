#include "tasks.h"

/// @brief 让电机开环旋转
/// @param htim 对应电机
/// @param Iq_ref 要求的Q轴值
/// @param Id_ref 要求的D轴值
/// @param Speed 角速度
/// @param Udc_ref 输入的电压值
/// @return
err Foc_Open_Loop(TIM_HandleTypeDef *htim, int16_t Iq_ref, int16_t Id_ref, int16_t Speed, int16_t Udc_ref)
{
    //初始化
    StepTyp Opcof;
    err Error;
    Extuo Lope;
    Opcof.zeta += Speed;
    if (Opcof.zeta >= 65535)
    {
        Opcof.zeta = 0;
    }

    Opcof.Udc = Udc_ref;
    Opcof.Ud_ref = Id_ref;
    Opcof.Uq_ref = Iq_ref;

    while (1)
    {
        Lope = Foc_Step(Opcof);
        Timer_DutyExchange(&htim, Lope);
        //错误判断
        if (Lope.Sector >= 7 || Lope.Sector <= 0)
        {
            Error = 1;
            break;
        }
    }
    return Error;
}

