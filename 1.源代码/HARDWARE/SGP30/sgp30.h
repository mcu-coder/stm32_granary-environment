#ifndef __SGP30_H
#define	__SGP30_H
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
 


/***************根据自己需求更改****************/
// SGP30 GPIO宏定义

#define		SGP30_IIC_CLK										RCC_APB2Periph_GPIOA
#define 	SGP30_IIC_PORT									GPIOA
#define 	SGP30_IIC_SCL_PIN								GPIO_Pin_5
#define 	SGP30_IIC_SDA_PIN								GPIO_Pin_6	

//IO操作函数	 
#define SGP30_IIC_SCL    PAout(5) 		//SCL
#define SGP30_IIC_SDA    PAout(6) 		//SDA	 
#define SGP30_READ_SDA   PAin(6) 		//输入SDA 


/*********************END**********************/

#define SGP30_read  0xb1                                    //SGP30的读地址
#define SGP30_write 0xb0                                    //SGP30的写地址

//SGP30
void SGP30_IIC_Start(void);				                          //发送IIC开始信号
void SGP30_IIC_Stop(void);	  			                        //发送IIC停止信号
void SGP30_IIC_Send_Byte(u8 txd);			                      //IIC发送一个字节
u16 SGP30_IIC_Read_Byte(unsigned char ack);                 //IIC读取一个字节
u8 SGP30_IIC_Wait_Ack(void); 				                        //IIC等待ACK信号
void SGP30_IIC_Ack(void);					                          //IIC发送ACK信号
void SGP30_IIC_NAck(void);				                          //IIC不发送ACK信号
void SGP30_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 SGP30_IIC_Read_One_Byte(u8 daddr,u8 addr);	
void SGP30_Init(void);				  
void SGP30_Write(u8 a, u8 b);
u32 SGP30_Read(void);
void Sgp30_Get_Co2_Value(u16 *co2_value);

#endif



