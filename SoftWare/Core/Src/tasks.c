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
    // 初始化
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
        // 错误判断
        if (Lope.Sector >= 7 || Lope.Sector <= 0)
        {
            Error = 1;
            break;
        }
    }
    return Error;
}

/// @brief PI控制器函数
/// @param PI_Control  
/// @param target
/// @param resultMax  最大输出限幅 为3分之根号三倍的Udc,表现为最大输出电压
void Curr_PI_Cal(PI_Type *PI_Control, int16_t target, int16_t resultMax)
{
    int32_t ErrSum;
    int32_t Result;

    PI_Control->error = PI_Control->value - target; // 误差计算

    ErrSum += (PI_Control->error * PI_Control->ki) / 32768; // 积分项累加

    if (ErrSum > resultMax)
    { // 积分限幅
        PI_Control->errSum = resultMax;
    }
    else if (ErrSum < (-resultMax))
    {
        PI_Control->errSum = ((int16_t)-resultMax);
    }

    Result = (PI_Control->kp * PI_Control->error) / 32768 + PI_Control->errSum; // 得到PI运算结果

    if (Result > resultMax)
    {
        PI_Control->result = resultMax;
    }
    else if (Result < (-resultMax))
    {
        PI_Control->result = ((int16_t)-resultMax);
    }
}

void Curr_Loop(PMSM_Handle *PMSM){
    
}