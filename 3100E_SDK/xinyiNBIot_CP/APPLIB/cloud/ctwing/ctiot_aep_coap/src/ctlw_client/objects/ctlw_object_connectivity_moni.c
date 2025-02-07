/*******************************************************************************
 *
 * Copyright (c) 2014 Bosch Software Innovations GmbH Germany.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *
 *******************************************************************************/

/*
 *  This Connectivity Monitoring object is optional and has a single instance
 *
 *  Resources:
 *
 *          Name             | ID | Oper. | Inst. | Mand.|  Type   | Range | Units |
 *  Network Bearer           |  0 |  R    | Single|  Yes | Integer |       |       |
 *  Available Network Bearer |  1 |  R    | Multi |  Yes | Integer |       |       |
 *  Radio Signal Strength    |  2 |  R    | Single|  Yes | Integer |       | dBm   |
 *  Link Quality             |  3 |  R    | Single|  No  | Integer | 0-100 |   %   |
 *  IP Addresses             |  4 |  R    | Multi |  Yes | String  |       |       |
 *  Router IP Addresses      |  5 |  R    | Multi |  No  | String  |       |       |
 *  Link Utilization         |  6 |  R    | Single|  No  | Integer | 0-100 |   %   |
 *  APN                      |  7 |  R    | Multi |  No  | String  |       |       |
 *  Cell ID                  |  8 |  R    | Single|  No  | Integer |       |       |
 *  SMNC                     |  9 |  R    | Single|  No  | Integer | 0-999 |   %   |
 *  SMCC                     | 10 |  R    | Single|  No  | Integer | 0-999 |       |
 *
 */

#include "ctlw_liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef PLATFORM_XYZ
#include "ps_lib_api.h"
#endif

#include "ctwing_util.h"

#include "ctlw_lwm2m_sdk.h"
#include "ctlw_platform.h"
// Resource Id's:
#define RES_M_NETWORK_BEARER 0
#define RES_M_AVL_NETWORK_BEARER 1
#define RES_M_RADIO_SIGNAL_STRENGTH 2
#define RES_O_LINK_QUALITY 3
#define RES_M_IP_ADDRESSES 4
#define RES_O_ROUTER_IP_ADDRESS 5
#define RES_O_LINK_UTILIZATION 6
#define RES_O_APN 7
#define RES_O_CELL_ID 8
#define RES_O_SMNC 9
#define RES_O_SMCC 10

#define VALUE_NETWORK_BEARER_GSM 0    //GSM see
#define VALUE_NETWORK_BEARER_NBIOT 7  //nbiot
#define VALUE_AVL_NETWORK_BEARER_1 0  //GSM
#define VALUE_AVL_NETWORK_BEARER_2 21 //WLAN
#define VALUE_AVL_NETWORK_BEARER_3 41 //Ethernet
#define VALUE_AVL_NETWORK_BEARER_4 42 //DSL
#define VALUE_AVL_NETWORK_BEARER_5 43 //PLC
#define VALUE_IP_ADDRESS_1 "192.168.178.101"
#define VALUE_IP_ADDRESS_2 "192.168.178.102"
#define VALUE_ROUTER_IP_ADDRESS_1 "192.168.178.001"
#define VALUE_ROUTER_IP_ADDRESS_2 "192.168.178.002"
#define VALUE_APN_1 "web.vodafone.de"
#define VALUE_APN_2 "cda.vodafone.de"
#define VALUE_CELL_ID 6888
#define VALUE_RADIO_SIGNAL_STRENGTH 80 //dBm
#define VALUE_LINK_QUALITY 98
#define VALUE_LINK_UTILIZATION 666
#define VALUE_SMNC 33
#define VALUE_SMCC 44

typedef struct
{
    char ipAddresses[2][16];       // limited to 2!
    char routerIpAddresses[2][16]; // limited to 2!
    long cellId;
    int signalStrength;
    int linkQuality;
    int linkUtilization;
} conn_m_data_t;

static uint8_t prv_set_value(lwm2m_data_t *dataP,
                             conn_m_data_t *connDataP)
{
    switch (dataP->id)
    {

    case RES_M_NETWORK_BEARER:
        ctlw_lwm2m_data_encode_int(VALUE_NETWORK_BEARER_NBIOT, dataP);
        return COAP_205_CONTENT;

    case RES_M_AVL_NETWORK_BEARER:
    {
        int riCnt = 1; // reduced to 1 instance to fit in one block size
        lwm2m_data_t *subTlvP;
        int networkbearer = 0;

        subTlvP = ctlw_lwm2m_data_new(riCnt);
        if (subTlvP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        subTlvP[0].id    = 0;
        (void)xy_ctlw_cmd_ioctl(CTLW_GET_NETWORK_BEARER, (char *)&networkbearer, sizeof(int));
        ctlw_lwm2m_data_encode_int(networkbearer, subTlvP);
        ctlw_lwm2m_data_encode_instances(subTlvP, riCnt, dataP);
        return COAP_205_CONTENT ;
    }

    case RES_M_RADIO_SIGNAL_STRENGTH: //s-int
    {
        int signalstrength = 0;
        (void)xy_ctlw_cmd_ioctl(CTLW_GET_SIGNAL_STRENGTH, (char *)&signalstrength, sizeof(int));
        ctlw_lwm2m_data_encode_int(connDataP->signalStrength, dataP);
        return COAP_205_CONTENT;
    }
    case RES_O_LINK_QUALITY: //s-int
    {
        int linkQuality;
        (void)xy_ctlw_cmd_ioctl(CTLW_GET_LINK_QUALITY, (char *)&linkQuality, sizeof(int));
        ctlw_lwm2m_data_encode_int(linkQuality, dataP);
        return COAP_205_CONTENT ;
    }
    case RES_M_IP_ADDRESSES:
    {
		int ri=0, riCnt = 0;
		int ipv4_exists=0,ipv6_exists=0;
		/*查询芯片ip*/
		uint8_t ipv4[INET_ADDRSTRLEN]={0},ipv6[INET6_ADDRSTRLEN]={0};
		if(ctlw_get_local_ip((char *)ipv4, AF_INET)==CTIOT_NB_SUCCESS)
		{
		   riCnt ++;
		   ipv4_exists = 1;
		}
		if(ctlw_get_local_ip((char *)ipv6, AF_INET6)==CTIOT_NB_SUCCESS)
		{
		   riCnt ++;
		   ipv6_exists = 1;
		}
		if(riCnt > 0)
		{
			lwm2m_data_t *subTlvP = ctlw_lwm2m_data_new(riCnt);
			if(ipv4_exists == 1)
			{
			   subTlvP[ri].id = ri;
			   ctlw_lwm2m_data_encode_string((char *)ipv4, subTlvP + ri);
			   ri++;
			}
			if(ipv6_exists == 1)
			{
			   subTlvP[ri].id = ri;
			   ctlw_lwm2m_data_encode_string((char *)ipv6, subTlvP + ri);
			}
			
			ctlw_lwm2m_data_encode_instances(subTlvP, riCnt, dataP);
			return COAP_205_CONTENT;
		}
		else
		{
			return COAP_500_INTERNAL_SERVER_ERROR;
		}
    }
    case RES_O_CELL_ID:
    {
		/*查询芯片cellid*/
		uint32_t cellID;
		if(ctchip_get_cell_id(&cellID)==0)
		{
	        ctlw_lwm2m_data_encode_int(cellID, dataP);
	        return COAP_205_CONTENT;
		}
		else
		{
			return COAP_500_INTERNAL_SERVER_ERROR;
		}
    }

    default:
        return COAP_404_NOT_FOUND;
    }
}

static uint8_t prv_conn_read(uint16_t instanceId,
                        int *numDataP,
                        lwm2m_data_t **dataArrayP,
                        ctlw_lwm2m_object_t *objectP)
{
    uint8_t result;
    int i;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
            RES_M_NETWORK_BEARER,
            RES_M_AVL_NETWORK_BEARER,
            RES_M_RADIO_SIGNAL_STRENGTH,
            RES_O_LINK_QUALITY,
            RES_M_IP_ADDRESSES,
            RES_O_ROUTER_IP_ADDRESS,
            RES_O_LINK_UTILIZATION,
            RES_O_APN,
            RES_O_CELL_ID,
            RES_O_SMNC,
            RES_O_SMCC
        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);

        *dataArrayP = ctlw_lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL)
            return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0; i < nbRes; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }

    i = 0;
    do
    {
        result = prv_set_value((*dataArrayP) + i, (conn_m_data_t *)(objectP->userData));
        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}
						
static uint8_t prv_conn_execute(uint16_t instanceId,
          uint16_t resourceId,
          uint8_t * buffer,
          int length,
          ctlw_lwm2m_object_t * objectP)
{
    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    if (length != 0) return COAP_400_BAD_REQUEST;

    switch (resourceId)
    {
	case RES_M_NETWORK_BEARER:
	case RES_M_AVL_NETWORK_BEARER:
	case RES_M_RADIO_SIGNAL_STRENGTH:
	case RES_M_IP_ADDRESSES:
	case RES_O_CELL_ID:
		return COAP_405_METHOD_NOT_ALLOWED;
    default:
        return COAP_404_NOT_FOUND;
    }
}
static uint8_t prv_conn_create(uint16_t instanceId,
                                 int numData,
                                 lwm2m_data_t *dataArray,
                                 ctlw_lwm2m_object_t *objectP)
{
    return COAP_405_METHOD_NOT_ALLOWED;
}
static uint8_t prv_conn_delete(uint16_t id,
                                 ctlw_lwm2m_object_t *objectP)
{
    objectP->instanceList = ctlw_lwm2m_list_find(objectP->instanceList, id);
    if (NULL == objectP->instanceList)
        return COAP_404_NOT_FOUND;
    return COAP_401_UNAUTHORIZED;
}
static uint8_t prv_conn_write(uint16_t instanceId,
                                int numData,
                                lwm2m_data_t *dataArray,
                                ctlw_lwm2m_object_t *objectP)
{
	lwm2m_list_t* targetP = ctlw_lwm2m_list_find(objectP->instanceList, instanceId);
	if(NULL == targetP)
		return COAP_404_NOT_FOUND;


	uint8_t result;
	int i = 0;
	
    do
    {
        switch (dataArray[i].id)
        {
	        case RES_M_NETWORK_BEARER:
	        case RES_M_AVL_NETWORK_BEARER:
	        case RES_M_RADIO_SIGNAL_STRENGTH:
	        case RES_M_IP_ADDRESSES:
	        case RES_O_CELL_ID:
	        {
	            result = COAP_405_METHOD_NOT_ALLOWED;
	            break;
	        }
			default:
	            return COAP_404_NOT_FOUND;
        }
        i++;
    } while (i < numData && result == COAP_204_CHANGED);
	
	return result;
}


static uint8_t prv_conn_discover(uint16_t instanceId,
								  int *numDataP,
								  lwm2m_data_t **dataArrayP,
								  ctlw_lwm2m_object_t *objectP)
{
    uint8_t result;
    int i;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    result = COAP_205_CONTENT;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
                RES_M_NETWORK_BEARER,
                RES_M_AVL_NETWORK_BEARER,
                RES_M_RADIO_SIGNAL_STRENGTH,
				RES_M_IP_ADDRESSES,
                RES_O_CELL_ID,
        };
        int nbRes = sizeof(resList)/sizeof(uint16_t);

        *dataArrayP = ctlw_lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0 ; i < nbRes ; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }
    else
    {
        for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
            case RES_M_NETWORK_BEARER:
            case RES_M_AVL_NETWORK_BEARER:
			case RES_M_RADIO_SIGNAL_STRENGTH:
			case RES_M_IP_ADDRESSES:
			case RES_O_CELL_ID:
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

ctlw_lwm2m_object_t *ctlw_get_object_conn_m(void)
{
    /*
     * The ctlw_get_object_conn_m() function create the object itself and return a pointer to the structure that represent it.
     */
    ctlw_lwm2m_object_t *connObj;

    connObj = (ctlw_lwm2m_object_t *)ctlw_lwm2m_malloc(sizeof(ctlw_lwm2m_object_t));

    if (NULL != connObj)
    {
        memset(connObj, 0, sizeof(ctlw_lwm2m_object_t));

        /*
         * It assigns his unique ID
         */
        connObj->objID = LWM2M_CONN_MONITOR_OBJECT_ID;

        /*
         * and its unique instance
         *
         */
        connObj->instanceList = (lwm2m_list_t *)ctlw_lwm2m_malloc(sizeof(lwm2m_list_t));
        if (NULL != connObj->instanceList)
        {
            memset(connObj->instanceList, 0, sizeof(lwm2m_list_t));
        }
        else
        {
            ctlw_lwm2m_free(connObj);
            return NULL;
        }

        /*
         * And the private function that will access the object.
         * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
         * know the resources of the object, only the server does.
         */
        connObj->readFunc = prv_conn_read;
        connObj->executeFunc = prv_conn_execute;
		connObj->createFunc = prv_conn_create;
		connObj->deleteFunc = prv_conn_delete;
		connObj->discoverFunc = prv_conn_discover;
		connObj->writeFunc = prv_conn_write;

    }
    return connObj;
}

void ctlw_free_object_conn_m(ctlw_lwm2m_object_t *objectP)
{
    //ctlw_lwm2m_free(objectP->userData);
    ctlw_lwm2m_list_free(objectP->instanceList);
    ctlw_lwm2m_free(objectP);
}

