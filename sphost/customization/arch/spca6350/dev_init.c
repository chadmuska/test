/**************************************************************************
 *
 *       Copyright (c) 2004-2018 by iCatch Technology, Inc.
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
 *
 *  Author: Matt Wang
 *
 **************************************************************************/
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "middleware/sensor_def.h"
#include "middleware/proflog.h"
#include "api/sp5k_disp_api.h"
#include "api/sp5k_disk_api.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_usb_api.h"
#include "api/sp5k_msg_def.h"
#include "api/sp5k_rsvblk_api.h"
#include "api/sysboot_api.h"
#include "api/multiboot_api.h"
#include "api/sp5k_modesw_api.h"
#include "api/sp5k_usbhost_api.h"
#include "api/sp5k_os_api.h" /*paul 2018.04.13 add to fixed compiler warning */

#include "customization/dev_init.h"
#include "customization/app_init.h" /*for fastboot hostInit()*/
#include "oss_usbhost.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "../../hostfw/include/app_infohandlerComm.h"
extern UINT32 powerInit(void);
#endif
#include GPIO_INC

/**************************************************************************
 *                             C O N S T A N T S
 **************************************************************************/
/* User has to make sure the GPIO setting is correct */

/**************************************************************************
 *                     E X T E R N A L   R E F E R E N C E
 **************************************************************************/
UINT32 diskDevAlloc(UINT32 devMax);
UINT32 diskNandOpen(void);
UINT32 diskSpiOpen(void);
UINT32 diskSdOpen(void);

extern void devRtcInit(void);
extern void codeSentry(UINT32 opt);
extern void pcHistoryInit(void);
extern void pcHistoryEn(UINT32 opt, UINT32 stoprule, UINT32 stepsize);
extern UINT32 hostInit( void );
extern UINT32 vfsDevIoCtrl(UINT32 devId, UINT32 operation, UINT32 ioCtrl);
extern UINT32 sdRwDly(UINT32 PatchDoFlg, UINT32 rDly, UINT32 wDly);
extern void appDiskIoCfg(iopin_t *io);
extern void appUsbIoCfg(iopin_t *io);
extern void gpioInitialCfgSet(void);
extern void diskScan(UINT32 drive);
extern void hostMsgMaxNumSet(UINT32 num);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern ErrTypeUpdate appInfoUpdateErrGet(ErrCodeUpdate * code,UINT8 * count);
extern void appInfoBootErrSet(void);
extern UINT8 appInfoBootErrGet(void);
extern void appInfoBootErrClear(void);
#endif
extern const rsvSize_t OSDRsvSize[];
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
#define APP_VERSION				1   /*VQ*/
#define APP_SUB_VERSION			72	/*VQ*/
#define APP_REVISION			0	/*VQ*/
#endif
#define _QUOTE(x)	#x
#define QUOTE(x)	_QUOTE(x)
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
UINT8 signature[64]		__attribute__ ((section (".signature")))=
	APP_MODEL " ver" QUOTE(APP_MODELNAME) "." QUOTE(APP_VERSION) "." QUOTE(APP_REVISION) "@" HW_MODEL;
#else
UINT8 signature[64]		__attribute__ ((section (".signature")))=
	APP_MODEL " v" QUOTE(APP_VERSION) "." QUOTE(APP_SUB_VERSION) "." QUOTE(APP_REVISION) "@" HW_MODEL;
#endif
UINT32 bUsbPowerOn  __attribute__((section(".signature"))) = 0x0;
/**************************************************************************
 *                 F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
void devDispEnum(void);
void devDiskEnumBoot(void);
void devDiskEnumMedia(void);
void devFrontEnum(void);
void devAudioEnum(void);
void devKeyBtnEnum(void);
void devUsbEnum(void);
void devOtgEnum(void);
/**************************************************************************
 *                           G L O B A L   D A T A
 **************************************************************************/
#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_MEDIA
static sp5kIoScan_t diskIoCfg = {
	.expireMs = 500/*1000*/,
	.msgCmd   = {SP5K_MSG_DISK_REMOVAL, SP5K_MSG_DISK_INSERT},
	.msgParam = {2, 2}, /* DRIVE_SD */
	.pfunc    = {diskScan, NULL},
};
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_MEDIA*/

#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_USB
#if SP5K_USB_VBUS_SCAN
static sp5kIoScan_t usbIoCfg = {
	.expireMs = 1000,
	.msgCmd   = {SP5K_MSG_USB_DEVICE_VBUS_IN, SP5K_MSG_USB_DEVICE_VBUS_OFF},
	.msgParam = {0, 0},
	.pfunc    = {NULL, NULL},
};
#endif
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_USB*/

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define APP_SYS_POR_BY_NOR        0
#define APP_SYS_POR_BY_RTC        1
#define APP_SYS_POR_BY_WDR        2
#define APP_SYS_POR_BY_RESET      3

/**************************************************************************
 *                           G L O B A L   E X P O R T
 **************************************************************************/
UINT32 Global_sdType = 0;

static void devDiskFW2FlagCheck(void)
{
	/*paul 2018.04.13 removed to fixed compiler warning */
	/*UINT32 rest_val;*/ 
#if SP5K_FW2_SUPPORT  	
	UINT32 info;
	UINT8 count = 0;	
	
	ErrTypeUpdate type;
	ErrCodeUpdate Errcode = ERRCODE_UPDATE_SUCCESS;



	sp5kSystemCfgGet(SP5K_SYS_CFG_BOOT_FW_INFO, &info);
	type = appInfoUpdateErrGet(&Errcode , &count);

	
  
	printf("APP_17DVR_FW=%d    FW 0x%x\n",APP_17DVR_FW , info);
 if (info != 0x80000001){
	
	SET_HW_WDT_CLK(0);
	if(type >= ERRTYPE_MAX||Errcode >= ERRCODE_MAX){
		UINT8 *pbuf;
		pbuf = sp5kMalloc(512);
		memset(pbuf, 0, 512);
		sp5kRawSegmentWrite(0, 512,pbuf);
		sp5kFree(pbuf);
		type = ERRTYPE_UPDATE_NOERR;
		Errcode = ERRCODE_UPDATE_SUCCESS;
	}
	
	if(type == ERRTYPE_UPDATE_NOERR && Errcode == ERRCODE_UPDATE_SUCCESS){
		*(volatile UINT8 *)(0xb0004070) = 1;
		appInfoBootErrSet();
		printf("FW2 boot\n");
		SET_HW_WDT_CLK(1);
		sp5kDscFirmwareBoot((UINT8 *)":DEF_FW2", 0xa0000000);
	}	
}
else{

		appInfoBootErrClear();
}

#endif	
}
#endif

/**************************************************************************
 *                                                                        *
 * Function Name: devInit                                                 *
 *                                                                        *
 * Desprictions:                                                          *
 *                                                                        *
 * Arguments:                                                             *
 *                                                                        *
 * Returns:                                                               *
 *                                                                        *
 * See also:                                                              *
 *                                                                        *
 **************************************************************************/

#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_START

void
devInitPrimary(
	void
)
{
	#if 0 /* FIXME: weak mode */
	void halAudVrefsmClear();
	halAudVrefsmClear();
	#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 rest_val;
	UINT8 _mode;
#endif
	profLogPrintf(0, "[host]%s s",__FUNCTION__);

# if MULTI_BOOT_STAGE
#  warning *FIXME* code sentry support for multiboot background loading...
	/*FIXME* code sentry support for multiboot background loading...*/
# else
	codeSentry(2);
# endif

	/* PC history stop by ALL (7) condition, and step size is 1<<(4+2) = 64 B */
	pcHistoryInit();
	pcHistoryEn(1, 7, 4);

    #if SP5K_ULTRA_FAST_BOOT
    profLogAdd(4,"ULTRA_FAST_BOOT:LogCTRL OFF");
    profLogPrintf(0, "[host]LOG DISABLE");
    sp5kLogControl(SP5K_LOG_DISABLE);
    #else
    profLogAdd(4,"ULTRA_FAST_BOOT:LogCTRL ON");
    profLogPrintf(0, "[host]LOG ENABLE");
    sp5kLogControl(SP5K_LOG_ENABLE);

    #endif

# if MULTI_BOOT_STAGE
	multibootCb(MULTI_BOOT_STAGE_DEV_START);
	multibootLoadWait(MULTI_BOOT_STAGE_DEV_RTC);
# endif /*MULTI_BOOT_STAGE*/

# if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_RTC

	profLogPrintf(0, "[host]init: devRtcInit s %d",__LINE__);
	devRtcInit();  /*boot param on RTC*/
	profLogPrintf(0, "[host]init: devRtcInit e %d",__LINE__);

	#if 0 /*spca6350 boot test*/ /*ADC FW not ready*/
	sp5kAdcInit();
	#endif /*spca6350 boot test*/

#  if MULTI_BOOT_STAGE
	multibootCb(MULTI_BOOT_STAGE_DEV_RTC);
#  endif /*MULTI_BOOT_STAGE*/

#  if MULTI_BOOT_STAGE
	multibootLoadWait(MULTI_BOOT_STAGE_DEV_TEST);
#  endif /*MULTI_BOOT_STAGE*/

#  if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_TEST
	profLogPrintf(0, "[host]init: Periphery Init s %d",__LINE__);
    sp5kAdcInit();
    gpioInitialCfgSet();
	profLogPrintf(0, "[host]init: Periphery Init e %d",__LINE__);
#   if MULTI_BOOT_STAGE
	multibootLoadWait(MULTI_BOOT_STAGE_DISK_RSV);
#   endif /*MULTI_BOOT_STAGE*/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	rest_val = powerInit();
/*#define APP_PARKING_MODE 0x11*/
	sp5kRtcExtraGet(&_mode, 0x11, sizeof(_mode));
	printf("%s Gsensor mode 0x%x power on source 0x%x\n",__FUNCTION__ , _mode,rest_val);
	if (rest_val != APP_SYS_POR_BY_RTC || _mode == 0xff)
		sp5kRawSegmentPreCbSet(devDiskFW2FlagCheck); 
#endif
#   if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_RSV
	profLogPrintf(0, "[host]init: devDiskEnumBoot s %d",__LINE__);
	devDiskEnumBoot();
	profLogPrintf(0, "[host]init: devDiskEnumBoot e %d",__LINE__);
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
	#if SP5K_FW2_SUPPORT
	UINT8 *pbuf;
	pbuf = sp5kMalloc(512);
	memset(pbuf, 0, sizeof(pbuf));
	sp5kRawSegmentRead(0, 512, pbuf);
	if(pbuf[0]==0x0 && pbuf[1]==0x1 && pbuf[2]==0x2
		&& pbuf[3]==0x3 && pbuf[4]==0x4 && pbuf[5]==0x5)
	{
		printf("FW2 boot\n");
		sp5kDscFirmwareBoot(":DEF_FW2", 0xa0000000);
	}
	sp5kFree(pbuf);
#endif
	#endif

#if SP5K_SENSOR_POWERIC_ENABLE
	appI2CInit();/* init I2C mutex */
#endif

#    if MULTI_BOOT_STAGE
	multibootCb(MULTI_BOOT_STAGE_DISK_RSV);
#    endif /*MULTI_BOOT_STAGE*/

#    if MULTI_BOOT_STAGE
	multibootLoadWait(MULTI_BOOT_STAGE_DISP);
#    endif /*MULTI_BOOT_STAGE*/

#    if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISP
	profLogPrintf(0, "[host]init: devDispEnum s %d",__LINE__);
	devDispEnum();
	profLogPrintf(0, "[host]init: devDispEnum e %d",__LINE__);

#     if MULTI_BOOT_STAGE
	multibootCb(MULTI_BOOT_STAGE_DISP);
	multibootLoadWait(MULTI_BOOT_STAGE_SENSOR);
#     endif /*MULTI_BOOT_STAGE*/

#     if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SENSOR

	/* front driver enum. */
	/*no need to init snap/pb max buf, moved back from heap_init.c*/
	#if !(POWERON_PB_WITHOUT_SENSOR)
		profLogPrintf(0, "[host]init: devFrontEnum s %d",__LINE__);
		devFrontEnum();
		profLogPrintf(0, "[host]init: devFrontEnum e %d",__LINE__);
	#endif

	profLogPrintf(0, "[host]%s e",__FUNCTION__);
#     endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SENSOR*/
#    endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISP*/
#   endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_RSV*/
#  endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_TEST*/
# endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_RTC*/
}
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_START*/


#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_SECOND
void
devInitSecondary(
	void
)
{
	profLogPrintf(0, "[host]%s s",__FUNCTION__);

	#ifdef HOST_MSG_MAX_SIZE
    hostMsgMaxNumSet(HOST_MSG_MAX_SIZE);
	#endif

    profLogPrintf(0, "[host]init: hostInit s %d",__LINE__);
    hostInit();
    profLogPrintf(0, "[host]init: hostInit e %d",__LINE__);

#if MULTI_BOOT_STAGE
	multibootCb(MULTI_BOOT_STAGE_DEV_SECOND);
	multibootLoadWait(MULTI_BOOT_STAGE_DISK_MEDIA);
#endif /*MULTI_BOOT_STAGE*/

#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_MEDIA
	/*
	 * media disk driver enum.
	 */
	profLogPrintf(0, "[host]init: devDiskEnumMedia s %d",__LINE__);
	devDiskEnumMedia();
	profLogPrintf(0, "[host]init: devDiskEnumMedia e %d",__LINE__);

# if MULTI_BOOT_STAGE
	multibootCb(MULTI_BOOT_STAGE_DISK_MEDIA);
	multibootLoadWait(MULTI_BOOT_STAGE_DEV_AUDIO);
# endif /*MULTI_BOOT_STAGE*/

# if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_AUDIO
	profLogPrintf(0, "[host]init: devAudioEnum s %d",__LINE__);
	devAudioEnum();
	profLogPrintf(0, "[host]init: devAudioEnum e %d",__LINE__);

	profLogPrintf(0, "[host]init: devKeyBtnEnum s %d",__LINE__);
	devKeyBtnEnum();
	profLogPrintf(0, "[host]init: devKeyBtnEnum e %d",__LINE__);

#  if MULTI_BOOT_STAGE
	multibootCb(MULTI_BOOT_STAGE_DEV_AUDIO);
	multibootLoadWait(MULTI_BOOT_STAGE_DEV_USB);
#  endif /*MULTI_BOOT_STAGE*/

#  if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_USB
	profLogPrintf(0, "[host]init: devUsbEnum s %d",__LINE__);
	devUsbEnum();
	profLogPrintf(0, "[host]init: devUsbEnum e %d",__LINE__);
#  endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_USB*/
# endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_AUDIO*/
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_MEDIA*/

	profLogPrintf(0, "[host]%s e",__FUNCTION__);
}
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_SECOND*/


/**************************************************************************
 *
 * Function Name: devDispEnum
 *
 * Descriptions:
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISP
void
devDispEnum(
	void
)
{
	sp5kDispDevAlloc(SP5K_DISP_CHNL_0, SP5K_DISP_CHNL_0_DEV_TOT);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	sp5kDispDevAlloc(SP5K_DISP_CHNL_1, SP5K_DISP_CHNL_1_DEV_TOT);
#endif
	#if 0
	/* device 1: lcd panel. */
	/* Register the callback function for HI/LO operation for SPI */
	spiCtrlFunSet(SPI_CTRL_CLK, spiUserCtrlCLK);
	spiCtrlFunSet(SPI_CTRL_DAT, spiUserCtrlDAT);
	spiCtrlFunSet(SPI_CTRL_CS, spiUserCtrlCS);
	#endif
	dispLcdInit();
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	dispCompositeTvOpen();  /* device 2: composite tv. */
#endif
	#if (KIT_WITH_HDMI)
	sp5kDispDevAlloc(SP5K_DISP_CHNL_1, SP5K_DISP_CHNL_1_DEV_TOT);
	dispCompositeTvOpen();  /* device 2: composite tv. */
	dispHdmiOpen();
	dispLcd2Init();
	#endif

	sp5kDispChnlSrcSet(SP5K_DISP_CHNL_0, SP5K_DISP_SRC_0);
	sp5kDispPowerOn(SP5K_DISP_CHNL_0, SP5K_DISP_CHNL_0_PANEL, 0, 0);
	#if 0
	sp5kDispCallbackReg(4);
	#endif
}
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISP*/


#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_RSV
/**************************************************************************
 *                                                                        *
 * Function Name: devDiskSdCallBack                                       *
 *                                                                        *
 * Purposes:                                                              *
 *                                                                        *
 * Desprictions:                                                          *
 *                                                                        *
 * Arguments:                                                             *
 *   None                                                                 *
 * Returns:                                                               *
 *   None                                                                 *
 * See also:                                                              *
 *                                                                        *
 **************************************************************************/
extern UINT32 CathyHostEvb; /*for V35 EVB HW only*/
BOOL IS_EYEFI=0;
void
devDiskSdCallBack(
	UINT32 type,
	UINT8 *pCid,
	UINT32 num
)
{

}

UINT32 devSdInfo, devSdSetting;
UINT8 devSdCid[16];

UINT32
devDiskSdIdGet(
	UINT32 sel
)
{
	UINT32 ret = 0;
	switch (sel) {
		case 0:
			ret = devSdInfo;
			break;
		case 1:
			ret = devSdSetting;
			break;
		case 2:
			ret = (UINT32)devSdCid;
	}

	return ret;
}

UINT32 devDiskSdIdCallBack(UINT32 info, UINT8 *pCid, UINT32 num)
{

	UINT32 specialsettings = 0;

	devSdInfo = info;
	if (pCid != NULL)
		memcpy(devSdCid, pCid, num);

#if SD_FINETUNE_EXAMPLE	/*For Debug*/
	UINT32 sdType, sdMode;
    char *sdTypeName[] = {"Unknown", "SD11", "SD20", "SDHC", "SDXC", "MMC"};

	sdType = (info & SD_INFO_TYPE_MASK) >> SD_INFO_TYPE_LSB;
	sdMode = (info & SD_INFO_MODE_MASK) >> SD_INFO_MODE_LSB;
	if (sdType >= SD_TYPE_MAX) sdType = SD_TYPE_UNKNOWN;

    printf("SD type = %s\n", sdTypeName[sdType]);
#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 sdType;
	char *sdTypeName[] = {"Unknown", "SD11", "SD20", "SDHC", "SDXC", "MMC"};

	sdType = (info & SD_INFO_TYPE_MASK) >> SD_INFO_TYPE_LSB;
	if (sdType >= SD_TYPE_MAX) sdType = SD_TYPE_UNKNOWN;

	printf("SD type = %s\n", sdTypeName[sdType]);
        Global_sdType = sdType;
#endif

   	
	/*paul 2018/04/13 removed set but not used variable*/
	/*IS_SDXC_CARD = sdType == SD_TYPE_SDXC;
	IS_MMC_CARD = sdType == SD_TYPE_MMC;*/
	
#if SD_FINETUNE_EXAMPLE	/* for SBC*/
	if (pCid != NULL && num == 16) {
		/* CID defintion:
		ID			bit
		Manufacturer ID 	8
		OEM ID				16
		Product name		40
		Product revision	8
		Product serial #	32
		Reserved			4
		Manufacturing date	12
		CRC 				7
		Reserved as 1		1
		*/
		/*
			If limitation removed, we can support up to 198Mhz SDR104 mode,
			but it's not stable.
		*/
		/* limit SD CLK <= 50MHz */
		specialsettings = (specialsettings & (~(SD_SP_MAXCLKMHZ_MASK|SD_SP_PHASEADJ_MASK))) | (100 << SD_SP_MAXCLKMHZ_LSB) | (0x80 << SD_SP_PHASEADJ_LSB);

		if ((sdMode >= SD_33V_HS) || IS_SDXC_CARD) {
			/* set driving 1~3 --> 4,6,8ma */
			specialsettings |= (specialsettings & (~SD_SP_DRIVEADJ_MASK)) | (0 << SD_SP_DRIVEADJ_LSB);
		}
	}
#endif

	devSdSetting = specialsettings;

	return specialsettings;
}

/**************************************************************************
 *
 * Function Name: devDiskEnum{Boot,Media}
 *
 * Descriptions:
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
void
devDiskEnumBoot(
	void
)
{
	UINT32 devNr = SP5K_DISK_TOTAL /* tiwu: how many? */
	#if ( (SP5K_DISK_NAND == 1 || SP5K_DISK_EMMC == 1) && SP5K_DISK_SPI == 1)
			- 1; /* SPI and NAND do not exist together */
	#endif
			;

	printf("Total Dev [%u]\n", devNr);
	diskDevAlloc(devNr); /* maximum storage device option */

	/*support FAT32 on 2GB SD*/
#if 0 /*sp5kSystemCfgSet not allowed at early stages of multiboot*/
	/*sp5kSystemCfgSet(SP5K_VFS_FAT32_2GB_SD_CFG, 1);*/
#else
	/*sp5kDiskCfgSet(SP5K_VFS_FAT32_2GB_SD_CFG, 1);*/
#endif

#if ( SP5K_DISK_NAND == 1 )
	diskNandOpen();
#elif ( SP5K_DISK_SPI == 1 )
	diskSpiOpen();
#elif ( SP5K_DISK_EMMC == 1 )
	diskeMmcOpen();
#endif
}
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_RSV*/


#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_MEDIA
void
devDiskEnumMedia(
	void
)
{
	#if (SP5K_DISK_RAM == 1 )
    	sp5kDiskRamAttrSet(0x00600000);/* 6MB */
    	diskRamOpen();
	#endif

	#if ( SP5K_DISK_SD == 1 )
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		UINT32 retval;
#endif
    	diskSdOpen();
		sp5kDiskCfgSet(SP5K_DISK_ID_CALLBACK_CFG, DRIVE_SD, devDiskSdIdCallBack);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
        /* Jason: SDXC No.12: suggest to set XPC=0 if below 150mA.
         * Do this before sp5DiskMount(SP5K_DRIVE_SD) one time only after power on.
         * set XPC=0 to enhance interoperability.
         */
/* comment out until SDXC branch is approved by customer. */
        retval = sp5kDiskCfgSet(SP5K_SD_IO_MODE_CFG, 0x00010000);
        if (retval == 0)
            printf("SDXC No.12: set XPC=0.\n");
        else
            printf("SDXC No.12: Warning: XPC Cfg return value = %d\n", retval);

		UINT32 enable_SD18V = 0;  /* set 0 to disable support of SD 1.8V  */
        retval = sp5kDiskCfgSet(SP5K_DISK_SD18V_CTRL_PIN_CFG, SP5K_DRIVE_SD, SP5K_GPIO_GRP_NO_USE, 0, enable_SD18V);
        if (retval == 0)
            printf("SDXC No.32: UHS Mode disabled.\n");
        else
            printf("SDXC No.32: Warning: UHS Mode return value = %d\n", retval);

#endif
    	iopin_t sdpin[SP5K_DISK_PIN_MAX];
    	appDiskIoCfg(sdpin);
    	/* Initial the GPIO */
    	sp5kDiskDriverPinCfg(
    		SP5K_DRIVE_SD,SP5K_DISK_DETECT_PIN,
    		sdpin[SP5K_DISK_DETECT_PIN].grp,
    		sdpin[SP5K_DISK_DETECT_PIN].id,
    		sdpin[SP5K_DISK_DETECT_PIN].pol);
    	#ifdef SP5K_SD_PWREN_GRP
        	sp5kDiskDriverPinCfg(
        		SP5K_DRIVE_SD,SP5K_DISK_POWER_ON_PIN,
        		sdpin[SP5K_DISK_POWER_ON_PIN].grp,
        		sdpin[SP5K_DISK_POWER_ON_PIN].id,
        		sdpin[SP5K_DISK_POWER_ON_PIN].pol);
    	#endif
    	#ifdef SP5K_SD_WRP_GRP
        	sp5kDiskDriverPinCfg(
        		SP5K_DRIVE_SD,SP5K_DISK_WRITE_PROTECT_PIN,
        		sdpin[SP5K_DISK_WRITE_PROTECT_PIN].grp,
        		sdpin[SP5K_DISK_WRITE_PROTECT_PIN].id,
        		sdpin[SP5K_DISK_WRITE_PROTECT_PIN].pol);
    	#endif
	#endif
	#if ( SP5K_DISK_MS == 1 )
	    diskMsOpen();
    	/* Initial the GPIO */
    	sp5kDiskDriverPinCfg(
    		SP5K_DRIVE_MS,SP5K_DISK_DETECT_PIN,
    		SP5K_SD_DETECT_GRP,
    		SP5K_SD_DETECT_ID,
    		SP5K_SD_DETECT_POL);
    	sp5kDiskDriverPinCfg(
    		SP5K_DRIVE_MS,SP5K_DISK_POWER_ON_PIN,
    		SP5K_SD_PWREN_GRP,
    		SP5K_SD_PWREN_ID,
    		SP5K_SD_PWREN_POL);
	#endif
	#if ( SP5K_DISK_XD == 1 )
    	diskxDOpen();
    	/* Initial the GPIO */
    	sp5kDiskDriverPinCfg(
    		SP5K_DRIVE_XD,SP5K_DISK_DETECT_PIN,
    		SP5K_SD_DETECT_GRP,
    		SP5K_SD_DETECT_ID,
    		SP5K_SD_DETECT_POL);
    	sp5kDiskDriverPinCfg(
    		SP5K_DRIVE_XD,SP5K_DISK_POWER_ON_PIN,
    		SP5K_SD_PWREN_GRP,
    		SP5K_SD_PWREN_ID,
    		SP5K_SD_PWREN_POL);
	#endif

	diskIoCfg.group = sdpin[SP5K_DISK_DETECT_PIN].grp;
	diskIoCfg.pinNum = sdpin[SP5K_DISK_DETECT_PIN].id;
	diskIoCfg.valIni = !sdpin[SP5K_DISK_DETECT_PIN].pol; /* init to no card */
	sp5kScanRegister(&diskIoCfg);

	profLogAdd(0, "[host]init: disk detect s");
	sp5kDiskMount(SP5K_START_DETECT);
	profLogAdd(0, "[host]init: disk detect e");

	#if (SP5K_DISK_NAND == 1 || SP5K_DISK_EMMC == 1)
    	printf( "[host]init: send nand insert msg.\n" );
    	sp5kHostMsgSend(SP5K_MSG_DISK_INSERT, SP5K_DRIVE_NAND);
	#endif

	#if LOAD_RESOURCE_FROM_CARD
    	printf( "[host]init: mount SD card directly.\n" );
    	sp5kDcfStdSysInit();
    	appDiskMountDirect(SP5K_DRIVE_SD);
    	appDiskMountMsgHandler( SP5K_MSG_DISK_MOUNT_COMPLETE, SP5K_DRIVE_SD );
	#endif
}
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_MEDIA*/

/**************************************************************************
 *
 * Function Name: devFrontEnum
 *
 * Descriptions:
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SENSOR
void
devFrontEnum(
	void
)
{
/*shutter, if applicable, must init before sensor service installed*/
	/*sensorMshInit();*/


/*select sensor TG master clock source, select either ONE of the 3*/
#if 0 /*SPCA6350 devFrontEnum mod  +++*/

#if SP5K_FRONT_MCLK_SRC_NOT_CONNECT /*sensor/XTG has own MCLK*/
	sensorMclkSrcCfgSet(SENSOR_MCLK_SRC_NOT_CONNECT);
#elif SP5K_FRONT_MCLK_SRC_SP5K_TGPLL
	sensorMclkSrcCfgSet(SENSOR_MCLK_SRC_SP5K_TGPLL);
#elif SP5K_FRONT_MCLK_SRC_ADCLPI_PAD
	sensorMclkSrcCfgSet(SENSOR_MCLK_SRC_ADCLPI_PAD);
#endif

/*select sensor SyncSigGen clock source master, select either ONE of the 2
 *NOTE: not all the sensor drivers support both modes
 *such sensor drivers will ignore this setting
 */
#if SP5K_FRONT_SYNC_SIG_SP5K_MASTER
	sensorSyncSigMasterCfgSet(SENSOR_SYNC_SIG_SP5K_MASTER);
#elif SP5K_FRONT_SYNC_SIG_SP5K_SLAVE
	sensorSyncSigMasterCfgSet(SENSOR_SYNC_SIG_SP5K_SLAVE);
#endif

#endif  /*SPCA6350 devFrontEnum mod  ---*/

	/*macros defined in macro_expand.h*/
#if SP5K_SENSOR_BACKUP_ENABLE
	devFrontTgDetect();
#else
	devFrontTgInstall();
	devFrontAfeInstall();
#endif
	devFrontTvInDecInstall();
}
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SENSOR*/


#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_AUDIO

#include "middleware/mixer_def.h"
/**************************************************************************
 *                                                                        *
 * Function Name: devAudioEnum                                            *
 *                                                                        *
 * Purposes:                                                              *
 *                                                                        *
 * Desprictions:                                                          *
 *                                                                        *
 * Arguments:                                                             *
 *   None                                                                 *
 * Returns:                                                               *
 *   None                                                                 *
 * See also:                                                              *
 *                                                                        *
 **************************************************************************/
void
devAudioEnum(
	void
)
{
	mixerPlayIni_t ini;
	UINT8 name[]="MIXER";

	ini.pdevName  = name;
	ini.devId     = 0;
	ini.ioId      = 0;
	ini.usBufLen  = 40000;
	ini.subTrkTot = 1;
	mixerPlayOpen(&ini);

	audioCodecInit();
	audioCodec2Init();

	#if SP5K_MULTI_FAST_BOOT
    extern UINT32 appPreLoadAudInit(void);
	appPreLoadAudInit();
	#endif
}

/**************************************************************************
 *
 * Function Name: devKeyBtnEnum
 *
 * Descriptions:
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
void
devKeyBtnEnum(
	void
)
{
#if 1 /*spca6350 boot test*/ /*ADC FW not ready*/
#if SP5K_BUTTON_DIRECT_GPIO_NUMBER || SP5K_BUTTON_ADC_NUMBER || SP5K_BUTTON_MATRIX_GPIO_NUMBER
	UINT32 ret = SUCCESS;
#endif
	/*sp5kAdcInit(); */

#if SP5K_BUTTON_DIRECT_GPIO_NUMBER
	ret = sp5kButtonGpioInit(SP5K_BUTTON_DIRECT_GPIO_NUMBER);
	if( ret != SUCCESS ){
		printf("GPIO Button Driver Init Fail\n");
	}
#endif

#if SP5K_BUTTON_ADC_NUMBER
  #if 0 /* always init already */
	sp5kAdcInit();/* To enable the adc */
  #endif

	ret = sp5kButtonAdcInit(SP5K_BUTTON_ADC_NUMBER);
	if( ret != SUCCESS ){
		printf("ADC Button Driver Init Fail\n");
	}
#endif

#if SP5K_BUTTON_MATRIX_GPIO_NUMBER
	ret = sp5kButtonMatrixInit(SP5K_BUTTON_MATRIX_GPIO_NUMBER);
	if( ret != SUCCESS ){
		printf("Matrix Button Driver Init Fail\n");
	}
#endif
#endif/*spca6350 boot test*/ /*ADC FW not ready*/
}

#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_AUDIO*/


#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_USB

/**************************************************************************
 *
 * Function Name: devUsbEnum
 *
 * Descriptions:
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
void
devUsbEnum(
	void
)
{
#if SP5K_USB_VBUS_SCAN
	iopin_t usbpin[1];

	appUsbIoCfg( usbpin );
	usbIoCfg.group = usbpin[0].grp;
	usbIoCfg.pinNum = usbpin[0].id;
	usbIoCfg.valIni = usbpin[0].pol;
	sp5kScanRegister(&usbIoCfg);
/*	sp5kUsbDriverPinCfg(SP5K_USB_DETECT_PIN,SP5K_VBUS_DETECT_GRP,SP5K_VBUS_DETECT_ID,SP5K_VBUS_DETECT_POL);
*/
#endif

}


/**************************************************************************
 *
 * Function Name: devOtgEnum
 *
 * Descriptions:
 *
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
void
devOtgEnum(
	void
)
{
	#if 0
	/* Initial the otg GPIO */
	usbDriverPinCfg(SP5K_OTG_ID_DETECT_PIN, SP5K_OTG_ID_DETECT_GRP, SP5K_OTG_ID_DETECT_ID , SP5K_GPIO_ACTIVE_LOW );
	#endif
}

#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_USB*/


/**************************************************************************
 *
 * Function Name: usbPowerOnGet
 *
 * Descriptions:
 *
 * Arguments:
 *
 * Returns:
 *
 * See also:
 *
 **************************************************************************/
UINT32
usbPowerOnGet(
	void
)
{
	return 0;
}


/**************************************************************************
 *                                                                        *
 * Function Name: devRtcInit                                              *
 *                                                                        *
 * Purposes:                                                              *
 *                                                                        *
 * Desprictions:                                                          *
 *                                                                        *
 * Arguments:                                                             *
 *   None                                                                 *
 * Returns:                                                               *
 *   None                                                                 *
 * See also:                                                              *
 *                                                                        *
 **************************************************************************/

#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_RTC

extern void (*fpTimeGetCb)(struct tm *ptime); /* time.c */
extern void CustomerTimeGetCb(struct tm *ptime); /* app_Rtc.c */
UINT32 devRtcFail=0;


typedef void (*TzTmxTimeSetCb)(tmx_t *ptime);
typedef void (*TzTmTimeSetCb)(struct tm *ptime);

static TzTmxTimeSetCb realRtcCb = NULL;
static TzTmTimeSetCb localTimeCb = NULL;

void
appTzrealRtcCbReg(
	void *pfunc
)
{
	realRtcCb = (TzTmxTimeSetCb)pfunc;
}

void
appTzlocalTimeCbReg(
	void *pfunc
)
{
	localTimeCb = (TzTmTimeSetCb)pfunc;
}

#define RTC_MAX_YEAR 2035

void customerTimeGetCb(struct tm *ptime)
{
	if(ptime->tm_year+1900 > RTC_MAX_YEAR) {
		/* Keep in max */
		ptime->tm_year = 2017-1900;
		ptime->tm_mon = 1;
		ptime->tm_mday = 1;
		ptime->tm_hour = 0;
		ptime->tm_min = 0;
		ptime->tm_sec = 0;

		if(realRtcCb)
			realRtcCb((tmx_t *)ptime);
	}

	if(localTimeCb)
		localTimeCb(ptime);
}

BOOL rtcWakeupPowerOnChk(void){
	BOOL ret;
	UINT32 val;
	#if defined (PRJ_17DVR)
	sp5kPwrDetectGet(SP5K_PWR_SRC_RTC, &val);	/*get rtc value*/
	ret = (val == 0x1000000)? 1 : 0;
	printf("rtc wakeup val: 0x%x\n",val);
	#else
	sp5kPwrDetectGet(0x1000f, &val);
	ret = (val == 0x10000)? 1 : 0;
	#endif
	printf("rtc wakeup irq: %d\n",ret);
	return ret;
}

void devRtcInit(void)
{
	UINT32 r,reliableCode;
	r = sp5kRtcReliableGet(&reliableCode);
	if ((r!=SUCCESS) ||(reliableCode != RTC_RELIABLE_CODE)) {
		/*
		#if RTC_WAKEUP_SUPPORT
		if (rtcWakeupPowerOnChk()) {
			goto __powerDown;
		}
		#endif
		*/
		printf("!!RTC Lost(%x)\n", reliableCode);
		tmx_t tmx = {
			.tmx_year = 2016-1900, .tmx_mon = 1, .tmx_mday = 1,
			.tmx_hour = 0, .tmx_min = 0, .tmx_sec = 0,
		};
		sp5kRtcDateTimeSet(SP5K_DATE_TIME_OPTION, &tmx);
		sp5kRtcReliableSet(RTC_RELIABLE_CODE);

		devRtcFail = 1;
	}
	sp5kCustomTimeCheckCallbackSet(customerTimeGetCb);
	#if !SPCA5310
	fpTimeGetCb = CustomerTimeGetCb;
	#endif
	return;
	/*
	#if RTC_WAKEUP_SUPPORT
	__powerDown:
	sp5kPowerCtrl(SP5K_POWER_OFF, 0);
	while (1)
		;
	#endif
	*/
}

#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_RTC*/


#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SENSOR
void
rsvBufInit(
	void
)
{
	sp5kResourceBufSizeSet(SP5K_RES_OSD_FONT,		OSDRsvSize[SP5K_RES_OSD_FONT].size);
	sp5kResourceBufSizeSet(SP5K_RES_OSD_ICON,		OSDRsvSize[SP5K_RES_OSD_ICON].size);
	sp5kResourceBufSizeSet(SP5K_RES_OSD_ICON_2,		OSDRsvSize[SP5K_RES_OSD_ICON_2].size);
	sp5kResourceBufSizeSet(SP5K_RES_OSD_ICON_3,		OSDRsvSize[SP5K_RES_OSD_ICON_3].size);
	sp5kResourceBufSizeSet(SP5K_RES_OSD_ICON_4,		OSDRsvSize[SP5K_RES_OSD_ICON_4].size);
	sp5kResourceBufSizeSet(SP5K_RES_OSD_ICON_5,		OSDRsvSize[SP5K_RES_OSD_ICON_5].size);
	sp5kResourceBufSizeSet(SP5K_RES_DATE_STAMP_OSD,	OSDRsvSize[SP5K_RES_DATE_STAMP_OSD].size);
	sp5kResourceBufSizeSet(SP5K_RES_OSD_STRING,		OSDRsvSize[SP5K_RES_OSD_STRING].size);
	sp5kResourceBufSizeSet(SP5K_RES_BP_VIEW,		OSDRsvSize[SP5K_RES_BP_VIEW].size);
	sp5kResourceBufSizeSet(SP5K_RES_BP_CAPTURE,	    OSDRsvSize[SP5K_RES_BP_CAPTURE].size);
}
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SENSOR*/

#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SENSOR
UINT32
appSensorInitDoneCb(
	UINT32 param
)
{
	return SUCCESS;
}
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SENSOR*/

