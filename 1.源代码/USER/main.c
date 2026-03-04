#include "stm32f10x.h"                 
#include "led.h"                       
#include "beep.h"                      
#include "usart.h"                     
#include "delay.h"                     
#include "oled.h"                      
#include "key.h"                       
#include "Modules.h"                   
#include "adcx.h"                      
#include "flash.h"                     
#include "usart2.h"                    
#include "usart3.h"                    
#include "IR.h"
#include "HW.h"
#include "sgp30.h"
#include "dht11.h"
#include "string.h"                    
#include "relay.h"                     
#include "timer.h"                     
#include "TIM3.h"                      
#include "TIM2.h"                      
#include "gizwits_product.h"           
#include "gizwits_protocol.h"        

#include "stepmotor.h"
 

#define KEY_Long1	11                           
#define KEY_1	1                     
#define KEY_2	2                     
#define KEY_3	3                     
#define KEY_4	4                     

#define AUTO_MODE     1   // 自动模式
#define MANUAL_MODE   2   // 手动模式
#define SETTINGS_MODE 3   // 设置模式

#define FLASH_START_ADDR	0x0801f000	// Flash阈值存储起始地址（STM32F103C8T6 Flash尾部区域）

//#define CONFIG_MODE_WINDOW 3000  

#ifndef GIZWITS_MODE_NORMAL
#define GIZWITS_MODE_NORMAL    0   // 正常模式
#endif
#ifndef GIZWITS_MODE_SOFTAP
#define GIZWITS_MODE_SOFTAP    1   // 软AP配网模式
#endif
#ifndef GIZWITS_MODE_AIRLINK
#define GIZWITS_MODE_AIRLINK   2   // 一键配网模式
#endif


// 全局变量定义
extern  SensorModules sensorData;								// 传感器数据结构体（存储温度、光伏电压/电流/功率/转换率）
extern  SensorThresholdValue Sensorthreshold;		// 传感器阈值结构体（存储温度、电压、电流阈值）
extern  DriveModules driveData;									// 驱动器状态结构体（存储LED、蜂鸣器、继电器开关状态）

uint8_t mode = 1;	// 系统模式：1=自动模式，2=手动模式，3=设置模式（默认自动模式）
u8 dakai;           // 串口3通信传递变量
u8 Flag_dakai;      // 串口3接收完成标志位
uint8_t is_secondary_menu = 0;  // 二级菜单标志位：0=一级菜单，1=二级菜单
uint8_t secondary_pos = 1;      // 二级菜单光标位置：
uint8_t secondary_type = 0;     // 二级菜单类型
//float pa0_voltage = 0.0f;       
uint8_t count_m = 1;  // 手动模式菜单计数
uint8_t count_s = 1;	 // 设置模式菜单计数
//extern unsigned char p[16];     // 温度显示字符串缓冲区

uint8_t last_mode = 0;  // 上一次系统模式（用于模式切换时清屏刷新显示）
extern dataPoint_t currentDataPoint;

uint8_t send_data[]  = "";//语音播放曲目1
uint8_t send_data1[] = "";//语音播放曲目2 

uint8_t motor_flag_last = 0;    // 上一次MOTOR_Flag的值（初始和driveData.MOTOR_Flag一致）
uint8_t motor_rotate_done = 1;  // 转动执行完成标志：1=已完成（初始完成，上电不执行）



/**
  * @brief  自动模式第一页固定菜单显示（仅显示标题，不包含动态数据）
  * @param  无
  * @retval 无
  */
void OLED_autoPage1(void)		//自动模式菜单第一页
{	
	// 显示"温度：  C"（0,0坐标开始，16x16字体）


	
	//显示“温度：  C”
	OLED_ShowChinese(0,0,0,16,1);	  //温
	OLED_ShowChinese(16,0,2,16,1);	//度
	OLED_ShowChar(32,0,':',16,1);
	
	//湿度
	OLED_ShowChinese(64,0,1,16,1); 
	OLED_ShowChinese(80,0,2,16,1);
	OLED_ShowChar(96,0,':',16,1);
	 
//	OLED_Refresh();

}

void OLED_autoPage2(void)   // 自动模式菜单第二页
{
	

}

/**
  * @brief  自动模式第一页动态传感器数据显示
  * @param  无
  * @retval 无
  */
void SensorDataDisplay1(void)		//传感器数据显示第一页
{

		 //显示温度数据
    OLED_ShowNum(40,0,sensorData.temp,2,16,1);
    //显示湿度数据
    OLED_ShowNum(104,0,sensorData.humi,2,16,1);
	
	
	  OLED_ShowNum(72, 16, sensorData.Smoge, 4, 16, 1);
	  OLED_ShowString(104, 16, "ppm", 16, 1);  
	
	  OLED_ShowNum(72, 32, sensorData.CO2, 4, 16, 1);
	  OLED_ShowString(104, 32, "ppm", 16, 1);  
	
//	 OLED_ShowNum(80, 48, sensorData.value, 1, 16, 1);
//	
//	 OLED_ShowNum(10, 48, sensorData.fire, 1, 16, 1);
		if(sensorData.fire )
		{
			OLED_ShowChinese(40,48,3,16,1);	
//			OLED_ShowChinese(16,48,6,16,0);	
		
		}
		else
		{
				OLED_ShowChinese(40,48,4,16,1);	
			
		}

		if(sensorData.value)
		{
			OLED_ShowChinese(112,48,3,16,1);	
		}
		else
		{
			OLED_ShowChinese(112,48,4,16,1);	
//			 OLED_ClearArea(80, 111, 48, 63);
				
		}

}

void SensorDataDisplay2(void)		// 自动模式第二页传感器数据显示
{


}

/**
  * @brief  手动模式第一页固定菜单显示（仅显示标题，不包含设备状态）
  * @param  无
  * @retval 无
  */
void OLED_manualPage1(void)
{
	// 显示"灯光： 
	OLED_ShowChinese(16,0,24,16,1);	// 
	OLED_ShowChinese(32,0,25,16,1);	// 
	OLED_ShowChinese(48,0,26,16,1);	// 
	OLED_ShowChar(64,0,':',16,1);    //
 
	
		// 显示
	OLED_ShowChinese(16,48,19,16,1);	
	OLED_ShowChinese(32,48,20,16,1);	
	OLED_ShowChar(48,48,':',16,1);  
}

/**
  * @brief  手动模式第一页设备状态显示（灯光、蜂鸣器的"开/关"状态）
  * @param  无
  * @retval 无
  */
void ManualSettingsDisplay1(void)
{
	 
	
		if(driveData.MOTOR_Flag == 1)
	{
		OLED_ShowChinese(96,32,36,16,1); 
	}
	else
	{
		OLED_ShowChinese(96,32,37,16,1); 
	}
}

/**
  * @brief  设置模式第一页固定菜单显示（仅显示阈值标题，不包含阈值数值）
  * @param  无
  * @retval 无
  */
void OLED_settingsPage1(void)
{
	// 显示"温度阈值： 
	OLED_ShowChinese(16,0,0,16,1);	
	OLED_ShowChinese(32,0,2,16,1);	
	OLED_ShowChinese(48,0,27,16,1);	
	OLED_ShowChinese(64,0,28,16,1);	
	OLED_ShowChar(80,0,':',16,1);   

	// 显示"湿度阈值：
	OLED_ShowChinese(16,16,1,16,1);	
	OLED_ShowChinese(32,16,2,16,1);	
	OLED_ShowChinese(48,16,27,16,1);
	OLED_ShowChinese(64,16,28,16,1);
	OLED_ShowChar(80,16,':',16,1);  
 
}

void OLED_settingsPage2(void)// 设置模式第二页固定菜单显示
{
	//防盗模式
	OLED_ShowChinese(16,0,38,16,1);	
	OLED_ShowChinese(32,0,39,16,1);	
	OLED_ShowChinese(48,0,40,16,1);
	OLED_ShowChinese(64,0,41,16,1);
	OLED_ShowChar(80,0,':',16,1); 

}
void OLED_settingsPage3(void)// 设置模式第三页固定菜单显示
{

}

/**
  * @brief  设置模式第一页阈值数值显示
  * @param  无
  * @retval 无
  */
void SettingsThresholdDisplay1(void)//实际阈值1
{
	
	OLED_ShowNum(90,0, Sensorthreshold.tempValue, 2,16,1);
	
	OLED_ShowNum(90, 16, Sensorthreshold.humiValue , 2,16,1);
	
  OLED_ShowNum(90, 32, Sensorthreshold.CO2Value , 4,16,1);
	
	OLED_ShowNum(90, 48, Sensorthreshold.SmogeValue , 4,16,1);
		
}

void SettingsThresholdDisplay2(void)// 设置模式第二页阈值数值显示
{
// 显示防盗模式状态（开/关）
    if(Sensorthreshold.anti_theft_mode == 1)
    {
        OLED_ShowChinese(90,0,36,16,1);  //开
    }
   
}

void SettingsThresholdDisplay3(void)// 设置模式第三页阈值数值显示
{

}

/**
  * @brief  自动模式下按键2短按计数
  * @param  无
  * @retval 返回自动模式菜单页数
  */
uint8_t SetAuto(void)  
{
 return 1;  // 自动模式只有一页，始终返回1
}

/**
  * @brief  手动模式下按键2短按计数（切换灯光/蜂鸣器控制菜单）
  * @param  无
  * @retval 返回当前手动模式菜单页码（1=灯光控制，2=蜂鸣器控制）
  */
uint8_t SetManual(void)  
{
	if(KeyNum == KEY_2)  // 检测按键2短按（菜单切换）
	{
		KeyNum = 0;        // 清空按键标识，避免重复触发
		count_m++;         // 手动模式菜单页码+1
		 
	}
	return count_m;      // 返回当前手动模式菜单页码
}

/**
  * @brief  设置模式下按键2短按计数
  * @param  无
  * @retval 返回当前设置模式菜单页码
  */
uint8_t SetSelection(void)
{
    if(KeyNum == KEY_2 && is_secondary_menu == 0)  // 按键2短按且处于一级菜单
    {
        KeyNum = 0;                                // 清空按键标识，避免重复触发
        count_s++;                                 // 设置模式菜单页码+1
        
				
    }
    return count_s;                                // 返回当前设置模式菜单页码
}

/**
  * @brief  手动模式菜单光标显示（指示当前选中的控制项）
  * @param  num：光标位置（1=灯光控制，2=蜂鸣器控制）
  * @retval 无
  */
void OLED_manualOption(uint8_t num)
{
	switch(num)
	{
		case 1:	
			OLED_ShowChar(0, 0,'>',16,1);  
			OLED_ShowChar(0,16,' ',16,1); 
			OLED_ShowChar(0,32,' ',16,1);  
			OLED_ShowChar(0,48,' ',16,1); 
			break;
		case 2:	
			OLED_ShowChar(0, 0,' ',16,1); 
			OLED_ShowChar(0,16,'>',16,1);  
			OLED_ShowChar(0,32,' ',16,1); 
			OLED_ShowChar(0,48,' ',16,1); 
			break;
		 
			
			default: break;
	}
}

/**
  * @brief  设置模式菜单光标显示（指示当前选中的阈值设置项）
  * @param  num：光标位置
  * @retval 无
  */
void OLED_settingsOption(uint8_t num)
{
	static uint8_t prev_num = 1;  // 上一次光标位置（用于清除历史光标）

    // 清除上一次光标位置（仅操作光标，不影响阈值数据显示）
    switch(prev_num)
    {
        case 1: OLED_ShowChar(0, 0, ' ', 16, 1); break; 
        case 2: OLED_ShowChar(0, 16, ' ', 16, 1); break; 
        case 3: OLED_ShowChar(0, 32, ' ', 16, 1); break; 
        case 4: OLED_ShowChar(0, 48, ' ', 16, 1); break; 
			  case 5: OLED_ShowChar(0, 0, ' ', 16, 1); break; 
        default: break;
    }
	switch(num)
	{
		case 1:	
			OLED_ShowChar(0, 0,'>',16,1);  
			OLED_ShowChar(0,16,' ',16,1);  
			OLED_ShowChar(0,32,' ',16,1);  
			OLED_ShowChar(0,48,' ',16,1);  
			break;
		case 2:	
			OLED_ShowChar(0, 0,' ',16,1);  
			OLED_ShowChar(0,16,'>',16,1);  
			OLED_ShowChar(0,32,' ',16,1);  
			OLED_ShowChar(0,48,' ',16,1);  
			break;
		 
		default: break;
	}
	 prev_num = num;  // 更新上一次光标位置为当前位置
  // OLED_Refresh(); // 仅刷新光标区域，数据区域保持不变
}

/**
  * @brief  自动模式控制逻辑（根据传感器数据与阈值对比，自动控制LED、蜂鸣器、继电器）
  * @param  无
  * @retval 无
  */
void AutoControl(void)//自动控制
{
 
	
	if( sensorData.CO2 > Sensorthreshold.CO2Value)                           
  {
   
		driveData.MOTOR_Flag =1; 
	
		
  }
  else
	{
	 
		driveData.MOTOR_Flag =0; 
	
  }
	
	if(sensorData.fire||sensorData.Smoge>Sensorthreshold.SmogeValue|| sensorData.CO2 > Sensorthreshold.CO2Value)
		{
			 driveData.BEEP_Flag =1; 
		
		}
		else
		{
			 driveData.BEEP_Flag =0; 
			
		}
		
		
		if(Sensorthreshold.anti_theft_mode == 1)  // 防盗模式已开启
{
    if(sensorData.value == 1)  // 光电红外传感器检测到有人
    {
 
           USART3_SendString(send_data1);   // 串口3发送send_data1[]
           delay_ms(1000);
          
        
    }
   
}
else  // 防盗模式关闭
{
   
}

}

/**
  * @brief  手动模式控制逻辑（根据按键操作，手动控制LED、蜂鸣器）
  * @param  num：当前手动模式菜单页码（1=灯光控制，2=蜂鸣器控制）
  * @retval 无
  */
void ManualControl(uint8_t num)
{
	switch(num)
	{
		case 1:  // 灯光控制菜单（控制继电器，对应灯光）
            if(KeyNum == KEY_3)  // 按键3短按=开启灯光
            {
                      driveData.LED_Flag = 1;  // 继电器开启标志位设1
                KeyNum = 0;                // 清空按键标识，避免重复触发
               
            }
            break;

		case 2:  // 蜂鸣器控制菜单（控制蜂鸣器）
            if(KeyNum == KEY_3)  // 按键3短按=开启蜂鸣器
            {
                KeyNum = 0;  
                if (driveData.BEEP_Flag != 1)  
                {
                    driveData.BEEP_Flag = 1;  
                }
            }
            
            break;
						
			case 3:  // 灯光控制菜单
            
            if(KeyNum == KEY_4)  
            {
               driveData.MOTOR_Flag = 0; 
               KeyNum = 0;              
               
            }
            break;
						
		case 4:  // 灯光控制菜单（控制继电器，对应灯光）
             
            if(KeyNum == KEY_4)  
            {
                driveData.RELAY_Flag = 0;  
                KeyNum = 0;                
               
            }
            break;
		default: break;
	}
}

/**
  * @brief  驱动器执行函数（根据driveData结构体标志位，控制LED、蜂鸣器、继电器硬件）
  * @param  无
  * @retval 无
  */
void Control_Manager(void)
{
     
		
		// 控制继电器（RELAY_Flag=1=开启，RELAY_Flag=0=关闭）
		if(driveData.RELAY_Flag )
    {	
       RELAY_ON;  // 继电器开启
    }
    else 
    {
       RELAY_OFF; // 继电器关闭
    }
		
		 
		 if(driveData.MOTOR_Flag != motor_flag_last)
    {
        motor_rotate_done = 0;  // 状态变化，重置执行标志（允许执行一次转动）
        motor_flag_last = driveData.MOTOR_Flag; // 更新上一次状态
    }

     
}

/**
  * @brief  阈值设置函数
  * @param  num：当前设置模式菜单页码
  * @retval 无
  */
void ThresholdSettings(uint8_t num)
{
	
	switch (num)
	{
		
		case 1:
			if (KeyNum == KEY_3)  // 按键3短按=温度阈值+1
			{
				KeyNum = 0;  // 清空按键标识，避免重复触发
				Sensorthreshold.tempValue += 1;  // 温度阈值+1
				if (Sensorthreshold.tempValue > 40)  // 超过最大值50℃
				{
					Sensorthreshold.tempValue = 20;  // 循环到最小值20℃
				}
				// 同步温度阈值到云端数据点
				currentDataPoint.valuetempValue = Sensorthreshold.tempValue;
			}
		 
			break;
		
		
		case 2:
			if (KeyNum == KEY_3)  
			{
				KeyNum = 0;  
				Sensorthreshold.humiValue += 5;  
				if (Sensorthreshold.humiValue > 70) 
				{
					Sensorthreshold.humiValue = 30;  
				}
				
				currentDataPoint.valuehumiValue = Sensorthreshold.humiValue;
				
			}
			 
			break;
		
		
	case 3:
			if (KeyNum == KEY_3)  
			{
				KeyNum = 0;  // 清空按键标识，避免重复触发
				Sensorthreshold.CO2Value += 50;  // 电流阈值+5mA
				if (Sensorthreshold.CO2Value > 2000)  // 超过最大值70mA
			{
					Sensorthreshold.CO2Value = 500;  // 循环到最小值10mA
				}
				
				currentDataPoint.valueCO2Value = Sensorthreshold.CO2Value;
			}
		 
        break;	
	
						
	case 4:
			if (KeyNum == KEY_3)  // 按键3短按=电流阈值+5mA
			{
				KeyNum = 0;  // 清空按键标识，避免重复触发
				Sensorthreshold.SmogeValue += 10;  // 电流阈值+5mA
				if (Sensorthreshold.SmogeValue > 150)  // 超过最大值70mA
				{
					Sensorthreshold.SmogeValue = 10;  // 循环到最小值10mA
				}
				
				currentDataPoint.valueSmogeValue= Sensorthreshold.SmogeValue;
			}
		 
			break;
      case 5: // 防盗模式设置
            if (KeyNum == KEY_3 || KeyNum == KEY_4)  // KEY3/KEY4都可切换
            {
                KeyNum = 0;  
                // 切换防盗模式：0→1，1→0
                Sensorthreshold.anti_theft_mode = !Sensorthreshold.anti_theft_mode;
            }
           
        default: break;
			
	}   
}

/**
  * @brief  从Flash读取阈值数据（系统上电时加载掉电保存的阈值）
  * @param  无
  * @retval 无
  */
void FLASH_ReadThreshold()
{
	  Sensorthreshold.tempValue = FLASH_R(FLASH_START_ADDR);          // +0
    Sensorthreshold.humiValue = FLASH_R(FLASH_START_ADDR + 2);      // +2
    Sensorthreshold.CO2Value = FLASH_R(FLASH_START_ADDR + 4);       // +4
    Sensorthreshold.SmogeValue = FLASH_R(FLASH_START_ADDR + 6);
	
	 // 合法性校验：
    if(Sensorthreshold.anti_theft_mode > 1)
    {
        Sensorthreshold.anti_theft_mode = 0;
    }
		Sensorthreshold.anti_theft_mode = FLASH_R(FLASH_START_ADDR + 8);       // +4
}


// 配网模式扫描函数（仅上电3秒内长按KEY3/KEY4触发）
void ScanGizwitsMode(void)
{
    static uint8_t key3_flag = 0;
    static uint8_t key4_flag = 0;
  //  static uint8_t last_mode = WIFI_INVALID_MODE; 

    if(!KEY3)
    {
        delay_ms(20); // 消抖
        if(!KEY3 && key3_flag == 0)
        {
            key3_flag = 1;
            // 清屏显示配网提示
            OLED_Clear();
            OLED_ShowChinese(32,16,42,16,1); //热
            OLED_ShowChinese(48,16,43,16,1); //点                               
            OLED_ShowChinese(64,16,46,16,1); //配
            OLED_ShowChinese(80,16,47,16,1); //网
					 
            OLED_Refresh();
					 delay_ms(1000); 
           

        }
    }
    else
    {
        key3_flag = 0;
    }
    
    // ===== KEY4（一键配网）检测 =====
    if(!KEY4)
    {
        delay_ms(20); // 消抖
        if(!KEY4 && key4_flag == 0)
        {
            key4_flag = 1;
            // 清屏显示配网提示
            OLED_Clear();
            OLED_ShowChinese(32,16,44,16,1); //一
            OLED_ShowChinese(48,16,45,16,1); //键
            OLED_ShowChinese(64,16,46,16,1); //配
            OLED_ShowChinese(80,16,47,16,1); //网
					 

        }
    }
    else
    {
        key4_flag = 0;
    }
}




/**
  * @brief  主函数（系统初始化、主循环逻辑）
  * @param  无
  * @retval int：返回值（实际无意义，符合C语言标准）
  */
int main(void)
{ 
    SystemInit();    
    delay_init(72);  
    ADCX_Init();     
      
    USART1_Config(); 
    USART2_Config(); 
    USART3_Config(); 
    Key_Init();      
    HW_Init();
	   
    OLED_Init();     
	  RELAY_Init();    
    OLED_Clear();    

    // 从Flash读取阈值（上电加载掉电保存的阈值）
    delay_ms(100);   // 延时100ms，确保Flash稳定
    FLASH_ReadThreshold();

      TIM2_Init(72-1,1000-1);  // 初始化定时器2（2ms定时中断，用于系统滴答计数）
	  TIM3_Int_Init(1000-1,72-1);		// 初始化定时器3（1ms定时中断，用于按键扫描/数据刷新）
    // 状态管理静态变量（主循环中保持状态）
    static uint32_t last_sensor_time = 0; // 传感器扫描时间戳（控制扫描频率）
    static uint32_t last_display_time = 0; // 显示刷新时间戳（控制显示频率）
    
    // 阈值合法性校验
    if (Sensorthreshold.tempValue < 20 || Sensorthreshold.tempValue > 50 ||
        Sensorthreshold.humiValue < 30 || Sensorthreshold.humiValue > 70 ||
        Sensorthreshold.CO2Value < 500 || Sensorthreshold.CO2Value > 1500 ||
		    Sensorthreshold.SmogeValue > 1000 ||
        Sensorthreshold.anti_theft_mode > 1)
    {
        // 初始化默认值
        Sensorthreshold.tempValue = 30;
        Sensorthreshold.humiValue = 50;
        Sensorthreshold.CO2Value = 1500;
			  Sensorthreshold.SmogeValue = 20;
        Sensorthreshold.anti_theft_mode = 0; // 默认防盗模式关
        
        // 写入默认值到Flash
        FLASH_Unlock();
        FLASH_ProgramHalfWord(FLASH_START_ADDR, Sensorthreshold.tempValue);
        FLASH_ProgramHalfWord(FLASH_START_ADDR + 2, Sensorthreshold.humiValue);
        FLASH_ProgramHalfWord(FLASH_START_ADDR + 4, Sensorthreshold.CO2Value);
        FLASH_ProgramHalfWord(FLASH_START_ADDR + 6, Sensorthreshold.SmogeValue);
			  FLASH_ProgramHalfWord(FLASH_START_ADDR + 8, Sensorthreshold.anti_theft_mode);
        FLASH_Lock();
    }

    USART3_SendString("AF:30");   //音量调到最大
    delay_ms(300);
    USART3_SendString("A7:00001");  //欢迎使用

	delay_ms(200);
	
  printf("Start \n"); 

   delay_ms(200);

    
    while (1)  // 主循环（无限循环，处理系统所有逻辑）
 {	
	 

        // ==================== 获取当前系统时间（基于定时器2滴答计数）====================
        uint32_t current_time = delay_get_tick();
        
        // ==================== 控制传感器扫描频率（每200ms扫描一次）====================
        if(current_time - last_sensor_time > 100) // 100个定时器2滴答 = 100*2ms=200ms
        {
            SensorScan(); 	// 扫描传感器，获取最新温度、光伏电压/电流/功率数据
					 // Update_Display_Parts(); // 拆分传感器数据为整数+小数部分（用于OLED显示）
            last_sensor_time = current_time; // 更新传感器扫描时间戳
        }
        
        // ==================== 立即处理按键（模式切换、参数调整等）====================
        uint8_t current_key_num = KeyNum; // 保存当前按键值（避免被后续逻辑修改）
        
        // 模式切换按键处理（仅响应模式切换相关按键）
        if(current_key_num != 0)
        {
            switch(mode)
            {
                case AUTO_MODE: // 当前为自动模式
                    if(current_key_num == KEY_1) // 按键1短按：切换到手动模式
                    {
                        mode = MANUAL_MODE;
                        count_m = 1; // 手动模式菜单页码重置为1（灯光控制）
                        // 切换到手动模式时，关闭LED和蜂鸣器
                        driveData.LED_Flag = 0;
                        driveData.BEEP_Flag = 0;
											  driveData.RELAY_Flag = 0; 

                        KeyNum = 0; // 清空按键标识
                    }
                    else if(current_key_num == KEY_Long1) // 按键1长按：切换到设置模式
                    {
                        mode = SETTINGS_MODE;
                        count_s = 1; // 设置模式菜单页码重置为1
                        KeyNum = 0; // 清空按键标识
                    }
                    break;
                    
                case MANUAL_MODE: // 当前为手动模式
                    if(current_key_num == KEY_1) // 按键1短按：切换到自动模式
                    {
                        mode = AUTO_MODE;
                        KeyNum = 0; // 清空按键标识
                    }
                    break;
                    
                case SETTINGS_MODE: // 当前为设置模式
                    // 设置模式内部按键在模式处理中单独处理
                    break;
            }
        }
        
        // ==================== 模式切换时清屏并刷新菜单 ====================
        if(last_mode != mode)
        {
            OLED_Clear(); // 清屏，避免不同模式菜单重叠
            
            // 绘制新模式的固定菜单标题
            switch(mode)
            {
                case AUTO_MODE:
                    OLED_autoPage1(); // 绘制自动模式第一页固定菜单
                    break;
                case MANUAL_MODE:
                    OLED_manualPage1(); // 绘制手动模式第一页固定菜单
                    break;
                case SETTINGS_MODE:
                    OLED_settingsPage1(); // 绘制设置模式第一页固定菜单
                    break;
            }
            OLED_Refresh(); // 立即刷新OLED显示
			      last_mode = mode; // 更新上一次模式为当前模式
        }
        
        // ==================== 各模式核心逻辑处理 ====================
        switch(mode)
        {
            case AUTO_MODE: // 自动模式
                // 显示传感器动态数据
                SensorDataDisplay1();	
                AutoControl(); // 自动控制LED、蜂鸣器、继电器
                Control_Manager(); // 执行驱动器控制（硬件操作）
                break;
                
            case MANUAL_MODE: // 手动模式
            {
                static uint8_t manual_page_initialized = 0; // 手动模式页面初始化标志
                static uint8_t last_manual_count = 0; // 上一次手动模式菜单页码
                static uint8_t last_LED_Flag = 0; // 上一次LED状态
                static uint8_t last_BEEP_Flag = 0; // 上一次蜂鸣器状态
                static uint8_t force_refresh = 0;  // 强制刷新标志（模式切换时使用）
                
                // 模式切换到手动模式时，初始化状态
                if(last_mode != mode)
                {
                    manual_page_initialized = 0;
                    last_manual_count = 0;
                    last_LED_Flag = driveData.LED_Flag;
                    last_BEEP_Flag = driveData.BEEP_Flag;
                    force_refresh = 1;  // 设置强制刷新标志
                    
                    count_m = 1; // 光标默认选中灯光控制
                    // 初始状态：关闭LED和蜂鸣器
                    driveData.LED_Flag = 0;
                    driveData.BEEP_Flag = 0;
									  driveData.RELAY_Flag = 0;
									
                }
                
                uint8_t current_manual_count = SetManual(); // 获取当前手动模式菜单页码
                
                // 检测设备状态是否变化（变化则需要刷新显示）
                uint8_t need_refresh = 0;
                if(driveData.LED_Flag != last_LED_Flag || driveData.BEEP_Flag != last_BEEP_Flag)
                {
                    need_refresh = 1;
                    last_LED_Flag = driveData.LED_Flag; // 更新LED状态
                    last_BEEP_Flag = driveData.BEEP_Flag; // 更新蜂鸣器状态
                }
                
                // 页面未初始化、页码变化、设备状态变化或强制刷新时，重绘页面
                if(!manual_page_initialized || current_manual_count != last_manual_count || need_refresh || force_refresh)
                {
                    OLED_manualPage1();          // 绘制固定菜单标题
                    OLED_manualOption(current_manual_count); // 显示当前光标位置
                    ManualSettingsDisplay1();    // 显示设备状态（开/关）
                    manual_page_initialized = 1; // 标记页面已初始化
                    last_manual_count = current_manual_count; // 更新上一次页码
                    force_refresh = 0;  // 清除强制刷新标志
                    OLED_Refresh(); // 强制刷新显示
                }
                
                // 处理手动模式按键（控制设备开关）
                if(current_key_num != 0)
                {
                    ManualControl(current_manual_count); // 执行手动控制逻辑
                    OLED_manualPage1();          // 重绘固定菜单标题
                    OLED_manualOption(current_manual_count); // 重绘光标
                    ManualSettingsDisplay1();    // 重绘设备状态
                    OLED_Refresh(); // 按键操作后立即刷新显示
                    KeyNum = 0; // 清空按键标识
                }
                
                // 确保显示内容始终正确（冗余绘制，避免显示异常）
                OLED_manualPage1();          // 绘制固定菜单标题
                OLED_manualOption(current_manual_count); // 显示光标
                ManualSettingsDisplay1();    // 显示设备状态
                
                Control_Manager(); // 执行驱动器控制（硬件操作）
                break;
            }
                
            case SETTINGS_MODE: // 设置模式
            {
                static uint8_t is_threshold_page_inited = 0; // 设置模式页面初始化标志
							  static uint8_t last_count_s = 1;             // 新增：记录上一次的页码，用于检测页面切换
                uint8_t curr_count_s = SetSelection(); // 获取当前设置模式菜单页码
                
							
							// 检测页码是否切换（跨页），切换则清屏并重置初始化标志
            if(curr_count_s != last_count_s)
           {
            if((curr_count_s <=4 && last_count_s ==5) || (curr_count_s ==5 && last_count_s <=4))
             {
               OLED_Clear();          // 仅跨页清屏
               is_threshold_page_inited = 0; 
             }
               last_count_s = curr_count_s;  
                                       }
                // 处理设置模式内部按键（修改阈值、返回自动模式）
                if(current_key_num != 0)
                {
                    if (is_secondary_menu == 1) // 处于二级菜单（预留扩展）
                    {
                        // 二级菜单按键处理（暂未实现）
                        if (current_key_num == KEY_2 || current_key_num == KEY_3 || current_key_num == KEY_4)
                        {
                            OLED_Refresh(); // 刷新显示
                            KeyNum = 0; // 清空按键标识
                        }
                        else if (current_key_num == KEY_1) // 按键1短按：返回一级菜单
                        {
                            is_secondary_menu = 0;
                            secondary_pos = 1;
                            OLED_Clear(); // 清屏
													if(curr_count_s == 5)
                {
                    OLED_settingsPage2(); // 绘制第二页
                    SettingsThresholdDisplay2();
                }
								 else
                {
                    OLED_settingsPage1(); // 绘制第一页
                    SettingsThresholdDisplay1();
                }

                            OLED_settingsOption(curr_count_s); // 重绘光标
                            OLED_Refresh(); // 刷新显示
                            KeyNum = 0; // 清空按键标识
                        }
                    }
                    else // 处于一级菜单
                    {
                        if (current_key_num == KEY_3 || current_key_num == KEY_4) // 按键3/4：修改阈值
                        {
                            ThresholdSettings(curr_count_s); // 执行阈值修改逻辑
													if(curr_count_s == 5)
                {
                    SettingsThresholdDisplay2(); // 第二页（防盗模式）
                }
                else
                {
                    SettingsThresholdDisplay1(); // 第一页（温/湿/CO2/烟雾）
                }
                            
                            OLED_Refresh(); // 刷新显示
                            KeyNum = 0; // 清空按键标识
                        }
                        else if (current_key_num == KEY_1) // 按键1短按：返回自动模式
                        {
                            mode = AUTO_MODE;
                            is_threshold_page_inited = 0; // 重置页面初始化标志
													 last_count_s = 1;             // 重置上一次页码
                            // 将修改后的阈值写入Flash（掉电保存）
                        FLASH_W(FLASH_START_ADDR, Sensorthreshold.tempValue, Sensorthreshold.humiValue,
                                    Sensorthreshold.CO2Value, Sensorthreshold.anti_theft_mode,Sensorthreshold.SmogeValue);
                           KeyNum = 0; // 清空按键标识
                        }
                    }
                }
                
                // 正常显示逻辑（一级菜单）
                if (is_secondary_menu == 1)
                {
                    // 二级菜单显示（暂未实现）
                 }
                else
                {
                    // 一级菜单显示
                    if (curr_count_s >= 1 && curr_count_s <= 5) // 页码在有效范围内
                    {
                        if (is_threshold_page_inited == 0) // 页面未初始化
                        {
													if(curr_count_s == 5)
                {
                    OLED_settingsPage2(); // 页码5→绘制第二页（防盗模式）
                }
                else
                {
                    OLED_settingsPage1(); // 页码1-4→绘制第一页
                }
                           
                            is_threshold_page_inited = 1; // 标记页面已初始化
                        }
                    }
                    OLED_settingsOption(curr_count_s); // 显示当前光标
                    // 根据页码显示对应阈值
                if(curr_count_s == 5)
                {
                  SettingsThresholdDisplay2(); // 第二页阈值（防盗模式）
                 }
                 else
                {
                   SettingsThresholdDisplay1(); // 第一页阈值（温/湿/CO2/烟雾）
                 }
                }
                  break;
            }
        }
        
        // ==================== 控制OLED显示刷新频率（每50ms刷新一次）====================
        if(current_time - last_display_time > 25) // 25个定时器2滴答 = 25*2ms=50ms
        {
            OLED_Refresh(); // 刷新OLED显示
            last_display_time = current_time; // 更新显示刷新时间戳
        }

				
    }
}


