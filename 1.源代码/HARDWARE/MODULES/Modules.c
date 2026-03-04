#include "Modules.h"
#include "sgp30.h"
#include "IR.h"
#include "HW.h"
#include "dht11.h"
#include "delay.h"
#include <stdlib.h>
#include "stdio.h"
#include "adcx.h" 

/************************** 全局变量定义 **************************/
SensorModules sensorData = {0};
SensorThresholdValue Sensorthreshold = {0};
DriveModules driveData = {0};
uint16_t co2_value = 0;						  	//co2值
u16 value;
u16 fire;
 
/************************** 传感器总扫描函数 **************************/
void SensorScan(void)
{
    uint32_t current_time = delay_get_tick();
    //温湿度
    DHT11_Read_Data(&sensorData.temp, &sensorData.humi);
//	
    //烟雾
    static uint8_t adc_init_flag = 0;
    if(adc_init_flag == 0)
    {
        ADCX_Init(); // 确保ADC初始化
        adc_init_flag = 1;
    }
    sensorData.Smoge = MQ2_GetData_PPM();
	
    sensorData.value=HW_GetData();
// printf("sensorData.value：%d\r\n", sensorData.value); 
	
    //火焰
    sensorData.fire = IR_FireData();
//	 printf("sensorData.fire：%d\r\n", sensorData.fire); 
    //二氧化碳（5次均值滤波）
    static u16 co2_buf[5] = {0};  // 静态数组：缓存最近5次CO?原始数据
    static u8 co2_index = 0;       // 静态索引：记录当前存储位置（0-4循环）
    u16 co2_raw = 0;               // 单次读取的原始CO?值
    u32 co2_sum = 0;               // 5次数据总和
    u8 i = 0;

    // 1. 读取单次原始CO?值
    Sgp30_Get_Co2_Value(&co2_raw);

    // 2. 异常值处理：无效值替换为上次有效值（避免拉低/拉高平均值）
    if (co2_raw == 0 || co2_raw > 5000)
    {
        // 若当前原始值异常，用数组中最后一次有效值替代（首次全0则赋值400，室内基准CO?浓度）
        co2_raw = (co2_buf[co2_index] == 0) ? 400 : co2_buf[co2_index];
    }

    // 3. 将有效原始值存入缓存数组
    co2_buf[co2_index] = co2_raw;

    // 4. 索引循环（0→1→2→3→4→0）
    co2_index = (co2_index + 1) % 5;

    // 5. 计算5次数据的平均值
    for (i = 0; i < 5; i++)
    {
        co2_sum += co2_buf[i];
    }
    sensorData.CO2 = co2_sum / 5;  // 均值赋值给传感器结构体

    // 兜底：确保最终值在合理范围（避免极端情况）
    if (sensorData.CO2 == 0 || sensorData.CO2 > 5000)
    {
        sensorData.CO2 = 0;
    }
}

