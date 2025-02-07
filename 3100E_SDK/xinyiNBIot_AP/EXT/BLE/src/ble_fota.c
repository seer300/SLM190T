/** 
* @file        
* @brief   该源文件为BLE实现的本地FOTA能力，以进行NB的版本差分升级
* @warning     
*/
#include "xy_system.h"
#include "xy_printf.h"
#include "urc_process.h"
#include "xy_cp.h"
#include "xy_lpm.h"
#include "xy_at_api.h"
#include "xy_memmap.h"
#include "ble_fota.h"
#include "ble_drv.h"
#include "ble_hci.h"
#include "ble_api.h"
#include "xy_flash.h"
#include "factory_nv.h"
#include "xy_fota.h"
#include "ble_ymodem.h"
#include "at_process.h"


int g_fotapatch_sn = 0;
int g_blefota_start = 0;


int is_blefota_started()
{
	return g_blefota_start;
}

static uint8_t chk_sum(const void*data,uint32_t length)
{
	uint8_t ret = 0;
	int   val = 0;
	uint8_t *tmp_ptr = (uint8_t *)data;
	uint8_t *tail;

	// xy_assert(((int)data%4) == 0);
	while(length>0)
	{
		val ^= *tmp_ptr;
		tmp_ptr++;
		length--;
	}
	ret ^= *((uint8_t *)&val);
	ret ^= *((uint8_t *)&val+1);
	ret ^= *((uint8_t *)&val+2);
	ret ^= *((uint8_t *)&val+3);

	tail = (uint8_t *)tmp_ptr;
	while((uint32_t)tail < (int)data+length)
	{
		ret ^= *tail;
		tail++;
	}
	return ret;
}

int at_fota_flash_erease()
{
	g_fotapatch_sn = 0;
	return AT_Send_And_Get_Rsp("AT+NFWUPD=0\r\n", 10, "\r\nOK\r\n",NULL);
}

int at_fota_downloading(uint8_t *data, uint16_t len)
{
	int at_ret = 0;
	uint8_t *data_str = xy_malloc(len * 2 + 1);
	char *at_cmd = xy_malloc(50 + len * 2 + 1);
	uint8_t crc_str[3] = {0};
	uint8_t data_crc;

	data_crc = chk_sum(data, len);
	bytes2hexstr(&data_crc, 1, (char*)crc_str, 3);
	bytes2hexstr(data, len, (char*)data_str, len * 2 + 1);

	snprintf(at_cmd, 50 + len * 2 + 1, "AT+NFWUPD=1,%d,%d,%s,%s\r\n", g_fotapatch_sn++, len, data_str, crc_str);
	at_ret = AT_Send_And_Get_Rsp(at_cmd, 10, "\r\nOK\r\n", NULL);

	xy_free(data_str);
	xy_free(at_cmd);
	return at_ret;
}

int at_fota_check()
{
	return AT_Send_And_Get_Rsp("AT+NFWUPD=2\r\n", 10, "\r\nOK\r\n",NULL);
}

int at_fota_update()
{
	return AT_Send_And_Get_Rsp("AT+NFWUPD=5\r\n", 10, "\r\nOK\r\n",NULL);
}

/*data[0]:ble_fota_cmd_E;data[1]:ble_fota_err_E*/
void ble_fota_cmd_deal(uint8_t* data, int len)
{
	uint8_t rsp_result[2] = {0};
	(void) len;
	rsp_result[0] = (uint8_t)data[0];

	switch((ble_fota_cmd_E)data[0])
	{
		case BLE_FOTA_CMD_START:
		{
			Boot_CP(WAIT_CP_BOOT_MS);
			/*增强BLE传输能力，加快数据传输速率，常见于FOTA差分包等大数据ymodem */
			enhance_send_capability();
			if (at_fota_flash_erease() == XY_OK)
			{
				g_blefota_start = 1;
				rsp_result[1] = (uint8_t)BLE_FOTA_STATE_CMD_OK;
				send_str_to_mcu("\r\n+BLEFOTAEVT:1\r\n");
			}
			else
				rsp_result[1] = (uint8_t)BLE_FOTA_STATE_CMD_ERR;
		}
		break;
		case BLE_FOTA_CMD_UPDATE:
		{
			send_str_to_mcu("\r\n+BLEFOTAEVT:4\r\n");
			at_fota_update();
			rsp_result[1] = (uint8_t)BLE_FOTA_STATE_CMD_ERR;
		}
		break;
		case BLE_FOTA_CMD_GIVEUP:
		{
			// xy_Soft_Reset(SOFT_RB_BY_FOTA);
			rsp_result[1] = (uint8_t)BLE_FOTA_STATE_CMD_ERR;
		}
		break;
		case BLE_FOTA_CMD_AGAIN:
		{
			rsp_result[1] = (uint8_t)BLE_FOTA_STATE_CMD_OK;
		}
		break;
		case BLE_FOTA_CMD_END:
		{
			rsp_result[1] = (uint8_t)BLE_FOTA_STATE_CMD_OK;
			send_str_to_mcu("\r\n+BLEFOTAEVT:3\r\n");
		}
		break;
		case BLE_FOTA_CMD_QUERY_PROGRESS:
		{
			if(at_fota_check() == XY_OK)
			{
				rsp_result[1] = (uint8_t)BLE_FOTA_STATE_OTA_CHECK;
			}
			else
			{
				rsp_result[1] = (uint8_t)BLE_FOTA_STATE_TASK_ABNORMAL;
			}
		}
		break;
		case BLE_FOTA_CMD_REPORT:
		{
			rsp_result[1] = *(data + 1);
		}
		break;
		case BLE_FOTA_CMD_TIMEOUT:
		{
            rsp_result[1] = (uint8_t)BLE_FOTA_STATE_CMD_OK;
			send_str_to_mcu("\r\n+BLEFOTAEVT:5\r\n");
		}
		break;
		default:
		{
			rsp_result[1] = (uint8_t)BLE_FOTA_STATE_CMD_ERR;
		}
		break;
	}
	
	if(hci_send_data(g_working_info->fotacmd_handle, rsp_result, 2)!=BLE_OK)
	{
		xy_assert(0);
	}
}
