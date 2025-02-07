/**
 * @file
 * @brief   该源文件为BLE相关的AT命令集实现，包括服务端AT请求及URC组装等。AT命令的服务端函数注册在at_cmd_regist.c中
 * @warning
 */
#include "xy_system.h"
#include "xy_printf.h"
#include "urc_process.h"
#include "xy_cp.h"
#include "xy_lpm.h"
#include "xy_at_api.h"
#include "xy_memmap.h"
#include "at_cmd_regist.h"
#include "at_uart.h"
#include "ble_at.h"
#include "ble_api.h"
#include "ble_drv.h"
#include "ble_hci.h"
#include "xy_utils.h"

#define QUOTATION_LEN (2)
#define HEX_DATA_MAX (1024)
#define PASSTHR_DATA_MAX (1024)

#define PASSTHR_QUIT_SYMBOL "+++"


/*LPUART透传BLE相关的码流数据*/
uint8_t g_ble_passthr_mode = 0;

char rcv_tail_buff[4] = {0};          //缓存接收到的码流最后3个字节，以供匹配是否为+++结束标识
uint32_t rcv_tail_len = 0;            //缓存接收到的+字符大小

/**
 * @brief  处于数据模式时,输入+++退出数据模式
 * @note   该接口内部实现不建议客户做任何修改
 */
bool find_passthr_quit_symbol(char *buf, uint32_t data_len)
{
    //先找开头，判断是否满足+++，++，+
	if (strlen(PASSTHR_QUIT_SYMBOL) - rcv_tail_len >= data_len)
	{
		memcpy(rcv_tail_buff + rcv_tail_len, buf, data_len);
	}
	else
	{
		memcpy(rcv_tail_buff + rcv_tail_len, buf, strlen(PASSTHR_QUIT_SYMBOL) - rcv_tail_len);
	}

	if (!strcmp(rcv_tail_buff, PASSTHR_QUIT_SYMBOL))
	{
		memset(rcv_tail_buff, 0, sizeof(rcv_tail_buff));
		rcv_tail_len = 0;
		return true;
	}
	else if (strlen(rcv_tail_buff) == 1 && *rcv_tail_buff == '+')
	{
		//ppp_tail_buff起始一个+
		rcv_tail_len = 1;
		return false;
	}
	else if (strlen(rcv_tail_buff) == 2 && !strcmp(rcv_tail_buff, "++"))
	{
		//ppp_tail_buff起始两个+
		rcv_tail_len = 2;
		return false;
	}
	else
	{
		//再找结尾，判断是否存在+++，++，+
		memset(rcv_tail_buff, 0, sizeof(rcv_tail_buff));
		if (data_len >= strlen(PASSTHR_QUIT_SYMBOL))
		{
			if (memcmp(buf + data_len - 3, PASSTHR_QUIT_SYMBOL, 3) == 0)
			{
				//数据末尾三字节为+++
				rcv_tail_len = 0;
				return true;
			}
			else if (strstr(buf, PASSTHR_QUIT_SYMBOL) != NULL)
			{
				//eg:abc+++def
				rcv_tail_len = 0;
				return true;
			}
			else if (memcmp(buf + data_len - 2, "++", 2) == 0)
			{
				memcpy(rcv_tail_buff, "++", 2);
				rcv_tail_len = 2;
			}
			else if (*(buf + data_len - 1) == '+')
			{
				*rcv_tail_buff = '+';
				rcv_tail_len = 1;
			}
		}
		else
		{
			memcpy(rcv_tail_buff, buf, data_len);
			rcv_tail_len = data_len;
		}	
		return false;
	}
}

uint8_t is_ble_in_passthr_mode(void)
{
	return g_ble_passthr_mode;
}

void ble_enter_passthr_mode(void)
{
	g_ble_passthr_mode = 1;
}

void ble_exit_passthr_mode(void)
{
	g_ble_passthr_mode = 0;
}

/*LPUART收到的码流，直接写BLE，但由于写接口需要等ack确认，可能出现LPUART处FIFO溢出*/
int ble_passthrough_data_proc(char *data, uint16_t len)
{
	if (len > PASSTHR_DATA_MAX)
	{
		return XY_ERR;
	}
	else
	{
		if (find_passthr_quit_symbol(data, len))
		{
			ble_exit_passthr_mode();
			Send_AT_to_Ext("\r\nOK\r\n");
			return XY_OK;
		}
		ble_send_data((uint8_t*)data, len);
	}
	return XY_OK;
}

/*用于配置蓝牙扩展功能*/
int at_QBTCFG_req(char *param, char **rsp_cmd)
{
	if (g_cmd_type == AT_CMD_REQ)
	{
		char cmd1[20]={0};
		char cmd2[8]={0};

		if (at_parse_param("%s,%s", param, cmd1,cmd2) != XY_OK)
			return XY_ERR_PARAM_INVALID;
		if (!strcmp((const char *)(cmd1), "ACCESSMODE"))
		{
			if(strlen(cmd2) == 0)
			{
				*rsp_cmd = xy_malloc(32);
				sprintf(*rsp_cmd,"%d",g_ble_fac_nv->accessmode);
				return XY_OK;
			}
			else if(!strcmp((const char *)(cmd2), "0") )
			{
				g_ble_fac_nv->accessmode = 0;
			}
			else if(!strcmp((const char *)(cmd2), "1") )
			{
				g_ble_fac_nv->accessmode = 1;
			}
			else
			{
				return XY_ERR_PARAM_INVALID;
			}
			SAVE_BLE_PARAM(accessmode);
		}
		else if (!strcmp((const char *)(cmd1), "CONNECTURC"))
		{
			if(strlen(cmd2) == 0)
			{
				*rsp_cmd = xy_malloc(32);
				sprintf(*rsp_cmd,"%d",g_ble_fac_nv->connecturc);
				return XY_OK;
			}
			else if(!strcmp((const char *)(cmd2), "0") )
			{
				g_ble_fac_nv->connecturc = 0;
			}
			else if(!strcmp((const char *)(cmd2), "1") )
			{
				g_ble_fac_nv->connecturc = 1;
			}
			else
			{
				return XY_ERR_PARAM_INVALID;
			}
			SAVE_BLE_PARAM(connecturc);
		}
		return XY_OK;
	}
	else if (g_cmd_type == AT_CMD_TEST)
	{
		*rsp_cmd = xy_malloc(64);
		sprintf(*rsp_cmd,"\"ACCESSMODE\",0,1\r\n+QBTCFG: \"CONNECTURC\",0,1");
		return XY_OK;
	}
	else
		return XY_ERR_PARAM_INVALID;
}

/*AT+BLETEST=mode */
int at_QBTTEST_req(char *param, char **rsp_cmd)
{
	if((g_working_info == NULL) || g_working_info->poweron == 0)
	{
		return XY_ERR_NOT_ALLOWED;
	}
	if (g_cmd_type == AT_CMD_REQ) // 设置类
	{
		int mode = 0;
		int pairing_mode = 1;

		if (at_parse_param("%d", param, &mode) != XY_OK)
			return XY_ERR_PARAM_INVALID;

		if (mode != 0 && mode != 1)
		{
			return XY_ERR_PARAM_INVALID;
		}
		if (mode)
		{
			g_ble_fac_nv->pairing_mode = 2;
		}
		else
		{
			g_ble_fac_nv->pairing_mode = 0;
		}
	}
	else if (g_cmd_type == AT_CMD_QUERY)
	{
		*rsp_cmd = xy_malloc(32);
		sprintf(*rsp_cmd, "%d", g_ble_fac_nv->pairing_mode > 1 ? 1 : 0);
	}
	else
		return XY_ERR_PARAM_INVALID;
	return XY_OK;
}

int at_QBTOPEN_req(char *param, char **rsp_cmd)
{
	(void)param;
	if (g_cmd_type == AT_CMD_ACTIVE)
	{
		if(g_working_info->poweron == 1)
		{
			return XY_ERR_NOT_ALLOWED;
		}

		if (ble_open() != BLE_OK)
		{
			*rsp_cmd = xy_malloc(32);
			sprintf(*rsp_cmd, "\r\nOK\r\n\r\n+QBTOPEN: 1\r\n");
		}//开启成功会在蓝牙初始化好后上报
	}
	else if (g_cmd_type == AT_CMD_QUERY)
	{
		*rsp_cmd = xy_malloc(32);
		sprintf(*rsp_cmd, "\r\n+QBTOPEN: %d", g_working_info->poweron);
	}
	else if (g_cmd_type == AT_CMD_TEST)
	{
		return XY_OK;
	}
	else
		return XY_ERR_PARAM_INVALID;
	return XY_OK;
}

int at_QBTCLOSE_req(char *param, char **rsp_cmd)
{
	(void)param;
	if (g_cmd_type == AT_CMD_ACTIVE)
	{
		if((g_working_info == NULL) || g_working_info->poweron == 0)
		{
			return XY_ERR_NOT_ALLOWED;
		}

		*rsp_cmd = xy_malloc(32);
		ble_close();
		sprintf(*rsp_cmd, "\r\nOK\r\n\r\n+QBTCLOSE: 0\r\n");
	}
	else if (g_cmd_type != AT_CMD_TEST)
		return XY_ERR_PARAM_INVALID;
	return XY_OK;
}

/*用于打开蓝牙广播。蓝牙广播打开后主设备可搜索到该蓝牙设备。*/
int at_QBTSTARTBLE_req(char *param, char **rsp_cmd)
{
	if((g_working_info == NULL) || g_working_info->poweron == 0)
	{
		return XY_ERR_NOT_ALLOWED;
	}
	(void)param;
	if (g_cmd_type == AT_CMD_ACTIVE)
	{

		*rsp_cmd = xy_malloc(32);

		if (ble_open_broadcast() != BLE_OK)
		{
			sprintf(*rsp_cmd, "\r\nOK\r\n\r\n+QBTSTARTBLE: 1\r\n");
		}
		else
		{
			sprintf(*rsp_cmd, "\r\nOK\r\n\r\n+QBTSTARTBLE: 0\r\n");
		}
	}
	else if (g_cmd_type == AT_CMD_QUERY)
	{
		*rsp_cmd = xy_malloc(32);

		sprintf(*rsp_cmd, "\r\n+QBTSTARTBLE: %d",g_ble_fac_nv->broadcast);

	}
	else if (g_cmd_type != AT_CMD_TEST)
		return XY_ERR_PARAM_INVALID;
	return XY_OK;
}

/*用于配置以及查询蓝牙设备名称,最大长度为 29 字节*/
int at_QBTNAME_req(char *param, char **rsp_cmd)
{
	if((g_working_info == NULL) || g_working_info->poweron == 0)
	{
		return XY_ERR_NOT_ALLOWED;
	}
	if (g_cmd_type == AT_CMD_REQ)
	{
		char device_name[31] = {0};

        if(at_parse_param("%31s", param, device_name) != XY_OK)
            return XY_ERR_PARAM_INVALID;

		if((strlen(device_name) > 29) || (strlen(device_name) == 0))
		{
			return XY_ERR_PARAM_INVALID;
		}

		memcpy(g_ble_fac_nv->ble_name,device_name,strlen((char *)device_name)+1);

		SAVE_BLE_PARAM(ble_name);
	}
	else if (g_cmd_type == AT_CMD_QUERY)
	{
		*rsp_cmd = xy_malloc(64);
		sprintf(*rsp_cmd, "\r\n+QBTNAME: %s", g_ble_fac_nv->ble_name);
	}
	else if (g_cmd_type == AT_CMD_ACTIVE)
		return XY_ERR_PARAM_INVALID;
	return XY_OK;
}

/*用于查询蓝牙设备地址,十六进制数值*/
int at_QBTLEADDR_req(char *param, char **rsp_cmd)
{
	if((g_working_info == NULL) || g_working_info->poweron == 0)
	{
		return XY_ERR_NOT_ALLOWED;
	}
	(void)param;
	if (g_cmd_type == AT_CMD_QUERY)
	{
		char ble_mac[20] = {0};
		*rsp_cmd = xy_malloc(64);

		bytes2hexstr(g_ble_fac_nv->ble_mac , 6, ble_mac, 13);

		sprintf(*rsp_cmd, "\r\n+QBTLEADDR: %-12s", ble_mac);
	}
	else if (g_cmd_type != AT_CMD_TEST)
		return XY_ERR_PARAM_INVALID;
	return XY_OK;
}

/*用于配置蓝牙设备名称及查询蓝牙设备名称和地址*/
int at_BLENAME_req(char *param, char **rsp_cmd)
{
	if((g_working_info == NULL) || g_working_info->poweron == 0)
	{
		return XY_ERR_NOT_ALLOWED;
	}
	if (g_cmd_type == AT_CMD_REQ)
	{
		char device_name[31] = {0};

        if(at_parse_param("%31s", param, device_name) != XY_OK)
            return XY_ERR_PARAM_INVALID;

		if((strlen(device_name) > 29) || (strlen(device_name) == 0))
		{
			return XY_ERR_PARAM_INVALID;
		}

        memcpy(g_ble_fac_nv->ble_name,device_name,strlen((char *)device_name)+1);

		SAVE_BLE_PARAM(ble_name);
	}
	else if (g_cmd_type == AT_CMD_QUERY)
	{
		char ble_mac[20] = {0};
		*rsp_cmd = xy_malloc(64);

		bytes2hexstr(g_ble_fac_nv->ble_mac , 6, ble_mac, 13);
		sprintf(*rsp_cmd, "\r\n+BLENAME: %s,%-12s", g_ble_fac_nv->ble_name,ble_mac);
	}
	else if (g_cmd_type == AT_CMD_ACTIVE)
		return XY_ERR_PARAM_INVALID;
	return XY_OK;
}

/*配置和查询蓝牙设备配对密码,密码仅可配置为六位数字*/
int at_QBTPASSKEY_req(char *param, char **rsp_cmd)
{
	if((g_working_info == NULL) || g_working_info->poweron == 0)
	{
		return XY_ERR_NOT_ALLOWED;
	}
	if (g_cmd_type == AT_CMD_REQ)
	{
		uint32_t passkey = 0;
		uint8_t passkey_str[8] = {0};
		uint8_t str_index = 0;

		//输入AT+QBTPASKEY=000012时会按照八进制解析成十进制的000010，故此处先把参数解析成字符串，再单独按照十进制转换成数字
		if (at_parse_param("%8s", param, passkey_str) != XY_OK)
			return XY_ERR_PARAM_INVALID;
		while (isdigit(passkey_str[str_index]))
		{
			str_index++;
		}
		if(str_index != 6)
			return XY_ERR_PARAM_INVALID;

		passkey = strtol((const char *)passkey_str,NULL,10);

		g_ble_fac_nv->passkey = passkey;

		SAVE_BLE_PARAM(passkey);
	}
	else if (g_cmd_type == AT_CMD_QUERY)
	{
		*rsp_cmd = xy_malloc(32);
		sprintf(*rsp_cmd, "\r\n+QBTPASSKEY: %06d", g_ble_fac_nv->passkey);
	}
	else if (g_cmd_type == AT_CMD_ACTIVE)
		return XY_ERR_PARAM_INVALID;
	return XY_OK;
}

/*用于传输蓝牙数据。执行命令可以进入数据模式下的透传数据功能。*/
int at_QBTWRITE_req(char *param, char **rsp_cmd)
{
	if((g_working_info == NULL) || g_working_info->poweron == 0 || g_working_info->connected == 0)
	{
		return XY_ERR_NOT_ALLOWED;
	}
	if (g_cmd_type == AT_CMD_REQ)
	{
		uint8_t *at_end = strchr(param, '\r');
		if(at_end != NULL)
			*at_end = '\0';
		else
			return XY_ERR_DROP_MORE;

		uint16_t datalen = strlen(param);
		/*至多1026字节,至少3字节（包括""）*/
		if (datalen > HEX_DATA_MAX + QUOTATION_LEN || datalen <= QUOTATION_LEN || *param != '"' || *(param + datalen - 1) != '"')
			return XY_ERR_PARAM_INVALID;
		if (ble_send_data((uint8_t *)(param+1), datalen - QUOTATION_LEN) != BLE_OK)
		{
			return XY_ERR_PARAM_INVALID;
		}
	}
	else if (g_cmd_type == AT_CMD_ACTIVE)
	{
		ble_enter_passthr_mode();
		*rsp_cmd = xy_malloc(24);
		if(g_ble_fac_nv->accessmode == 0)
		{
			sprintf(*rsp_cmd, "\r\n>");
		}
		else if(g_ble_fac_nv->accessmode == 1)
			sprintf(*rsp_cmd, "\r\nCONNECT\r\n");
	}
	return XY_OK;
}

/*AT+BLENV*/
int at_BLENV_req(char *at_buf, char **rsp_cmd)
{
	if (g_cmd_type == AT_CMD_REQ)
	{
		char cmd[6] = {0};
		char subcmd[16] = {0};
		if (at_parse_param("%6s,%16s", at_buf, cmd, subcmd) != XY_OK)
		{
			return XY_ERR_PARAM_INVALID;
		}

		if (!strcmp((const char *)(cmd), "SET"))
		{
			int param = -1;
			if (!strcmp((const char *)(subcmd), "HIB"))
			{
				if (at_parse_param(",,%d", at_buf, &param) != XY_OK)
				{
					return XY_ERR_PARAM_INVALID;
				}
				g_ble_fac_nv->hib_enable = param;
				SAVE_BLE_PARAM(hib_enable);
			}
			else if (!strcmp((const char *)(subcmd), "PAIRINGMODE"))
			{
				if (at_parse_param(",,%d", at_buf, &param) != XY_OK)
				{
					return XY_ERR_PARAM_INVALID;
				}
				g_ble_fac_nv->pairing_mode = param;
				SAVE_BLE_PARAM(pairing_mode);
			}
			else if (!strcmp((const char *)(subcmd), "KEYMODE"))
			{
				if (at_parse_param(",,%d", at_buf, &param) != XY_OK)
				{
					return XY_ERR_PARAM_INVALID;
				}
				g_ble_fac_nv->key_mode = param;
				SAVE_BLE_PARAM(key_mode);
			}
			else if (!strcmp((const char *)(subcmd), "LOG"))
			{
				if (at_parse_param(",,%d", at_buf, &param) != XY_OK)
				{
					return XY_ERR_PARAM_INVALID;
				}
				g_ble_fac_nv->log_enable = param;
				SAVE_BLE_PARAM(log_enable);
			}
			else if (!strcmp((const char *)(subcmd), "NAME"))
			{
				uint8_t name[30] = {0};
				if (at_parse_param(",,%30s", at_buf, name) != XY_OK)
				{
					return XY_ERR_PARAM_INVALID;
				}
				memcpy(g_ble_fac_nv->ble_name, name, 30);
				SAVE_BLE_PARAM(ble_name);
			}
			else
			{
				return XY_ERR_PARAM_INVALID;
			}
			return XY_OK;
		}
		else if ((!strcmp((const char *)(cmd), "GET")))
		{
			*rsp_cmd = xy_malloc(64);
			if (!strcmp((const char *)(subcmd), "HIB"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->hib_enable);
			}
			else if (!strcmp((const char *)(subcmd), "PAIRINGMODE"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->pairing_mode);
			}
			else if (!strcmp((const char *)(subcmd), "KEYMODE"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->key_mode);
			}
			else if (!strcmp((const char *)(subcmd), "RFPOWER"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->rf_power);
			}
			else if (!strcmp((const char *)(subcmd), "PASSKEY"))
			{
				sprintf(*rsp_cmd, "\r\n%06d", g_ble_fac_nv->passkey);
			}
			else if (!strcmp((const char *)(subcmd), "MTUSIZE"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->mtusize);
			}
			else if (!strcmp((const char *)(subcmd), "FREQOFFSET"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->freq_offset);
			}
			else if (!strcmp((const char *)(subcmd), "LOG"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->log_enable);
			}
			else if (!strcmp((const char *)(subcmd), "INTMIN"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->interval_min);
			}
			else if (!strcmp((const char *)(subcmd), "INTMAX"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->interval_max);
			}
			else if (!strcmp((const char *)(subcmd), "LATENCY"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->latency);
			}
			else if (!strcmp((const char *)(subcmd), "TIMOOUT"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->conn_timeout);
			}
			else if (!strcmp((const char *)(subcmd), "NAME"))
			{
				sprintf(*rsp_cmd, "\r\n%s", g_ble_fac_nv->ble_name);
			}
			else if (!strcmp((const char *)(subcmd), "NAMEADV"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->ble_name_adv);
			}
			else if (!strcmp((const char *)(subcmd), "ADVTMIN"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->advt_min);
			}
			else if (!strcmp((const char *)(subcmd), "ADVTMAX"))
			{
				sprintf(*rsp_cmd, "\r\n%d", g_ble_fac_nv->advt_max);
			}
			else if (!strcmp((const char *)(subcmd), "MAC"))
			{
				char ble_mac[20] = {0};
				*rsp_cmd = xy_malloc(64);

				bytes2hexstr(g_ble_fac_nv->ble_mac , 6, ble_mac, 13);

				sprintf(*rsp_cmd, "\r\n+QBTLEADDR:%-12s", ble_mac);
			}
			else
			{
				xy_free(*rsp_cmd);
				*rsp_cmd = NULL;
				return XY_ERR_PARAM_INVALID;
			}
				return XY_OK;
		}
		else
		{
			return XY_ERR_PARAM_INVALID;
		}
	}
	else
	{
		return XY_ERR_PARAM_INVALID;
	}

}

extern void ble_boot_init();
int at_BLETEST_req(char *at_buf, char **rsp_cmd)
{
	(void)rsp_cmd;
	if (g_cmd_type == AT_CMD_REQ)
	{
		char subcmd[16] = {0};
		int param = 0;
		if (at_parse_param("%16s", at_buf, subcmd) != XY_OK)
		{
			return XY_ERR_PARAM_INVALID;
		}

		if (!strcmp((const char *)(subcmd), "OFF"))
		{
			if (at_parse_param(",%d", at_buf, &param) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			if(param == 0)
			{
				ble_close();
			}
			else if(param == 1)
			{
				ble_power_clock_deinit();
			}
			else if(param == 2)
			{
				ble_power_clock_deinit();
				ble_pin_deinit();
			}
			else if(param == 3)
			{
				ble_pin_deinit();
			}
			else if(param == 4)
			{
				McuUartRxDis(1);
				McuUartTxDis(1);
				McuGpioModeSet(BLE_WAKEUP_PIN, 0x13);
				McuGpioModeSet(NB_STATE_PIN, 0x13);
				McuGpioModeSet(BLE_STATE_PIN, 0x13);
				McuGpioModeSet(BLE_UART_TX_PIN, 0x13);
				McuGpioModeSet(BLE_UART_RX_PIN, 0x13);
			}
			else if(param == 5)
			{
				McuUartRxDis(1);
				McuUartTxDis(1);
			}
			else if(param == 6)
			{
				ble_into_lpm();
			}
			else if(param == 7)
			{
				McuGpioModeSet(BLE_RESET_PIN, 0x13);
			}
			else if(param == 8)
			{
				McuUartRxDis(1);
				McuUartTxDis(1);
				McuGpioModeSet(BLE_WAKEUP_PIN, 0x00), McuGpioWrite(BLE_WAKEUP_PIN, 0);
				McuGpioModeSet(NB_STATE_PIN, 0x00), McuGpioWrite(NB_STATE_PIN, 0);
				McuGpioModeSet(BLE_STATE_PIN, 0x13);
				McuGpioModeSet(BLE_UART_TX_PIN, 0x00), McuGpioWrite(BLE_UART_TX_PIN, 0);
				McuGpioModeSet(BLE_UART_RX_PIN, 0x00), McuGpioWrite(BLE_UART_RX_PIN, 0);
			}

		}
		else if (!strcmp((const char *)(subcmd), "ON"))
		{
			if (at_parse_param(",%d", at_buf, &param) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			if(param == 0)
			{
				ble_power_clock_init();
				ble_uart_init();
				ble_gpio_init();
				ble_boot_init();
			}
			else if(param == 1)
			{
				ble_power_clock_init();
			}
			else if(param == 2)
			{
				ble_uart_init();
				ble_gpio_init();
			}
			else if(param == 3)
			{
				extern void ble_pin_unused_init();
				ble_pin_unused_init();
			}
			else if(param == 4)
			{
				McuGpioModeSet(GPIO_PAD_NUM_26, 0x00);
				McuGpioWrite(GPIO_PAD_NUM_26, 0);
				HAL_Delay(10);
				McuGpioWrite(GPIO_PAD_NUM_26, 1);
				for(volatile int i = 0; i<130000; i++);//大概50ms
				McuGpioWrite(GPIO_PAD_NUM_26, 0);
				HAL_Delay(10);
				McuGpioWrite(GPIO_PAD_NUM_26, 1);
			}
			else if(param == 6)
			{
				ble_wakeup_from_lpm();
			}

		}
		else if (!strcmp((const char *)(subcmd), "POWERON"))
		{
			LPM_LOCK(DSLEEP_BLE_LOCK); //开蓝牙需关闭DEEPSLEEP
			LPM_LOCK(STANDBY_BLE_LOCK);//开蓝牙需关闭STANDBY
            LPM_LOCK(USER_WFI_LOCK1);
			ble_power_clock_init();
		}
		else if (!strcmp((const char *)(subcmd), "POWEROFF"))
		{
			ble_power_clock_deinit();
			LPM_UNLOCK(DSLEEP_BLE_LOCK); 
			LPM_UNLOCK(STANDBY_BLE_LOCK);
            LPM_UNLOCK(USER_WFI_LOCK1);
		}
		else if (!strcmp((const char *)(subcmd), "GPIO"))
		{
			xy_ftl_regist((void*)NV_FLASH_FACTORY_BASE, NV_FLASH_FACTORY_LEN);
	
			if(g_working_info == NULL)
			{
				g_working_info = xy_malloc(sizeof(ble_work_info_T));
				memset(g_working_info,0,sizeof(ble_work_info_T));
			}
			if(g_ble_fac_nv == NULL)
			{
				g_ble_fac_nv = xy_malloc(sizeof(BLE_cfg_t));

				if(xy_ftl_read(BLE_FAC_NV_BASE,(void *)g_ble_fac_nv,sizeof(BLE_cfg_t)) ==0)
					memset(g_ble_fac_nv,0,sizeof(BLE_cfg_t));
			}
			if(g_ble_rsp_info == NULL)
			{
				g_ble_rsp_info = xy_malloc(sizeof(rcv_payload_info_T));
				memset(g_ble_rsp_info,0,sizeof(rcv_payload_info_T));
			}
			ble_uart_init();
			ble_gpio_init();
		}
		else if (!strcmp((const char *)(subcmd), "BAUD"))
		{
			int baud = 160;
			if (at_parse_param(",%d", at_buf, &baud) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			ble_uartbaud_change(baud);
		}
		else if (!strcmp((const char *)(subcmd), "RESET"))
		{
			ble_boot_init();
		}
		else if(!strcmp(subcmd, "RFPARAM"))//FOR SYSTEM TEST
		{
			int ble_rfparam = 0;//频偏
			int ble_rfpower = 0;//功率
			*rsp_cmd = xy_malloc(60);

			extern int g_ble_rfparam;

			if(at_parse_param(",%d,%d", at_buf, &ble_rfparam, &ble_rfpower) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}

			if((ble_rfparam < 0) || ((ble_rfparam > 31) && (ble_rfparam != 100)))
			{
				return XY_ERR_PARAM_INVALID;
			}
			if((ble_rfpower != 0) && (ble_rfpower != 1))
			{
				return XY_ERR_PARAM_INVALID;
			}

			extern int g_blerf_test;
			extern int g_ble_rfpower;
			g_blerf_test = 0;
			g_ble_rfparam = ble_rfparam;
			g_ble_rfpower = ble_rfpower;

			ble_boot_init();

			extern void BLERF_UART_Init();
			BLERF_UART_Init();

			g_blerf_test = 1;
			if(ble_rfparam == 100)
			{
				snprintf(*rsp_cmd, 60, "\r\n+XYBLE:SYSTEST CHANGED DEF_PATCH");
			}
			else
			{
				snprintf(*rsp_cmd, 60, "\r\n+XYBLE:SYSTEST CHANGED TEST_PATCH");
			}

			return XY_OK;
		}
		else if (!strcmp((const char *)(subcmd), "PINDOWN"))
		{
			ble_pin_deinit();
			return XY_OK;
		}
		else if (!strcmp((const char *)(subcmd), "HIB"))
		{
			BLE_ERR_E ret = BLE_OK;
			ret = ble_config_op(HCI_CMD_LE_SET_HIBERNATE, NULL, 0);
			if (ret!=BLE_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}

			ble_pin_deinit();

			return XY_OK;
		}
		else if (!strcmp((const char *)(subcmd), "NBACTIVE"))
		{
			if (at_parse_param(",%d", at_buf, &param) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			if (param == 0)
			{
				McuGpioWrite(NB_STATE_PIN, 0);
			}
			else if(param == 1)
			{
				McuGpioWrite(NB_STATE_PIN, 1);
			}

		}
		else if (!strcmp((const char *)(subcmd), "WAKBLE"))
		{
			if (at_parse_param(",%d", at_buf, &param) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			if (param == 0)
			{
				McuGpioModeSet(BLE_WAKEUP_PIN, 0x00), McuGpioWrite(BLE_WAKEUP_PIN, 0);
			}
			else if(param == 1)
			{
				McuGpioModeSet(BLE_WAKEUP_PIN, 0x00), McuGpioWrite(BLE_WAKEUP_PIN, 1);
			}
		}
		else if (!strcmp((const char *)(subcmd), "MAC"))
		{
			BLE_ERR_E ret = BLE_OK;
			char ble_mac[16]={0};
			char ble_mac_b[16]={0};
			if (at_parse_param(",%14s", at_buf, ble_mac) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}

			if(strlen(ble_mac) != 12)
				return XY_ERR_PARAM_INVALID;
			if(hexstr2bytes(ble_mac,12,ble_mac_b,6) == -1)
				return XY_ERR_PARAM_INVALID;

			str_back_order(ble_mac_b,6);

			ret = ble_config_op(HCI_CMD_SET_BLE_ADDR, ble_mac_b, 6);
			if (ret!=BLE_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			return XY_OK;
		}
		else if (!strcmp((const char *)(subcmd), "ADVT"))
		{
			BLE_ERR_E ret = BLE_OK;
			uint16_t advt = 160;
			if (at_parse_param(",%2d", at_buf, &advt) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			ret = ble_config_op(HCI_CMD_LE_SET_ADV_PARM, &advt, sizeof(uint16_t));
			if (ret!=BLE_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			return XY_OK;
		}
		else if (!strcmp((const char *)(subcmd), "RFPOWER"))
		{
			BLE_ERR_E ret = BLE_OK;
			int rfpower = 0;
			if (at_parse_param(",%d", at_buf, &rfpower) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			extern const uint8_t g_RF_power_list[];
			RF_power_E ble_rfpw = g_RF_power_list[rfpower];
			ret = ble_config_op(HCI_CMD_SET_TX_POWER, &ble_rfpw, sizeof(uint8_t));
			if (ret!=BLE_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			return XY_OK;
		}
		else if (!strcmp((const char *)(subcmd), "CONNUP"))
		{
			uint16_t  min = 0, max = 0, latency = 0, timeout = 0;
			BLE_ERR_E ret = BLE_OK;

			if (at_parse_param(",%2d,%2d,%2d,%2d", at_buf, &min,&max,&latency,&timeout) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			uint16_t param_tx[4] = {min, max, latency, timeout};
			ret = ble_config_op(HCI_CMD_LE_SEND_CONN_UPDATE_REQ, param_tx, sizeof(param_tx));
			if (ret!=BLE_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			return XY_OK;
		}
		else if (!strcmp((const char *)(subcmd), "DATA"))
		{
			ble_uart_write("1234567890",strlen("1234567890"));
		}
		else if (!strcmp((const char *)(subcmd), "BLEPASS"))
		{
			int passmode = 0;
			if (at_parse_param(",%d", at_buf, &passmode) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			extern uint8_t g_ymodem_passth;
			g_ymodem_passth = passmode;
		}
		else if (!strcmp((const char *)(subcmd), "LOCKSBY"))
		{
			LPM_LOCK(STANDBY_BLE_LOCK);
		}
		else if (!strcmp((const char *)(subcmd), "LOCKDEP"))
		{
			LPM_LOCK(DSLEEP_BLE_LOCK);
		}
		else if (!strcmp((const char *)(subcmd), "UNLOCKSBY"))
		{
			LPM_UNLOCK(STANDBY_BLE_LOCK);
		}
		else if (!strcmp((const char *)(subcmd), "UNLOCKDEP"))
		{
			LPM_UNLOCK(DSLEEP_BLE_LOCK);
		}
		else if (!strcmp((const char *)(subcmd), "LOCKWFI"))
		{
			LPM_LOCK(USER_WFI_LOCK1);
		}
		else if (!strcmp((const char *)(subcmd), "UNLOCKWFI"))
		{
			LPM_UNLOCK(USER_WFI_LOCK1);
		}
		else if (!strcmp((const char *)(subcmd), "SETGPIO"))
		{
			int  Mode = 0;
			int  Gpionum = 0;
			int  ucConfig = 0;
			if (at_parse_param(",%d,%d,%d", at_buf, &Mode, &Gpionum, &ucConfig) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			BLE_ERR_E ble_param = BLE_OK;
			ble_param = ble_gpio_set(Mode, Gpionum,ucConfig);
			if(ble_param != BLE_OK)
			{
				return XY_ERR_NOT_ALLOWED;
			}
		}
		else if (!strcmp((const char *)(subcmd), "GETGPIO"))
		{
			int  Gpionum = 0;
			if (at_parse_param(",%d", at_buf, &Gpionum) != XY_OK)
			{
				return XY_ERR_PARAM_INVALID;
			}
			uint8_t ble_param = 0;
			ble_param = ble_gpio_get(Gpionum);
			if(ble_param == 2)
			{
				return XY_ERR_NOT_ALLOWED;
			}
			else
			{
				*rsp_cmd = xy_malloc(32);
				snprintf(*rsp_cmd, 32, "\r\n+GPIO:%d",ble_param);
			}
		}
		else
			return XY_ERR_PARAM_INVALID;
		return XY_OK;
	}	
	else
		return XY_ERR_PARAM_INVALID;
}