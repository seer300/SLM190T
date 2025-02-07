/** 
* @file        
* @brief   该头文件为BLE实现的本地FOTA能力，以进行NB的版本差分升级
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

typedef enum
{
    BLE_FOTA_CMD_START          = 0,   //初始化
    BLE_FOTA_CMD_UPDATE         = 0x01,//经BLE_HANDLE_FOTA物理通道接收完差分包后，通过该命令触发重启升级
    BLE_FOTA_CMD_GIVEUP         = 0x02,//用户取消FOTA传输
    BLE_FOTA_CMD_AGAIN          = 0x03,//重新进行fota传输
    BLE_FOTA_CMD_END            = 0x04,//差分包接收完成，后续准备重启升级
    BLE_FOTA_CMD_QUERY_PROGRESS = 0x05,//fota传输进度查询
    BLE_FOTA_CMD_REPORT         = 0x06,//fota异常上报
    BLE_FOTA_CMD_TIMEOUT        = 0x08 //fota手动确认超时
}ble_fota_cmd_E;


typedef enum
{
    BLE_FOTA_STATE_CMD_ERR        = 0,
    BLE_FOTA_STATE_CMD_OK         = 0x01,
    BLE_FOTA_STATE_TASK_ABNORMAL  = 0x02,
    BLE_FOTA_STATE_OTA_CHECK      = 0x03,
    BLE_FOTA_STATE_TASK_RUNNING   = 0x04
}ble_fota_err_E;



void ble_fota_cmd_deal(uint8_t* data, int len);
int at_fota_downloading(uint8_t *data, uint16_t len);
int is_blefota_started();

