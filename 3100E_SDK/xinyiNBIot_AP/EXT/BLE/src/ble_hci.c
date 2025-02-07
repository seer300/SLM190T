#include "xy_system.h"
#include "xy_printf.h"
#include "ble_drv.h"
#include "hal_uart.h"
#include "gpio.h"
#include "ble_msg.h"
#include <math.h>
#include "ble_api.h"
#include "ble_hci.h"
#include "xy_timer.h"
#include "factory_nv.h"
#include "xy_utils.h"
#include "at_process.h"
#include "xy_timer.h"
#include "hal_gpio.h"
#include "ble_main.h"
#include "driver_utils.h"

/*发送的opcode，以便接收时根据该值区分具体的操作码*/
uint8_t g_req_opt = 0;

/*仅调试使用！当BLE_HCI_PACKETTYPE_EVENT时，记录对应的操作码请求，应该与g_req_opt一样*/
uint8_t g_ack_opt_debug = 0;

/*1表示BLE回复了ACK消息*/
uint32_t g_opt_acked = 0;


/*将来可以考虑删除！！！hci_recv_event_E收到的命令类携带的BLE芯片的特征值，一般仅做查询或调试使用*/
rcv_payload_info_T *g_ble_rsp_info = NULL;


const uint8_t g_RF_power_list[] = {
	BLE_RFPOWER_F20DB,
	BLE_RFPOWER_F5DB,
	BLE_RFPOWER_F3DB,
	BLE_RFPOWER_0DB,
	BLE_RFPOWER_3DB,
	BLE_RFPOWER_5DB,
	BLE_RFPOWER_6DB,
	BLE_RFPOWER_7DB,
	BLE_RFPOWER_10DB,
	BLE_RFPOWER_0DB
};



/*增强BLE传输能力，加快数据传输速率，常见于FOTA差分包等大数据ymodem */
void enhance_send_capability()
{
	if((g_ble_fac_nv->interval_min != BLE_CONN_MIN)
		|| (g_ble_fac_nv->interval_max != BLE_CONN_MAX)
		|| (g_ble_fac_nv->latency != BLE_CONN_LATENCY)
		|| (g_ble_fac_nv->conn_timeout != BLE_CONN_TIMEOUT))
	{
		uint16_t param_tx[4] = {BLE_CONN_MIN,BLE_CONN_MAX,BLE_CONN_LATENCY,BLE_CONN_TIMEOUT};
		ble_config_op(HCI_CMD_LE_SEND_CONN_UPDATE_REQ, param_tx, sizeof(param_tx));
	}
}


/*超时等待BLE芯片的ack应答*/
BLE_ERR_E hci_wait_cmd_cmp()
{
	uint32_t tickstart = Get_Tick();
	while(1)
	{
		if(g_opt_acked == 1)
		{
			g_opt_acked = 0;
			return BLE_OK;
		}
		else if(Check_Ms_Timeout(tickstart,BLE_WAIT_CMP_MS))
		{
			ble_errno = BLE_SEND_FAIL;
			return BLE_SEND_FAIL;
		}
	}
}


/*HCI空闲信令的底层发送，并死等BLE芯片的ack确认消息*/
BLE_ERR_E HciSendCmdAndWaitAck(uint8_t *data, uint32_t len)
{
	BLE_ERR_E ret = BLE_OK;

	g_req_opt = *(data+1);
	ble_uart_write(data, len);
	ret = hci_wait_cmd_cmp();
	
	// if((*(data+1) != HCI_CMD_SEND_BLE_DATA) && ret!=BLE_OK)
	// {
	// 	xy_assert(0);
	// }

	return ret;
}


/**
  * @brief   发送cmd给BLE芯片，并死等ack等数据结果，且进行控制面数据处理
  */
BLE_ERR_E hci_cmd_dispatch(ble_hci_cmd_E opcode, uint16_t handle, uint8_t *buf, uint32_t len)
{
	uint32_t remain_len = len;
	BLE_ERR_E ret = BLE_OK;

	ble_msg_t *ble_send_cmd = (ble_msg_t *)xy_malloc(PAYLOAD_MAX_LEN + HCI_HEADER_LEN);
	ble_send_cmd->type = HCI_SEND_EVENT;
	ble_send_cmd->opcode = opcode;
	xy_printf("[hci_cmd_dispatch] opcode:%d, handle:%d, len:%d", opcode, handle, len);
	xy_assert(!IS_IRQ_MODE());

	if(g_working_info->poweron == 0)
	{
		goto end;
	}

	if (HCI_CMD_SEND_BLE_DATA == opcode)
	{
		if (NULL == buf || 0 == len)
		{
			xy_assert(0);
			goto end;
		}

		if (g_working_info->connected == 0)
			goto end;

		ble_wake_up();

		ble_send_cmd->length = PAYLOAD_DATA_MAX_LEN + DATA_HANDLE_LEN;
		*((uint16_t *)ble_send_cmd->payload) = handle;

		do
		{
			if (len < remain_len)
			{
				xy_assert(0);
			}
			if (remain_len / PAYLOAD_DATA_MAX_LEN)
			{
				memcpy(ble_send_cmd->payload+DATA_HANDLE_LEN, buf + (len - remain_len), PAYLOAD_DATA_MAX_LEN);
				if (remain_len < PAYLOAD_DATA_MAX_LEN)
				{
					xy_assert(0);
				}
				remain_len -= PAYLOAD_DATA_MAX_LEN;
			}
			else
			{
				memcpy(ble_send_cmd->payload+DATA_HANDLE_LEN, buf + (len - remain_len), remain_len);
				ble_send_cmd->length = remain_len + DATA_HANDLE_LEN;
				remain_len = 0;
			}

			ret = HciSendCmdAndWaitAck((uint8_t *)ble_send_cmd, ble_send_cmd->length + HCI_HEADER_LEN);
			if(ret != BLE_OK)
				ret = HciSendCmdAndWaitAck((uint8_t *)ble_send_cmd, ble_send_cmd->length + HCI_HEADER_LEN);
			if(ret != BLE_OK)
			{
				if (g_working_info->connected == 0)
				{
					ret = BLE_NO_ALLOWED;
					goto end;
				}
				send_str_to_mcu("+BLEDBG: Send data fail!\r\n");
			}

		} while (remain_len);
	}
	else
	{
		if (len > PAYLOAD_DATA_MAX_LEN)
		{
			xy_assert(0);
			goto end;
		}

		ble_wake_up();

		ble_send_cmd->length = remain_len;

		if (remain_len)
		{
			memcpy(ble_send_cmd->payload, buf, remain_len);
		}

		ret = HciSendCmdAndWaitAck((uint8_t *)ble_send_cmd, ble_send_cmd->length + HCI_HEADER_LEN);
		if(ret != BLE_OK)
			ret = HciSendCmdAndWaitAck((uint8_t *)ble_send_cmd, ble_send_cmd->length + HCI_HEADER_LEN);
		if(ret != BLE_OK)
		{
			send_str_to_mcu("+BLEDBG: Send Config fail!\r\n");
		}
	}

end:
	xy_free(ble_send_cmd);
	return ret;
}

BLE_ERR_E hci_send_data(uint16_t handle, void *data, int len)
{
	return hci_cmd_dispatch(HCI_CMD_SEND_BLE_DATA, handle, data, len);
}

/**
 * @brief   根据OPT，组装HCI格式数据，发送给BLE，并死等ack应答数据
 */
BLE_ERR_E ble_config_op(ble_hci_cmd_E opcode, void *param, uint32_t len)
{
	char ascii_data[10] = {0};
	uint8_t param_len = 0;
	BLE_ERR_E ret = BLE_OK;

	switch (opcode)
	{
	case HCI_CMD_VERSION_REQUEST:
	case HCI_CMD_POWER_REQ:
	case HCI_CMD_READ_GPIO:
	case HCI_CMD_ADD_SERVICE_UUID:
	case HCI_CMD_ADD_CHARACTERISTIC_UUID:
	case HCI_CMD_LE_GET_FREQ_OFFSET:
	case HCI_CMD_LE_ADV_TYPE:
		ret = hci_cmd_dispatch(opcode, g_working_info->chhandle_none, param, len);
		break;
	case HCI_CMD_SET_UART_BAUD:
		param_len = int_to_ascii((*(int *)param), ascii_data, 1);
		ret = hci_cmd_dispatch(opcode, g_working_info->chhandle_none, (uint8_t *)ascii_data, param_len);
		break;
	default:
		ret = hci_cmd_dispatch(opcode, g_working_info->chhandle_none, param, len);
		break;
	}
	return ret;
}


/*HCI_EVENT_CMD_RES确认命令的处理*/
__RAM_FUNC void ble_ack_cmd_result_proc(ble_msg_t *msg)
{
	/*0表示ack成功*/
	if(msg->payload[1] == 0)
	{
		g_opt_acked = 1;
		g_ack_opt_debug = msg->payload[0];

		/*问答时opt，进而必须一致*/
		xy_assert(g_req_opt == g_ack_opt_debug);

		/*根据发送的opt进行区分*/
		switch ((uint8_t)(msg->payload[0]))
		{
		case HCI_CMD_VERSION_REQUEST:
			g_ble_rsp_info->firmware_ver = *(unsigned short *)(&msg->payload[2]);
			break;

		case HCI_CMD_POWER_REQ:
			g_ble_rsp_info->power_val = *(uint16_t *)(&msg->payload[2]);
			break;

		case HCI_CMD_READ_GPIO:
			g_ble_rsp_info->gpio_state = *(uint8_t *)(&msg->payload[2]);
			break;

		case HCI_CMD_LE_GET_FREQ_OFFSET:
			g_ble_rsp_info->freqoffset = *(uint16_t *)(&msg->payload[2]);
			break;

		default:
			break;
		}
	}
}

/*临时缓存BLE芯片告知的动态handle句柄值*/
uint16_t g_hanlde_val = 0;

/*在中断函数中，快速识别BLE芯片发送来的ACK等消息，以减轻main主函数的处理压力*/
__RAM_FUNC uint8_t check_ble_cmd_rsp(uint8_t *buf)
{
	uint8_t ret = 0;
	ble_msg_t *hci_msg = (ble_msg_t *)buf;
	if (hci_msg->opcode == HCI_EVENT_CMD_RES)
	{
		ble_ack_cmd_result_proc(hci_msg);
		ret = 1;
	}
	else if(hci_msg->opcode == HCI_EVENT_STATUS_RES)
	{
		g_opt_acked = 1;
		g_ble_rsp_info->status_res = *(uint8_t *)hci_msg->payload;
		if (g_ble_rsp_info->status_res & 0x20)
		{
			g_working_info->connected = 1;
		}
		else
		{
			g_working_info->connected = 0;
		}
		ret = 1;
	}
	/*无法通过payload[0]来识别opt，进而通过全局识别*/
	else if(hci_msg->opcode == HCI_EVENT_UUID_HANDLE)
	{
		if ((*(uint16_t *)hci_msg->payload) == HCI_CMD_LE_ADV_TYPE)
		{
			g_opt_acked = 1;
		}
		else
		{
			if (g_req_opt == HCI_CMD_ADD_SERVICE_UUID)
			{
				g_opt_acked = 1;
				g_ble_rsp_info->service_handle = *(unsigned short *)hci_msg->payload;
			}
			else if (g_req_opt == HCI_CMD_ADD_CHARACTERISTIC_UUID)
			{
				g_opt_acked = 1;
				g_hanlde_val = *(unsigned short *)hci_msg->payload;
			}
		}
		ret = 1;
	}

	return ret;
}

uint8_t ble_pairmode_select(uint8_t pairmode)
{
    uint8_t pair_byte = 0x00;
    switch(pairmode)
    {
        case BLE_PAIRING_NONE:
            pair_byte = 0x00;
            break;
        case BLE_PAIRING_JUSTWORK:
            pair_byte = 0x01;
            break;
        case BLE_PAIRING_PASSKEY:
            pair_byte = 0x02;
            break;
        case BLE_PAIRING_SC_JUSTWORK:
            pair_byte = 0x81;
            break;
        case BLE_PAIRING_SC_PASSKEY:
            pair_byte = 0x83;
            break;
        default:
            pair_byte = 0x00;
            break;
    }
    return pair_byte;
}


BLE_ERR_E channel_create_init()
{
	BLE_ERR_E ret = BLE_OK;
	BLE_Handle_E ble_chuuid_data;
	
	ble_chuuid_data = BLE_CHUUID_AT;
	ret = ble_config_op(HCI_CMD_ADD_CHARACTERISTIC_UUID, &ble_chuuid_data, 5);

	g_working_info->at_handle = g_hanlde_val;

	// 获取fota uuid
	ble_chuuid_data = BLE_CHUUID_FOTA;
	ret = ble_config_op(HCI_CMD_ADD_CHARACTERISTIC_UUID, &ble_chuuid_data, 5);

	g_working_info->fota_handle = g_hanlde_val;

	// 获取log uuid
	ble_chuuid_data = BLE_CHUUID_LOG;
	ret = ble_config_op(HCI_CMD_ADD_CHARACTERISTIC_UUID, &ble_chuuid_data, 5);

	g_working_info->log_handle = g_hanlde_val;


	// 获取透传 uuid
	ble_chuuid_data = BLE_CHUUID_PASSTHROUGH;
	ret = ble_config_op(HCI_CMD_ADD_CHARACTERISTIC_UUID, &ble_chuuid_data, 5);

	g_working_info->default_handle = g_hanlde_val;

	// 获取fota cmd uuid
	ble_chuuid_data = BLE_CHUUID_FOTA_CMD;
	ret = ble_config_op(HCI_CMD_ADD_CHARACTERISTIC_UUID, &ble_chuuid_data, 5);

	g_working_info->fotacmd_handle = g_hanlde_val;

	xy_printf("BLE handle AT:%d FT:%d LG:%d PS:%d FC:%d",g_working_info->at_handle,g_working_info->fota_handle,
	g_working_info->log_handle,g_working_info->default_handle,g_working_info->fotacmd_handle);
	return ret;
}


int ble_Boot_init_cmd(uint8_t cmd_type)
{
	int ble_echo_state = BLE_FALSE;
	ble_msg_node_t *msg_node = NULL;
	uint8_t cmd[4] = {BLE_CMD,cmd_type,BLE_CMD_OGF,0x00};
	uint8_t rsp[7] = {0x04, 0x0e, 4, 1, cmd_type, BLE_CMD_OGF, 0x00};
	uint32_t tickstart = Get_Tick();

	ble_uart_write(cmd, 4);
	//等待ble_uart_write触发的BLE_CMD命令的回复
	while (GetListNum(&g_ble_msg_head) == 0 && !Check_Ms_Timeout(tickstart,BLE_WAIT_CMP_MS));

	while ((msg_node = (ble_msg_node_t *)ListRemove(&g_ble_msg_head)) != NULL)
	{
		if (strncmp((char *)msg_node->data, (char *)rsp, 7) == 0 && msg_node->len == msg_node->data[2] + HCI_HEADER_LEN)
		{
			ble_echo_state = BLE_TRUE;
		}
		else
		{
			ble_echo_state = BLE_FALSE;
		}
		xy_free(msg_node);
		msg_node = NULL;
	}
	return ble_echo_state;
}


/*通知BLE芯片进行波特率切换*/
int ble_send_baud_cmd(uint32_t baud)
{
    if(0 == baud)
    {
        return BLE_FALSE;
    }
    
    uint8_t cmd[6] = {0};
    uint32_t baud_param = (uint32_t)BLE_UART_CLOCK/(uint32_t)baud;

    cmd[0] = BLE_CMD;
    cmd[1] = BLE_CMD_BAUD; 
    cmd[2] = BLE_CMD_OGF;
    cmd[3] = 0x02;
    cmd[4] = (uint8_t)(baud_param & 0xff);
    cmd[5] = (uint8_t)(baud_param >> 8 & 0x0f);

    ble_uart_write(cmd,6);
    
    //delay 2.5ms
    for(volatile int delay=0; delay<580; delay++);

    return BLE_TRUE;
}


/*BOOT完成后，收到BLE芯片的HCI_EVENT_STANDBY_REP上报后，进行BLE芯片初始化配置。如果BLE默认一致，无需再次配置*/
BLE_ERR_E ble_config_init()
{
	char ble_mac[7] = {0}; 
	uint8_t ble_name_len = (uint8_t)strlen((const char *)g_ble_fac_nv->ble_name);
	BLE_ERR_E ret = BLE_OK;

	if ((g_ble_fac_nv->ble_mac[0] + g_ble_fac_nv->ble_mac[1] + g_ble_fac_nv->ble_mac[2] + g_ble_fac_nv->ble_mac[3] + g_ble_fac_nv->ble_mac[4] + g_ble_fac_nv->ble_mac[5]) == 0)
	{
		uint64_t rand_mac;
		
		xy_seed();		
		rand_mac = (uint64_t)get_rand_val(0,0XFFFFFFFF);

		memcpy(ble_mac, &rand_mac, 6);
		memcpy(g_ble_fac_nv->ble_mac, ble_mac, 6);
	}

	if ((!strcmp((char *)g_ble_fac_nv->ble_name, "XY_3100E")))
	{
		strcat((char *)g_ble_fac_nv->ble_name, "-");

		ble_name_len = (uint8_t)strlen((const char *)g_ble_fac_nv->ble_name);
		if (ble_name_len <= 10)
		{
			bytes2hexstr(g_ble_fac_nv->ble_mac, 3, ble_mac, 7);
			memcpy(g_ble_fac_nv->ble_name + ble_name_len, ble_mac, 6);
		}

		ble_name_len = (uint8_t)strlen((const char *)g_ble_fac_nv->ble_name);
	}

	xy_printf("BLE name:%s",g_ble_fac_nv->ble_name);

	// 设置蓝牙名称
	if (ble_name_len)
		ret = ble_config_op(HCI_CMD_SET_BLE_NAME, g_ble_fac_nv->ble_name, strlen((char*)g_ble_fac_nv->ble_name));
	SAVE_BLE_PARAM(ble_name);

	// 设置蓝牙是否开启广播
	char ble_vis = 4;
    if(g_ble_fac_nv->broadcast == 0)
    {
        ble_vis = 0;
    }
    ret = ble_config_op(HCI_CMD_SET_VISIBILITY, &ble_vis, sizeof(uint8_t));

	// 设置蓝牙广播周期
	uint16_t advt = (g_ble_fac_nv->advt_max + g_ble_fac_nv->advt_min) / 2;
	ret = ble_config_op(HCI_CMD_LE_SET_ADV_PARM, &advt, sizeof(uint16_t));

	// 设置蓝牙mac地址
	memcpy(ble_mac, g_ble_fac_nv->ble_mac, 6);
	str_back_order(ble_mac, 6);
	ret = ble_config_op(HCI_CMD_SET_BLE_ADDR, ble_mac, 6);
	SAVE_BLE_PARAM(ble_mac);

	// 设置蓝牙发射功率
	RF_power_E ble_rfpw = g_RF_power_list[g_ble_fac_nv->rf_power];
	ret = ble_config_op(HCI_CMD_SET_TX_POWER, &ble_rfpw, sizeof(uint8_t));


	// 设置蓝牙发射频偏
	int8_t ble_rfparam = get_ble_freqoffset();
	ret = ble_config_op(HCI_CMD_LE_SET_FREQ_OFFSET, (uint8_t *)&ble_rfparam, 1);

	extern uint8_t g_ble_paried;
	// 配对模式配置
	if (g_ble_fac_nv->pairing_mode != 0)
	{
		g_ble_paried = 1;
		uint8_t pair_byte = 0;
		pair_byte = ble_pairmode_select(g_ble_fac_nv->pairing_mode);
		ret = ble_config_op(HCI_CMD_LE_SET_PAIRING, &pair_byte, sizeof(uint8_t));

		// 配对验证码配置
		if (2 == g_ble_fac_nv->pairing_mode || 4 == g_ble_fac_nv->pairing_mode)
		{
			if (g_ble_fac_nv->passkey)
			{
				uint8_t passkey_payload[5] = {0};
				passkey_payload[0] = 0x01;
				memcpy(passkey_payload + 1, &g_ble_fac_nv->passkey, 4);
				ret = ble_config_op(HCI_CMD_LE_SET_FIXED_PASSKEY, passkey_payload, 5);

				g_ble_fac_nv->key_mode = 1;
			}
		}
	}
	else
	{
		g_ble_paried = 0;
		if (ble_set_pairing_mode(0) != BLE_OK)
		{
			return ret;
		}
	}

	// 恢复BLE NVRAM
	extern BLE_ERR_E restore_nvram_to_ble();
	if (restore_nvram_to_ble() != BLE_OK)
	{
		goto end;
	}

	// 获取service uuid
	ble_uuid ble_uuid_data = BLE_UUID_SYS;
	ret = ble_config_op(HCI_CMD_ADD_SERVICE_UUID, &ble_uuid_data, 3);
	
	if(channel_create_init() != BLE_OK)
		goto end;
	
	// 设置MTU大小
	if ((g_ble_fac_nv->mtusize != 0) && (g_ble_fac_nv->mtusize != 512))
	{
		ret = ble_config_op(HCI_CMD_LE_SET_MTUSIZE, &g_ble_fac_nv->mtusize, 2);
	}

	ret = ble_config_op(HCI_CMD_STATUS_REQUEST, NULL, 0);

end:
	return ret;
}


