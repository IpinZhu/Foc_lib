#include "svpwm.h"

/**
 * @brief  SVPWM定时器回调函数，回调TIM4中断，用于误差反馈控制
 * @retval 扇区
 */
void Foc_Timer_Callback()
{
}

/**
 * @brief  初始化函数
 * @retval Nien
 */
void foc_initialize(void)
{
  /* (no initialization code required) */
}

/// @brief SVPWM模块函数，先判断扇区，然后返回一个占空比(作用于CCR1CCR2CCR3)，表现对应矢量的作用时间，同样使用Q1.15形式，优化浮点数计算
/// @param valfromval
/// @return CCR寄存器的值
Extuo Svpwm_func(Extui valfromval)
{
  Extuo Vout;
  int32_t wUAlpha;
  int32_t WX;
  int32_t WY;
  int32_t WZ;
  int32_t Timepha;
  int32_t Timephb;
  int32_t Timephc;

  int32_t M_ratio; // 调制比 = sqrt(3)*Ts/Udc

  M_ratio = (int32_t)SQRT3FACTOR * 2 * valfromval.Tpwm / valfromval.Udc;

  wUAlpha = valfromval.alpha * (int32_t)SQRT3FACTOR / 32768; // Ualpha*sqrt(3)

  WX = valfromval.beta;     // Ubeta*Ts/Udc
  WY = (-WX + wUAlpha) / 2; // sqrt(3)/2*Ualpha -Ubeta/2
  WZ = (-WX - wUAlpha) / 2; //-sqrt(3)/2*Ualpha -Ubeta/2

  if (WY < 0)
  {
    if (WZ < 0)
    {
      Vout.Sector = SECTOR_2;
      Timepha = -(M_ratio * WY) / 32768;                       // T2
      Timephb = -(M_ratio * WZ) / 32768;                       // T6
      Timephc = (2 * valfromval.Tpwm - Timepha - Timephb) / 2; // T0 = T7;

      Vout.Tcmp1 = Timephc / 2 + Timepha / 2;     // T2/2+T0/2
      Vout.Tcmp2 = Timephc / 2;                   // T0/2
      Vout.Tcmp3 = valfromval.Tpwm - Timephc / 2; // T0/2+T2/2+T6/2 = Ts/2 - T7/2
    }
    else // Z>=0
      if (WX >= 0)
      {
        Vout.Sector = SECTOR_3;

        Timepha = (M_ratio * WX) / 32768;                        // T2
        Timephb = (M_ratio * WZ) / 32768;                        // T3
        Timephc = (2 * valfromval.Tpwm - Timepha - Timephb) / 2; // T0 = T7

        Vout.Tcmp1 = valfromval.Tpwm - Timephc / 2; // Ts- T7
        Vout.Tcmp2 = Timephc / 2;                   // T0/2;
        Vout.Tcmp3 = Timephc / 2 + Timepha / 2;     // T0/2 + T2/2
      }
      else if (WX < 0)
      {
        Vout.Sector = SECTOR_4;

        Timepha = -(M_ratio * WX) / 32768;                       // T1
        Timephb = -(M_ratio * WY) / 32768;                       // T3
        Timephc = (2 * valfromval.Tpwm - Timepha - Timephb) / 2; // T0 = T7

        Vout.Tcmp1 = valfromval.Tpwm - Timephc / 2; // Ts- T7
        Vout.Tcmp2 = Timephc / 2 + Timepha / 2;     // T0/2 +T1/2
        Vout.Tcmp3 = Timephc / 2;                   // T0/2
      }
  }
  else if (WY >= 0)
  {
    if (WZ < 0)
    {
      if (WX >= 0)
      {
        Vout.Sector = SECTOR_1;

        Timepha = (M_ratio * WY) / 32768;                        // T4
        Timephb = (M_ratio * WX) / 32768;                        // T6
        Timephc = (2 * valfromval.Tpwm - Timepha - Timephb) / 2; // T0 = T7;

        Vout.Tcmp1 = Timephc / 2;                   // T0/2
        Vout.Tcmp2 = Timephc / 2 + Timepha / 2;     // T0/2 +T4/2
        Vout.Tcmp3 = valfromval.Tpwm - Timephc / 2; // Ts/2 - T7/2
      }
      else if (WX < 0)
      {
        Vout.Sector = SECTOR_6;

        Timepha = -(M_ratio * WZ) / 32768;                       // T4
        Timephb = -(M_ratio * WX) / 32768;                       // T5
        Timephc = (2 * valfromval.Tpwm - Timepha - Timephb) / 2; // T0 = T7;

        Vout.Tcmp1 = Timephc / 2;                   // T0/2
        Vout.Tcmp2 = valfromval.Tpwm - Timephc / 2; // TS/2 -T7/2
        Vout.Tcmp3 = Timephc / 2 + Timepha / 2;     // T0/2 + T4/2
      }
    } // WZ>=0
    else if (WX < 0)
    {
      Vout.Sector = SECTOR_5;

      Timepha = (M_ratio * WZ) / 32768;                        // T1
      Timephb = (M_ratio * WY) / 32768;                        // T5
      Timephc = (2 * valfromval.Tpwm - Timepha - Timephb) / 2; // T0 = T7;

      Vout.Tcmp1 = Timephc / 2 + Timepha / 2;     // T0/2 + T1/2
      Vout.Tcmp2 = valfromval.Tpwm - Timephc / 2; // TS/2 -T7/2
      Vout.Tcmp3 = Timephc / 2;                   // T0/2
    }
    return Vout;
  }
}

/// @brief Foc开环函数，每运行一次为执行一次Step活动，返回一个定时器的占空比，需要初始化Step类型，可以类比为步进电机
/// @param Step 用于电机开环运行的结构体
/// @return StepRet 返回占空比
Extuo Foc_Step(StepTyp Step)
{
  qd_t AlBt_Tmp;
  alphabeta_t AlBt;
  Extuo StepRet;
  Extui Svtmp;

  AlBt_Tmp.d = Step.Ud_ref;
  AlBt_Tmp.q = Step.Uq_ref;

  AlBt = Rev_Park(AlBt_Tmp, Step.zeta); // rePark To Ualpha and Ubeta

  Svtmp.alpha = AlBt.alpha;
  Svtmp.beta = AlBt.beta;
  Svtmp.Tpwm = Step.Tpwm;
  Svtmp.Udc = Step.Udc;

  StepRet = Svpwm_func(Svtmp);

  return StepRet;
}

/// @brief 改变CCR寄存器的值
/// @param htim
/// @param Vabc
void Timer_DutyExchange(TIM_HandleTypeDef *htim, Extuo Vabc)
{
  htim->Instance->CCR1 = (uint16_t)Vabc.Tcmp1;
  htim->Instance->CCR2 = (uint16_t)Vabc.Tcmp2;
  htim->Instance->CCR3 = (uint16_t)Vabc.Tcmp3;
  /*User change allowed*/
}

