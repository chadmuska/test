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
#ifndef APP_RTC_H
#define APP_RTC_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define APP_RTC_RELIABLE_CODE    0x5A

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appRTC_s {
	UINT16 year;
	UINT16 mon;
	UINT16 day;
	UINT16 hour;
	UINT16 min;
	UINT16 sec;
} appRTC_t;

typedef struct appTZ_s {
	UINT32 tzsigned;
	UINT32 hour;
	UINT32 min;
} appTZ_t;
/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appUtcTimeZoneSet(appTZ_t *tzoffset);
void appUtcTimeZoneGet(appTZ_t *tzoffset);
void appShiftTimeZone(BOOL shift, tmx_t *rtc);
void appLocalTimeSet(struct tm *ptime);
void appRtcDefaultSet(void);
void appRtcSet(appRTC_t *ptime);
void appRealRtcSet(tmx_t *ptime);
void appRtcWakeUpSet(appRTC_t *ptime);
void appRtcGet(appRTC_t *ptime);
BOOL appRtcLostCheck(void);
BOOL appLeapYearCheck(UINT16 year);
void appRtcYearCheck(void);
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

#endif  /* APP_RTC_H*/

