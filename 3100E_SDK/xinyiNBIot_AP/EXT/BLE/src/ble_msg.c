
#include "ble_msg.h"
#include "ble_drv.h"
#include <stdint.h>
#include "ble_hci.h"
#include "ble_api.h"
#include "ble_main.h"

/* BLE主函数消息链表，含HCI上报事件和AP核本地发送消息*/
ListHeader_t g_ble_msg_head = {0};


/*仅限HCI底层数据的插入链表，以便main主函数处理。*/
__RAM_FUNC static void Insert_BLE_Event_Node(uint8_t *data, uint16_t len,ListHeader_t *list_head)
{
	ble_msg_node_t *pxlist;

	pxlist = xy_malloc(sizeof(ble_msg_node_t) + len);

	pxlist->next = NULL;
	pxlist->len = len;
	memcpy(pxlist->data, data, len);

	ListInsert((List_t *)pxlist,list_head);
}

__RAM_FUNC void ble_conn_state_update(uint8_t *buf)
{
	ble_msg_t *hci_msg = (ble_msg_t *)buf;
	if (hci_msg->opcode == HCI_EVENT_LE_CONN_REP)
	{
		g_working_info->connected = 1;
	}
	else if(hci_msg->opcode == HCI_EVENT_LE_DIS_REP)
	{
		g_working_info->connected = 0;
	}
}


extern uint8_t check_ble_cmd_rsp(uint8_t *buf);

/*中断函数中调用，所有子函数必须用__RAM_FUNC修饰*/
__RAM_FUNC void ble_event_interupt_proc(uint8_t *data, uint16_t len)
{
	ble_msg_t *hci_msg = (ble_msg_t *)data;
	if(hci_msg->type == BLE_BOOT_EVENT)
		goto Do;
	if (hci_msg->type != HCI_RECV_EVENT)
		return;
	if (check_ble_cmd_rsp(data))
		return;
	ble_conn_state_update(data);//连接状态在中断中更新，防止状态更新慢导致流程错误
Do:	do
	{
		uint16_t hci_len = hci_msg->length + HCI_HEADER_LEN;
		if (len == hci_len)
		{
			Insert_BLE_Event_Node(data, hci_len, &g_ble_msg_head);
			break;
		}
		else if(len > hci_len)
		{
			Insert_BLE_Event_Node(data, hci_len, &g_ble_msg_head);
			data += hci_len;
			hci_msg = (ble_msg_t *)data;
			len -= hci_len;
		}
		else
		{
			if(g_working_info->configed == 1)
				xy_assert(0);
			break;
		}
	}while (1);
}

void ble_clear_event_msg(void)
{
	ListFreeAll(&g_ble_msg_head);
}


/*供AP核业务模块发送字符串给主函数，接口内部会在字符串尾部填充'\0'，与HCI底层复用一个消息结构体*/
void send_str_to_main(uint8_t type,void *data,int len)
{
	ble_msg_node_t *pxlist = xy_malloc(sizeof(ble_msg_node_t)+sizeof(ble_msg_t) + len + 1);
	
	ble_msg_t *hci_msg = (ble_msg_t *)pxlist->data;

	hci_msg->type = type;
	memcpy(hci_msg->payload,data,len);
	/*ble_rsp_process中进行AT命令发送，需要尾部加0*/
	*(hci_msg->payload+len) = '\0';

	pxlist->next = NULL;
	pxlist->len = sizeof(ble_msg_t) + len;//长度不包含末尾的\0

	ListInsert((List_t *)pxlist,&g_ble_msg_head);
}

/*供AP核业务模块发送数据给主函数，与HCI底层复用一个消息结构体*/
void send_data_to_main(uint8_t type,void *data,int len)
{
	ble_msg_node_t *pxlist = xy_malloc(sizeof(ble_msg_node_t)+sizeof(ble_msg_t) + len);
	
	ble_msg_t *hci_msg = (ble_msg_t *)pxlist->data;

	hci_msg->type = type;
	memcpy(hci_msg->payload,data,len);

	pxlist->next = NULL;
	pxlist->len = sizeof(ble_msg_t) + len;

	ListInsert((List_t *)pxlist,&g_ble_msg_head);
}