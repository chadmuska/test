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

#ifndef __NDK_HOSTAP_H__
#define __NDK_HOSTAP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* \brief Host AP option name and value pair which can replace the one defined in configure file.
 */
typedef struct
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
int ndk_hapd_start(const char* cfg_file, NDKHapdOpt *opts);

typedef struct {
	const char *cfg_file;
	NDKHapdOpt *cfg_opts;
} NDKHapdCfg;

/*
 * Support multiple interfaces..
 */
int ndk_hapd_start2(int num, NDKHapdCfg *cfgs);

int hostapd_cli_main(int argc, char *argv[]);

/* \brief Stop hostapd daemon program.
 */
void ndk_hapd_stop();

BOOL ndk_hapd_is_stopped();

void ndk_hapd_reduce_debug_level();

int ndk_hapd_global_init(const char *options);

#ifdef __cplusplus
}
#endif

#endif
