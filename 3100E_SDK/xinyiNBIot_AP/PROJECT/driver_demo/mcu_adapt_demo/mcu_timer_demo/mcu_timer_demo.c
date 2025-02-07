#include "mcu_adapt.h"
#include "xy_system.h"
#include "system.h"

#if 1 // 1:验证TIMER功能, 0:验证TIMER接口耗时

__RAM_FUNC void lptimer1_callback(void)
{
    McuGpioToggle(MCU_GPIO20);
}

__RAM_FUNC void lptimer2_callback(void)
{
    McuGpioToggle(MCU_GPIO21);
}

__RAM_FUNC void timer1_callback(void)
{
    McuGpioToggle(MCU_GPIO22);
}

__RAM_FUNC int main(void)
{
    SystemInit();

    //推挽输出，默认低电平
    McuGpioModeSet(MCU_GPIO20, 0x00), McuGpioWrite(MCU_GPIO20, 0);
    McuGpioModeSet(MCU_GPIO21, 0x00), McuGpioWrite(MCU_GPIO21, 0);
    McuGpioModeSet(MCU_GPIO22, 0x00), McuGpioWrite(MCU_GPIO22, 0);

    //lptimer1周期定时1ms
    McuLptimerSetPeriod(5);
    McuLptimerIrqReg(lptimer1_callback);
    McuLptimerEn();

    //lptimer2周期定时10ms
    McuLptimer2SetPeriod(10);
    McuLptimer2IrqReg(lptimer2_callback);
    McuLptimer2En();

    //timer1周期定时1ms
    McuTimerSetPeriod(1, 1);
    McuTimerIrqReg(1, timer1_callback);
    McuTimerEn(1);

    //timer2周期输出频率为50k、占空比为50%的PWM
    McuTimerSetPWM2(2, 50, 50, MCU_GPIO53);
    McuTimerEn(2);

    while (1)
    {
		;
    }
}

#else

volatile bool lptimer_toggle_flag = 0;
volatile bool timer1_toggle_flag = 0;
volatile bool timer2_toggle_flag = 0;

__RAM_FUNC void lptimer_callback(void)
{
    if( lptimer_toggle_flag )
    {
        McuGpioWrite(MCU_WKP1, 1);
        lptimer_toggle_flag = false;
    }
    else
    {
        McuGpioWrite(MCU_WKP1, 0);
        lptimer_toggle_flag = true;
    }
}

__RAM_FUNC void timer1_callback(void)
{
    if( timer1_toggle_flag )
    {
        McuGpioWrite(MCU_WKP2, 1);
        timer1_toggle_flag = false;
    }
    else
    {
        McuGpioWrite(MCU_WKP2, 0);
        timer1_toggle_flag = true;
    }
}

__RAM_FUNC void timer2_callback(void)
{
    if( timer2_toggle_flag )
    {
        McuGpioWrite(MCU_WKP3, 1);
        timer2_toggle_flag = false;
    }
    else
    {
        McuGpioWrite(MCU_WKP3, 0);
        timer2_toggle_flag = true;
    }
}

__RAM_FUNC int main(void)
{
    SystemInit();
    EnablePrimask();

    McuGpioModeSet(MCU_WKP1, 0x00);
    McuGpioModeSet(MCU_WKP2, 0x00); 
    McuGpioModeSet(MCU_WKP3, 0x00);


    Debug_Runtime_Init();
    Debug_Runtime_Add("START");

    //lptimer周期定时8ms
    McuLptimerSetPeriod(1000);
    Debug_Runtime_Add("McuLptimerSetPeriod(8);");

    McuLptimerIrqReg(lptimer_callback);
    Debug_Runtime_Add("McuLptimerIrqReg(lptimer_callback);");

    McuLptimerEn();
    Debug_Runtime_Add("McuLptimerEn();");

    McuLptimerDis();
    Debug_Runtime_Add("McuLptimerDis();");

    //timer1周期定时2ms
    McuTimerSetPeriod(1, 2);
    Debug_Runtime_Add("McuTimerSetPeriod(1, 2);");
    McuTimerIrqReg(1, timer1_callback);
    Debug_Runtime_Add("McuTimerIrqReg(1, timer1_callback);");

    McuTimerEn(1);
    Debug_Runtime_Add("McuTimerEn(1);");

    McuTimerDis(1);
    Debug_Runtime_Add("McuTimerDis(1);");

    //timer2周期定时4ms
    McuTimerSetPeriod(2, 4);
    Debug_Runtime_Add("McuTimerSetPeriod(2, 4);");
    McuTimerIrqReg(2, timer2_callback);
    Debug_Runtime_Add("McuTimerIrqReg(2, timer2_callback);");

    McuTimerEn(2);
    Debug_Runtime_Add("McuTimerEn(2);");

    McuTimerDis(2);
    Debug_Runtime_Add("McuTimerDis(2);");

    while (1)
    {
        ;
    }
 }

#endif
