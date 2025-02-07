/** 
* @file        
* @brief   该头文件BLE无线通路码流的传输机制，基于不同的物理通道.当前仅考虑FOTA差分包传输
* @warning     
*/
#pragma once


#define MODEM_SOH  0x01     //指示为起始帧，对应长度为PACKET_SIZE，接收正常回应0x06(含文件信息的第一个包接收正常需回应0x06、0x43) 
#define MODEM_STX  0x02 	//指示为普通数据包，对应长度为PACKET_1K_SIZE，接收正常回应0x06
#define MODEM_EOT  0x04 	//发送文件传输结束命令，接收正常回应0x06、0x43(启动空包发送)
#define MODEM_ACK  0x06 	//发送确认应答，接收方crc校验成功或收到已定义的命令
#define MODEM_NAK  0x15 	//发送重传当前数据包请求，接收方crc校验出错
#define MODEM_CAN  0x18 	//发送取消传输命令,连续发送5个字符
#define MODEM_C    0x43 	//发送大写字母C(三种情况下发送该字符: 1.启动通信握手.2.启动数据包发送.3.启动空包发送)
#define MODEM_ABORT 0x44 

#define PACKET_SEQNO_INDEX 0x01
#define PACKET_SEQNO_COMP_INDEX 0x02
#define PACKET_HEADER 3
#define PACKET_SIZE 133      /*控制包的大小*/
#define PACKET_1K_SIZE 495   /*数据包的大小*/
#define MODEM_MAX_ERRORS 5
#define MODEM_FILE_NAME_LENGTH 31
#define MODEM_FILE_SIZE_LENGTH 16
#define MODEM_FILE_MAX_SIZE (4096*200)

typedef struct{ 
    uint32_t  rcv_len; 			//已接收数据的总长度，最终需等于filelen
    uint32_t  filelen;          //Ymodem可有带文件名称和长度 
    uint16_t  next_num;         //待接收帧序号  
    uint8_t   filename[32]; 
	uint8_t   rec_err;          //数据块接收状态 
	uint8_t   eot_count;		//收到的EOT结束符数量
	uint16_t  max_len; 			//当前待接收帧最大长度，参见宏值PACKET_SIZE或PACKET_1K_SIZE
	uint16_t  buf_len;	        //buf中数据长度，最终需等于max_len
	uint8_t  *buf;				//缓存buf
}ymodem_info_T;

int16_t ymodem_rcv_proc(uint8_t **out_buf, uint8_t *data, uint32_t len, uint16_t handle);

