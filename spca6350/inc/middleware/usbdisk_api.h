/**************************************************************************
 *
 *       Copyright (c) 2012-2016 by iCatch Technology, Inc.
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
 *  Author:                                                               *
 *                                                                        *
 **************************************************************************/
#ifndef _USBDISK_API_H_
#define _USBDISK_API_H_

/*#include "general.h"*/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define DISK_MAX_USB_LUN (4)

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/**
 1: Prevent Circular Linkage ("bottom of middleware implementation" calling "top of middleware implementation")
 0: Original implementation. Its easily causing broken build for enabling multiboot.
 */
#define PREVENT_CIRCULAR_LINKAGE (1)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct msdcCtrl_s {
	void             *priv;          /* Private data */
	volatile SINT32  result;         /* Last operation result */
	SINT32           init;           /* Initialized? */
	volatile SINT32  busy;           /* Busy? */
} msdcCtrl_t;

/* ---------------------------------------------------------------------------
 Private data which describes this channel
  ----------------------------------------------------------------------------*/
typedef struct msdcChan_s {
	struct msdcFunc_s *funs;
	void             *dev_priv;        /* device private data */
	msdcCtrl_t       *controller;      /* pointer to controller */
	SINT32           mbr_support;      /* true if disk has MBR */
	SINT32           valid;            /* true if device valid  */
	SINT32           init;             /* true if initialized */
	UINT32           dev_type;         /* device type */
	UINT32           mounts;           /* count of number of mounts */
	UINT32           blknum;           /* total block number */
	UINT32           byte_per_sector;  /* default 512 */
} msdcChan_t;

typedef struct msdcFunc_s {

    /* Read block data into buf */
    int (*read)(msdcChan_t     *priv,
             	 void           *buf,
                UINT32         en,
                UINT32         block_num);

    /* Write block data from buf */
    int (*write)(msdcChan_t   *priv,
                 const void   *buf,
                 UINT32       len,
                 UINT32       block_num);

    /* Set hardware configuration */
    int (*inquiry)(msdcChan_t   *priv,
                   void         *buf);
} msdcFunc_t;


extern msdcChan_t msdcChan0;
extern msdcChan_t msdcChan1;
extern msdcChan_t msdcChan2;
extern msdcChan_t msdcChan3;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 msdcInit(void);
UINT32 msdcWarmDetect(UINT32);
UINT32 msdcShowErr(void *);

UINT32 msdcFormat(void *,UINT32);
UINT32 msdcRead(void *,UINT32,UINT32,UINT8 *);
UINT32 msdcWrite(void *,UINT32,UINT32,UINT8 *);
UINT32 msdcIsIdle(void);
UINT32 msdcIsBusy(void);
UINT32 msdcSetDeviceInfo(msdcChan_t *,UINT32);

UINT32 msdcFlush(void *);
UINT32 msdcFormat(void *,UINT32 );
UINT32 msdcMount(UINT32 lun);
UINT32 msdcRemove(UINT32 lun);
void * msdcGetPdev(SINT32 msdcNum);

#if PREVENT_CIRCULAR_LINKAGE
/**
 @brief Install (READ) callback function for libdisk to prevent circular linkage
 @callergraph middleware\libusbhost\usbhost_api.c::usbhostInit(UINT32 mode, UINT32 memSize)
 @callergraph middleware\libusbhost\usbhost_api.c::usbhostExit(void)
 */
void usbDiskReadInstall(SINT32 (*fp)(msdcChan_t *chn, void *buf, UINT32 len, UINT32 stblk));

/**
 @brief Install (WRITE) callback function for libdisk to prevent circular linkage
 @callergraph middleware\libusbhost\usbhost_api.c::usbhostInit(UINT32 mode, UINT32 memSize)
 @callergraph middleware\libusbhost\usbhost_api.c::usbhostExit(void)
 */
void usbDiskWriteInstall(SINT32 (*fp)(msdcChan_t *chn, const void *buf, UINT32 len, UINT32 stblk));

/**
 @brief Install (INQUIRY) callback function for libdisk to prevent circular linkage
 @callergraph middleware\libusbhost\usbhost_api.c::usbhostInit(UINT32 mode, UINT32 memSize)
 @callergraph middleware\libusbhost\usbhost_api.c::usbhostExit(void)
 */
void usbDiskInquiryInstall(SINT32 (*fp)(msdcChan_t *chn, void *buf));
#endif


#endif /* _USBDISK_API_H_ */


