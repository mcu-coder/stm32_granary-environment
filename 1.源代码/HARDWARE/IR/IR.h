#ifndef __LDR_H
#define	__LDR_H
#include "stm32f10x.h"
#include "adcx.h"
#include "delay.h"
#include "math.h"
 

#define IR_READ_TIMES	10  //火焰传感器ADC循环读取次数

//模式选择	
//模拟AO:	1
//数字DO:	0
#define	MODE 	0

/***************根据自己需求更改****************/
// LDR GPIO宏定义
#if MODE
#define		IR_AO_GPIO_CLK								RCC_APB2Periph_GPIOA
#define 	IR_AO_GPIO_PORT								GPIOA
#define 	IR_AO_GPIO_PIN								GPIO_Pin_0
#define   ADC_CHANNEL               		ADC_Channel_0	// ADC 通道宏定义

#else
#define		IR_DO_GPIO_CLK								RCC_APB2Periph_GPIOA
#define 	IR_DO_GPIO_PORT								GPIOA
#define 	IR_DO_GPIO_PIN								GPIO_Pin_1			

#endif
/*********************END**********************/


void IR_Init(void);
uint16_t IR_FireData(void);

#endif /* __ADC_H */

