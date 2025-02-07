/** 
* @file     user_hook.c
* @brief    用户重新实现的弱函数，根据自身需求实现需要的弱函数即可
*/

#include "xy_utils_hook.h"
#include "xy_at_api.h"
#include "xy_memmap.h"
#include "xy_system.h"
#include "xy_utils.h"
#include "xy_rtc_api.h"
#include "xy_at_api.h"
#include "at_uart.h"
#include "xy_flash.h"
#include "oss_nv.h"




void *g_user_nv = NULL;

void malloc_user_mem()
{
#if 0
	if(NOT_ALLOWED_SAVE_FLASH())
	{
		g_user_nv = NV_MALLOC(1000);

		/*若AP保持供电，则该部分内存有效，不得初始化0*/
		if(Is_Fac_InVar_NV_Valid())
			return;
		memset(g_user_nv,0,1000);
	}
	else
		g_user_nv = xy_malloc(1000);
#endif
}



application_init(malloc_user_mem);



