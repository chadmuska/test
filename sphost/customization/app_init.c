/**************************************************************************
 *
 *       Copyright (c) 2002-2018 by iCatch Technology, Inc.
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
 **************************************************************************/
/**
 * @file app_init.c
 *
 * @author
 */

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include <stdio.h>
#include "common.h"
#include "middleware/slide_def.h"
#include "middleware/proflog.h"
#include "middleware/fs_def.h"
#include "api/sp5k_usb_api.h"
#include "api/sp5k_usbhost_api.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_ptp_api.h"
#include "api/sp5k_rsvblk_api.h"
#include "api/sp5k_os_api.h"
#include "api/sp5k_exif_api.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_rsvblk_api.h"
#include "api/sysboot_api.h"
#include "api/multiboot_api.h"
#include "api/sp5k_modesw_api.h"
#include "api/sp5k_af_api.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_ftrack_api.h"
#if SP5K_ICAT_EIS
#include "api/sp5k_eis_rsc_algo_api.h"
#endif

#include "customization/dev_init.h"
#include "customization/app_init.h"

#if SP5K_MULTI_FAST_BOOT
#include "api/sp5k_os_api.h"
#include "api/sp5k_disk_api.h"
#endif

#if TEST_ADAS_FCWS || TEST_ADAS_LPD
#include "api/sp5k_ftrack_api.h"
#endif

#if defined(PRJ_JETDVR) || defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "hostfw/include/app_infohandlerComm.h"
extern void appInfoHandlerInit(void);
extern void appBPlusIsrInit(void);
extern UINT8 appGsensorSettingGet(void);
extern PowerDetect appInfoPowerDETStateGet(void);
extern void appInfHWWDT(UINT32 ms);
extern UINT32 appInfoParkingSettingGet(void);
extern BOOL appGsensorModeRestore();
extern UINT32 appInfoBatteryLvGet(void);
extern void appInfoSWWDTWakeup(UINT32);
extern void halPwrcEnable(UINT32 en);
#endif

#if SP5K_DIQ_FRMWRK
#include "api/sp5k_dq_api.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
	/*only for SFN file */
	const rsvSize_t OSDRsvSize[] = {
		[SP5K_RES_OSD_FONT	]			={128*1024},
		[SP5K_RES_OSD_ICON	]			={160*1024},
		[SP5K_RES_OSD_ICON_2]			={160*1024},
		[SP5K_RES_OSD_ICON_3]			={32*1024},
		[SP5K_RES_OSD_ICON_4]			={80*1024},
		[SP5K_RES_OSD_ICON_5]			={32*1024},
#if defined(PRJ_PDVR)		
		[SP5K_RES_DATE_STAMP_OSD]		={80*1024},
#else		
		[SP5K_RES_DATE_STAMP_OSD]		={40*1024},
#endif
		[SP5K_RES_OSD_STRING]			={20*1024},
		[SP5K_RES_BP_VIEW]				={16*1024},
		[SP5K_RES_BP_CAPTURE]			={64*1024},
	};

#define APP_SYS_POWER_ON_LOOP_COUNT                     1       /* ms */

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if SP5K_ICAT_EIS
extern void eisInit(void);
#else
#define eisInit()
#endif

#if SP5K_ICAT_EIS
extern void rscInit(void);
#else
#define rscInit()
#endif

#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)
#define READ8(a) (*((volatile UINT8*)(a)))
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if GPS_OPEN
UINT8 *GpsTempBuff=NULL;
UINT8 *GpsTempPos=NULL;
UINT8 *logBuff=NULL;
UINT32 GpsTempSize;
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 #if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern UINT32 powerInit(void);
#else
extern void powerInit(void);
#endif
extern UINT32 hostInit( void );
extern void VC_init(UINT32 *pCallbackFunTab, UINT32 numCallbackFun);
extern UINT32 halDramMBSizeGet(void);
extern UINT32 mediaMovRecorderInit(void);
extern UINT32 mediaMovPlayerInit(void);
extern void sp5kUsbMSDCFuncReg(void);
extern void sp5kUsbVNDRFuncReg(void);
extern void ldcPvModeInit(void);
extern void sp5kCalBpInit(void);
extern void sp5kCalLscInit(void);
UINT32 snapStaticInit( void );
void snapHeapBlockSet(UINT32 mode, UINT32 rawBlocks, UINT32 jpegBlocks, UINT32 w, UINT32 h, UINT32 codeSize, UINT32 qvW, UINT32 qvH);
void snapMaxSensorSizeGet( UINT32 *, UINT32 * );
UINT32 pbStaticInit( void );
UINT32 pbEditStaticInit( void );
void pbMaxBufSizeSet(UINT32 maxWidth, UINT32 maxHeight, UINT32 yuvRatio, UINT32 jpgRatio);
void sp5kGfxPaletteCallbackInit(void);

#if GPS_OPEN
extern void  appUart2ndInit(void);
#endif
extern void uhDRecordInit(void);

#if SP5K_ICAT_EIS
extern void pvRawRecordInit(void);
#endif

/*Isaac for 17DVR*/
#if 1/*GSENSOR*/
extern void appGsensorInit(void);
extern BOOL appGsesnorParkingEvent();
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 *  @brief      callback function for disk mount to return status
 *
 *  @param[in]  drvId    drive ID of disk
 *  @param[in]  status   the status of disk mount
 *
 *  @return     SUCCESS or FAIL
 *
 *  Default middleware will send sp5k disk message to host to notify
 *  mount status except following is done.
 *  appDiskMountStatusCb() is enabled when host set 1 to SP5K_DISK_CALLBACK_CFG
 *  selector in sp5kSystemCfgSet(). After host calls sp5kDiskMount(drvId),
 *  the callback function will be called by disk mount task in middleware.
 */

#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_RSV

UINT32
appDiskMountStatusCb(
	UINT32 drvId,
	UINT32 status
)
{
	UINT32 ret=SUCCESS;

	#if 0
	profLogPrintf(0, "mntStatus %d:%d", drvId, status);

	if (status == DISK_MOUNT_STATUS_START) { /* start to mount */
	}
	else if (status == DISK_MOUNT_STATUS_COMPLETE) { /* mount complete */
		UINT32 mnt=0;
		if (isSdCardExist()) {/* SD */
			if (drvId == SP5K_DRIVE_SD) {
				mnt = 1;
			}
		}
		else { /* NAND */
			if (drvId == SP5K_DRIVE_NAND) {
				mnt = 1;
			}
		}
		if (mnt) { /* ready to init DCF */
			UINT8 dirFreeChar[6] = "DICAM";
			UINT8 fileFreeChar[6] = "DSCI";

			if (sp5kDcfFsInit(drvId, 0, 0) == SUCCESS) {
				sp5kDcfFsActive(drvId);
				sp5kDcfFsFreeCharSet(dirFreeChar, fileFreeChar, SP5K_DCF_NEXTKEY_MAX);
			}
		}
	}
	else if (status == DISK_MOUNT_STATUS_FAIL) { /* fail */
	}
	#endif

	return ret;
}
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DISK_RSV*/

#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_APP_START

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Limitations:                                                          *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
#if GPS_OPEN
void appGpsTempBuffSizeSet(UINT32 size)
{
	GpsTempSize=size;
}

UINT32 appGpsTempBuffSizeGet(void)
{
	return GpsTempSize;
}
#endif


/**************************************************************************
 *  Function Name: appResetBtnCtrl                              *
 **************************************************************************/
#define APP_RESET_CODE 33
#define APP_RESET_REG  10

void
appResetBtnCtrlSet(
	UINT8 bNormalOff
)
{
    UINT8 ResetCode;

    if(bNormalOff){
        ResetCode = 0;
        sp5kRtcExtraSet(&ResetCode, APP_RESET_REG, 1);
    }
    else {
        ResetCode = APP_RESET_CODE;
        sp5kRtcExtraSet(&ResetCode, APP_RESET_REG, 1);
    }
}

UINT8
appResetBtnCtrlGet(
	void
)
{
    UINT8 ResetCode, ret = FALSE;

    sp5kRtcExtraGet(&ResetCode, APP_RESET_REG, 1);
    if(ResetCode == APP_RESET_CODE)
        ret = TRUE;
    else
        ret = FALSE;

    return ret;
}

/**************************************************************************
 *  Function Name: powerInit                                        *
 **************************************************************************/
#define APP_SYS_POR_BY_NOR        0
#define APP_SYS_POR_BY_RTC        1
#define APP_SYS_POR_BY_WDR        2
#define APP_SYS_POR_BY_RESET      3

UINT32 gPowerKey = SP5K_PWR_RISE_EVT_PWRON;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
static UINT32 hostPowerOnSource=APP_SYS_POR_BY_NOR;
UINT32 powerReason(){	
	return hostPowerOnSource;
}
#endif


#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
UINT32 powerInit(void)
#else
void powerInit(void)
#endif
{
	
	UINT32 rtc_val, rest_val;
   
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
UINT32 BP,hwRest;
#else
 UINT32 hostPowerOnSource=APP_SYS_POR_BY_NOR;
#endif
	printf("-------------------------------- powerInit s\n");
	/*
	 * To do power on, debounce the power on pin at
	 * the same status for n times, if power on pin
	 * status change < n times, the power on fail and
	 * return to power off state. The LOOP_COUNT is by 1ms
 	 * Define Power Key behavior(force power off or reboot)
	 * from "..\sphost\include\customization\dev_init.h.tmpl".
	 */
#if	defined(HW_SBC) || defined(HW_JETDVR)    
	sp5kSystemCfgSet(SP5K_SYS_PWRONSRC_PINMASK_CFG, SP5K_PWR_SRC_PLAYBACK);
#elif defined(PRJ_PDVR)||defined(PRJ_17DVR)||defined(PRJ_19DVR)
	sp5kSystemCfgSet(SP5K_SYS_PWRONSRC_PINMASK_CFG, SP5K_PWR_SRC_USB);
#else    
	sp5kSystemCfgSet(SP5K_SYS_PWRONSRC_PINMASK_CFG, SP5K_PWR_SRC_PWRON);
#endif

	UINT8 wdgrstflag =  READ8(0xB0001014);		    /*get wdt flag*/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 pwrVal;
	sp5kGpioRead(SP5K_GPIO_GRP_POCU,0xffffffff,&pwrVal);
	gpioRead(SP5K_GPIO_GRP_FML ,BIT1, &BP);
	sp5kPwrDetectGet(SP5K_PWR_RISE_EVT_PWRON, &hwRest);	/*get reset status*/
#endif
	sp5kPwrDetectGet(SP5K_PWR_SRC_RTC, &rtc_val);	/*get rtc value*/
	sp5kPwrDetectGet(SP5K_PWR_FALL_EVT_CUSTOM, &rest_val);	/*get reset status*/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
printf("%s   SP5K_PWR_FALL_EVT_CUSTOM rest_val 0x%x rtc_val 0x%x hwRest 0x%x  pwrVal 0x%x  \n",__FUNCTION__ , rest_val, rtc_val,hwRest ,pwrVal);
#endif

	if(wdgrstflag) {
		hostPowerOnSource=APP_SYS_POR_BY_WDR;
	}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	else if (pwrVal & BIT1) {		
		hostPowerOnSource = APP_SYS_POR_BY_NOR;	
	}
#endif
	else if(rtc_val) {
		hostPowerOnSource=APP_SYS_POR_BY_RTC;
	}
	else if(rest_val && appResetBtnCtrlGet()) {
		hostPowerOnSource=APP_SYS_POR_BY_RESET;
	}
	else {
		hostPowerOnSource=APP_SYS_POR_BY_NOR;
#if	!defined(HW_SBC) || !defined(HW_JETDVR)|| !defined(HW_PDVR)|| !defined(HW_17DVR)|| !defined(HW_19DVR)
        sp5kPwrDetectGet(SP5K_PWR_FALL_EVT_PLAYBACK, &gPowerKey);
#endif
	}

	if(hostPowerOnSource==APP_SYS_POR_BY_NOR)
	{
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		UINT8 ret;
		/*SP5K_POWER_ON just can modify when normal power on*/
		ret = sp5kPowerCtrl(SP5K_POWER_ON, APP_SYS_POWER_ON_LOOP_COUNT);

		if( ret == SUCCESS )
		{
		}
		else
		{
			sp5kPowerCtrl(SP5K_POWER_OFF, 0);
		}
#endif
	}
	/*for Hw WDT rest*/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if(hwRest & SP5K_PWR_RISE_EVT_PWRON){
		/*	SAR2 9900 = 1.8V ==> battery 8.9V */		
		if(appInfoBatteryLvGet() <  9900 ) {			
			sp5kPowerCtrl(SP5K_POWER_OFF, 0);			
			while(1);		
		}	
	}
#endif
    appResetBtnCtrlSet(FALSE);

	printf("-------------------------------- powerInit e");
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	return hostPowerOnSource;
#endif
}

void
appInit(
	void
)
{
#if CXX_SUPPORT
	extern void cxxHeapInit();
	cxxHeapInit();
#endif

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 pwrVal=0 , rest_val = 0;
	UINT32 info;
#endif

	extern UINT32 diskUsbHostOpen(void);
	extern void net_system_init();

	printf("[host]init: -------------------\n");
	printf("[host]init: appInit start:%dms\n",sp5kOsTimeGet() * 10);
	printf("[host]init: -------------------\n");

	#ifndef FPGA
	/* power on */
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
powerInit();
#endif
	#endif /* #ifndef FPGA */

	#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SYS_RES
	#if SP5K_DIQ_FRMWRK && !API_TEST_INIT && !POWERON_PB_WITHOUT_SENSOR
		sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_FIF_WORK_DIR, "A:\\RO_RES\\");
		/*sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_CDSP_RES_NO_REDUCE,1);*/	/* if you don't wanna reduce cdsp resource memory */
		sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_FIF_INIT,1);
	#endif
	#endif

	#if defined(USB_VNDR_ICAT_E3)
	/*extern void sp5kCmdExec(char *cmd);
	*/
	sp5kUsbCmdProcessorReg( 0xB0, sp5kCmdExec );
	printf( ">> sp5kUsbCmdProcessorReg( 0xB0 ) <<\n" );
	#endif

	#if SPCA6330
	/*Only for 6330 base rev.14665 and 6350 base rev.5492*/
	/* install sp5k mode function separately */
	sp5kModeStillPrevInit();/* sp5k still pv mode */
	sp5kModeStillSnapInit();/* sp5k still snap mode */
	sp5kModeVideoPrevInit();/* sp5k video pv mode */
	sp5kModeVideoRecInit();/* sp5k video rec mode */
	sp5kModeStillPbInit();/* sp5k still pb mode */
	sp5kModeMultiPbInit();/* sp5k multi pb mode */
	sp5kModeAudioRecInit();/* sp5k audio rec mode */
	sp5kModeAvPlayInit();/* sp5k AV playback mode */
	sp5kModePowerSaveInit();
	/* note: the sp5k mode not initialized will be set as idle mode */
	#endif

	#if MULTI_BOOT_STAGE
	multibootCb(MULTI_BOOT_STAGE_APP_START);
	multibootLoadWait(MULTI_BOOT_STAGE_CMD);
	#endif

	#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_CMD
	cmdTaskInit();

#if !(POWERON_PB_WITHOUT_SENSOR)
	snapStaticInit();
	sp5kStillCapCfgSet(SP5K_CAPTURE_MAX_COMPRESSION_RATIO, SNAP_MAX_COMPRESSION_RATIO);
#endif

	#ifdef SDIO_INTERFACE_ENABLE
	sp5kSystemCfgSet(SP5K_DISK_SD_HW_PIN_CFG, 1);   /* Cathy_V2 GPIO redefined */
	#endif

	UINT32 w,h;
	snapMaxSensorSizeGet(&w, &h);
	#if SPCA5210
	if (halDramMBSizeGet() > 32)
		pbMaxBufSizeSet(w, h, 1, 4);
	else
		pbMaxBufSizeSet(w, h, 4, 4); /* Edward 20080828 */
	#endif

	snapFastBkPreviewModAdd(SNAP_FAST_BK_PREVIEW_SUPPORT);
	pbStaticInit();
	pbEditStaticInit();
	sp5kGfxPaletteCallbackInit();

	/*media decoders*/
	h264DecoderRegister();
	mjpgDecoderRegister();
	pcmDecoderRegister();
	mulawDecoderRegister();
	alawDecoderRegister();
	adpcmDecoderRegister();
	mp3DecoderRegister();
	wmaDecoderRegister();
	aacDecoderRegister();

	/*media encoders*/
	h264EncoderRegister();
	mjpgEncoderRegister();
	pcmEncoderRegister();
	mulawEncoderRegister();
	alawEncoderRegister();
	adpcmEncoderRegister();
	aacEncoderRegister();

	/*media mux recorders*/
	mediaAviRecorderInit();
	mediaAsfRecorderInit();
	mediaWavRecorderInit();
	mediaMp3RecorderInit();
	mediaMovRecorderInit();

	/*media demux players*/
	mediaAviPlayerInit();
	mediaAsfPlayerInit();
	mediaWavPlayerInit();
	mediaMp3PlayerInit();
	mediaWmaPlayerInit();
	mediaMovPlayerInit();

	/*media demux editors*/
	mediaAviEditorInit();
	mediaMovEditorInit();

	/*iCatch default ae/awb*/
	aeDefaultInit();
	awbDefaultInit();

	/*4K2K Init*/

	uhDRecordInit();


	/*ALG effect*/
#if ALG_EFFECT_FACE_BEAUTY_SUPPORT
	faceBeautyProcAdd();
#endif

#if ALG_EFFECT_SUPER_RESOLUTION_SUPPORT
	superResolutionProcAdd();
#endif

#if ALG_EFFECT_MULTI_WDR_SUPPORT
	multiWdrProcAdd();
#endif

#if ALG_EFFECT_DYNAMIC_TONE_SUPPORT
	dynamicToneEffectProcAdd();
#endif

#if ALG_EFFECT_SEPIA_SUPPORT
	sepiaEffectProcAdd();
#endif

#if ALG_EFFECT_POSTERIZE_SUPPORT
	posterizeEffectProcAdd();
#endif

#if ALG_EFFECT_SOFT_SUPPORT
	softEffectProcAdd();
#endif

#if ALG_EFFECT_PAINT_SUPPORT
	paintEffectProcAdd();
#endif

    #if SP5K_ICAT_EIS
        pvRawRecordInit();
    #endif

	/* -- iCatch DIST support -- */
	videoDistInit();

	/* -- iCatch EIS support -- */
    eisInit();
    rscInit();
#if (SP5K_ICAT_EIS)
    sp5kRscAlgoInit(SP5K_RSC_ALGO_VERSION_1);
#endif
/*SHEN: for DIST or IME switch, move to appVideo_SetRscEis().*/

	#if USB_DRIVER_INIT

	#if USB_DISK_SUPPORT
	/* register oss/usbhost module functions. */
	diskUsbHostOpen();
	profLogPrintf(0, "[host]init: usbhostFuncInstall s %d",__LINE__);
	usbhostFuncInstall(); /*Shen, usbhostMoudleFuncReg() MUST be earlier than sp5kUsbhostControl()*/
	profLogPrintf(0, "[host]init: usbhostFuncInstall e %d",__LINE__);
	sp5kUsbhostControl(SP5K_USBHOST_CTRL_INIT, 0xff, 1024 * 1024, 0);
	profLogPrintf(0, "[host]init: sp5kUsbhostControl e %d",__LINE__);
	#endif


	sp5kUsbMSDCFuncReg();
	sp5kPtpFuncReg(SP5K_PTP_USB);
	sp5kUsbVNDRFuncReg();
	sp5kUsbDPSFuncReg();
#if GPS_OPEN	
	appUart2ndInit();	
#endif	
/*Isaac for 17DVR*/	
#if defined(PRJ_JETDVR) || defined(PRJ_17DVR)|| defined(PRJ_19DVR)/*Add for UART data recdvicer and handler command to do  Isaac 20170322 */	
	appInfoHandlerInit();
#endif
#if defined(HW_PDVR)	
	appGsensorInit();	
#endif	
#if defined(HW_17DVR)
	appGsensorInit();	
	appBPlusIsrInit();
#elif defined(HW_19DVR)
	appGsensorInit();	
	appBPlusIsrInit();
#endif
	#if API_TEST_INIT
	VC_init(NULL, 0);  /* For API-Test */
	#endif
	/*sp5kUsbFuncCreate();*/
	#endif /*#if USB_DRIVER_INIT*/

	#if !MULTI_BOOT_STAGE
	appInitDelay();
	#endif /*!MULTI_BOOT_STAGE*/
	#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_CMD*/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	appInfHWWDT(250);    
	sp5kSystemCfgGet(SP5K_SYS_CFG_BOOT_FW_INFO, &info);    
	if (info != 0x80000001) {        
		if (powerReason() == APP_SYS_POR_BY_RTC) {            
			printf("%s Set parking mode G sensor\n", __FUNCTION__);            
			appGsensorModeRestore();	         
			sp5kGpioRead(SP5K_GPIO_GRP_POCU, BIT1, &pwrVal);	       
			if (pwrVal) {	            
				appInfoSWWDTWakeup(1);		    
				/* sp5kTimeDelay(UINT32 unit, UINT32 count);*/	        
			}	        
			halPwrcEnable(0);	        

			while (1)	 ;	
		}	
		else {       
			BOOL _parking;        
			_parking = appGsesnorParkingEvent();       
			if (_parking)            
				rest_val = SP5K_PWR_FALL_EVT_CUSTOM;    
			}    
	}
	else {        
		BOOL _parking;       
		_parking = appGsesnorParkingEvent();        
		if (_parking)            
			rest_val = SP5K_PWR_FALL_EVT_CUSTOM;    
	}	
	printf("%s   SP5K_PWR_FALL_EVT_CUSTOM 0x%x\n",__FUNCTION__ , rest_val);	
	if(  rest_val  == SP5K_PWR_FALL_EVT_CUSTOM)  /*for parking mode wakeup pwron3*/	
	{		
		printf("appInit   Parking wakeup\n");		/*err =*/ 		
		sp5kHostMsgSend(INFO_QUEUEMSG_POWER, POWER_DET_PARKING_WAKEUP);	
	}	
	else	{		/* no parking mode, should be ACC on */		
		printf("ACC wakeup  Gsensor set 0x%X\n" , appGsensorSettingGet());		/*err = */		
		sp5kHostMsgSend(INFO_QUEUEMSG_POWER, POWER_DET_ACC_ON);	
	}
#endif
}
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_APP_START*/


#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_APP_DELAY
/**
 * \brief loaded after host init, if multi boot supported,
 * suggesting to init least priority applications here
 */
void
appInitDelay(
	void
)
{
#if TEST_ADAS
    sp5kAdasInit();
#if TEST_ADAS_LDWS
    /* LDWS init */
    laneDetInstall();
#endif

#if TEST_ADAS_FCWS || TEST_ADAS_LPD
    /* Because of LPD based on FCWS */
    ptrackCarInit();
    ftrackPvInit();
    ftrackPvSoftIIModuleAdd();
    /* FCWS init */
    sp5kPatternTrackTargetSet(SP5K_PATTERN_TRACK_TARGET_CAR);
    sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_MIN_FACE_SIZE, 76);
#endif

#if TEST_ADAS_LPD
    lpdInstall();
#endif

#if TEST_ADAS_ROD
    rodInstall();
 #endif

#endif

#if HW_FACETRACK
	/* -- iCatch face tracking support -- */
	ftrackAlgoInit(FACE_TRACK_ALGO_TYPE);
	sp5kPatternTrackTargetSet(SP5K_PATTERN_TRACK_TARGET_FACE);
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_MIN_FACE_SIZE, 76);
	ftrackPvInit();
	ftrackSnapInit();
	ftrackPbInit();
	ftrackPvSoftIIModuleAdd();
	ftrackFDSoftIIModuleAdd();
	ftrackFDSoftUVModuleAdd();
	ftrackFrameFleshModuleAdd();
	profLogAdd(0, "FD installed!");
#endif
	#if MULTI_BOOT_STAGE
	multibootCb(MULTI_BOOT_STAGE_APP_DELAY);
	multibootLoadWait(MULTI_BOOT_STAGE_SLIDE);
	#endif

	#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SLIDE

	printf("[host]init: ~~ %s %d \n",__FUNCTION__ ,__LINE__);

	#if MULTI_BOOT_STAGE
	multibootCb(MULTI_BOOT_STAGE_SLIDE);
	#endif
	#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_SLIDE*/
	printf("[host]init: ~~ %s %d \n",__FUNCTION__ ,__LINE__);

	#if GPS_OPEN
	appUart2ndInit();
	#endif
	ldcPvModeInit();
	sp5kCalBpInit();
	sp5kCalLscInit();

	#if LIB_DEBUG_MODE
	_dbg_proc_ps_en(3);
	#endif

	printf("[host]init: ~~ %s %d \n",__FUNCTION__ ,__LINE__);
}
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_APP_DELAY*/

UINT32 appImageEditSaveCb(UINT8 *buf, UINT32 size)
{
	return 0;
}

#if !MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_AUDIO
SP5K_THREAD *pPreLoadAudThread = NULL;
void
appPreLoadAudTask(
	UINT32 param
)
{
	while(1)
	{
		profLogAdd(0, "[host]init: audio fif init -s");
		if (fsFifOpen("A:\\RO_RES\\WAV\\AUDIO.BIN")==0)
		{
			profLogAdd(0, "[host]init: audio load fail..");
		}
		profLogAdd(0, "[host]init: audio fif init -e");

		sp5kOsThreadTerminate(pPreLoadAudThread);
		sp5kOsThreadDelete(pPreLoadAudThread);
	}
}

UINT32
appPreLoadAudInit(
	void
)
{
	pPreLoadAudThread = sp5kOsThreadCreate("PreLoadAudTask",
								  appPreLoadAudTask,
								  0,
								  15,
								  15,
								  0,
								  1);

	return SUCCESS;
}
#endif /*!MULTI_BOOT_STAGE || MULTI_BOOT_STAGE >= MULTI_BOOT_STAGE_DEV_AUDIO*/
