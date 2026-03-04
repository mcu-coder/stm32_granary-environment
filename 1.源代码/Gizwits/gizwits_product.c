
#include <stdio.h>
#include <string.h>
#include "gizwits_product.h"
#include "usart2.h"
#include "usart.h"
#include "Modules.h"
#include "oled.h"

#define FLASH_START_ADDR	0x0801f000	// 与 main.c 中的地址保持统一

void FLASH_W(u32 add,u16 dat,u16 dat2,u16 dat3,u16 dat4,u16 dat5);

void OLED_autoPage1(void);    // 自动模式标题绘制函数声明
void OLED_manualPage1(void);  // 手动模式标题绘制函数声明

extern SensorModules sensorData;								//声明传感器数据结构体变量
extern SensorThresholdValue Sensorthreshold;		//声明传感器阈值结构体变量（已存在，无需新增）
extern DriveModules driveData;									//声明驱动器状态结构体变量
extern uint8_t mode;                  // 全局模式变量（1=自动，2=手动，3=设置）
extern uint8_t count_m;               // 手动模式计数
extern uint8_t last_mode;             


static uint32_t timerMsCount;

/** Current datapoint */
dataPoint_t currentDataPoint;

/**@} */
/**@name Gizwits User Interface
* @{
*/

/**
* @brief Event handling interface

* Description:

* 1. Users can customize the changes in WiFi module status

* 2. Users can add data points in the function of event processing logic, such as calling the relevant hardware peripherals operating interface

* @param [in] info: event queue
* @param [in] data: protocol data
* @param [in] len: protocol data length
* @return NULL
* @ref gizwits_protocol.h
*/
int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *gizdata, uint32_t len)
{
    uint8_t i = 0;
    dataPoint_t *dataPointPtr = (dataPoint_t *)gizdata;
    moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)gizdata;
    protocolTime_t *ptime = (protocolTime_t *)gizdata;
    
#if MODULE_TYPE
    gprsInfo_t *gprsInfoData = (gprsInfo_t *)gizdata;
#else
    moduleInfo_t *ptModuleInfo = (moduleInfo_t *)gizdata;
#endif

    if((NULL == info) || (NULL == gizdata))
    {
        return -1;
    }

    for(i=0; i<info->num; i++)
    {
        switch(info->event[i])
        {
        case EVENT_LED:
            currentDataPoint.valueLED = dataPointPtr->valueLED;
            GIZWITS_LOG("Evt: EVENT_LED %d \n", currentDataPoint.valueLED);
            if(0x01 == currentDataPoint.valueLED)
            {
            driveData.LED_Flag = 1;
           }
         else
          {
        driveData.LED_Flag = 0;
            }
            break;
        case EVENT_BEEP:
            currentDataPoint.valueBEEP = dataPointPtr->valueBEEP;
            GIZWITS_LOG("Evt: EVENT_BEEP %d \n", currentDataPoint.valueBEEP);
            if(0x01 == currentDataPoint.valueBEEP)
            {
                driveData.BEEP_Flag =1;
           }
         else
          {
        driveData.BEEP_Flag =0;
            }
            break;
        case EVENT_MOTOR:
            currentDataPoint.valueMOTOR = dataPointPtr->valueMOTOR;
            GIZWITS_LOG("Evt: EVENT_MOTOR %d \n", currentDataPoint.valueMOTOR);
            if(0x01 == currentDataPoint.valueMOTOR)
            {
            driveData.MOTOR_Flag = 1;
             }
           else
           {
        driveData.MOTOR_Flag  = 0 ;
            }
            break;
        case EVENT_RELAY:
            currentDataPoint.valueRELAY = dataPointPtr->valueRELAY;
            GIZWITS_LOG("Evt: EVENT_RELAY %d \n", currentDataPoint.valueRELAY);
            if(0x01 == currentDataPoint.valueRELAY)
            {
              driveData.RELAY_Flag = 1;
    }
    else
    {
        driveData.RELAY_Flag = 0 ;
            }
            break;

        case EVENT_mode:
            currentDataPoint.valuemode = dataPointPtr->valuemode;
            GIZWITS_LOG("Evt: EVENT_mode %d\n", currentDataPoint.valuemode);
            switch(currentDataPoint.valuemode)
    {
        case mode_VALUE0:  // 云端下发「自动模式」指令
            mode = AUTO_MODE;
            last_mode = mode; // 同步last_mode，避免主循环重复判断
            OLED_Clear();     // 清屏（清除手动模式残留）
            OLED_autoPage1(); // 绘制自动模式标题（温度/湿度/烟雾等文字）
            OLED_Refresh();   // 强制刷新到屏幕，标题立即显示
            count_m = 1;
            break;
        case mode_VALUE1:  // 云端下发「手动模式」指令
            mode = MANUAL_MODE;
            last_mode = mode;
            OLED_Clear();
            OLED_manualPage1(); // 绘制手动模式标题（灯光/蜂鸣器等文字）
            OLED_Refresh();
            count_m = 1;
            driveData.LED_Flag = 0;
            driveData.BEEP_Flag = 0;
            driveData.RELAY_Flag = 0;
            break;
        default:
            break;
    }
            break;
        case EVENT_anti_theft_mode:
				{
            currentDataPoint.valueanti_theft_mode = dataPointPtr->valueanti_theft_mode;
            GIZWITS_LOG("Evt: EVENT_anti_theft_mode %d\n", currentDataPoint.valueanti_theft_mode);
             // 1. 合法性校验
    uint8_t new_anti_theft = currentDataPoint.valueanti_theft_mode;
    if (new_anti_theft > 1) {
        new_anti_theft = 0; 
        currentDataPoint.valueanti_theft_mode = 0; 
    }
    // 2. 更新本地变量
    Sensorthreshold.anti_theft_mode = new_anti_theft;
    // 3. 按读取顺序写入Flash：temp, humi, CO2, 烟雾, 防盗
    FLASH_W(FLASH_START_ADDR, 
            Sensorthreshold.tempValue,        // dat  +0 温度
            Sensorthreshold.humiValue,        // dat2 +2 湿度
            Sensorthreshold.CO2Value,         // dat3 +4 CO2
            Sensorthreshold.SmogeValue,       // dat4 +6 烟雾
            new_anti_theft);                  // dat5 +8 防盗模式（关键！）
    // 恢复日志验证
   // GIZWITS_LOG("Anti-theft mode updated to: %d (saved to Flash)\n", new_anti_theft);
}
break;
    case EVENT_tempValue:
{  
    currentDataPoint.valuetempValue = dataPointPtr->valuetempValue;
    GIZWITS_LOG("Evt:EVENT_tempValue %d\n",currentDataPoint.valuetempValue);
    uint8_t new_temp = currentDataPoint.valuetempValue;  
    if (new_temp < 20) new_temp = 20;
    if (new_temp > 40) new_temp = 40;
    Sensorthreshold.tempValue = new_temp;
    // 按顺序写入Flash
    FLASH_W(FLASH_START_ADDR, 
            new_temp,                        // dat  +0 温度（新值）
            Sensorthreshold.humiValue,       // dat2 +2 湿度
            Sensorthreshold.CO2Value,        // dat3 +4 CO2
            Sensorthreshold.SmogeValue,      // dat4 +6 烟雾
            Sensorthreshold.anti_theft_mode); // dat5 +8 防盗
    break;
}
				
       case EVENT_humiValue:
{   
    currentDataPoint.valuehumiValue = dataPointPtr->valuehumiValue;
    GIZWITS_LOG("Evt:EVENT_humiValue %d\n",currentDataPoint.valuehumiValue);
    uint8_t new_humi = currentDataPoint.valuehumiValue;  
    if (new_humi < 30) new_humi = 30;
    if (new_humi > 70) new_humi = 70;
    Sensorthreshold.humiValue = new_humi;
    // 按顺序写入Flash
    FLASH_W(FLASH_START_ADDR, 
            Sensorthreshold.tempValue,       // dat  +0 温度
            new_humi,                        // dat2 +2 湿度（新值）
            Sensorthreshold.CO2Value,        // dat3 +4 CO2
            Sensorthreshold.SmogeValue,      // dat4 +6 烟雾
            Sensorthreshold.anti_theft_mode); // dat5 +8 防盗
    break;
}
				
     case EVENT_SmogeValue:
{  
    currentDataPoint.valueSmogeValue = dataPointPtr->valueSmogeValue;
    GIZWITS_LOG("Evt:EVENT_SmogeValue %d\n",currentDataPoint.valueSmogeValue);
    uint16_t new_smoge = currentDataPoint.valueSmogeValue;  
    if (new_smoge < 10) new_smoge = 10;
    if (new_smoge > 150) new_smoge = 150;
    Sensorthreshold.SmogeValue = new_smoge;
    // 按顺序写入Flash
    FLASH_W(FLASH_START_ADDR, 
            Sensorthreshold.tempValue,       // dat  +0 温度
            Sensorthreshold.humiValue,       // dat2 +2 湿度
            Sensorthreshold.CO2Value,        // dat3 +4 CO2
            new_smoge,                       // dat4 +6 烟雾（新值）
            Sensorthreshold.anti_theft_mode); // dat5 +8 防盗
    break;
}
        case EVENT_CO2Value:
				{
            currentDataPoint.valueCO2Value = dataPointPtr->valueCO2Value;
            GIZWITS_LOG("Evt:EVENT_CO2Value %d\n",currentDataPoint.valueCO2Value);
           uint16_t new_CO2 = currentDataPoint.valueCO2Value;  
    if (new_CO2 < 500) new_CO2 = 500;
    if (new_CO2 > 2000) new_CO2 = 2000;
    Sensorthreshold.CO2Value = new_CO2;
    // 按顺序写入Flash
    FLASH_W(FLASH_START_ADDR, 
            Sensorthreshold.tempValue,       // dat  +0 温度
            Sensorthreshold.humiValue,       // dat2 +2 湿度
            new_CO2,                         // dat3 +4 CO2（新值）
            Sensorthreshold.SmogeValue,      // dat4 +6 烟雾
            Sensorthreshold.anti_theft_mode); // dat5 +8 防盗
    break;
}

        case WIFI_SOFTAP:
            break;
        case WIFI_AIRLINK:
            break;
        case WIFI_STATION:
            break;
        case WIFI_CON_ROUTER:
 
            break;
        case WIFI_DISCON_ROUTER:
 
            break;
        case WIFI_CON_M2M:
 
            break;
        case WIFI_DISCON_M2M:
            break;
        case WIFI_RSSI:
            GIZWITS_LOG("RSSI %d\n", wifiData->rssi);
            break;
        case TRANSPARENT_DATA:
            GIZWITS_LOG("TRANSPARENT_DATA \n");
            //user handle , Fetch data from [data] , size is [len]
            break;
        case WIFI_NTP:
            GIZWITS_LOG("WIFI_NTP : [%d-%d-%d %02d:%02d:%02d][%d] \n",ptime->year,ptime->month,ptime->day,ptime->hour,ptime->minute,ptime->second,ptime->ntp);
            break;
        case MODULE_INFO:
            GIZWITS_LOG("MODULE INFO ...\n");
#if MODULE_TYPE
            GIZWITS_LOG("GPRS MODULE ...\n");
            //Format By gprsInfo_t
            GIZWITS_LOG("moduleType : [%d] \n",gprsInfoData->Type);
#else
            GIZWITS_LOG("WIF MODULE ...\n");
            //Format By moduleInfo_t
            GIZWITS_LOG("moduleType : [%d] \n",ptModuleInfo->moduleType);
#endif
        break;
        default:
            break;
        }
    }

    return 0;
}

/**
* User data acquisition

* Here users need to achieve in addition to data points other than the collection of data collection, can be self-defined acquisition frequency and design data filtering algorithm

* @param none
* @return none
*/
void userHandle(void)
{
  currentDataPoint.valuefire =  sensorData.fire;//Add Sensor Data Collection
    currentDataPoint.valuevalue =sensorData.value;//Add Sensor Data Collection
    currentDataPoint.valuetemp = sensorData.temp;//Add Sensor Data Collection
    currentDataPoint.valuehumi = sensorData.humi;//Add Sensor Data Collection
    currentDataPoint.valuesmoge =sensorData.Smoge ;//Add Sensor Data Collection
    currentDataPoint.valueCO2 = sensorData.CO2;//Add Sensor Data Collection
    
	
		currentDataPoint.valuetempValue = Sensorthreshold.tempValue;       // 温度阈值（从Flash读取后的值）
    currentDataPoint.valuehumiValue = Sensorthreshold.humiValue;
    currentDataPoint.valueSmogeValue = Sensorthreshold.SmogeValue;
	  currentDataPoint.valueCO2Value = Sensorthreshold.CO2Value;
	  currentDataPoint.valueanti_theft_mode = Sensorthreshold.anti_theft_mode;
	
	
	//  驱动器（蜂鸣器、LED、继电器）状态同步（核心：手动/自动模式均生效）
    currentDataPoint.valueBEEP = driveData.BEEP_Flag;
    currentDataPoint.valueLED = driveData.LED_Flag;
    currentDataPoint.valueRELAY = driveData.RELAY_Flag;
	  currentDataPoint.valueMOTOR = driveData.MOTOR_Flag;

	
	if(mode == AUTO_MODE)
    {
        currentDataPoint.valuemode = mode_VALUE0;  // 自动→对应云端mode_VALUE0
    }
    else if(mode == MANUAL_MODE)
    {
        currentDataPoint.valuemode = mode_VALUE1;  // 手动→对应云端mode_VALUE1
    }
}

/**
* Data point initialization function

* In the function to complete the initial user-related data
* @param none
* @return none
* @note The developer can add a data point state initialization value within this function
*/
void userInit(void)
{
  memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));
    
    /** Warning !!! DataPoint Variables Init , Must Within The Data Range **/ 
  
    currentDataPoint.valueLED = 0;
    currentDataPoint.valueBEEP = 0;
    currentDataPoint.valueMOTOR = 0;
    currentDataPoint.valueRELAY = 0;
    currentDataPoint.valuefire = 1;
    currentDataPoint.valuevalue =0 ;
    currentDataPoint.valuemode = mode_VALUE0;
    currentDataPoint.valueanti_theft_mode = 0;
    currentDataPoint.valuetemp = 0;
    currentDataPoint.valuehumi = 0;
    currentDataPoint.valuetempValue = Sensorthreshold.tempValue ;
    currentDataPoint.valuehumiValue = Sensorthreshold.humiValue;
    currentDataPoint.valueSmogeValue =  Sensorthreshold.SmogeValue;
    currentDataPoint.valueCO2Value =  Sensorthreshold.CO2Value;
    currentDataPoint.valuesmoge = 0 ;
    currentDataPoint.valueCO2 = 0 ;
   
	   printf("机智云数据点初始化完成\n");
}


/**
* @brief  gizTimerMs

* millisecond timer maintenance function ,Millisecond increment , Overflow to zero

* @param none
* @return none
*/
void gizTimerMs(void)
{
    timerMsCount++;
}

/**
* @brief gizGetTimerCount

* Read system time, millisecond timer

* @param none
* @return System time millisecond
*/
uint32_t gizGetTimerCount(void)
{
    return timerMsCount;
}

/**
* @brief mcuRestart

* MCU Reset function

* @param none
* @return none
*/
void mcuRestart(void)
{
__set_FAULTMASK(1);
   NVIC_SystemReset();
}
/**@} */

/**
* @brief TIMER_IRQ_FUN

* Timer Interrupt handler function

* @param none
* @return none
*/
void TIMER_IRQ_FUN(void)
{
  gizTimerMs();
}

/**
* @brief UART_IRQ_FUN

* UART Serial interrupt function ，For Module communication

* Used to receive serial port protocol data between WiFi module

* @param none
* @return none
*/
void UART_IRQ_FUN(void)
{
 uint8_t value = 0;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  // 增加接收中断判断（避免误触发）
  {
      value = USART_ReceiveData(USART2);  // 取消注释，接收云端数据
      gizPutData(&value, 1);              // 传入机智云协议栈解析
		 USART_ClearITPendingBit(USART2, USART_IT_RXNE); // 新增：清除中断标志位（避免漏收）
  }
}


/**
* @brief uartWrite

* Serial write operation, send data to the WiFi module

* @param buf      : Data address
* @param len       : Data length
*
* @return : Not 0,Serial send success;
*           -1，Input Param Illegal
*/
int32_t uartWrite(uint8_t *buf, uint32_t len)
{
    uint32_t i = 0;
    
    if(NULL == buf)
    {
        return -1;
    }
    
    #ifdef PROTOCOL_DEBUG
   // GIZWITS_LOG("MCU2WiFi[%4d:%4d]: ", gizGetTimerCount(), len);
    for(i=0; i<len; i++)
    {
      //  GIZWITS_LOG("%02x ", buf[i]);
    }
   // GIZWITS_LOG("\n");
    #endif

    for(i=0; i<len; i++)
    {
        USART_SendData(USART2, buf[i]);//STM32 test demo
			while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        //Serial port to achieve the function, the buf[i] sent to the module
        if(i >=2 && buf[i] == 0xFF)
        {
					USART_SendData(USART2,0x55);
          while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
          //Serial port to achieve the function, the 0x55 sent to the module
          //USART_SendData(UART, 0x55);//STM32 test demo
        }
    }

    
    return len;
}




    
