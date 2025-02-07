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
#include "ble_fota.h"
#include "ble_ymodem.h"
#include "ble_hci.h"
#include "xy_fota.h"

/**************************************************************************************************
** YMODEM数据包格式为：帧头+包号+包号反码+ data +校验高位+校验低位，参阅：https://blog.csdn.net/huangdenan/article/details/103611081
***************************************************************************************************/ 

ymodem_info_T *g_ymodem_head = NULL; //ymodem协议全局

void ymodem_init()
{
	if(g_ymodem_head == NULL)
	{
		g_ymodem_head = xy_malloc(sizeof(ymodem_info_T));
		memset((void *)g_ymodem_head,0,sizeof(ymodem_info_T));
		g_ymodem_head->buf = xy_malloc(PACKET_1K_SIZE);
		g_ymodem_head->max_len = PACKET_SIZE;	
	}
	else
	{
		memset(g_ymodem_head, 0x00, sizeof(ymodem_info_T) - 4);
	}
}

void ymodem_deinit()
{
	if(g_ymodem_head != NULL)
	{
		if(g_ymodem_head->buf != NULL)
			xy_free(g_ymodem_head->buf);

		xy_free(g_ymodem_head);
		g_ymodem_head = NULL; 
	}
}

/**************************************************************************************************
** 函数名称 : ymodem_crc16_check
** 功能描述 : 检测YMODEM数据包校验值
** 入口参数 : <data>[in] 接收到的数据
			 <length>[in] 接收的数据长度
** 返 回 值 :   true 
			   flase
** 其他说明 : 无
***************************************************************************************************/  
bool ymodem_crc16_check(uint8_t *data, uint32_t len)
{
	uint16_t crcin = 0x0000;
	uint16_t cpoly = 0x1021;
	uint16_t crc16 = (data[len] << 8) + data[len+1];
	uint8_t temp = 0;

	while (len--)
	{
		temp = *(data++);
		crcin ^= (temp << 8);
		for(int i = 0;i < 8;i++)
		{
			if(crcin & 0x8000)
				crcin = (crcin << 1) ^ cpoly;
			else
				crcin = crcin << 1;
		}
	}

	return (crcin == crc16?true:false);
}

/**************************************************************************************************
** 函数名称 : ymodem_packet_check
** 功能描述 : 从发送方基于串口查询方式接收一个数据包
** 入口参数 : <data>[in] 接收到的数据
			 <length>[in] 接收的数据长度
** 返 回 值 :	0 正常返回
				1 数据包结束
			   -1 数据包错误
			   -2 用户中止传输
			   -3 发送方中止传输
			   -4 校验序号和补码错误
** 其他说明 : 无
***************************************************************************************************/  
int32_t ymodem_packet_check (uint8_t *data, uint16_t length, uint16_t *packet_length)
{
	xy_printf("check ymodem packet c:%d, len:%d \r\n", data[0], length);
	uint16_t packet_size;
	uint8_t c = data[0];
	*packet_length = 0;

	switch (c) // c表示接收到的数据的第一个字节
	{
		case MODEM_SOH: // 数据包开始
			packet_size = PACKET_SIZE;
			break;
		case MODEM_STX:// 正文开始
			packet_size = PACKET_1K_SIZE;
			break;
		case MODEM_EOT: // 数据包结束
			packet_size = 1;
			return 1;
		case MODEM_CAN:  // 发送方中止传输
			if (data[1] == MODEM_CAN)
			{ 
				return -3;//中止传输返回
			}
			else
			{ 
				return -1; 
			}
		case MODEM_ABORT://A
			//用户中止传输
			return -2;
		default:
			return -1;
	}

	uint8_t temp1 = data[PACKET_SEQNO_INDEX];
	uint8_t temp2 = data[PACKET_SEQNO_COMP_INDEX];
	temp2 = ((temp2 ^ 0xff) & 0xff);
	if (temp1 != temp2)// 校验序号和补码
	{     
		xy_printf("temp1 != ((temp2 ^ 0xff) & 0xff)  \r\n");
		return -4;
	}
	
	//接收数据长度与协议规定长度不匹配
	if(length != packet_size)
		return -1;
	
	*packet_length = packet_size;

	return 0;
}

/**************************************************************************************************
** 函数名称 : Send_Byte
** 功能描述 : 检测YMODEM数据包校验值
** 入口参数 : <handle>[in] 发送的BLE数据类型句柄
			 <data>[in] 发送的数据
** 返 回 值 : 无
** 其他说明 : 无
***************************************************************************************************/  
void Send_Byte(uint16_t handle, uint8_t data)
{
	uint8_t temp[3] = {0};
	temp[0] = data;

	if(data == MODEM_CAN)
	{
		temp[1] = data;
		hci_send_data(handle,(uint8_t *)temp,2);
	}
	xy_printf("\r\nsend ymodem ack:%d\r\n", data);
	hci_send_data(handle, (char *)temp, 1);	
}

/**************************************************************************************************
** 函数名称 : ymodem_packaging
** 功能描述 : 将接收到的数据组成YMODEM报文
** 入口参数 : <data>[in] 接收到的数据
			 <length>[in] 接收的数据长度
** 返 回 值 :   true 
			   flase
** 其他说明 : 无
***************************************************************************************************/  
bool ymodem_packaging(uint8_t *data, uint32_t len, uint16_t handle)
{
    //收到第一包数据，初始化全局
    if(len == PACKET_SIZE && data[0] == MODEM_SOH && data[1] == 0 && ((data[len-2] != 0x00) && (data[len-1] != 0x00)))
    {
		ymodem_init();
    }

	xy_printf("ymodem recv:%d %d %d %d", data[0], g_ymodem_head->buf_len, len, g_ymodem_head->max_len);
	if(len < PACKET_1K_SIZE && g_ymodem_head != NULL
		&& (g_ymodem_head->next_num <= (((g_ymodem_head->filelen)+(PACKET_1K_SIZE-5)-1)/(PACKET_1K_SIZE-5))))
	{
		xy_printf("g_ymodem_head: %d %d %d %d", g_ymodem_head->next_num, g_ymodem_head->filelen, g_ymodem_head->buf, len);
        if((g_ymodem_head->buf_len == 0 && data[0] == MODEM_STX) 
            || (g_ymodem_head->buf_len > 0 && g_ymodem_head->buf[0] == MODEM_STX))
            g_ymodem_head->max_len = PACKET_1K_SIZE;
        else
            g_ymodem_head->max_len = PACKET_SIZE;
		if(g_ymodem_head->buf_len + len <= g_ymodem_head->max_len)
        {
            memcpy(g_ymodem_head->buf + g_ymodem_head->buf_len, data, len);
            g_ymodem_head->buf_len += len;

            if(g_ymodem_head->buf_len != g_ymodem_head->max_len)
                return false;//数据不全，等待后续数据
        }
		else
		{
            Send_Byte(handle, MODEM_NAK);//发送应答NAK，接收失败要求重发
            g_ymodem_head->buf_len = 0;
			return false;
		}
	}
	else if(len == 1 && data[0] == MODEM_EOT)
	{
		memcpy(g_ymodem_head->buf, data, len);
		g_ymodem_head->buf_len = len;
		g_ymodem_head->max_len = 1;
	}
	else if(g_ymodem_head->next_num > 0)
	{
		memcpy(g_ymodem_head->buf, data, len);
		g_ymodem_head->buf_len = len;
		g_ymodem_head->max_len = len;
	}

	return true;
}
 
/**************************************************************************************************
** 函数名称 : ymodem_rcv_proc
** 功能描述 : 解析Ymodem协议数据
** 入口参数 : <data> Ymodem包数据
			<len> Ymodem包数据
			<handle> BLE服务句柄
** 出口参数 : <out_buf> 有效数据BUF
** 返 回 值 : 
		>0 有效数据长度
		0  Ymodem协议开始传输数据、结束包、数据暂时不全
		-1 非Ymodem协议包
		-2 数据包序号错误
		-3 文件大小超出限制
		-4 传输完成，传输终止
		-5 数据包分段有错误
***************************************************************************************************/ 
int16_t ymodem_rcv_proc(uint8_t **out_buf, uint8_t *data, uint32_t len, uint16_t handle)
{

	uint16_t session_done=0, file_done=0, ret=0;

	if(ymodem_packaging(data, len, handle) == false)
	{
		xy_printf("ymodem package false");
		return 0;
	}

	xy_printf("ymodem recv oriagen data:");
	switch (ymodem_packet_check(g_ymodem_head->buf, g_ymodem_head->buf_len, &g_ymodem_head->max_len))
	{
		/*起始帧和数据帧*/
		case 0: 
			/*数据帧号不一致*/
			if (g_ymodem_head->buf[PACKET_SEQNO_INDEX] != (uint8_t)g_ymodem_head->next_num) 
			{
				xy_printf("ymodem recv seqnum error:%d %d", g_ymodem_head->buf[PACKET_SEQNO_INDEX], g_ymodem_head->next_num);
				Send_Byte(handle, MODEM_NAK);//发送应答NAK，接收失败要求重发
				return -2;
			}

			xy_printf("ymodem recv packet num:%d", g_ymodem_head->next_num);
			/*起始帧*/
			if ((g_ymodem_head->next_num == 0)) 
			{
				if(ymodem_crc16_check((g_ymodem_head->buf+PACKET_HEADER), g_ymodem_head->buf_len - 5) != 1)
				{ 	//CRC验证不通过
					Send_Byte(handle, MODEM_NAK);
					xy_printf("ymodem data CRC error");
					break;
				}

				/*非结束帧，表明后续还有数据帧，记录文件名和文件长*/
				if ((g_ymodem_head->buf[g_ymodem_head->buf_len-2] != 0x00) && (g_ymodem_head->buf[g_ymodem_head->buf_len-1] != 0x00)) 
				{
					int i;
					uint8_t file_size[MODEM_FILE_SIZE_LENGTH+1];
					char *file_ptr;
					
					// 取出文件名--32B用于存储
					for (i = 0, file_ptr = (char *)(g_ymodem_head->buf + PACKET_HEADER); (*file_ptr != 0) && (i < MODEM_FILE_NAME_LENGTH);)
					{
						g_ymodem_head->filename[i++] = *file_ptr++;
					}
										
					//取出文件大小--2B用于存储
					for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < MODEM_FILE_SIZE_LENGTH);)
					{
						file_size[i++] = *file_ptr++;
					}
					file_size[i++] = '\0';
					
					g_ymodem_head->filelen = atoi((const char*)file_size); //字符转整型    

					xy_printf("ymodem data size:%d", g_ymodem_head->filelen);
					// 文件大小超出限制
					if (g_ymodem_head->filelen > (MODEM_FILE_MAX_SIZE - 1))
					{
						Send_Byte(handle, MODEM_CAN); // 中止通信
						return -3;
					}
					g_ymodem_head->buf_len = 0;
					Send_Byte(handle, MODEM_ACK); // 发送应答ACK
					Send_Byte(handle, MODEM_C); // 发送“C”，等待接收下一包数据包
				}
				else//空文件，读取结束包 MODEM_SOH 00 FF 00…00[128个00] CRCH CRCL
				{
					Send_Byte(handle, MODEM_ACK);
					file_done = 1;  // 文件传输中止
					session_done = 1; // 传输中止
					xy_printf("ymodem endding packet");
					break;
				}
			}
			/*数据帧+结束帧*/
			else
			{
				ret = g_ymodem_head->buf_len - 5;
				g_ymodem_head->rcv_len += ret;
				if(g_ymodem_head->rcv_len > g_ymodem_head->filelen)
				{
					ret -= g_ymodem_head->rcv_len - g_ymodem_head->filelen;
					xy_printf("fota packet received over!");
				}

				*out_buf = g_ymodem_head->buf + PACKET_HEADER;
				g_ymodem_head->buf_len = 0;
				Send_Byte(handle, MODEM_ACK);
			}

			g_ymodem_head->next_num++;  //接收到的数据包加1
			break;
		case 1: //结束帧
			if(g_ymodem_head->eot_count == 0)
			{
				Send_Byte(handle, MODEM_NAK);
				g_ymodem_head->eot_count++;
			}
			else
			{
				Send_Byte(handle, MODEM_ACK);
				Send_Byte(handle, MODEM_C);
				g_ymodem_head->next_num = 0;
				g_ymodem_head->buf_len = 0;
				// file_done = 1;
			}
			break;
		case -1: //数据包格式错误             
			return -1;
		case -2: //由用户输入A(a)中止传输
			Send_Byte(handle, MODEM_CAN); //发送字节CA
			session_done = 1;
			break; 
		case -3: // 由发送方终止传输
			Send_Byte(handle, MODEM_ACK); 
			session_done = 1;  
			break;
		case -4: //  数据包中序号和补码不匹配，终止数据发送           
			Send_Byte(handle, MODEM_ACK);
			file_done = 1;
			break;
		default:
			Send_Byte(handle, MODEM_C);//发送“C”
			break;
	}
	
	if (file_done != 0 || session_done != 0) //文件传输中止
	{
		ymodem_deinit();
		return -4;
	}

	return ret;
}
