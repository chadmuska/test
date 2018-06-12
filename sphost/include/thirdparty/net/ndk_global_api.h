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

#define NDK_GLOBAL_REV  "$Rev: 19112 $"

#include <ndk_types.h>
#include <ndk_tcpip.h>
#include <net_version.h>
#include <ndk_init_api.h>

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************/
/*  NDK API Functions                                                              */
/***********************************************************************************/

/* \brief Print information of all network interfaces including loop-back interface.
 */
extern void _ndk_netif_printall();

/* \brief Set static IPv4 address for network interface
 */
extern int _ndk_netif_set_address(const char* ifname
	, struct in_addr* ipaddr
	, struct in_addr* netmask
	, struct in_addr* gateway);

extern int _ndk_netif_set_address_s(const char* ifname
	, const char* ipaddr
	, const char* netmask
	, const char* gateway);

/* \brief net-interface IOCTLs */
enum {
	/* SET */
	NDK_IOCS_FIRST = 0x0000,
	/* \brief Set network interface up. */
	NDK_IOCS_IF_UP = NDK_IOCS_FIRST, /* arg: netif name, like "wlan0"; value: not used */
	/* \brief Set network interface down. */
	NDK_IOCS_IF_DOWN,               /* arg: netif name; value: not used */
	/* \breif Start/Stop dump network packets to file in PCAP format. */
	NDK_IOCS_IF_PCAP_START,         /* arg: netif name; value: long, capture size. */
	NDK_IOCS_IF_PCAP_STOP,          /* arg: netif name; value: not used. */
	NDK_IOCS_IF_FLAGS,              /* arg: netif name; value: short*, new flags. Ref. SIOCSIFFLAGS*/
	NDK_IOCS_IF_DEFAULT,            /* arg: netif name; */
	/* ...... */

	NDK_IOCS_WIFI_FIRST = 0x0100-1, /* Change the start number */
	NDK_IOCS_WIFI_SUSPEND,          /* arg: netif name; value: not used. */
	NDK_IOCS_WIFI_RESUME,           /* arg: netif name; value: not used. */
	NDK_IOCS_WIFI_COUNTRY_US,       /* arg: netif name; value: not used. Channel 1 - 11 */
	NDK_IOCS_WIFI_COUNTRY_EU,       /* arg: netif name; value: not used. Channel 1 - 13 */
	NDK_IOCS_WIFI_COUNTRY_JP,       /* arg: netif name; value: not used. Channel 1 - 14 */
	NDK_IOCS_WIFI_CCK_PWR,          /* arg: netif name; value: long*, cck power (rtk: 0 ~ 3F) */
	NDK_IOCS_WIFI_STAINFO_SETMAC,   /* arg: netif name value: mac address */
	NDK_IOCS_WIFI_TXFIFO_CONFIG,    /* arg: netif name value: '[delay|retry|wait] value' */
	                                /* delay - ms:  sleep n ms after xmit success */
	                                /* retry - count , wait - ms:  if retry over limit then sleep wait for n ms */
	NDK_IOCS_WIFI_QUEUE_CLR,
	NDK_IOCS_WIFI_EFUSE_READ,       /* arg: netif name; value: address pointer that store efuse context */
	NDK_IOCS_WIFI_EFUSE_WRITE,      /* arg: netif name; value: address pointer that update context into efuse */
	/* ...... */

	NDK_IOCS_MISC_FIRST = 0x0200-1,
	NDK_IOCS_MAC_CONSTRAIN,
	NDK_IOCS_DBG_LEVEL,             /* arg: netif name; value: debug message level 0:off 1:always 2:emerg 3:alert 4:crit
                                       5:err, 6:warnng, 7:notice, 8:info, 9:dump, 10:debug */
	NDK_IOCS_ETHER_SETMAC,
	/* ...... */

	/* GET */
	NDK_IOCG_FIRST = 0x1000,
	/* Get the name of the network interface corresponding to the interface index.
	   Index start from 1. Reference: if_indextoname. */
	NDK_IOCG_IF_INDEXTONAME = NDK_IOCG_FIRST, /* arg: int index, char ifname[IF_NAMESIZE] */
	/* Get the index of the network interface corresponding to the name.
	   index 0 means error. Reference: if_nametoindex */
	NDK_IOCG_IF_NAMETOINDEX,        /* arg: const char *ifname, int *index */

	/* \brief Get the hw-address of the interface. To convert mac-address to the
	   string format (00:11:22:33:44:55 ), use the eth_addr_ntoa function. */
	NDK_IOCG_IF_HWADDR,             /* arg: netif name; value: UINT8[HWADDR_NAMESIZE]*/
	/* \brief Get the IPv4-address of the interface */
	NDK_IOCG_IF_ADDR,               /* arg: netif name; value: struct in_addr*. IP Address. */
	NDK_IOCG_IF_LOADING,            /* arg: netif name; value: 0~100 */
	NDK_IOCG_IF_FLAGS,              /* arg: netif name; value: short*, flags. Ref. SIOCGIFFLAGS. */
	NDK_IOCG_IF_CONF,               /* arg: not used; value: NDKIfConf*. Ref. SIOCGIFCONF. */
	NDK_IOCG_IF_NETMASK,            /* arg: netif name; value: struct in_addr*. */
	NDK_IOCG_IF_DEFAULT,            /* arg: None; value: char[IFNAMSIZ]. Get default net-interface */
	NDK_IOCG_IF_GATEWAY,            /* arg: netif name; value: struct in_addr*. IP Address. */
	NDK_IOCG_IF_CARRIER,            /* arg: netif name; value: long*. 1: network interface carrier is on; 0: off. */
	/* ...... */

	NDK_IOCG_WIFI_FIRST = 0x1100-1,
	NDK_IOCG_WIFI_STA_NR,           /* arg: netif name; value: number. */
	NDK_IOCG_WIFI_STAINFO_NORESP,   /* arg: netif name value: no response in x second */

	/* \brief Auto channel selection of 2.4G or 5G */
	NDK_IOCG_WIFI_ACS_24G,          /* arg: netif name; value: channel */
	NDK_IOCG_WIFI_ACS_5G,           /* arg: netif name; value: channel */
	/* \brief Get the best channel list.
	   arg: netif name; value: NDKWifiChanArray24G.
	   The best channel is stored in channels[0], second best is in channels[1], ....
	   Channel number: 1 ~ 11. */
	NDK_IOCG_WIFI_BEST_CHAN_24G,
	NDK_IOCG_WIFI_EFUSE_LEN,       /* arg: netif name; value: efuse size */
	NDK_IOCG_WIFI_RSSI,            /* arg: netif name; value: long*, dbm */
	/* ...... */

	NDK_IOCG_MISC_FIRST = 0x1200-1,
	/* ...... */
};

typedef struct {
	int num; /* Actual channel number. */
	int channels[11];
} NDKWifiChanArray24G;

typedef struct {
	char   name[16];
	struct sockaddr addr; /* IP address */
} NDKIfConfIfInfo;

typedef struct {
	int num; /* Number of NDKIfConfIfInfo */
	NDKIfConfIfInfo *ifinfos;
} NDKIfConf;

/* \return 0: succeed; < 0: failed
 */
extern int _ndk_netif_ioctl(const char* revision, int code, long arg, long *value);
extern int ndk_netif_get_netload(const char *ifname);
/* Return default network interface name. */
extern const char *ndk_netif_get_default();

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
extern void _ndk_sysevt_handler_set(NDKSysEvtHandler handler, unsigned long udata);

/***********************************************************************************/
/*  DHCP client and server functions                                               */
/***********************************************************************************/

/* \brief Startup the DHCP client service
 * \param ifname: interface name, such as: wlan0
 * \return 0: succeed; < 0: failed
 * Notes:
 *   Before call this function, you should call ndk_netif_set_address function
 *   to set the interface's ip-address to zero.
 */
extern int _ndk_dhcp_start(const char *ifname);

/* \brief Stop the DHCP client service
 * \param ifname: interface name, such as: wlan0
 */
extern void _ndk_dhcp_stop(const char *ifname);

/***********************************************************************************/
/*  DHCP Daemon functions                                                          */
/***********************************************************************************/
/* Option ID for _ndk_dhcpd_opts_set */
enum {
	NDK_DHCPD_OPT_GATEWAY = 3, /* Argument: struct in_addr* */
	NDK_DHCPD_OPT_DNS     = 6, /* Argument: struct in_addr* */
};
/*
 * Arguments:
 *   ifname: interface name, such as "wlan0", "eth0"
 *   ipa_start: start address for allocation. The address must be in network byte order.
 *   str_start: start address in Internet standard dot notation.
 *   nr: number of addres for allocating.
 * \return 0: succeed; < 0: failed
 */
extern int  _ndk_dhcpd_start(const char *ifname, struct in_addr* ipa_start, int nr);
extern int  _ndk_dhcpd_start_s(const char* ifname, const char* str_start, int nr);
extern int  _ndk_dhcpd_opt_set(const char* ifname, unsigned optid, int services, ...);
extern int  _ndk_dhcpd_opt_get(char* optInfo, unsigned optid, int services, ...);

extern void _ndk_dhcpd_stop();
extern BOOL _ndk_dhcpd_is_started();
extern BOOL _ndk_dhcpd_find_macaddr(struct in_addr* inaddr, UINT8 *macaddr_ret[6]);
extern BOOL _ndk_dhcpd_find_inaddr(UINT8 *macaddr, struct in_addr *inaddr_ret);
extern void _ndk_dhcpd_printall();
extern int _ndk_dhcpd_ip_alloc_check(u32_t s_addr);

/***********************************************************************************/
/*  WPA Supplicant                                                                 */
/***********************************************************************************/
typedef struct _NDKWpasOpt
{
	const char *name;
	const char *value;
} NDKWpasOpt;

enum {
	NDK_WPAS_KEY_NONE         = 1 << 0,
	NDK_WPAS_KEY_WEP          = 1 << 1,
	NDK_WPAS_KEY_WPA_WPA2_PSK = 1 << 2,
	NDK_WPAS_KEY_EAP          = 1 << 3,
	NDK_WPAS_KEY_UNKNOW       = 1 << 16
};

typedef struct _NDKWpasBSSInfo
{
	unsigned char bssid[6]; /* Mac Address */
	int  freq;  /* Frequency */
	int  level; /* Signal Level */
	unsigned int keymgnt; /* Key Management */
	char ssid[32];
	char _reserved;
} NDKWpasBSSInfo;

/* \brief Start wpa_supplicant daemon program.
 * Arguments:
 *  -ccfg-file  : cfg-file is the path of the configure file.
 *  -Cwpas_conf : if no -c, this must be provided.
 *  -Dwext      : Driver name.
 *  -iifname    : ifname is network interface name, e.g. wlan0, wlan1.
 * Sample Codes:
	char *av[] = { "-Cwpas_conf", "-Dwext", "-iwlan0" };
	res = ndk_wpas_start_daemon(3, av);
 * \return 0: succeed; <0: failed.
 */
extern int _ndk_wpas_start_daemon(int argc, char **argv);

/* \brief Stop wpa_supplicant daemon program.
 */
extern void _ndk_wpas_stop_daemon();

/* \brief Add a network. Please refer the website http://hostap.epitest.fi/wpa_supplicant/ to
 * get the detail of options.
 * Sample Codes:
 *   Connect to an AP with WPA-PSK encryption.
		NDKWpasOpt opts[] = {
			{"proto", "WPA RSN"},
			{"key_mgmt", "WPA-PSK"},
			{"pairwise", "CCMP TKIP"},
			{"group", "CCMP TKIP WEP104 WEP40"},
			{"*psk", "12345678"},
			{NULL, NULL}
		};

		int res = _ndk_wpas_add_network(ssid, opts);
 * Note:
 *  If an option name has a prefix '*', it's value will be added with double quote at the head and tail.
 *  The reason is if you provide a password '12345678', the WPA Supplicant daemon program
 *  treate it as a Hex Number String. A password in character format must be double quoted.
 * \return 0: succeed; <0: failed.
 */
extern int _ndk_wpas_add_network(const char *ssid, NDKWpasOpt *opts/* End with {NULL, NULL}*/);

/* \brief Remove a network.
 * \param ssid: SSID name.
 * \return 0: succeed; <0: failed.
 */
extern int _ndk_wpas_remove_network(const char *ssid);

/* \brief Enable or disable a network.
 * \param ssid: SSID name.
 * \param enable: TRUE, enable; FALSE, disable.
 * \return 0: succeed; <0: failed.
 */
extern int _ndk_wpas_enable_network(const char *ssid, BOOL enable);

/* \brief Test if a network exist or not.
 * \param ssid: SSID name.
 * \param enable: TRUE, found; FALSE, not found.
 * \return 0: succeed; <0: failed.
 * \note the found is meaningful only when the function returns 0.
 */
extern int _ndk_wpas_find_network(const char *ssid, BOOL *found);

extern int _ndk_wpas_get_bss_list(NDKWpasBSSInfo **infos, int *num);

/* \brief Save network configures in file.
 * \note The wpa_supplicant daemon must be started with -c (not -C) argument.
 * \return 0: succeed; <0: failed.
 */
extern int _ndk_wpas_save_config();

/***********************************************************************************/
/*  Host AP                                                                        */
/***********************************************************************************/
/* \brief Host AP option name and value pair which can replace the one defined in configure file.
 */
typedef struct _NDKHapdOpt
{
	const char *name;
	const char *value;
} NDKHapdOpt;

/* \brief Start hostapd daemon program.
 * \param cfg_file: configure file for hostapd.
 * \param opts: Array of option pairs terminated by a NULL pair. JUST REPLACE the SAME option (if it
 *              can be found in configure file) defined in configure file.
 *              If opts is NULL, use all options defined in configure file.
 * Option array example:
	NDKHapdOpt opts[] = {
		{"ssid", "ssid-name"},
		{"wpa_passphrase", "1234567890"},
		{"channel", "auto"}, # auto: enable AutoChannelSelection.
		{"wpa", NULL},  # this option is skiped because opt-value is NULL.
		{NULL, NULL}    # The option name of last pair must be NULL.
	};
 * NOTE: If an option in the option array isn't FOUND in configure file, it will not be used.
 * \return 0: succeed; <0: failed.
 */
extern int _ndk_hapd_start(const char* cfg_file, NDKHapdOpt *opts);

/* \brief Stop hostapd daemon program.
 */
extern void _ndk_hapd_stop();

extern BOOL _ndk_hapd_is_stopped();

/* \brief Do ping test through ICMP echo message
 * Arguments:
 *   host: Host to ping.
 *   timeout: Timeout in milli-second.
 *   triptime - Trip-time in 1/10000 second.
 * \return 0: succeed; < 0: failed
 */
extern int _ndk_ping(struct in_addr *host, long timeout, long *triptime);
extern int _ndk_ping_s(const char *host, long timeout, long *triptime);

/*\brief  Start and stop ftpd service */
extern int  _ndk_ftpd_start(const char* rootdir, int port, const char *opts);
extern void _ndk_ftpd_stop();

/***********************************************************************************/
/* NDK Global API Definitions. Don't call the above functions directly             */
/***********************************************************************************/
#define ndk_netif_printall()                _ndk_netif_printall()
#define ndk_netif_set_address(a,b,c,d)      _ndk_netif_set_address(a,b,c,d)
#define ndk_netif_set_address_s(a,b,c,d)    _ndk_netif_set_address_s(a,b,c,d)
#define ndk_netif_ioctl(a,b,c)              _ndk_netif_ioctl(NDK_GLOBAL_REV, a,b,c)
#define ndk_sysevt_handler_set(a,b)         _ndk_sysevt_handler_set(a,b)
#define ndk_dhcp_start(a)                   _ndk_dhcp_start(a)
#define ndk_dhcp_stop(a)                    _ndk_dhcp_stop(a)
#define ndk_dhcpd_start(a,b,c)              _ndk_dhcpd_start(a,b,c)
#define ndk_dhcpd_start_s(a,b,c)            _ndk_dhcpd_start_s(a,b,c)
#define ndk_dhcpd_stop()                    _ndk_dhcpd_stop()
#define ndk_dhcpd_opt_set(args...)          _ndk_dhcpd_opt_set(args)
#define ndk_dhcpd_opt_get(args...)          _ndk_dhcpd_opt_get(args)
#define ndk_dhcpd_is_started()              _ndk_dhcpd_is_started()
#define ndk_dhcpd_find_macaddr(a,b)         _ndk_dhcpd_find_macaddr(a,b)
#define ndk_dhcpd_find_inaddr(a,b)          _ndk_dhcpd_find_inaddr(a,b)
#define ndk_dhcpd_printall()                _ndk_dhcpd_printall()
#define ndk_dhcpd_ip_alloc_check(a)         _ndk_dhcpd_ip_alloc_check(a)
#define ndk_wpas_start_daemon(a,b)          _ndk_wpas_start_daemon(a,b)
#define ndk_wpas_stop_daemon()              _ndk_wpas_stop_daemon()
#define ndk_wpas_add_network(a, b)          _ndk_wpas_add_network(a, b)
#define ndk_wpas_remove_network(a)          _ndk_wpas_remove_network(a)
#define ndk_wpas_enable_network(a,b)        _ndk_wpas_enable_network(a,b)
#define ndk_wpas_find_network(a,b)          _ndk_wpas_find_network(a,b)
#define ndk_wpas_save_config()              _ndk_wpas_save_config()
#define ndk_wpas_get_bss_list(a,b)          _ndk_wpas_get_bss_list(a,b)
#define ndk_hapd_start(a,b)                 _ndk_hapd_start(a,b)
#define ndk_hapd_stop()                     _ndk_hapd_stop()
#define ndk_hapd_is_stopped()               _ndk_hapd_is_stopped()
#define ndk_ping(a,b,c)                     _ndk_ping(a,b,c)
#define ndk_ping_s(a,b,c)                   _ndk_ping_s(a,b,c)
#define ndk_ftpd_start(a,b)                 _ndk_ftpd_start(a,b,NULL)
#define ndk_ftpd_start_ex(a,b,c)            _ndk_ftpd_start(a,b,c)
#define ndk_ftpd_stop()                     _ndk_ftpd_stop()

/* Fix Host Compiling errors. Will be removed in the future. */
#define ndk_wpas_start()
#define ndk_wpac_begin()
#define ndk_wpac_request(a)
#define ndk_wpac_end()

#ifdef __cplusplus
}
#endif

#endif

