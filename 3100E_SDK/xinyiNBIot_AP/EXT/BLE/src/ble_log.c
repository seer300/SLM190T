/** 
* @file        
* @brief   该源文件为通过BLE进行无线log输出，目前仅支持AP应用核的简单log输出
* @warning     
*/
#include "xy_system.h"
#include "xy_printf.h"
#include "urc_process.h"
#include "xy_cp.h"
#include "xy_lpm.h"
#include "xy_at_api.h"
#include "xy_memmap.h"
#include "ble_hci.h"
#include "ble_fota.h"

uint8_t ble_log_is_open(void)
{
	if(g_ble_fac_nv->log_enable == 0 || g_working_info->connected == 0 || is_blefota_started())
		return 0;
	return 1;
}

void ble_send_log(uint8_t *buf, int len)
{
	send_data_to_main(MSG_SEND_LOG,buf,len);
}