/**
 * Copyright (c) 2017 China Mobile IOT.
 * All rights reserved.
 * Reference:
 *  tinydtls - https://sourceforge.net/projects/tinydtls/?source=navbar
**/

#ifndef NBIOT_SOURCE_DTLS_PRNG_H_
#define NBIOT_SOURCE_DTLS_PRNG_H_
#include "cis_if_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Fills \p buf with \p len random bytes. This is the default
 * implementation for prng().  You might want to change prng() to use
 * a better PRNG on your specific platform.
*/
static inline int dtls_prng( unsigned char *buf,
                             size_t         len )
{
    while ( len-- )
        *buf++ = cissys_rand() & 0xFF;
    return 1;
}

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* NBIOT_SOURCE_DTLS_PRNG_H_ */