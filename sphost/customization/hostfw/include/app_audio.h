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

#ifndef APP_AUDIO_API_H
#define APP_AUDIO_API_H
#include "customization/app_init.h"


/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define VIDEO_REC_MUTE_AT_LENS_ZOOMING
		/*-- To control muting audio rec while optical Lens is moving
		 *   while VIDEO REC.
		 */

#define AUDIO_REC_SAMPLING_RATE		(AUDIO_SAMPLE_RATE)	/*- samples per sec */
#define AUDIO_REC_SAMPLE_BITS		(16)		/*- bits per sample */

#if 0
#define AUDIO_REC_STATUS_STOP 0
#define AUDIO_REC_STATUS_RECORDING 1
#define AUDIO_REC_STATUS_PAUSE 2
#else
typedef enum {
	AUDIO_REC_STATUS_NULL,
	AUDIO_REC_STATUS_STOP,
	AUDIO_REC_STATUS_RECORD_STARTING,
	AUDIO_REC_STATUS_RECORDING,
	AUDIO_REC_STATUS_PAUSE,
} appAudioRecStatus_t;
#endif

#define AUDIO_REC_STATE_INIT 0
#define AUDIO_REC_STATE_CLOSE 1

/*#define AUDIO_ONE_SECOND_FILE_SIZE 8  //in K byte*/
#define AUDIO_ONE_SECOND_FILE_SIZE ((AUDIO_REC_SAMPLING_RATE*(AUDIO_REC_SAMPLE_BITS>>3))>>10) /* in K byte*/

#define CAN_I_START_AUDIO_RECORD	0
#define CAN_I_LEAVE_AUDIO	1
#define AUDIO_STATE_YES 0
#define AUDIO_STATE_NO 	1
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appAudioCB_s {
	UINT32 recStatus;
	UINT32 recTimer;
	UINT32 deferredStopTimer;
	UINT32 recSec;
	UINT32 osd;
	UINT32 state;
	UINT32 s1press;
	UINT32 err;
	UINT16 isDiskReady;
	UINT16 isDiskError;
} appAudioCB_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
extern appAudioCB_t gAudioCB;
/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appAudioOsdShow(UINT32 clr);
void appAudioOsdProgressBarDraw(UINT8 bShow,UINT32 CurrentTime_msec, UINT32 TotalTime_msec);
UINT32 appAudioDiskRemainGet(void);
void appAudioRecVolumeRestore(void);
void appAudioRecVolumeMute(void);
void appAudioRecVolumeConfig(void);
void appAudioOsdDiskShow(void);
void appAudioOsdInit(void);
void appAudioOsdStatusDraw(UINT8 Audiostatus);
void appAudioOsd_BatteryDraw(UINT8 bShoyw, UINT8 idx);
void appAudioOsdRecRemainingShow(UINT32 remainingSec);
void appAudioOsdElsTimeShow(void);
#endif  /* APP_VIDEO_API_H */


