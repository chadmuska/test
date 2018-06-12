/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __LWIP_ERR_H__
#define __LWIP_ERR_H__

#include "lwip/opt.h"
#include "lwip/arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Define LWIP_ERR_T in cc.h if you want to use
 *  a different type for your platform (must be signed). */
#ifdef LWIP_ERR_T
typedef LWIP_ERR_T err_t;
#else /* LWIP_ERR_T */
typedef s8_t err_t;
#endif /* LWIP_ERR_T*/

/* Definitions for error constants. */
#if 0
#define LWIP_ERR_CODE(e)	({ printf("{lwip_has_err @ %s:%d, code %d}\n",__func__,__LINE__,e); e; })
#define ERR_IS_FATAL(e)		((e) < -9)
#else
#define LWIP_ERR_CODE(e)	(e)
#endif

#define ERR_OK          0    /* No error, everything OK. */
#define ERR_MEM         LWIP_ERR_CODE(-1)    /* Out of memory error.     */
#define ERR_BUF         LWIP_ERR_CODE(-2)    /* Buffer error.            */
#define ERR_TIMEOUT     LWIP_ERR_CODE(-3)    /* Timeout.                 */
#define ERR_RTE         LWIP_ERR_CODE(-4)    /* Routing problem.         */
#define ERR_INPROGRESS  LWIP_ERR_CODE(-5)    /* Operation in progress    */
#define ERR_VAL         LWIP_ERR_CODE(-6)    /* Illegal value.           */
#define ERR_WOULDBLOCK  LWIP_ERR_CODE(-7)    /* Operation would block.   */
#define ERR_USE         LWIP_ERR_CODE(-8)    /* Address in use.          */
#define ERR_ISCONN      LWIP_ERR_CODE(-9)    /* Already connected.       */

#ifndef ERR_IS_FATAL
#define ERR_IS_FATAL(e) ((e) < ERR_ISCONN)
#endif

#define ERR_ABRT        LWIP_ERR_CODE(-10)   /* Connection aborted.      */
#define ERR_RST         LWIP_ERR_CODE(-11)   /* Connection reset.        */
#define ERR_CLSD        LWIP_ERR_CODE(-12)   /* Connection closed.       */
#define ERR_CONN        LWIP_ERR_CODE(-13)   /* Not connected.           */

#define ERR_ARG         LWIP_ERR_CODE(-14)   /* Illegal argument.        */

#define ERR_IF          LWIP_ERR_CODE(-15)   /* Low-level netif error    */

#ifdef LWIP_DEBUG
extern const char *lwip_strerr(err_t err);
#else
#define lwip_strerr(x) ""
#endif /* LWIP_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_ERR_H__ */
