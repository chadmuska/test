/**************************************************************************
 *
 *       Copyright (c) 2013 by iCatch Technology, Inc.
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

#ifndef __NDK_INIT_API_H__
#define __NDK_INIT_API_H__

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************/
/*  Types                   .                                                      */
/***********************************************************************************/
/* Global Initialize Options */
enum {
	NDK_INIT_OPT_PRIORITY,        /* type: long. default: 20 */
	NDK_INIT_OPT_HIGH_SPEED_SDIO, /* type: long. 0: disable (default); 1: enable */
	NDK_INIT_OPT_MEMPOOL_SIZE,    /* type: long. 0: use default value. */
};

typedef struct {
	long name;  /* Option name */
	long value; /* Option value */
} NDKInitOpt;

/***********************************************************************************/
/*  Initialization Functions.                                                      */
/***********************************************************************************/

/* \brief Get NDK release version.
 */
extern const char* _ndk_get_version();

/* \brief Initialize network driver.
 * \param drvname: name of driver
 * \return 0: succeed. < 0: failed.
 */
extern int _ndk_netdrv_init(const char *drvname);

/* Un-initialize network driver
 */
extern void _ndk_netdrv_exit();

extern void _ndk_netdrv_mpset(int enable);

/*
 * \brief NDK system global init.
 * \param thread_priority Default thread priority for NDK threads.
 * \param high_speed_sdio 1: Enable high speed sdio; 0: disable.
 */
extern void _ndk_global_init(int thread_priority, int high_speed_sdio);
extern void _ndk_global_init_ex(int num, NDKInitOpt opts[]);

extern void _ndk_global_deinit();

/* \brief The default thread priority level used by NDK to create threads.
 * Custom can reference this priority level to create new threads.
 */
extern int _ndk_thread_get_priority();

/***********************************************************************************/
/* NDK API Definitions. Don't call above functions directly                        */
/***********************************************************************************/
#define ndk_get_version()               _ndk_get_version()
#define ndk_netdrv_init(a)              _ndk_netdrv_init(a)
#define ndk_netdrv_exit()               _ndk_netdrv_exit()
#define ndk_netdrv_mpset(a)             _ndk_netdrv_mpset(a)
#define ndk_global_init(a)              _ndk_global_init(a, 0)
#define ndk_global_init_ex(a, b)        _ndk_global_init_ex(a, b)
#define ndk_global_deinit()             _ndk_global_deinit();
#define ndk_thread_get_priority()       _ndk_thread_get_priority()

#ifdef __cplusplus
}
#endif

#endif

