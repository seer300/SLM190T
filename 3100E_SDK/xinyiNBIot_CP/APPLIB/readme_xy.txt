��Դ��Ϣ˵��
======
  ARM��ʹ�ÿ�Դϵͳfreertos���汾FreeRTOS Kernel V10.3.1����ҵ��ʹ����CDP sdk��ONENET sdk��
  ͬʱо��SDK������mbedtls��libcoap��wakaama��Դ�⣬��Щ��Դ���Ѿ����Ա���ã��û�����ֱ��ʹ�á����ļ������˿�Դ��ı������Ӻ�ʹ�á�

�û����ο���
========
DEMO��ʹ�ã����Ķ���о��XY1200ƽ̨����ָ��_V1.1�����Լ�demo·���µ�readme

��α������ӿ�Դ�⣿��
===========
  ��SDK�У�\xinyiNBIoT_OS_custom\TARGETS\xinyiNBSoC\GCC-ARM\make\define.mk��ʹ����ؿ�Դ��ĺ궨�弴�ɡ�
  
  ARM mbedtls��Դ�� --- MBEDTLS_SUPPORT
  
  libcoap��Դ��     --- LIBCOAP_SUPPORT
  
  wakaama��Դ��     --- WAKAAMA_SUPPORT
  
  eg��
  	LIBCOAP_SUPPORT=y//��������libcoap��Դ��
  	
  	LIBCOAP_SUPPORT=n//����������libcoap��Դ��


FreeRTOS
======
  ARM��ʹ�ÿ�ԴϵͳFreeRTOS��
  FreeRTOS��ѭ�Ŀ�ԴЭ����MIT���֤������MIT���֤������������ʹ�á��޸ĺͷַ�FreeRTOS������������ҵĿ�ģ�ֻ����������������а���MIT���֤�İ�Ȩ���������������MIT���֤��һ�ֿ��ɵĿ�Դ���֤����������Ա������Ŀ�м��ɺ�ʹ��FreeRTOS������֧�����û��ܵ����ơ�
�汾�� V10.3.1
·����https://www.freertos.org/a00104.html


CDP sdk
======
  cdpҵ�񿪷�ʹ�õ�sdk��֧�ֶԵ���CTWing����ͨ�ͻ�Ϊ��ƽ̨������
  CDP sdk�Ǽ�����LiteOS�ڵģ��汾���ȡ·������LiteOS
�汾��LiteOSV200R001C50B021
·����https://github.com/LiteOS/LiteOS/releases/tag/tag_LiteOS_V200R001C50B021_20180629  
  
ONENET sdk  
======
  �����ṩ��������ҵ��sdk��֧��������onenet ��ƽ̨�ĶԽ�
�汾��2.3.0
·����https://open.iot.10086.cn/doc/nb-iot/book/device-develop/get_SDK.html
  

ARM mbedtls��Դ��
======
  ARM mbedtlsʹ������Ա���Էǳ����ɵ��ڣ�Ƕ��ʽ��Ʒ�м�����ܺ� SSL/TLS ���ܣ������ṩ�˾���ֱ�۵� API �Ϳɶ�Դ����� SSL 
�⡣�ù��߼������ã������ڴ󲿷�ϵͳ��ֱ�ӹ�������Ҳ�����ֶ�ѡ������ø���ܡ�

  mbedtls���ṩ��һ��ɵ���ʹ�úͱ���ļ��������������ʹ�õ�������ͷ�ļ�������ų���Щ�����
  
  �ӹ��ܽǶ���������mbedtls��Ϊ������Ҫ���֣�   
	- SSL/TLS Э��ʵʩ�� 	
	- һ�����ܿ⡣ 	
	- һ�� X.509 ֤�鴦��⡣
  mbed TLS��ѭ�Ŀ�ԴЭ����Apache���֤2.0������Apache���֤2.0������������ʹ�á��޸ĺͷַ�mbed TLS������������ҵĿ�ģ�ֻ���������֤���������������Ʒ�а���ԭʼ��Ȩ���������������Apache���֤2.0��һ�ֿ��ɵĿ�Դ���֤����������Ա������Ŀ�м��ɺ�ʹ��mbed TLS������֧�����û��ܵ����ơ�	
�汾��V3.2.1
·����https://tls.mbed.org	

libcoap��Դ��https://libcoap.net
======
  libcoap��CoAPЭ���C����ʵ�֣�libcoap�ṩserver��client���ܣ����ǵ���CoAP���������ߣ�sdk��
������libcoap����ͨ���Ѹ��򵥵�����˵��libcoap��ʹ�÷�����
  CoAP��Constrained Application Protocol����һ��רΪ���޻��������������豸����Ƶ�Ӧ�ò�Э�飬ͨ����������Դ���޵������н��е͹���ͨ�š�
CoAPЭ�鱾��û���ض��Ŀ�Դ���Э�飬������IETF��Internet Engineering Task Force�����б�׼������ˣ�CoAPЭ���ʹ�ò����ض��Ŀ�Դ���Э�����ƣ�������ѭIETF�ı�׼�����̺͹淶��
�汾��release-4.2.0  
·����https://github.com/obgm/libcoap/tree/release-4.2.0


wakaama��LWM2M��ԴЭ��ջhttps://github.com/eclipse/wakaama
======
  wakaama������ʽ������һ��C�Ŀ��ļ���������Դ�������ʽ��ֱ�Ӻ���Ŀ�������ϱ��롣  
  SDKֻ�����˿ͻ��˴��룬�û������Լ�����ʹ�á�
  LwM2MЭ�鱾��û���ض��Ŀ�Դ���Э�飬������OMA��Open Mobile Alliance���ƶ��͹���OMAͨ������RAND��Reasonable and Non-Discriminatory�����ģʽ������ζ��LwM2MЭ��ı�׼��������ѭ����ͷ�������������ʹ�á���ˣ�LwM2MЭ���ʹ�ò����ض��Ŀ�Դ���Э�����ƣ�����Ҫ��ѭOMA�ı�׼���淶��
�汾�� 
·����https://github.com/eclipse/wakaama
  

MQTT��mqtt��ԴЭ��ջhttps://github.com/eclipse/paho.mqtt.embedded-c/tree/master
======
  MQTT��һ�����ڿͻ���-����������Ϣ����/���Ĵ���Э�飬��Դ�������ʽ���֣�ֱ�Ӻ���Ŀ�������ϱ��롣
  MQTT��Message Queuing Telemetry Transport��Э����һ������������Ϣ����Э�飬ͨ�������������豸֮���ͨ�š�
  MQTTЭ�鱾��û���ض��Ŀ�Դ���Э�飬������OASIS�Ĺ涨��MQTTЭ��ı�׼�ǿ��ŵģ����ҿ������ʹ�ú�ʵ�֡�
  ��ˣ�MQTTЭ���ʹ�ò����ض��Ŀ�Դ���Э�����ƣ�����Ҫ��ѭOASIS�ı�׼���淶��
�汾: MQTT ��Դ����ٷ��Ѳ��ٸ��£�Ŀǰʹ�õ��� master ��֧�����°汾 
·����https://github.com/eclipse/paho.mqtt.embedded-c/tree/master


HTTP��Դ��
======
  HTTPЭ����һ�����ڴ��䳬�ı���Ӧ�ò�Э�顣
  HTTP��Դ���Ǽ�����AliOS Things�ڵģ��汾���ȡ·������AliOS Things��
  HTTP��Hypertext Transfer Protocol����һ�����ڴ��䳬�ı���Ӧ�ò�Э�飬������Web�������Web������֮�䴫�����ݡ�HTTPЭ�鱾��û���ض��Ŀ�Դ���Э�飬������IETF��Internet Engineering Task Force�����б�׼����
  HTTPЭ��ı�׼�ǹ����ģ����ҿ������ʹ�ú�ʵ�֣�����������ض��Ŀ�Դ���Э�����ơ�������Ա�������ɵ�ʹ��HTTPЭ��������WebӦ�ó��򣬶����赣�Ŀ�Դ������⡣
�汾��rel_3.3.0
·����https://gitee.com/alios-things-admin/AliOS-Things/tree/rel_3.3.0/

LwIP��ѭ���¿�ԴЭ�飺
  BSD��Berkeley Software Distribution����Դ���֤��
  MIT��Դ���֤��
  GPL��GNU General Public License����
�汾��rel_2.1.3
·����https://savannah.nongnu.org/projects/lwip/
	
test

 
