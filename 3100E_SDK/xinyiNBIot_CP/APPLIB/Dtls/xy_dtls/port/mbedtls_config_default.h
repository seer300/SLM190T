/*
 *  Minimal configuration for TLS 1.2 with PSK and AES-CCM ciphersuites
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 * Minimal configuration for TLS 1.2 with PSK and AES-CCM ciphersuites
 * Distinguishing features:
 * - no bignum, no PK, no X509
 * - fully modern and secure (provided the pre-shared keys have high entropy)
 * - very low record overhead with CCM-8
 * - optimized for low RAM usage
 */
#ifndef MBEDTLS_CONFIG_ALT_H
#define MBEDTLS_CONFIG_ALT_H

#include "mem_adapt.h"
#include "mbedtls_init.h"

/* System support */
#define MBEDTLS_HAVE_TIME /* Optionally used in Hello messages */
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_PLATFORM_TIME_ALT
#define MBEDTLS_TIMING_ALT

#define MBEDTLS_PLATFORM_C 
#define MBEDTLS_PLATFORM_PRINTF_ALT      
#define MBEDTLS_PLATFORM_SNPRINTF_ALT

/* mbed TLS feature support */
/*CTR_DRBG*/
//#define MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_ENTROPY_C
//#define MBEDTLS_ENTROPY_FORCE_SHA256

/*
 * You should adjust this to the exact number of sources you're using: default
 * is the "platform_entropy_poll" source, but you may want to add other ones
 * Minimum is 2 for the entropy test suite.
 */
#define MBEDTLS_ENTROPY_MAX_SOURCES 1

//#define MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
//#define MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
//#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
//#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED

#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY

/* mbed TLS modules */
#define MBEDTLS_AES_C
#define MBEDTLS_CCM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_MD_C
//#define MBEDTLS_NET_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA224_C
#define MBEDTLS_SSL_CLI_C
//#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TLS_C

/* Error messages and TLS debugging traces
 * (huge code size increase, needed for tests/ssl-opt.sh) */
//#define MBEDTLS_DEBUG_C
//#define MBEDTLS_ERROR_C

#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_CALLOC_MACRO(n,size)        XY_ZALLOC((n)*(size)) /**< Default allocator macro to use, can be undefined */
#define MBEDTLS_PLATFORM_FREE_MACRO(ptr)             dtls_free(ptr)   /**< Default free macro to use, can be undefined */

/* Save RAM at the expense of ROM */
#define MBEDTLS_AES_ROM_TABLES

//#define MBEDTLS_AES_FEWER_TABLES

/* Save some RAM by adjusting to your exact needs */
#define MBEDTLS_PSK_MAX_LEN    32 /* 128-bits keys are generally enough */

/*
 * Save RAM at the expense of interoperability: do this only if you control
 * both ends of the connection!  (See comments in "mbedtls/ssl.h".)
 * The optimal size here depends on the typical size of records.
 */
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH         
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY
#define MBEDTLS_SSL_ALL_ALERT_MESSAGES
#define MBEDTLS_SSL_RENEGOTIATION
#define MBEDTLS_SSL_CACHE_C
#define MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN

/*
 * The minimum size here depends on the certificate chain used as well as the
 * typical size of records.
 */
#define MBEDTLS_SSL_IN_CONTENT_LEN             (4*1024)
#define MBEDTLS_SSL_OUT_CONTENT_LEN            (4*1024)
#define MBEDTLS_SSL_DTLS_MAX_BUFFERING         (4*1024)

#define MBEDTLS_X509_MAX_INTERMEDIATE_CA   5   /**< Maximum number of intermediate CAs in a verification chain. */
#define MBEDTLS_X509_MAX_FILE_PATH_LEN     256 /**< Maximum length of a path/filename string in bytes including the null terminator character ('\0'). */

#define MBEDTLS_MPI_WINDOW_SIZE            1 /**< Maximum window size used. */

/*
 * save ROM and a few bytes of RAM by specifying our own ciphersuite list
 */
#if 0 // We should support two encryption algorithm
#define MBEDTLS_CCM_C
#define MBEDTLS_SSL_CIPHERSUITES                        \
        MBEDTLS_TLS_PSK_WITH_AES_128_CCM_8
#define MBEDTLS_WITH_ONLY_AEAD_CHIPERS
#define MBEDTLS_ONLY_CCM_8_CHIPERS
#else
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_SSL_CIPHERSUITES                        \
        MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA256,MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256//MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256,MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256
#endif

#define MBEDTLS_RSA_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_OID_C
#define MBEDTLS_PKCS1_V21
#define MBEDTLS_ASN1_PARSE_C

#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_PEM_PARSE_C

//#define MBEDTLS_ECDH_C
//#define MBEDTLS_ECDSA_C
//#define MBEDTLS_ECP_C
//#define MBEDTLS_GCM_C
#define MBEDTLS_RSA_C
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_SHA1_C
//#define MBEDTLS_SHA512_C

//#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
//#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
//#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
//#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
//#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
//#define MBEDTLS_ECP_DP_SECP192K1_ENABLED
//#define MBEDTLS_ECP_DP_SECP224K1_ENABLED
//#define MBEDTLS_ECP_DP_SECP256K1_ENABLED
//#define MBEDTLS_ECP_DP_BP256R1_ENABLED
//#define MBEDTLS_ECP_DP_BP384R1_ENABLED
//#define MBEDTLS_ECP_DP_BP512R1_ENABLED
//#define MBEDTLS_ASN1_WRITE_C

#include "../../../Dtls/mbedtls-3.2.1/include/mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_ALT_H */
