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
#include "app_rtc.h"
#include "sp5k_global_api.h"
#include "app_ui_para.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define JET_RTC_DEFAULT_YEAR 2014
#define JET_RTC_DEFAULT_MONTH 1
#define JET_RTC_DEFAULT_DAY 1
#endif
#define MINSPERHOUR	60
#define HOURSPERDAY	24
#define MONSPERYEAR	12
const unsigned char days_in_months[MONSPERYEAR] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 /*common years*/
};

/*default (GMT+08:00)*/
static appTZ_t tzCurOffset = {
	.tzsigned	= 1, /*1:positive, 2:negative*/
	.hour		= 8,
	.min		= 0,
};

void
appUtcTimeZoneSet(
	appTZ_t *tzoffset
)
{
	uiPara_t* puiPara = appUiParaGet();

	puiPara->tzSigned = tzoffset->tzsigned;
	puiPara->tzHour = tzoffset->hour;
	puiPara->tzMin = tzoffset->min;
	DBG_PRINT("%s, (%d)hour%d min%d\n",__func__,puiPara->tzSigned,puiPara->tzHour,puiPara->tzMin);
}

void
appUtcTimeZoneGet(
	appTZ_t *tzoffset
)
{
	*tzoffset = tzCurOffset;
}

void
appShiftTimeZone(
	BOOL revertshift,
	tmx_t *rtc
)
{
	uiPara_t* puiPara = appUiParaGet();

	if(puiPara->tzSigned==1 || puiPara->tzSigned==2)
	{
		tzCurOffset.tzsigned = puiPara->tzSigned;
		tzCurOffset.hour = puiPara->tzHour;
		tzCurOffset.min = puiPara->tzMin;
	}
	DBG_PRINT("%s, (%d)hour%d min%d\n",__func__,tzCurOffset.tzsigned,tzCurOffset.hour,tzCurOffset.min);
	if(tzCurOffset.tzsigned == 2) {
		revertshift = !revertshift;
	}

	if(revertshift){ /*convert Local time to UTC time*/
		rtc->tmx_min  = rtc->tmx_min-tzCurOffset.min;
		if(rtc->tmx_min < 0){
			rtc->tmx_min = rtc->tmx_min + MINSPERHOUR;
			rtc->tmx_hour = rtc->tmx_hour - 1;
		}

		rtc->tmx_hour  = rtc->tmx_hour-tzCurOffset.hour;
		if(rtc->tmx_hour < 0){
			rtc->tmx_hour = rtc->tmx_hour + HOURSPERDAY;
			rtc->tmx_mday = rtc->tmx_mday - 1;
		}

		if(rtc->tmx_mday == 0){
			rtc->tmx_mon  = rtc->tmx_mon - 1;
			if(rtc->tmx_mon == 0){
				rtc->tmx_mon = MONSPERYEAR;
			}
			rtc->tmx_mday = days_in_months[rtc->tmx_mon-1];
		}

	}
	else{ /*convert UTC time to Local time*/
		rtc->tmx_min  = rtc->tmx_min+tzCurOffset.min;
		if(rtc->tmx_min >= MINSPERHOUR){
			rtc->tmx_min = rtc->tmx_min - MINSPERHOUR;
			rtc->tmx_hour = rtc->tmx_hour + 1;
		}

		rtc->tmx_hour  = rtc->tmx_hour+tzCurOffset.hour;
		if(rtc->tmx_hour >= HOURSPERDAY){
			rtc->tmx_hour = rtc->tmx_hour - HOURSPERDAY;
			rtc->tmx_mday = rtc->tmx_mday + 1;
		}

		if(rtc->tmx_mday > days_in_months[rtc->tmx_mon-1]){
			rtc->tmx_mon  = rtc->tmx_mon + 1;
			if(rtc->tmx_mon > MONSPERYEAR){
				rtc->tmx_mon = 1;
			}
			rtc->tmx_mday = 1;
		}

	}
}

void
appLocalTimeSet(
	struct tm *ptime
)
{
	tmx_t rtc;

	rtc.tmx_year = ptime->tm_year;
	rtc.tmx_mon  = ptime->tm_mon+1; /*tmx_t.tmx_mon:1~12*/
	rtc.tmx_mday = ptime->tm_mday;
	rtc.tmx_hour = ptime->tm_hour;
	rtc.tmx_min  = ptime->tm_min;
	rtc.tmx_sec  = ptime->tm_sec;

	appShiftTimeZone(0,&rtc);

	ptime->tm_year = rtc.tmx_year;
	ptime->tm_mon  = rtc.tmx_mon-1; /*tm.tm_mon:0~11*/
	ptime->tm_mday = rtc.tmx_mday;
	ptime->tm_hour = rtc.tmx_hour;
	ptime->tm_min  = rtc.tmx_min ;
	ptime->tm_sec  = rtc.tmx_sec ;
}

BOOL
appRtcLostCheck(
	void
)
{
	UINT32 rtcReliable = 0;

	profLogAdd(0, "rtc: check");

	sp5kRtcReliableGet(&rtcReliable);
	if (rtcReliable != APP_RTC_RELIABLE_CODE && rtcReliable != 0x5b){
        HOST_PROF_LOG_ADD(LEVEL_WARN, "rtc: lost");
		return TRUE;
	}
	#if HOST_DBG
	else {
		appRTC_t tt;
		appRtcGet(&tt);
		DBG_PRINT("rtc: %d : %d : %d : %d : %d : %d\n\n",tt.year, tt.mon,tt.day,tt.hour,tt.min,tt.sec);
	}
	#endif
	return FALSE;
}

void
appRtcSet(
	appRTC_t *ptime
)
{
	tmx_t rtc;

	rtc.tmx_year = ptime->year-1900;
	rtc.tmx_mon  = ptime->mon;
	rtc.tmx_mday = ptime->day;
	rtc.tmx_hour = ptime->hour;
	rtc.tmx_min  = ptime->min;
	rtc.tmx_sec  = ptime->sec;
	appShiftTimeZone(1,&rtc);

	printf("%s,(1900+%d)/%d/%d %d:%d\n",__func__,rtc.tmx_year,rtc.tmx_mon,rtc.tmx_mday,rtc.tmx_hour,rtc.tmx_min);
	sp5kRtcDateTimeSet( SP5K_DATE_TIME_OPTION, &rtc);
}

void
appRealRtcSet(
	tmx_t *localtime
)
{
	tmx_t rtc;

	rtc.tmx_year = localtime->tmx_year;
	rtc.tmx_mon  = localtime->tmx_mon;
	rtc.tmx_mday = localtime->tmx_mday;
	rtc.tmx_hour = localtime->tmx_hour;
	rtc.tmx_min  = localtime->tmx_min;
	rtc.tmx_sec  = localtime->tmx_sec;

	appShiftTimeZone(1,&rtc);

	DBG_PRINT("%s,(1900+%d)/%d/%d %d:%d\n",__func__,rtc.tmx_year,rtc.tmx_mon,rtc.tmx_mday,rtc.tmx_hour,rtc.tmx_min);

	sp5kRtcDateTimeSet( SP5K_DATE_TIME_OPTION, &rtc);
}

void
appRtcWakeUpSet(
	appRTC_t *ptime
)
{
	tmx_t rtc;

	rtc.tmx_year = ptime->year-1900;
	rtc.tmx_mon  = ptime->mon;
	rtc.tmx_mday = ptime->day;
	rtc.tmx_hour = ptime->hour;
	rtc.tmx_min  = ptime->min;
	rtc.tmx_sec  = ptime->sec;

	appShiftTimeZone(1,&rtc);

	sp5kRtcDateTimeSet( SP5K_WAKEUP_OPTION, &rtc);
}

void
appRtcGet(
	appRTC_t *ptime
)
{
	tmx_t rtc;
	sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &rtc);

	ptime->year = rtc.tmx_year+1900;
	ptime->mon  = rtc.tmx_mon;
	ptime->day  = rtc.tmx_mday;
	ptime->hour = rtc.tmx_hour;
	ptime->min  = rtc.tmx_min;
	ptime->sec  = rtc.tmx_sec;
	DBG_PRINT("%s, (1900+%d)/%d/%d %d:%d\n",__func__,rtc.tmx_year,rtc.tmx_mon,rtc.tmx_mday,rtc.tmx_hour,rtc.tmx_min);
}

BOOL
appLeapYearCheck(
	UINT16 year
)
{

	/* if mutiple of 4 */
	if ( year % 4 ) {
		return FALSE;
	}
	/* if mutiple of 100, but not 400 */
	if ( year % 100 == 0 ) {
		if ( year % 400 ) {
			return FALSE;
		}
	}
	return TRUE;
}

void
appRtcDefaultSet(
	void
)
{
#if DBG_PRINT_EN
	uiPara_t* puiPara = appUiParaGet();
#endif
	appRTC_t time;

	profLogAdd(0, "rtc: reset");
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	time.year = JET_RTC_DEFAULT_YEAR;
	time.mon = JET_RTC_DEFAULT_MONTH;
	time.day = JET_RTC_DEFAULT_DAY;
#else
	time.year = 2017;
	time.mon = 6;
	time.day = 1;
#endif
	time.hour = 0;
	time.min = 0;
	time.sec = 0;
	DBG_PRINT("%s, (1900+%d)/%d/%d %d:%d\n",__func__,puiPara->rtcYear,puiPara->rtcMonth,puiPara->rtcDay,puiPara->rtcHour,puiPara->rtcMinute);
	appRtcSet(&time);
}

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

void appRtcYearCheck(void){
	tmx_t time;
	
	sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &time);
	if(time.tmx_year+1900 > JET_RTC_DEFAULT_YEAR+20 )
		appRtcDefaultSet();
}
#endif
