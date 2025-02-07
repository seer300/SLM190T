#include "mcu_adapt.h"
#include "xy_system.h"
#include "system.h"
#include "user_config.h"
#include "sema.h"
#include "at_uart.h"

#define UART_NUM_USED (4)   //使用的uart个数
#define UART_LEN_MAX (256)  //uart最大收发长度，单位字节

uint8_t uart_rx_buff[UART_LEN_MAX] = {0};
uint8_t uart_tx_buff[UART_LEN_MAX] = {0};

uint16_t uart_rx_cnt = 0;
uint16_t uart_tx_cnt = 0;

volatile uint8_t uart_rx_done = 0;
volatile uint8_t uart_tx_done = 0;

uint16_t uart_tx_len = 0;

uint8_t g_uart_num = 0xFF; //初始化为一个无效值

#if AT_LPUART
extern HAL_LPUART_HandleTypeDef g_at_lpuart;
#endif

__RAM_FUNC void UartRxIrqHandle(uint8_t rx_status)
{
    (void)rx_status;
    //读取数据
    McuUartRead(g_uart_num, &uart_rx_buff[uart_rx_cnt]);

    //结束字符判断
    if(uart_rx_buff[uart_rx_cnt] == '\n')
    {
        uart_rx_done = 1;
    }
    uart_rx_cnt++;
  
#if AT_LPUART
    if(g_uart_num == 0)//避免数据接收后立马进入深睡
        g_at_lpuart.RxXferCount = uart_rx_cnt;
#endif

}

__RAM_FUNC void UartTxIrqHandle(void)
{
    //结束判断
    if(uart_tx_cnt >= uart_tx_len)
    {
        uart_tx_done = 1;
    }
    else
    {
        McuUartWrite(g_uart_num, uart_tx_buff[uart_tx_cnt]);//发送数据
    }
    uart_tx_cnt++;
}

#if 0
extern void PrepareSendData(uint32_t length, uint8_t *ptxdata, uint8_t inc);
void Uart_Work()
{
    static uint8_t uart0_init_flag = 0, uart1_init_flag = 0; 
    if(g_uart_num > 3)
        return;

    //测试uart，此外设不受起停CP影响，需要外部串口工具，由外部触发uart工作,uart工作具有随机性(需要关闭深睡)，可能与起停cp碰撞
    if(g_uart_num == 1) 
    {
        if(uart0_init_flag == 0)
        {
            McuUartSet(g_uart_num, 9600, 8, 2, 0, 18, 19); 
            McuUartRxEn(g_uart_num);
            
            McuUartRxIrqReg(g_uart_num, UartRxIrqHandle);
            McuUartTxIrqReg(g_uart_num, UartTxIrqHandle);  

            uart0_init_flag = 1;
        }  
            
        if (uart_rx_done == 1)
        {
            sprintf((char *)uart_tx_buff,"\r\nuart%d received %dbytes, they are %s\r\n",g_uart_num,uart_rx_cnt,uart_rx_buff);
            uart_tx_len = strlen((const char *)uart_tx_buff);                
            
            //打开uart tx，发送第一个数据，发送数组下标加1
            McuUartTxEn(g_uart_num);
            McuUartWrite(g_uart_num, uart_tx_buff[uart_tx_cnt]);
            uart_tx_cnt++;

            //为下次数据接收做准备
            uart_rx_cnt = 0;
            uart_rx_done = 0;
            memset(uart_rx_buff, 0, UART_LEN_MAX);
        }
    

        if (uart_tx_done == 1)
        {
            //关闭uart tx
            McuUartTxDis(g_uart_num);

            //为下次数据发送做准备
            uart_tx_cnt = 0;
            uart_tx_done = 0;

            uart0_init_flag = 0;
        }    
    }  

    //测试lpuart，不受起停CP影响，可唤醒深睡，主要用于起停CP与深睡都开启情况下的测试，需要外部串口工具，由外部触发uart工作,uart工作具有随机性  
    else if(g_uart_num == 0)  
    {
    	uint32_t sema_have = 0;
        if(uart1_init_flag == 0) 
        {
            McuUartSet(g_uart_num, 9600, 8, 2, 0, 18, 19); 
            McuUartRxEn(g_uart_num);
            McuUartTxEn(g_uart_num);
            
            McuUartRxIrqReg(g_uart_num, UartRxIrqHandle);
            //McuUartTxIrqReg(g_uart_num, UartTxIrqHandle);  

            uart1_init_flag = 1;
        }  
            
        if (uart_rx_done == 1)
        {
            sprintf((char *)uart_tx_buff,"\r\nuart%d received %dbytes, they are %s\r\n",g_uart_num,uart_rx_cnt,uart_rx_buff);
            uart_tx_len = strlen((const char *)uart_tx_buff);  
            
            if (CP_Is_Alive() == true)
            {
                /*长URC命令，由于在AP核输出耗时过久造成内存耗尽，进而放在CP核直接输出，通过硬件锁保证AP和CP共抢发送串口*/
                do{
                    SEMA_RequestNonBlocking(SEMA_ATWR_AUX , SEMA_SEMA_DMAC_NO_REQ, SEMA_SEMA_REQ_PRI_0, SEMA_MASK_CP);
                }while(SEMA_MASTER_AP != SEMA_MasterGet(SEMA_ATWR_AUX));
				sema_have = 1;
            }

            McuUartWriteFram(g_uart_num, uart_tx_buff, uart_tx_len);             

            if (sema_have == 1)
            {
                SEMA_Release(SEMA_ATWR_AUX, SEMA_MASK_NULL);
            }         
       
            //为下次数据接收做准备
            uart_rx_cnt = 0;
            uart_rx_done = 0;
#if AT_LPUART
            g_at_lpuart.RxXferCount = 0; //清0，表示lpuart相关处理已完成，保证能够进入睡眠
#endif
            memset(uart_rx_buff, 0, UART_LEN_MAX);
        }
    }
    
    //测试csp2_uart与csp3_uart，受起停CP影响，此接口内为一个完整的uart工作周期(无需关深睡)，以保证uart与起停CP无碰撞，采用回环测试，测试时将TX与RX接起来，无需要外部串口工具

    else 
    {        
        //产生1-10之间的随机数
        srand(Get_Tick());
        uint8_t inc = (uint8_t)(rand() % 10 + 1);
        //准备发送数据
        PrepareSendData(UART_LEN_MAX-2, uart_tx_buff, inc);
        uart_tx_buff[UART_LEN_MAX-2] = '\r';
        uart_tx_buff[UART_LEN_MAX-1] = '\n';
        McuUartSet(g_uart_num, 9600, 8, 2, 0, 18, 19); 
            
        McuUartRxIrqReg(g_uart_num, UartRxIrqHandle);
        McuUartTxIrqReg(g_uart_num, UartTxIrqHandle); 
            
        uart_tx_len = UART_LEN_MAX;
        McuUartTxEn(g_uart_num);
        McuUartRxEn(g_uart_num);
        McuUartWrite(g_uart_num, uart_tx_buff[uart_tx_cnt]);
        uart_tx_cnt++;
        while (uart_tx_done != 1);
        
        //关闭uart tx
        McuUartTxDis(g_uart_num);
        
        while(uart_rx_done != 1);             

        //为下次数据发送做准备
        uart_tx_cnt = 0;
        uart_tx_done = 0;         
        uart_rx_cnt = 0;
        uart_rx_done = 0;
            
        //比较每次读写数据内容，并打印比较结果
        if(memcmp(uart_tx_buff, uart_rx_buff, UART_LEN_MAX) == 0)
        {
            xy_printf("uart%d tramsmit & recieve pass\n",g_uart_num);
        }  
        else
        {
            xy_printf("uart%d tramsmit & recieve fail\n",g_uart_num);
        }
        memset(uart_rx_buff, 0, UART_LEN_MAX);   
        memset(uart_tx_buff, 0, UART_LEN_MAX);   
    }    

    
}
#endif