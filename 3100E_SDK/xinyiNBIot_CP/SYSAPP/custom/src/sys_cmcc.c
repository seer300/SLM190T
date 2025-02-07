#if 0

#include "xy_at_api.h"
#include "xy_system.h"


void Sys_Up_URC()
{
	char *at_str = xy_malloc(64);
	snprintf(at_str, 64, "+DBGINFO:POWERON,FastRecovery %d\r\n", at_get_power_on(),Is_UP_From_FastRecvry());
	send_debug_by_at_uart(at_str);


    if (!Is_WakeUp_From_Dsleep())
    {
		snprintf(at_str, 64, "\r\nMATREADY\r\n");
		send_urc_to_ext_NoCache(at_str, strlen(at_str));
    }
	xy_free(dbg_urc);
    if (g_softap_fac_nv->g_NPSMR_enable == 1)
        send_rsp_at_to_ext("\r\n+NPSMR:0\r\n");

}



#endif /* VER_CMIOT */