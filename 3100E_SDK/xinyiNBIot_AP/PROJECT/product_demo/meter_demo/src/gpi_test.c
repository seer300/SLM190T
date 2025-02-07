/**
 * @brief  该工程默认为上电就加载CP核的传统数据模式，用户可以参考其他DEMO，实现OPENCPU产品相关代码
 * @warning OPENCPU开发时，注意Boot_CP调用点、低功耗相关外设的使用细节、User_Init_FastRecovery的实现、看门狗的使用、容错策略等，细节参考相关DEMO工程
 * @note  用户编译default工程时，请根据自身产品形态，设置好define.cmake中XY_SOC_VER和VER_BC95等重要宏值
 */
#include "hal_gpio.h"
#include "xy_printf.h"
#include "xy_cp.h"
#include "xy_lpm.h"
#include "at_uart.h"
#include "auto_baudrate.h"
#include "urc_process.h"
#include "xy_system.h"
#include "xy_svd.h"
#include "ap_watchdog.h"
#include "basic_config.h"
#include "sys_rc32k.h"
#include "mcu_adapt.h"

#if 0

//1：使用wkp1、2测试干簧管
//2：使用gpi5、6测试干簧管
#define TEST (2)

#if TEST==1
    #define input_pin1  (MCU_WKP1)
    #define input_pin2  (MCU_WKP2)
#elif TEST==2
    #define input_pin1  (MCU_GPIO5)
    #define input_pin2  (MCU_GPIO6)
#endif

#define test_pin1  (MCU_GPIO0)
#define test_pin2  (MCU_GPIO1)

extern void uart_print_init();
extern void uart_printf(const char *fmt, ...);

__RAM_FUNC void User_Init_FastRecovery(void)
{
    uart_print_init();
}

#define Second             (1000)
#define Minute             (60*Second)
#define BootCP_by_5Min     (1 * Minute)  //5分钟
#define Printf_by_10Sec    (10 * Second) //10秒钟

#define VALID_INTERVAL       (100)

volatile uint8_t deepsleep = 0;
volatile uint8_t print = 0;
volatile uint8_t bootcp = 0;

typedef struct 
{
    uint32_t CntFallingEdge;
    uint32_t CntRisingEdge;
    uint32_t CntReedSwitch;
    uint32_t TickRisingEdge;
    uint32_t TickFallingEdge
} ReedSwitch_Typedef;

volatile ReedSwitch_Typedef ReedSwitch1 = {0};
volatile ReedSwitch_Typedef ReedSwitch2 = {0};

__RAM_FUNC void input_pin1_callback(void)
{
    //后来上升沿
    if(McuGpioRead(input_pin1))
    {
        McuGpioWrite(test_pin1, 1);
        McuGpioWrite(test_pin1, 0);
        // 上升沿次数+1
        ReedSwitch1.CntRisingEdge++;
        // 记录干簧管1的上升沿对应的系统tick
        ReedSwitch1.TickRisingEdge = Get_Tick();
        // 如果本次上升沿与上一个下降沿的tick差值大于100ms，则算为一次有效的计次
        if (ReedSwitch1.TickRisingEdge - ReedSwitch1.TickFallingEdge >= VALID_INTERVAL)
        {
            ReedSwitch1.CntReedSwitch++;
        }
    }
    //先来下降沿
    else
    {
        McuGpioWrite(test_pin2, 1);
        McuGpioWrite(test_pin2, 0);
        //下降沿次数+1
        ReedSwitch1.CntFallingEdge++;
        //记录本次下降沿的系统tick
        ReedSwitch1.TickFallingEdge = Get_Tick();
    }

    deepsleep = 1; // 深睡标志位使能
}

__RAM_FUNC void input_pin2_callback(void)
{
    //后来上升沿
    if(McuGpioRead(input_pin2))
    {
        ReedSwitch2.CntRisingEdge++;
        ReedSwitch2.TickRisingEdge = Get_Tick();
        if (ReedSwitch2.TickRisingEdge - ReedSwitch2.TickFallingEdge >= VALID_INTERVAL)
        {
            ReedSwitch2.CntReedSwitch++;
        }
    }
    //先来下降沿
    else
    {
        ReedSwitch2.CntFallingEdge++;
        ReedSwitch2.TickFallingEdge = Get_Tick();
    }

    deepsleep = 1; // 深睡标志位使能
}

__RAM_FUNC static void xytimer_bootcp_callback(void)
{
    bootcp = 1;
}

__RAM_FUNC static void xytimer_print_callback(void)
{
    print = 1;
}

char txt[150] = {0};
extern char g_at_RecvBuffer[];
extern uint16_t g_at_recved_len;
__RAM_FUNC int main(void)
{     
    SystemInit();

    FastRecov_Init_Hook_Regist(User_Init_FastRecovery);

    uart_print_init();

    at_uart_init();

    EnablePrimask();

    McuGpioModeSet(test_pin1, 0x00);
    McuGpioModeSet(test_pin2, 0x00);

    //初始化指定引脚为双边沿触发
    McuGpioModeSet(input_pin1, 0x11);
    McuGpioIrqReg(input_pin1, input_pin1_callback, MCU_GPIO_INT_BOTH);
    McuGpioIrqEn(input_pin1);

    McuGpioModeSet(input_pin2, 0x11);
    McuGpioIrqReg(input_pin2, input_pin2_callback, MCU_GPIO_INT_BOTH);
    McuGpioIrqEn(input_pin2);

    //周期性每10分钟起CP
    Timer_AddEvent(TIMER_LP_USER1, BootCP_by_5Min, xytimer_bootcp_callback, 1);

    //周期性每10秒钟打印一次中断计数
    Timer_AddEvent(TIMER_LP_USER2, Printf_by_10Sec, xytimer_print_callback, 1);

    LPM_LOCK(STANDBY_DEFAULT);   // 禁止进入STANDBY
    LPM_LOCK(USER_DSLEEP_LOCK1); // 深睡锁加锁，在wkup1/2的回调函数中解锁

    while (1)
    {
        if(bootcp == 1)
        {
            bootcp = 0;
            if(!Boot_CP(WAIT_CP_BOOT_MS))
            {
                while(1);
            }
            HAL_Delay(10*1000);

            uint8_t cnt = 0;
            for (uint8_t i = 0; i < 2; i++)
            {
                cnt = 300;
                do{
                    at_uart_recv_and_process();
                    HAL_Delay(10);
                } while (cnt--);
                switch(i)
                {
                    case 0 :
                    { 
                        snprintf(g_at_RecvBuffer, 50, "AT+CGATT?\r\n"); 
                        g_at_recved_len = strlen("AT+CGATT?\r\n");
                        Send_AT_to_Ext("AT+CGATT?\r\n");
                        break;
                    }
                    case 1 :
                    { 
                        snprintf(g_at_RecvBuffer, 50, "AT+NPING=221.229.214.202,32,20,30,1\r\n"); 
                        g_at_recved_len = strlen("AT+NPING=221.229.214.202,32,20,30,1\r\n");
                        Send_AT_to_Ext("AT+NPING=221.229.214.202,32,20,30,1\r\n");
                        break; 
                    }
                    default:;
                }
            }
        }

        at_uart_recv_and_process();

        //打印两路干簧管的上升沿次数、下降沿次数、有效计次
        if (print == 1)
        {
            print = 0;
            snprintf(txt, 150, "\r\nWKP1:FEdge=%ld,REdge=%ld,RS=%ld\r\nWKP2:FEdge=%ld,REdge=%ld,RS=%ld\r\n\r\n",
                    ReedSwitch1.CntFallingEdge, ReedSwitch1.CntRisingEdge, ReedSwitch1.CntReedSwitch,
                    ReedSwitch2.CntFallingEdge, ReedSwitch2.CntRisingEdge, ReedSwitch2.CntReedSwitch);
            uart_printf("%s", txt);
        }
        
        CP_URC_Process();

        RC32k_Cali_Process();

        //允许进入深睡
        if (deepsleep == 1)
        {
            deepsleep = 0;
            LPM_UNLOCK(USER_DSLEEP_LOCK1);
        }

        Enter_LowPower_Mode(LPM_DSLEEP);
    }
}

#endif