#include "mcu_adapt.h"
#include "xy_system.h"
#include "system.h"

#if 1 // 1:验证UART功能, 0:验证UART接口耗时

#define UART_LEN_MAX (512)  //uart最大收发长度，单位字节

uint8_t rxbuff0[UART_LEN_MAX] = {0};
uint8_t txbuff0[UART_LEN_MAX] = {0};

uint8_t rxbuff1[UART_LEN_MAX] = {0};
uint8_t txbuff1[UART_LEN_MAX] = {0};

uint8_t rxbuff2[UART_LEN_MAX] = {0};
uint8_t txbuff2[UART_LEN_MAX] = {0};

uint8_t rxbuff3[UART_LEN_MAX] = {0};
uint8_t txbuff3[UART_LEN_MAX] = {0};

struct 
{
    volatile uint16_t rxcnt;
    volatile bool rxdone;
    volatile uint16_t txlen;
    volatile uint16_t txcnt;
    volatile bool txdone;
} uart0 = {0}, uart1 = {0}, uart2 = {0}, uart3 = {0};

/********************************LPUART中断回调函数*************************************/
/**
 * @brief 串口接收回调函数
 * @param rx_status 0：有接收数据产生，1：RX数据线空闲
 */
__RAM_FUNC void Uart0RxIrqHandle(uint8_t rx_status)
{
    if(rx_status == 0)
    {
        uint8_t data = 0;
        if(McuUartRead(0, &data)) //读取数据
        {
            rxbuff0[uart0.rxcnt++] = data;
        }
    }
    else
    {
        uart0.rxdone = true;
    }
}

/**********************************UART中断回调函数*************************************/
/**
 * @brief 串口接收回调函数
 * @param rx_status 0：有接收数据产生，1：RX数据线空闲
 */
__RAM_FUNC void Uart1RxIrqHandle(uint8_t rx_status)
{
    if(rx_status == 0)
    {
        uint8_t data = 0;
        if(McuUartRead(1, &data)) //读取数据
        {
            rxbuff1[uart1.rxcnt++] = data;
        }
    }
    else
    {
        uart1.rxdone = true;
    }
}

/********************************CSP2_UART中断回调函数**********************************/
/**
 * @brief 串口接收回调函数
 * @param rx_status 0：有接收数据产生，1：RX数据线空闲
 */
__RAM_FUNC void Uart2RxIrqHandle(uint8_t rx_status)
{
    if(rx_status == 0)
    {
        uint8_t data = 0;
        if(McuUartRead(2, &data)) //读取数据
        {
            rxbuff2[uart2.rxcnt++] = data;
        }
    }
    else
    {
        uart2.rxdone = true;
    }
}

__RAM_FUNC void Uart2TxIrqHandle(void)
{
    if(uart2.txcnt == uart2.txlen) //发送完成判断
    {
        McuUartTxDis(2);
        uart2.txcnt = 0;
        uart2.txlen = 0;
    }
    else
    {
        McuUartWrite(2, txbuff2[uart2.txcnt++]);
    }
}

/********************************CSP2_UART中断回调函数***********************************/
/**
 * @brief 串口接收回调函数
 * @param rx_status 0：有接收数据产生，1：RX数据线空闲
 */
__RAM_FUNC void Uart3RxIrqHandle(uint8_t rx_status)
{
    if(rx_status == 0)
    {
        uint8_t data = 0;
        if(McuUartRead(3, &data)) //读取数据
        {
            rxbuff3[uart3.rxcnt++] = data;
        }
    }
    else
    {
        uart3.rxdone = true;
    }
}

__RAM_FUNC void Uart3TxIrqHandle(void)
{
    if(uart3.txcnt == uart3.txlen) //发送完成判断
    {
        McuUartTxDis(3);
        uart3.txcnt = 0;
        uart3.txlen = 0;
    }
    else
    {
        McuUartWrite(3, txbuff3[uart3.txcnt++]);
    }
}

/***************************************主函数******************************************/
volatile bool dsleep_fastwakeup_status = false;
__RAM_FUNC int main(void)
{
    SystemInit();

    McuUartSet(0, 9600, 8, 2, 0, 3, 4);
    McuUartRxIrqReg(0, Uart0RxIrqHandle);
    McuUartRxEn(0);
    McuUartTxEn(0);

    if(Get_Boot_Reason() != WAKEUP_DSLEEP) 
    {
        McuUartWriteFram(0, (uint8_t *)"\r\nPOWERON or RESET\r\n", strlen("\r\nPOWERON or RESET\r\n"));
    }

    McuUartSet(1, 115200, 8, 2, 0, 18, 19);
    McuUartRxIrqReg(1, Uart1RxIrqHandle);
    McuUartRxEn(1);
    McuUartTxEn(1);

    McuUartSet(2, 115200, 8, 1, 0, 14, 20);
    McuUartRxIrqReg(2, Uart2RxIrqHandle);
    McuUartTxIrqReg(2, Uart2TxIrqHandle);
    McuUartRxEn(2);

    McuUartSet(3, 115200, 8, 1, 0, 22, 23);
    McuUartRxIrqReg(3, Uart3RxIrqHandle);
    McuUartTxIrqReg(3, Uart3TxIrqHandle);
    McuUartRxEn(3);

    while (1)
    {
        //接收完成回显数据
        if (uart0.rxdone)
        {
            //准备待发送数据
            uart0.txcnt = 0;
            uart0.txlen = uart0.rxcnt;
            memcpy(txbuff0, rxbuff0, uart0.txlen);

            //为下次接收数据做准备
            uart0.rxcnt = 0;
            uart0.rxdone = false;

            //打开uart tx，发送数据
            McuUartWriteFram(0, txbuff0, uart0.txlen);
            uart0.txdone = true;
        }

        //回显完成进入深睡
        if(uart0.txdone)
        {
            uart0.txdone = false;
            //dsleep_fastwakeup_status = Enter_LowPower_Mode(LPM_DSLEEP);

            //XY_SOC_VER为2、4、5时深睡唤醒才会执行快速恢复，使用mcu_uart接口且num入参为0时，请务必保证AT_LPUART宏为0。
            // if(dsleep_fastwakeup_status && (Get_Boot_Reason() == WAKEUP_DSLEEP))
            // {
            // McuUartWriteFram(0, "\r\nFastRecovery Dsleep Wkup\r\n", strlen("\r\nFastRecovery Dsleep Wkup\r\n"));
            // }
        }

        if (uart1.rxdone)
        {
            //准备待发送数据
            uart1.txcnt = 0;
            uart1.txlen = uart1.rxcnt;
            memcpy(txbuff1, rxbuff1, uart1.txlen);

            //为下次接收数据做准备
            uart1.rxcnt = 0;
            uart1.rxdone = false;

            //打开uart tx，发送数据
            McuUartWriteFram(1, txbuff1, uart1.txlen);
        }

        if (uart2.rxdone)
        {
            //准备待发送数据
            uart2.txcnt = 0;
            uart2.txlen = uart2.rxcnt;
            memcpy(txbuff2, rxbuff2, uart2.txlen);

            //为下次接收数据做准备
            uart2.rxcnt = 0;
            uart2.rxdone = false;

            //打开uart tx，发送第一个数据，发送数组下标加1
            McuUartTxEn(2);
            McuUartWrite(2, txbuff2[uart2.txcnt++]);
        }

        if (uart3.rxdone)
        {
            //准备待发送数据
            uart3.txcnt = 0;
            uart3.txlen = uart3.rxcnt;
            memcpy(txbuff3, rxbuff3, uart3.txlen);

            //为下次接收数据做准备
            uart3.rxcnt = 0;
            uart3.rxdone = false;

            //打开uart tx，发送第一个数据，发送数组下标加1
            McuUartTxEn(3);
            McuUartWrite(3, txbuff3[uart3.txcnt++]);
        }
    }
}

#else

#define UART_NUM_USED (4)   //使用的uart个数
#define UART_LEN_MAX (256)  //uart最大收发长度，单位字节

uint8_t uart_rx_buff[UART_NUM_USED][UART_LEN_MAX] = {0};
uint8_t uart_tx_buff[UART_NUM_USED][UART_LEN_MAX] = {0};

uint16_t uart_rx_cnt[UART_NUM_USED] = {0};
uint16_t uart_tx_cnt[UART_NUM_USED] = {0};

volatile uint8_t uart_rx_done[UART_NUM_USED] = {0};
volatile uint8_t uart_tx_done[UART_NUM_USED] = {0};

uint16_t uart_tx_len[UART_NUM_USED] = {0};

__RAM_FUNC void Uart0RxIrqHandle(void)
{
    Debug_Runtime_Add("START2");
    //读取数据
    McuUartRead(0, &uart_rx_buff[0][uart_rx_cnt[0]]);
    Debug_Runtime_Add("McuUartRead(0, &uart_rx_buff[0][uart_rx_cnt[0]]);");

    //结束字符判断
    if(uart_rx_buff[0][uart_rx_cnt[0]] == '\n')
    {
        uart_rx_done[0] = 1;
    }
    uart_rx_cnt[0]++;
}

__RAM_FUNC void Uart0TxIrqHandle(void)
{
    //结束判断
    if(uart_tx_cnt[0] >= uart_tx_len[0])
    {
        uart_tx_done[0] = 1;
    }
    else
    {
        McuUartWrite(0, uart_tx_buff[0][uart_tx_cnt[0]]);//发送数据
    }
    uart_tx_cnt[0]++;
}

__RAM_FUNC void Uart1RxIrqHandle(void)
{
    Debug_Runtime_Add("START2");
    //读取数据
    McuUartRead(1, &uart_rx_buff[1][uart_rx_cnt[1]]);
    Debug_Runtime_Add("McuUartRead(1, &uart_rx_buff[1][uart_rx_cnt[1]]);");

    //结束字符判断
    if(uart_rx_buff[1][uart_rx_cnt[1]] == '\n')
    {
        uart_rx_done[1] = 1;
    }
    uart_rx_cnt[1]++;
}

__RAM_FUNC void Uart1TxIrqHandle(void)
{
    //结束判断
    if(uart_tx_cnt[1] >= uart_tx_len[1])
    {
        uart_tx_done[1] = 1;
    }
    else
    {
        McuUartWrite(1, uart_tx_buff[1][uart_tx_cnt[1]]);//发送数据
    }
    uart_tx_cnt[1]++;
}

__RAM_FUNC void Uart2RxIrqHandle(void)
{
    Debug_Runtime_Add("START2");
    //读取数据
    McuUartRead(2, &uart_rx_buff[2][uart_rx_cnt[2]]);
    Debug_Runtime_Add("McuUartRead(2, &uart_rx_buff[2][uart_rx_cnt[2]]);");

    //结束字符判断
    if(uart_rx_buff[2][uart_rx_cnt[2]] == '\n')
    {
        uart_rx_done[2] = 1;
    }
    uart_rx_cnt[2]++;
}

__RAM_FUNC void Uart2TxIrqHandle(void)
{
    //结束判断
    if(uart_tx_cnt[2] >= uart_tx_len[2])
    {
        uart_tx_done[2] = 1;
    }
    else
    {
        McuUartWrite(2, uart_tx_buff[2][uart_tx_cnt[2]]);//发送数据
    }
    uart_tx_cnt[2]++;
}

__RAM_FUNC void Uart3RxIrqHandle(void)
{
    Debug_Runtime_Add("START2");
    //读取数据
    McuUartRead(3, &uart_rx_buff[3][uart_rx_cnt[3]]);
    Debug_Runtime_Add("McuUartRead(3, &uart_rx_buff[3][uart_rx_cnt[3]]);");

    //结束字符判断
    if(uart_rx_buff[3][uart_rx_cnt[3]] == '\n')
    {
        uart_rx_done[3] = 1;
    }
    uart_rx_cnt[3]++;
}

__RAM_FUNC void Uart3TxIrqHandle(void)
{
    //结束判断
    if(uart_tx_cnt[3] >= uart_tx_len[3])
    {
        uart_tx_done[3] = 1;
    }
    else
    {
        McuUartWrite(3, uart_tx_buff[3][uart_tx_cnt[3]]);//发送数据
    }
    uart_tx_cnt[3]++;
}

char temp_str[100] = {0};
__RAM_FUNC int main(void)
{
    SystemInit();
    EnablePrimask();

    Debug_Runtime_Init();

    Debug_Runtime_Add("START1");
    
    McuUartSet(0, 115200, 8, 2, 0); //在OpenCPU模式下，uart0的参考时钟频率与Boot_CP无关，Boot_CP后无需重新配置，最大速率支持230400
    Debug_Runtime_Add("McuUartSet(0, 115200, 8, 2, 0);");

    McuUartRxEn(0);
    Debug_Runtime_Add("McuUartRxEn(0);");

    McuUartRxIrqReg(0, Uart0RxIrqHandle);
    Debug_Runtime_Add("McuUartRxIrqReg(0, Uart0RxIrqHandle);");

    McuUartTxIrqReg(0, Uart0TxIrqHandle);   
    Debug_Runtime_Add("McuUartTxIrqReg(0, Uart0TxIrqHandle);  ");

    McuUartSet(1, 115200, 8, 2, 0); //在OpenCPU模式下，uart1参考时钟频率与Boot_CP无关，Boot_CP后无需重新配置，最大速率支持460800
    Debug_Runtime_Add("McuUartSet(1, 115200, 8, 2, 0);");
    
    McuUartRxEn(1);
    Debug_Runtime_Add("McuUartRxEn(1);");

    McuUartRxIrqReg(1, Uart1RxIrqHandle);
    Debug_Runtime_Add("McuUartRxIrqReg(1, Uart1RxIrqHandle);");

    McuUartTxIrqReg(1, Uart1TxIrqHandle);  
    Debug_Runtime_Add("McuUartTxIrqReg(1, Uart1TxIrqHandle);"); 

    McuUartSet(2, 115200, 8, 1, 0); //在OpenCPU模式下，uart2参考时钟频率与Boot_CP有关，Boot_CP前最大速率支持380400，Boot_CP后需要重新配置且最大速率支持921600
    Debug_Runtime_Add("McuUartSet(2, 115200, 8, 1, 0);");

    McuUartRxEn(2);
    Debug_Runtime_Add("McuUartRxEn(2);");

    McuUartRxIrqReg(2, Uart2RxIrqHandle);
    Debug_Runtime_Add("McuUartRxIrqReg(2, Uart2RxIrqHandle);");

    McuUartTxIrqReg(2, Uart2TxIrqHandle);  
    Debug_Runtime_Add("McuUartTxIrqReg(2, Uart2TxIrqHandle);");  

    McuUartSet(3, 115200, 8, 1, 0); //在OpenCPU模式下，uart3参考时钟频率与Boot_CP有关，Boot_CP前最大速率支持380400，Boot_CP后需要重新配置且最大速率支持921600
    Debug_Runtime_Add("McuUartSet(3, 115200, 8, 1, 0);");
    
    McuUartRxEn(3);
    Debug_Runtime_Add("McuUartRxEn(3);");

    McuUartRxIrqReg(3, Uart3RxIrqHandle);
    Debug_Runtime_Add("McuUartRxIrqReg(3, Uart3RxIrqHandle);");

    McuUartTxIrqReg(3, Uart3TxIrqHandle);
    Debug_Runtime_Add("McuUartTxIrqReg(3, Uart3TxIrqHandle);");

    while (1)
    {
        for (uint8_t num = 0; num < UART_NUM_USED; num++)
        {
            if (uart_rx_done[num] == 1)
            {
                sprintf((char *)uart_tx_buff[num],"\r\nuart%d received %dbytes, they are %s\r\n",num,uart_rx_cnt[num],uart_rx_buff[num]);
                uart_tx_len[num] = strlen((const char *)uart_tx_buff[num]);

                sprintf(temp_str, "McuUartTxEn(%d);", num);
                Debug_Runtime_Add("START3");
                //打开uart tx，发送第一个数据，发送数组下标加1
                McuUartTxEn(num);
                Debug_Runtime_Add(temp_str);

                sprintf(temp_str, "McuUartWrite(%d, uart_tx_buff[%d][uart_tx_cnt[%d]]);", num, num, num);
                Debug_Runtime_Add("START4");        
                McuUartWrite(num, uart_tx_buff[num][uart_tx_cnt[num]]);
                Debug_Runtime_Add(temp_str);

                uart_tx_cnt[num]++;

                //为下次数据接收做准备
                uart_rx_cnt[num] = 0;
                uart_rx_done[num] = 0;
                memset(uart_rx_buff[num], 0, UART_LEN_MAX);
            }

            if (uart_tx_done[num] == 1)
            {
                sprintf(temp_str, "McuUartTxDis(%d);", num);
                Debug_Runtime_Add("START5");  
                //关闭uart tx
                McuUartTxDis(num);
                Debug_Runtime_Add(temp_str);

                //为下次数据发送做准备
                uart_tx_cnt[num] = 0;
                uart_tx_done[num] = 0;
            }
        }
    }
}

#endif