/** 
* @file        
* @brief       vsim适配层，内部不得长时间锁中断，以防止影响3GPP的时序。默认硬SIM，通过AT+NV=SET,VSIM,1可以切换为软SIM
* @warning     
*/
#pragma once
#include <ctype.h>
#include <float.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vsim_adapt.h"
#include "xy_system.h"
#include "xy_ps_api.h"
#include "smartcard.h"



void vsim_Init()
{}



bool Is_vsim_type()
{
#if 1
	return 0;
#else
	return (g_softap_fac_nv->sim_type==0);
#endif
}



/*内部不得长时间锁中断，以防止影响3GPP的时序*/
int vsim_apdu_process(uint8_t *apdu_req,uint16_t apdu_req_len,uint8_t *apdu_rsp,uint16_t *apdu_rsp_len)
{
	return 1;
}





