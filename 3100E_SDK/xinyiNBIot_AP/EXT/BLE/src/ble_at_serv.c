/**
 * @file
 * @brief   该源文件为通过BLE无线通道进行NB芯片相关AT命令的收发，以操控NB芯片。如通过BLE发送AT+CFUN=0、云通信AT命令等
 * @warning
 */
#include "xy_system.h"
#include "xy_printf.h"
#include "urc_process.h"
#include "xy_cp.h"
#include "xy_lpm.h"
#include "xy_at_api.h"
#include "xy_memmap.h"
#include "at_uart.h"
#include "ble_hci.h"
#include "at_utils.h"
#include "ble_api.h"
#include "xy_at_api.h"
#include "at_cmd_regist.h"
#include "sys_ipc.h"
#include "ble_msg.h"
#include "ble_main.h"
#include "at_ipc_process.h"


#define AT_HEAD_LEN		5		//AT hci的前缀，包括hci头部和at通道的id
#define AT_LEN_MAX		3072
#define BLE_AT_TIME_MS	1000	//BLE发长AT时会分成多条hci消息，收到第一条hci后开始计时，一定时间内未收到'\r'则报错


/*指示是否有BLE过来的AT请求发送给CP核，进而决定ICM_AT_ASYNC中结果码是否发送给BLE*/
volatile uint32_t g_ble_AT_2_CP = 0;

typedef enum
{
	HCI_NOT_AT = 0,			//当前hci msg非AT命令
	HCI_AT_END,				//当前hci msg是AT命令，且包含尾部'\r'
	HCI_AT_NO_END,			//当前hci msg是AT命令，但未收到尾部'\r'，需退出继续接收后续内容
	HCI_AT_ERR,				//当前hci msg是AT命令，AT命令出错，例如长度超过限制，或超时未收到'\r'
};

uint8_t *g_ble_at_buff = NULL;
uint16_t g_ble_at_len = 0;

void reset_ble_at_buf(void)
{
	xy_free(g_ble_at_buff);
	g_ble_at_buff = NULL;
	g_ble_at_len = 0;
	Timer_DeleteEvent(TIMER_BLE_AT_END);
}

void wait_ble_at_end_timeout_callback(void)
{
	reset_ble_at_buf();
	uint8_t *rsp = at_err_build(XY_ERR_DROP_MORE);
	send_str_to_main(MSG_SEND_AT_RSP, rsp,strlen(rsp));
	xy_free(rsp);
}

uint8_t check_ble_at_cmd(char *buf, uint8_t size)
{
	uint8_t ret = HCI_NOT_AT;

	if (*((uint8_t *)buf + size - 1) != '\r' && *((uint8_t *)buf + size - 2) != '\r')
	{
		if (g_ble_at_buff == NULL)
		{
			Timer_AddEvent(TIMER_BLE_AT_END, BLE_AT_TIME_MS, wait_ble_at_end_timeout_callback, 0); // 5s后更新连接参数，非必要操作
			g_ble_at_buff = xy_malloc(size);
			memcpy(g_ble_at_buff, buf, size);
		}
		else
		{
			uint8_t *temp_buff = xy_malloc(size + g_ble_at_len);
			memcpy(temp_buff, g_ble_at_buff, g_ble_at_len);
			memcpy(temp_buff + g_ble_at_len, buf, size);
			xy_free(g_ble_at_buff);
			g_ble_at_buff = temp_buff;
		}
		g_ble_at_len += size;
		ret = HCI_AT_NO_END;
	}
	else
	{
		if (g_ble_at_len != 0)
		{
			uint8_t *temp_buff = xy_malloc(size + g_ble_at_len);
			memcpy(temp_buff, g_ble_at_buff, g_ble_at_len);
			memcpy(temp_buff + g_ble_at_len, buf, size);
			xy_free(g_ble_at_buff);
			g_ble_at_buff = temp_buff;
			g_ble_at_len += size;
			ret = HCI_AT_END;
		}
	}
	if (g_ble_at_len > AT_LEN_MAX)
		ret = HCI_AT_ERR;
	return ret;
}

/*BLE的CP核AT请求和URC上报*/
void ipc_proc_Ble_AT(void *data) 
{
	IPC_At_Data *at_str = (IPC_At_Data *)data;
	IPC_Message pMsg = {ICM_MEM_FREE, &(at_str->at_buf), 4};

	xy_assert(at_str->at_buf != NULL && at_str->at_size != 0);

	send_str_to_main(MSG_SEND_AT_RSP,at_str->at_buf,at_str->at_size);

	if(Is_OK_ERR_rsp(at_str->at_buf) == true)
	{
		g_ble_AT_2_CP = 0;
	}
	IPC_WriteMessage(&pMsg);
}

extern uint8_t get_at_cmd_type(char *buf);
extern char *at_ok_build();
/*仿造at_process_cmd+Match_AT_Cmd接口实现，对AT命令进行本地处理或转发给CP核*/
void ble_at_proc(char *str,uint32_t size)
{
	at_cmd_t *p_at_list = (at_cmd_t *)g_AT_cmd_list;
	char *param = NULL;
	char *rsp = NULL;
	int ret = XY_OK;

	//logview通过ble发的AT命令以"\r\n"结尾，不带'\0',xy_printf使用%s解析str时需遇到'\0'才会停止，此处若不把末尾的'\n'改成'\0',会导致蓝牙log输出乱码。
	if(str[size -1] == '\n')
		str[size - 1] = '\0';

	xy_printf("recv at form ble:%s", str);
	while(p_at_list->at_prefix != 0)
	{
		if((param = at_prefix_strstr(str, p_at_list->at_prefix)) != NULL)
		{
			g_cmd_type = get_at_cmd_type(param);

			if(g_cmd_type == AT_CMD_REQ)
				param++;
			
			ret = p_at_list->proc(param, &rsp);
			
			if (rsp == NULL)
			{
				if (ret == XY_OK)
					rsp = at_ok_build();
				else if (ret == XY_FORWARD) 
				{
					goto FORWARD;
				}
				else
					rsp = at_err_build(ret);
			}
			hci_send_data(g_working_info->at_handle,rsp,strlen(rsp));
			xy_free(rsp);
			if (ret == XY_FORWARD) 
			{
				goto FORWARD;
			}
			return ;
		}
		else
			p_at_list++;
	}

/*借用异步通道发送给CP核，进而在IPC_ProcessEvent中定制接收转发应答结果*/
FORWARD:
		if(g_ble_AT_2_CP == 1)
		{
			rsp = at_err_build(XY_ERR_CHANNEL_BUSY);
			hci_send_data(g_working_info->at_handle,rsp,strlen(rsp));
			xy_free(rsp);
			return;
		}
		Boot_CP(WAIT_CP_BOOT_MS);
		ret = AT_Send_To_CP(str,size,AT_FROM_BLE);

		if(ret != XY_OK)
		{
			rsp = at_err_build(ret);
			hci_send_data(g_working_info->at_handle,rsp,strlen(rsp));
			xy_free(rsp);
			return;
		}
		g_ble_AT_2_CP = 1;
}

void ble_at_server(char *str,uint32_t size)
{
	uint8_t ble_at_flag = check_ble_at_cmd(str, size);
	if(ble_at_flag == HCI_AT_NO_END)
		return;
	else if(ble_at_flag == HCI_AT_ERR)
	{
		reset_ble_at_buf();
		uint8_t *rsp = at_err_build(XY_ERR_NOT_ALLOWED);
		send_str_to_main(MSG_SEND_AT_RSP, rsp,strlen(rsp));
		xy_free(rsp);
		return;
	}
	else if(ble_at_flag == HCI_AT_END)
	{
		ble_at_proc(g_ble_at_buff,g_ble_at_len);
		reset_ble_at_buf();
	}
	else
	{
		ble_at_proc(str,size);
	}
}
