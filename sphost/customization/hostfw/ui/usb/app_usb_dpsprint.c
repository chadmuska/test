/**************************************************************************
 *
 *       Copyright (c) 2006-2018 by iCatch Technology, Inc.
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

#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"
#include "app_dev_plug.h"
#include "sp5k_ptp_api.h"
#include "app_osdfont.h"
#include "app_osd_api.h"
#include "app_menu_frameosd.h"
#include "app_usb_dpslib.h"
#include "app_errorosd.h"
#include "app_usb_dps_menu.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 IsSetImage = 0 ;
UINT16 dpsErrType = 0;
UINT32 dpsPrintStatus = PIMA_SERVICESTATUS_IDLE;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern appSubMenuInfo_t gDpsSubMenu;
extern dpsInfo_t  gDpsInfo;
extern dpsImage_t gDpsImage;
extern UINT32 enPartial;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PrintErrorMsg
 *------------------------------------------------------------------------*/
void appUsbDps_PrintErrorMsg(
	UINT32 errorreason
)
{
	switch (errorreason) 
	{
		case PIMA_ERROR_NO_REASON:
			appOsdMessageBoxShow(TRUE,STRTK_F_ERROR);
			break;
		case PIMA_ERROR_PAPER_RELATED:
			appOsdMessageBoxShow(TRUE,STRTK_NO_PAPER);
			break;
		case PIMA_ERROR_PAPER_EMPTY:
			appOsdMessageBoxShow(TRUE,STRTK_NO_PAPER);
			break;
		case PIMA_ERROR_LOAD_ERROR:
			appOsdMessageBoxShow(TRUE,STRTK_LAOD_ERR);
			break;		
		case PIMA_ERROR_EJECT_ERROR:
			appOsdMessageBoxShow(TRUE,STRTK_EJECT_ERR);
			break;		
		case PIMA_ERROR_MEDIA_ERROR:
			appOsdMessageBoxShow(TRUE,STRTK_F_ERROR);
			break;
		case PIMA_ERROR_PAPER_JAM:
			appOsdMessageBoxShow(TRUE,STRTK_PAPER_JAM);
			break;
		case PIMA_ERROR_PAPER_NEARLY_EMPTY:
			appOsdMessageBoxShow(TRUE,STRTK_PAPER_ERR);
			break;		
		case PIMA_ERROR_PAPER_TYPE_NOTSUPPORT:
			appOsdMessageBoxShow(TRUE,STRTK_PAPER_ERR);
			break;		
		case PIMA_ERROR_INK_RELATED:
			appOsdMessageBoxShow(TRUE,STRTK_INK_EMPTY);
			break;
		case PIMA_ERROR_INK_EMPTY:
			appOsdMessageBoxShow(TRUE,STRTK_INK_EMPTY);
			break;
		case PIMA_ERROR_INK_LOW:
			appOsdMessageBoxShow(TRUE,STRTK_INK_LOW);
			break;		
		case PIMA_ERROR_WASTE_INK_ERROR:
			appOsdMessageBoxShow(TRUE,STRTK_WASTE_INK);
			break;		
		case PIMA_ERROR_HARDWARE_RELATEDR:
			appOsdMessageBoxShow(TRUE,STRTK_PRINTER_ERR);
			break;
		case PIMA_ERROR_SERVICE_CALL:
			appOsdMessageBoxShow(TRUE,STRTK_SERVICE_CALL);
			break;
		case PIMA_ERROR_PRINT_UNAVAILABLE:
			appOsdMessageBoxShow(TRUE,STRTK_PRINTER_ERR);
			break;		
		case PIMA_ERROR_PRINT_BUSY:
			appOsdMessageBoxShow(TRUE,STRTK_PRINTER_BUSY);
			break;		
		case PIMA_ERROR_LEVER_ERROR:
			appOsdMessageBoxShow(TRUE,STRTK_LEVER_ERR);
			break;		
		case PIMA_ERROR_COVER_OPEN:
			appOsdMessageBoxShow(TRUE,STRTK_COVER_OPEN);
			break;		
		case PIMA_ERROR_NO_MARKING_AGENT:
			appOsdMessageBoxShow(TRUE,STRTK_PRINTER_ERR);
			break;
		case PIMA_ERROR_INK_COVER_OPEN:
			appOsdMessageBoxShow(TRUE,STRTK_INK_COVER_OPEN);
			break;
		case PIMA_ERROR_NO_INK_CARTRIDGE:
			appOsdMessageBoxShow(TRUE,STRTK_NO_INK_CARTRIDGE);
			break;		
		case PIMA_ERROR_FILE_RELATED:
			appOsdMessageBoxShow(TRUE,STRTK_F_ERROR);
			break;		
		case PIMA_ERROR_PRINTINFO_ERROR:
			appOsdMessageBoxShow(TRUE,STRTK_INFO_ERR);
			break;
		case PIMA_ERROR_FILE_DECODE_ERROR:
			appOsdMessageBoxShow(TRUE,STRTK_F_ERROR);
			break;
		default:
			appOsdMessageBoxShow(TRUE,STRTK_F_ERROR);
			break;

	}

}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PrintServiceStatus
 *------------------------------------------------------------------------*/
void appUsbDps_PrintServiceStatus(
	UINT32 status,
	UINT32 endReason,
	UINT32 errorStatus
)
{
	switch (status) 
	{
		case PIMA_SERVICE_STATUS_PRINTING:
			gDpsInfo.dpsModePara = DPS_PRINTINGMODE;
			appOsdMessageBoxShow(TRUE,STRTK_PRINTING);
			break;
		case PIMA_SERVICESTATUS_IDLE:
			if(endReason==PIMA_JOBEND_REASON_ABORTJOB){
				enPartial = 0;
				gDpsInfo.PatialPics =0;
				gDpsInfo.dpsModePara = DPS_PRINT_ALL_COMPLETEMODE;
				appOsdMessageBoxShow(TRUE,STRTK_COMPLETED);
				memset(gDpsInfo.PicBuf,0,gDpsImage.totalImg);
				appTimeDelayMs(2000);
				appStateChange(APP_STATE_USB_PRINTMENU,STATE_PARAM_NORMAL_INIT);
			}else {
				if((enPartial == 1)&&(gDpsInfo.PatialPics + DPS_PARTIAL_PIC_NUM)<gDpsImage.totalImg){
					gDpsInfo.PatialPics+= DPS_PARTIAL_PIC_NUM;
					_DpsAddImageProcess();
					appUsbDps_PrintStart;
					gDpsInfo.dpsModePara=DPS_PRINTINGMODE;

				}else{
					enPartial = 0;
					gDpsInfo.PatialPics =0;
					gDpsInfo.dpsModePara = DPS_PRINT_ALL_COMPLETEMODE;
					appOsdMessageBoxShow(TRUE,STRTK_COMPLETED);
					memset(gDpsInfo.PicBuf,0,gDpsImage.totalImg);
					appTimeDelayMs(2000);
					appStateChange(APP_STATE_USB_PRINTMENU,STATE_PARAM_NORMAL_INIT);
				}
			}
			break;
		case PIMA_SERVICESTATUS_PAUSED:
			gDpsInfo.dpsModePara = DPS_PAUSEMODE;
			break;		
		default:
			HOST_PROF_LOG_ADD(LEVEL_ERROR,"usb dps pr: PrintServiceStatus error");
			break;

	}
	dpsPrintStatus = status;

}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PrintServiceStatus
 *------------------------------------------------------------------------*/
void appUsbDps_PrintErrorStatus(
	UINT32 errorstatus,
	UINT32 errorreason
)
{
//temp hsinhau 04/21
	switch (errorstatus) 
	{
		case PIMA_ERROR_STATUS_NO_ERROR:
			break;
		case PIMA_ERROR_STATUS_WARNING:
			appUsbDps_PrintErrorMsg(errorreason);
			gDpsInfo.dpsModePara = DPS_PAUSEMODE;
			break;
		case PIMA_ERROR_STATUS_FATAL_ERROR:
			appUsbDps_PrintAbort;
			appOsdMessageBoxShow(TRUE,STRTK_FATAL_ERROR);
			appTimeDelayMs(2000);
			memset(gDpsInfo.PicBuf,0,gDpsImage.totalImg);
			
			appStateChange(APP_STATE_USB_PRINTMENU,STATE_PARAM_NORMAL_INIT);
			break;		
		default:
            HOST_PROF_LOG_ADD(LEVEL_ERROR,"usb dps pr: nPrintErrorStatus error");
			break;

	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDpsPrintStateInit
 *------------------------------------------------------------------------*/
void
appUsbDps_PrintStateInit(
	void
)
{
	CLEAR_OSD_SRC;
	appUsbDps_DpsPrintStatePrintInitProcess();
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsPrintStateOnKey
 *------------------------------------------------------------------------*/
void 
appUsbDps_DpsPrintStateOnKey(
	UINT32 msg,
	UINT32 param)
{
	switch (msg) 
	{
		case APP_KEY_PRESS_S1:
		case APP_KEY_PRESS_TELE:
		case APP_KEY_PRESS_WIDE:
		case APP_KEY_RELEASE_WIDE:
		case APP_KEY_RELEASE_TELE:
		case APP_KEY_PRESS_MODE:	
			break;	
		case APP_KEY_PRESS_MENU:
			switch(gDpsInfo.dpsModePara)
			{
				case DPS_PRINTINGMODE:
				case DPS_PAUSEMODE:
				case DPS_HALTEDMODE:
					CLEAR_OSD_SRC;
					appUsbDps_PrintAbort;
					appOsdMessageBoxShow(TRUE,STRTK_CANCELING);
					appTimeDelayMs(2000);
					gDpsInfo.dpsModePara=DPS_PRINT_ALL_COMPLETEMODE;
					enPartial = 0;
					gDpsInfo.PatialPics =0;
					memset(gDpsInfo.PicBuf,0,gDpsImage.totalImg);
					break;
				default:
					break;

			}
			appStateChange(APP_STATE_USB_PRINTMENU,STATE_PARAM_NORMAL_INIT);
			break;	
	      case APP_KEY_PRESS_SET:
			switch(gDpsInfo.dpsModePara)
			{
				case DPS_PAUSEMODE:
					appUsbDps_PrintContinue;
					CLEAR_OSD_SRC;
					appOsdMessageBoxShow(TRUE,STRTK_PRINTING);
					gDpsInfo.dpsModePara = DPS_PRINTINGMODE;
					break;
				case DPS_PRINTCOMPLETEMODE:
				case DPS_PRINT_ALL_COMPLETEMODE:
					gDpsInfo.dpsModePara=DPS_PRINT_ALL_COMPLETEMODE;
					enPartial = 0;
					gDpsInfo.PatialPics =0;
					memset(gDpsInfo.PicBuf,0,gDpsImage.totalImg);
					appStateChange(APP_STATE_USB_PRINTMENU,STATE_PARAM_NORMAL_INIT);
					break;
				default:
					break;
			}
			break;
		case APP_KEY_PRESS_RIGHT:
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_UP:
		case APP_KEY_PRESS_DOWN:
			break;	
		default:
			break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsPrintStateOnMsg
 *------------------------------------------------------------------------*/
void 
appUsbDps_DpsPrintStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{

	DBG_PRINT("usb dps pr: *****************\n");
	DBG_PRINT("usb dps pr:     msg = %x \n",msg);
	DBG_PRINT("usb dps pr:     param = %x \n",param);
	DBG_PRINT("usb dps pr: *****************\n");
	
	switch ( msg) {
	case SP5K_MSG_USB_DPS_DISCOVER:	
	case SP5K_MSG_USB_DPS_NOTIFY_JOB_STATUS:	
	case SP5K_MSG_USB_DPS_NOTIFY_DEVICE_STATUS:	
		appUsbDps_Dps_HostMsgProcess(msg,param);
		break;

	default:
		break;
	}	
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDpsPrintState
 *------------------------------------------------------------------------*/
void 
appUsbDpsPrintState(
	UINT32 msg,
	UINT32 param
)
{	
	DBG_PRINT("usb dps pr: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);
	
	switch (msg) {
	case APP_STATE_MSG_INIT:	
		RPrintf("appUsbDpsPrintState");
		appUsbDps_PrintStateInit();
		break;
	case APP_STATE_MSG_CLOSE:	
		IsSetImage = 0;
		appStateCloseDone();
		break;
	case APP_UI_MSG_USB_PLUG:	
		sp5kFree(gDpsInfo.PicBuf);
		appStateCloseDone();
		break;
	default:	
		if (IS_APP_KEY_MSG(msg)) {
			appUsbDps_DpsPrintStateOnKey(msg,param);
		}
		else{
			appUsbDps_DpsPrintStateOnMsg(msg,param);
		}
		break;
	}
} 

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_Dps_HostMsgProcess
 *------------------------------------------------------------------------*/
void appUsbDps_Dps_HostMsgProcess(UINT64  hMsg,UINT32 param )
{

	UINT8 str[4];
	UINT16 progressN;
	sp5kDpsJobStatus_t * pjobStatus;
	sp5kDpsDeviceStatus_t * pdeviceStatus;

	switch(hMsg){


		case SP5K_MSG_USB_DPS_DISCOVER:
			break;

		case SP5K_MSG_USB_DPS_NOTIFY_JOB_STATUS:

			DBG_PRINT("usb dps pr: >>>>>>APP_CMD_DPS_NOTIFY_JOB_STATUS(APP_STATE_USB_DPS)<<<<<<<<\n");
			pjobStatus = (sp5kDpsJobStatus_t *)((UINT32)param);
			DBG_PRINT("usb dps pr: prtPID[%s]\n",pjobStatus->prtPID);
			DBG_PRINT("usb dps pr: filePath[%s]\n",pjobStatus->filePath);
			DBG_PRINT("usb dps pr: copyID[%s]\n",pjobStatus->copyID);
			DBG_PRINT("usb dps pr: progress[%s]\n",pjobStatus->progress);
			DBG_PRINT("usb dps pr: imagesPrinted[%s]\n",pjobStatus->imagesPrinted);

			if(gDpsSubMenu.curItem== DPS_PRINT_INDEX){
				UINT16 progressT;
				memcpy(str, &pjobStatus->progress[4],3);
				str[3] = 0;
				progressT =(UINT8)atoi(str);
				if(progressT != 0){

					printf("DPS_HOST_MSG_PRINTING_TOTAL\n");
					gDpsInfo.TotalSelPics = progressT;
				}
			}
			memcpy(str, &pjobStatus->progress[0],3);
			str[3] = 0;
			progressN =(UINT8)atoi(str);

			break;
		case SP5K_MSG_USB_DPS_NOTIFY_DEVICE_STATUS:

			DBG_PRINT("usb dps pr: >>>>>>GET_DPS_NOTIFY_DEVICE_STATUS(APP_STATE_USB_DPS)<<<<<<\n");
			pdeviceStatus = (sp5kDpsDeviceStatus_t *)((UINT32)param);
			DBG_PRINT("usb dps pr: ***********dps_NotifyDeviceStatus call****************\n");
			DBG_PRINT("usb dps pr: dpsPrintServiceStatus[0x%x]\n",pdeviceStatus->dpsPrintServiceStatus);
			DBG_PRINT("usb dps pr: jobEndReason[0x%x]\n",pdeviceStatus->jobEndReason);
			DBG_PRINT("usb dps pr: errorStatus[0x%x]\n",pdeviceStatus->errorStatus);
			DBG_PRINT("usb dps pr: errorReason[0x%x]\n",pdeviceStatus->errorReason);
			DBG_PRINT("usb dps pr: disconnectEnable[0x%x]\n",pdeviceStatus->disconnectEnable);
			DBG_PRINT("usb dps pr: capabilityChanged[0x%x]\n",pdeviceStatus->capabilityChanged);
			DBG_PRINT("usb dps pr: newJobOK[0x%x]\n",pdeviceStatus->newJobOK);

			appUsbDps_PrintServiceStatus(pdeviceStatus->dpsPrintServiceStatus,pdeviceStatus->jobEndReason,pdeviceStatus->errorStatus);
			appUsbDps_PrintErrorStatus(pdeviceStatus->errorStatus,pdeviceStatus->errorReason);
			break;

		default:
			break;
	}
}
/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsPrintStatePrintInitProcess
 *------------------------------------------------------------------------*/
void appUsbDps_DpsPrintStatePrintInitProcess()
{
	UINT32 Error;	
	_DpsAddImageProcess();
	if(	gDpsSubMenu.curItem==DPS_PRINT_DPOF)
	{
		appOsdMessageBoxShow(TRUE,STRTK_PRINTING);
		Error = appUsbDps_PrintDpof;
		if(Error == 0x15FF0007)
		{/*print timeout*/
			if( dpsErrType != 0 )
			{
				appUsbDps_PrintErrorMsg(dpsErrType);
			}
			else
			{
				appUsbDps_PrintErrorMsg(0x300);
			}
		}
	}
	else if((gDpsSubMenu.curItem==DPS_PRINT_STANDARD)||(gDpsSubMenu.curItem==DPS_PRINT_INDEX))
	{
		if((gDpsInfo.TotalSelPics)&&(IsSetImage))
		{
			appOsdMessageBoxShow(TRUE,STRTK_PRINTING);
			Error=appUsbDps_PrintStart;
			if(Error == 0x15FF0007)
			{/*print timeout*/
				if( dpsErrType != 0 )
				{
					appUsbDps_PrintErrorMsg(dpsErrType);
				}
				else
				{
					appUsbDps_PrintErrorMsg(0x300);
				}
			}
			IsSetImage=0;
		}
		else
		{		  
			appOsdMessageBoxShow(TRUE,STRTK_NO_JPEG);
			appTimeDelayMs(2000);
			gDpsInfo.dpsModePara=DPS_HALTEDMODE;
			memset(gDpsInfo.PicBuf,0,gDpsImage.totalImg);
			appStateChange(APP_STATE_USB_PRINTMENU,STATE_PARAM_NORMAL_INIT);
		}
	}
}
 
