/** 
* @file        
* @brief   该源文件为BLE的AP核主控函数，运行在裸核架构的main主函数中，负责处理ble_drv.c传递来的事件，如控制消息、数据消息等。
* @warning     
*/
#include "xy_system.h"
#include "xy_printf.h"
#include "urc_process.h"
#include "xy_cp.h"
#include "xy_lpm.h"
#include "xy_at_api.h"
#include "xy_memmap.h"
#include "ble_main.h"
#include <stdint.h>
#include "ble_msg.h"
#include "ble_hci.h"
#include "ble_api.h"
#include "ble_ymodem.h"
#include "at_uart.h"
#include "ble_fota.h"
#include "ble_at.h"

uint8_t g_ymodem_passth = 0;


/*刚有终端接入时，BLE尚不稳定，延迟若干毫秒后再进行链接参数的更新触发*/
uint8_t g_conn_have_stable = 0;


ble_work_info_T *g_working_info = NULL;
BLE_cfg_t *g_ble_fac_nv =NULL;


/*刚有终端接入时，BLE尚不稳定，延迟若干毫秒后再进行链接参数的更新触发*/
__RAM_FUNC void Conn_Update_Timeout(void)
{
	g_conn_have_stable = 1;
}


/*缺省通道收到的数据处理，非数据模式下，上报URC*/
void default_data_rcv_proc(uint8_t *data,int len)
{
	if (is_ble_in_passthr_mode() == 0)
	{
		//字符串
		char *urcdata = xy_malloc(len * 2 + strlen("\r\n+QBTDATA: \r\n") + 1);
		sprintf(urcdata, "\r\n+QBTDATA: ");

		memcpy(urcdata+strlen(urcdata),data,len);
		memcpy(urcdata + strlen("\r\n+QBTDATA: ") + len, "\r\n", 2);
 
		at_uart_write_data(urcdata,strlen("\r\n+QBTDATA: \r\n") + len);
		xy_free(urcdata);

		//hex模式
		// char *urcdata = xy_malloc(len * 2 + strlen("\r\n+QBTDATA:\r\n") + 1);
		// sprintf(urcdata, "\r\n+QBTDATA:");

		// bytes2hexstr(data, len, urcdata + strlen(urcdata), len * 2 + 1);
		// sprintf(urcdata + strlen(urcdata), "\r\n");
		// Send_AT_to_Ext(urcdata);
		// xy_free(urcdata);
	}
	else
	{
		if(len)
			at_uart_write_data(data, len);
	}
}

/*物理通道接收到的数据内容处理*/
void ble_rcved_data_proc(ble_msg_t *msg)
{
	uint16_t handle = *((uint16_t *)(msg->payload));
	uint8_t *data = (uint8_t *)(msg->payload + DATA_HANDLE_LEN);
	uint32_t len = msg->length - DATA_HANDLE_LEN;

	if(handle == g_working_info->at_handle)
	{
		extern void ble_at_server(char *str,uint32_t size);
		ble_at_server((char *)data,len);
	}
	else if(handle == g_working_info->fota_handle)
	{
		uint8_t *fota_pack = NULL;
		int16_t pack_len = 0;
		pack_len = ymodem_rcv_proc(&fota_pack, data, len, g_working_info->fota_handle);
		if (pack_len > 0)
		{
			at_fota_downloading(fota_pack, pack_len);
			Send_Byte(handle, MODEM_ACK);
		}
		else
		{
			char *err_str = xy_malloc(64);
			snprintf(err_str, 64, "\r\nymodem data error:%d\r\n", pack_len);
			send_str_to_mcu(err_str);
			xy_free(err_str);
		}
	}
	else if(handle == g_working_info->log_handle)
	{
	}
	else if(handle == g_working_info->default_handle)
	{
		if(g_ymodem_passth == 1)
		{
			uint8_t *passth_pack = NULL;
			int16_t pack_len = 0;
			pack_len = ymodem_rcv_proc(&passth_pack, data, len, g_working_info->default_handle);
			if (pack_len > 0)
			{
				at_uart_write_data(passth_pack, pack_len);
				HAL_Delay(10);
				/*BLE接收大数据时(例如100K)，收到一包数据后立即回复ACK,会导致主线程长时间卡在蓝牙数据接收和处理流程,而无法退出while(1)去刷新看门狗，导致看门狗死机，
				改为收到数据后先执行数据处理，再回复ACK可以延后下一包数据到来的时间，使得主线程来得及刷新看门狗，从而避免死机*/
				Send_Byte(handle, MODEM_ACK);
			}
		}
		else
			default_data_rcv_proc(data,len);
	}
	else if(handle == g_working_info->fotacmd_handle)
	{
		ble_fota_cmd_deal(data,len);
	}
	else
	{
		xy_printf("ERROR!!!UNKNOWN HANDLE:%#x",handle);
		//xy_assert(0);
	}
}


void ble_rcv_cfg_cmd(ble_msg_t *msg)
{
	switch (msg->opcode)
	{
		case HCI_EVENT_NVRAM_REP:
		{
			extern void save_ble_nvram(void *payload);
			save_ble_nvram(msg->payload);
		}
		break;
		case HCI_EVENT_LE_TK:
		{
			g_ble_rsp_info->le_tk = *(unsigned int *)msg->payload;

			char tk_urc[64] = {0};
			snprintf(tk_urc, 64, "\r\n+BLEDBG:BLETKSHOW,%d,,%06d\r\n", BLE_CONN_HANDLE, (int)g_ble_rsp_info->le_tk);
			send_str_to_mcu(tk_urc);
		}
		break;
		case HCI_EVENT_LE_PAIRING_STATE:
		{
			g_ble_rsp_info->pairing_state = *(unsigned short *)msg->payload;
			char pai_urc[32] = {0};
			if (g_ble_rsp_info->pairing_state == 0x0080)
			{
				sprintf(pai_urc, "\r\n+BLEDBG:BLEPAIRDONE,%d,0\r\n", BLE_CONN_HANDLE);
			}
			else if (g_ble_rsp_info->pairing_state == 0x0180)
			{
				sprintf(pai_urc, "\r\n+BLEDBG:BLEPAIRDONE,%d,1\r\n", BLE_CONN_HANDLE);
			}
			send_str_to_mcu(pai_urc);
			if(g_ble_rsp_info->pairing_state == 0x0180 && g_working_info->connected == 1)
			{
				ble_config_op(HCI_CMD_BLE_DISCONNECT, NULL, 0);//dongle配对失败不会主动断开，需要从机断开
			}
		}
		break;
		case HCI_EVENT_LE_ENCRYPTION_STATE:
		{
			g_ble_rsp_info->le_encryption = *(unsigned char *)msg->payload;
		}
		break;
		case HCI_EVENT_LE_GKEY:
		{
			g_ble_rsp_info->le_key = *(unsigned int *)msg->payload;
			// little2big(g_ble_rsp_info->le_key);
		}
		break;

		default:
			break;
	}
}

extern volatile uint32_t g_ble_AT_2_CP;
uint8_t g_ble_paried = 0;
void ble_recv_and_process(void)
{
	extern void blerf_recv_process();
	blerf_recv_process();//ble连接仪表测试数据收发

	ble_msg_node_t *msg_node = NULL;

	check_ble_poweron_key();

	/*刚有终端接入时，BLE尚不稳定，延迟若干毫秒后再进行链接参数的更新触发*/
	if (g_conn_have_stable == 1)
	{
		g_conn_have_stable = 0;
		uint16_t param_tx[4] = {g_ble_fac_nv->interval_min / 1.25, g_ble_fac_nv->interval_max / 1.25, g_ble_fac_nv->latency, g_ble_fac_nv->conn_timeout / 10};
		if(g_working_info->connected == 1)
			ble_config_op(HCI_CMD_LE_SEND_CONN_UPDATE_REQ, param_tx, sizeof(param_tx));
	}

	while((msg_node = (ble_msg_node_t *)ListRemove(&g_ble_msg_head)) != NULL)
	{
		ble_msg_t *hci_msg = (ble_msg_t *)msg_node->data;

		if(hci_msg->type == MSG_SEND_AT_RSP)
		{
			xy_printf("ble_recv_and_process send AT_RSP %d", msg_node->len - sizeof(ble_msg_t));
			hci_send_data(g_working_info->at_handle,hci_msg->payload, msg_node->len - sizeof(ble_msg_t));
		}
		else if(hci_msg->type == MSG_SEND_LOG)
		{
			hci_send_data(g_working_info->log_handle,hci_msg->payload, msg_node->len - sizeof(ble_msg_t));
		}
		/*BLE芯片发送来的消息数据*/
		else if(hci_msg->type == HCI_RECV_EVENT)
		{
			switch (hci_msg->opcode)
			{
				case HCI_EVENT_SPP_CONN_REP: // 经典蓝牙
				case HCI_EVENT_SPP_DIS_REP:	 // 经典蓝牙
				case HCI_EVENT_SPP_DATA_REP: // 经典蓝牙
				case HCI_EVENT_GKEY:		 // 经典蓝牙
				case HCI_EVENT_GET_PASSKEY:	 // 经典蓝牙
					break;

				case HCI_EVENT_STATUS_RES:
				case HCI_EVENT_NVRAM_REP:
				case HCI_EVENT_LE_TK:
				case HCI_EVENT_LE_PAIRING_STATE:
				case HCI_EVENT_LE_ENCRYPTION_STATE:
				case HCI_EVENT_LE_GKEY:
				case HCI_EVENT_UUID_HANDLE:  //在check_ble_cmd_rsp中已处理
				case HCI_EVENT_CMD_RES:      //在check_ble_cmd_rsp中已处理
					ble_rcv_cfg_cmd(hci_msg);
					break;

				case HCI_EVENT_LE_CONN_REP:  //有终端接入
					g_working_info->connected = 1;
					Timer_AddEvent(TIMER_NON_LP_BLE,5000, Conn_Update_Timeout, 0);//5s后更新连接参数，非必要操作
					if ((2 == g_ble_fac_nv->pairing_mode || 4 == g_ble_fac_nv->pairing_mode) && (g_ble_paried == 1))
					{
						// master请求CONNECT_IND之后，BLE就会上报CONNECTED，配对模式下不需要delay，可以直接开始配对，若配对失败则直接断开连接
						ble_config_op(HCI_CMD_LE_START_PAIRING, 0, 0);
					}
					else
					{
						// master请求CONNECT_IND之后，BLE就会上报CONNECTED，需要延迟一段时间保证LL层连接的初始化交互完成，再进行数据业务
						//HAL_Delay(2000);延迟2s上报已经连接上信息
					}

					if(g_ble_fac_nv->connecturc == 1)
					{
						Send_AT_to_Ext("+QBTURC: \"connected\"\r\n");
					}

					if(g_ble_fac_nv->accessmode == 1)
					{
						Send_AT_to_Ext("\r\nCONNECT\r\n");
						extern void ble_enter_passthr_mode(void);
						ble_enter_passthr_mode();
					}
					break;

				case HCI_EVENT_LE_DIS_REP:
					g_working_info->connected = 0;
					
					Timer_DeleteEvent(TIMER_NON_LP_BLE);

					Send_AT_to_Ext("+QBTURC: \"disconnected\"\r\n");
					extern void ble_exit_passthr_mode(void);
					if (is_ble_in_passthr_mode())
						ble_exit_passthr_mode();
					break;

				case HCI_EVENT_STANDBY_REP:
					{
						char *ble_mac = xy_malloc(16);
						memset(ble_mac,0,16);
						
						if(g_ble_fac_nv->passkey == 0)
						{
							get_blekey_from_imei(&(g_ble_fac_nv->passkey));
						}
						
						/*若为全0，则需要从CP核读取一次，保存后下次无需再从CP核读取*/
						if(!memcmp(g_ble_fac_nv->ble_mac,ble_mac,6))
						{
							get_rfnv_blemac(ble_mac);
							memcpy(g_ble_fac_nv->ble_mac, ble_mac, 6);
							SAVE_BLE_PARAM(ble_mac);
						}
						xy_free(ble_mac);
						
						send_str_to_mcu("+BLEDBG:BLEREADY\r\n");

						extern BLE_ERR_E ble_config_init();
						if (ble_config_init() != BLE_OK)
						{
							g_working_info->configed = 0;
						}
						else
						{
							g_working_info->configed = 1;
						}
                        Send_AT_to_Ext("\r\n+QBTOPEN: 0\r\n");
						LPM_UNLOCK(STANDBY_BLE_LOCK); //与ble_boot中STANDBY锁对应
					}
					break;
				case HCI_EVENT_LE_DATA_REP:
					ble_rcved_data_proc(hci_msg);
					break;
				default:
					break;
			}
		}

		xy_free(msg_node);
		msg_node = NULL;
	}
}
