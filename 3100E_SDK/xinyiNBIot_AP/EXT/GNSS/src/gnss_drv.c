/** 
* @file        
* @brief   该源文件为GNSS芯片底层驱动接口，负责控制GNSS芯片，并收发数据
* @warning     
*/
#include "xy_system.h"
#include "xy_printf.h"
#include "urc_process.h"
#include "xy_cp.h"
#include "xy_lpm.h"
#include "xy_at_api.h"
#include "xy_memmap.h"
#include "sys_mem.h"
#include "hal_gpio.h"
#include "hal_csp.h"
#include "gnss_api.h"
#include "gnss_msg.h"
#include "csp.h"

#define GNSS_RX_PIN GPIO_PAD_NUM_21     // GPIO_PAD_NUM_24
#define GNSS_TX_PIN GPIO_PAD_NUM_20     // GPIO_PAD_NUM_14
#define GNSS_PRRSTX GPIO_PAD_NUM_12     // GPIO_PAD_NUM_21
#define GNSS_PRTRG  GPIO_PAD_NUM_23
#define GNSS_WKP_PIN MCU_WKP2
#define GNSS_CLOCK_POWER MCU_WKP3
#define GNSS_BAK_POWER_PIN GPIO_PAD_NUM_6

uint8_t *g_gnss_data;

HAL_CSP_HandleTypeDef *gnss_uart_handl = NULL;

/**
 * @brief 错误中断回调函数，用户在使用时可自行添加中断处理函数.
 */
__RAM_FUNC void HAL_CSP2_ErrorCallback(HAL_CSP_HandleTypeDef *hcsp)
{
	xy_printf("CSP2 ERR:%d",hcsp->ErrorCode);
	hcsp->ErrorCode = HAL_CSP_ERROR_NONE;
}

/**
 * @brief 接收完成回调函数，用户在使用时可自行添加中断处理函数.
 */
__RAM_FUNC void HAL_CSP2_RxCpltCallback(HAL_CSP_HandleTypeDef *hcsp)
{
    (void)hcsp;
    send_msg_to_mainctl(GNSS_STREAM,(void *)g_gnss_data, gnss_uart_handl->RxXferCount);
	HAL_CSP_Receive_IT(gnss_uart_handl, g_gnss_data, GNSS_RCV_MAX_LEN, GNSS_UART_MAX_TIMEOUT); //继续下一次接收
}


/**
 * @brief CSP_UART初始化函数，主要完成以下配置：
 * 		  1. CSP_UART的TXD、RXD引脚的GPIO初始化配置
 * 		  2. CSP_UART的波特率、位宽、奇偶检验模式、停止位
 */
void GNSS_UART_Init()
{
	if(gnss_uart_handl == NULL)
	{
		gnss_uart_handl = xy_malloc(sizeof(HAL_CSP_HandleTypeDef));
		memset(gnss_uart_handl, 0, sizeof(HAL_CSP_HandleTypeDef));
	}

	if(g_gnss_data == NULL)
	{
		g_gnss_data = xy_malloc(GNSS_RCV_MAX_LEN);
		memset(g_gnss_data, 0, GNSS_RCV_MAX_LEN);
	}

	
    //gpio初始化
    HAL_GPIO_InitTypeDef gpio_init = {0};

	gpio_init.Pin = GNSS_TX_PIN;
	gpio_init.Mode = GPIO_MODE_HW_PER;
	gpio_init.PinRemap = GPIO_CSP2_TXD;
	HAL_GPIO_Init(&gpio_init);

	gpio_init.Pin = GNSS_RX_PIN;
	gpio_init.Mode = GPIO_MODE_HW_PER;
	gpio_init.PinRemap = GPIO_CSP2_RXD;
	HAL_GPIO_Init(&gpio_init);

	//初始化CSP为UART
	gnss_uart_handl->Instance = HAL_CSP2;
	gnss_uart_handl->CSP_UART_Init.BaudRate = HAL_CSP_UART_BAUD_9600;
	gnss_uart_handl->CSP_UART_Init.WordLength = HAL_CSP_UART_WORDLENGTH_8;
	gnss_uart_handl->CSP_UART_Init.Parity = HAL_CSP_UART_PARITY_NONE;
	gnss_uart_handl->CSP_UART_Init.StopBits = HAL_CSP_UART_STOPBITS_1;
	HAL_CSP_UART_Init(gnss_uart_handl);

	HAL_CSP_Receive_IT(gnss_uart_handl, g_gnss_data, GNSS_RCV_MAX_LEN, GNSS_UART_MAX_TIMEOUT);
}

/**
 * @brief CSP_UART去初始化函数
 */
void GNSS_UART_DeInit()
{
	CSP_IntDisable(gnss_uart_handl->Instance, CSP_INT_ALL);//关中断清标记位，防止没有gnss版本的时候误触发中断
    CSP_IntClear(gnss_uart_handl->Instance, CSP_INT_ALL);

	if(gnss_uart_handl != NULL)
	{
		HAL_CSP_DeInit(gnss_uart_handl);
		xy_free(gnss_uart_handl);
		gnss_uart_handl = NULL;
	}
	else
		return;

	if(g_gnss_data != NULL)
	{
		xy_free(g_gnss_data);
		g_gnss_data = NULL;
	}

	McuGpioModeSet(GNSS_TX_PIN, 0x00), McuGpioWrite(GNSS_TX_PIN, 0);
	McuGpioModeSet(GNSS_RX_PIN, 0x00), McuGpioWrite(GNSS_RX_PIN, 0);
}


/**
 * @brief 向gnss对应串口发送数据
 * 		  
 */
int write_to_gnss(uint8_t *buf, int size)
{
	HAL_CSP_Transmit(gnss_uart_handl, (uint8_t *)buf, (uint16_t)size, HAL_MAX_DELAY);
    return 0;
}

/**
 * @brief 切换gnss对应串口波特率
 * 		  
 */
void gnss_uartbaud_change(uint32_t baud)
{
	//根据波特率参数配置波特率
	CSP_UARTModeSet(HAL_CSP2, GetPCLK2Freq(), baud, HAL_CSP_UART_WORDLENGTH_8, HAL_CSP_UART_PARITY_NONE, HAL_CSP_UART_STOPBITS_1);
	//delay 110us
	for(volatile int delay=0; delay<25; delay++);
    
}

/**
 * @brief 初始化gnss主供电和26M时钟
 * 		  
 */
__RAM_FUNC void gnss_main_power_init()
{	
	//输出26M时钟配置：
	HWREGB(0x4000485A) |= 0xC;
	HWREGB(0x40004859) &= ~0x40;
	HWREGB(0x40004859) |= 0x80;
	HWREGB(0x4000485A) &= ~0x3;
	HWREGB(0x4000485A) &= ~0x20;

	//给GNSS RST引脚拉低
	McuGpioModeSet(GNSS_PRRSTX, 0x00), McuGpioWrite(GNSS_PRRSTX, 0);

	//给GNSS供电
	HAL_Delay(10);
	HWREGB(0x40004044) |= 0x02;  //vddio_out_user1

	//给GNSS RST引脚拉高
	HAL_Delay(10);
	McuGpioModeSet(GNSS_PRRSTX, 0x00), McuGpioWrite(GNSS_PRRSTX, 1);
}

/**
 * @brief 去初始化gnss主供电和26M时钟
 * 
 */
void gnss_main_power_deinit()
{
	//GNSS供电
	HWREGB(0x40004044) &= ~0x02;  //vddio_out_user1

	//给GNSS RST设为输入拉低
	HAL_Delay(10);
	McuGpioModeSet(GNSS_PRRSTX, 0x13);

	//输出26M时钟配置：
	HWREGB(0x40004859) |= 0x40;
	HWREGB(0x40004859) &= ~0x80;
	HWREGB(0x4000485A) |= 0x20;
}


/**
 * @brief 初始化gnss供电和时钟
 * 		  
 */
void gnss_pin_reset()
{
	//给GNSS RST引脚拉低
	McuGpioModeSet(GNSS_PRRSTX, 0x00), McuGpioWrite(GNSS_PRRSTX, 0);
	HAL_Delay(100);
	McuGpioWrite(GNSS_PRRSTX, 1);
}

/**
 * @brief gnss备电供电开关设置
 * 		  
 */
static uint8_t g_gnss_bak_power_select = 0; // 0：内部io供电，1：外部用户供电
__RAM_FUNC void gnss_bak_power_set(uint8_t value)
{
    if (g_gnss_bak_power_select == 0)
    {
        //GPIO6是给GNSS备电供电用
        McuGpioModeSet(GNSS_BAK_POWER_PIN, 0x00), McuGpioWrite(GNSS_BAK_POWER_PIN, value);
    }
}

/**
 * @brief 初始化gnss供电和时钟
 * 		  
 */
void gnss_power_clock_init()
{	
	//使用_32K_CLK_MODENV配置时钟源为XTAL32K
	//输出XTAL32K配置：
	HWREGB(0x4000080E) |= 0x10;
	HWREGB(0x4000080D) |= 0x1;

	//给GNSS RST引脚拉低
	McuGpioModeSet(GNSS_PRRSTX, 0x00), McuGpioWrite(GNSS_PRRSTX, 0);

	//GPIO6是给GNSS备电供电用
	gnss_bak_power_set(1);

	gnss_main_power_init();
}

/**
 * @brief 去初始化gnss供电和时钟
 * 		  
 */
__RAM_FUNC void gnss_power_clock_deinit()
{
	//下电时序要求延迟500ms才能断电
	HAL_Delay(500);

	//GPIO6备电下电
	gnss_bak_power_set(0);

	gnss_main_power_deinit();

	//输出XTAL32K配置：
	HWREGB(0x4000080E) &= ~0x10;
}

/**
 * @brief gnss初始化函数
 * 		  
 */
extern void gnss_write_hex_stream(char *hex_str);
static uint8_t g_gnss_cold_start = 0;
void gnss_system_init()
{
	if((g_gnss_cold_start != 0) || (Get_Boot_Reason() != WAKEUP_DSLEEP))
	{
		gnss_power_clock_init();
		HAL_Delay(300);
	}
	else
	{
		gnss_main_power_init();
		HAL_Delay(150);
	}
	
	GNSS_UART_Init();
    /*GNSS模块下电*/
	gnss_write_hex_stream("F1D90641050000000000034F64");
	GNSS_UART_DeInit();

	//LPM_LOCK(STANDBY_GNSS_LOCK);
}

/**
 * @brief gnss nv读取
 * 		  
 */
void gnss_nv_read(void)
{
    //读取nv里备电选择参数
    g_gnss_bak_power_select = READ_FAC_NV(uint8_t, gnss_bak_power_select);

    //读取nv里启动模式参数
    g_gnss_cold_start = READ_FAC_NV(uint8_t, gnss_cold_start);
}

/**
 * @brief gnss深睡恢复函数，在深睡失败恢复时调用
 * 		  
 */
void gnss_deepsleep_recover()
{
	//拉高给TXCO供电的GPIO_WKP3
	//McuGpioWrite(MCU_WKP3, 1); //由于打开了tcxo电源自动控制功能，wkup3退出standy或deepsleep会自动上电
	Prcm_PowerUpXtal32k(),Prcm_PowerUpRc32k();

	if(g_gnss_cold_start != 0)
		gnss_power_clock_init();
	else
		gnss_main_power_init();
}

/**
 * @brief gnss深睡管理函数，在深睡前调用
 * 		  
 */
__RAM_FUNC void gnss_deepsleep_manage()
{
	if(g_gnss_cold_start != 0)
		gnss_power_clock_deinit();
	else
		gnss_main_power_deinit();
		
	//切换时钟后，拉低给TXCO供电的GPIO_WKP3
	//McuGpioModeSet(MCU_WKP3, 0x13);  //由于打开了tcxo电源自动控制功能，wkup3进入standy或deepsleep会自动断电
}

/**
 * @brief gnss深睡去初始化函数，在深睡前调用
 * 		  
 */
__RAM_FUNC void gnss_lowpower_set()
{
	if(g_gnss_cold_start == 2)
	{
		PRCM_LPUA_PWR_Ctl(LPUA_DEEPSLEEP_MODE_OFF);//关闭LPUART的电源
		AONPRCM->AGPIWKUP_CTRL |= 0x1100;//初始化APGPIO1为唤醒引脚(WKP2)
		PRCM_IOLDO1_ModeCtl(IOLDO1_LPMODE_Enable);
		PRCM_AonWkupSourceDisable(AON_WKUP_SOURCE_LPUART1);
		PRCM_PowerOffRetldo();
		HWREGB(0x40000800) = ((HWREGB(0x40000800) & (~0x70)) | 0x20);   //lpldo 1V
		utc_cnt_delay(2);
		HWREGB(0x40000814) &= ~0x10;   //uvlen off
		Prcm_PowerOffXtal32k(),Prcm_PowerOffRc32k();
		while(HWREGB(0x40000030) & 0xc0);//等待RC32K关闭完成
	}
}

extern void gnss_on();
/**
 * @brief gnss进入BOOT模式，下载固件
 * 		  
 */
void gnss_boot_mode()
{
	gnss_main_power_init();
	McuGpioModeSet(GNSS_PRTRG, 0x00), McuGpioWrite(GNSS_PRTRG, 0);
	HAL_Delay(100);
	gnss_on();
	HAL_Delay(300);
	//McuGpioWrite(GNSS_PRTRG, 1);
	McuGpioModeSet(GNSS_PRTRG, 0x24);
}