/** 
* @file        
* @brief   该头文件为BLE的hci协议相关，包含组包等
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
#include <stdint.h>
#include "factory_nv.h"
#include "ble_api.h"
#include "xy_ftl.h"

/*发送HCI命令同步等待命令完成的时间，ms*/
#define BLE_WAIT_CMP_MS    1000

/*BLE系统初始化命令内容*/
#define BLE_CMD        0x01
#define BLE_CMD_OGF    0xfc
#define BLE_CMD_RESET  0x00  //蓝牙efuse复位
#define BLE_CMD_BAUD   0x02  //启动阶段修改蓝牙波特率
#define BLE_CMD_ECHO   0x05  //确认波特率是否切换成功
#define BLE_BOOT_EVENT 0x04  //命令回复的首字节内容

/*链接相关参数宏*/
#define     BLE_CONN_MIN      10
#define     BLE_CONN_MAX      10
#define     BLE_CONN_LATENCY  5
#define     BLE_CONN_TIMEOUT  50


#define BLE_CONN_HANDLE    (51)//默认上报的ble连接句柄,同透传通道handle值

#define HCI_HEADER_LEN     (3)	// HCI码流头三字节，对应ble_hci_msg_t的前三参数, Byte0:传输方向, Byte1:opcode, Byte2:payload length
#define DATA_HANDLE_LEN    (2) // HCI数据码流对应的handle变量长，存储在ble_hci_msg_t的payload头两个字节

#define PAYLOAD_MAX_LEN    (0xF6)  //HCI数据码流的总长度，含2个字节handle长度，即ble_hci_msg_t的payload的最大长度
#define PAYLOAD_DATA_MAX_LEN  (PAYLOAD_MAX_LEN - DATA_HANDLE_LEN)    //HCI数据码流的有效总长度，不包含2个字节handle长度





/*opcode操作码,用于发送命令给BLE芯片，需要等待ble_hci_cmd_E的确认，直至全局g_opt_acked置为1*/
typedef enum
{
	HCI_CMD_SET_BT_ADDR = 0x00,				  // 设置 BT3.0 地址
	HCI_CMD_SET_BLE_ADDR = 0x01,			  // 设置 BLE 地址
	HCI_CMD_SET_VISIBILITY = 0x02,			  // 设置可发现和广播
	HCI_CMD_SET_BT_NAME = 0x03,				  // 设置 BT3.0 名称
	HCI_CMD_SET_BLE_NAME = 0x04,			  // 设置 BLE 名称
	HCI_CMD_SEND_SPP_DATA = 0x05,			  // 发送 BT3.0（SPP）数据
	HCI_CMD_SEND_BLE_DATA = 0x09,			  // 发送 BLE 数据,所有数据面数据都使用该ID，通过handle区分物理通道
	HCI_CMD_STATUS_REQUEST = 0x0B,			  // 请求蓝牙状态
	HCI_CMD_SET_PAIRING_MODE = 0x0C,		  // 设置BT3.0配对模式
	HCI_CMD_SET_PINCODE = 0x0D,				  // 设置配对码
	HCI_CMD_SET_UART_FLOW = 0x0E,			  // 设置 UART 流控
	HCI_CMD_SET_UART_BAUD = 0x0F,			  // 设置 UART 波特率
	HCI_CMD_VERSION_REQUEST = 0x10,			  // 查询模块固件版本
	HCI_CMD_BT_DISCONNECT = 0x11,			  // 断开 BT3.0 连接
	HCI_CMD_BLE_DISCONNECT = 0x12,			  // 断开 BLE 连接
	HCI_CMD_SET_NVRAM = 0x26,				  // 下发 NV 数据
	HCI_CMD_CONFIRM_GKEY = 0x28,			  // Numeric Comparison 配对方式中 对密钥的比较
	HCI_CMD_SET_CREDIT_GIVEN = 0x29,		  // 设置 Spp 流控
	HCI_CMD_SET_ADV_DATA = 0x2A,			  // 设置 ADV 数据
	HCI_CMD_POWER_REQ = 0x2B,				  // 查询模块电源电压
	HCI_CMD_POWER_SET = 0x2C,				  // 读取电源电压功能开关
	HCI_CMD_PASSKEY_ENTRY = 0x30,			  // 输入 Passkey
	HCI_CMD_SET_GPIO = 0x31,				  // 初始化 gpio
	HCI_CMD_READ_GPIO = 0x32,				  // 读取 gpio 状态
	HCI_CMD_LE_SET_PAIRING = 0x33,			  // 设置BLE配对模式
	HCI_CMD_LE_SET_ADV_DATA = 0x34,			  // 设置 adv 数据
	HCI_CMD_LE_SET_SCAN_DATA = 0x35,		  // 设置 scan 数据
	HCI_CMD_LE_SEND_CONN_UPDATE_REQ = 0x36,	  // 更新连接参数
	HCI_CMD_LE_SET_ADV_PARM = 0x37,			  // 设置广播参数
	HCI_CMD_LE_START_PAIRING = 0x38,		  // 开始配对
	HCI_CMD_SET_WAKE_GPIO = 0x40,			  // 设置唤醒 IO
	HCI_CMD_SET_TX_POWER = 0x42,			  // 设置发射功率
	HCI_CMD_LE_CONFIRM_GKEY = 0x48,			  // Ble Numeric Comparison 配对方 式中对密钥的比较
	HCI_CMD_REJECT_JUSTWORK = 0x49,			  // 拒绝 justwork 配对方式（pci 认 证时候使用）
	HCI_CMD_RESET_CHIP_REQ = 0x51,			  // 复位芯片
	HCI_CMD_LE_SET_FIXED_PASSKEY = 0x61,	  // 设置固定的 passkey
	HCI_CMD_DELETE_CUSTOMIZE_SE_RVICE = 0x76, // 删除 BLE 非系统服务及特征
	HCI_CMD_ADD_SERVICE_UUID = 0x77,		  // 增加 BLE 自定义服务，对应宏值ble_uuid
	HCI_CMD_ADD_CHARACTERISTIC_UUID = 0x78,	  // 增加 BLE 自定义特征，对应宏值ble_characteristic
	HCI_CMD_LE_ADV_TYPE = 0x7C,				  // 更改广播类型
	HCI_CMD_LE_GET_FREQ_OFFSET = 0xA0,		  // 获取频偏值
	HCI_CMD_LE_SET_FREQ_OFFSET = 0xA1,		  // 设置频偏值
	HCI_CMD_LE_SET_HIBERNATE = 0xA4,		  // 进入hibernate
	HCI_CMD_LE_SET_MTUSIZE = 0xA5,			  // 设置mtu大小
	HCI_DEFAULT_HCI_CMD = 0xF0,
	HCI_TEST_CMD_CLOSE_LPM = 0xFF // 关闭 LPM
} ble_hci_cmd_E;


/*BLE芯片发送给NB的事件ID，部分为ble_hci_cmd_E的确认，也存在主动上报事件*/
typedef enum
{
	HCI_EVENT_SPP_CONN_REP = 0x00,		  // BT3.0 连接建立
	HCI_EVENT_LE_CONN_REP = 0x02,		  // BLE 连接建立,有终端用户接入，可以进行远程通信
	HCI_EVENT_SPP_DIS_REP = 0x03,		  // BT3.0 连接断开
	HCI_EVENT_LE_DIS_REP = 0x05,		  // BLE 连接断开
	HCI_EVENT_CMD_RES = 0x06,			  // 命令已完成，为ble_hci_cmd_E的确认
	HCI_EVENT_SPP_DATA_REP = 0x07,		  // 接收到 BT3.0 数据（SPP）
	HCI_EVENT_LE_DATA_REP = 0x08,		  // 接收到BLE发送来的数据面数据
	HCI_EVENT_STANDBY_REP = 0x09,		  // BLE芯片boot完备，通知AP主控进行handle等创建
	HCI_EVENT_STATUS_RES = 0x0A,		  // 状态回复
	HCI_EVENT_NVRAM_REP = 0x0D,			  // 上传 NVRAM 数据，替BLE芯片保存历史信息
	HCI_EVENT_GKEY = 0x0E,				  // 发送 Numeric Comparison 配对方式 中产生的密钥
	HCI_EVENT_INVALID_PACKET = 0x0F,	  // 通知 MCU 发出的包格式错误
	HCI_EVENT_GET_PASSKEY = 0x10,		  // 通知 MCU 可以下发 Passkey
	HCI_EVENT_LE_TK = 0x11,				  // BLE PASSKEY 配对方式中通 知 MCU 返回密钥
	HCI_EVENT_LE_PAIRING_STATE = 0x14,	  // 通知 MCU BLE 的配对状态
	HCI_EVENT_LE_ENCRYPTION_STATE = 0x15, // 通知 MCU 当前加密状态
	HCI_EVENT_LE_GKEY = 0x1D,			  // Numeric Comparison 配对方式中 产生的密钥
	HCI_EVENT_UUID_HANDLE = 0x29		  // 通知 MCU 新设置的 UUID 对应的 Handle
} hci_recv_event_E;


/*默认的服务句柄，用于挂载多个handle*/
typedef enum
{
	BLE_UUID_SYS          = 0xff0102,//bytes0:uuid's length,bytes1-2:uuid(LittleEndian)
}ble_uuid;

/*特征，即数据传输句柄，不支持客户自定义*/
typedef enum
{
    BLE_CHUUID_AT          = 0x01fe010218,//bytes0:enum ble_characteristic_bit,bytes1:uuid's length,bytes2-3:uuid(LittleEndian),bytes4:0x01
    BLE_CHUUID_FOTA        = 0x01fe020218, //FOTA差分包码流通道
    BLE_CHUUID_LOG         = 0x01fe030218,
    BLE_CHUUID_PASSTHROUGH = 0x01fe040218,
    BLE_CHUUID_FOTA_CMD    = 0x01fe050218, //FOTA控制命令通道
}BLE_Handle_E;

/*配对模式*/
typedef enum
{
    BLE_PAIRING_NONE = 0,
    BLE_PAIRING_JUSTWORK,
    BLE_PAIRING_PASSKEY,
    BLE_PAIRING_SC_JUSTWORK,
    BLE_PAIRING_SC_PASSKEY
}pairing_mode_E;

typedef enum
{
    BLE_RFPOWER_0DB    = 0,
    BLE_RFPOWER_3DB    = 0x03,
    BLE_RFPOWER_5DB    = 0x05,
    BLE_RFPOWER_6DB    = 0x06,
    BLE_RFPOWER_7DB    = 0x07,
    BLE_RFPOWER_10DB   = 0x0a,
    BLE_RFPOWER_F3DB   = 0x83,
    BLE_RFPOWER_F5DB   = 0x85,
    BLE_RFPOWER_F20DB  = 0x94,
    BLE_RFPOWER_F30DB  = 0x9e
}RF_power_E;

/*将来可以考虑删除！！！hci_recv_event_E收到的命令类携带的BLE芯片的特征值，一般仅做查询或调试使用*/
typedef struct{
	uint32_t le_tk;          //在BLE PASSKEY配对方式中，TK值校验
	uint32_t le_key;         //在BLENumericComparison配对方式中产生的密钥，与配对另一方比较后发送
	uint16_t firmware_ver;   //蓝牙固件版本号; complete event
	uint16_t power_val;      //蓝牙模块电源电压; complete event
	//unsigned short  adv_type;     //HCI_CMD_LE_ADV_TYPE，返回opcode，无需记录 cmd event
	uint16_t service_handle; //蓝牙收到UUID相关CMD后回复当前UUID的handle; cmd event
	uint16_t pairing_state;  //蓝牙发起配对时，模块向MCU发送的数据
	uint8_t gpio_state;     //蓝牙gpio高低电平;complete event
	uint8_t	 *nvram_rep;     //模块需要将 NVRAM 数据保存至 MCU 时发送的数据
	uint8_t	 le_encryption;  //在收到加密指令之后，模 块 会 向 MCU发 送 加 密 状 态
	uint8_t	 status_res;     //回复HCI_CMD_STAUS_REQUEST，蓝牙连接和可发现状态;  cmd event
    uint16_t freqoffset;     //获取频偏值
}rcv_payload_info_T;



extern rcv_payload_info_T *g_ble_rsp_info;


extern uint8_t g_req_opt;

/*仅调试使用！当BLE_HCI_PACKETTYPE_EVENT时，记录对应的操作码请求，应该与g_req_opt一样*/
extern uint8_t g_ack_opt_debug;


/**
 * @brief  HCI数据面数据的发送接口
 */
BLE_ERR_E hci_send_data(uint16_t handle, void *data, int len);


/**
 * @brief  增强BLE传输能力，加快数据传输速率，常见于FOTA差分包等大数据ymodem 
 */
void enhance_send_capability();


/**
 * @brief 向蓝牙模块发送配置类命令
 */
BLE_ERR_E ble_config_op(ble_hci_cmd_E opcode, void *param, uint32_t len);


/**
  * @brief 将配对模式值转换成hci协议要求的值
  */
uint8_t ble_pairmode_select(uint8_t pairmode);

