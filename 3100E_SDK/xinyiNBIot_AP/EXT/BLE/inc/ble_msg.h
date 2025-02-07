/** 
* @file        
* @brief   该头文件为BLE的ringbuf相关文档
* @warning     
*/
#pragma once
#include <stdint.h>
#include "xy_list.h"


typedef struct
{
	struct List_t *next;
	uint16_t len;  /*ble_msg_t实体长度*/
	uint8_t  data[0];  /*ble_msg_t实体*/
} ble_msg_node_t;



/*ble_main主函数的消息类型*/
typedef enum
{
	/*HCI物理消息类型，指示与BLE芯片通信的方向*/
	HCI_SEND_EVENT = 0x01,   /*发送给BLE芯片的消息*/
	HCI_RECV_EVENT = 0x02,   /*接收到的BLE芯片消息*/


	/*以下为ble_main主函数扩展的外部消息类型*/
	MSG_SEND_AT_RSP = 0x0A,
	MSG_SEND_LOG = 0x0B,
} ble_msg_E;


/*BLE主函数消息结构体，用于HCI上报消息或AP核本地发送给BLE的消息*/
typedef struct
{
	uint8_t type;         /*消息类型，see @ref ble_msg_E*/
	uint8_t opcode;       /*仅对HCI消息有意义。发送时，对应ble_hci_cmd_E；接收时，对应ble_hci_event*/
	uint8_t length;       /*payload长度。如opcode为HCI_CMD_SEND_BLE_DATA或HCI_EVENT_LE_DATA_REP时，则包含2字节的handle长*/
	uint8_t payload[0];   /*对于接收而言，当opcode为HCI_EVENT_CMD_RES时，1字节表示发送的opt；2字节为0表示ACK*/
} ble_msg_t;


extern ListHeader_t g_ble_msg_head;



/******************************************************************************************************************************
 * @brief  供AP核业务模块发送字符串给主函数，与HCI底层复用一个消息结构体
 * @param  type   see @ref ble_msg_E
 * @param  data   待发送的数据内容
 * @note   通常用于发送AT应答结果给对端蓝牙
 ******************************************************************************************************************************/
void send_str_to_main(uint8_t type,void *data,int len);

/******************************************************************************************************************************
 * @brief  供AP核业务模块发送数据给主函数，与HCI底层复用一个消息结构体
 * @param  type   see @ref ble_msg_E
 * @param  data   待发送的数据内容
 * @note   数据会直接透传，可用于发送log到对端蓝牙
 ******************************************************************************************************************************/
void send_data_to_main(uint8_t type,void *data,int len);






void ble_clear_event_msg(void);
void ble_event_interupt_proc(uint8_t *data, uint16_t len);

