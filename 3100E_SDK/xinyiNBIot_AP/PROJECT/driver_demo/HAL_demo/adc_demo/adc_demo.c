/**
* @file   adc_demo.c
* @note   在这个DEMO中，每隔一秒使用ADC先获取芯片温度，VBAT,再获取HAL_ADC_GPIO9脚上的电压值，并将获取到的结果通过串口打印出来。
*
*************************************************************************************************************************/
#include "hal_adc.h"
#include "xy_printf.h"
#include "at_uart.h"
#include "hal_def.h"
#include "runtime_dbg.h"

#if 1
/**
 * @brief 	demo主函数。
 * @note	用户使用xy_printf需要将XY_DEBUG宏置为1,详情参考xy_printf.c。
 */
__RAM_FUNC int main(void)
{
	int16_t voltage9,temprature,vbat,voltageADC2;
	HAL_ADC_HandleTypeDef adc_single = {0};

	SystemInit();

	Debug_Runtime_Init();

	while (1)
	{

		Debug_Runtime_Add("ADC voltage9 start");	
		adc_single.Channel= HAL_ADC_GPIO9;	
		voltage9 = HAL_ADC_GetValue(&adc_single);
		Debug_Runtime_Add("ADC voltage9 stop temprature start");  //根据此标签可以获取GPIO9通道的转换时间
		adc_single.Channel= HAL_ADC_TEMPRATURE;
		temprature = HAL_ADC_GetValue(&adc_single);
		Debug_Runtime_Add("ADC temprature stop ADC vbat start");  //根据此标签可以获取TEMPRATURE通道的转换时间		
		adc_single.Channel= HAL_ADC_VBAT;
		vbat = HAL_ADC_GetValue(&adc_single);
		Debug_Runtime_Add("ADC vbat stop ADC2 start");						  //根据此标签可以获取VBAT通道的转换时间	
		adc_single.Channel= HAL_ADC_AUX_ADC2;
		voltageADC2 = HAL_ADC_GetValue(&adc_single);
		Debug_Runtime_Add("ADC ADC2 stop");						  //根据此标签可以获取VBAT通道的转换时间		

		xy_printf("\r\nTemprature Value: %d\r\n\r\nVbat Value: %d\r\n\
			\r\nADC_GPIO9 Voltage Value: %d\r\n\r\nADC_ADC2 Voltage Value: %d\r\n",\
			temprature,vbat, voltage9,voltageADC2);
	
		HAL_Delay(1000);
	}

	return 0;  
}

#else

__RAM_FUNC int main(void)
{
	SystemInit();
	Debug_Runtime_Init();

    //ADC SINGLE测试
	int16_t voltage9,temprature,vbat,voltageADC2;
	HAL_ADC_HandleTypeDef adc_single = {0};

    Debug_Runtime_Add("----Single Start----");	
    adc_single.Channel= HAL_ADC_GPIO9;	
    voltage9 = HAL_ADC_GetValue(&adc_single);
    Debug_Runtime_Add("GPIO9");

    adc_single.Channel= HAL_ADC_TEMPRATURE;
    temprature = HAL_ADC_GetValue(&adc_single);
    Debug_Runtime_Add("TEMP");

    adc_single.Channel= HAL_ADC_VBAT;
    vbat = HAL_ADC_GetValue(&adc_single);
    Debug_Runtime_Add("VBAT");	

    adc_single.Channel= HAL_ADC_AUX_ADC2;
    voltageADC2 = HAL_ADC_GetValue(&adc_single);
    Debug_Runtime_Add("ADC2");
    Debug_Runtime_Add("----Single Stop----");

    //ADC SCAN测试
    HAL_ADC_Scan_HandleTypeDef adc_scan = {0};
    HAL_ADC_Scan_ChannelTypeDef ADC_Scan_Channel[ADC_MAX_CHANNELNUM];
    ADC_Scan_Channel[0].Channel = HAL_ADC_TEMPRATURE;
	ADC_Scan_Channel[1].Channel = HAL_ADC_VBAT;
	ADC_Scan_Channel[2].Channel = HAL_ADC_GPIO9;
	ADC_Scan_Channel[3].Channel = HAL_ADC_GPIO10;
	ADC_Scan_Channel[4].Channel = HAL_ADC_AUX_ADC2;
    
    Debug_Runtime_Add("----Scan Start----");	
    adc_scan.Scan_NbrsChn = 1;
    adc_scan.Scan_channel = ADC_Scan_Channel;	
    HAL_ADC_GetValue_2(&adc_scan);
    Debug_Runtime_Add("1, TEMP");

    adc_scan.Scan_NbrsChn = 2;
    adc_scan.Scan_channel = ADC_Scan_Channel;	
    HAL_ADC_GetValue_2(&adc_scan);
    Debug_Runtime_Add("2, TEMP VBAT");

    adc_scan.Scan_NbrsChn = 3;
    adc_scan.Scan_channel = ADC_Scan_Channel;	
    HAL_ADC_GetValue_2(&adc_scan);
    Debug_Runtime_Add("3, TEMP VBAT GPIO9");

    adc_scan.Scan_NbrsChn = 4;
    adc_scan.Scan_channel = ADC_Scan_Channel;	
    HAL_ADC_GetValue_2(&adc_scan);
    Debug_Runtime_Add("4, TEMP VBAT GPIO9 GPIO10");

    adc_scan.Scan_NbrsChn = 5;
    adc_scan.Scan_channel = ADC_Scan_Channel;	
    HAL_ADC_GetValue_2(&adc_scan);
    Debug_Runtime_Add("5, TEMP VBAT GPIO9 GPIO10 ADC2");
    Debug_Runtime_Add("----Scan Stop----");

    while(1);

    return 0;  
}

#endif
