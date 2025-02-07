/** 
* @file        
* @brief   该头文件为通过BLE进行无线log输出，目前仅支持AP应用核的简单log输出
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
#include "ble_hci.h"


#define BLE_LOG_SIZE 256


uint8_t ble_log_is_open(void);
void ble_send_log(uint8_t *buf, int len);