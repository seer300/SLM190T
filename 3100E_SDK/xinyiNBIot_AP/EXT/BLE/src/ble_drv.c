/** 
* @file        
* @brief   该源文件为BLE的底层驱动，包括数据通信串口驱动及IO中断等
* @warning     
*/
#include "xy_system.h"
#include "xy_printf.h"
#include "ble_drv.h"
#include "hal_uart.h"
#include "gpio.h"
#include "ble_api.h"
#include "ble_msg.h"
#include "ble_hci.h"
#include "ble_main.h"
#include "mcu_adapt.h"
#include "at_uart.h"
#include "hal_csp.h"

#define BLE_INTERRUPT_TIME           3		//单位MS，UART硬件超时，最大超时时长为 1/baudrate*1000*31，例如当波特率为9600时，最大超时时长约3ms。
#define BLE_ONCE_DATA_LEN    		 1024   //蓝牙串口一次性接收长度不得超过BLE_ONCE_DATA_LEN
#define BLE_STATE_TIME          	 5000	//单位MS，BLE唤醒NB后NB维持活跃态时间

/*uart数据接收缓存*/
uint8_t *g_ble_recv_data = NULL;
/*调试用uart状态机*/
ble_drv_stat_t *g_ble_drv_stat = NULL;

volatile uint16_t g_ble_rxcnt = 0;

//通过按键开启ble，检测到按键中断时置1
volatile uint8_t g_ble_power_on = 0;

void check_ble_poweron_key()
{
	if(g_ble_power_on == 1)
		ble_open();
	g_ble_power_on = 0;
}


/**
 * @brief ble串口接收回调函数
 * @param rx_status 0：有接收数据产生，1：RX数据线空闲
 */
__RAM_FUNC void bleUart1RxIrqHandle(uint8_t rx_status)
{
    if(rx_status == 0)
    {
        uint8_t data = 0;
        if(McuUartRead(1, &data)) //读取数据
        {
			if(g_ble_rxcnt >= BLE_ONCE_DATA_LEN)
			{
				xy_assert(0);
			}
            g_ble_recv_data[g_ble_rxcnt++] = data;
        }
    }
    else
    {
		if(g_ble_rxcnt == 0)
		{
			xy_printf("BleUart1 Recv Length 0");
		}
		extern int g_blerf_test;
		extern void send_blere_to_main(void *data,int len);
		if(g_blerf_test == 0)
		{
			if(g_ble_rxcnt != 0)
				ble_event_interupt_proc(g_ble_recv_data, g_ble_rxcnt);
		}
		else if(g_blerf_test == 1)
		{
			if(g_ble_rxcnt != 0)
				send_blere_to_main(g_ble_recv_data, g_ble_rxcnt);
		}
        g_ble_rxcnt = 0;
    }
}

/**
 * @brief UART初始化函数，主要完成以下配置：
 * 		  1. UART的TXD、RXD引脚的GPIO初始化配置
 * 		  2. UART的波特率、位宽、奇偶检验模式的配置
 */
__RAM_FUNC void ble_uart_init(void)
{
	if(g_ble_recv_data == NULL)
	{
		g_ble_recv_data = xy_malloc(BLE_ONCE_DATA_LEN);
		memset(g_ble_recv_data, 0, BLE_ONCE_DATA_LEN);
	}

	if( g_ble_drv_stat== NULL)
	{
		g_ble_drv_stat = xy_malloc(sizeof(ble_drv_stat_t));
		memset(g_ble_drv_stat, 0, sizeof(ble_drv_stat_t));
	}

	McuUartSet(1, BLE_UART_CONFIG_BAUDRATE, 8, 2, 0, BLE_UART_TX_PIN, BLE_UART_RX_PIN); //uart1参考时钟频率与Boot_CP无关，Boot_CP后无需重新配置，最大速率支持921600
    McuUartRxEn(1);
    McuUartRxIrqReg(1, bleUart1RxIrqHandle);
	McuUartTxEn(1);

	g_ble_drv_stat->uart_disenable = 0;
}

void ble_uart_write(uint8_t *data, uint32_t len)
{
	McuUartWriteFram(1,(uint8_t *)data,(uint16_t)len);
}

void ble_uartbaud_change(uint32_t baud)
{
	//根据波特率参数配置波特率
	UARTConfigSetExpClk((uint32_t)BLE_UART_PORT, GetlsioFreq(), baud, UART_WORDLENGTH_8 | UART_PARITY_NONE | UART_CTL_ENDIAN_LITTLE);
	//delay 110us
	for(volatile int delay=0; delay<25; delay++);
    
}

volatile uint32_t g_ble_state_tick = 0;
__RAM_FUNC void ble_state_callback(void) //双边沿
{
	g_ble_drv_stat->gpio_int_times++;
	McuGpioModeSet(NB_STATE_PIN, 0x00), McuGpioWrite(NB_STATE_PIN, 1);
	g_ble_state_tick = Get_Tick();
	//g_ble_state_tick = Get_Tick();
}

void ble_gpio_init(void)
{
	McuGpioModeSet(NB_STATE_PIN, 0x00), McuGpioWrite(NB_STATE_PIN, 1);
    McuGpioModeSet(BLE_WAKEUP_PIN, 0x00), McuGpioWrite(BLE_WAKEUP_PIN, 1);

	McuGpioModeSet(BLE_STATE_PIN, 0x13);
    McuGpioIrqReg(BLE_STATE_PIN, ble_state_callback, MCU_GPIO_INT_BOTH);
    McuGpioIrqEn(BLE_STATE_PIN);
}

__RAM_FUNC void ble_poweron_pin_callback(void) // 上升沿中断
{
	if (McuGpioRead(BLE_POWERON_PIN) == 1)
	{
		g_ble_power_on = 1;
	}
}

void ble_powerkey_init()
{
	McuGpioModeSet(BLE_POWERON_PIN, 0x11);
	McuGpioIrqReg(BLE_POWERON_PIN, ble_poweron_pin_callback, MCU_GPIO_INT_RISING);
	McuGpioIrqEn(BLE_POWERON_PIN);
}


/*
  * @brief   蓝牙硬件复位，需要重新下载patch code
  */
void ble_hard_reset(void)
{
	McuGpioModeSet(BLE_RESET_PIN, 0x00), McuGpioWrite(BLE_RESET_PIN, 1);

	//delay 110us(min)
	for(volatile int delay=0; delay<250; delay++);
	McuGpioWrite(BLE_RESET_PIN, 0);//低

	//delay 110us(min)
	for(volatile int delay=0; delay<250; delay++);

	McuGpioWrite(BLE_RESET_PIN, 1);  //高
}

/**发送数据或命令给BLE时，需先通过PIN脚唤醒BLE*/
void ble_wake_up(void)
{
	McuGpioModeSet(NB_STATE_PIN, 0x00), McuGpioWrite(NB_STATE_PIN, 1);
	if(g_ble_drv_stat->lpm_IO_state == 2)
	{
		HAL_Delay(6);//延时等待BLE退出LPM，TEST LEAST 4
		g_ble_drv_stat->lpm_IO_state = 0;
	}
	if(g_ble_drv_stat->uart_disenable == 1)
	{
		UARTEnable((uint32_t)BLE_UART_PORT);
		g_ble_drv_stat->uart_disenable = 0;
	}
}

/**
 * @brief 初始化ble供电和时钟
 * 		  
 */
void ble_power_clock_init()
{
	//给BLE供电
	HWREGB(0x400000A5) |= 0x02;  //vddio_out_user0
	
	//输出26M时钟配置：
	//HWREGB(0x4000485A) |= 0xC;
	HWREGB(0x40004859) &= ~0x40;
	HWREGB(0x40004859) |= 0x80;
	HWREGB(0x4000485A) &= ~0x3;
	HWREGB(0x4000485A) &= ~0x20;
}

/**
 * @brief 去初始化ble供电和时钟
 * 		  
 */
void ble_power_clock_deinit()
{
	//给BLE下电
	HWREGB(0x400000A5) &= ~0x02;  //vddio_out_user0

	//26M时钟配置去初始化：
	HWREGB(0x40004859) |= 0x40;
	HWREGB(0x40004859) &= ~0x80;
	HWREGB(0x4000485A) |= 0x20;
}

/**
 * @brief BLE掉电后对应引脚要设置成输入下拉，防止漏电
 * 		  
 */
void ble_pin_deinit()
{
	McuUartRxDis(1);
	McuUartTxDis(1);//uart失能
	McuGpioIrqDis(BLE_STATE_PIN);//具有唤醒功能的io失能

	McuGpioModeSet(BLE_STATE_PIN, 0x13);//IOLDO1_LPMODE_Enable下不保持
	McuGpioModeSet(NB_STATE_PIN, 0x13);
    McuGpioModeSet(BLE_WAKEUP_PIN, 0x13);
    McuGpioModeSet(BLE_UART_TX_PIN, 0x13);
	McuGpioModeSet(BLE_UART_RX_PIN, 0x13);
	//McuGpioModeSet(BLE_RESET_PIN, 0x13);
}

extern int ble_Boot_init_cmd(uint8_t cmd_type);
extern int ble_send_baud_cmd(uint32_t baud);
void ble_boot_init()
{    
    uint8_t ret = BLE_TRUE;
    uint8_t error_count = 0;

begin:
	ble_clear_event_msg();

    error_count++;
    if(error_count > 10)
    {
        xy_assert(0);
    }

	LPM_LOCK(STANDBY_BLE_LOCK);//BLE初始化成功之前不允许进STANDBY

	ble_uartbaud_change(BLE_UART_START_BAUDRATE);
#if 0
	ble_hard_reset();
#else
    ble_power_clock_deinit();
    ble_pin_deinit();
    //for(volatile int delay=0; delay<250; delay++);
    HAL_Delay(250);
    ble_power_clock_init();
    HAL_Delay(250);
    ble_uart_init();
    ble_gpio_init();

    ble_uartbaud_change(BLE_UART_START_BAUDRATE);
#endif
	HAL_Delay(200);

    ret = ble_Boot_init_cmd(BLE_CMD_RESET);

    if(BLE_TRUE != ret)//efuse失败后需满足进硬重启条件
    {
        Send_AT_to_Ext("ble reset fail\r\n");
        goto begin;
    }

    ble_send_baud_cmd(BLE_UART_CONFIG_BAUDRATE);

    ble_uartbaud_change(BLE_UART_CONFIG_BAUDRATE);

    ret = ble_Boot_init_cmd(BLE_CMD_ECHO);

    if(BLE_TRUE != ret)
    {
        Send_AT_to_Ext("ble echo fail\r\n");
        goto begin;
    }

	extern int ble_patch_init(void);
    ret = ble_patch_init();
    if(BLE_TRUE != ret)
    {
        Send_AT_to_Ext("ble patch init fail\r\n");
        goto begin;
    }
	McuGpioWrite(NB_STATE_PIN, 1);
	McuGpioWrite(BLE_WAKEUP_PIN, 1);
}

/**
 * @brief   开启BLE做的初始化动作,包括uart初始化，蓝牙波特率配置，patch下载等
 */
void ble_start_init()
{
	if(g_working_info == NULL)
	{
		g_working_info = xy_malloc(sizeof(ble_work_info_T));
		memset(g_working_info,0,sizeof(ble_work_info_T));
	}

	if(g_ble_rsp_info == NULL)
	{
		g_ble_rsp_info = xy_malloc(sizeof(rcv_payload_info_T));
		memset(g_ble_rsp_info,0,sizeof(rcv_payload_info_T));
	}

	ble_power_clock_init();
	ble_uart_init();
	ble_gpio_init();
	ble_boot_init();
}

void ble_facnv_init()
{
	if(g_ble_fac_nv == NULL)
	{
		g_ble_fac_nv = xy_malloc(sizeof(BLE_cfg_t));
		if(xy_ftl_read(BLE_FAC_NV_BASE,(void *)g_ble_fac_nv,sizeof(BLE_cfg_t)) ==0)
		{
			memset(g_ble_fac_nv,0,sizeof(BLE_cfg_t));
		}

	}
}

void ble_pin_unused_init()
{
	McuGpioModeSet(NB_STATE_PIN, 0x24);
    McuGpioModeSet(BLE_WAKEUP_PIN, 0x24);
	McuGpioModeSet(BLE_STATE_PIN, 0x24);
    McuGpioModeSet(BLE_UART_TX_PIN, 0x24);
	McuGpioModeSet(BLE_UART_RX_PIN, 0x24);
	McuGpioModeSet(BLE_RESET_PIN, 0x24);
}

/**
 * @brief   开启BLE模块相关的初始化动作，非打开BLE
 */
void ble_module_init()
{
	ble_pin_unused_init();
	ble_facnv_init();
	// ble_powerkey_init();
}

/**
 * @brief  检测当前是否有待收发处理的BLE数据，若未处理则退出睡眠
 * @return false ：BLE数据收发处理未结束，BLE通道处于非IDLE态
 *         true  ：BLE数据收发处理已结束，BLE通道处于IDLE态
 */
__RAM_FUNC bool ble_uart_idle(void)
{
	if(g_ble_rxcnt)
    {
        return false;
    }

    for(volatile int i = 0; i<50; i++);
    //BLEUART_RXFIFO非空，或RX总线活跃
    if((!UARTRxFifoStatusGet(BLE_UART_PORT, UART_FIFO_EMPTY)) || (!UARTRxIdle(BLE_UART_PORT)))
    {
        return false;
    }

	if(!UARTTxFifoStatusGet(BLE_UART_PORT, UART_FIFO_EMPTY))
    {
        return false;
    }

	if(GetListNum(&g_ble_msg_head))
    {
        return false;
    }

    return true;
}

extern int g_blerf_test;
/**
  * @brief BLE进入LPM模式引脚配置
  * @return 1:可以进入睡眠，0：不可进入睡眠
  */
__RAM_FUNC int ble_into_lpm(void)
{
    if(g_working_info->poweron == 0 || g_blerf_test == 1)
    {
        return 1;
    }

	if(!Check_Ms_Timeout(g_ble_state_tick, BLE_STATE_TIME))
	{
		return 0;
	}

    if (ble_uart_idle() == false)
    {
        return 0;
    }

	UARTDisable(BLE_UART_PORT);
    McuGpioModeSet(BLE_WAKEUP_PIN, 0x13);

	McuGpioModeSet(NB_STATE_PIN, 0x13);
	McuGpioModeSet(BLE_UART_RX_PIN, 0x13);
	for(volatile int i = 0; i<20; i++);//延时等待BLE进LPM，防止TX给BLE产生脏数据导致BLE异常
	McuGpioModeSet(BLE_UART_TX_PIN, 0x11);
	g_ble_drv_stat->lpm_IO_state = 1;

	return 1;
}

/**
  * @brief BLE退出LPM后引脚配置
  */
__RAM_FUNC void ble_wakeup_from_lpm()
{
	if(g_working_info->poweron == 0 || g_blerf_test == 1)
    {
        return;
    }
    
    g_ble_state_tick = Get_Tick();
	ble_uart_init();
    McuGpioModeSet(BLE_WAKEUP_PIN, 0x00), McuGpioWrite(BLE_WAKEUP_PIN, 1);
	//McuGpioModeSet(NB_STATE_PIN, 0x00), McuGpioWrite(NB_STATE_PIN, 1);//调整到接收中断和发送数据前
	

	g_ble_drv_stat->lpm_IO_state = 2;
}


//BLE连接仪表测试相关
#define BLERF_RX_PIN GPIO_PAD_NUM_12
#define BLERF_TX_PIN GPIO_PAD_NUM_5
#define BLERF_INTERRUPT_TIME          3
#define BLERF_MAX_LEN          512

uint8_t *g_blerf_data=NULL;

HAL_CSP_HandleTypeDef *g_blerf_uart_handl = NULL;
ListHeader_t g_blerf_msg_head = {0};    // 仪表发送给BLE的消息
ListHeader_t g_blere_msg_head = {0};    // BLE发送给仪表的消息

typedef struct
{
	struct List_t *next;
	uint16_t length;       /*payload长度。*/
	uint8_t payload[0];   /**/
} blerf_msg_t;

__RAM_FUNC void send_rfmsg_to_main(void *data,int len)
{
	blerf_msg_t *node = xy_malloc(sizeof(blerf_msg_t) + len + 1);

	node->length = len;
	memcpy(node->payload,data,len);
	*(node->payload+len) = '\0';

	node->next = NULL;

	ListInsert((List_t *)node,&g_blerf_msg_head);
}

__RAM_FUNC void send_blere_to_main(void *data,int len)
{
	blerf_msg_t *node = xy_malloc(sizeof(blerf_msg_t) + len + 1);

	node->length = len;
	memcpy(node->payload,data,len);
	*(node->payload+len) = '\0';

	node->next = NULL;

	ListInsert((List_t *)node,&g_blere_msg_head);
}

extern int g_blerf_test;
void blerf_send_process()
{
	blerf_msg_t *msg_node = NULL;

	if(g_blerf_test == 0)
		return;

	while(msg_node = (blerf_msg_t *)ListRemove(&g_blere_msg_head))
	{
		extern int write_to_blerf(uint8_t *buf, int size);

		write_to_blerf(msg_node->payload,msg_node->length);

		xy_free(msg_node);
		msg_node = NULL;
	}
}


uint16_t g_blerf_rxcount = 0;
/**
 * @brief ble仪表测试串口接收回调函数
 * @param rx_status 0：有接收数据产生，1：RX数据线空闲
 */
__RAM_FUNC void BleCsp2RxIrqHandle(uint8_t rx_status)
{
    if(rx_status == 0)
    {
        uint8_t data = 0;
        if(McuUartRead(2, &data)) //读取数据
        {
			if(g_blerf_rxcount >= BLERF_MAX_LEN)
			{
				xy_assert(0);
			}
            g_blerf_data[g_blerf_rxcount++] = data;
        }
    }
    else
    {
		send_rfmsg_to_main(g_blerf_data, g_blerf_rxcount);
        g_blerf_rxcount = 0;
    }
}

void BLERF_UART_Init()
{
	if(g_blerf_data == NULL)
	{
		g_blerf_data = xy_malloc(BLERF_MAX_LEN);
		memset(g_blerf_data, 0, BLERF_MAX_LEN);
	}

	McuUartSet(2, HAL_CSP_UART_BAUD_921600, 8, 1, 0, BLERF_TX_PIN, BLERF_RX_PIN); //uart1参考时钟频率与Boot_CP无关，Boot_CP后无需重新配置，最大速率支持921600
    McuUartRxEn(2);
    McuUartRxIrqReg(2, BleCsp2RxIrqHandle);
	McuUartTxEn(2);
}

/**
 * @brief 向blerf对应串口发送数据
 * 		  
 */
int write_to_blerf(uint8_t *buf, int size)
{
	McuUartWriteFram(2,(uint8_t *)buf,(uint16_t)size);
    return 0;
}

int g_blerf_test = 0;

/**
 * @brief BLE连接仪表的数据处理，在main主函数中执行
 * 		  
 */
void blerf_recv_process()
{
	blerf_msg_t *msg_node = NULL;

	if(g_blerf_test == 0)
		return;

	while(msg_node = (blerf_msg_t *)ListRemove(&g_blerf_msg_head))
	{
		ble_uart_write(msg_node->payload,msg_node->length);

		xy_free(msg_node);
		msg_node = NULL;
	}
	blerf_send_process();
}