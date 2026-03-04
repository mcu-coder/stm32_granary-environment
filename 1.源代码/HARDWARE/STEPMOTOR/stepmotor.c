#include "stepmotor.h"





void MOTOR_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 使能GPIOA和GPIOB时钟
	RCC_APB2PeriphClockCmd(MOTOR_CLK_A | MOTOR_CLK_B, ENABLE);
	
	// PA15禁用JTAG，保留SWD
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	// 初始化GPIOB的PB8/PB9
	GPIO_InitStructure.GPIO_Pin = MOTOR_A | MOTOR_B;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(MOTOR_A_PORT, &GPIO_InitStructure);
	
	// 初始化GPIOA的PA12/PA15
	GPIO_InitStructure.GPIO_Pin = MOTOR_C | MOTOR_D;
	GPIO_Init(MOTOR_C_PORT, &GPIO_InitStructure);

	// 初始状态：所有引脚置低（修正表达式语法）
	MOTOR_A_LOW;
	MOTOR_B_LOW;
	MOTOR_C_LOW;
	MOTOR_D_LOW;
}

// 四相单拍驱动
void MOTOR_Rhythm_4_1_4(uint8_t step,uint8_t dly)
{
	switch(step)
	{
		case 1 :
			MOTOR_A_HIGH;MOTOR_B_LOW;MOTOR_C_LOW;MOTOR_D_LOW;
		break;
		
		case 2 :
			MOTOR_A_LOW;MOTOR_B_HIGH;MOTOR_C_LOW;MOTOR_D_LOW;
		break;
		
		case 3 :
			MOTOR_A_LOW;MOTOR_B_LOW;MOTOR_C_HIGH;MOTOR_D_LOW;
		break;
		
		case 4 :
			MOTOR_A_LOW;MOTOR_B_LOW;MOTOR_C_LOW;MOTOR_D_HIGH;
		break;		
	}
	delay_ms(dly);
}

// 四相双拍驱动
void MOTOR_Rhythm_4_2_4(uint8_t step,uint8_t dly)
{
	switch(step)
	{
		case 0 : break;
		
		case 1 :
			MOTOR_A_HIGH;MOTOR_B_LOW;MOTOR_C_LOW;MOTOR_D_HIGH;
		break;
		
		case 2 :
			MOTOR_A_HIGH;MOTOR_B_HIGH;MOTOR_C_LOW;MOTOR_D_LOW;
		break;
		
		case 3 :
			MOTOR_A_LOW;MOTOR_B_HIGH;MOTOR_C_HIGH;MOTOR_D_LOW;
		break;
		
		case 4 :
			MOTOR_A_LOW;MOTOR_B_LOW;MOTOR_C_HIGH;MOTOR_D_HIGH;
		break;		
	}
	delay_ms(dly);
}

// 八拍驱动
void MOTOR_Rhythm_4_1_8(uint8_t step,uint8_t dly)
{
	switch(step)
	{
		case 0 : break;
		
		case 1 :
			MOTOR_A_HIGH;MOTOR_B_LOW;MOTOR_C_LOW;MOTOR_D_LOW;
		break;
		
		case 2 :
			MOTOR_A_HIGH;MOTOR_B_HIGH;MOTOR_C_LOW;MOTOR_D_LOW;
		break;
		
		case 3 :
			MOTOR_A_LOW;MOTOR_B_HIGH;MOTOR_C_LOW;MOTOR_D_LOW;
		break;
		
		case 4 :
			MOTOR_A_LOW;MOTOR_B_HIGH;MOTOR_C_HIGH;MOTOR_D_LOW;
		break;	
		
		case 5 :
			MOTOR_A_LOW;MOTOR_B_LOW;MOTOR_C_HIGH;MOTOR_D_LOW;
		break;
		
		case 6 :
			MOTOR_A_LOW;MOTOR_B_LOW;MOTOR_C_HIGH;MOTOR_D_HIGH;
		break;
		
		case 7 :
			MOTOR_A_LOW;MOTOR_B_LOW;MOTOR_C_LOW;MOTOR_D_HIGH;
		break;
		
		case 8 :
			MOTOR_A_HIGH;MOTOR_B_LOW;MOTOR_C_LOW;MOTOR_D_HIGH;
		break;			
	}
	delay_ms(dly);
}

// 方向控制
void MOTOR_Direction(uint8_t dir,uint8_t num,uint8_t dly)	
{
	if(dir)
	{
		switch(num)
		{
			case 0:for(uint8_t i=1;i<9;i++){MOTOR_Rhythm_4_1_8(i,dly);}break;
			case 1:for(uint8_t i=1;i<5;i++){MOTOR_Rhythm_4_1_4(i,dly);}break;
			case 2:for(uint8_t i=1;i<5;i++){MOTOR_Rhythm_4_2_4(i,dly);}break;
			default:break;
		}
	}
	else
	{
		switch(num)
		{
			case 0:for(uint8_t i=8;i>0;i--){MOTOR_Rhythm_4_1_8(i,dly);}break;
			case 1:for(uint8_t i=4;i>0;i--){MOTOR_Rhythm_4_1_4(i,dly);}break;
			case 2:for(uint8_t i=4;i>0;i--){MOTOR_Rhythm_4_2_4(i,dly);}break;
			default:break;
		}	
	}
}

// 角度控制
void MOTOR_Direction_Angle(uint8_t dir,uint8_t num,u16 angle,uint8_t dly)
{
	for(u16 i=0;i<(64*angle/45);i++)	
	{
		MOTOR_Direction(dir,num,dly);	
	}
}

// 停止函数（修正语法）
void MOTOR_STOP(void)
{
	MOTOR_A_LOW;
	MOTOR_B_LOW;
	MOTOR_C_LOW;
	MOTOR_D_LOW;
}

