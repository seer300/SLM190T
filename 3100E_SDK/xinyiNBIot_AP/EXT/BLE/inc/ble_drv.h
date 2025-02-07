/** 
* @file        
* @brief   该头文件为BLE的底层驱动，包括中断服务函数及BLE外设的寄存器级别操作
* @warning     
*/
#pragma once
#include "xy_system.h"
#include "xy_printf.h"
#include "ble_api.h"


#define BLE_UART_PORT                ((UART_TypeDef *) UART2_BASE) //蓝牙串口使用uart2   
#define BLE_UART_START_BAUDRATE      115200                        //蓝牙串口初始波特率             
#define BLE_UART_CONFIG_BAUDRATE     921600                        //蓝牙串口后续配置波特率
#define BLE_UART_CLOCK               48000000
#define BLE_UART_TX_PIN              GPIO_PAD_NUM_46 //GPIO_PAD_NUM_51 //GPIO_PAD_NUM_12//3100E/1200E引脚选取不同
#define BLE_UART_RX_PIN              GPIO_PAD_NUM_44 //GPIO_PAD_NUM_53 //GPIO_PAD_NUM_11//BLE的TX，NB的RX

#define BLE_RESET_PIN                GPIO_PAD_NUM_10 //GPIO_PAD_NUM_52 //GPIO_PAD_NUM_5
#define BLE_STATE_PIN                GPIO_PAD_NUM_7  //GPIO_PAD_NUM_44 //GPIO_PAD_NUM_1
#define NB_STATE_PIN                 GPIO_PAD_NUM_22 //GPIO_PAD_NUM_46 //GPIO_PAD_NUM_9
#define BLE_WAKEUP_PIN               GPIO_PAD_NUM_24 //GPIO_PAD_NUM_48 //GPIO_PAD_NUM_10

//BLE开关按键，测试用，后续根据实际的硬件设计修改相应PIN
#define BLE_POWERON_PIN              MCU_WKP1

//BLE连接仪表，不同测试patch需要修改的位置
#define BLE_RF_POWER_SITE1 4009
#define BLE_RF_POWER_SITE2 4013
#define BLE_RF_POWER_SITE3 4017
#define BLE_RF_POWER_SITE4 4021
#define BLE_RF_FREQOFFSET_SITE1 4099
#define BLE_RF_CHECK_SITE2 4234 

typedef struct
{
    volatile uint16_t uart_intsta;   /*UART底层监控异常*/
    volatile uint16_t uart_state;    /*UART底层监控异常*/
	  volatile uint16_t gpio_int_times;/*gpio中断产生的次数*/
    uint8_t lpm_IO_state;   		     /*0，BLE未进LPM；1，BLE进入LPM；2:BLE刚退出LPM*/
    uint8_t uart_disenable;			     /*0，uart使能；1，uart失能*/
    uint8_t uart_ovflag;			       /*0，uart未溢出；1，uart溢出*/
}ble_drv_stat_t;
extern ble_drv_stat_t *g_ble_drv_stat;

/**
 * @brief UART初始化函数，主要完成以下配置：
 * 		  1. UART的TXD、RXD引脚的GPIO初始化配置
 * 		  2. UART的波特率、位宽、奇偶检验模式的配置
 */
void ble_uart_init(void);

/**
  * @brief   将信息发送给uart
  * @param   buf 传递的数据
  * @param   len 传递的数据长度
  * @return
  * @note   
  */
void ble_uart_write(uint8_t *data, uint32_t len);

/**
  * @brief   uart fifo读空
  */
void ble_uart_fifo_clear(void);

/**
  * @brief   修改和BLE对接的UART口波特率
  * @param   baud: 波特率
  */
void ble_uartbaud_change(uint32_t baud);

/**
  * @brief 蓝牙gpio 初始化
  */
void ble_gpio_init(void);

/*
  * @brief 蓝牙硬件复位，需要重新下载patch code
  */
void ble_hard_reset(void);

/*
  * @brief 发送数据或命令给BLE时，需先通过PIN脚唤醒BLE
  */
void ble_wake_up(void);

/**
 * @brief 初始化ble供电和时钟
 * 		  
 */
void ble_power_clock_init();

/**
 * @brief 去初始化ble供电和时钟
 * 		  
 */
void ble_power_clock_deinit();

/**
 * @brief 拉低所有ble相关引脚，防止漏电
 * 		  
 */
void ble_pin_deinit();

/**
  * @brief 蓝牙模块初始化，包括uart初始化，蓝牙波特率配置，patch下载等
  */
void ble_start_init();

/**
  * @brief 蓝牙按键GPIO初始化
  */
void ble_powerkey_init();

/**
  * @brief 判断蓝牙开关按键是否被按下，并执行相应操作
  */
void check_ble_poweron_key();

/**
 * @brief  检测当前是否有待收发处理的BLE数据，若未处理则退出睡眠
 * @return false ：BLE数据收发处理未结束，BLE通道处于非IDLE态
 *         true  ：BLE数据收发处理已结束，BLE通道处于IDLE态
 */
bool ble_uart_idle();

/**
  * @brief BLE进入LPM模式引脚配置
  */
int ble_into_lpm(void);

/**
  * @brief BLE退出LPM后引脚配置
  */
void ble_wakeup_from_lpm();