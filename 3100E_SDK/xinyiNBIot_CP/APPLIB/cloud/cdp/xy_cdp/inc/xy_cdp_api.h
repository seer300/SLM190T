    /**
* @file        xy_cdp_api.h
* @brief       This file is the Cdp API that users can call.
* @attention   请参考cdp_opencpu_demo.c进行二次开发。 \n
*  服务器的搭建，请按照文档《芯翼XY1100电信云开发及调试指南_V1.1》操作
*/
#pragma once
#include <stdint.h>



/**
 * @ingroup cdpapi
 * @brief 设置云平台的IP和PORT信息
 * @param ip_addr_str is server ip
 * @param port is server port
 * @return BOOL,see  @ref  xy_ret_Status_t.
 */
int cdp_cloud_setting(char *ip_addr_str, int port);

/**
 * @ingroup cdpapi
 * @brief 执行云平台注册流程
 * @param lifetime is cdp life
 * @param timeout is registering waiting time, 92 seconds or more is recommended
 * @return BOOL,see  @ref  xy_ret_Status_t.
 */
int cdp_register(int lifetime, int timeout);

/**
 * @ingroup cdpapi
 * @brief 执行云平台去注册流程
 * @param timeout is deregistering waiting time, 92 seconds or more is recommended
 * @return BOOL,see  @ref  xy_ret_Status_t.
 */
int cdp_deregister(int timeout);

/**
 * @ingroup cdpapi
 * @brief 发送数据到云平台的同步接口
 * @param data is uplink data content
 * @param len is data size
 * @param msg_type, 0 is cdp_CON; 1 is cdp_NON; 2 is cdp_NON_RAI; 3 is cdp_CON_RAI; 4 is cdp_CON_WAIT_REPLY_RAI
 * @return BOOL,see  @ref  xy_ret_Status_t.
 * @note 推荐使用该接口发送上行数据
 */
int cdp_send_syn(char *data, int len, int msg_type);


/**
 * @ingroup cdpapi
 * @brief 发送数据到云平台的异步接口
 * @param data is uplink data content
 * @param len is data size
 * @param msg_type, 0 is CON;1 is NON
 * @return MID值；若为XY_ERR，表示发送异常
 * @note 该接口可配合cdp_send_asyn_ack回调函数使用，
 *       函数返回的mid与回调函数中的mid是一一对应的    
 */
int cdp_send_asyn(char *data, int len, int msg_type);

/**
 * @ingroup cdpapi
 * @brief 更新云平台的lifetime时间
 * @param timeout is cloud replay ack time out, 92 seconds or more is recommended
 * @return BOOL,see  @ref  xy_ret_Status_t.
 */
int cdp_lifetime_update(int timeout);

/**
 * @ingroup cdpapi
 * @brief 检测是否可以发送数据到云平台
 * @return BOOL,see  @ref  xy_ret_Status_t.
 */
int cdp_send_status_check();

/**
 * @ingroup cdpapi
 * @brief 确认消息ACK的回调接口，需与cdp_send_asyn接口配合使用
 * @param mid is ack package message id
 * @param BOOL,see  @ref  xy_ret_Status_t.
 * @warning 用户需自行实现该函数，使用cdp_callbak_set接口进行注册
 *          用户在该回调函数中不能耗时太长或阻塞处理数据，应将数据取出在另一线程中处理  
 */
typedef void (*cdp_send_asyn_ack)(int mid, int result);
extern cdp_send_asyn_ack g_cdp_send_asyn_ack;

/**
 * @ingroup cdpapi
 * @brief 接收下行数据包的回调函数
 * @param data is downlink data content
 * @param data_len is downlink data length
 * @warning 用户需自行实现该函数，使用cdp_callbak_set接口进行注册
 *          用户在该回调函数中不能耗时太长或阻塞处理数据，应将数据取出在另一线程中处理  
 */
typedef void (*cdp_downstream_callback)(char *data, int data_len);
extern cdp_downstream_callback g_cdp_downstream_callback;

/**
 * @ingroup cdpapi
 * @brief 设置API相关的回调函数
 * @param downstream_callback 接收下行数据包的回调函数指针
 * @param cdp_send_asyn_ack 确认消息ACK的回调函数指针
 * @warning 该函数可设置两个回调函数，用户根据自身需求进行设置
            如若只设置回调函数，另一个参数传NULL即可
 */
int cdp_callbak_set( cdp_downstream_callback downstream_callback, cdp_send_asyn_ack send_asyn_ack);

/**
 * @ingroup cdpapi
 * @brief 设置cdp Iot平台endpointname接口
 * @param char * endpointname 用户设置Iot平台的endpointname 
 * @warning 该函数可根据用户需求设置endpointname，长度不可超过255个字符         
 */
int cdp_set_endpoint_name(char * endpointname);

/**
 * @ingroup cdpapi
 * @brief 获取cdp Iot平台endpointname接口
 * @param NULL 
 * @return char* 返回有效的endpointname
 */
char *cdp_get_endpoint_name();


