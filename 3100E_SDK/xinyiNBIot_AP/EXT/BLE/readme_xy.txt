
该文件夹里内容是BLE相关，选用的芯片为易兆微。

NB系列为芯翼的1200L，AP核可用内存RAM总计93K左右，FLASH空间约400K左右。

实现从模式的数据收发能力，未来支持客户扩展数据类型，以进行NB相关的OPENCPU开发

也支持外部MCU通过标准AT命令进行BLE的操控和数据收发。


HciSendCmdAndWaitAck中调用ble_bit_set的目的？？？ble_bit_clear呢？？？

hci_cmd_bit_get是否考虑做成数组方式？

ble_into_hib、set_ble_hibernate啥意思？？？

ble_con_param_t？？？

hci_cmd_dispatch-》hci_cmd_send-》ble_h5tl_tx-》HAL_UART_Transmit，发送消息数据给BLE

hci_wait_cmd_cmp,检查是否收到ack确认数据

HAL_UART_RxCpltCallback-》ble_event_msg_recv-》check_ble_cmd_rsp-》ble_bit_set，在接收中断中识别出短的ACK数据后，直接置位图；以便hci_wait_cmd_cmp死循环等到ACK确认结果



BLE_cfg_t、g_softap_fac_nv、g_ble_var_cfg、ble_var_config_t，统一放ble_hci.h中，且不考虑FLASH和SRAM先
ble_con_param_t、chuuid_handle_t去除，直接展开
cmd_payload_rsp



xy_list.h移到utitls文件夹中