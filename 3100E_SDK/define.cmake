set(PRJ_DEF_LIST "_REENT_SMALL" "_REENT_GLOBAL_ATEXIT" "PLATFORM_M3_AP" "GPIO_NEW_DRIVER")

##################用户仅需关注如下几个配置宏，不得轻易修改其他宏值#################

if (NOT DEFINED XY_SOC_VER)
	set(XY_SOC_VER 1)  #1:3100E模组;
endif ()

set(VER_BC95 0)                           #需要与CP核保持一套AT命令集。1表示对标BC95的AT命令
	list(APPEND PRJ_DEF_LIST "VER_BC95=${VER_BC95}")  

set(VER_BC25 0)                           #需要与CP核保持一套AT命令集。1表示对标BC25的AT命令
	list(APPEND PRJ_DEF_LIST "VER_BC25=${VER_BC25}")  
	
set(VER_260Y 0)                           #需要与CP核保持一套AT命令集。1表示对标BC260Y的AT命令
	list(APPEND PRJ_DEF_LIST "VER_260Y=${VER_260Y}") 
	
list(APPEND PRJ_DEF_LIST "FOTA_SUCC_REPORT=1")   #仅对OPENCPU产品有效，指示FOTA升级成功后是否立即启动CP核上报FOTA成功结果给云平台

list(APPEND PRJ_DEF_LIST "XY_DUMP=0")    #用于AP和CP断言等异常死机定位。设为1后，关闭看门狗，且死机时维持现场，以供在线JLINK查看。

list(APPEND PRJ_DEF_LIST "XY_LOG=1")     #AP核打印输出控制宏，开启后会影响运行流程，产线或功耗测试必须设为0

list(APPEND PRJ_DEF_LIST "XY_AT_CTL=0")  #芯翼的AP和CP核间通信的AT框架开关。0:关闭，常见于表计客户，必须配合at_uart_read接口使用；1:开启，常见于普通客户

list(APPEND PRJ_DEF_LIST "AT_LPUART=1")  #LPUART作为与外部MCU的AT通路的开关。0:关闭，常用于OPENCPU形态；1:开启，常用于模组

list(APPEND PRJ_DEF_LIST "KEEP_CP_ALIVE=1")  #开启CP核的RTC唤醒能力，即PSM/TAU/UPDATE等定时器的深睡唤醒能力

list(APPEND PRJ_DEF_LIST "MPU_EN=0")     #设为1表示开启FLASH上禁止运行代码的能力，以供用户通过Flash_mpu_Lock()排查不合理的FLASH运行代码
#####################################################################################

set(CONFIG_COMPILE_PROJECT_SELECT "default") # PROJECT文件夹下文件夹名"default""menci_demo""cloud_demo""meter_demo""water_meter_demo""gas_meter_demo"

list(APPEND PRJ_DEF_LIST "XY_DEBUG=0")   #AP核调试代码编译宏，仅芯翼内部使用。产线或功耗测试必须设为0

list(APPEND PRJ_DEF_LIST "BLE_EN=0")     #BLE功能开关

list(APPEND PRJ_DEF_LIST "GNSS_EN=1")    #GNSS功能开关

if(${XY_SOC_VER} EQUAL 1)   #3100E模组
	list(APPEND PRJ_DEF_LIST "MODULE_VER=1")#模组产品形态 
    list(APPEND PRJ_DEF_LIST "AT_WAKEUP_SUPPORT=1")  #是否支持AT唤醒。0：不支持，1：支持; OPENCPU版本只能选1
	list(APPEND PRJ_DEF_LIST "BAN_WRITE_FLASH=0")
	list(APPEND PRJ_DEF_LIST "LLPM_VER=0")
	set(CONFIG_FLASH_GIGA_2M "y")# y:2M_FLASH；n:4M_FLASH
	list(APPEND PRJ_DEF_LIST "SYS_CLK_SRC=3")    #系统时钟选择，目前仅支持1或3取值。0:32K_CLK 1:HRC,26M 2:XTAL 3:PLL,368.64M 
	list(APPEND PRJ_DEF_LIST "RC26MCLKDIV=2")    #当SYS_CLK_SRC选择PLL时，该参数无意义
	list(APPEND PRJ_DEF_LIST "AP_HCLK_DIV=10")   #模组形态使用PLL的AP分频系数，目前仅支持10，值为PLL/分频系数=36.8M
	list(APPEND PRJ_DEF_LIST "DEBUG_MODULE_RUN_TIME=0")  #模组形态使用，精度ms
    set(XY_SOFTWARE_VERSION "V3100EB10001R00C0002")
else()   ###若再新增选项，请务必同步修改config.cmake
	MESSAGE(FATAL_ERROR "config error")	
endif()

list(APPEND PRJ_DEF_LIST "XY_SOFTWARE_VERSION=\"${XY_SOFTWARE_VERSION}\"")
set(CONFIG_DYN_LOAD_SELECT 0)   #只要不向SO文件夹中放代码，动态加载未生效，即SO_AVAILABLE=0。故默认开启该宏
list(APPEND PRJ_DEF_LIST "CP_USED_APRAM=45")  #CP向AP借用的RAM大小，KB为单位，目前未使用，严禁客户修改
list(APPEND PRJ_DEF_LIST "LSIO_CLK_SRC=0x3")# 1:FROM_32K_CLK；2:FROM_XTALM；3:FROM_AON_HRC_CLK
list(APPEND PRJ_DEF_LIST "CP_HCLK_DIV=6")
list(APPEND PRJ_DEF_LIST "PERI1_PCLK_DIV=1")
list(APPEND PRJ_DEF_LIST "PERI2_PCLK_DIV=1")

list(APPEND PRJ_DEF_LIST "XY_FOTA=1")
list(APPEND PRJ_DEF_LIST "CONFIG_FEATURE_FOTA=1") #此功能宏为CDP云FOTA功能，若用户不用CDP云FOTA功能，可以将此行注释掉或删除
list(APPEND PRJ_DEF_LIST "WEBLOG=0")

list(APPEND PRJ_DEF_LIST "DROP_URC=0")   #0:默认状态，URC既上报给AP侧，也会向AT口输出;1：要求CP侧不将URC上报给AP侧;2:要求CP侧不将URC向AT口输出（debug输出不受影响）

if(${CONFIG_FLASH_GIGA_2M})
    list(APPEND PRJ_DEF_LIST "FLASH_2M=1")#2M FLASH
else()
    list(APPEND PRJ_DEF_LIST "FLASH_2M=0")#4M FLASH
endif()



#动态加载，复用CP RAM
if(${CONFIG_DYN_LOAD_SELECT} EQUAL 0) 
    list(APPEND PRJ_DEF_LIST "DYN_LOAD=0")
elseif(${CONFIG_DYN_LOAD_SELECT} EQUAL 1)  
    list(APPEND PRJ_DEF_LIST "DYN_LOAD=1")
else()
    MESSAGE(FATAL_ERROR "config error") 
endif()


list(APPEND PRJ_DEF_LIST "CACHE_ENABLE_SET=1") #指示是否启用cache，极低功耗产品建议设为0；0：不启用cache；1：表示启用cache

list(APPEND PRJ_DEF_LIST "CP_FAST_RECOVERY_FUNCTION=0") #1:打开CP快速恢复功能；0：关闭CP快速恢复功能
list(APPEND PRJ_DEF_LIST "STANDBY_SUPPORT=1") #1:支持standby；0：不支持standby，opencpu形态可不编译standby相关代码
list(APPEND PRJ_DEF_LIST "LPM_LOG_DEBUG=1") 
list(APPEND PRJ_DEF_LIST "DEEPSLEEP_ADVANCE_TIME_CALCULTATE=0") #1：统计睡眠提前量；

###############################################################
#以下功能宏为特殊用途使用，默认全关闭！不经芯翼FAE支持，请务必不要更改
###############################################################

list(APPEND PRJ_DEF_LIST "OPENCPU_TEST=0")   #"AT+APTEST"测试命令的开关，产线上必须设为0；

list(APPEND PRJ_DEF_LIST "DRIVER_TEST=0")    #产线上必须设为0； 0:不进行驱动测试；1:主机驱动测试；2:从机驱动测试

list(APPEND PRJ_DEF_LIST "USER_IPC_MSG=0")    #用户扩展的自定义核间消息的开关

##！以下宏值用于统计芯翼基础软件函数级耗时，注意不能与DEBUG_MODULE_RUN_TIME同时开启 ！###
list(APPEND PRJ_DEF_LIST "DEBUG_OPENCPU_RUN_TIME=0")  #opencpu形态使用，精度0.1us
list(APPEND PRJ_DEF_LIST "XY_PING=1")
list(APPEND PRJ_DEF_LIST "AT_SOCKET=1")
list(APPEND PRJ_DEF_LIST "XY_WIRESHARK=0")
list(APPEND PRJ_DEF_LIST "XY_PERF=0")
list(APPEND PRJ_DEF_LIST "XY_HTTP=1")
list(APPEND PRJ_DEF_LIST "TELECOM_VER=0")
list(APPEND PRJ_DEF_LIST "MOBILE_VER=0")
list(APPEND PRJ_DEF_LIST "CTWING_VER=0")
list(APPEND PRJ_DEF_LIST "LIBCOAP=0")
list(APPEND PRJ_DEF_LIST "MQTT=1")
list(APPEND PRJ_DEF_LIST "WAKAAMA=0")
# list(APPEND PRJ_DEF_LIST "WEBLOG=0")
list(APPEND PRJ_DEF_LIST "XY_DM=0")
list(APPEND PRJ_DEF_LIST "WITH_MBEDTLS_SUPPORT=0")
# list(APPEND PRJ_DEF_LIST "VER_BC95=0")
list(APPEND PRJ_DEF_LIST "VER_CMIOT=0")
list(APPEND PRJ_DEF_LIST "URC_CACHE=1")

list(APPEND PRJ_DEF_LIST "WITH_LWIP=1")
list(APPEND PRJ_DEF_LIST "LWM2M_CLIENT_MODE=1")
list(APPEND PRJ_DEF_LIST "LWM2M_LITTLE_ENDIAN=1")
list(APPEND PRJ_DEF_LIST "WITH_MBEDTLS=1")
list(APPEND PRJ_DEF_LIST "XINYI_LWM2M_CLIENT_MODE=1")
list(APPEND PRJ_DEF_LIST "XINYI_LWM2M_LITTLE_ENDIAN=1")
list(APPEND PRJ_DEF_LIST "MBEDTLS_CONFIG_FILE=<mbedtls_config_default.h>")
list(APPEND PRJ_DEF_LIST "MBEDTLS_ALLOW_PRIVATE_ACCESS")

list(APPEND PRJ_DEF_LIST "NBIOT_SMS_FEATURE")
list(APPEND PRJ_DEF_LIST "BIP_FEATURE")
list(APPEND PRJ_DEF_LIST "R14_FEATURE")
list(APPEND PRJ_DEF_LIST "ESM_DEDICATED_EPS_BEARER")
list(APPEND PRJ_DEF_LIST "ESM_EPS_BEARER_MODIFY")
list(APPEND PRJ_DEF_LIST "EPS_BEARER_TFT_SUPPORT")
list(APPEND PRJ_DEF_LIST "USIM_FILE_NO_AVAILABLE")
list(APPEND PRJ_DEF_LIST "TEST_MODE_FEATURE")
list(APPEND PRJ_DEF_LIST "SINGLE_CORE")
list(APPEND PRJ_DEF_LIST "_CURR_VERSION=1")
list(APPEND PRJ_DEF_LIST "Custom_09=0")
