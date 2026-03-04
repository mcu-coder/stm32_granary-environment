#ifndef __STEPMOTOR_H
#define	__STEPMOTOR_H
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"

// 时钟宏定义
#define	MOTOR_CLK_A 				RCC_APB2Periph_GPIOA
#define	MOTOR_CLK_B 				RCC_APB2Periph_GPIOB

// 引脚宏定义（核心修正：确保宏展开为合法表达式）
#define MOTOR_A 					(uint16_t)GPIO_Pin_8    // PB8
#define MOTOR_A_PORT 				GPIOB         
#define MOTOR_B 					(uint16_t)GPIO_Pin_9    // PB9
#define MOTOR_B_PORT 				GPIOB         
#define MOTOR_C 					(uint16_t)GPIO_Pin_12   // PA12
#define MOTOR_C_PORT 				GPIOA         
#define MOTOR_D 					(uint16_t)GPIO_Pin_15   // PA15
#define MOTOR_D_PORT 				GPIOA         

// 高低电平宏（核心修正：添加括号，确保语法合法）
#define MOTOR_A_LOW	 		do{GPIO_ResetBits(MOTOR_A_PORT, MOTOR_A);}while(0)
#define MOTOR_A_HIGH	 	do{GPIO_SetBits(MOTOR_A_PORT, MOTOR_A);}while(0)
#define MOTOR_B_LOW	 		do{GPIO_ResetBits(MOTOR_B_PORT, MOTOR_B);}while(0)
#define MOTOR_B_HIGH 	do{GPIO_SetBits(MOTOR_B_PORT, MOTOR_B);}while(0)
#define MOTOR_C_LOW 	 	do{GPIO_ResetBits(MOTOR_C_PORT, MOTOR_C);}while(0)
#define MOTOR_C_HIGH 	do{GPIO_SetBits(MOTOR_C_PORT, MOTOR_C);}while(0)
#define MOTOR_D_LOW  	do{GPIO_ResetBits(MOTOR_D_PORT, MOTOR_D);}while(0)
#define MOTOR_D_HIGH 	do{GPIO_SetBits(MOTOR_D_PORT, MOTOR_D);}while(0)

// 函数声明
void MOTOR_Init(void);
void MOTOR_Rhythm_4_1_4(u8 step,u8 dly);
void MOTOR_Rhythm_4_2_4(u8 step,u8 dly);  // 补充遗漏的声明
void MOTOR_Rhythm_4_1_8(u8 step,u8 dly);  // 补充遗漏的声明
void MOTOR_Direction(u8 dir,u8 num,u8 dly);
void MOTOR_Direction_Angle(u8 dir,u8 num,u16 angle,u8 dly);
void MOTOR_STOP(void);
#endif


