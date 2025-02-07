/**
 * @file
 * @brief   该头文件为BLE的AP核主控函数，运行在裸核架构的main主函数中，负责处理ble_drv.c传递来的事件，如控制消息、数据消息等。
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
#include "ble_api.h"


#define CP_FAC_LEN            (((uint32_t)&(((factory_nv_t *)0)->softap_fac_nv)))
#define SOTAP_FAC_LEN         ((uint32_t)&(((softap_fac_nv_t *)0)->ble_cfg))
#define BLE_FAC_NV_BASE       (NV_FLASH_FACTORY_BASE + CP_FAC_LEN + SOTAP_FAC_LEN)

#define BLE_PARAM_OFFSET(param) (((uint32_t)&(((BLE_cfg_t *)0)->param)))
#define BLE_PARAM_LEN(param)    (sizeof(((BLE_cfg_t *)0)->param))
#define SAVE_BLE_PARAM(param)	xy_ftl_write(BLE_FAC_NV_BASE+BLE_PARAM_OFFSET(param), (uint8_t *)&g_ble_fac_nv->param, BLE_PARAM_LEN(param))


/*BLE运行态状态机，断电或深睡后需要重新open初始化*/
typedef struct
{
	uint8_t poweron;	     //1表示已给BLE上电
	uint8_t connected;	     //1表示有终端连接上BLE，才容许进行HCI_CMD_SEND_BLE_DATA数据的发送
    uint8_t configed;	     //1表示完成对BLE的初始化配置
	
	uint8_t hib_flag;	     // 是否进hib
	uint8_t padding[4];

	uint16_t default_handle; // 默认数据通路，可用于外部MCU数据的BLE传输
	uint16_t at_handle;		 // BLE间用于AT server的AT命令传输，为SoC内部AT命令
	uint16_t fota_handle;	 // FOTA差分包码流的ymdoem传输通道
	uint16_t log_handle;	 // log专用通道
	uint16_t fotacmd_handle; // FOTA控制消息传输通道
	uint16_t chhandle_none;	 // 待删除
} ble_work_info_T;


extern ble_work_info_T *g_working_info;
extern BLE_cfg_t *g_ble_fac_nv;



void ble_recv_and_process(void);

