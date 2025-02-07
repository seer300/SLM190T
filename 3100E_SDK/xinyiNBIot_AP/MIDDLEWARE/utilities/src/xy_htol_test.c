#if BLE_EN

#include "xy_printf.h"
#include "at_process.h"
#include "xy_cp.h"
#include "xy_system.h"
#include "at_uart.h"
#include "xy_event.h"
#include "cloud_process.h"
#include "xy_utils.h"
#include "sys_mem.h"
#include "ble_hci.h"

uint32_t g_htol_flag = 0;

void htol_test_init(void)
{
    if((READ_FAC_NV(uint8_t, test) != 2)
       || (Get_Boot_Reason() != POWER_ON && Get_Boot_Reason() != WAKEUP_DSLEEP))
        return;
    
    g_htol_flag = 1;
    *(uint32_t*)(BAK_MEM_RF_MODE) = 1;
}

void htol_test(void)
{
    if(g_htol_flag != 1)
        return;
    
    int i = 0, ret = 0, j = 0;
    char *at_rf_str = xy_malloc(100);
    char *at_response = xy_malloc(100);
    uint64_t prevtime,currtime;
    int param= 0, ble_param = 0;

    if(Get_Boot_Reason() == WAKEUP_DSLEEP)
        goto timer;

    //开启蓝牙
    if (ble_open() != BLE_OK)
    {
        xy_printf("[HTOL_TEST]: BLE  OPEN ERR");
    }

    #if 0
    //set max rx gain
    if((ret = AT_Send_And_Get_Rsp("AT+RF=MGCON,1\r\n",  10, "+", "%a", at_response)) != 0)
        goto error;
    if((ret = AT_Send_And_Get_Rsp("AT+RF=MGCIDX,0\r\n",  10, "+", "%a", at_response)) != 0)
        goto error;
    #endif
    
    for(j=0; j<30; j++)
    {
        //RF射频测试
        for(i = 0; i < 15; i++)
        {
            prevtime = GetAbsoluteTick();
            if((ret = AT_Send_And_Get_Rsp("AT+RF=Tx,915000000,960000000,1575,0,23,0,255,7\r\n",  10, "+", "%a", at_response)) != 0)
                goto error;
            currtime = GetAbsoluteTick(); 
            if((currtime - prevtime) < 256) HAL_Delay(256-(currtime - prevtime));  
            currtime = GetAbsoluteTick(); 
            xy_printf("[HTOL_TEST][%d]TX:%s %d", i, at_response, (uint32_t)(currtime - prevtime));

            prevtime = GetAbsoluteTick();
            if((ret =AT_Send_And_Get_Rsp("AT+RF=RXTEST,960000000,2525,0,61,8,0,1,4,20\r\n",  10, "+", "%a", at_response)) != 0)
                goto error;
            currtime = GetAbsoluteTick(); 
            xy_printf("[HTOL_TEST]RXTEST:%s %d", at_response, (currtime - prevtime));
        }
        
        prevtime = GetAbsoluteTick();
        if((ret =AT_Send_And_Get_Rsp("AT+RF=RXTEST,960000000,2525,0,61,8,0,1,4,480\r\n", 10, "+", "%a", at_response)) != 0)
            goto error;
        currtime = GetAbsoluteTick(); 

        if((ret =AT_Send_And_Get_Rsp("AT+RF=STOP\r\n", "%s", 60, at_response)) != 0)
        {
            goto error;
        }
        xy_printf("[HTOL_TEST]RXTEST:%s %d", at_response, (currtime - prevtime));

        prevtime = GetAbsoluteTick(); 
        param = 80;//param = param/0.625;
        ble_param = ble_config_op(HCI_CMD_LE_SET_ADV_PARM, (char *)&param, 2);
        if(ble_param != BLE_OK)
        {
            xy_printf("[HTOL_TEST]BLETEST ERROR");
            goto error;
        }
        currtime = GetAbsoluteTick(); 
        xy_printf("[HTOL_TEST]BLETEST:%d", (currtime - prevtime));
        if((currtime - prevtime) < 6700) HAL_Delay(6700-((currtime - prevtime)*2));
        prevtime = GetAbsoluteTick(); 

        param = 8000;//param = param/0.625;
        ble_param = ble_config_op(HCI_CMD_LE_SET_ADV_PARM, (char *)&param, 2);
        if(ble_param != BLE_OK)
        {
            xy_printf("[HTOL_TEST]BLETEST ERROR");
            goto error;
        }
        currtime = GetAbsoluteTick(); 
        xy_printf("[HTOL_TEST]BLETEST:%d", (currtime - prevtime));
    }

    prevtime = GetAbsoluteTick();
    for(i = 0; i < 17000; i++) 
    {
        xy_Flash_Read(ARM_FLASH_BASE_ADDR + (100) * i, at_response, 100);
    }
    currtime = GetAbsoluteTick();
    xy_printf("[HTOL_TEST]READ_FLASH:%d", (currtime - prevtime));

timer:
    Stop_CP(0);
    if(*((uint32_t *)USER_FLASH_BASE) == 0xFFFFFFFF)
    {
        uint32_t val_1 = 0;
        xy_Flash_Write(USER_FLASH_BASE, &val_1, 4);
    }

    if(*(uint32_t*)(USER_FLASH_BASE) < 12)
    {
        //设置1S的唤醒定时器
        Timer_AddEvent(TIMER_LP_USER1, 1000, NULL, 1);

        uint32_t val_2 = *(uint32_t *)USER_FLASH_BASE;
        val_2=val_2+1;
        xy_Flash_Write(USER_FLASH_BASE, &val_2, 4);

    }
    else
    {
        Timer_DeleteEvent(TIMER_LP_USER1);

        uint32_t val_3 = 0xFFFFFFFF;
        xy_Flash_Write(USER_FLASH_BASE, &val_3, 4);
    }
    
    //设置校准模式
    *(uint32_t*)(BAK_MEM_RF_MODE) = 0;
    g_htol_flag = 0;

    ble_close();

    xy_free(at_rf_str);
    xy_free(at_response);
    return;

error:
    //设置校准模式
    *(uint32_t*)(BAK_MEM_RF_MODE) = 0;
    xy_free(at_rf_str);
    xy_free(at_response);
    Stop_CP(0);
    g_htol_flag = 0;
    xy_printf("[HTOL_TEST]TEST SUCCESS");
}


__RAM_FUNC void Htol_Sido_Sleep_On(void)
{
    AONPRCM->AONPWR_CTRL |= 0x800; //sido on
}

#endif