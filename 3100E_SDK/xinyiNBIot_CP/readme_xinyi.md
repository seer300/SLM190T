# SDK˵��

## ���뻷������ 

1. ����kernel��Ҫarmcc���빤�ߡ��ṩ�������ַ�ʽʹ��armcc��
    (1) ʹ����ql-cross-tool\win32\owtoolchainĿ¼���ṩ��ammcc���빤�ߣ�����Ҫ��ql-cross-tool\win32\owtoolchainĿ¼��������license��
	(2) ��װDS-5�������������armcc���빤������Ŀ¼���뻷������PATH�У���ӱ���ARMLMD_LICENSE_FILE�����������У�ARMLMD_LICENSE_FILE����Ϊarmcc��ʹ�õ�license��
2. ����APP��ql-application����Ҫarm-gcc���빤�ߣ�SDK�����ṩ��

## Ŀ¼�ṹ

### xinyiNBIot_AP	��Ŀ¼ 

	---APPLIB         ҵ��⣬�û����޸ģ�����ά��
		---at_cmd	    AT�����
		---cJSON	    ��Դ�⣬�ͻ�����ά��
		---cloud	    �����Ʋο�����
			---cdp	        ��Ϊ�Ʋο�����
			---ctwing	    �����Ʋο�����
			---onenet	    �����Ʋο�����
			---utils	    �����ƹ�������
		---dm	      ��ͨ/������ע��ο�����
		---Dtls	      DTLS��Դ��
		---fs	      littlefs�ļ�ϵͳ��֧����������
		---http	      http�ο�����
		---libcoap	  coap��Դ��
		---lwip	      tcpipЭ��ջ�ο����룬������ͻ��޸�
		---mqtt	      mqtt�ο�����
		---netled	  ��·ָʾ�Ʋο�����
		---perf	      �������perf�ο����룬��о���ڲ�ʹ��
		---ping	      PING���ο����룬�ͻ�����ά��
		---socket	  socket��AT����ο�����
		---wakaama	  lwm2m��Դ��
		---wireshark  ץ�����Դ��룬��о���ڲ�ʹ��
		---xy_fota	  о������FOTA�ο�����

		
	---ARCH           cortex-M3�ܹ���ش��룬����ϵͳ�������
		
		
	---DRIVERS        о��ײ������⺯��
	
	
	---KERNEL         freertos���û����ܵ���cmsis_os2.h�еĽӿ�

	---NBIotPs	      3GPPЭ��ջ

	---NBPHY	      3GPP�����
	
	---TARGETS	      ������ڣ��û����������ض������__WEAK������
	
		
	---SYSAPP	  ϵͳ�ں˴��뼰��ؿ⣬�û������޸ģ�ֻ�ܵ���xy_��ͷ��ͷ�ļ���API�ӿ�
		---at_ctrl	   AT�����
		---at_uart     AT����������أ�����ATͨ���ҽ���CP�˷���Ч
		---diag_ctrl   log����ģ��
		---flash       flash�����ӿڣ���eftlĥ���㷨�ӿ�
		---rtc_tmr     RTCģ��
		---shm_msg     �˼���Ϣ����
		---smartcard   SIM������
		---system      ����ƽ̨ϵͳ���ӿڣ��������Ź�/ʡ���


## ����ָ�

1. ql-sdkĿ¼�£�ִ�� build.bat app		����APP����(application)������appδ���κ��޸ģ����Բ�ִ��
2. ql-sdkĿ¼�£�ִ�� build.bat kernel		����kernel������kernelδ���κ��޸ģ����Բ�ִ��
3. ql-sdkĿ¼�£�ִ�� build.bat bootloader	����bootloader������bootloaderδ���κ��޸ģ����Բ�ִ��
4. ql-sdkĿ¼�£�ִ�� build.bat firmware	���ɹ̼�

>> ע��δִ�б�������������Ĭ��ʹ��ql-sdk\ql-config\quec-project\aboot\imagesĿ¼�µľ���


## Release History��

**[XINYI1200] 2022-10-15**
- 1��SDK ���棬�ṩ�����ܡ�
- 2���ṩ�ں˱���ָ�
- 3�����SIM���豸��Ϣ��������Ϣ��UART��GPIO�����ݲ��š�OS��socket���ļ�ϵͳ�ȹ��ܼ�API��


