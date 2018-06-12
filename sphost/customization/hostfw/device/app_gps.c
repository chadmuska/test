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
 *	File name    : App_gps.c
 *	Function     : About GPS implementation.
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.07  Edward.Lu  "Add description and modify code"
 **************************************************************************/
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_os_api.h"
#include "sp5k_global_api.h"
#include "sp5k_utility_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_media_api.h"

#include "app_gps.h"
#include "app_rtc.h"
#include "app_osd_api.h"
#if GSENSOR
#include "app_gsensor.h"
#endif

#include "app_menu_frameosd.h"
extern UINT32 RTCTimer;

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define HOST_DBG 0
#define GPS_DBG_NULL_FUNC	do {} while(0)

#if HOST_DBG
#define	DBG_GPS_PROF DBG_PRINT
#else
#define DBG_GPS_PROF(str, arg...)  GPS_DBG_NULL_FUNC
#endif

#define UART_PIN_TX         28
#define UART_PIN_RX         29

#define GPS_INFO_GRP		6
#define GPS_INFO_LINE_MAX	128

UINT8 stamp_GPSSpeedBuf[20];
UINT8 PositionFixIndicator_flag=1;
UINT8 gps_Status_Invalid_flag=1;//A=data valid or V=data not valid

#if LATITUDE
UINT8 stamp_GPS_NSIndicatorBuf[128]; //Latitude
UINT8 stamp_GPS_EWIndicatorBuf[128]; //Latitude
#endif

UINT8 HL_gpsStatus_flag=0;
UINT8 HL_gps_isexist_flag=0;

UINT8 gps_update_RtcTime_flag=0;
UINT8 RTCTimeDataCheckCout=0;
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
SP5K_EVENT_FLAGS_GROUP Uart_sig;

static UINT8 sUartBuff[GPS_INFO_GRP][GPS_INFO_LINE_MAX+3]={\
{"$GPGGA, 161229.487, 3723.2475, N, 12158.3416, W, 1, 07, 1.0, 9.0, M.0000*18"},\
{"$GPRMC, 161229.487, A, 3723.2475,N, 12158.3416, W, 0.13, 309.62, 120598,,*10"},\
{"$GPVTG, 309.62, T, M, 0.13, N, 0.2, K*6E"},\
{"$GPGSV, 2, 1, 07, 07, 79, 048, 42, 02, 51, 062, 43, 26, 36, 256, 42, 27, 27, 138, 42*71"},\
};

static UINT8 * pUart = &sUartBuff[0][0];
static UINT8 uartBuffGrpIdx;

gpsInfo_t gGpsInfo;
static UINT8 gga[GPS_INFO_LINE_MAX+3];
static UINT8 rmc[GPS_INFO_LINE_MAX+3];
static UINT8 vtg[GPS_INFO_LINE_MAX+3];
static UINT8 gsv[GPS_INFO_LINE_MAX+3];
static UINT8 gsv1[GPS_INFO_LINE_MAX+3];
static UINT8 gsv2[GPS_INFO_LINE_MAX+3];
static UINT8 gsv3[GPS_INFO_LINE_MAX+3];

#ifdef SUPPORT_GNSS_MSG
static UINT8 gnss_rmc[GPS_INFO_LINE_MAX+3];
#endif

SP5K_MUTEX GPSGSVmutex;
//static UINT8 tempgsv[GPS_INFO_LINE_MAX+3];
SP5K_MUTEX GPSRMCmutex;

#ifdef SUPPORT_GNSS_MSG
SP5K_MUTEX GNRMCmutex;
gnss_rmc_recieved_flag=0;
volatile UINT8 gsv_mode=0;
UINT8 gnss_active=0;
UINT32 gnss_lost_count=0;
volatile UINT8 gnss_test_mode=0;
#endif

UINT8 gsvtotalMessagenumber=0;
UINT8 CurrentMessageIdex=0;
UINT8 gsvcount=0;
volatile UINT8 SatellitesinViewCount;
UINT8 SatellitesinViewReadyProcCount;

UINT8 gPath[128];
UINT32 usrDataCnt;
UINT8 udatWriteSts;
extern UINT8 GPSledflag;
extern UINT8 *GpsTempBuff;
extern UINT8 *GpsTempPos;
extern UINT8 LastFileNotRellyClose;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

static UINT8 HL_getSettingDataGPSSTATUS(void){
	return pUiSetting->GPSSTATUS;
}

static void HL_Video_view_GPS_icon_show(UINT8 OnOff){
	if(OnOff)
	{
		appOsd_FontIconFontSet(ID_FONT_TK_ICON);
		appOsdLib_OsdLayIconDraw(4+(38*2), 6,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_16_IN_1);
	}
	else
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 4+(38*2), 6,36, 32, 0, 0);
	}
}

void appGPSGSVInfoInit(){
	UINT8 buf2[128]={0};

	APP_OSD_REFRESH_OFF;
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0,320, 240, 0, 0);

	appOsd_ColorDrawShapeColorSet(PAL_RED, PAL_BLEND_100, PAL_RED, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,0,TKTYPE_SETUP_MENU_SETLSTART_Y+32,320,2, 0, 0);

	#if 0
	appOsd_FontIconFontSet(ID_FONT_TK_FONT1);
	appOsdLib_OsdLayIconDraw(TKTYPE_SETUP_MENU_SETSTART_X, TKTYPE_SETUP_MENU_SETLSTART_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_56X40_TAB_SETUP);
	#endif
	sprintf(buf2,"total:%2d ",0);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(20, 40, SP5K_GFX_ALIGN_TOP_LEFT, buf2);
	APP_OSD_REFRESH_ON;
}

void appGPSGSVInfoStateOnMsg(UINT32 msg,UINT32 param){
	switch ( msg) {
    	case SP5K_MSG_TIMER:
    		if(param == RTCTimer){
    			sp5kHostMsgSend(APP_UI_MSG_GPS_GSVINFOUPDATE, 0);
    			}
    		break;
    	case APP_UI_MSG_GPS_GSVINFOUPDATE:
    		#if GPS_OPEN
    		HL_appGpsInfoParse();
    		#endif
    		break;
    	default:
    		break;
	}
}

void appGPSGSVInfoStateOnKey(UINT32 msg,UINT32 param){
	switch(msg){
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
	case APP_KEY_PRESS_MODE:
		sp5kHostMsgSend(APP_STATE_MSG_INIT, 0);
		break;
	case APP_KEY_PRESS_SET:
		appTimerClear(&RTCTimer);
		//appStateChange(APP_STATE_VIDEOSETUP_SUBMENU,STATE_PARAM_NORMAL_INIT);
		break;
	default:
		break;
	}
}

void appGPSGSVINFOState(UINT32 msg, UINT32 param){
	switch(msg){
	case APP_STATE_MSG_INIT:
		appGPSGSVInfoInit();
		if((RTCTimer == NULL) || (RTCTimer == TIMER_NULL)){
			appTimerClear(&RTCTimer);
			RTCTimer = appTimerSet(1000,"RTCTimer");
		}
		break;
	case APP_STATE_MSG_CLOSE:
		CLEAR_OSD_SRC;
		appTimerClear(&RTCTimer);
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appGPSGSVInfoStateOnKey(msg,param);
		}else
		{
			appGPSGSVInfoStateOnMsg(msg,param);
		}
		break;
	}
}

 
/*-------------------------------------------------------------------------
 *  File Name : appUserDateWriteTempBuff
 *------------------------------------------------------------------------*/ 
void appUserDateWriteTempBuff(UINT8 *ptr,UINT32 len)
{
	if((GpsTempPos-GpsTempBuff+len)>appGpsTempBuffSizeGet())
	{
		return;
	}
	if(ptr!=NULL)
	{
		if(len)
		{
			strncpy(GpsTempPos,ptr,len);
			GpsTempPos=GpsTempPos+len;
			GPrintf("W2TB");
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appTempBuff2Mov
 *------------------------------------------------------------------------*/
void appTempBuff2Mov(UINT8*ptr)
{	
	if(ptr!=NULL)
	{
		UINT32 len;
		len=strlen(ptr);
		if(len)
		{
			sp5kMediaRecUserDataUpdate(GpsTempBuff, len, 0, 1);
			GPrintf("TB2MOV");
		}
	}
	LastFileNotRellyClose=3;
	memset(GpsTempBuff,0,appGpsTempBuffSizeGet());/*20K*/
	GpsTempPos=GpsTempBuff;
}

/*-------------------------------------------------------------------------
 *  File Name : HL_GPS_STATUS_LED_GPSIcon_init
 *------------------------------------------------------------------------*/
void HL_GPS_STATUS_LED_GPSIcon_init(void)
{
    if(HL_getSettingDataGPSSTATUS()==1)//gps  setup on
    {
        #if 0
        appStatusLightCtrl(1);
        #endif

        HL_Video_view_GPS_icon_show(1);
    }
    else
    {
        #if 0
        appStatusLightCtrl(0);
        #endif
        HL_Video_view_GPS_icon_show(0);
    }
}

/*-------------------------------------------------------------------------
 *  File Name : HL_GPS_STATUS_LED_GPSIcon_Proc
 *------------------------------------------------------------------------*/
void HL_GPS_STATUS_LED_GPSIcon_Proc(void)
{
	if(HL_getSettingDataGPSSTATUS()==1)//gps  setup on
	{
		if(GPSledflag==0)
		{
			GPSledflag=1;
			#if 0
		   	appStatusLightCtrl(1);
		   	#endif
		  	HL_Video_view_GPS_icon_show(1);
		}
		else
		{
			GPSledflag=0;
			#if 0
		   	appStatusLightCtrl(0);
		   	#endif
		   	HL_Video_view_GPS_icon_show(0);
		}
	}
}

/*****************************************************************************************/
/* appRTC_t tt_time;
/*appTimezone(&tt_time,zone);
/*tt_time------ is the time of zero time zone.  zone is The local time zone;0-24     W12-W1 0 E1-E12
/*****************************************************************************************/
/*-------------------------------------------------------------------------
 *  File Name : appTimezone
 *------------------------------------------------------------------------*/
void appTimezone(appRTC_t *ptime,UINT8 zone)
{
	 UINT8 zonetime=0;
	 UINT8  daymax;

 	//printf("time zone==%d:   gps receive time=%4d-%2d-%2d  %2d:%2d:%2d\n",zone,ptime->year ,ptime->mon,ptime->day,ptime->hour,ptime->min,ptime->sec);

    daymax = 31;
    if ((ptime->mon == 4) || (ptime->mon == 6) || (ptime->mon == 9) || (ptime->mon == 11))
    {
    daymax = 30;
    }
    else if(ptime->mon == 2)
    {
        if ( (((ptime->year % 4) == 0) && ((ptime->year% 100) != 0)) || ((ptime->year % 400) == 0) )
        {
            daymax = 29;
        }
        else
        {
            daymax = 28;
        }
    }

    if(zone>=12)
    {
        zonetime=zone-12;
        ptime->hour +=zonetime;
        //printf("@@----- ptime->hour=%d   ptime->day=%d \n", ptime->hour,ptime->day);
     
        if(ptime->hour>=24)
        {
            ptime->hour-=24;
            ptime->day++;
            
            if(ptime->day>daymax)
            {
                ptime->day=1;
                ptime->mon++;
                
                if(ptime->mon>12)
                {
                    ptime->mon=1;
                    ptime->year++;
                }

             }
        }
    }
    else
    {

        zonetime=12-zone;
        if(ptime->hour>=zonetime)
        {
            ptime->hour-=zonetime;
        }
        else
        {
            ptime->hour=24+ptime->hour-zonetime;
            if(ptime->day>1)
            {
                ptime->day--;
            }
            else
            {
                ptime->day=31;
                if(ptime->mon>1)
                {
        	        ptime->mon--;
                }
                else
                {
                    ptime->mon=12;
                    ptime->year--;
                }
            }    

            daymax = 31;
            if ((ptime->mon == 4) || (ptime->mon == 6) || (ptime->mon == 9) || (ptime->mon == 11))
            {
                daymax = 30;
            }
            else if(ptime->mon == 2)
            {
                if ( (((ptime->year % 4) == 0) && ((ptime->year% 100) != 0)) || ((ptime->year % 400) == 0) )
                {
                    daymax = 29;
                }
                else
                {
                    daymax = 28;
                }
            }
            
            if(ptime->day>=daymax)
            {
                ptime->day=daymax;
            }
        }
    }
    //printf(" end --zone==%d:  update rtc time=%4d:%2d:%2d:%2d:%2d:%2d\n",zone,ptime->year ,ptime->mon,ptime->day,ptime->hour,ptime->min,ptime->sec);

}

/*-------------------------------------------------------------------------
 *  File Name : HL_GPS_time_string_ASCII_Get
 *------------------------------------------------------------------------*/
void HL_GPS_time_string_ASCII_Get(appRTC_t *ptime)
{
	ptime->year=(gGpsInfo.rmcInfo.gprmcDate[0]-0x30)*1000+(gGpsInfo.rmcInfo.gprmcDate[1]-0x30)*100+(gGpsInfo.rmcInfo.gprmcDate[6]-0x30)*10+(gGpsInfo.rmcInfo.gprmcDate[7]-0x30);
	ptime->mon=(gGpsInfo.rmcInfo.gprmcDate[4]-0x30)*10+(gGpsInfo.rmcInfo.gprmcDate[5]-0x30);
	ptime->day=(gGpsInfo.rmcInfo.gprmcDate[2]-0x30)*10+(gGpsInfo.rmcInfo.gprmcDate[3]-0x30);;
	ptime->hour=(gGpsInfo.rmcInfo.gprmcTime[0]-0x30)*10+(gGpsInfo.rmcInfo.gprmcTime[1]-0x30);
	ptime->min=(gGpsInfo.rmcInfo.gprmcTime[2]-0x30)*10+(gGpsInfo.rmcInfo.gprmcTime[3]-0x30);
	ptime->sec=(gGpsInfo.rmcInfo.gprmcTime[4]-0x30)*10+(gGpsInfo.rmcInfo.gprmcTime[5]-0x30);
}

/*-------------------------------------------------------------------------
 *  File Name : strSkipSpace
 *------------------------------------------------------------------------*/
void
strSkipSpace(
	UINT8 **pptr,
	UINT8 c
)
{
	UINT8 *ptr;

	ptr=*pptr;

	while(*ptr!='\0'){
		if(*ptr == c){
			ptr++;
		}else{
			break;
		}
	}
	*pptr = ptr;
}

/*-------------------------------------------------------------------------
 *  File Name : appGpathSave
 *------------------------------------------------------------------------*/
void appGpathSave(void)
{
	memset(gPath, 0 , sizeof(gPath));
	strcpy(gPath,sp5kFsGetCurPath());
	printf("path:%s\n",gPath);
}

#ifdef SUPPORT_GNSS_MSG
/*-------------------------------------------------------------------------
 *  File Name : reset_gsv_info
 *------------------------------------------------------------------------*/
void reset_gsv_info(void)
{
	gsvcount=0;
	SatellitesinViewCount=0;

}
#endif

/*-------------------------------------------------------------------------
 *  File Name : appGpsModeSwitch
 *------------------------------------------------------------------------*/
void appGpsModeSwitch(void)
{	/* for new GPS module switch state between working and hibernate */
	sp5kGpioDirSet(SP5K_GPIO_GRP_GEN, 0x00000001<<10, 0x00000001<<10);
	sp5kGpioWrite(SP5K_GPIO_GRP_GEN, 0x00000001<<10, 0x00000001<<10);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 120);

	sp5kGpioWrite(SP5K_GPIO_GRP_GEN, 0x00000000<<10, 0x00000001<<10);
}

/*-------------------------------------------------------------------------
 *  File Name : appGpsInfoSave
 *------------------------------------------------------------------------*/
#if GPS_BY_UART0
UINT32
#else
void
#endif
appGpsInfoSave(
	UINT32 ch
)
{
 	static UINT8 lnIdx = 0;
	UINT8 c=(UINT8)ch;
    
    #if 0
    UINT8 bkgsvArray[GPS_INFO_LINE_MAX+3];
    UINT8 m,n;
    #endif

	if (c != '$') {
		if (lnIdx < GPS_INFO_LINE_MAX) {
			*pUart++ = c; /* reserved one byte for this */
			if (c == 0x0a) {
				*pUart++ = '\0'; /* reserved one byte for this */
			}
			lnIdx++;
		}
	} 
    else 
    {
		*pUart++ = '\0'; /* reserved one byte for this */

		if (++uartBuffGrpIdx > (GPS_INFO_GRP - 1)) {
			uartBuffGrpIdx = 0;
		}
		pUart = &sUartBuff[uartBuffGrpIdx][0];

		*pUart++ = '$';
		lnIdx = 0;
	}
    
    //printf("%c",c);
	if(c== '$'){
        UINT8 idx;
        idx=uartBuffGrpIdx>0?(uartBuffGrpIdx-1):(GPS_INFO_GRP - 1);

        #ifdef SUPPORT_GNSS_MSG
        if(strncmp(&sUartBuff[idx][0], "$GNRMC",6)==0)
        {
            sp5kOsMutexGet(&GNRMCmutex, TX_WAIT_FOREVER);
            
            if(gnss_test_mode==0)
        		gnss_rmc_recieved_flag|=0x01;

            memcpy (gnss_rmc, &sUartBuff[idx][0], sizeof (gnss_rmc));
        	sp5kOsMutexPut(&GNRMCmutex);
        	//printf("GNSS GNRMC\n");
        }
        else
        #endif
		if(strncmp(&sUartBuff[idx][0], "$GPGGA",6)==0){
			memcpy (gga, &sUartBuff[idx][0], sizeof (gga));
			DBG_GPS_PROF("gps: Num:%d->%s\n",idx,gga);
		}else if(strncmp(&sUartBuff[idx][0], "$GPRMC",6)==0){
			memcpy (rmc, &sUartBuff[idx][0], sizeof (rmc));
			//printf("Num:%d->%s\n",idx,rmc);
		}else if(strncmp(&sUartBuff[idx][0], "$GPVTG",6)==0){
			memcpy (vtg, &sUartBuff[idx][0], sizeof (vtg));
			DBG_GPS_PROF("gps: Num:%d->%s\n",idx,vtg);
		}
            
		#if 1
		else

        #ifdef SUPPORT_GNSS_MSG
        if((strncmp(&sUartBuff[idx][0], "$GPGSV",6)==0)||(strncmp(&sUartBuff[idx][0], "$GLGSV",6)==0))
        #else
        if(strncmp(&sUartBuff[idx][0], "$GPGSV",6)==0)
        #endif
        {

            if(appActiveStateGet()==APP_STATE_GPS_GSVINFO)
            {
                #ifdef SUPPORT_GNSS_MSG
                if(gsv_mode==0)//GNSS
                {
                    if(strncmp(&sUartBuff[idx][0], "$GPGSV",6)==0)
                    goto exit;
                }
                else//GPS
                {
                    if(strncmp(&sUartBuff[idx][0], "$GLGSV",6)==0)
                    goto exit;
                }
                #endif

	  			//printf("@@----@@-----gsv  info  \n");
	  			//profLogPrintf(3, "GSVst=%s", &sUartBuff[idx][0]);
				if(gsvcount==0)
				{
					sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
					memcpy (gsv, &sUartBuff[idx][0], sizeof (gsv));
					sp5kOsMutexPut(&GPSGSVmutex);

					//memcpy (gsv, tempgsv, sizeof (gsv));
					//printf("gsv0  --->%s\n",gsv);
					//printf("Num:%d->%s\n",idx,gsv);
					
					gsvtotalMessagenumber=gsv[7]-0x30;
					CurrentMessageIdex=gsv[9]-0x30;
					SatellitesinViewCount=(gsv[11]-0x30)*10+(gsv[12]-0x30);
                    
					if((CurrentMessageIdex==1)&&(CurrentMessageIdex<gsvtotalMessagenumber))
					{
					    gsvcount=1;
					}
                    else
                    {
						gsvcount=0;
					}
				}
                else if(gsvcount==1)
				{
					sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
					memcpy (gsv1, &sUartBuff[idx][0], sizeof (gsv1));
					sp5kOsMutexPut(&GPSGSVmutex);

					//memcpy (gsv1, tempgsv, sizeof (gsv1));
					//printf("gsv1  --->%s\n",gsv1);
					
					gsvtotalMessagenumber=gsv1[7]-0x30;
					CurrentMessageIdex=gsv1[9]-0x30;
					SatellitesinViewCount=(gsv1[11]-0x30)*10+(gsv1[12]-0x30);
                    
					if((CurrentMessageIdex==2)&&(CurrentMessageIdex<gsvtotalMessagenumber))
					{
						gsvcount=2;
					}
                    else
                    {
						gsvcount=0;
					}
				}
				else if(gsvcount==2)
				{
                    sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
                    memcpy (gsv2, &sUartBuff[idx][0], sizeof (gsv2));
                    //memcpy (gsv2, tempgsv, sizeof (gsv2));
                    sp5kOsMutexPut(&GPSGSVmutex);

                    //printf("gsv2  --->%s\n",gsv2);
                    CurrentMessageIdex=gsv2[9]-0x30;
                    SatellitesinViewCount=(gsv2[11]-0x30)*10+(gsv2[12]-0x30);

                    if((CurrentMessageIdex==3)&&(CurrentMessageIdex<gsvtotalMessagenumber))
                    {
                        gsvcount=3;
                    }else{
                        gsvcount=0;
                    }
                    
                }
                else  if(gsvcount==3)
                {
                    sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
                    memcpy (gsv3, &sUartBuff[idx][0], sizeof (gsv3));
                    //memcpy (gsv2, tempgsv, sizeof (gsv2));
                    sp5kOsMutexPut(&GPSGSVmutex);

                    //printf("gsv2  --->%s\n",gsv2);
                    CurrentMessageIdex=gsv3[9]-0x30;
                    SatellitesinViewCount=(gsv3[11]-0x30)*10+(gsv3[12]-0x30);
                    gsvcount=0;
                }
			}
		}
		#endif
	 }
#ifdef SUPPORT_GNSS_MSG
exit:
#endif
	gGpsInfo.gpsStatus = GPS_RUN;
	HL_gps_isexist_flag=GPS_RUN;
    
    #if GPS_BY_UART0
	return 1;
    #endif
 }


/*-------------------------------------------------------------------------
 *  File Name : appUart_thread
 *------------------------------------------------------------------------*/
void
appUart_thread(
	ULONG param
)
{
	UINT32 c;
	/*UINT8 buffer[32];*/
	ULONG flags;

	for (;;) {
		UINT32 err;
		err = sp5kOsEventFlagsGet(&Uart_sig, 1, TX_OR_CLEAR, &flags, TX_WAIT_FOREVER);
		HOST_ASSERT(err == SP5K_SUCCESS);

		while(1)
		{
			sp5kUartRead(1, &c);
			appGpsInfoSave(c);

			//sprintf(buffer,"%c",c);
			//sp5kUartWrite(1, Buffer,strlen(Buffer));  /*Transmit message to device*/
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUart2ndInit
 *------------------------------------------------------------------------*/
void
appUart2ndInit(
	void
)
{
	DBG_GPS_PROF("gps: SY==>appUart2ndInit\n");

    #if GPS_BY_UART0
	sp5kUartRxCallbackSet(0,appGpsInfoSave);
	sp5kOsMutexCreate((SP5K_MUTEX *)&GPSGSVmutex, (char*)"GSVMutex", TX_INHERIT);
    #ifdef SUPPORT_GNSS_MSG
    sp5kOsMutexCreate((SP5K_MUTEX *)&GNRMCmutex, (char*)"GNRMCMutex", TX_INHERIT);
    #endif
    #else
	//sp5kUartInit(1, 115200);				 /*Set Uart Baud Rate :115200*/
	sp5kUartInit(1, 9600);
	sp5kGpioFuncSet(SP5K_GPIO_GRP_FML,(1<<UART_PIN_TX),0);
	sp5kGpioFuncSet(SP5K_GPIO_GRP_FML,(1<<UART_PIN_RX),0);

	SP5K_THREAD *thr;
	UINT32 err;
	err = sp5kOsEventFlagsCreate(&Uart_sig, "sigApp");
	HOST_ASSERT(err == SP5K_SUCCESS);

	sp5kOsMutexCreate((SP5K_MUTEX *)&GPSGSVmutex, (char*)"GSVMutex", TX_INHERIT);

    #ifdef SUPPORT_GNSS_MSG
    sp5kOsMutexCreate((SP5K_MUTEX *)&GNRMCmutex, (char*)"GNRMCMutex", TX_INHERIT);
    #endif

	thr = sp5kOsThreadCreate("thrAppUart", appUart_thread, 0/*param*/,
				14/*prio*/, 14/*preempt_thr*/, 0/*time_slice*/, 1/*auto_start*/);
	HOST_ASSERT(thr != 0);

	err = sp5kOsEventFlagsSet(&Uart_sig, 1, TX_OR);
	HOST_ASSERT(err == SP5K_SUCCESS);
    #endif

    #if 1
    {
        gGpsInfo.ggaInfo.gpggaLatitudeInd='N';
        strncpy(gGpsInfo.ggaInfo.gpggaLatitude,"00000000",8);
        gGpsInfo.ggaInfo.gpggaLongitudeInd='E';
        strncpy(gGpsInfo.ggaInfo.gpggaLongitude,"000000000",9);

        strncpy(gGpsInfo.ggaInfo.gpggaMSLAltitude,"+0000",5);
        strncpy(gGpsInfo.vtgInfo.gpvtgSpeed,"000",3);
	}
    #endif

    //sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
    //sp5kOsMutexPut(&GPSGSVmutex);
}

/*-------------------------------------------------------------------------
 *  File Name : appGpsStrGPRMCProc
 *------------------------------------------------------------------------*/
UINT8
appGpsStrGPRMCProc(
	UINT8 *ptr
)
{
	UINT8 i,j,num;

	if(ptr == NULL)
		printf("%s ptr == NULL\n",__func__);
		
	DBG_GPS_PROF("gps: ptr:%s\n",ptr);

	for (i=j=0; *ptr!='\0'; i++,ptr++){
		if (*ptr== ',')
		{
			j++;
			while (ptr[1] == ','){
				j++;
				ptr++;
			}
			ptr++;

			switch (j) {
			case 1: 	/* time (UTC)  */
				strSkipSpace(&ptr,' ');
				strncpy(gGpsInfo.rmcInfo.gprmcTime,ptr,sizeof(gGpsInfo.rmcInfo.gprmcTime));
				gGpsInfo.rmcInfo.gprmcTime[6]='\0';
				DBG_GPS_PROF("gps: gprmcTime:%s\n",gGpsInfo.rmcInfo.gprmcTime);
				break;
                
			case 2: 	/* Status 'A'=>valid,'V'==>invalid */
				strSkipSpace(&ptr,' ');
				gGpsInfo.rmcInfo.gprmcSts=*ptr;
				DBG_GPS_PROF("gps: gprmcSts:%c\n",gGpsInfo.rmcInfo.gprmcSts);

				//printf("--******---gps gprmcSts:%d\n",gGpsInfo.rmcInfo.gprmcSts);

				if(gGpsInfo.rmcInfo.gprmcSts==0x41)//A=data valid or V=data not valid  A=0x41 v=0x56
				{
					gps_Status_Invalid_flag=0;
				}
				else
				{
					gps_Status_Invalid_flag=1;
				}
				break;
                
			case 3: /*GPRMC Latitude */
				strSkipSpace(&ptr,' ');
				strncpy(gGpsInfo.rmcInfo.gprmcLatitude,ptr,4);

				ptr+=5;
				strSkipSpace(&ptr,' ');
				strncpy(&gGpsInfo.rmcInfo.gprmcLatitude[4],ptr,4);
				
				gGpsInfo.rmcInfo.gprmcLatitude[8]='\0';

				DBG_GPS_PROF("gps: *****gps Latitude:%s\n",gGpsInfo.rmcInfo.gprmcLatitude);

                #if 0
            	for(num=0;num<8;num++)
            	{
            		if(((gGpsInfo.rmcInfo.gprmcLatitude[num])>'9')||((gGpsInfo.rmcInfo.gprmcLatitude[num])<'0'))
            		{
            		     gGpsInfo.rmcInfo.gprmcLatitude[num]='0';
            		}
            	}
                #endif
			    break;
                
			case 4:  /*GPRMC Latitude  Indicator*/
				strSkipSpace(&ptr,' ');
				gGpsInfo.rmcInfo.gprmcLatitudeInd=*ptr;
				DBG_GPS_PROF("gps: xxx--gps gprmcLatitudeInd:%c\n",gGpsInfo.rmcInfo.gprmcLatitudeInd);
				break;
                
			case 5: /*GPRMC Longitude */
				strSkipSpace(&ptr,' ');
				strncpy(gGpsInfo.rmcInfo.gprmcLongitude,ptr,5);

				ptr+=6;
				strSkipSpace(&ptr,' ');
				strncpy(&gGpsInfo.rmcInfo.gprmcLongitude[4],ptr,4);
				
				gGpsInfo.rmcInfo.gprmcLongitude[9]='\0';
				DBG_GPS_PROF("gps: ---11--gps Latitude:%s\n",gGpsInfo.rmcInfo.gprmcLongitude);

                #if 0
            	for(num=0;num<9;num++)
            	{
            		if(((gGpsInfo.rmcInfo.gprmcLongitude[num])>'9')||((gGpsInfo.rmcInfo.gprmcLongitude[num])<'0'))
            		{
            		     gGpsInfo.rmcInfo.gprmcLongitude[num]='0';
            		}
            	}
                #endif
				break;
                
			case 6: /*GPRMC Longitude  Indicator*/
				strSkipSpace(&ptr,' ');
				gGpsInfo.rmcInfo.gprmcLongitudeInd=*ptr;
				DBG_GPS_PROF("gps: --22--gps gprmcLongitudeInd:%c\n",gGpsInfo.rmcInfo.gprmcLongitudeInd);
				break;
                
			case 9: /* Date (UTC) */
				gGpsInfo.rmcInfo.gprmcDate[0]='2';
				gGpsInfo.rmcInfo.gprmcDate[1]='0';
				strSkipSpace(&ptr,' ');
				strncpy(&gGpsInfo.rmcInfo.gprmcDate[2],ptr,6);
				gGpsInfo.rmcInfo.gprmcDate[8]='\0';

				DBG_GPS_PROF("gps: gprmcDate:%s\n",gGpsInfo.rmcInfo.gprmcDate);
				break;
                
			default:
				break;
			}
		}
	}
	return TRUE;
}

/*-------------------------------------------------------------------------
 *  File Name : appGpsStrGPGGAProc
 *------------------------------------------------------------------------*/
UINT8
appGpsStrGPGGAProc(
	UINT8 *ptr
)
{
	UINT8 i,j,k,num;
	UINT8 buf[32];

	DBG_GPS_PROF("gps:: ptr:%s\n",ptr);

	for (i=j=0; *ptr!='\0'; i++,ptr++){
		if (*ptr== ',')
		{
			j++;
			ptr++;

			switch (j) {
			case 1: 	/* Time (UTC)  */
				strSkipSpace(&ptr,' ');
				strncpy(gGpsInfo.ggaInfo.gpggaTime,ptr,sizeof(gGpsInfo.ggaInfo.gpggaTime));
				gGpsInfo.ggaInfo.gpggaTime[6]='\0';

				DBG_GPS_PROF("gps: time %s\n",gGpsInfo.ggaInfo.gpggaTime);
			break;
            
			case 2: /*GPGGA Latitude */
				strSkipSpace(&ptr,' ');
				strncpy(gGpsInfo.ggaInfo.gpggaLatitude,ptr,4);

				ptr+=5;
				strSkipSpace(&ptr,' ');
				strncpy(&gGpsInfo.ggaInfo.gpggaLatitude[4],ptr,4);
				
				gGpsInfo.ggaInfo.gpggaLatitude[8]='\0';
				DBG_GPS_PROF("gps: Latitude:%s\n",gGpsInfo.ggaInfo.gpggaLatitude);

                #if 1
                for(num=0;num<8;num++)
                {
                    if(((gGpsInfo.ggaInfo.gpggaLatitude[num])>'9')||((gGpsInfo.ggaInfo.gpggaLatitude[num])<'0'))
                    {
                        gGpsInfo.ggaInfo.gpggaLatitude[num]='0';
                    }
                }
                #endif
			break;
                
			case 3: /*GPGGA Latitude Indicator*/
				strSkipSpace(&ptr,' ');
				gGpsInfo.ggaInfo.gpggaLatitudeInd=*ptr;
				DBG_GPS_PROF("gps: gpggaLatitudeInd:%c\n",gGpsInfo.ggaInfo.gpggaLatitudeInd);
				break;
                
			case 4: /*GPGGA Longitude */
				strSkipSpace(&ptr,' ');
				strncpy(gGpsInfo.ggaInfo.gpggaLongitude,ptr,5);

				ptr+=6;
				strSkipSpace(&ptr,' ');
				strncpy(&gGpsInfo.ggaInfo.gpggaLongitude[5],ptr,4);
				
				gGpsInfo.ggaInfo.gpggaLongitude[9]='\0';
				DBG_GPS_PROF("gps: gpggaLongitude:%s\n",gGpsInfo.ggaInfo.gpggaLongitude);

            	#if 1
        		for(num=0;num<9;num++)
        		{
        			if(((gGpsInfo.ggaInfo.gpggaLongitude[num]>'9'))||((gGpsInfo.ggaInfo.gpggaLongitude[num])<'0'))
        			{
        			    gGpsInfo.ggaInfo.gpggaLongitude[num]='0';
        			}
        		}
            	#endif
			break;
                
			case 5: /*GPGGA Longitude Indicator*/
				strSkipSpace(&ptr,' ');
				gGpsInfo.ggaInfo.gpggaLongitudeInd=*ptr;
				DBG_GPS_PROF("gps: gpggaLongitudeInd:%c\n",gGpsInfo.ggaInfo.gpggaLongitudeInd);
			break;
                
			case 6: /*GPGGA PositionFixIndicator*/
				strSkipSpace(&ptr,' ');
				gGpsInfo.ggaInfo.gpggaPFI=*ptr;
				DBG_GPS_PROF("gps: gpggaPFI:%c\n",gGpsInfo.ggaInfo.gpggaPFI);

				//printf("****gps PositionFix****gps gpggaPFI:%d\n",gGpsInfo.ggaInfo.gpggaPFI);
				/*
				if(gGpsInfo.ggaInfo.gpggaPFI==0x30)//'0' Fix not available or invalid
					PositionFixIndicator_flag=1;
				else PositionFixIndicator_flag=0;
				*/
			break;
                
			case 9: /*GPGGA Altitude*/
				strSkipSpace(&ptr,' ');
				strncpy(gGpsInfo.ggaInfo.gpggaMSLAltitude,ptr,5);
				gGpsInfo.ggaInfo.gpggaMSLAltitude[5]='\0';

				k=0;
				while(gGpsInfo.ggaInfo.gpggaMSLAltitude[k++]!='\0'){
					if(gGpsInfo.ggaInfo.gpggaMSLAltitude[k]=='.'){
						gGpsInfo.ggaInfo.gpggaMSLAltitude[k]='\0';
						break;
					}
				}

				k= sizeof(gGpsInfo.ggaInfo.gpggaMSLAltitude) - strlen(gGpsInfo.ggaInfo.gpggaMSLAltitude);
				if(k>1){
					memset(buf, '0' ,sizeof(buf));
					if(gGpsInfo.ggaInfo.gpggaMSLAltitude[0]=='-'){
						buf[0]='-';
						gGpsInfo.ggaInfo.gpggaMSLAltitude[0]='0';
					}
					else{
						buf[0]='+';
					}
					strcpy(&buf[k-1],gGpsInfo.ggaInfo.gpggaMSLAltitude);
					buf[5]='\0';
					strcpy(gGpsInfo.ggaInfo.gpggaMSLAltitude,buf);
				}

				DBG_GPS_PROF("gps: gpggaMSLAltitude:%s\n",gGpsInfo.ggaInfo.gpggaMSLAltitude);
			break;
            
			default:
				break;
			}
		}
	}

	return TRUE;
}

/*-------------------------------------------------------------------------
 *  File Name : appGpsStrGPVTGProc
 *------------------------------------------------------------------------*/
UINT8
appGpsStrGPVTGProc(
	UINT8 *ptr
)
{
	UINT8 i,j,k,num;
	UINT8 buf[32];

	DBG_GPS_PROF("gps: ptr:%s\n",ptr);

	for (i=j=0; *ptr!='\0'; i++,ptr++){
		if (*ptr== ',')
		{
			j++;
			ptr++;

			switch (j) {
			case 6: /*GPVTG Speed. KM/h*/
				strSkipSpace(&ptr,' ');
				strncpy(gGpsInfo.vtgInfo.gpvtgSpeed,ptr,3);
				gGpsInfo.vtgInfo.gpvtgSpeed[3]='\0';
				k=0;
				while(gGpsInfo.vtgInfo.gpvtgSpeed[k++]!='\0'){
					if(gGpsInfo.vtgInfo.gpvtgSpeed[k]=='.'){
						gGpsInfo.vtgInfo.gpvtgSpeed[k]='\0';
						break;
					}
				}
				if(strlen(gGpsInfo.vtgInfo.gpvtgSpeed)==0){
					memset(gGpsInfo.vtgInfo.gpvtgSpeed, '0' ,3);
					break;
				}
				k= sizeof(gGpsInfo.vtgInfo.gpvtgSpeed) - strlen(gGpsInfo.vtgInfo.gpvtgSpeed);
				if(k>1){
					memset(buf, '0' ,sizeof(buf));
					strcpy(&buf[k-1],gGpsInfo.vtgInfo.gpvtgSpeed);
					buf[3]='\0';
					strcpy(gGpsInfo.vtgInfo.gpvtgSpeed,buf);
				}

                #if 1
            	for(num=0;num<3;num++)
            	{
            		if(((gGpsInfo.vtgInfo.gpvtgSpeed[num])>'9')||((gGpsInfo.vtgInfo.gpvtgSpeed[num])<'0'))
            		{
            		     gGpsInfo.vtgInfo.gpvtgSpeed[num]='0';
            		}
            	}
                #endif
                
				DBG_GPS_PROF("gps: gpvtgSpeed:%s\n",gGpsInfo.vtgInfo.gpvtgSpeed);
				break;
			default:
				break;
			}
		}
	}

	return TRUE;
}

/*-------------------------------------------------------------------------
 *  File Name : adj_gsv_satellite_id
 *------------------------------------------------------------------------*/
void adj_gsv_satellite_id(UINT8 *str)
{
	if(str[0]!=',')
	{
		if(str[2]==',')//2位 satellite ID
			str[2]=0;
		else    //3位 satellite ID
		    str[3]=0;
	}
	else
	{
		str[0]='-';
		str[1]='-';
		str[2]=0;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appGpsStrGPGSVProc
 *------------------------------------------------------------------------*/
UINT8
appGpsStrGPGSVProc(
	UINT8 *ptr  ,UINT8 group
)
{
	UINT8 i,j,k,num;
	UINT8 buf[32];

	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 40,280, 30, 0, 0);

	if(group==1)
	{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 80,280, 30, 0, 0);

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID0,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID0));
		memset(gGpsInfo.gsvInfo.gpgsvSNR0,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR0));

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID1,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID1));
		memset(gGpsInfo.gsvInfo.gpgsvSNR1,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR1));

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID2,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID2));
		memset(gGpsInfo.gsvInfo.gpgsvSNR2,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR2));

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID3,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID3));
		memset(gGpsInfo.gsvInfo.gpgsvSNR3,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR3));
	}else if(group==2)
	{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 120,280, 30, 0, 0);
		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID4,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID4));
		memset(gGpsInfo.gsvInfo.gpgsvSNR4,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR4));

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID5,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID5));
		memset(gGpsInfo.gsvInfo.gpgsvSNR5,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR5));

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID6,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID6));
		memset(gGpsInfo.gsvInfo.gpgsvSNR6,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR6));

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID7,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID7));
		memset(gGpsInfo.gsvInfo.gpgsvSNR7,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR7));
	}
	else if(group==3)
	{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 160,280, 30, 0, 0);

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID8,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID8));
		memset(gGpsInfo.gsvInfo.gpgsvSNR8,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR8));

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID9,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID9));
		memset(gGpsInfo.gsvInfo.gpgsvSNR9,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR9));

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID10,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID10));
		memset(gGpsInfo.gsvInfo.gpgsvSNR10,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR10));

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID11,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID11));
		memset(gGpsInfo.gsvInfo.gpgsvSNR11,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR11));
	}
	else if(group==4)
	{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 200,280, 30, 0, 0);

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID12,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID12));
		memset(gGpsInfo.gsvInfo.gpgsvSNR12,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR12));

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID13,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID13));
		memset(gGpsInfo.gsvInfo.gpgsvSNR13,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR13));

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID14,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID14));
		memset(gGpsInfo.gsvInfo.gpgsvSNR14,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR14));

		memset(gGpsInfo.gsvInfo.gpgsvSatelliteID15,0,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID15));
		memset(gGpsInfo.gsvInfo.gpgsvSNR15,0,sizeof(gGpsInfo.gsvInfo.gpgsvSNR15));
	}
	//printf("ptr:%s\n",ptr);

	for (i=j=0; *ptr!='\0'; i++,ptr++){
		if (*ptr== ',')
		{
			j++;
			ptr++;

			switch (j) 
            {

				case 1:/*Number ofMessage*/
				strSkipSpace(&ptr,' ');
				gGpsInfo.gsvInfo.ggsvtotalnumber=*ptr;
				DBG_GPS_PROF("gps: gsv total number:%c\n",gGpsInfo.gsvInfo.ggsvtotalnumber);
				break;

				case 2:/* Message Number*/
				strSkipSpace(&ptr,' ');
				gGpsInfo.gsvInfo.ggsvMessageNumber=*ptr;
				DBG_GPS_PROF("gps: gsv Message number:%c\n",gGpsInfo.gsvInfo.ggsvMessageNumber);
				break;

				case 3: /* Satellites in View */
				strSkipSpace(&ptr,' ');
				strncpy(gGpsInfo.gsvInfo.ggsvSatellites,ptr,sizeof(gGpsInfo.gsvInfo.ggsvSatellites));
				gGpsInfo.gsvInfo.ggsvSatellites[2]='\0';
				DBG_GPS_PROF("gps: gsv Satellites in View --- %s\n",gGpsInfo.gsvInfo.ggsvSatellites);
				break;

			    case 4: /* Satellite ID0*/
				strSkipSpace(&ptr,' ');
				if(group==1)
				{
                    strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID0,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID0));
                    adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID0);
                    DBG_GPS_PROF("gps: gsv gpgsvSatelliteID0 ---%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID0);
				}
                else if(group==2)
				{
                    strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID4,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID4));
                    adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID4);
                    DBG_GPS_PROF("gps: gsv gpgsvSatelliteID4 ---%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID4);
				}
                else if(group==3)
				{
                    strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID8,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID8));
                    adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID8);
                    DBG_GPS_PROF("gps: gsv gpgsvSatelliteID8 ---%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID8);
				}
				else if(group==4)
				{
					strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID12,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID12));
					adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID12);
					DBG_GPS_PROF("gps: gsv gpgsvSatelliteID12 ---%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID12);
				}
				break;
                
	     		case 7: /* SNR(C/NO)  */
				strSkipSpace(&ptr,' ');
                
				if(group==1)
                {
    				strncpy(gGpsInfo.gsvInfo.gpgsvSNR0,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR0));
    				gGpsInfo.gsvInfo.gpgsvSNR0[2]='\0';
    				DBG_GPS_PROF("gps: gsv gpgsvSNR0 ---%s\n",gGpsInfo.gsvInfo.gpgsvSNR0);
				}
                else if(group==2)
				{
    				strncpy(gGpsInfo.gsvInfo.gpgsvSNR4,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR4));
    				gGpsInfo.gsvInfo.gpgsvSNR4[2]='\0';
    				DBG_GPS_PROF("gps: gsv gpgsvSNR4 ---%s\n",gGpsInfo.gsvInfo.gpgsvSNR4);
				}
                else if(group==3)
				{
                    strncpy(gGpsInfo.gsvInfo.gpgsvSNR8,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR8));
                    gGpsInfo.gsvInfo.gpgsvSNR8[2]='\0';
                    DBG_GPS_PROF("gps: gsv gpgsvSNR8 ---%s\n",gGpsInfo.gsvInfo.gpgsvSNR8);
				}
				else if(group==4)
                {
                    strncpy(gGpsInfo.gsvInfo.gpgsvSNR12,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR12));
                    gGpsInfo.gsvInfo.gpgsvSNR12[2]='\0';
                    DBG_GPS_PROF("gps: gsv gpgsvSNR12 ---%s\n",gGpsInfo.gsvInfo.gpgsvSNR12);
				}
				break;

			    case 8: /* Satellite ID1*/
				strSkipSpace(&ptr,' ');
                
				if(group==1)
                {
    				strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID1,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID1));
    				adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID1);
    				DBG_GPS_PROF("gps: gsv gpgsvSatelliteID1 :%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID1);
				}
                else if(group==2)
				{
                    strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID5,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID5));
                    adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID5);
                    DBG_GPS_PROF("gps: gsv gpgsvSatelliteID5 :%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID5);
				}
				else if(group==3)
				{
					strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID9,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID9));
					adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID9);
					DBG_GPS_PROF("gps: gsv gpgsvSatelliteID9 :%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID9);
				}
				else if(group==4)
				{
					strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID13,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID13));
					adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID13);
					DBG_GPS_PROF("gps: gsv gpgsvSatelliteID13 :%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID13);
				}
				break;
                
	     		case 11: /* SNR(C/NO)  */
				strSkipSpace(&ptr,' ');
                
				if(group==1)
                {
    				strncpy(gGpsInfo.gsvInfo.gpgsvSNR1,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR1));
    				gGpsInfo.gsvInfo.gpgsvSNR1[2]='\0';
    				DBG_GPS_PROF("gps: gsv gpgsvSNR1 :%s\n",gGpsInfo.gsvInfo.gpgsvSNR1);
				}
                else if(group==2)
				{
    				strncpy(gGpsInfo.gsvInfo.gpgsvSNR5,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR5));
    				gGpsInfo.gsvInfo.gpgsvSNR5[2]='\0';
    				DBG_GPS_PROF("gps: gsv gpgsvSNR5 :%s\n",gGpsInfo.gsvInfo.gpgsvSNR5);
				}
				else if(group==3)
				{
    				strncpy(gGpsInfo.gsvInfo.gpgsvSNR9,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR9));
    				gGpsInfo.gsvInfo.gpgsvSNR9[2]='\0';
    				DBG_GPS_PROF("gps: gsv gpgsvSNR9 :%s\n",gGpsInfo.gsvInfo.gpgsvSNR9);
				}
				else if(group==4)
				{
    				strncpy(gGpsInfo.gsvInfo.gpgsvSNR13,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR13));
    				gGpsInfo.gsvInfo.gpgsvSNR13[2]='\0';
    				DBG_GPS_PROF("gps: gsv gpgsvSNR13 :%s\n",gGpsInfo.gsvInfo.gpgsvSNR13);
				}
				break;

			    case 12: /* Satellite ID2*/
				strSkipSpace(&ptr,' ');
                
				if(group==1)
                {
    				strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID2,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID2));
    				adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID2);
    				DBG_GPS_PROF("gps: gsv gpgsvSatelliteID2:%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID2);
				}
                else if(group==2)
                {
    				strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID6,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID6));
    				adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID6);
    				DBG_GPS_PROF("gps: gsv gpgsvSatelliteID6:%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID6);
				}
				else if(group==3)
                {
    				strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID10,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID10));
    				adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID10);
    				DBG_GPS_PROF("gps: gsv gpgsvSatelliteID10:%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID10);
				}
				else if(group==4)
                {
                    strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID14,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID14));
                    adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID14);
                    DBG_GPS_PROF("gps: gsv gpgsvSatelliteID14:%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID14);
				}
				break;
                
	     		case 15: /* SNR(C/NO)  */
				strSkipSpace(&ptr,' ');

				if(group==1)
                {
				    strncpy(gGpsInfo.gsvInfo.gpgsvSNR2,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR2));
    				gGpsInfo.gsvInfo.gpgsvSNR2[2]='\0';
	    			DBG_GPS_PROF("gps: gsv gpgsvSNR2 :%s\n",gGpsInfo.gsvInfo.gpgsvSNR2);
				}
                else if(group==2)
				{
				    strncpy(gGpsInfo.gsvInfo.gpgsvSNR6,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR6));
    				gGpsInfo.gsvInfo.gpgsvSNR6[2]='\0';
	    			DBG_GPS_PROF("gps: gsv gpgsvSNR6 :%s\n",gGpsInfo.gsvInfo.gpgsvSNR6);
				}
                else if(group==3)
				{
				    strncpy(gGpsInfo.gsvInfo.gpgsvSNR10,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR10));
    				gGpsInfo.gsvInfo.gpgsvSNR10[2]='\0';
	    			DBG_GPS_PROF("gps: gsv gpgsvSNR10 :%s\n",gGpsInfo.gsvInfo.gpgsvSNR10);
				}
    		  	else if(group==4)
				{
				    strncpy(gGpsInfo.gsvInfo.gpgsvSNR14,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR14));
    				gGpsInfo.gsvInfo.gpgsvSNR14[2]='\0';
	    			DBG_GPS_PROF("gps: gsv gpgsvSNR14 :%s\n",gGpsInfo.gsvInfo.gpgsvSNR14);
				}
				break;

	   	        case 16: /* Satellite ID3*/
				strSkipSpace(&ptr,' ');
                
				if(group==1)
                {
				    strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID3,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID3));
    				adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID3);
	    			DBG_GPS_PROF("gps: gsv gpgsvSatelliteID3 :%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID3);
				}
                else if(group==2)
                {
				    strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID7,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID7));
				    adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID7);
				    DBG_GPS_PROF("gps: gsv gpgsvSatelliteID7 :%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID7);
				}
                else if(group==3)
                {
				    strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID11,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID11));
    				adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID11);
	    			DBG_GPS_PROF("gps: gsv gpgsvSatelliteID11 :%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID11);
				}
				else if(group==4)
                {
				    strncpy(gGpsInfo.gsvInfo.gpgsvSatelliteID15,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSatelliteID15));
    				adj_gsv_satellite_id(gGpsInfo.gsvInfo.gpgsvSatelliteID15);
	    			DBG_GPS_PROF("gps: gsv gpgsvSatelliteID15 :%s\n",gGpsInfo.gsvInfo.gpgsvSatelliteID15);
				}
				break;
                
	            case 19: /* SNR(C/NO)  */
				strSkipSpace(&ptr,' ');
                
				if(group==1)
                {
				    strncpy(gGpsInfo.gsvInfo.gpgsvSNR3,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR3));
    				gGpsInfo.gsvInfo.gpgsvSNR3[2]='\0';
	    			DBG_GPS_PROF("gps: gsv gpgsvSNR3 :%s\n",gGpsInfo.gsvInfo.gpgsvSNR3);
				}
                else if(group==2)
                {
				    strncpy(gGpsInfo.gsvInfo.gpgsvSNR7,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR7));
    				gGpsInfo.gsvInfo.gpgsvSNR7[2]='\0';
	    			DBG_GPS_PROF("gps: gsv gpgsvSNR7 :%s\n",gGpsInfo.gsvInfo.gpgsvSNR7);
				}
                else if(group==3)
                {
				    strncpy(gGpsInfo.gsvInfo.gpgsvSNR11,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR11));
    				gGpsInfo.gsvInfo.gpgsvSNR11[2]='\0';
	    			DBG_GPS_PROF("gps: gsv gpgsvSNR11 :%s\n",gGpsInfo.gsvInfo.gpgsvSNR11);
				}
				else if(group==4)
                {
				    strncpy(gGpsInfo.gsvInfo.gpgsvSNR15,ptr,sizeof(gGpsInfo.gsvInfo.gpgsvSNR15));
    				gGpsInfo.gsvInfo.gpgsvSNR15[2]='\0';
	    			DBG_GPS_PROF("gps: gsv gpgsvSNR15 :%s\n",gGpsInfo.gsvInfo.gpgsvSNR15);
				}
				break;
                
			default:
				break;
			}
		}
	}

	return TRUE;
}

/*-------------------------------------------------------------------------
 *  File Name : HL_disp_gpGSV_show
 *------------------------------------------------------------------------*/
void HL_disp_gpGSV_show(group)
{
	UINT8 buf[128]={0};
	UINT8 buf2[128]={0};

    appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
    appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 40,280, 30, 0, 0);

    #ifdef SUPPORT_GNSS_MSG
    if(gsv_mode==0)
    	sprintf(buf2,"GNSS total:%2d ",SatellitesinViewCount);
    else
        sprintf(buf2,"GPS total:%2d ",SatellitesinViewCount);
    #else
    sprintf(buf2,"total:%2d ",SatellitesinViewCount);
    #endif
    
    appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
    appOsdLib_TextDraw(20, 40, SP5K_GFX_ALIGN_TOP_LEFT, buf2);

	if(group==1)
	{
        //printf("@@---HL_disp_gpGSV_show--group==1 \n");

        if(SatellitesinViewCount==1)
        {
            sprintf(buf,"%s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID0,gGpsInfo.gsvInfo.gpgsvSNR0);
            SatellitesinViewReadyProcCount=1;
        }
        else if(SatellitesinViewCount==2)
        {
            sprintf(buf,"%s-%s   %s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID0,gGpsInfo.gsvInfo.gpgsvSNR0,gGpsInfo.gsvInfo.gpgsvSatelliteID1,gGpsInfo.gsvInfo.gpgsvSNR1);
            SatellitesinViewReadyProcCount=2;
        }
        else if(SatellitesinViewCount==3)
        {
            sprintf(buf,"%s-%s   %s-%s   %s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID0,gGpsInfo.gsvInfo.gpgsvSNR0,gGpsInfo.gsvInfo.gpgsvSatelliteID1,gGpsInfo.gsvInfo.gpgsvSNR1,gGpsInfo.gsvInfo.gpgsvSatelliteID2,gGpsInfo.gsvInfo.gpgsvSNR2);
            SatellitesinViewReadyProcCount=3;
        }else
        {
            sprintf(buf,"%s-%s   %s-%s   %s-%s   %s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID0,gGpsInfo.gsvInfo.gpgsvSNR0,gGpsInfo.gsvInfo.gpgsvSatelliteID1,gGpsInfo.gsvInfo.gpgsvSNR1,gGpsInfo.gsvInfo.gpgsvSatelliteID2,gGpsInfo.gsvInfo.gpgsvSNR2,gGpsInfo.gsvInfo.gpgsvSatelliteID3,gGpsInfo.gsvInfo.gpgsvSNR3);
            SatellitesinViewReadyProcCount=4;
        }
        appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
        appOsdLib_TextDraw(20, 80, SP5K_GFX_ALIGN_TOP_LEFT, buf);
	}
	else if(group==2)
	{
		//printf("@@---HL_disp_gpGSV_show--group==2 \n");
        if((SatellitesinViewCount-SatellitesinViewReadyProcCount)==1)
        {
            sprintf(buf,"%s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID4,gGpsInfo.gsvInfo.gpgsvSNR4);
            SatellitesinViewReadyProcCount=5;
        }
        else if((SatellitesinViewCount-SatellitesinViewReadyProcCount)==2)
        {
            sprintf(buf,"%s-%s   %s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID4,gGpsInfo.gsvInfo.gpgsvSNR4,gGpsInfo.gsvInfo.gpgsvSatelliteID5,gGpsInfo.gsvInfo.gpgsvSNR5);
            SatellitesinViewReadyProcCount=6;
        }
        else if((SatellitesinViewCount-SatellitesinViewReadyProcCount)==3)
        {
            sprintf(buf,"%s-%s   %s-%s   %s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID4,gGpsInfo.gsvInfo.gpgsvSNR4,gGpsInfo.gsvInfo.gpgsvSatelliteID5,gGpsInfo.gsvInfo.gpgsvSNR5,gGpsInfo.gsvInfo.gpgsvSatelliteID6,gGpsInfo.gsvInfo.gpgsvSNR6);
            SatellitesinViewReadyProcCount=7;
        }
        else
        {
            sprintf(buf,"%s-%s   %s-%s   %s-%s   %s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID4,gGpsInfo.gsvInfo.gpgsvSNR4,gGpsInfo.gsvInfo.gpgsvSatelliteID5,gGpsInfo.gsvInfo.gpgsvSNR5,gGpsInfo.gsvInfo.gpgsvSatelliteID6,gGpsInfo.gsvInfo.gpgsvSNR6,gGpsInfo.gsvInfo.gpgsvSatelliteID7,gGpsInfo.gsvInfo.gpgsvSNR7);
            SatellitesinViewReadyProcCount=8;
        }
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(20, 120, SP5K_GFX_ALIGN_TOP_LEFT, buf);

	}
	else if(group==3)
	{
		//printf("@@---HL_disp_gpGSV_show--group==3 \n");

        if((SatellitesinViewCount-SatellitesinViewReadyProcCount)==1)
        {
            sprintf(buf,"%s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID8,gGpsInfo.gsvInfo.gpgsvSNR8);
            SatellitesinViewReadyProcCount=9;
        }
        else if((SatellitesinViewCount-SatellitesinViewReadyProcCount)==2)
        {
            sprintf(buf,"%s-%s   %s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID8,gGpsInfo.gsvInfo.gpgsvSNR8,gGpsInfo.gsvInfo.gpgsvSatelliteID9,gGpsInfo.gsvInfo.gpgsvSNR9);
            SatellitesinViewReadyProcCount=10;
        }
        else if((SatellitesinViewCount-SatellitesinViewReadyProcCount)==3)
        {
            sprintf(buf,"%s-%s   %s-%s   %s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID8,gGpsInfo.gsvInfo.gpgsvSNR8,gGpsInfo.gsvInfo.gpgsvSatelliteID9,gGpsInfo.gsvInfo.gpgsvSNR9,gGpsInfo.gsvInfo.gpgsvSatelliteID10,gGpsInfo.gsvInfo.gpgsvSNR10);
            SatellitesinViewReadyProcCount=11;
        }
        else
        {
            sprintf(buf,"%s-%s   %s-%s   %s-%s   %s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID8,gGpsInfo.gsvInfo.gpgsvSNR8,gGpsInfo.gsvInfo.gpgsvSatelliteID9,gGpsInfo.gsvInfo.gpgsvSNR9,gGpsInfo.gsvInfo.gpgsvSatelliteID10,gGpsInfo.gsvInfo.gpgsvSNR10,gGpsInfo.gsvInfo.gpgsvSatelliteID11,gGpsInfo.gsvInfo.gpgsvSNR11);
            SatellitesinViewReadyProcCount=12;
        }

		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(20, 160, SP5K_GFX_ALIGN_TOP_LEFT, buf);
	}
	else if(group==4)
	{
		//printf("@@---HL_disp_gpGSV_show--group==3 \n");

        if((SatellitesinViewCount-SatellitesinViewReadyProcCount)==1)
        {
            sprintf(buf,"%s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID12,gGpsInfo.gsvInfo.gpgsvSNR12);
            SatellitesinViewReadyProcCount=13;
        }
        else if((SatellitesinViewCount-SatellitesinViewReadyProcCount)==2)
        {
            sprintf(buf,"%s-%s   %s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID12,gGpsInfo.gsvInfo.gpgsvSNR12,gGpsInfo.gsvInfo.gpgsvSatelliteID13,gGpsInfo.gsvInfo.gpgsvSNR13);
            SatellitesinViewReadyProcCount=14;
        }
        else if((SatellitesinViewCount-SatellitesinViewReadyProcCount)==3)
        {
            sprintf(buf,"%s-%s   %s-%s   %s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID12,gGpsInfo.gsvInfo.gpgsvSNR12,gGpsInfo.gsvInfo.gpgsvSatelliteID13,gGpsInfo.gsvInfo.gpgsvSNR13,gGpsInfo.gsvInfo.gpgsvSatelliteID14,gGpsInfo.gsvInfo.gpgsvSNR14);
            SatellitesinViewReadyProcCount=15;
        }
        else
        {
            sprintf(buf,"%s-%s   %s-%s   %s-%s   %s-%s",gGpsInfo.gsvInfo.gpgsvSatelliteID12,gGpsInfo.gsvInfo.gpgsvSNR12,gGpsInfo.gsvInfo.gpgsvSatelliteID13,gGpsInfo.gsvInfo.gpgsvSNR13,gGpsInfo.gsvInfo.gpgsvSatelliteID14,gGpsInfo.gsvInfo.gpgsvSNR14,gGpsInfo.gsvInfo.gpgsvSatelliteID15,gGpsInfo.gsvInfo.gpgsvSNR15);
            SatellitesinViewReadyProcCount=16;
        }

		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(20, 200, SP5K_GFX_ALIGN_TOP_LEFT, buf);
	}
}

#if 0
/*-------------------------------------------------------------------------
 *  File Name : appGGInfoHdrInt
 *------------------------------------------------------------------------*/
void appGGInfoHdrInt(void)
{
	UINT8 vtagstr[256];
	UINT8 buf[128];
	UINT8 *dir;
	appRTC_t tt;
	UINT32 len;

	usrDataCnt=0;

	memset(vtagstr, 0 , sizeof(vtagstr));
	/*Dir */

	dir = (UINT8 *)strrchr(gPath, '/');
	if(dir==NULL)printf("Path err\n");

	dir++;
	strcpy(vtagstr,dir);

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x55 , 0x1d-0x08+1);
	strcat(vtagstr,buf);

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0xFE , 2);
	strcat(vtagstr,buf);

	appRtcGet(&tt);
	memset(buf, 0 , sizeof(buf));
	sprintf(buf,"%04d%02d%02d%02d%02d%02d",tt.year,tt.mon,tt.day,tt.hour,tt.min,tt.sec);
	strcat(vtagstr,buf);

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x0C , 1);
	strcat(vtagstr,buf);

	len=strlen(vtagstr);
	usrDataCnt+=len;
	sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);

	DBG_GPS_PROF("%s\n",vtagstr);

}
#else
/*-------------------------------------------------------------------------
 *  File Name : appGGInfoHdrWriteEn
 *------------------------------------------------------------------------*/
UINT8 ggHdrEn; 
void
appGGInfoHdrWriteEn(
	UINT8 en
)
{
	ggHdrEn=en;
}

#endif

/*-------------------------------------------------------------------------
 *  File Name : HL_appGpsInfoWrite2Mov_whenGPS_stop
 *------------------------------------------------------------------------*/
void  HL_appGpsInfoWrite2Mov_whenGPS_stop(void)
{
	UINT8 vtagstr = sp5kMalloc(512);
	UINT8 buf = sp5kMalloc(128);
	UINT8 buf222 = sp5kMalloc(10);
	UINT32 len;
	appRTC_t tt;
	usrDataCnt=0;
	memset(vtagstr, 0 , sizeof(vtagstr));

	if(ggHdrEn){
		UINT8 *dir;
		/*Dir */
		dir = (UINT8 *)strrchr(gPath, '/');
		if(dir==NULL)
		{
		printf("Path err\n");
		}
		dir++;
		strcpy(vtagstr,dir);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x55 , 0x1d-0x08+1);
		strcat(vtagstr,buf);
		#if 1
		memset(buf, 0 , sizeof(buf));
		memset(buf, 0xFE , 2);
		strcat(vtagstr,buf);

		appRtcGet(&tt);
		memset(buf, 0 , sizeof(buf));
		sprintf(buf,"%04d%02d%02d%02d%02d%02d",tt.year,tt.mon,tt.day,tt.hour,tt.min,tt.sec);
		strcat(vtagstr,buf);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x0C , 1);
		strcat(vtagstr,buf);

		len=strlen(vtagstr);
		usrDataCnt+=len;
		appGGInfoHdrWriteEn(0);
    	#endif
	}    
    else
    {
		memset(buf, 0 , sizeof(buf));
		#if 1
		memset(buf, 0 , sizeof(buf));
		memset(buf, 0xFE , 2);
		strcat(vtagstr,buf);

		appRtcGet(&tt);
		memset(buf, 0 , sizeof(buf));
		sprintf(buf,"%04d%02d%02d%02d%02d%02d",tt.year,tt.mon,tt.day,tt.hour,tt.min,tt.sec);
		strcat(vtagstr,buf);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x0C , 1);
		strcat(vtagstr,buf);

		len=strlen(vtagstr);
		usrDataCnt+=len;
	    #endif
    }

    appRtcGet(&tt);
    memset(buf, 0 , sizeof(buf));
    sprintf(buf,"%04d%02d%02d%02d%02d%02d",tt.year,tt.mon,tt.day,tt.hour,tt.min,tt.sec);
    strcat(vtagstr,buf);

    memset(buf222, 0 , sizeof(buf222));
    memset(buf222, 0x01 , 1);
    strcat(vtagstr,buf222);

    /*N*/
    memset(buf, 0 , sizeof(buf));
    memset(buf, 0x4E , 1);
    strcat(vtagstr,buf);

    memset(buf, 0 , sizeof(buf));
    memset(buf, 0x30 , 8);
    strcat(vtagstr,buf);

    /*E*/
    memset(buf, 0 , sizeof(buf));
    memset(buf, 0x45 , 1);
    strcat(vtagstr,buf);

    memset(buf, 0 , sizeof(buf));
    memset(buf, 0x30 , 8);
    strcat(vtagstr,buf);

    memset(buf, 0 , sizeof(buf));
    memset(buf, 0x30 ,1);
    strcat(vtagstr,buf);
    /*+*/
    memset(buf, 0 , sizeof(buf));
    memset(buf, 0x2B ,1);
    strcat(vtagstr,buf);

    memset(buf, 0 , sizeof(buf));
    memset(buf, 0x30 ,7);
    strcat(vtagstr,buf);

    len=strlen(vtagstr);
    usrDataCnt+=len;

	if(LastFileNotRellyClose==1)
	{
		appUserDateWriteTempBuff(vtagstr,len);
	}
	else if(LastFileNotRellyClose==2)
	{
		appTempBuff2Mov(GpsTempBuff);
		sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);
	}
	else
	{
		sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);
	}

	sp5kFree(vtagstr);
	sp5kFree(buf);
	sp5kFree(buf222);
}

/*-------------------------------------------------------------------------
 *  File Name : HL_appGpsInfoWrite2Mov_whenGPSinfoError
 *------------------------------------------------------------------------*/
void HL_appGpsInfoWrite2Mov_whenGPSinfoError(void)
{
	UINT8 vtagstr = sp5kMalloc(512);
	UINT8 buf = sp5kMalloc(128);
	UINT8 buf222 = sp5kMalloc(10);
	UINT32 len;
	
	usrDataCnt=0;
	memset(vtagstr, 0 , sizeof(vtagstr));

	if(ggHdrEn){
		UINT8 *dir;
		appRTC_t tt;
		/*Dir */

		dir = (UINT8 *)strrchr(gPath, '/');
		if(dir==NULL)
		{
		printf("Path err\n");
		}
		dir++;
		strcpy(vtagstr,dir);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x55 , 0x1d-0x08+1);
		strcat(vtagstr,buf);
		#if 1
		memset(buf, 0 , sizeof(buf));
		memset(buf, 0xFE , 2);
		strcat(vtagstr,buf);

		appRtcGet(&tt);
		memset(buf, 0 , sizeof(buf));
		sprintf(buf,"%04d%02d%02d%02d%02d%02d",tt.year,tt.mon,tt.day,tt.hour,tt.min,tt.sec);
		strcat(vtagstr,buf);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x0C , 1);
		strcat(vtagstr,buf);

		len=strlen(vtagstr);
		usrDataCnt+=len;
		appGGInfoHdrWriteEn(0);
	    #endif
	}
    else
    {
		memset(buf, 0 , sizeof(buf));
		#if 1
		memset(buf, 0 , sizeof(buf));
		memset(buf, 0xFE , 2);
		strcat(vtagstr,buf);

		appRtcGet(&tt);
		memset(buf, 0 , sizeof(buf));
		sprintf(buf,"%04d%02d%02d%02d%02d%02d",tt.year,tt.mon,tt.day,tt.hour,tt.min,tt.sec);
		strcat(vtagstr,buf);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x0C , 1);
		strcat(vtagstr,buf);

		len=strlen(vtagstr);
		usrDataCnt+=len;
	    #endif
    }

    #if 0 //feng20120905
    memset(buf, 0 , sizeof(buf));
	HL_GPS_time_string_ASCII_Get(&GPStime);
	appTimezone(&GPStime,pUiSetting->TimezoneSTATUS);
	// printf(" GPStime time=%4d:%2d:%2d:%2d:%2d:%2d\n",GPStime.year ,GPStime.mon,GPStime.day,GPStime.hour,GPStime.min,GPStime.sec);
	sprintf(buf,"%04d%02d%02d%02d%02d%02d",GPStime.year,GPStime.mon,GPStime.day,GPStime.hour,GPStime.min,GPStime.sec);
	strcat(vtagstr,buf);
    #else
    appRtcGet(&tt);
	memset(buf, 0 , sizeof(buf));
	sprintf(buf,"%04d%02d%02d%02d%02d%02d",tt.year,tt.mon,tt.day,tt.hour,tt.min,tt.sec);
	strcat(vtagstr,buf);
    #endif

	//sprintf(buf,"%c",gGpsInfo.ggaInfo.gpggaPFI);
	//strcat(vtagstr,buf);

	memset(buf222, 0 , sizeof(buf222));
	memset(buf222, 0x02 , 1);
	strcat(vtagstr,buf222);

	#if 0
	sprintf(buf,"%c",gGpsInfo.rmcInfo.gprmcLatitudeInd);/*N /S*/
	strcat(vtagstr,buf);
	strcat(vtagstr,gGpsInfo.rmcInfo.gprmcLatitude);

	sprintf(buf,"%c",gGpsInfo.rmcInfo.gprmcLongitudeInd);/*E /W*/
	strcat(vtagstr,buf);
	strcat(vtagstr,gGpsInfo.rmcInfo.gprmcLongitude);
	#else
	sprintf(buf,"%c",gGpsInfo.ggaInfo.gpggaLatitudeInd);
	strcat(vtagstr,buf);
	strcat(vtagstr,gGpsInfo.ggaInfo.gpggaLatitude);

	sprintf(buf,"%c",gGpsInfo.ggaInfo.gpggaLongitudeInd);
	strcat(vtagstr,buf);
	strcat(vtagstr,gGpsInfo.ggaInfo.gpggaLongitude);
    #endif

	strcat(vtagstr,gGpsInfo.ggaInfo.gpggaMSLAltitude);
	strcat(vtagstr,gGpsInfo.vtgInfo.gpvtgSpeed);

	//printf("%s\n",vtagstr);
	len=strlen(vtagstr);
	usrDataCnt+=len;
	if(LastFileNotRellyClose==1)
	{
		appUserDateWriteTempBuff(vtagstr,len);
	}
	else if(LastFileNotRellyClose==2)
	{
		appTempBuff2Mov(GpsTempBuff);
		sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);
	}
	else
	{
		sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);
	}

	sp5kFree(vtagstr);
	sp5kFree(buf);
	sp5kFree(buf222);
}

/*-------------------------------------------------------------------------
 *  File Name : appGpsInfoWrite2Mov
 *------------------------------------------------------------------------*/
void
appGpsInfoWrite2Mov(
	void
)
{
	UINT8 vtagstr = sp5kMalloc(512);
	UINT8 buf = sp5kMalloc(128);
	UINT8 buf222 = sp5kMalloc(10);
	UINT32 len;
	appRTC_t tt;
	usrDataCnt=0;
	appRTC_t GPStime;

	memset(vtagstr, 0 , sizeof(vtagstr));

	if(ggHdrEn){
		UINT8 *dir;
		/*Dir */
		dir = (UINT8 *)strrchr(gPath, '/');
		if(dir==NULL)
		{
		printf("Path err\n");
		}
		dir++;
		strcpy(vtagstr,dir);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x55 , 0x1d-0x08+1);
		strcat(vtagstr,buf);
		#if 1
		memset(buf, 0 , sizeof(buf));
		memset(buf, 0xFE , 2);
		strcat(vtagstr,buf);

		appRtcGet(&tt);
		memset(buf, 0 , sizeof(buf));
		sprintf(buf,"%04d%02d%02d%02d%02d%02d",tt.year,tt.mon,tt.day,tt.hour,tt.min,tt.sec);
		strcat(vtagstr,buf);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x0C , 1);
		strcat(vtagstr,buf);

		len=strlen(vtagstr);
		usrDataCnt+=len;
		appGGInfoHdrWriteEn(0);
    	#endif
	}   
    else
    {
		memset(buf, 0 , sizeof(buf));
		#if 1
		memset(buf, 0 , sizeof(buf));
		memset(buf, 0xFE , 2);
		strcat(vtagstr,buf);

		appRtcGet(&tt);
		memset(buf, 0 , sizeof(buf));
		sprintf(buf,"%04d%02d%02d%02d%02d%02d",tt.year,tt.mon,tt.day,tt.hour,tt.min,tt.sec);
		strcat(vtagstr,buf);

		memset(buf, 0 , sizeof(buf));
		memset(buf, 0x0C , 1);
		strcat(vtagstr,buf);

		len=strlen(vtagstr);
		usrDataCnt+=len;
	    #endif
    }

	memset(buf, 0 , sizeof(buf));

    #if 1  //feng20120905
	HL_GPS_time_string_ASCII_Get(&GPStime);
	appTimezone(&GPStime,pUiSetting->TimezoneSTATUS);
	//printf(" GPStime time=%4d:%2d:%2d:%2d:%2d:%2d\n",GPStime.year ,GPStime.mon,GPStime.day,GPStime.hour,GPStime.min,GPStime.sec);
	sprintf(buf,"%04d%02d%02d%02d%02d%02d",GPStime.year,GPStime.mon,GPStime.day,GPStime.hour,GPStime.min,GPStime.sec);
	strcat(vtagstr,buf);
    #else
	strcat(vtagstr,gGpsInfo.rmcInfo.gprmcDate);
	strcat(vtagstr,gGpsInfo.rmcInfo.gprmcTime);
    #endif

	//sprintf(buf,"%c",gGpsInfo.ggaInfo.gpggaPFI);
	//strcat(vtagstr,buf);

	memset(buf222, 0 , sizeof(buf222));
	memset(buf222, 0x03 , 1);
	strcat(vtagstr,buf222);

	#if 0
	sprintf(buf,"%c",gGpsInfo.rmcInfo.gprmcLatitudeInd);/*N /S*/
	strcat(vtagstr,buf);
	strcat(vtagstr,gGpsInfo.rmcInfo.gprmcLatitude);

	sprintf(buf,"%c",gGpsInfo.rmcInfo.gprmcLongitudeInd);/*E /W*/
	strcat(vtagstr,buf);
	strcat(vtagstr,gGpsInfo.rmcInfo.gprmcLongitude);
	#else
	sprintf(buf,"%c",gGpsInfo.ggaInfo.gpggaLatitudeInd);
	strcat(vtagstr,buf);
	strcat(vtagstr,gGpsInfo.ggaInfo.gpggaLatitude);

	sprintf(buf,"%c",gGpsInfo.ggaInfo.gpggaLongitudeInd);
	strcat(vtagstr,buf);
	strcat(vtagstr,gGpsInfo.ggaInfo.gpggaLongitude);
    #endif

	strcat(vtagstr,gGpsInfo.ggaInfo.gpggaMSLAltitude);
	strcat(vtagstr,gGpsInfo.vtgInfo.gpvtgSpeed);

  	memset(stamp_GPSSpeedBuf, 0 , sizeof(stamp_GPSSpeedBuf));
  	sprintf(stamp_GPSSpeedBuf,"%s%s",gGpsInfo.vtgInfo.gpvtgSpeed,"km/h");
  	//printf("@@--speed -%s \n",stamp_GPSSpeedBuf);

	//printf("%s\n",vtagstr);
	len=strlen(vtagstr);
	usrDataCnt+=len;
	if(LastFileNotRellyClose==1)
	{
		appUserDateWriteTempBuff(vtagstr,len);
	}
	else if(LastFileNotRellyClose==2)
	{
		appTempBuff2Mov(GpsTempBuff);
		sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);
	}
	else
	{
		sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);
	}

	sp5kFree(vtagstr);
	sp5kFree(buf);
	sp5kFree(buf222);
}

/*-------------------------------------------------------------------------
 *  File Name : appG_senorInfoWrite2Mov
 *------------------------------------------------------------------------*/
void
appG_senorInfoWrite2Mov(
	void
)
{
	UINT8 vtagstr = sp5kMalloc(512);
	//UINT8 buf[128];
	UINT32 len;

	memset(vtagstr, 0 , sizeof(vtagstr));
	//memset(buf, 0 , sizeof(buf));

	#if 0
	appGSensorInfoGet(vtagstr,16);
	#endif
	//printf("G_senorInfo  %s\n",vtagstr);
    //DBG_GPS_PROF("%s\n",vtagstr);

	len=strlen(vtagstr);
	usrDataCnt+=len;
	if(LastFileNotRellyClose==1)
	{
		appUserDateWriteTempBuff(vtagstr,len);
	}
	else if(LastFileNotRellyClose==2)
	{
		appTempBuff2Mov(GpsTempBuff);
		sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);
	}
	else
	{
		sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);
	}
	sp5kFree(vtagstr);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsrDataWriteEnd
 *------------------------------------------------------------------------*/
void
appUsrDataWriteEnd(
	void
)
{
	UINT8 vtagstr = sp5kMalloc(512);
	UINT8 buf = sp5kMalloc(128);
	UINT32 len;
	UINT8 i=0;

	//if(udatWriteSts==0)return;
	appTempBuff2Mov(GpsTempBuff);

	memset(vtagstr, 0 , sizeof(vtagstr));

	#if 0
	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x30 , 72);
	strcat(vtagstr,buf);
	#endif

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0xFD , 2);
	strcat(vtagstr,buf);


	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x30 , 24);
	strcat(vtagstr,buf);

	/*
	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x58 , 12);
	strcat(vtagstr,buf);
	*/

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x58 , 8);
	strcat(vtagstr,buf);

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x2e , 1);
	strcat(vtagstr,buf);

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x4d , 1);
	strcat(vtagstr,buf);

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x4f , 1);
	strcat(vtagstr,buf);

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0x56 , 1);
	strcat(vtagstr,buf);

	len=strlen(vtagstr);
	usrDataCnt+=len;
	usrDataCnt+=14;

	memset(buf, 0 , sizeof(buf));
	sprintf(buf,"%12x",usrDataCnt);
	strcat(vtagstr,buf);

	memset(buf, 0 , sizeof(buf));
	memset(buf, 0xFC , 2);
	strcat(vtagstr,buf);
	len+=14;

	UINT8 *p1=vtagstr;
	UINT8 buf9[24]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	UINT8 *p=buf9;

	for(i=0;i<24;i++)
	{
	   *(p1+2+i)=*(p+i);
	}
	//	*(p1+len+i)=*(p+i);  sp5kMediaRecUserDataUpdate(p1, (len+24), 0, 1);
	sp5kMediaRecUserDataUpdate(vtagstr, len, 0, 1);

	sp5kFree(vtagstr);
	sp5kFree(buf);
}

#ifdef SUPPORT_GNSS_MSG
#define GNSS_LOST_COUNT_LIMIT 5
#define Knot 1852
/*-------------------------------------------------------------------------
 *  File Name : appGpsStrGNRMCProc
 *------------------------------------------------------------------------*/
UINT32 appGpsStrGNRMCProc(UINT8 *ptr)
{
	UINT8 pos=0;
	UINT8 *data;
	UINT32 i;
    
	for(i=0;i<GPS_INFO_LINE_MAX;i++,ptr++)
	{
		if(*ptr==',')
		{
    		data=ptr+1;
    		pos++;
    		switch(pos)
    		{
    			case 1:
    			if(*data!=',')
    			{
    			    strncpy(gGpsInfo.rmcInfo.gprmcTime,data,sizeof(gGpsInfo.rmcInfo.gprmcTime));
    			    gGpsInfo.rmcInfo.gprmcTime[6]='\0';
    		    }
    			break;
                
    			case 2:
    			if(*data!=',')
				{
                    if(*data=='A')
                    {
                        gnss_active=1;
                        gps_Status_Invalid_flag=0;
                        gnss_lost_count=0;
                        //printf("GNSS_ACTIVE");
                    }
                    else
                    {
                        gps_Status_Invalid_flag=1;
                        gnss_lost_count++;
                        if(gnss_lost_count>=GNSS_LOST_COUNT_LIMIT)
                        {
                        	gnss_active=0;
                        	//printf("GNSS LOST");
                        }
                        return gnss_active;
                    }
                    gGpsInfo.rmcInfo.gprmcSts=*data;
    			}
    			break;
                
    			case 3://
    			if(*data!=',')
    			{
    				strncpy(gGpsInfo.ggaInfo.gpggaLatitude,data,4);
    				strncpy(&gGpsInfo.ggaInfo.gpggaLatitude[4],data+5,4);
    				gGpsInfo.ggaInfo.gpggaLatitude[8]=0;
    			}
    			break;
                
    			case 4:
                if(*data!=',')
                {
                	gGpsInfo.ggaInfo.gpggaLatitudeInd=*data;
                }
                break;
                
    			case 5:
				if(*data!=',')
				{
					strncpy(gGpsInfo.ggaInfo.gpggaLongitude,data,5);
					strncpy(&gGpsInfo.ggaInfo.gpggaLongitude[5],data+6,4);
					gGpsInfo.ggaInfo.gpggaLongitude[9]=0;
				}
				break;
                
    			case 6:
                if(*data!=',')
                {
                    gGpsInfo.ggaInfo.gpggaLongitudeInd=*data;
                }
                break;
                
    			case 7:
				if(*data!=',')
				{
                    UINT32 speed;
                    if(data[1]=='.')
                    {
                    	speed=((data[0]-0x30)*Knot+(data[2]-0x30)*Knot/10+(data[3]-0x30)*Knot/100)/1000;
                    }
                    else if(data[2]=='.')
                    {
                    	speed=((data[0]-0x30)*Knot*10+(data[1]-0x30)*Knot+(data[3]-0x30)*Knot/10)/1000;
                    }
                    else if(data[3]=='.')
                    {
                    	speed=((data[0]-0x30)*Knot*100+(data[1]-0x30)*Knot*10+(data[2]-0x30)*Knot)/1000;
                    }
                    else if(data[4]=='.')
                    {
                        speed=((data[0]-0x30)*Knot*1000+(data[1]-0x30)*Knot*100+(data[2]-0x30)*Knot*10+(data[3]-0x30))/1000;
                    }
                    else
                    {
                    	speed=0;
                    }
                    sprintf(gGpsInfo.vtgInfo.gpvtgSpeed,"%03u",speed);
                    gGpsInfo.vtgInfo.gpvtgSpeed[3]=0;
                    //return gnss_active;
				}
				break;
                
    			case 9: /* Date (UTC) */
    			gGpsInfo.rmcInfo.gprmcDate[0]='2';
    			gGpsInfo.rmcInfo.gprmcDate[1]='0';
    			strncpy(&gGpsInfo.rmcInfo.gprmcDate[2],data,6);
    			gGpsInfo.rmcInfo.gprmcDate[8]='\0';
    			break;
                
    			default:
    				break;
    		}
	    }
	}
	return gnss_active;
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : appGpsInfoParse
 *------------------------------------------------------------------------*/
void
appGpsInfoParse(
	void
)
{
	#ifdef SUPPORT_GNSS_MSG
	sp5kOsMutexGet(&GNRMCmutex, TX_WAIT_FOREVER);
	if(gnss_rmc_recieved_flag&0x01)
	{
		gnss_rmc_recieved_flag&=~0x01;
        appGpsStrGNRMCProc(gnss_rmc);
	}
	else
	{
		if(gnss_active)
		{
			gps_Status_Invalid_flag=1;
			gnss_lost_count++;
			if(gnss_lost_count>=GNSS_LOST_COUNT_LIMIT)
			{
				gnss_active=0;
			}
		}
	}
	sp5kOsMutexPut(&GNRMCmutex);
    
	if(gnss_active)
		return;
	#endif
    
	if(strncmp(gga, "$GPGGA",6)==0){
		appGpsStrGPGGAProc(gga);
	}

	sp5kOsMutexGet(&GPSRMCmutex, TX_WAIT_FOREVER);
	if(strncmp(rmc, "$GPRMC",6)==0){
		appGpsStrGPRMCProc(rmc);
	}
	sp5kOsMutexPut(&GPSRMCmutex);

	if(strncmp(vtg, "$GPVTG",6)==0){
		appGpsStrGPVTGProc(vtg);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : HL_GPSgvsdata_Adjustment
 *------------------------------------------------------------------------*/
UINT8 bkgsvArray[(GPS_INFO_LINE_MAX+3)*3];
void HL_GPSgvsdata_Adjustment(UINT8* GPSInfoPtr, UINT32 size)
{

    UINT32 m,n;
    memset(bkgsvArray,0,sizeof(bkgsvArray));
    
    #if 0
    if(groupid==0)
    {
    	//	gsv0  --->$GPGSV,2,1,07,15,76,265,,26,59,030,,02,39,121,,05,38,041,*73
    	//	gsv1  --->$GPGSV,2,2,07,09,28,162,,21,15,318,,18,09,280,*4F
    	for(m=0,n=0;m< sizeof (gsv);m++)
    	{
    		if(m>4)
    		{
    			if(((gsv[m-1]==',')&&(gsv[m]==','))||((gsv[m-1]==',')&&(gsv[m]=='*')))
    			{
    				bkgsvArray[n]='0';
    				n++;
    				bkgsvArray[n]='0';
    				n++;
    			}
    		}
    		bkgsvArray[n]=gsv[m];
    		n++;
    	}
    	memcpy (gsv, bkgsvArray, sizeof (gsv));
    }
    else if(groupid==1)
    {
    		for(m=0,n=0;m< sizeof (gsv1);m++)
    	{

    		if(m>4)
    		{
    			if(((gsv1[m-1]==',')&&(gsv1[m]==','))||((gsv1[m-1]==',')&&(gsv1[m]=='*')))
    			{
    				bkgsvArray[n]='0';
    				n++;
    				bkgsvArray[n]='0';
    				n++;
    			}
    		}

    		bkgsvArray[n]=gsv1[m];
    		n++;
    	}
    	memcpy (gsv1, bkgsvArray, sizeof (gsv1));
    }
    else if(groupid==2)

    {
    		for(m=0,n=0;m< sizeof (gsv2);m++)
    	{

    		if(m>4)
    		{
    			if(((gsv2[m-1]==',')&&(gsv2[m]==','))||((gsv2[m-1]==',')&&(gsv2[m]=='*')))
    			{
    				bkgsvArray[n]='0';
    				n++;
    				bkgsvArray[n]='0';
    				n++;
    			}
    		}
    		bkgsvArray[n]=gsv2[m];
    		n++;
    	}
    	memcpy (gsv2, bkgsvArray, sizeof (gsv2));
    }
    #else
	//	gsv0  --->$GPGSV,2,1,07,15,76,265,,26,59,030,,02,39,121,,05,38,041,*73
	//	gsv1  --->$GPGSV,2,2,07,09,28,162,,21,15,318,,18,09,280,*4F
	for(m=0,n=0;m< size;m++)
	{
		if(m>4)
		{
			if(((GPSInfoPtr[m-1]==',')&&(GPSInfoPtr[m]==','))||((GPSInfoPtr[m-1]==',')&&(GPSInfoPtr[m]=='*')))
			{
				bkgsvArray[n]='0';
				n++;
				bkgsvArray[n]='0';
				n++;
			}
		}
		bkgsvArray[n]=GPSInfoPtr[m];
		n++;
	}
	if (size <= sizeof(bkgsvArray))
	{
		memcpy (GPSInfoPtr, bkgsvArray, size);
	}
	else
	{
	    printf("GpsInfo Buffer overflow!!");
	}
    #endif
}

/*-------------------------------------------------------------------------
 *  File Name : HL_appGpsInfoParse
 *------------------------------------------------------------------------*/
void HL_appGpsInfoParse(void)
{
	UINT8 GpsGSVInfo_tmp[GPS_INFO_LINE_MAX+3]={0};
	UINT8 SatellitesinViewCount1;
    
	if(gGpsInfo.gpsStatus==GPS_STOP)
	{
        gsvtotalMessagenumber=0;
        CurrentMessageIdex=0;
        gsvcount=0;
        SatellitesinViewCount=0;
        SatellitesinViewReadyProcCount=0;

        memset(gsv,0,sizeof(gsv));
        memset(gsv1,0,sizeof(gsv1));
        memset(gsv2,0,sizeof(gsv2));

        appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
        appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 36,320,(240-36), 0, 0);
        
        #ifdef SUPPORT_GNSS_MSG
        if(gsv_mode==0)
        sprintf(GpsGSVInfo_tmp,"GNSS total:%2d ",SatellitesinViewCount);
        else
        sprintf(GpsGSVInfo_tmp,"GPS total:%2d ",SatellitesinViewCount);
        #else
        sprintf(GpsGSVInfo_tmp,"total:%2d ",SatellitesinViewCount);
        #endif
        
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(20, 40, SP5K_GFX_ALIGN_TOP_LEFT, GpsGSVInfo_tmp);
		return;
	}

	SatellitesinViewReadyProcCount=0;
	if(SatellitesinViewCount>16)
    	SatellitesinViewCount1=16;
	else
	    SatellitesinViewCount1=SatellitesinViewCount;
    
	if(SatellitesinViewCount1<5)
	{
        sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
        memcpy (GpsGSVInfo_tmp, gsv, sizeof (GpsGSVInfo_tmp));
        sp5kOsMutexPut(&GPSGSVmutex);
        
        #ifdef SUPPORT_GNSS_MSG
        if((strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)||(strncmp(GpsGSVInfo_tmp, "$GLGSV",6)==0))
        #else
        if(strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)
        #endif
        {
            //printf("@@---proc  gsv0   SatellitesinViewCount=%d \n",SatellitesinViewCount);
            appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
            appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 80,280, 30, 0, 0);
            appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 120,280, 30, 0, 0);
            appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 160,280, 30, 0, 0);
            appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 200,280, 30, 0, 0);

            HL_GPSgvsdata_Adjustment(GpsGSVInfo_tmp,sizeof(GpsGSVInfo_tmp));
            appGpsStrGPGSVProc(GpsGSVInfo_tmp,1);
            HL_disp_gpGSV_show(1);
        }
	}
    else if((SatellitesinViewCount1<9)&&(SatellitesinViewCount1>4))
	{
		//printf("@@---proc  gsv0  and   gsv01---SatellitesinViewCount=%d \n",SatellitesinViewCount);
		appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 80,280, 30, 0, 0);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 120,280, 30, 0, 0);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 160,280, 30, 0, 0);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 200,280, 30, 0, 0);

		sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
		memcpy (GpsGSVInfo_tmp, gsv, sizeof (GpsGSVInfo_tmp));
		sp5kOsMutexPut(&GPSGSVmutex);

        #ifdef SUPPORT_GNSS_MSG
        if((strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)||(strncmp(GpsGSVInfo_tmp, "$GLGSV",6)==0))
        #else
        if(strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)
        #endif
        {
            HL_GPSgvsdata_Adjustment(GpsGSVInfo_tmp,sizeof(GpsGSVInfo_tmp));
            appGpsStrGPGSVProc(GpsGSVInfo_tmp,1);
            HL_disp_gpGSV_show(1);
        }

		sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
		memcpy (GpsGSVInfo_tmp, gsv1, sizeof (GpsGSVInfo_tmp));
		sp5kOsMutexPut(&GPSGSVmutex);

        #ifdef SUPPORT_GNSS_MSG
        if((strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)||(strncmp(GpsGSVInfo_tmp, "$GLGSV",6)==0))
        #else
        if(strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)
        #endif
        {
            HL_GPSgvsdata_Adjustment(GpsGSVInfo_tmp,sizeof(GpsGSVInfo_tmp));
            appGpsStrGPGSVProc(GpsGSVInfo_tmp,2);
            HL_disp_gpGSV_show(2);
        }
	}
	else if((SatellitesinViewCount1<13)&&(SatellitesinViewCount1>8))
	{
        //printf("@@---proc  gsv0  ,   gsv01  ,  gsv02---SatellitesinViewCount=%d \n",SatellitesinViewCount);
        appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
        appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 80,280, 30, 0, 0);
        appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 120,280, 30, 0, 0);
        appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 160,280, 30, 0, 0);
        appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 200,280, 30, 0, 0);

        sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
        memcpy (GpsGSVInfo_tmp, gsv, sizeof (GpsGSVInfo_tmp));
        sp5kOsMutexPut(&GPSGSVmutex);

        #ifdef SUPPORT_GNSS_MSG
        if((strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)||(strncmp(GpsGSVInfo_tmp, "$GLGSV",6)==0))
        #else
        if(strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)
        #endif
        {
            HL_GPSgvsdata_Adjustment(GpsGSVInfo_tmp,sizeof(GpsGSVInfo_tmp));
            appGpsStrGPGSVProc(GpsGSVInfo_tmp,1);
            HL_disp_gpGSV_show(1);
        }

        sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
        memcpy (GpsGSVInfo_tmp, gsv1, sizeof (GpsGSVInfo_tmp));
        sp5kOsMutexPut(&GPSGSVmutex);

        #ifdef SUPPORT_GNSS_MSG
		if((strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)||(strncmp(GpsGSVInfo_tmp, "$GLGSV",6)==0))
        #else
		if(strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)
        #endif
        {
            HL_GPSgvsdata_Adjustment(GpsGSVInfo_tmp,sizeof(GpsGSVInfo_tmp));
            appGpsStrGPGSVProc(GpsGSVInfo_tmp,2);
            HL_disp_gpGSV_show(2);
        }

		sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
		memcpy (GpsGSVInfo_tmp, gsv2, sizeof (GpsGSVInfo_tmp));
		sp5kOsMutexPut(&GPSGSVmutex);

        #ifdef SUPPORT_GNSS_MSG
        if((strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)||(strncmp(GpsGSVInfo_tmp, "$GLGSV",6)==0))
        #else
        if(strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)
        #endif
        {
            HL_GPSgvsdata_Adjustment(GpsGSVInfo_tmp,sizeof(GpsGSVInfo_tmp));
            appGpsStrGPGSVProc(GpsGSVInfo_tmp,3);
            HL_disp_gpGSV_show(3);
        }

    }
    else if((SatellitesinViewCount1<17)&&(SatellitesinViewCount1>12))
	{
		//printf("@@---proc  gsv0  ,   gsv01  ,  gsv02---SatellitesinViewCount=%d \n",SatellitesinViewCount);
		appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 80,280, 30, 0, 0);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 120,280, 30, 0, 0);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 160,280, 30, 0, 0);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 20, 200,280, 30, 0, 0);

  		sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
		memcpy (GpsGSVInfo_tmp, gsv, sizeof (GpsGSVInfo_tmp));
		sp5kOsMutexPut(&GPSGSVmutex);

        #ifdef SUPPORT_GNSS_MSG
        if((strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)||(strncmp(GpsGSVInfo_tmp, "$GLGSV",6)==0))
        #else
        if(strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)
        #endif
        {
            HL_GPSgvsdata_Adjustment(GpsGSVInfo_tmp,sizeof(GpsGSVInfo_tmp));
            appGpsStrGPGSVProc(GpsGSVInfo_tmp,1);
            HL_disp_gpGSV_show(1);
        }

        sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
        memcpy (GpsGSVInfo_tmp, gsv1, sizeof (GpsGSVInfo_tmp));
        sp5kOsMutexPut(&GPSGSVmutex);

        #ifdef SUPPORT_GNSS_MSG
		if((strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)||(strncmp(GpsGSVInfo_tmp, "$GLGSV",6)==0))
        #else
		if(strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)
        #endif
        {
            HL_GPSgvsdata_Adjustment(GpsGSVInfo_tmp,sizeof(GpsGSVInfo_tmp));
            appGpsStrGPGSVProc(GpsGSVInfo_tmp,2);
            HL_disp_gpGSV_show(2);
        }

		sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
		memcpy (GpsGSVInfo_tmp, gsv2, sizeof (GpsGSVInfo_tmp));
		sp5kOsMutexPut(&GPSGSVmutex);

        #ifdef SUPPORT_GNSS_MSG
        if((strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)||(strncmp(GpsGSVInfo_tmp, "$GLGSV",6)==0))
        #else
        if(strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)
        #endif
        {
            HL_GPSgvsdata_Adjustment(GpsGSVInfo_tmp,sizeof(GpsGSVInfo_tmp));
            appGpsStrGPGSVProc(GpsGSVInfo_tmp,3);
            HL_disp_gpGSV_show(3);
        }

        sp5kOsMutexGet(&GPSGSVmutex, TX_WAIT_FOREVER);
        memcpy (GpsGSVInfo_tmp, gsv3, sizeof (GpsGSVInfo_tmp));
        sp5kOsMutexPut(&GPSGSVmutex);

        #ifdef SUPPORT_GNSS_MSG
		if((strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)||(strncmp(GpsGSVInfo_tmp, "$GLGSV",6)==0))
        #else
		if(strncmp(GpsGSVInfo_tmp, "$GPGSV",6)==0)
        #endif
        {
			HL_GPSgvsdata_Adjustment(GpsGSVInfo_tmp,sizeof(GpsGSVInfo_tmp));
			appGpsStrGPGSVProc(GpsGSVInfo_tmp,4);
			HL_disp_gpGSV_show(4);
        }
	}
	gGpsInfo.gpsStatus=GPS_STOP;
}

/*-------------------------------------------------------------------------
 *  File Name : appUsrDataWrite
 *------------------------------------------------------------------------*/
void
appUsrDataWrite(
	void
)
{
	appRTC_t GPS_Update_RTCtime;

	udatWriteSts=0;
    
	if(gGpsInfo.gpsStatus == GPS_STOP)
    {
		#if 1 //feng
		UINT8 vtagstr = sp5kMalloc(512);
		memset(stamp_GPSSpeedBuf, 0 , sizeof(stamp_GPSSpeedBuf));
		sprintf(stamp_GPSSpeedBuf,"%3d%s",0,"km/h");
		memset(vtagstr, 0 , sizeof(vtagstr));
        
		#if 0
		appGSensorInfoGet(vtagstr,16);
		//printf("G_senorInfo  %s\n",vtagstr);
		#endif
		#endif
        
	    HL_gpsStatus_flag=1;
		gps_Status_Invalid_flag=1;

		#if 1
		GPSledflag=0;
		HL_GPS_STATUS_LED_GPSIcon_init();
		#endif
        
		gps_update_RtcTime_flag=0;
		memset(gga,0,sizeof(gga));
		memset(rmc,0,sizeof(rmc));
		memset(vtg,0,sizeof(vtg));
		printf("rmc => %s\n", rmc);

		#if 1
		HL_appGpsInfoWrite2Mov_whenGPS_stop();
		appG_senorInfoWrite2Mov();
		#endif

		sp5kFree(vtagstr);

		return;
	}

	HL_gpsStatus_flag=0;

	/*gps  work status  gpsled  stauts*/
	/*The function update new gps information*/
	HL_GPS_STATUS_LED_GPSIcon_init();

	appGpsInfoParse();

	if((gps_Status_Invalid_flag==0)&&(gGpsInfo.rmcInfo.gprmcSts==0x41))
  	{
		appGpsInfoWrite2Mov();

		#if 1
		if(gps_update_RtcTime_flag==0)
		{
			RTCTimeDataCheckCout++;
			if(RTCTimeDataCheckCout>5)
			{
				/*update RTC time*/
				HL_GPS_time_string_ASCII_Get(&GPS_Update_RTCtime);
				appTimezone(&GPS_Update_RTCtime,pUiSetting->TimezoneSTATUS);
				appRtcSet(&GPS_Update_RTCtime);
				RTCTimeDataCheckCout=0;
				gps_update_RtcTime_flag=1;
			}
		}
		#endif
		HL_GPS_STATUS_LED_GPSIcon_Proc();
  	}
  	else
  	{
		HL_appGpsInfoWrite2Mov_whenGPSinfoError();
  	}

	appG_senorInfoWrite2Mov();
	gGpsInfo.gpsStatus = GPS_STOP;
	udatWriteSts=1;
	return ;
}

/*-------------------------------------------------------------------------
 *  File Name : appGpsInfoGet
 *------------------------------------------------------------------------*/
UINT8
appGpsInfoGet(
	UINT8 id,
	UINT8 buf[]
)
{
	UINT8 len=0;

	switch(id){
		case GPS_INFO_LATITUDE:
			len+=8;
			strncpy(buf,gGpsInfo.rmcInfo.gprmcLatitude,len);
			buf+=len;
			*buf = '\0';
			break;
		case GPS_INFO_LATITUDE_IND:
			len=1;
			*buf=gGpsInfo.rmcInfo.gprmcLatitudeInd;
			buf+=len;
			*buf = '\0';
			break;
		case GPS_INFO_LONGITUDE:
			len+=9;
			strncpy(buf,gGpsInfo.rmcInfo.gprmcLongitude,len);
			buf+=len;
			*buf = '\0';
			break;
		case GPS_INFO_LONGITUDE_IND:
			len=1;
			*buf=gGpsInfo.rmcInfo.gprmcLongitudeInd;
			buf+=len;
			*buf = '\0';
			break;
		case GPS_INFO_PFI:
			len=1;
			*buf=gGpsInfo.ggaInfo.gpggaPFI;
			buf+=len;
			*buf = '\0';
			break;
		case GPS_INFO_MSLALTITUDE:
			len+=5;
			strncpy(buf,gGpsInfo.ggaInfo.gpggaMSLAltitude,len);
			buf+=len;
			*buf = '\0';
			break;
		case GPS_INFO_SPEED:
			len+=3;
			strncpy(buf,gGpsInfo.vtgInfo.gpvtgSpeed,len);
			buf+=len;
			*buf = '\0';
			break;
		default:break;
	}

	return len;
}

/*-------------------------------------------------------------------------
 *  File Name : HL_GPS_STATUS_FLAG_Proc
 *------------------------------------------------------------------------*/
void HL_GPS_STATUS_FLAG_Proc(void)
{
	appRTC_t GPS_Update_RTCtime;

	#if 1//
	//if((HL_gps_isexist_flag==GPS_STOP)&&(appActiveStateGet()==APP_STATE_VIDEO_PREVIEW))
	if(HL_gps_isexist_flag==GPS_STOP)
	{
        #ifdef SUPPORT_GNSS_MSG
        gnss_active=0;
        #endif
        
		memset(rmc, 0 , sizeof(rmc));
		gps_Status_Invalid_flag=1;

        #if 1
		GPSledflag=0;
	    HL_GPS_STATUS_LED_GPSIcon_init();
		#endif
        
		gps_update_RtcTime_flag=0;//?路??PS赂锟解?	//   	printf("@@-HL----videoview --- gps stop---close gps flashing led light \n ");
	    return;
	}
	#endif

    HL_GPS_STATUS_LED_GPSIcon_init();

    #ifdef SUPPORT_GNSS_MSG
	sp5kOsMutexGet(&GNRMCmutex, TX_WAIT_FOREVER);

    if(gnss_rmc_recieved_flag&0x01)
    {
        gnss_rmc_recieved_flag&=~0x01;
        appGpsStrGNRMCProc(gnss_rmc);
    }
	else
	{
        if(gnss_active)
    	{
    		gps_Status_Invalid_flag=1;
    		gnss_lost_count++;
    		if(gnss_lost_count>=GNSS_LOST_COUNT_LIMIT)
			{
				gnss_active=0;
				HL_Video_view_GPS_icon_show(0);
			}
    	}
	}
	sp5kOsMutexPut(&GNRMCmutex);
	#endif
    
    #ifdef SUPPORT_GNSS_MSG
    if(gnss_active==0)
    #endif
    {
    	sp5kOsMutexGet(&GPSRMCmutex, TX_WAIT_FOREVER);
    	if(strncmp(rmc, "$GPRMC",6)==0){
    		appGpsStrGPRMCProc(rmc);
    	}
    	sp5kOsMutexPut(&GPSRMCmutex);
    }
    
	#if 1
	if((gps_Status_Invalid_flag==0)&&(gGpsInfo.rmcInfo.gprmcSts==0x41))
	{
		if(gps_update_RtcTime_flag==0)
		{
			RTCTimeDataCheckCout++;
			if(RTCTimeDataCheckCout>5)
			{
				/*update RTC time*/
				HL_GPS_time_string_ASCII_Get(&GPS_Update_RTCtime);
				appTimezone(&GPS_Update_RTCtime,pUiSetting->TimezoneSTATUS);
				appRtcSet(&GPS_Update_RTCtime);
				RTCTimeDataCheckCout=0;
				gps_update_RtcTime_flag=1;
			}
		}

        HL_GPS_STATUS_LED_GPSIcon_Proc();
        gps_Status_Invalid_flag=1;
        HL_gps_isexist_flag = GPS_STOP;
	}
	#endif
}
