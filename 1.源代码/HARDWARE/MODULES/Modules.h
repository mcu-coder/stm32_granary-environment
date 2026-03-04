#ifndef	__MODULES_H_
#define __MODULES_H_

#include "stm32f10x.h"                  // Device header

#include "adcx.h"

typedef enum
{
    AUTO_MODE = 1,    // 自动模式
    MANUAL_MODE,      // 手动模式（值=2）
    SETTINGS_MODE     // 设置模式（值=3）
} MODE_PAGES;  // 枚举类型名，所有文件可直接使用

typedef struct
{

    uint8_t humi;
    uint8_t temp;
    uint16_t lux;	
    uint16_t soilHumi;
    uint16_t Smoge;	
    uint16_t AQI;
    uint16_t CO2;
    uint16_t hPa;
    uint8_t distance;
	  uint16_t fire;
	  uint16_t value;

} SensorModules;
typedef struct
{
	//float tempValue;
	uint8_t humiValue;
	uint8_t tempValue;
	uint16_t luxValue;	
	uint16_t soilHumiValue;
	uint16_t CO2Value;	
	uint16_t AQIValue;
	uint16_t hPaValue;
	uint16_t SmogeValue;

	uint8_t distanceValue;
  uint8_t anti_theft_mode;// 防盗模式：0=关，1=开
	
}SensorThresholdValue;

typedef struct
{
  //uint8_t	NOW_LED_Flag;
	uint8_t LED_Flag;
	uint8_t BEEP_Flag;
	uint8_t NOW_Curtain_Flag;
	uint8_t Curtain_Flag;	
	uint8_t NOW_Window_Flag;
	uint8_t Window_Flag;	
	uint8_t Fan_Flag;
	uint8_t Humidifier_Flag;
	uint8_t Bump_Flag;
  uint8_t RELAY_Flag;
	uint8_t MOTOR_Flag;
}DriveModules;
 	// 初始化驱动数据（全局变量）


extern SensorModules sensorData;			//声明传感器模块的结构体变量
extern SensorThresholdValue Sensorthreshold;	//声明传感器阈值结构体变量
extern DriveModules driveData;				//声明驱动器状态的结构体变量
void SensorScan(void);
#endif


