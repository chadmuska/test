/**************************************************************************
 *
 *       Copyright (c) 2015 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/

#ifndef __TCPIP_NET_DEVICE_H__
#define __TCPIP_NET_DEVICE_H__

#include <linux/if.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Constants
 *****************************************************************************/

/******************************************************************************
 * Types
 *****************************************************************************/
struct net_device;
struct sk_buff;

/******************************************************************************
 * Network interface statistics
 *****************************************************************************/
extern UINT32 netif_sta_tx_cnt;
extern UINT32 netif_sta_rx_cnt;
extern UINT32 netif_sta_tx_copy_cnt;
extern UINT32 netif_sta_tx_bytes;
extern UINT32 netif_sta_rx_bytes;

#ifdef __cplusplus
}
#endif

#endif

