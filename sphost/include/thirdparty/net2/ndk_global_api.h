/**************************************************************************
 *
 *       Copyright (c) 2012 by iCatch Technology, Inc.
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

#ifndef __NDK_GLOBAL_API_H__
#define __NDK_GLOBAL_API_H__

#include <ndk_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************/
/*  Global Initialize                                                              */
/***********************************************************************************/
enum {
	NDK_INIT_OPT_PRIORITY,        /* type: long. default: 20 */
	NDK_INIT_OPT_MEMPOOL_SIZE,    /* type: long. 0: use default value. */
};

typedef struct {
	long name;  /* Option name */
	long value; /* Option value */
} NDKInitOpt;

/* \brief Get NDK release version.
 */
const char* ndk_get_version();

/*
 * \brief NDK system global init.
 * \param thread_priority Default thread priority for NDK threads.
 * \param high_speed_sdio 1: Enable high speed sdio; 0: disable.
 */
int ndk_global_init(int num, NDKInitOpt opts[]);

/***********************************************************************************/
/*  System Events                                                                  */
/***********************************************************************************/
typedef enum {
	/* \brief In HostAP mode, station is associated.
	   \param UINT8 macaddr[6]
	*/
	NDK_SYSEVT_HAPD_ASSOC = 0,

	/* \brief In HostAP mode, station is disassociated.
	   \param UINT8 macaddr[6]
	 */
	NDK_SYSEVT_HAPD_DISASSOC,

	/* \brief In station mode, connect to AP successfully.
	   \param const char* : network interface name.
	*/
	NDK_SYSEVT_STA_CONNECTED,

	/* \brief In station mode, disassociated with ap.
	   \param None
	 */
	NDK_SYSEVT_STA_DISCONNECTED,

	/* \brief AP scanning is done.
	   \param None
	 */
	NDK_SYSEVT_STA_SCAN_DONE,

	/* \brief In HostAP mode, an IP-Address is allocated to station.
	   \param const char* : network interface name.
	 */
	NDK_SYSEVT_DHCPD_ALLOC_ADDR,

	/* \brief In station mode, ip address is bound to the interface.
	   \param const char* : network interface name.
	 */
	NDK_SYSEVT_DHCP_BOUND,

	/* \brief Downloading file is start or stop.
	   What is downloading: Send file to client.
	   \param unsigned long. 1: downloading is started; 0: downloading is stopped.
	 */
	NDK_SYSEVT_DOWNLOAD_FILE,

	/* \brief Uploading file is start or stop.
	   What is uploading: Receive file from client.
	   \param unsigned long. 1: uploading is started; 0: uploading is stopped.
	 */
	NDK_SYSEVT_UPLOAD_FILE,

	NDK_SYSEVT_NR
} NDKSysEvt;

typedef void (*NDKSysEvtHandler)(NDKSysEvt evt, unsigned long param, unsigned long udata);
void ndk_sysevt_handler_set(NDKSysEvtHandler handler, unsigned long udata);

/***********************************************************************************/
/*  Network Interface IOCTLs                                                       */
/***********************************************************************************/
enum {
	/*******
	 * SET *
	 *******/
	__NDK_IOCS_IF_START = 0x1000-1,
	/* \brief Set network interface up. */
	NDK_IOCS_IF_UP,                 /* arg: netif name, like "wlan0"; value: not used */
	/* \brief Set network interface down. */
	NDK_IOCS_IF_DOWN,               /* arg: netif name; value: not used */
	/* \breif Start/Stop dump network packets to file in PCAP format. */
	NDK_IOCS_IF_ARP_REQUEST,
	NDK_IOCS_IF_ARP_QUERY,
	NDK_IOCS_IF_PCAP_START,         /* arg: netif name; value: long, capture size. */
	NDK_IOCS_IF_PCAP_STOP,          /* arg: netif name; value: not used. */
	NDK_IOCS_IF_FLAGS,              /* arg: netif name; value: short*, new flags. Ref. SIOCSIFFLAGS*/
	NDK_IOCS_IF_DEFAULT,            /* arg: netif name; Set default network interface. */
	NDK_IOCS_IF_HWADDR,             /* arg: netif name; value: UINT8[IF_HWADDRSIZE] */

	/*******
	 * GET *
	 *******/
	__NDK_IOCG_IF_START = 0x1100-1,
	/* Get the name of the network interface corresponding to the interface index.
	   Index start from 1. Reference: if_indextoname. */
	NDK_IOCG_IF_INDEXTONAME,        /* arg: int index, char ifname[IF_NAMESIZE] */
	/* Get the index of the network interface corresponding to the name.
	   index 0 means error. Reference: if_nametoindex */
	NDK_IOCG_IF_NAMETOINDEX,        /* arg: const char *ifname, int *index */

	/* \brief Get the hw-address of the interface. To convert mac-address to the
	   string format (00:11:22:33:44:55 ), use the eth_addr_ntoa function. */
	NDK_IOCG_IF_HWADDR,             /* arg: netif name; value: UINT8[IF_HWADDRSIZE] */
	/* \brief Get the IPv4-address of the interface */
	NDK_IOCG_IF_ADDR,               /* arg: netif name; value: struct in_addr*. IP Address. */
	NDK_IOCG_IF_FLAGS,              /* arg: netif name; value: short*, flags. Ref. SIOCGIFFLAGS. */
	NDK_IOCG_IF_CONF,               /* arg: not used; value: NDKIfConf*. Ref. SIOCGIFCONF. */
	NDK_IOCG_IF_NETMASK,            /* arg: netif name; value: struct in_addr*. */
	NDK_IOCG_IF_DEFAULT,            /* arg: None; value: char[IFNAMSIZ]. Get default net-interface */
	NDK_IOCG_IF_GATEWAY,            /* arg: netif name; value: struct in_addr*. IP Address. */
	NDK_IOCG_IF_CARRIER,            /* arg: netif name; value: long*. 1: network interface carrier is on; 0: off. */

	__NDK_IOCG_WIFI_START = 0x1800-1,
	/* The following will call the private ioctl function of the network device. */
	NDK_IOCG_IF_LOADING,            /* arg: netif name; value: 0~100 */
	NDK_IOCG_WIFI_STA_NR,           /* arg: netif name; value: number. */
	/* \brief Auto channel selection of 2.4G or 5G */
	NDK_IOCG_WIFI_ACS_24G,          /* arg: netif name; value: channel */
	NDK_IOCG_WIFI_ACS_5G,           /* arg: netif name; value: channel */

	/* \brief Get the best channel list.
	   arg: netif name; value: NDKWifiChanArray24G.
	   The best channel is stored in channels[0], second best is in channels[1], ....
	   Channel number: 1 ~ 11. */
	NDK_IOCG_WIFI_BEST_CHAN_24G,

	/* \brief Get the WiFi RSSI value. Works in station mode.
	   arg: netif name;
	   value: long rssi[2].
	     rssi[0] is signal quality in percentage (0% ~ 100%).
	     rssi[1] is signal stregth in dbm.
	 */
	NDK_IOCG_WIFI_RSSI,

	/* \brief Get the RSSI for station. Works in AP mode.
	   arg: netif name;
	   value: NDKStaRssi*
	 */
	NDK_IOCG_WIFI_STA_RSSI,

	/* NDK IOCTL Range. */
	NDK_IOC_MIN = 0x1000,
	NDK_IOC_MAX = 0x1FFF
};

struct in_addr;

typedef struct {
	int num; /* Actual channel number. */
	int channels[13];
} NDKWifiChanArray24G;

typedef struct {
	char name[16];
	struct {
		UINT16 sa_family;
		char   a_data[14];
	} addr; /* IP address, struct sockaddr */
} NDKIfConfIfInfo;

typedef struct {
	int num; /* Number of NDKIfConfIfInfo */
	NDKIfConfIfInfo *ifinfos;
} NDKIfConf;

typedef struct {
	unsigned char macaddr[6];
	/* rssi[0] is signal quality in percentage (0 ~ 100).
	   rssi[1] is signal stregth in dbm. */
	long rssi[2];
} NDKStaRssi;

/* \return 0: succeed; < 0: failed */
int ndk_netif_ioctl(int code, long arg, long *value);

/* \brief Return network loading. */
int ndk_netif_get_netload(const char *ifname);

/* \brief Return default network interface name. */
const char *ndk_netif_get_default();

/* \brief Return net-interface support station mode or NULL if failed. */
const char *ndk_netif_get_station();

/* \brief Return net-interface support hostap mode or NULL if failed. */
const char *ndk_netif_get_hostap();

/* \brief Print information of all network interfaces including loop-back interface. */
void ndk_netif_printall();

/* \brief Set static IPv4 address for network interface */
int ndk_netif_set_address(const char* ifname
	, struct in_addr* ipaddr
	, struct in_addr* netmask
	, struct in_addr* gateway);

int ndk_netif_set_address_s(const char* ifname
	, const char* ipaddr
	, const char* netmask
	, const char* gateway);

/***********************************************************************************/
/*  Network Device                                                                 */
/***********************************************************************************/
int ndk_netdev_priv_iocmd(const char *cmd, const char *ifname, long *value);
/* Load net-device driver by name */
int ndk_netdev_load_driver(const char *name, const char *options);
/* Get wireless-lan driver name by index */
const char *ndk_netdev_get_wlan_driver_name(int index);
int ndk_netdev_unload_driver(const char *name);
int ndk_netdev_set_driver_mp(const char *name, int enable);

#ifdef __cplusplus
}
#endif

#endif

