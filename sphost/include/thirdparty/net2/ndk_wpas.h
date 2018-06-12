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

#ifndef __NDK_WPA_SUPPLICANT_H__
#define __NDK_WPA_SUPPLICANT_H__

#ifdef __cplusplus
extern "C" {
#endif

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

int ndk_wpas_global_init(const char *options);

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
int ndk_wpas_start_daemon(int argc, char **argv);

/* \brief Stop wpa_supplicant daemon program.
 */
void ndk_wpas_stop_daemon();

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

		int res = ndk_wpas_add_network(ssid, opts);
 * Note:
 *  If an option name has a prefix '*', it's value will be added with double quote at the head and tail.
 *  The reason is if you provide a password '12345678', the WPA Supplicant daemon program
 *  treate it as a Hex Number String. A password in character format must be double quoted.
 * \return 0: succeed; <0: failed.
 */
int ndk_wpas_add_network(const char *ssid, NDKWpasOpt *opts/* End with {NULL, NULL}*/);

/* \brief Remove a network.
 * \param ssid: SSID name.
 * \return 0: succeed; <0: failed.
 */
int ndk_wpas_remove_network(const char *ssid);

/* \brief Enable or disable a network.
 * \param ssid: SSID name.
 * \param enable: TRUE, enable; FALSE, disable.
 * \return 0: succeed; <0: failed.
 */
int ndk_wpas_enable_network(const char *ssid, BOOL enable);

/* \brief Test if a network exist or not.
 * \param ssid: SSID name.
 * \param enable: TRUE, found; FALSE, not found.
 * \return 0: succeed; <0: failed.
 * \note the found is meaningful only when the function returns 0.
 */
int ndk_wpas_find_network(const char *ssid, BOOL *found);

int ndk_wpas_get_bss_list(NDKWpasBSSInfo **infos, int *num);

/* \brief Save network configures in file.
 * \note The wpa_supplicant daemon must be started with -c (not -C) argument.
 * \return 0: succeed; <0: failed.
 */
int ndk_wpas_save_config();

#ifdef __cplusplus
}
#endif

#endif
