/**
 * @file
 * @brief   该源文件为BLE芯片操作的API接口，以供应用用户通过API接口进行BLE硬件的操控。该接口可供用户在AP核上进行BLE相关OPENCPU的二次开发
 * @warning
 */
#include "xy_system.h"
#include "xy_printf.h"
#include "urc_process.h"
#include "xy_cp.h"
#include "xy_lpm.h"
#include "xy_at_api.h"
#include "xy_memmap.h"
#include "ble_main.h"
#include <stdint.h>
#include "ble_drv.h"
#include "ble_api.h"
#include "at_process.h"
#include "hal_uart.h"
#include "hal_def.h"
#include "ble_hci.h"
#include "ble_drv.h"
#include "ble_msg.h"
#include "ble_api.h"
#include "xy_ftl.h"

/*类似socket的errno*/
uint32_t ble_errno = 0;


/*通过LPUART的AT通道，发送AT命令给外部MCU，含URC、DEBUG等特殊AT命令*/
//DEBUG宏开启才打印
void send_str_to_mcu(char *buf)
{
    (void)buf;
#if XY_DEBUG
	Send_AT_to_Ext(buf);
#endif
}


BLE_ERR_E ble_open()
{
	if (g_working_info->poweron == 1)
		return BLE_OK;

	LPM_LOCK(DSLEEP_BLE_LOCK); //开蓝牙需关闭DEEPSLEEP

	ble_start_init();
	g_working_info->poweron = 1;
    return BLE_OK;
}

BLE_ERR_E ble_close()
{
	BLE_ERR_E ret = BLE_OK;

	if(g_working_info==NULL || g_working_info->poweron==0)
		return ret;

	Timer_DeleteEvent(TIMER_NON_LP_BLE);
	
	ble_power_clock_deinit();
	ble_pin_deinit();

	LPM_UNLOCK(DSLEEP_BLE_LOCK);  //与ble_open中DEEPSLEEP锁对应

	g_working_info->poweron = 0;
	g_ble_fac_nv->broadcast = 0;
	g_working_info->connected = 0;
    return ret;
}

/*设置配对模式*/
BLE_ERR_E ble_set_pairing_mode(uint32_t pairing_mode)
{
	BLE_ERR_E ret;
	uint8_t pair_byte = 0;

	if(g_working_info==NULL || g_working_info->poweron==0)
		return BLE_NO_ALLOWED;

	pair_byte = ble_pairmode_select(pairing_mode);

	ret = ble_config_op(HCI_CMD_LE_SET_PAIRING, &pair_byte, 1);
	if(ret == BLE_OK)
		g_ble_fac_nv->pairing_mode = pairing_mode;
	
	return ret;
}

BLE_ERR_E ble_set_passkey(uint32_t passkey)
{
    uint8_t passkey_payload[5]= {0};
	BLE_ERR_E ret;

	if(g_working_info==NULL || g_working_info->poweron==0)
		return BLE_NO_ALLOWED;
	
    if(passkey > 999999)
    {
        return BLE_PARAM_INVALID;
    }

    passkey_payload[0]=0x01;
    memcpy(passkey_payload + 1, &passkey, 4);
    
    ret = ble_config_op(HCI_CMD_LE_SET_FIXED_PASSKEY, passkey_payload, 5);
	if(ret == BLE_OK)
		g_ble_fac_nv->passkey = passkey;

    return ret;
}

BLE_ERR_E ble_open_broadcast()
{
    int enable = 4;
	BLE_ERR_E ret;

	if(g_working_info==NULL || g_working_info->poweron==0)
		return BLE_NO_ALLOWED;
	
    ret = ble_config_op(HCI_CMD_SET_VISIBILITY, &enable, 1);
	if(ret == BLE_OK)
		g_ble_fac_nv->broadcast = 1;
    return ret;
}

BLE_ERR_E ble_close_broadcast()
{
    int enable = 0;
	BLE_ERR_E ret;

	if(g_working_info==NULL || g_working_info->poweron==0)
		return BLE_NO_ALLOWED;
	
    ret = ble_config_op(HCI_CMD_SET_VISIBILITY, &enable, 1);
	if(ret == BLE_OK)
		g_ble_fac_nv->broadcast = 0;
	
    return ret;
}

/*透传数据发送接口，目前主要用于发送MCU的数据*/
BLE_ERR_E ble_send_data(uint8_t *data, uint32_t len)
{
	BLE_ERR_E ret;

	if(g_working_info==NULL || g_working_info->connected==0)
		return BLE_NO_ALLOWED;

	ret = hci_send_data(g_working_info->default_handle, data, len);

	return ret;
}

BLE_ERR_E ble_set_name(char *name, uint8_t size)
{
    uint8_t param[30]={0};
	BLE_ERR_E ret;

	if(g_working_info==NULL || g_working_info->poweron==0)
		return BLE_NO_ALLOWED;
	
	if(size > 29)
	{
		return BLE_NO_SUPPORT;
	}
    memcpy(param,name,size);
	
    ret = ble_config_op(HCI_CMD_SET_BLE_NAME, param, strlen((char *)param));
	if(ret == BLE_OK)
		memcpy(g_ble_fac_nv->ble_name,param,strlen((char *)param)+1);
    return ret;
}


BLE_ERR_E ble_get_dev_addr(uint8_t *addr)
{
	memcpy(addr,g_ble_fac_nv->ble_mac,6);
	return BLE_OK;
}

/*设置RFNV里面BLE的MAC地址，长度6字节*/
int set_rfnv_blemac(char *blemac)
{
	int ret = 1;
    char ble_mac_rf[32] = {0};
	char *at_str = xy_malloc(64);

    bytes2hexstr((unsigned char*)blemac,6,ble_mac_rf,13);

    snprintf(at_str, 50, "AT+RF=SETBLEMAC,%s\r\n", ble_mac_rf);

    ret = AT_Send_And_Get_Rsp(at_str, 10, NULL, NULL);
	if(ret != XY_OK)
    {
        ret = 0;
    }

	xy_free(at_str);
    return ret;
}


/*从RFNV里面获取BLE的MAC地址，长度6字节*/
int get_rfnv_blemac(char *blemac)
{
    int ret = 1;
	uint8_t rfmac[20] = {0};

    ret = AT_Send_And_Get_Rsp("AT+RF=GETBLEMAC\r\n", 10, "+RF:BLEMAC,","%s",rfmac);
    if(ret != XY_OK)
    {
        return ret;
    }

	if(hexstr2bytes((char *)rfmac,12,blemac,6) == -1)
    {
        ret = -1;
    }
    return ret;
}


/*passkey默认成imei号的后六位*/
int get_blekey_from_imei(int* blekey)
{
    int ret = 1;
	uint8_t rfimei[32] = {0};
    uint8_t tail[8] = {0};

    ret = AT_Send_And_Get_Rsp("AT+CGSN=1\r\n", 10, "+CGSN: ","%s",rfimei);
    if(ret != XY_OK)
    {
        return ret;
    }
    memcpy(tail,rfimei+9,6);

    *blekey = atoi(tail);

    return ret;
}

/**
  * @brief 获取存放在RFNV中的BLE频偏，等同于AT+RF=GETBLELO
  */
uint8_t get_ble_freqoffset()
{
	uint16_t bleFreqOffset = HWREGH(NV_FLASH_RF_BASE + 0xE90 + 4);//FTL偏移4字节
    if(bleFreqOffset == 0xFFFF)
    {
        return g_ble_fac_nv->freq_offset;
    }
    else
    {
        if(g_ble_fac_nv->freq_offset != (uint8_t)bleFreqOffset)
        {
            g_ble_fac_nv->freq_offset = (uint8_t)bleFreqOffset;
            SAVE_BLE_PARAM(freq_offset);
        }
        return g_ble_fac_nv->freq_offset;
    }
}

/**
  * @brief   设置BLEGPIO
  * @param   Mode:输入/输出,BLE_GPIO_MODE_IN/BLE_GPIO_MODE_OUT
  * @param   Gpionum:blegpio号
  * @param   ucConfig:设置输入的上下拉或者输出的高低电平
  * @return  BLE_OK:设置成功
  * @note   
  */
BLE_ERR_E ble_gpio_set(unsigned char Mode, unsigned char Gpionum, unsigned char ucConfig)
{
    uint8_t param[5] = {0};
    BLE_ERR_E ble_param = BLE_OK;

    param[0] = Mode;
    param[1] = Gpionum;
    param[2] = ucConfig;
    ble_param = ble_config_op(HCI_CMD_SET_GPIO, (char *)param, 3);

    return ble_param;
}

/**
  * @brief   获取BLEGPIO电平状态
  * @param   Gpionum:blegpio号
  * @return  0:低；1:高; 2:error
  * @note   
  */
uint8_t ble_gpio_get(unsigned char Gpionum)
{
	BLE_ERR_E ble_param = BLE_OK;

    ble_param = ble_config_op(HCI_CMD_READ_GPIO, &Gpionum, 1);

	if(ble_param == BLE_OK)
	{
		return g_ble_rsp_info->gpio_state;
	}
	else
		return 2;
}