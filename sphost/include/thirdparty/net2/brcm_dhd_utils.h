/**************************************************************************
 *
 *       Copyright (c) 2009-2017 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
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
 *  Hsin-Chu, Taiwan.
 *                                                                        *
 **************************************************************************/

#ifndef __BRCM_DHD_UTILS_H__
#define __BRCM_DHD_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

int brcm_dhd_ioctl(
	const char *ifname,
	const char *cmdstr);

/* Use sp5kFree to delete the pkt returned. */
int brcm_dhd_generate_udp_keepalive_packet(
	const char *ifname,		/* interface to send packet */
	UINT16 port,			/* local UDP port */
	struct sockaddr_in *rem_addr,	/* remote IP address */
	UINT8  *data,			/* data to send */
	UINT32 data_len,		/* data length */
	UINT32 timeout,			/* milli-seconds */
	UINT8  **pkt,			/* Return keep-alive pakcet with Ethernet, IP and UDP headers. */
	UINT32 *pkt_len);		/* packet length */

int brcm_dhd_set_keepalive_packet(
	const char *ifname,
	UINT32 id,
	UINT32 period,
	UINT8  *pkt,
	UINT32 len);

// Filter string: <offset> 0xFFFFFFFFFFFF 0xFFFFFFFFFFFF
// Convert to : <id> 0 0 <offset> 0xFFFFFFFFFFFF 0xFFFFFFFFFFFF
// Thread unsafe.
// Return filterid which is used in brcm_dhd_del_rx_filter
int brcm_dhd_add_rx_filter_string(
	const char *ifname,
	const char *filter);

int brcm_dhd_add_udp_rx_filter(
	const char *ifname,
	UINT8  *data,
	UINT8  *mask,
	UINT32 len);

void brcm_dhd_del_rx_filter(
	const char *ifname,
	int id);

#ifdef __cplusplus
}
#endif

#endif
