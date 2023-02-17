#include "foc.h"
/*FOC基本模块函数，不通过浮点数的形式实现*/
/**
 * @brief Clark变换函数，采用Q1.15形式，等功率运算，无浮点数
 *                               alpha = a
 *                       beta = (2*b+a)/sqrt(3)
 * @param  Input: ADC采集的IA和IB
 * @retval Clark变换产生的Iβ和Iα
 */
alphabeta_t Clarke(ab_t Input)
{
  alphabeta_t Output;

  int32_t a_divSQRT3_tmp;
  int32_t b_divSQRT3_tmp;
  int32_t wbeta_tmp;
  int16_t hbeta_tmp;

  /* qIalpha = qIas*/
  Output.alpha = Input.a;

  a_divSQRT3_tmp = divSQRT_3 * ((int32_t)Input.a);

  b_divSQRT3_tmp = divSQRT_3 * ((int32_t)Input.b);

  /*qIbeta = (2*Ib+Ia)/sqrt(3)，消除Q1.15格式产生多余的乘积，使其有效等于除以根号3*/
  wbeta_tmp = ((a_divSQRT3_tmp) + (b_divSQRT3_tmp) + (b_divSQRT3_tmp)) >> 15;

  /* 饱和值判断，若溢出，按照最大值来算 */
  if (wbeta_tmp > INT16_MAX)
  {
    hbeta_tmp = INT16_MAX;
  }
  else if (wbeta_tmp < (-32768))
  {
    hbeta_tmp = ((int16_t)-32768);
  }
  else
  {
    hbeta_tmp = ((int16_t)wbeta_tmp);
  }

  Output.beta = hbeta_tmp;

  if (((int16_t)-32768) == Output.beta)
  {
    Output.beta = -32767;
  }

  return (Output);
}

/**
 * @brief  Park变换
 *                   d= alpha *cos(theta)+ beta *sin(Theta)
 *                   q=-alpha *sin(Theta)+ beta *cos(Theta)
 * @param  Input: α和β
 * @param  Theta: q1.15形式的角度值
 * @retval d轴和q轴的值
 */
qd_t Park(alphabeta_t Input, int16_t Theta)
{
  qd_t Output;
  int32_t d_tmp_1;
  int32_t d_tmp_2;
  int32_t q_tmp_1;
  int32_t q_tmp_2;
  int32_t wqd_tmp;
  int16_t hqd_tmp;
  Trig_Components Local_Vector_Components;

  Local_Vector_Components = Trig_Functions(Theta);

  /*No overflow guaranteed*/
  q_tmp_1 = Input.alpha * ((int32_t)Local_Vector_Components.hSin); // alpha *sin(Theta)

  /*No overflow guaranteed*/
  q_tmp_2 = Input.beta * ((int32_t)Local_Vector_Components.hCos); // beta *cos(Theta)
  // 转化成q1.15
  wqd_tmp = (-q_tmp_1 + q_tmp_2) >> 15; // cstat !MISRAC2012-Rule-1.3_n !ATH-shift-neg !MISRAC2012-Rule-10.1_R6

  /* Check saturation of Iq */
  if (wqd_tmp > INT16_MAX)
  {
    hqd_tmp = INT16_MAX;
  }
  else if (wqd_tmp < (-32768))
  {
    hqd_tmp = ((int16_t)-32768);
  }
  else
  {
    hqd_tmp = ((int16_t)wqd_tmp);
  }

  Output.q = hqd_tmp;

  if (((int16_t)-32768) == Output.q)
  {
    Output.q = -32767;
  }

  /*No overflow guaranteed*/
  d_tmp_1 = Input.alpha * ((int32_t)Local_Vector_Components.hCos); // alpha *cos(theta)

  /*No overflow guaranteed*/
  d_tmp_2 = Input.beta * ((int32_t)Local_Vector_Components.hSin); // beta *sin(Theta)

  /*Id component in Q1.15 Format */
  wqd_tmp = (d_tmp_1 + d_tmp_2) >> 15; // cstat !MISRAC2012-Rule-1.3_n !ATH-shift-neg !MISRAC2012-Rule-10.1_R6

  /* Check saturation of Id */
  if (wqd_tmp > INT16_MAX)
  {
    hqd_tmp = INT16_MAX;
  }
  else if (wqd_tmp < (-32768))
  {
    hqd_tmp = ((int16_t)-32768);
  }
  else
  {
    hqd_tmp = ((int16_t)wqd_tmp);
  }

  Output.d = hqd_tmp;

  if (((int16_t)-32768) == Output.d)
  {
    Output.d = -32767;
  }

  return (Output);
}

/**
 * @brief 返回角度值的正弦和余弦值，以便做下一步运算
 * @param  hAngle: q1.15形式的角度
 * @retval 正弦和余弦值
 */
Trig_Components Trig_Functions(int16_t hAngle)
{

  int32_t shindex;
  uint16_t uhindex;

  Trig_Components Local_Components;

  /* 10 bit index computation  */
  shindex = (((int32_t)32768) + ((int32_t)hAngle));
  uhindex = (uint16_t)shindex;
  uhindex /= ((uint16_t)64);

  switch (((uint16_t)uhindex) & SIN_MASK)
  {
  case U0_90:
  {
    Local_Components.hSin = hSin_Cos_Table[(uint8_t)(uhindex)];
    Local_Components.hCos = hSin_Cos_Table[(uint8_t)(0xFFu - (uint8_t)(uhindex))];
    break;
  }

  case U90_180:
  {
    Local_Components.hSin = hSin_Cos_Table[(uint8_t)(0xFFu - (uint8_t)(uhindex))];
    Local_Components.hCos = -hSin_Cos_Table[(uint8_t)(uhindex)];
    break;
  }

  case U180_270:
  {
    Local_Components.hSin = -hSin_Cos_Table[(uint8_t)(uhindex)];
    Local_Components.hCos = -hSin_Cos_Table[(uint8_t)(0xFFu - (uint8_t)(uhindex))];
    break;
  }

  case U270_360:
  {
    Local_Components.hSin = -hSin_Cos_Table[(uint8_t)(0xFFu - (uint8_t)(uhindex))];
    Local_Components.hCos = hSin_Cos_Table[(uint8_t)(uhindex)];
    break;
  }

  default:
    break;
  }
  return (Local_Components);
}

/**
 * @brief 对输入整数求平方根，当输入是负数的时候返回0
 * @param  Input int32_t number
 * @retval int32_t Square root of Input (0 if Input<0)
 */
int32_t Sqrt(int32_t wInput)
{
  int32_t wtemprootnew;

  if (wInput > 0)
  {
    uint8_t biter = 0u;
    int32_t wtemproot;

    if (wInput <= ((int32_t)2097152))
    {
      wtemproot = ((int32_t)128);
    }
    else
    {
      wtemproot = ((int32_t)8192);
    }

    do
    {
      wtemprootnew = (wtemproot + (wInput / wtemproot)) / (int32_t)2;
      if ((wtemprootnew == wtemproot) || ((int32_t)0 == wtemproot))
      {
        biter = 6U;
      }
      else
      {
        biter++;
        wtemproot = wtemprootnew;
      }
    } while (biter < 6U);
  }
  else
  {
    wtemprootnew = (int32_t)0;
  }

  return (wtemprootnew);
}

/**
 * @brief  浮点数强制转化成uint32型，根据浮点数的定义符合q1.31标准
 * @param  float 浮点数
 * @retval uint32_t无符号整数
 */
uint32_t floatToIntBit(float_t x) // cstat !MISRAC2012-Dir-4.6_a
{
  uint32_t *pInt;
  pInt = (uint32_t *)(&x); // cstat !MISRAC2012-Rule-11.3
  return (*pInt);
}

/**
 * @brief 反Park变换
 *                  Valfa= Vq*Cos(theta)- Vd*Sin(theta)
 *                  Vbeta= Vq*Sin(theta)+ Vd*Cos(theta)
 * @param  Input: Q轴和D轴的值
 * @param  Theta: rotating frame angular position in q1.15 format
 * @retval Stator voltage Valpha and Vbeta in qd_t format
 */
alphabeta_t Rev_Park(qd_t Input, int16_t Theta)
{
  int32_t alpha_tmp1;
  int32_t alpha_tmp2;
  int32_t beta_tmp1;
  int32_t beta_tmp2;
  Trig_Components Local_Vector_Components;
  alphabeta_t Output;

  Local_Vector_Components = Trig_Functions(Theta);

  /*No overflow guaranteed*/
  alpha_tmp1 = Input.q * ((int32_t)Local_Vector_Components.hCos);  //Vq*Cos(theta)
  alpha_tmp2 = Input.d * ((int32_t)Local_Vector_Components.hSin);  //Vd*Sin(theta)

  Output.alpha = (int16_t)(((alpha_tmp1) - (alpha_tmp2)) >> 15);

  beta_tmp1 = Input.q * ((int32_t)Local_Vector_Components.hSin);  //Vq*Sin(theta)
  beta_tmp2 = Input.d * ((int32_t)Local_Vector_Components.hCos);  //Vd*Cos(theta)

  // 重新标幺化
  Output.beta = (int16_t)((beta_tmp2 + beta_tmp1) >> 15);

  return (Output);
}
