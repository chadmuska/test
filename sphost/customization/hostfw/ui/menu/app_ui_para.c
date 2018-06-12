/**************************************************************************
 *         Copyright(c) 2012 by iCatch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch Tech-  *
 *  nology Co., Ltd. All rights are reserved by iCatch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of iCatch Technology Co., Ltd.                 *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/

/**
 * @file		app_ui_para.c
 * @brief		App for UI setting parameter
 * @author		Phil Lin
 * @since		2012-09-20
 * @date		2012-09-20
 */
 
#include "app_com_def.h"
#include "app_ui_para.h"
#include "app_still.h"
#include "app_dcf.h"
#include "app_rtc.h"
#include "app_fast_boot.h"
#include "app_battery.h"
#include "app_view_param_def.h"
#include "app_user_setting.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_view_param.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/*Don't modify this value!*/
#define UI_PARAM_SIZE		1024

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define ADF_FILENAME  "A:\\ADF\\ADF.BIN"
#define UDF_FILENAME  "B:\\UDF\\UDF.BIN"
#define MODE_SAVE_WHEN_POWER_OFF 1
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
static UINT8 uiPara[UI_PARAM_SIZE];
static UINT8 uiPara_Base[UI_PARAM_SIZE];


#if 0
UINT8  itemModify;
static UINT8 itemSts[] = {0};
#endif
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
/**
 * @fn        UINT32 appUDFCheckSum(void)
 * @brief     appUDFCheckSum
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    yongsun
 * @since     2012-11-23
 * @todo      N/A
 * @bug       N/A
*/ 
UINT32 appUDFCheckSum(void)
{
	uiPara_t* ptestUiPara;
	UINT32 checksum;
	UINT16 i;

	ptestUiPara = appUiParaGet();

	checksum = 0;
	for( i=0; i<(sizeof(uiPara_t) - 4); i++)
	{
		checksum += *(((UINT8 *)ptestUiPara + i));
	}

	if(ptestUiPara->iCatchFlag[0]=='I'\
		&&ptestUiPara->iCatchFlag[1]=='C'\
		&&ptestUiPara->iCatchFlag[2]=='A'\
		&&ptestUiPara->iCatchFlag[3]=='T'\
		&&ptestUiPara->iCatchFlag[4]=='C'\
		&&ptestUiPara->iCatchFlag[5]=='H'
		)
	{
		if(checksum != ptestUiPara->Checksum)//checksum err
		{
			return FAIL;
		}
		else//check sum ok
		{
			return SUCCESS;
		}
	}
	else{
		return FAIL;
	}
}

/**
 * @fn        UINT32 appUDFLoad(void)
 * @brief     appUDFLoad
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-20
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appUDFLoad(
	void
)
{
	if(ReadFile((UINT8 *)UDF_FILENAME, &uiPara[0], sizeof(uiPara_t))!= SUCCESS) 
	{
		DBG_PRINT("[ERR] read file[%s] fail\n", UDF_FILENAME);
		return FAIL;
	}
	return appUDFCheckSum();
}

/**
 * @fn        UINT32 appADFLoad(void)
 * @brief     appADFLoad
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-20
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appADFLoad(
	void
)
{
	if(ReadFile((UINT8 *)ADF_FILENAME, &uiPara[0], sizeof(uiPara_t))!= SUCCESS) 
	{
		DBG_PRINT("[ERR] read file[%s] fail\n", ADF_FILENAME);
		return FAIL;
	}

	return appUDFCheckSum();
}


//-----------------------------------------------------------------------------
//appUiParaInit
//-----------------------------------------------------------------------------
/**
 * @brief	UI parameter initialization
 * @param	mode:init mode
 * @retval	NULL
 * @see
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/
#if 1 /*mark for Cathylite+ez_menu*/
void appUiParaInit(uiParaInitMode_t mode)
{
	uiPara_t *psPara = (uiPara_t*)&uiPara[0];

	
	if (mode == PARA_INIT_FROM_UDF) {
		//Load UDF from nand reserve block
		psPara->EmptyPara = 0;
		if(appUDFLoad()==SUCCESS)
		{
			appRTCUiParaInit();
			/*appUiParaInit_Print("UDF load!\n");*/
			/*psPara->USBMode = USB_DEFAULT;*/ /*mark for mantis bug 0048653*/
			gStillCB.cap.CurScene = psPara->CurScene;
			gStillCB.lastcap = gStillCB.cap;
			return;
		}
	} 

	if(appADFLoad()==SUCCESS)//UI config tool flow
	{
		appRTCUiParaInit();
	/*appUiParaInit_Print("adf load success\n");*/
	}
	else// FW flow
	{
		appUIParaReset();
		/*appUiParaInit_Print("adf load fail\n");*/
		memcpy(&psPara->iCatchFlag[0], "ICATCH", sizeof(psPara->iCatchFlag));
	}
}
#endif
//-----------------------------------------------------------------------------
//appUIParaSave
//-----------------------------------------------------------------------------
/**
 * @brief	UI parameter save
 * @param	mode:power off mode
 * @retval	NULL
 * @see
 * @author	yongsun
 * @since	2012-11-23
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/

UINT32
appUIParaSave(
	void
)
{
	uiPara_t *psPara = appUiParaGet();
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	view_param_t *pParam = appViewParamGet();
#endif
	UINT16 i;
	
	psPara->Checksum = 0;

	psPara->CurViewMode =gStillCB.cap.CurViewMode;
	psPara->CurScene = gStillCB.cap.CurScene;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	psPara->VideoSize = pParam->videoSize;
#endif
#if !defined(HW_17DVR)
	appDcfKeySettingUpdate(&psPara->dcfDirKey, &psPara->dcfFileKey);
#endif
	//simply add every ui para to checksum
	for( i=0; i<(sizeof(uiPara_t) - 4); i++)
	{
		(psPara->Checksum) += *(((UINT8 *)psPara + i));
	}
	
	if(WriteFile((UINT8 *)UDF_FILENAME, &uiPara[0], sizeof(uiPara_t))!=SUCCESS) 
	{		
		DBG_PRINT("[ERR] write file[%s] fail\n", UDF_FILENAME);
		return FAIL;
	}
	
	appFastBootLensModeSet(psPara->CurViewMode);
	
	appFastBootLensBattSet(psPara->batteryType,appLensBootBatteryLevelGet());
	
	return SUCCESS;

}
void appUiParaRevert2Base(void)
{
	memcpy(&uiPara[0],&uiPara_Base[0],sizeof(uiPara_t));
}

uiPara_t*  appUiParaBaseGet(void)
{
	return(uiPara_t*)&uiPara_Base[0];
}

//-----------------------------------------------------------------------------
//appUiParaGet
//-----------------------------------------------------------------------------
/**
 * @brief	Get ui parameter pointer
 * @param	NULL
 * @retval	ui parameter pointer
 * @see
 * @author	Phil Lin
 * @since	2008-02-20
 * @todo	Something need to be done.
 * @bug		a bug need to be resolved.
*/
uiPara_t* appUiParaGet(void)
{
	return (uiPara_t*)&uiPara[0];
}
//-----------------------------------------------------------------------------
//appRTCUiParaInit
//-----------------------------------------------------------------------------
/**
 * @brief	
 * @param	NULL
 * @retval	
 * @see
 * @author	Phil Lin
 * @since	2014-11-11
 * @todo	Something need to be done.
 * 
*/
void appRTCUiParaInit(void)
{
	uiPara_t* puiPara = appUiParaGet();
	appRTC_t rtc;
	appRtcGet(&rtc);
	printf("\033[36mrtc.year =%d,rtc.mon =%d,rtc.day =%d,rtc.hour =%d,rtc.min =%d\n puiPara->rtcYear =%d,puiPara->rtcMonth =%d,puiPara->rtcDay =%d,puiPara->rtcHour =%d,puiPara->rtcMinute =%d\033[0m\n",\
		rtc.year,rtc.mon,rtc.day,rtc.hour,rtc.min,\
		puiPara->rtcYear,puiPara->rtcMonth,puiPara->rtcDay,puiPara->rtcHour,puiPara->rtcMinute);
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
	if((((rtc.year - 2000)==puiPara->rtcYear)&&((rtc.mon - 1)<puiPara->rtcMonth))||\
	    (((rtc.year - 2000)==puiPara->rtcYear)&&((rtc.mon - 1)==puiPara->rtcMonth)&&((rtc.day - 1)<puiPara->rtcDay))||\
	    (((rtc.year - 2000)==puiPara->rtcYear)&&((rtc.mon - 1)==puiPara->rtcMonth)&&((rtc.day - 1)==puiPara->rtcDay)&&(rtc.hour<puiPara->rtcHour))||\
	    (((rtc.year - 2000)==puiPara->rtcYear)&&((rtc.mon - 1)==puiPara->rtcMonth)&&((rtc.day - 1)==puiPara->rtcDay)&&(rtc.hour==puiPara->rtcHour)&&(rtc.min<puiPara->rtcMinute))||\
	     ((rtc.year - 2000)<puiPara->rtcYear))
	{
		rtc.year = puiPara->rtcYear+2000;
		rtc.mon = puiPara->rtcMonth+1;
		rtc.day = puiPara->rtcDay+1;
		rtc.hour = puiPara->rtcHour;
		rtc.min = puiPara->rtcMinute;
		rtc.sec = 0;
		appRtcSet(&rtc);
		printf("\033[36mrtc lost,use ui para\033[0m\n");
	}
	else
#endif
	{
		puiPara->rtcYear = rtc.year - 2000;
		puiPara->rtcMonth = rtc.mon - 1;
		puiPara->rtcDay = rtc.day - 1;
		puiPara->rtcHour = rtc.hour;
		puiPara->rtcMinute = rtc.min;
		printf("\033[36mrtc ok,use rtc time\033[0m\n");
	}
}

