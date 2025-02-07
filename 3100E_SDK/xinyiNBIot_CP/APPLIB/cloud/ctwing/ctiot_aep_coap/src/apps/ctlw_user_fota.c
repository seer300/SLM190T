#if 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "ctlw_user_fota.h"
#include "ctlw_parseuri.h"
#include "../core/er-coap-13/ctlw-er-coap-13.h"
#include "ctlw_connection.h"

#include "ctlw_lwm2m_sdk.h"
#include "ctlw_abstract_signal.h"

#define MAX_PACKET_SIZE_1 1024

extern ctiotUserFotaManage fotaStateManage;

CTIOT_URI fotaUrl;

static int messageID = 6000;
static int sockfd;
struct sockaddr_in addr;
int addr_len = sizeof(struct sockaddr_in);

char *uri = NULL;
char *query = NULL;
uint32_t blockNumber = 0;
uint8_t resendCount = 5;
thread_handle_t fota_app_tid;
/*芯片投票handler*/
static uint8_t ctlw_user_fota_handler = 0xff;
uint8_t ctlw_get_app_vote_handler()
{
	return ctlw_user_fota_handler;
}
void ctlw_init_vote_hander(void)
{
	ctchip_init_app_slp_handler("USER_VOTE_APP",&ctlw_user_fota_handler);
}
/******************************************************************/

void ctlw_user_fota_state_changed(void)
{
	lwm2m_uri_t uri;
	ctlw_lwm2m_stringToUri("/5/0", 4, &uri);

	lwm2m_data_t dataArrayP[2];
	memset(dataArrayP,0,2*sizeof(lwm2m_data_t));
	dataArrayP[0].id = 3;
	ctlw_lwm2m_data_encode_int(fotaStateManage.fotaState, &dataArrayP[0]);
	dataArrayP[1].id = 5;
	ctlw_lwm2m_data_encode_int(fotaStateManage.fotaResult, &dataArrayP[1]);
	char * buff = NULL;
	extern int16_t ctiot_lwm2m_obj_notify_data_serialize(char* uri, uint16_t size, lwm2m_data_t * dataP, ctiot_send_format_e sendFormat, char** buff);
	int len = ctiot_lwm2m_obj_notify_data_serialize("/5/0", 2, dataArrayP, DATA_FORMAT_TLV, &buff);
	if(len < 0)
	{
		ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"fota result serialize error");
		return ;
	}
	uint16_t msgId = 0;
	uint16_t result = ctiot_lwm2m_obj_notify(buff, len, SENDMODE_CON, DATA_FORMAT_TLV, "/5/0", &msgId);
	ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"fota result notify : %u\t msgid:%u", result,msgId);
	ctlw_lwm2m_free(buff);
	buff = NULL;
	return;
}

static void setUrl(void)
{
	char *queryTemp = NULL;
	uri = strtok(fotaUrl.uri, "?");
	queryTemp = strtok(NULL, "?");
	printf("uri is %s\n", uri);
	if (queryTemp != NULL)
	{
		query = (char *)ctlw_lwm2m_malloc(strlen(queryTemp) + 2);
		memset(query, 0, strlen(queryTemp) + 2);
		strcpy(query, "?");
		strcat(query, queryTemp);

		ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"query is %s\n", query);
	}
}

static int send_resquest(uint32_t num, uint8_t more, uint16_t size)
{
	int result = 0;

	static ctlw_coap_packet_t message[1];
	ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"message ID is %d\n", messageID);
	ctlw_coap_init_message(message, COAP_TYPE_CON, COAP_GET, messageID);
	ctlw_coap_set_header_uri_path(message, uri);
	if (query != NULL)
		ctlw_coap_set_header_uri_query(message, query);
	ctlw_coap_set_header_block2(message, num, more, size);
	uint8_t *pktBuffer;
	size_t pktBufferLen = 0;
	size_t allocLen;

	allocLen = ctlw_coap_serialize_get_size(message);
	if (allocLen == 0)
		return COAP_500_INTERNAL_SERVER_ERROR;

	pktBuffer = (uint8_t *)ctlw_lwm2m_malloc(allocLen);
	if (pktBuffer != NULL)
	{
		pktBufferLen = ctlw_coap_serialize_message(message, pktBuffer);
		if (0 != pktBufferLen)
		{
			result = sendto(sockfd, pktBuffer, pktBufferLen, 0, (struct sockaddr *)&addr, addr_len);
			ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"sendto result %d\n", result);
		}
		ctlw_lwm2m_free(pktBuffer);
	}

	return result;
}

uint32_t fsize(FILE *fp)
{
	uint32_t n;
	fpos_t fpos;		//当前位置
	fgetpos(fp, &fpos); //获取当前位置
	fseek(fp, 0, SEEK_END);
	n = ftell(fp);
	fsetpos(fp, &fpos); //恢复之前的位置
	return n;
}

static void *user_firmware_fota(void *arg)
{
	uint32_t packetSize = 0;
	bool runThread = 1;
	FILE *fp;
	fp = fopen("nbfota.txt", "wb");
	if (fp == NULL)
	{
		ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"open nbfota.txt error\n");
	}
	setUrl();
	send_resquest(START_NUM, START_MORE, BLOCK_SIZE);

	while (runThread)
	{
		struct timeval tv;
		int result;
		tv.tv_sec = 5; //读socket时间
		tv.tv_usec = 0;
		uint8_t buffer[MAX_PACKET_SIZE_1];
		int numBytes;
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		result = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
		if (result > 0)
		{
			if (FD_ISSET(sockfd, &readfds))
			{
				struct sockaddr_storage addr;
				socklen_t addrLen;
				addrLen = sizeof(addr);
				numBytes = recvfrom(sockfd, buffer, MAX_PACKET_SIZE_1, 0, (struct sockaddr *)&addr, &addrLen);

				if (0 > numBytes)
				{
					fprintf(stderr, "Error in recvfrom(): %d %s\r\n", errno, strerror(errno));
				}
				else if (0 < numBytes)
				{
					//fprintf(stdout,"~~~~~~fota ~~~~~~~~~~~ numbytes %d readed! ~~~~~~~~~~\n",numBytes);

					uint8_t ctlw_coap_error_code = NO_ERROR;
					static ctlw_coap_packet_t message[1];
					ctlw_coap_error_code = ctlw_coap_parse_message(message, buffer, numBytes);
					//printf("Parsed: ver %u, type %u, tkl %u, code %u.%.2u, mid %u, Content type: %d\n",
					//message->version, message->type, message->token_len, message->code >> 5, message->code & 0x1F, message->mid, message->content_type);
					//printf("PayloadLen:%d\tPayload: %s\n", message->payload_len, message->payload);
					if (ctlw_coap_error_code == NO_ERROR)
					{
						switch (message->type)
						{
						case COAP_TYPE_NON:
						case COAP_TYPE_CON:
						case COAP_TYPE_RST:
							printf("no ack\n");
							break;
						case COAP_TYPE_ACK:
						{
							ctlw_coap_get_header_size(message, &packetSize);
							//packetSize++;
							ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"packetSize is %d\n", packetSize);
							//printf("receive buffer is %s\n",message->payload);
							ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"receive buffer len is %d\n", message->payload_len);

							if (fwrite(message->payload, message->payload_len, 1, fp) <= 0)
							{
								runThread = 0;
								fclose(fp);
								ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"write file error\n");
							}

							ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"message->block2_more is %d\n", message->block2_more);
							if (message->block2_more != 0)
							{
								resendCount = 5;
								blockNumber++;
								messageID++;
								if (send_resquest(message->block2_num + 1, 0, message->block2_size) == -1)
								{
									fotaStateManage.fotaState = FOTA_STATE_DOWNLOADING;
									fotaStateManage.fotaResult = FOTA_RESULT_DISCONNECT;
									ctlw_user_fota_state_changed();
								}
							}
							else
							{
								ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"fsize(fp) is %d\n", fsize(fp));
								if (fsize(fp) == packetSize)
								{
									fotaStateManage.fotaState = FOTA_STATE_DOWNLOADED;
									fotaStateManage.fotaResult = FOTA_RESULT_INIT;
									ctlw_user_fota_state_changed();
								}
								else
								{
									fotaStateManage.fotaState = FOTA_STATE_DOWNLOADING;
									fotaStateManage.fotaResult = FOTA_RESULT_CHECKFAIL;
									ctlw_user_fota_state_changed();
								}

								runThread = 0;
								fclose(fp);
								ctlw_lwm2m_free(query);
								ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"runThread ended\n");
							}

							break;
						}

						default:
							break;
						}
					} /* Request or Response */
				}
			}
		}
		else
		{
			if (resendCount == 0)
			{
				printf("platform no ack received!\n");
				runThread = 0;
				fclose(fp);
				ctlw_lwm2m_free(query);
				ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"runThread ended\n");
			}
			else
			{
				send_resquest(blockNumber, START_MORE, BLOCK_SIZE);
				ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"!!!!!!!!!!!!!!!resend packet!!!!!!!!!!!!!!!!\n");
				resendCount--;
			}
		}
	}

	/*
		if(fotaStateManage.fotaState = FOTA_STATE_DOWNLOADING && (fotaStateManage.fotaResult == FOTA_RESULT_DISCONNECT || fotaStateManage.fotaResult == FOTA_RESULT_CHECKFAIL))
		{
			ReDownloadtimes++;
			if(ReDownloadtimes <= MAX_DOWNLOAD_TIMES)
				goto ReDownload;
		}
    */
    ctiot_chip_vote(ctlw_get_app_vote_handler(), SYSTEM_STATUS_FREE);
	thread_exit(NULL);
	return NULL;
}

static bool ctlw_user_fota_create_socket(char *host, int port)
{
	bool result = true;
	/* 建立socket，注意必须是SOCK_DGRAM */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket");
		result = false;
	}

	/* 填写sockaddr_in*/
	memset(&addr, 0x00, sizeof(addr));
	addr.sin_family = AF_UNSPEC;
	addr.sin_port = htons(fotaUrl.port);
	addr.sin_addr.s_addr = inet_addr(fotaUrl.address);

	return result;
}

int ctlw_user_fota_start(char *url)
{
	int err = 0;
	bool result = true;
	thread_handle_t tid;
	ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"ctlw_user_fota_start object 5\r\n");
	memset(&fotaUrl, 0, sizeof(fotaUrl));
	result = ctiot_parse_url(url, &fotaUrl);
	if (result == false)
	{
		fotaStateManage.fotaState = FOTA_STATE_IDIL;
		fotaStateManage.fotaResult = FOTA_RESULT_URIINVALID;
		ctlw_user_fota_state_changed();
		goto exit;
	}
	if (strcmp(fotaUrl.protocol, "coap") != 0)
	{
		printf("fotaUrl.protocol is %s\n", fotaUrl.protocol);
		fotaStateManage.fotaState = FOTA_STATE_IDIL;
		fotaStateManage.fotaResult = FOTA_RESULT_PROTOCOLFAIL;
		ctlw_user_fota_state_changed();
		goto exit;
	}

	//create socket
	if (ctlw_user_fota_create_socket(fotaUrl.address, fotaUrl.port) == false)
	{
		ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"can't create socket\n");
		goto exit;
	}

	//create thread
	err = thread_create(&tid, NULL, user_firmware_fota, NULL);
	if (err != 0)
	{
		ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"can't create thread: %s\n", strerror(err));
		goto exit;
	}

exit:
	return result;
}

static uint16_t ctiotprv_check_ip_and_eps(uint8_t type) // type = 0 IP和EPS都检查  type=1 仅检查IP
{
	uint16_t result = CTIOT_NB_SUCCESS;
	ctiot_context_t *pContext = ctiot_get_context();

	//检查IP
	chip_ip_type_e ipType = (chip_ip_type_e)ctiot_signal_get_chip_ip_type();
	if (pContext->socketIPType == 1)
	{
		if(ipType == CHIP_IP_TYPE_V6ONLY || ipType == CHIP_IP_TYPE_V6ONLY_V6PREPARING)
		{
			result = CTIOT_IP_TYPE_ERROR;
		}
		else if(ipType == CHIP_IP_TYPE_FALSE)
		{
			result = CTIOT_IP_NOK_ERROR;
		}
		if(result != CTIOT_NB_SUCCESS)
			return result;
	}
	else if(pContext->socketIPType == 2)
	{
		if(ipType == CHIP_IP_TYPE_V4ONLY)
		{
			result = CTIOT_IP_TYPE_ERROR;
		}
		else if(ipType == CHIP_IP_TYPE_FALSE)
		{
			result = CTIOT_IP_NOK_ERROR;
		}
		else if(ipType == CHIP_IP_TYPE_V6ONLY_V6PREPARING || ipType == CHIP_IP_TYPE_V4V6_V6PREPARING)
		{
			result = CTIOT_IPV6_ONGOING_ERROR;
		}
		if(result != CTIOT_NB_SUCCESS)
			return result;
	}

	//判断EPS
	if(type != 1)
	{
		uint8_t epsValue = ctchip_sync_cstate();
		if(epsValue != NETWORK_CONNECTED_HOME_NETWORK && epsValue != NETWORK_CONNECTED_ROAMING)
		{
			result = epsValue + CTIOT_NETWORK_ERROR_BASE;
		}
	}
	return result;
}




static void* ctlw_fota_thread_func(void* param)
{
	int fota_started = 0;//0-无fota,1=fota 处理中
	int fota_result = 0;//fota 处理结果,0-失败，1-成功
	ctiot_context_t* pContext = ctiot_get_context();
	FILE *fp = fopen("fota_data.txt", "r");
	if(fp==NULL)
	{
		ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"open fota_data.txt failed\r\n");
		goto exit;
	}
	while(!feof(fp))
	{
		fscanf(fp,"%d %d",&fota_started,&fota_result);
	}
	if(fota_started == 0)
	{
		goto exit;
	}
	while(ctiotprv_check_ip_and_eps(0)!=CTIOT_NB_SUCCESS || pContext->sessionStatus != UE_LOGINED)
	{
		ctlw_usleep(1000);
	}

	//准备fota结果，并发送
	if(fota_result == 0)
	{
		fotaStateManage.fotaState = FOTA_STATE_DOWNLOADED;
		fotaStateManage.fotaResult = FOTA_RESULT_UPDATEFAIL;
		ctlw_user_fota_state_changed();
	}
	else if(fota_result == 1)
	{
		fotaStateManage.fotaState = FOTA_STATE_IDIL;
		fotaStateManage.fotaResult = FOTA_RESULT_SUCCESS;
		ctlw_user_fota_state_changed();
	}
exit:
	if(fp!=NULL)
	{
		fclose(fp);
		remove("fota_data.txt");
	}
	ctiot_chip_vote(ctlw_user_fota_handler, SYSTEM_STATUS_FREE);
	ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"fota app ended...\r\n");
	thread_exit(NULL);
	return NULL;
}


uint16_t ctlw_user_fota_start_func(void)
{
	uint16_t result = 0;
	ctlw_init_vote_hander();
	printf("fota app start...\r\n");
	ctiot_chip_vote(ctlw_user_fota_handler, SYSTEM_STATUS_BUSY);
	int err = thread_create(&fota_app_tid, NULL, ctlw_fota_thread_func, NULL);
	if (err != 0)
	{
		printf("can't create thread: %s\n", strerror(err));
	}
	return result;
}



/*app notify*/
void ctlw_fota_app_notify(char* uri, ctiot_notify_type notifyType, ctiot_notify_subType subType, uint16_t value, uint32_t data1, uint32_t data2, void *reservedData)

{
	ctiot_log_debug(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"ctlw_fota_app_notify,notifyType=%u,subType=%u,value=%u\r\n",notifyType,subType,value);
	if (subType == CTIOT_NOTIFY_SUBTYPE_LSTATUS && value >= 2)
	{
		return;
	}
	switch (notifyType)
	{
		case CTIOT_NOTIFY_ASYNC_NOTICE:
		{
			switch (subType)
			{
				case CTIOT_NOTIFY_SUBTYPE_REG:
				{
					ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"CTIOT_NOTIFY_SUBTYPE_REG,result=%u\r\n",value);
					break;
				}
				case CTIOT_NOTIFY_SUBTYPE_LWEVENT:
				{
					ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"CTIOT_NOTIFY_SUBTYPE_REG,result=%u,data1=%u\r\n",value,data1);
					break;
				}
				case CTIOT_NOTIFY_SUBTYPE_UPDATE:
				{
					ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"CTIOT_NOTIFY_SUBTYPE_REG,result=%u,data1=%u\r\n",value,data1);
					break;
				}
				case CTIOT_NOTIFY_SUBTYPE_DEREG:
				{
					ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"CTIOT_NOTIFY_SUBTYPE_REG,result=%u\r\n",value);
					break;
				}
				case CTIOT_NOTIFY_SUBTYPE_SEND:
				{
					ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"CTIOT_NOTIFY_SUBTYPE_REG,result=%u,data1=%u\r\n",value,data1);
					break;
				}
				case CTIOT_NOTIFY_SUBTYPE_LSTATUS:
				{
					ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"CTIOT_NOTIFY_SUBTYPE_REG,result=%u,data1=%u\r\n",value,data1);
					break;
				}
				case CTIOT_NOTIFY_SUBTYPE_DTLS_HS:
				{
					ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"CTIOT_NOTIFY_SUBTYPE_REG,result=%u\r\n",value);
					break;
				}
				case CTIOT_NOTIFY_SUBTYPE_SESSION_RESTORE:
				{
					ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"CTIOT_NOTIFY_SUBTYPE_SESSION_RESTORE\r\n");
					ctlw_user_fota_start_func();
					break;
				}
				default:
				{
					ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"unknown subtype\r\n");
					break;
				}
			}
			break;
		}
		case CTIOT_SYS_API_ERROR:
		{
			ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"send error,value = %u",value);
			break;
		}
		case CTIOT_NOTIFY_RECV_DATA:
		{
			if(subType == 0)
			{
				ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"CTIOT_NOTIFY_RECV_DATA:reservedData:%s\r\n",(char *)reservedData);
			}
			else if(subType == 1)
			{
				ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"CTIOT_NOTIFY_RECV_DATA:NULL\r\n");
			}
			break;
		}
		default:
		{
			ctiot_log_info(LOG_COMMON_MODULE,LOG_OTHER_CLASS,"unknown notifyType\r\n");
			break;
		}
	}
}

#endif
