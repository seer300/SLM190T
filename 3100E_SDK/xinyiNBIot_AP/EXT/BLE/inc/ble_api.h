/** 
* @file        
* @brief   该头文件为BLE芯片操作的API接口，以供应用用户通过API接口进行BLE硬件的操控。该接口可供用户在AP核上进行BLE相关OPENCPU的二次开发
* @warning     
*/
#pragma once
#include "xy_system.h"
#include "xy_printf.h"
#include "urc_process.h"
#include "xy_cp.h"
#include "xy_lpm.h"
#include "xy_at_api.h"
#include "xy_memmap.h"
#include "ble_drv.h"
#include "factory_nv.h"
#include "ble_msg.h"
#include "ble_main.h"


#define BLE_TRUE    1
#define BLE_FALSE   0

/*AT命令相关结果码，主要为转换BLE_ERR_E*/
typedef enum
{
	BLE_OK = 0,
	BLE_ERR = 1,            /*unknown err*/
	BLE_WAIT_ACK = 2,       /*正在等待BLE芯片的ack确认，超时时长为BLE_WAIT_ACK_MS*/
	BLE_NO_ALLOWED = 3,     /*Operation not allowed*/
	BLE_NO_SUPPORT = 4,     /*Operation not supported*/
	BLE_SEND_FAIL = 5,       /*HCI发送后等待ACK超时失败*/
	BLE_EXE_FAIL = 50,      /*Execution failed*/
	BLE_PARAM_NOSUPPORT = 52,   /*Option not supported，不支持该选项*/
	BLE_PARAM_INVALID = 53      /*Parameter invalid*/
}BLE_ERR_E;


#define BLE_WAIT_ACK_MS                 200  /*等待ACK确认的最大超时时长，ms*/

/*类似socket的errno*/
extern uint32_t ble_errno;




/******************************************************************************************************************************
 * @brief  通过LPUART的AT通道，发送AT命令给外部MCU，含URC、DEBUG等特殊AT命令
 * @param 
 ******************************************************************************************************************************/
void send_str_to_mcu(char *buf);


/******************************************************************************************************************************
 * @brief  打开BLE
 * @param 
 * @return see @ref BLE_ERR_E
 ******************************************************************************************************************************/
BLE_ERR_E ble_open();


/******************************************************************************************************************************
 * @brief  关闭BLE
 * @param 
 * @return see @ref BLE_ERR_E
 ******************************************************************************************************************************/
BLE_ERR_E ble_close();


/******************************************************************************************************************************
 * @brief  设置密码
 * @param  key   仅可配置为六位数字，如654321
 * @return see @ref BLE_ERR_E
 ******************************************************************************************************************************/
BLE_ERR_E ble_set_passkey(uint32_t passkey);


/******************************************************************************************************************************
 * @brief  设置配对模式
 * @param  pairing_mode
 * @return see @ref BLE_ERR_E
 ******************************************************************************************************************************/
BLE_ERR_E ble_set_pairing_mode(uint32_t pairing_mode);

/******************************************************************************************************************************
 * @brief  打开广播
 * @return see @ref BLE_ERR_E
 ******************************************************************************************************************************/
BLE_ERR_E ble_open_broadcast();

/******************************************************************************************************************************
 * @brief  关闭广播
 * @param  
 * @return see @ref BLE_ERR_E
 ******************************************************************************************************************************/
BLE_ERR_E ble_close_broadcast();


/******************************************************************************************************************************
 * @brief  经底层驱动发送数据给BLE模组
 * @param  data  待写入码流
 * @return see @ref BLE_ERR_E
 * @warning 接口内部阻塞等BLE芯片的确认应答，进而不能运行在中断服务程序中
 ******************************************************************************************************************************/
BLE_ERR_E ble_send_data(uint8_t *data,uint32_t len);


/******************************************************************************************************************************
 * @brief  设置设备名
 * @param  name   设置的名称
 * @param  size   名称的长度，最长29字节，
 * @return see @ref BLE_ERR_E
 ******************************************************************************************************************************/
BLE_ERR_E ble_set_name(char *name, uint8_t size);


/******************************************************************************************************************************
 * @brief  获取设备地址
 * @param  addr   addr地址空间由调用者申请，内部进行内存拷贝。长度固定6字节，需要通过bytes2hexstr转换为字符串
 * @return 
 ******************************************************************************************************************************/
BLE_ERR_E ble_get_dev_addr(uint8_t *addr);


/**
  * @brief 设置存放在RFNV中的BLEMAC地址
  */
int set_rfnv_blemac(char *blemac);


/**
  * @brief 获取存放在RFNV中的BLEMAC地址
  */
int get_rfnv_blemac(char *blemac);

/**
  * @brief blepasskey默认imei号后6位
  */
int get_blekey_from_imei(int* blekey);

/**
  * @brief 获取存放在RFNV中的BLE频偏
  */
uint8_t get_ble_freqoffset();

/******************************************************************************************************************************
 * @brief  BLE设备底层事件的回调函数，内部由上层用户对event进行识别处理
 * @param  event   BLE设备底层事件
 * @return 
 * @warning 用户通过Regist_Event_Hook接口进行注册，由BLE底层驱动收到BLE设备事件后调用
 ******************************************************************************************************************************/
typedef void (*BleEventHook)(uint8_t event);

void Regist_Event_Hook(BleEventHook eventFunc);


/******************************************************************************************************************************
 * @brief  BLE设备接收到的远程码流数据的回调函数，内部由上层用户执行内存申请并拷贝码流后，处理数据
 * @param  data   BLE设备接收到的远程码流数据
 * @return 
 * @warning 用户通过Regist_Event_Hook接口进行注册，由BLE底层驱动收到BLE设备上报的码流数据后调用
 ******************************************************************************************************************************/
typedef void (*BleRcvDataHook)(uint8_t *data,uint32_t len);

void Regist_Data_Hook(BleRcvDataHook dataFunc);

/**
  * @brief   设置BLEGPIO
  * @param   Mode:输入/输出,BLE_GPIO_MODE_IN/BLE_GPIO_MODE_OUT
  * @param   Gpionum:blegpio号
  * @param   ucConfig:设置输入的上下拉或者输出的高低电平
  * @return  BLE_OK:设置成功
  * @note   
  */
BLE_ERR_E ble_gpio_set(unsigned char Mode, unsigned char Gpionum, unsigned char ucConfig);

/**
  * @brief   获取BLEGPIO电平状态
  * @param   Gpionum:blegpio号
  * @return  0:低；1:高; 2:error
  * @note   
  */
uint8_t ble_gpio_get(unsigned char Gpionum);

