#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

 
 
void TIM1_PWM_Init(u16 arr,u16 psc);
void TIM2_Cap_Init(u16 arr,u16 psc);
void TIM4_Cap_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc); 
	void motor_cw(uint32_t);
	void motor_ccw(uint32_t);

#endif
