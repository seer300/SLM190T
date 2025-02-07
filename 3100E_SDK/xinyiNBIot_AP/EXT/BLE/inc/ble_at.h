/** 
* @file        
* @brief   该头文件为BLE相关的AT命令集实现，包括服务端AT请求及URC组装等。AT命令的服务端函数注册在at_cmd_regist.c中
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
#include "ble_main.h"
#include <stdint.h>

uint8_t is_ble_in_passthr_mode(void);
int ble_passthrough_data_proc(char *data, uint16_t len);

int at_QBTCFG_req(char *param, char **rsp_cmd);
int at_QBTTEST_req(char *param,char **rsp_cmd);
int at_QBTOPEN_req(char *param,char **rsp_cmd);
int at_QBTCLOSE_req(char *param,char **rsp_cmd);
int at_QBTSTARTBLE_req(char *param,char **rsp_cmd);
int at_QBTNAME_req(char *param,char **rsp_cmd);
int at_QBTLEADDR_req(char *param,char **rsp_cmd);
int at_BLENAME_req(char *param, char **rsp_cmd);
int at_QBTPASSKEY_req(char *param,char **rsp_cmd);
int at_QBTWRITE_req(char *param,char **rsp_cmd);
int at_BLENV_req(char *at_buf, char **rsp_cmd);
int at_BLETEST_req(char *at_buf, char **rsp_cmd);