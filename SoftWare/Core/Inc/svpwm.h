#ifndef __SVPWM_H_
#define __SVPWM_H_
#include "main.h"
#include "foc.h"
#include "tim.h"
#include "adc.h"

/// @brief Svpwm输入值
typedef struct
{
    int16_t alpha;
    int16_t beta;
    int16_t Udc;
    int16_t Tpwm;
} Extui;
/// @brief 作用在定时器上的值
typedef struct
{
    int16_t Tcmp1;
    int16_t Tcmp2;
    int16_t Tcmp3;
    int8_t Sector;
} Extuo;

/// @brief Step函数使用
typedef struct
{
    int16_t zeta;
    int16_t Ud_ref;
    int16_t Uq_ref;
    uint16_t Udc;
    uint16_t Tpwm;
} StepTyp;
// @param : sectors
#define SECTOR_1 0U
#define SECTOR_2 1U
#define SECTOR_3 2U
#define SECTOR_4 3U
#define SECTOR_5 4U
#define SECTOR_6 5U

//@param : operator maybe翻译成算子or maybe reverse
#define SQRT3FACTOR ((uint16_t)0xDDB4) /* = (32768 * 1.732051) 表示根号三*/
#define TS_INTIMER 16800 / 2


void foc_initialize(void);
Extuo Svpwm_func(Extui valfromval);
Extuo Foc_Step(StepTyp Step);
void Timer_DutyExchange(TIM_HandleTypeDef *htim,Extuo Vabc);


#endif // !__SVPWM_H
