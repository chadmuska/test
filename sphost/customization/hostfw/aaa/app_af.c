/**************************************************************************
 *
 *       Copyright (c) 2012-2018 by iCatch Technology, Inc.
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

#define HOST_DBG	1
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "app_com_def.h"
#include "app_com_api.h"
#include "api/sp5k_af_api.h"
#include "app_lens_api.h"
#include "api/sp5k_ae_api.h"
#include "api/sp5k_awb_api.h"
#include "app_state_ctrl.h"
#include "app_view_param.h"
#include "app_still.h"
#include "sp5k_sensor_api.h"
#include "app_afalg_api.h"
#include "app_vcm_api.h"

#ifdef VCM_DRIVER_DW9714A
#include "VCM_DrvDW9714A.h"
#endif

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

#define WRITE8(a,b)		*(volatile UINT8*)(a) = (b)
#define WRITE16(a,b)	*(volatile UINT16*)(a) = (b)


static UINT32 isFaceAf = 0;
static UINT16 CurrPos, bg , end;
static UINT16 itvl =1;
static UINT32 afvaluemax = 0, afvaluemin = 0xffffffff;
static UINT16 afvaluemaxpos = 0, afvalueminpos = 0;
static UINT32 afval_test[9];
static BOOL IsBracket = FALSE;
static UINT8 BktInterval= 5,BktSnapNum = 10;
static UINT8 IsFirst = TRUE;
static UINT8 Agc_ISO100 = 0;

UINT8 afend =0;

static UINT8 NosCntTbl[6] = {12, 16, 24, 32, 52, 88};
#define NosSlope(agc)				(UINT32)((UINT32)(NosCntTbl[(agc/Agc_ISO100) + 1] - (UINT32)NosCntTbl[(agc/Agc_ISO100)]) <<6) /*  X1024   and then /16  */
#define CurrCorThr					(*(volatile UINT8 *)0xb0002405)

#define AF_LOG
#ifdef	AF_LOG
static UINT8 *AF_LOG_BUF = NULL, *AF_LOG_REC = NULL;
UINT32 AF_LOG_REC_fd;
#define AF_LOG_BUF_RESET			AF_LOG_BUF = AF_LOG_REC;memset(AF_LOG_BUF, 0x00, 4096 * sizeof(UINT8))
#define AF_LOG_BUF_GET				AF_LOG_REC + strlen((const char *)AF_LOG_REC)
#define AF_LOG_ADD(fmt, args...)	printf("\n");printf(fmt, ##args);sprintf((char *)AF_LOG_BUF_GET, (const char *)"\n");sprintf((char *)AF_LOG_BUF_GET, (const char *)fmt, ##args)
#define AF_LOG_DUMP					sp5kFsFileWrite(AF_LOG_REC_fd, AF_LOG_REC, 4096 * sizeof(UINT8));AF_LOG_BUF_RESET
#define AF_LOG_CLOSE				AF_LOG_DUMP;sp5kFsFileClose(AF_LOG_REC_fd);sp5kFree(AF_LOG_BUF)
#else
#define AF_LOG_ADD					DBG_PRINT
#endif

#ifdef AF_CURVE_SHOW
static UINT32 *AfCurve = NULL;
#define ValueRange					(afvaluemax - afvaluemin)
#define ScanRange					(end - bg)
#define LCD_SIZE_X					320
#define LCD_SIZE_Y					240
#define AF_CURVE_X_Scale			(UINT32)(LCD_SIZE_X/(ScanRange/itvl))
#define AF_CURVE_Y_Scale			(afvaluemax < LCD_SIZE_Y ? 1 : (afvaluemax / LCD_SIZE_Y))

void AfValueCurveNormal(void);


#endif

static UINT8 IsKeyInNosThr = FALSE;

#define RE_1ST_SCAN

//Modified by Brian 08/08/08
/* Z1 ~Z7 Macro*/
static UINT16 afscanInfo[ZOOM_ZS_NUM+1][3] ={
	{FOCUS_0_S + FOCUS_POSITION_OFFSET, FOCUS_0_E + FOCUS_POSITION_OFFSET, FOCUS_0_STEP},
	{FOCUS_1_S + FOCUS_POSITION_OFFSET, FOCUS_1_E + FOCUS_POSITION_OFFSET, FOCUS_1_STEP},
	{FOCUS_2_S + FOCUS_POSITION_OFFSET, FOCUS_2_E + FOCUS_POSITION_OFFSET, FOCUS_2_STEP},
	{FOCUS_3_S + FOCUS_POSITION_OFFSET, FOCUS_3_E + FOCUS_POSITION_OFFSET, FOCUS_3_STEP},
	{FOCUS_4_S + FOCUS_POSITION_OFFSET, FOCUS_4_E + FOCUS_POSITION_OFFSET, FOCUS_4_STEP},
	{FOCUS_5_S + FOCUS_POSITION_OFFSET, FOCUS_5_E + FOCUS_POSITION_OFFSET, FOCUS_5_STEP},
	{FOCUS_6_S + FOCUS_POSITION_OFFSET, FOCUS_6_E + FOCUS_POSITION_OFFSET, FOCUS_6_STEP},
	{FOCUS_7_S + FOCUS_POSITION_OFFSET, FOCUS_7_E + FOCUS_POSITION_OFFSET, FOCUS_7_STEP}
};

#define _NORMAL_AF_		0x01

UINT8 tp_gAFRegionMap3x3[AF_MULTI_SUBWIN_GRID_Y][AF_MULTI_SUBWIN_GRID_X] =
{
{1,	2,	3},
{4, 5,	6},
{7,	8,	9},
};
UINT8 tp_gAFRegionMapSingleSub[AF_SINGLE_SUBWIN_GRID_Y][AF_SINGLE_SUBWIN_GRID_X] =
{
{1,	2,	3},
{4,	5,	6},
};
UINT8 tp_gAFRegionMapDefocus[AF_DEFOCUS_SUBWIN_GRID_X][AF_DEFOCUS_SUBWIN_GRID_Y] =
{
{1, 0,	3},
{0, 5,	0},
{2, 0,	4},
};

UINT32 tp_ulAFAvgVerEdge_G[LENS_LMC_MAPPING_MODE_LB1848][AF_MAX_SUBWIN_GRID_Y] ;
UINT32 tp_ulAFAvgFIREdge_G[LENS_LMC_MAPPING_MODE_LB1848][AF_MAX_SUBWIN_GRID_Y] ;
UINT32 tp_ulAFAvgIIREdge_G[LENS_LMC_MAPPING_MODE_LB1848][AF_MAX_SUBWIN_GRID_Y];
UINT32 tp_ulAFAvgSingleEdge_G[LENS_LMC_MAPPING_MODE_LB1848][AF_MAX_SUBWIN_GRID_Y];
UINT32 tp_afval[5*5];
UINT32 tp_afvalSign[5*5];
UINT32 tp_afvalVert[5*5];
UINT32 tp_afvalContiouneEdge[3*3];

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern 	void tbUtil_DataNormal(UINT32 *pData, UINT32 NumData,UINT32 DataMax, UINT32 DataMin, UINT32 NorMaxX, UINT32 NorMaxY);
extern 	void tbUtil_DataCurveDraw(UINT32 *pData, UINT32 NumData);

extern SINT8 _giFocusStartSearchOfs[FOCUS_TOTAL_DEF_POS];
extern SINT8 _giFocusStopSearchOfs_Macro[FOCUS_TOTAL_DEF_POS];
extern SINT8 _giFocusStopSearchOfs[FOCUS_TOTAL_DEF_POS];
extern SINT8 _giFocusStartSearchOfs_Macro[FOCUS_TOTAL_DEF_POS];
extern SINT8 _giSearchSteps[FOCUS_TOTAL_DEF_POS];
extern SINT8 _giSearchSteps_M[FOCUS_TOTAL_DEF_POS];
extern SINT8 _giSearchSteps_LowLight[FOCUS_TOTAL_DEF_POS];
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

static void AfScanRangeInit()
{
		UINT16 zlevel;

		zlevel=appLensQuery(LENS_QUERY_ZOOM_SEG_POS); //lensZoomCurSegGet();
		#if 0
		if (pViewParam->af==UI_FOCUS_MACRO)
		{
			#if 0
			bg =afscanInfo[ZOOM_ZS_NUM][0];
			end =afscanInfo[ZOOM_ZS_NUM][1];
			itvl = afscanInfo[ZOOM_ZS_NUM][2];
			#else
			bg =afscanInfo[0][1] -20;
			end =afscanInfo[0][1]+ 80;
			itvl = afscanInfo[ZOOM_ZS_NUM][2];
			#endif
			DBG_PRINT("%s: MACRO, zlevel(%d), bg(%d), end(%d), itvl(%d)", __FUNCTION__, zlevel, bg, end, itvl);
		}
		else
		#endif
		{
			#ifdef afskip
			bg =afscanInfo[zlevel][0] - afscanInfo[zlevel][2];
			#else
			bg =afscanInfo[zlevel][0];
			#endif
			end =afscanInfo[zlevel][1];
			itvl = afscanInfo[zlevel][2];
			DBG_PRINT("%s: NORMAL, zlevel(%d), bg(%d), end(%d), itvl(%d)", __FUNCTION__, zlevel, bg, end, itvl);
		}
		DBG_PRINT("af info :(%d) %d %d %d\n", zlevel, bg, end, itvl);
		CurrPos = bg;

}


UINT16 appFocusInitPosGet(UINT8 idx)
{
	UINT16 InitPos=0;
	switch (idx)
	{
		case UI_FOCUS_NORMAL :
			InitPos = ((afscanInfo[appLensQuery(LENS_QUERY_ZOOM_SEG_POS)][1])-20);
			break;
		case UI_FOCUS_MACRO:
			InitPos = afscanInfo[ZOOM_ZS_NUM][1];
			break;
		case UI_FOCUS_INFINITY:
			InitPos = afscanInfo[appLensQuery(LENS_QUERY_ZOOM_SEG_POS)][0];
			break;
		default :
			break;
	}
	return InitPos;
}

UINT16* appFocusInfoGet(void)
{
	UINT16 *p= &afscanInfo[0][0];
	return(p);
}


UINT16 appFocusMaxPosGet()
{
	return (afvaluemaxpos - itvl);
}


UINT8 AfStatusGet()
{
	return afend;
}

void AfStatusSet(UINT8 status)
{
	afend = status;
}
void appFocusModeChange(UINT8 idx)
{
	//appLensCreate();
	appFocusPosGo(appFocusInitPosGet(idx));
	//appFocusStopWait();
	//appLensDestroy();
}

void
appAfValueReset(
	void)
{
	afvaluemax = 0;
	afvaluemin = 0xffffffff;
	afvaluemaxpos= CurrPos = bg =end= itvl = 0;
}

/*
appFocusScanPosGo() waits VD to make moving of focus lens overlap flushing sensor.
This benefits the accuracy of AF value.
*/
UINT32 appFocusScanPosGo(
	SINT16 pos)
{
	UINT32 ret;
	//appFocusStopWait();
	sp5kSensorVvalidWait();
	ret = appFocusPosGo(pos);
	return ret;

}
#if 0
UINT8 NosThrGet(SINT32 exp, UINT32 agc)
{
	agc -= (CurrPos - bg) / 100;
	agc = ((agc > 80) ? 80 : agc);
	UINT32 NosThr = 0;
	if(IsKeyInNosThr){
		NosThr = CurrCorThr;
	}else{
	 	NosThr = (((NosSlope(agc))*(agc - ((agc / Agc_ISO100) * Agc_ISO100)))>> 10) + NosCntTbl[(agc/Agc_ISO100)];
		NosThr = NosThr&0x000000FF;
	}
	AF_LOG_ADD("\n\n NosThrGet(%d, %d)=%d\n",exp, agc, NosThr);
	return NosThr;
}
#else
UINT8 NosThrGet(SINT32 exp, UINT32 agc)
{
	agc = ((agc > 80) ? 80 : agc);
	UINT32 NosThr = 0;
	if(IsKeyInNosThr){
		NosThr = CurrCorThr;
	}else{
	 	NosThr = (((NosSlope(agc))*(agc - ((agc / Agc_ISO100) * Agc_ISO100)))>> 10) + NosCntTbl[(agc/Agc_ISO100)];
		NosThr = NosThr&0x000000FF;
	}
	AF_LOG_ADD("\n\n NosThrGet(%d, %u)=%u\n",exp, agc, NosThr);
	return NosThr;
}

#endif

void aaa_AutoFocus()
{
	SINT32 CurrExp;
	UINT32 CurrAgc;
	UINT8 CurrNos;
	afend = AF_BUSY;
	//appFocusStopWait();
	sp5kAfModeSet(SP5K_AF_MODE_ON);
	appAEPreviewExpAgcGet(&CurrExp, &CurrAgc);
	CurrNos = NosThrGet(CurrExp, CurrAgc);
	sp5kAfCfgSet(SP5K_AF_CORING_THR, CurrNos);
	sp5kAfCfgSet(SP5K_AF_CORING_VALUE, CurrNos);
	#ifdef RE_1ST_SCAN
	IsFirst = TRUE;
	#endif
	/*
	for ensuring the setting of SP5K_AF_CORING_THR
	printf("\n\nSP5K_AF_CORING_THR == %d\n", *(volatile UINT8 *)0xb0002405);
	*/

}

UINT32 appAfInit()
{
	UINT32 ret;

	#ifdef AF_LOG
	if(AF_LOG_BUF != NULL && AF_LOG_BUF != AF_LOG_REC)
	{
		AF_LOG_BUF_RESET;
	}
	else
	{
		AF_LOG_BUF = sp5kMalloc(4096 * sizeof(UINT8));
		memset(AF_LOG_BUF, 0x00, 4096 * sizeof(UINT8));
		AF_LOG_REC = AF_LOG_BUF;
	}
	AF_LOG_REC_fd = sp5kFsFileOpen((const UINT8 *)"D:\\AF___LOG.LOG", SP5K_FS_OPEN_APPEND);
	if(!AF_LOG_REC_fd)
	{
		AF_LOG_REC_fd = sp5kFsFileOpen((const UINT8 *)"D:\\AF___LOG.LOG", SP5K_FS_OPEN_CREATE);
	}
	sp5kFsFileSeek(AF_LOG_REC_fd, 0, SP5K_FS_SEEK_END);
	#endif


	DBG_PRINT("%s ...\n", __FUNCTION__);
	appAaaAfWinSet(&isFaceAf);

	appAeCb_t *paeCb = appAeCbGet();
	Agc_ISO100 = paeCb->pviso100agc;

	AfScanRangeInit();
	//appLensAFCreate();

	ret = appFocusPosGo(CurrPos);

	#ifdef AF_CURVE_SHOW
	AfCurve = sp5kMalloc(((end - bg)/itvl + 1) * sizeof(UINT32));
	memset(AfCurve, 0x00, ((end - bg)/itvl + 1));
	#endif


	return ret;
}

void appFocusScan(UINT32 param)
{
	memset(afval_test, 0x00, sizeof(afval_test));
	/*
	The 1st AF_READY is not relevant, because the exposure is not stated till SP5K_AF_MODE_ON;
	however, it's no way to be predict the timing of executing "sp5kAfModeSet(SP5K_AF_MODE_ON)"
	, and this results in shortening the time of exposure.
	*/
	#ifdef RE_1ST_SCAN
	if(IsFirst == TRUE && CurrPos == bg){
		IsFirst = FALSE;
		return;
	}
	#endif
	if (CurrPos <= end ){
		UINT32 sum_afvalue = 0;
		
		if (sp5kAfWinGet(param, SP5K_AF_STAT_VAL, afval_test)) {
			DBG_PRINT("\n\n!!!!!!!! AfWinGet failed !!!!!!!!!\n\n");
			return;
		}
		if(isFaceAf) {
			sum_afvalue = afval_test[0];
		}
		else {
			UINT8 i;
			for(i = 0; i < 9; i ++) {
				sum_afvalue += afval_test[i];
			}
		}
		sum_afvalue = (sum_afvalue == 0 ? 1:sum_afvalue);

		#ifdef AF_CURVE_SHOW
		*(AfCurve + ((CurrPos - bg)/itvl)) = sum_afvalue;
		#endif

		if (sum_afvalue > afvaluemax){
			afvaluemax = sum_afvalue;
			afvaluemaxpos = CurrPos;
			}
		if (sum_afvalue < afvaluemin){
			afvaluemin = sum_afvalue;
			afvalueminpos = CurrPos;
			}
		//AF_LOG_ADD("%d: %d, %d, %d, %d, %d, %d",CurrPos, afval_test[0], afval_test[1], afval_test[2], afval_test[3], afval_test[4], afval_test[5]);
		AF_LOG_ADD("%d, %d [%d, %d], %d: %u", appLensQuery(LENS_QUERY_ZOOM_SEG_POS), appMsTimeGet(), bg, end, CurrPos, sum_afvalue);

		profLogPrintf(0 , "AF e, %x", param);
		appFocusScanPosGo(CurrPos+itvl);
		CurrPos += itvl;
		}

	if(CurrPos > end ) {
		if (afend == AF_BUSY){
		/*
		Threshold of Success: Max/Min >=2
		*/
			if(afvaluemin >(afvaluemax >> 1) || (appFocusMaxPosGet() + (itvl<<1) >= end)) {
				AF_LOG_ADD("\nAF_FAIL !!\n");
				afend = AF_FAIL;
			}
			else {
				AF_LOG_ADD("\nAF_SUCCESS !!\n");
				afend = AF_SUCCESS;
			}
			if(!(IS_CALIB_GROUP(appActiveStateGet()))){
				if(IsBracket){
					appFocusPosGo(appFocusMaxPosGet() - BktInterval * BktSnapNum);
				}else{
					appFocusPosGo(appFocusMaxPosGet());
				}
			}
		}
		if (appLensQuery(LENS_QUERY_FOCUS_MOTION_STATE) == 1){
			//appFocusStopWait();
		}else
		{
			DBG_PRINT("%d: max pos %d\n", appMsTimeGet(), appFocusMaxPosGet());
			//appLensAFDestroy();
			sp5kAfModeSet(SP5K_AF_MODE_OFF);
			#ifdef AF_LOG
			UINT16 dirkey, filekey;
			sp5kDcfFsNextKeyGet(&dirkey, &filekey);
			AF_LOG_ADD("dirkey = %d, filekey = %d\n", dirkey, filekey);
			AF_LOG_CLOSE;
			#endif
			#ifdef AF_CURVE_SHOW
			AfValueCurveNormal();
			#endif

			}
		}
}



/*
The following functions and commands are designed for AF tests,
,including bracket tests and noise-related tests. These functions
have nothing to do with normal AF operation or calibration.

Brian 2008/09/26.
*/

#define FOCUS_STOP_TAG (~0x00)

#ifdef AF_CURVE_SHOW

void AfValueCurveNormal(void)
{
	tbUtil_DataNormal(AfCurve, ((end - bg)/itvl + 1), afvaluemax, afvaluemin, LCD_SIZE_X, LCD_SIZE_Y);
}

void AfValueCurveDraw(UINT8 focusStatus)
{
	SINT32 CurrExp;
	UINT32 CurrAgc;
	UINT8 *szStr = sp5kMalloc(32 * sizeof(UINT8));

	appAEPreviewExpAgcGet(&CurrExp, &CurrAgc);
	sprintf(szStr, "(%3d,%3u):%2d   (%3d,%3d),%2d,%2d",
					CurrExp, CurrAgc, CurrCorThr, bg, end, appFocusMaxPosGet(), (end - bg)/itvl + 1);
	appOsdLib_TextDraw(0, 0, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szStr);
	sprintf(szStr, "%u--%u", afvaluemin, afvaluemax);
	appOsdLib_TextDraw(0, 32, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szStr);

	if(focusStatus == AF_SUCCESS){
		appOsd_ColorDrawShapeColorSet(12, 4, 12, 4);	//GREEN
	}else
	{
		appOsd_ColorDrawShapeColorSet(18, 4, 18, 4);	//RED
	}
	tbUtil_DataCurveDraw(AfCurve, ((end - bg)/itvl + 1));
	sp5kFree(AfCurve);
	sp5kFree(szStr);
}
#endif

void AfFocusStopTagFind(void){
	UINT32 msg, param;
	profLogAdd(0, "Tag 1");
	do
	{
		sp5kHostMsgReceive(&msg, &param);
		if(SP5K_MSG_AF_READY == msg)
		profLogPrintf(0, "Recv AF Rdy, %x", param);
	}while((param != FOCUS_STOP_TAG) && (msg != SP5K_MSG_AF_READY)) ;

	profLogAdd(0, "Tag 2");
}

static UINT32 AfCurrRdyParamGet(void){
	UINT32 msg, param;
	AfFocusStopTagFind();
	do
	{
		sp5kHostMsgReceive(&msg, &param);
	}while((msg != SP5K_MSG_AF_READY));

	return param;
}
void TestOpenProcessFrame(UINT32 fid)
{
	sp5kAfWinGet(fid,SP5K_AF_STAT_VAL,afval_test);
    #if 0
	printf("R:\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%u\n"
		, afval_test[0],afval_test[1],afval_test[2],afval_test[3],afval_test[4],afval_test[5],afval_test[6],afval_test[7],afval_test[8],afval_test[9]);
    #endif
}

void appAF_ProcessFrameGet(UINT32 fid)
{

	SINT32 uRet = 0 , uRetS = 0 , uRetV =0;
	UINT16 c1=0;

	/*CAF off  & normal-AF off*/
    if(!afALG_IS_CONTINUE_AF_ON() && !afALG_IS_SEARCHING()){
		return;
    }


    if ( !afALG_IS_RUNNING_AF_OPNE_PROCESS_FRAME() )
    {
       	afALG_SET_RUNNING_AF_OPNE_PROCESS_FRAME();
    }

    afALG_Continue_OpenProcessFrameGet( fid );

	memset(tp_ulAFAvgVerEdge_G, 0, sizeof(tp_ulAFAvgVerEdge_G));
	memset(tp_ulAFAvgFIREdge_G, 0, sizeof(tp_ulAFAvgFIREdge_G));
	memset(tp_ulAFAvgIIREdge_G, 0, sizeof(tp_ulAFAvgIIREdge_G));
	memset(tp_ulAFAvgSingleEdge_G, 0, sizeof(tp_ulAFAvgSingleEdge_G));
	memset(tp_afval, 0, sizeof(tp_afval));


	memset(tp_afvalSign, 0, sizeof(tp_afvalSign));
	memset(tp_afvalVert, 0, sizeof(tp_afvalVert));
	uRet  = sp5kAfWinGet(fid,SP5K_AF_STAT_VAL,tp_afval);
	uRetS = sp5kAfWinGet(fid,SP5K_AF_SINGLE_VAL,tp_afvalSign);
	uRetV = sp5kAfWinGet(fid,SP5K_AF_VERT_VAL,tp_afvalVert);

	for(c1= 0; c1 <(_gsFocus.uwSubWinNumX*_gsFocus.uwSubWinNumY) ; c1++)
	{

		if (uRet==0)
        {

	        UINT32 x,y;
			x=c1%(UINT32)_gsFocus.uwSubWinNumX;
			y=c1/ (UINT32)_gsFocus.uwSubWinNumX;
			timeDelay(3,1);
			if(tp_afval[c1] == 0){
				tp_ulAFAvgFIREdge_G[y][x] = 1;
	        		tp_ulAFAvgIIREdge_G[y][x] = 1;
				profLogPrintf(3,"[AF]Contrast-afval 0!!");
			}
			else if(tp_afval[c1] == 0xffffffff){
				tp_ulAFAvgFIREdge_G[y][x] = 0xfffffff0;
	        		tp_ulAFAvgIIREdge_G[y][x] = 0xfffffff0;
				profLogPrintf(3,"[AF]Contrast-afval Max!!");
			}
			else
			{
            			tp_ulAFAvgFIREdge_G[y][x] = tp_afval[c1];
            			tp_ulAFAvgIIREdge_G[y][x] = tp_afval[c1];
			}
         }
         if (uRetS==0)
         {
         	      if(tp_afvalSign == 0){
				/*contrast value can't be zero*/
				tp_ulAFAvgSingleEdge_G[c1/_gsFocus.uwSubWinNumX][c1%_gsFocus.uwSubWinNumX] = 1;
				profLogPrintf(3,"[AF]Contrast-afvalSign 0!!");
        		}
			else if(tp_afvalSign == 0xffffffff){
				tp_ulAFAvgSingleEdge_G[c1/_gsFocus.uwSubWinNumX][c1%_gsFocus.uwSubWinNumX] = 0xfffffff0;
				profLogPrintf(3,"[AF]Contrast-afvalSign Max!!");
			}
			else{
	            		 tp_ulAFAvgSingleEdge_G[c1/_gsFocus.uwSubWinNumX][c1%_gsFocus.uwSubWinNumX]
	             		= (tp_afvalSign[0]);
			}
          }
          if (uRetV==0)
          {
           		if(tp_afvalVert == 0){
				/*contrast value can't be zero*/
				tp_ulAFAvgVerEdge_G[c1/_gsFocus.uwSubWinNumX][c1%_gsFocus.uwSubWinNumX] = 1;
				profLogPrintf(3,"[AF]Contrast-afvalVert 0!!");
        		}
			else if (tp_afvalVert == 0xffffffff){
				tp_ulAFAvgVerEdge_G[c1/_gsFocus.uwSubWinNumX][c1%_gsFocus.uwSubWinNumX] = 0xfffffff0;
				profLogPrintf(3,"[AF]Contrast-afvalVert Max!!");
			}
			else{
				 tp_ulAFAvgVerEdge_G[c1/_gsFocus.uwSubWinNumX][c1%_gsFocus.uwSubWinNumX]
				 = (tp_afvalVert[0]/(_gsFocus.uwSubWinNumX*_gsFocus.uwSubWinNumY));
			}
          }

	}
	afALG_SET_GET_EDGE_READY_ON();
}



void appAF_PowerOnInit(void)
{

	#ifdef _ENABLE_DUMP_AF_PowerOnInit_
	printf("[lens]afALG_PowerOnInit s\n");
	#endif
	sp5kAfInit();
	sp5kAfCfgSet(SP5K_AF_WIN_PATH, SP5K_AF_WIN_PATH_PREPROCESS);

	/*Lens initial*/
	afALG_Lens_PowerOnInit();
	/*ZOOM_Module */

	/*Setting VCM config*/
	#if (LENS_INIT)
	#else
        /*extern UINT32  afALG_VCM_PowerOnInit(void);*/
		afALG_VCM_PowerOnInit();
	#endif

	/*Set VCM to infinity postion while power on */
    //afALG_TISR_MovingDir_Config( FOCUS_DIRECT_PWM_MOVING_ABSOLUTE_POS  , afALG_IntPosGet(), TRUE );
   /* extern void afALG_VCM_GoPosition_No_Watting(SINT32 iPos);*/
    afALG_VCM_GoPosition_No_Watting(afALG_IntPosGet());
	/*install AF call back*/
	sp5kAfCustomCbSet(appAF_ProcessFrameGet);

   	/* create thread, afALG_Focus_Thread */
	#ifdef _ENABLE_DUMP_AF_PowerOnInit_
	printf("[lens]sp5kOsThreadCreate(afALG_Focus_Thread) s\n");
	#endif
	sp5kOsMutexCreate(&_gsFocus.txMutex, "afALG_Focus_Thread", TX_INHERIT);
	sp5kOsEventFlagsCreate(&_gsFocus.txEvent, "afALG_Focus_Thread");
	#if 1

	/* reduce stack size */
	sp5kOsThreadCreate("appFocusThread"
			, afALG_Focus_Thread
			, 0
			, 17	/*4*/  /* avoid CAF to influence DQ thread */
			, 4
			, 3
			, TX_AUTO_START);

	#else
	sp5kOsThreadCreate("afALG_Focus_Thread"
			, afALG_Focus_Thread
			, 0
			, 4
			, 4
			, 3
			, TX_AUTO_START);
	#endif
}


UINT32 appAF_FocusEdgeGet(SINT16 iFocusPos)
{
	if( _gsAFocusSearch.iMultiWinFVidx >= 0)
	{
		UINT32 i=0, j=0;
		UINT32 uMultiAFEdgeSum[AF_MULTI_REGION_TOTAL+1] = {0};
		UINT32 uMultiAFEdgeCnt[AF_MULTI_REGION_TOTAL+1] = {0};

		UINT32 uVerEdgeSum[AF_MULTI_REGION_TOTAL+1] = {0};
		UINT32 uVerEdgeCnt[AF_MULTI_REGION_TOTAL+1] = {0};

		UINT32 uSingleEdgeSum[AF_MULTI_REGION_TOTAL+1] = {0};
		UINT32 uSingleEdgeCnt[AF_MULTI_REGION_TOTAL+1] = {0};

		for(i=0; i<_gsFocus.uwSubWinNumY; i++)
		{
			for(j=0; j<_gsFocus.uwSubWinNumX; j++)
			{
				uMultiAFEdgeSum[tp_gAFRegionMap3x3[i][j]] += tp_ulAFAvgIIREdge_G[i][j];
				uMultiAFEdgeCnt[tp_gAFRegionMap3x3[i][j]] ++;
				uVerEdgeSum[tp_gAFRegionMapSingleSub[i][j]] += tp_ulAFAvgVerEdge_G[i][j];
				uVerEdgeCnt[tp_gAFRegionMapSingleSub[i][j]] ++;
				uSingleEdgeSum[tp_gAFRegionMapSingleSub[i][j]] += tp_ulAFAvgSingleEdge_G[i][j];
				uSingleEdgeCnt[tp_gAFRegionMapSingleSub[i][j]] ++;
			}
		}


		for(i=1; i<=_gsFocus.uwTotalRegion; i++)
		{
			_gsAFocusSearch.uMultiWinFocusEdge[i][_gsAFocusSearch.iMultiWinFVidx] = 0;
			if( uMultiAFEdgeCnt[i] )
			{
				_gsAFocusSearch.uMultiWinFocusEdge[i][(UINT16)_gsAFocusSearch.iMultiWinFVidx] = uMultiAFEdgeSum[i] /uMultiAFEdgeCnt[i];

				_gsAFocusSearch.uOriMultiWinFocusEdge[i][_gsAFocusSearch.iMultiWinFVidx] = _gsAFocusSearch.uMultiWinFocusEdge[i][_gsAFocusSearch.iMultiWinFVidx] ;
			}



			if( uVerEdgeCnt[i] )
			{
				_gsAFocusSearch.uVerticalFocusEdge[1][_gsAFocusSearch.iMultiWinFVidx] = uVerEdgeSum[i] /uVerEdgeCnt[i];
			}

			if( uSingleEdgeCnt[i] )
			{
				_gsAFocusSearch.uSingleFocusEdge[1][_gsAFocusSearch.iMultiWinFVidx] = uSingleEdgeSum[i] /uSingleEdgeCnt[i];
			}


		}

		_gsAFocusSearch.uMultiWindowsCnt ++;
	}
	_gsAFocusSearch.iFocusPos[_gsAFocusSearch.iMultiWinFVidx] = iFocusPos;
	_gsAFocusSearch.iMultiWinFVidx ++;

	return(ERR_NONE);
}

UINT32 appAF_MoveFocusCalculate(SINT32 iFocusPos)
{
   	UINT32  uRet = ERR_NONE ;
   	UINT32 cnt=0;

	afALG_SET_GET_EDGE_READY_OFF();
	afALG_TISR_MovingDir_Config( FOCUS_DIRECT_PWM_MOVING_ABSOLUTE_POS  , iFocusPos , TRUE );

	while(!afALG_IS_GET_EDGE_READY())
	{
	    cnt++;
	    if(cnt > 300 )
	    {
	    	_gsAFocusSearch.cAF_Result = ERR_AFSEARCH_GET_EDGE_READY_ERROR;
	    	break;
	    }
	    sp5kTimeDelay( SP5K_TIME_DELAY_1MS, 1 );
	 }
	appAF_FocusEdgeGet(iFocusPos);

   	return( uRet );

}

UINT32 appAF_EdgeCurveOptimize(S_AFOCUS_SEARCH  *pAFSearch)
{
	SINT32  iBestEdgePosIdx[_gsFocus.uwTotalRegion] ;
	SINT16  iPos =0 ;
	UINT32 i = 0, uEdge=0;


	for (  i =1 ; i <= _gsFocus.uwTotalRegion ; i++ )
	{
		iBestEdgePosIdx[i] = afALG_ParaBolaMaxXFind( pAFSearch->uMultiWindowsCnt, pAFSearch->iFocusPos, pAFSearch->uMultiWinFocusEdge[i], &iPos, &uEdge);
		pAFSearch->iMultiWinBestPos[i]  = iPos ;
		pAFSearch->iMultiWinBestEdge[i] = pAFSearch->uMultiWinFocusEdge[i][iBestEdgePosIdx[i]] ;
		pAFSearch->iMultiWinMinEdge[i] = afALG_MultiWindowsMinEdgeGet(pAFSearch->uMultiWinFocusEdge[i]);
	}

	return (ERR_NONE);
}

SINT16 appAF_BestAFPosDecide(S_AFOCUS_SEARCH  *pAFSearch)
{
	SINT16 iBestPos=0;
 	UINT32 uX=0 ,uY = 0 ;
 	printf("===Start===\n");
 	printf("uMultiWindowsCnt%d\n",_gsAFocusSearch.uMultiWindowsCnt);
 	for( uX = 0; uX <=_gsAFocusSearch.uMultiWindowsCnt ; uX ++ )
 	{
  		printf("pos:%04d", pAFSearch->iFocusPos[uX] );
  		for (uY=1 ; uY<=_gsFocus.uwTotalRegion ; uY++ )
  		{
   			printf("M:%07d ", pAFSearch->uMultiWinFocusEdge[uY][uX]);
  		}
  		printf("V:%07d ", pAFSearch->uVerticalFocusEdge[1][uX]);
  		printf("S:%07d ", pAFSearch->uSingleFocusEdge[1][uX]);
  		printf("\r\n" );
 	}

	printf("INF%d 10cm:%d\n", afALG_OTP_InfAFGet(), afALG_OTP_100mmAFGet());

	appAF_EdgeCurveOptimize(pAFSearch);
	iBestPos = pAFSearch->iMultiWinBestPos[AF_SINGLE_SUB_CENTRAL_REGION_IDX];


	return (iBestPos);

}

UINT32 appAF_FullSearch(SINT16 iStartPos, SINT16 iStopPos, SINT16 iSteps)
{
	UINT32  uRet = ERR_NONE;
	SINT16   iFocusPos;
	SINT16   i, iTotal;

	_gsAFocusSearch.iStartPos = iStartPos;
	_gsAFocusSearch.iStopPos  = iStopPos;
	_gsAFocusSearch.iSteps    = iSteps;
	_gsAFocusSearch.iBestPos  = iStartPos;

	iTotal = ((iStopPos - iStartPos + iSteps-1)/ iSteps);
	_gsAFocusSearch.iStopPos = iStartPos +(iTotal * iSteps);

	afALG_CustomCbStart(_NORMAL_AF_);
	afALG_TISR_MovingDir_Config( FOCUS_DIRECT_PWM_MOVING_ABSOLUTE_POS  , iStartPos - 2 , TRUE );
       afALG_RunningCustomCbWait();


	for( iFocusPos = iStartPos , i = 0; (iFocusPos <= iStopPos ) && ( i < MAX_FOCUS_EDGES ) ; i ++, iFocusPos += iSteps )
	{
	 	uRet = appAF_MoveFocusCalculate(iFocusPos);
	}


	afALG_CustomCbStop(_NORMAL_AF_);

	return (uRet);

}



SINT8 appAF_SearchStepGet(void)
{
	SINT8 	iSteps = 2;
	UINT32	uZoomPosIdx = appZoomCurrentPosGet();

	/*FOCUS_MODE_NORMAL*/
	{
		/*normal AF jump*/
		iSteps = _giSearchSteps[uZoomPosIdx];
	}

	return iSteps;

}
SINT16 appAF_MultiWindowNormalSearch(SINT16 iStartPos, SINT16 iStopPos)
{

	SINT16	iBestPos=0;
	SINT16	iSteps;

	iSteps = appAF_SearchStepGet();

	appAF_FullSearch(iStartPos, iStopPos, iSteps);

	if (!afALG_IsSearchAbort() )
	{
		iBestPos = appAF_BestAFPosDecide(&_gsAFocusSearch);

	}
	return( iBestPos);
}

void appAF_SearchReset(void)
{
   	memset(&_gsAFocusSearch, 0, sizeof(S_AFOCUS_SEARCH));
	_gsAFocusSearch.uZoomPosIdx = appZoomCurrentPosGet();
   	_gsAFocusSearch.iDelayVD = 0 ;
   	_gsAFocusSearch.iFVidx = (-1)*(_gsAFocusSearch.iDelayVD);
	/*vance closed*/
	//_gsAFocusSearch.iMultiWinFVidx = (-1)*( _gsAFocusSearch.iDelayVD ) ;
	_gsAFocusSearch.iMultiWinFVidx = 0 ;
   	_gsAFocusSearch.iFPidx = 0;
   	_gsAFocusSearch.cAF_Result = ERR_NONE;
   	_gsAFocusSearch.cSign = 1;
   	_gsAFocusSearch.cPreviousSign = 1;
   	_gsAFocusSearch.cSignCount = 0;
   	_gsAFocusSearch.uBolaCnt = 0;
   	_gsAFocusSearch.cMaxCntDown = 2;
   	afALG_USE_IIR_FILTER_EDGE();
   	afALG_DISABLE_LOW_LIGHT_CLOSE_FAF();
}

void appAF_GeometrySet(void){
		sp5kAfWinSize_t afwin = {0};
		afALG_RoiWindowGet(&afwin.hoffset, &afwin.voffset,  &afwin.hsize, &afwin.vsize);
		printf("[afwin] hoffset=%d,voffset=%d,hsize=%d,vsize=%d \n",afwin.hoffset,afwin.voffset,afwin.hsize,afwin.vsize);

		afwin.hoffset=80;
		afwin.voffset=80;
		afwin.hsize = (afwin.hsize / AF_MULTI_SUBWIN_GRID_X);
		afwin.hsize-= (afwin.hsize%4); // V35 ROI size should 4N
		afwin.vsize = (afwin.vsize / AF_MULTI_SUBWIN_GRID_Y);
		afwin.vsize-= (afwin.vsize%4); // V35 ROI size should 4N
		afwin.hwincnt = AF_MULTI_SUBWIN_GRID_X;
		afwin.vwincnt = AF_MULTI_SUBWIN_GRID_Y;
		afwin.hintvl = 48;// V35  should 4N
		afwin.vintvl = 48;// V35  should 4N

		_gsFocus.uwRegionType = FOCUS_REGION_3x3;
		_gsFocus.uwTotalRegion = AF_MULTI_REGION_TOTAL;

		 /*update ROI info*/
		 _gsAFocusSearch.uwWinX = afwin.hoffset;
		 _gsAFocusSearch.uwWinY = afwin.voffset;
	 	_gsFocus.uwSubWinNumX = afwin.hwincnt;
	 	_gsFocus.uwSubWinNumY = afwin.vwincnt;
		 #if 1
	 	printf("\nafwin.hwincnt=%u;\n",afwin.hwincnt);
	 	printf("afwin.vwincnt=%u;\n",afwin.vwincnt);
	 	printf("afwin.hoffset=%u;\n",afwin.hoffset);
	 	printf("afwin.voffset=%u;\n",afwin.voffset);
	 	printf("afwin.hsize=%u;\n",afwin.hsize);
	 	printf("afwin.vsize=%u;\n",afwin.vsize);
	 	printf("afwin.hintvl=%u;\n",afwin.hintvl);
	 	printf("afwin.vintvl=%u;\n\n",afwin.vintvl);
		 #endif
		sp5kAfWinSizeSet(&afwin);
}

UINT32 appAF_StatisticsSet (void){
	#if 1
	/*setting Vertical-window parameter*/
	sp5kAfCfgSet(SP5K_VAF_STAT_MODE, SP5K_AF_STAT_G);
	sp5kAfCfgSet(SP5K_VAF_CFG_GAMMA_TYPE, 2);
	sp5kAfCfgSet(SP5K_VAF_GAIN, 64);
	sp5kAfCfgSet(SP5K_VAF_CORING_THR, 0);
	sp5kAfCfgSet(SP5K_VAF_CORING_VALUE, 0);
	sp5kAfCfgSet(SP5K_VAF_NOISE_THR, 0);
	#endif
	/*setting Multi-window parameter*/
	sp5kAfCfgSet(SP5K_AF_STAT_MODE, SP5K_AF_STAT_G);
	sp5kAfCfgSet(SP5K_AF_CFG_GAMMA_TYPE, 2);
	sp5kAfCfgSet(SP5K_AF_GAIN, 64);
	sp5kAfCfgSet(SP5K_AF_CORING_THR, 0);
	sp5kAfCfgSet(SP5K_AF_CORING_VALUE, 0);
	sp5kAfCfgSet(SP5K_AF_NOISE_THR, 0);
	sp5kAfCfgSet(SP5K_AF_SINGLE_MODE, 1);
	sp5kAfCfgSet(SP5K_AF_SINGLE_HPF, 0);
	sp5kAfCfgSet(SP5K_AF_SINGLE_MEDIAN, 0);
	sp5kAfCfgSet(SP5K_AF_SINGLE_LPF, 9);
	sp5kAfCfgSet(SP5K_AF_CFG_FILTER_WEIGHT, 0x80000107);
}

SINT16 appAF_FocusDo(UINT32 *uEdgeValue)
{
	SINT16			iStartPos, iStopPos, iSteps = 2, iBestPos = 0,temp_pos=0;
	UINT32 			uSearchRangeRet;
	SINT8 			temp=0;
	UINT8			  uAELock_flag=FALSE,uAWBLock_flag=FALSE,uWaitCount=0;

	appAF_SearchReset();
	uSearchRangeRet =afALG_SearchRangeGet(&iStartPos, &iStopPos);

	afALG_AFArea_Set(AF_AREA_MULTI);
	appAF_GeometrySet();/*AF WIN */
	appAF_StatisticsSet(); /*AF HW */
	afALG_BeginSearch();
	switch(afALG_ModeGet())
	{
		case FOCUS_MODE_INFINITY_FIXED:
			iBestPos=afALG_IntPosGet();
			break;

		default:
		case FOCUS_MODE_NORMAL:
			iBestPos =appAF_MultiWindowNormalSearch(iStartPos, iStopPos);
			break;
	}

	afALG_TISR_MovingDir_Config( FOCUS_DIRECT_PWM_MOVING_ABSOLUTE_POS  , iBestPos , TRUE );

	afALG_EndSearch();

	if(uEdgeValue)
	{
		*uEdgeValue = _gsAFocusSearch.uBestEdge;
	}

	printf("iBestPos=%d\n",iBestPos);
	return(iBestPos);

}
void appAF_ContrastGet(UINT32 num){
	int n=0,p=0;

	appAF_GeometrySet();/*AF WIN */
	appAF_StatisticsSet(); /*AF HW */
	afALG_CustomCbStart(_NORMAL_AF_);

	afALG_SET_SEARCHING_ON() ;

	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();
	sp5kSensorVvalidWait();
	for(p=0;p<num;p++){
		int cnt=0;
		afALG_SET_GET_EDGE_READY_OFF();


		while(!afALG_IS_GET_EDGE_READY()){
			cnt++;
		    if(cnt > 300){
		    	_gsAFocusSearch.cAF_Result = ERR_AFSEARCH_GET_EDGE_READY_ERROR;
		    	printf("[AF]GetEdgeOverTime 300ms \n");
		   		break;
		    }
			sp5kTimeDelay( SP5K_TIME_DELAY_1MS, 1 );
		}

		printf("[%d]cnt=%d\n",p,cnt);

		printf("Multi:\n");
		for(n=0;n<9;n++){
			if((n%3==0) &&( n!=0)){
				printf("\n");
			}
			printf("%u  ,",tp_afval[n]);
		}
		printf("\n");
	}
	afALG_SET_SEARCHING_OFF();

	/*Close AF Mode*/
	afALG_CustomCbStop(_NORMAL_AF_);
}
void appAF_initCB(void){
	sp5kAfCustomCbSet(appAF_ProcessFrameGet);
}
void
cmdAF(
	UINT32 argc,
	UINT8 *arg[],
	UINT32 v[]
)
{
	if (argc==1) {
		printf("af usage:\n"
			" af vcm step:0~1023 "
			" af trigger : need initial appAF_PowerOnInit() "
			" af contrast time:0~100\n"
			" af win 0|1 |2|4: show afwin \n"
			#if 0
			" af info 0|1 : disable/enable AF info\n"
			" af exp IDX : set preview exp\n"
			" af agc IDX : set preview agc\n"
			" af cexp IDX : set capture exp\n"
			" af cagc IDX : set capture agc\n"
			" af report : show aeInitReport \n"
			" af freq 0|1 : control freqDbg \n"
			#endif
		);
	}
	else if(strcmp((const char *)arg[1], "nosthrget") ==0){

			v[2] = ((v[2] > 80) ? 80 : v[2]);
			appAeCb_t *paeCb = appAeCbGet();
			Agc_ISO100 = paeCb->pviso100agc;

			UINT32 v1 = ((UINT32)v[2]);
			UINT32 remainder = v1 - ((v1 / Agc_ISO100) * Agc_ISO100);
			UINT32 slop = NosSlope(v[2]);
			UINT32 NosThr = ((slop*remainder)>> 10) + NosCntTbl[(v[2]/Agc_ISO100)];
			printf("Slop = %u \n", slop);
			printf("((UINT8)v[2]) = %u, (v1 mod( Agc_ISO100)) = %u\n", v1, remainder);
			printf("NosThrGet(EXP = n/a, AGC = %u) = %u\n", v[2], NosThr);
	}else if(strcmp((const char *)arg[1], "nosthrset") ==0){
		if(strcmp((const char *)arg[2], "auto")==0) {
			IsKeyInNosThr = FALSE;
			return;
		}else{
			IsKeyInNosThr = TRUE;
			sp5kAfCfgSet(SP5K_AF_CORING_THR, v[2]);
		}

	}else if(strcmp((const char *)arg[1], "focusae") ==0){
		UINT32 NumAFGet = 0, StepIntval = 0;
		UINT8 i;
		SINT32 CurrExp;
		UINT32 CurrAgc;

		appAeEnable(1);
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 200);

		if(argc == 5 && v[2] >= 100 && v[4] >= 1)	//Continuously get AF value within a specific range.
				{
					StepIntval = v[4];
					NumAFGet = (v[3] - v[2])/v[4];
				}else{
					return;
				}

				for(i = 0; i <= NumAFGet; i++)
				{
					printf("focus [%d] --> [%d], ", appLensQuery(LENS_QUERY_FOCUS_STEP_POS), v[2] + (StepIntval * i));
					if(StepIntval!= 0 || appLensQuery(LENS_QUERY_FOCUS_STEP_POS) != v[2]) {
					//lensFocusJob( v[2] + (StepIntval * i));
					appAEPreviewExpAgcGet(&CurrExp, &CurrAgc);
					printf("EXP/ AGC = %d, %u\n", CurrExp, CurrAgc);
					}
				}


	}
}

BOOL IsBracketMode(void){
	return IsBracket;
}
UINT8 appBracketIntervalGet(void){
	return BktInterval;
}
UINT8 appBracketSnapNumGet(void){
	return BktSnapNum;
}

#define FOCUSSTART 0
#define FOCUSPROCCESSING 1
#define FOCUSSTOP 2
#define VDDELAY 3
#define MAXSTEPCNT 100
#define SMALLSTEP 10
static UINT32 afCustomStatus=FOCUSSTOP;
static UINT32 afVdDelay=VDDELAY;
static UINT32 afPeakIdx = 0;
static UINT32 afPosIdx = 0;
static UINT32 afPos[MAXSTEPCNT]={0};
static UINT32 afCv[MAXSTEPCNT]={0};
static UINT8 bAfInit=0;
static UINT32 afCvTemp;
static UINT8 PosIdx=0;
static UINT16 CurrentPos , StopPos;

static void afCustomCB(UINT32 pid)
{
	UINT32 afwin[3][3];

	sp5kAfWinGet(pid, SP5K_AF_G, afwin);


	if(afCustomStatus==FOCUSSTART)
	{
		afVdDelay--;

		if(afVdDelay==0)
		    {
			afCustomStatus=FOCUSPROCCESSING;
                    afVdDelay=VDDELAY;
		    }
	}
	else if(afCustomStatus==FOCUSPROCCESSING)
	{
                if(afVdDelay>0)
                {
                    afVdDelay--;
                    return;
                }

		/*Center AF Only*/
		afCv[afPosIdx]=afwin[1][1];

		/*Peak Estimation s*/
		if(afCv[afPosIdx]>afCv[afPeakIdx])
			afPeakIdx=afPosIdx;

		if(afPosIdx>=4 &&afCv[afPosIdx]*100<afCv[afPosIdx-1]*98&&afCv[afPosIdx-1]*100<afCv[afPosIdx-2]*98&&afCv[afPosIdx-2]*100<afCv[afPosIdx-3]*98&&afCv[afPosIdx-3]*100<afCv[afPosIdx-4]*98)
			{
			    afCustomStatus=FOCUSSTOP;
                sp5kHostMsgSend(SP5K_MSG_AF_READY,AF_SUCCESS);

				return;
                };
		/*Peak Estimation e*/

			/*Go Small Step*/
			CurrentPos+=SMALLSTEP;
        
			if(CurrentPos>=StopPos)
			{
			    afCustomStatus=FOCUSSTOP;
                sp5kHostMsgSend(SP5K_MSG_AF_READY,AF_SUCCESS);

				return;
                };
#if 0          
			VCM_DrvDW9714A_GoPosition_No_Wait(CurrentPos);//No delay in AF CB
#endif
			PosIdx++;

			/*Set Current Position*/
			afPos[PosIdx]=CurrentPos;
            afVdDelay=VDDELAY;

		afPosIdx++;
	}
}

/***********************************************************************/
/*do appappaf_start() in appStillAaaStateOnMsg() ...if(appStillAaa_af_on == 1)          */
/***********************************************************************/
void appaf_start()
{
		/*AF/VCM Init*/
		if(bAfInit==0)
		{
			sp5kAfInit();
#if 0            
			VCM_DrvDW9714A_PowerOnInit();
#endif
			bAfInit=1;
		}

		/*AF Window ROI Init*/
		cdspAfwinSize_t afParam = {3, 3, 80, 80, 256, 256, 48, 48};
		sp5kAfWinSizeSet(&afParam);

		/*AF Curve Parameter Init*/
		sp5kAfCfgSet(SP5K_AF_STAT_MODE,SP5K_AF_STAT_G_SQR);
		sp5kAfCfgSet(SP5K_AF_CFG_GAMMA_TYPE,2);
		sp5kAfCfgSet(SP5K_AF_GAIN,64);        
		sp5kAfCfgSet(SP5K_AF_CORING_THR,0);       
		sp5kAfCfgSet(SP5K_AF_CORING_VALUE,0);       
		sp5kAfCfgSet(SP5K_AF_NOISE_THR,0);    
		sp5kAfCfgSet(SP5K_AF_SINGLE_MODE,1);    
		sp5kAfCfgSet(SP5K_AF_SINGLE_HPF,0);   
		sp5kAfCfgSet(SP5K_AF_SINGLE_MEDIAN,0);  
		sp5kAfCfgSet(SP5K_AF_SINGLE_LPF,9);          
		sp5kAfCfgSet(SP5K_AF_CFG_FILTER_WEIGHT,0x80000107);   

		/*AF Callback Register*/
		sp5kAfCustomCbSet(afCustomCB);

		afVdDelay=VDDELAY;
		afPeakIdx=0;
		afPosIdx=0;
		PosIdx=0;
		memset(afPos,0,sizeof(afPos));
		memset(afCv,0,sizeof(afCv));
		afPos[PosIdx]=CurrentPos=AF_VCM_PARA_START_CURRENT_DEGREE_LEVEL;
		StopPos=AF_VCM_PARA_STOP_CURRENT_DEGREE_LEVEL;

		/*Go To  Infinity*/
#if 0           
		VCM_DrvDW9714A_GoPosition(CurrentPos);
#endif
		/*AF Callback ON*/
		sp5kAfModeSet(SP5K_AF_MODE_ON);

		afCustomStatus=FOCUSSTART;

}
/***********************************************************************/
/*do appaf_end() in appStillAaaStateOnMsg() .......SP5K_MSG_AF_READY                  */
/***********************************************************************/
void appaf_end()
{
		UINT8 i=0;
		/*Go To Peak Pos  - afPos[afPeakIdx]*/
#if 0        
		VCM_DrvDW9714A_GoPosition(afPos[afPeakIdx]);
#endif
		for(i=0;i<MAXSTEPCNT;i++)
			printf("%u:%10u\n",afPos[i],afCv[i]);

		printf("Focus Pos=%u , CV=%u\n",afPos[afPeakIdx],afCv[afPeakIdx]);

		/*AF Callback OFF*/
		sp5kAfModeSet(SP5K_AF_MODE_OFF);
}