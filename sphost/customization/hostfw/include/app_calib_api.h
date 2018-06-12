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
#ifndef APP_CALIB_API_H
#define APP_CALIB_API_H
#include "api/sp5k_ae_api.h"
#include "api/sp5k_awb_api.h"
#include "app_lens_api.h"
/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define CALIB_EV_UNIT		(16)
#define CALIB_SV_UNIT		(CALIB_EV_UNIT)
#define CALIB_TV_UNIT		(CALIB_EV_UNIT)

#define CALIB_ISO_50		(CALIB_ISO_100 - CALIB_SV_UNIT/2)
#define CALIB_ISO_100		(80)
#define CALIB_ISO_200		(CALIB_ISO_100 + CALIB_SV_UNIT)
#define CALIB_ISO_400		(CALIB_ISO_200 + CALIB_SV_UNIT)
#define CALIB_ISO_800		(CALIB_ISO_400 + CALIB_SV_UNIT)
#define CALIB_ISO_1600		(CALIB_ISO_800 + CALIB_SV_UNIT)

#define CALIB_EXP_16_01	(CALIB_EXP_8_01 - CALIB_TV_UNIT)
#define CALIB_EXP_8_01	(CALIB_EXP_4_01 - CALIB_TV_UNIT)
#define CALIB_EXP_4_01	(CALIB_EXP_2_01 - CALIB_TV_UNIT)
#define CALIB_EXP_2_01	(CALIB_EXP_1_01 - CALIB_TV_UNIT)

#define CALIB_EXP_1_01	(CALIB_EXP_1_02 - CALIB_TV_UNIT) // 0
#define CALIB_EXP_1_02	(CALIB_EXP_1_04 - CALIB_TV_UNIT) // 16

#define CALIB_EXP_1_04	(CALIB_EXP_1_08 - CALIB_TV_UNIT) // 32
#define CALIB_EXP_1_08	(CALIB_EXP_1_16 - CALIB_TV_UNIT) // 48
#define CALIB_EXP_1_16	(CALIB_EXP_1_32 - CALIB_TV_UNIT) // 64
#define CALIB_EXP_1_24	(CALIB_EXP_1_32 - (CALIB_TV_UNIT / 2))
#define CALIB_EXP_1_32	(80)
#define CALIB_EXP_1_64	(CALIB_EXP_1_32 + CALIB_TV_UNIT) //96
#define CALIB_EXP_1_128	(CALIB_EXP_1_64 + CALIB_TV_UNIT) //112
#define CALIB_EXP_1_256	(CALIB_EXP_1_128 + CALIB_TV_UNIT) //128
#define CALIB_EXP_1_512	(CALIB_EXP_1_256 + CALIB_TV_UNIT) //144
#define CALIB_EXP_1_1024	(CALIB_EXP_1_512 + CALIB_TV_UNIT) //160
#define CALIB_EXP_1_2048	(CALIB_EXP_1_1024 + CALIB_TV_UNIT) //176
#define CALIB_EXP_1_4096	(CALIB_EXP_1_2048 + CALIB_TV_UNIT) // 192
#define CALIB_EXP_1_8192	(CALIB_EXP_1_4096 + CALIB_TV_UNIT)

//#define DX	48 /* 2*N */
//#define DY	40 /* FIELD_NO*2*N */

#define RAW_AREA_RATIO		2 /* get center area */


#define CALIB_AE_DEFAULT_OB	1000
#define CALIB_AE_DEFAULT_SHUTTER_EARLY_1 	360
#define CALIB_AE_DEFAULT_SHUTTER_EARLY_2		390

#define	CALIB_WB_DEFAULT_RGAIN	1750
#define	CALIB_WB_DEFAULT_BGAIN	1950

#define CALIB_BP_CAP_MAX		30000
#define CALIB_BP_VIEW_THR_R	2048	/* 16 bits */
#define CALIB_BP_VIEW_THR_Gr	2048
#define CALIB_BP_VIEW_THR_Gb	2048
#define CALIB_BP_VIEW_THR_B	2048
#define CALIB_BP_CAP_THR_R		2048
#define CALIB_BP_CAP_THR_Gr	2048
#define CALIB_BP_CAP_THR_Gb	2048
#define CALIB_BP_CAP_THR_B		2048
#define CALIB_BP_white_Luma 	200  /*8bit*/
#define CALIB_BP_black_Luma 	40 
/*BP Error code*/
#define CALIB_BP_ERR_tooBright 0x02
#define CALIB_BP_ERR_tooDark  0x03


#if SP5K_SENSOR_SONYIMX175
#define CALIB_DEFAULT_OB   ((UINT32)16<<4)//7002 sony FAE setting 16

#elif SP5K_SENSOR_TG_AD9000_SHARPRJ23V3B
#define CALIB_DEFAULT_OB   50

#elif SP5K_SENSOR_TG_AD9000_SHARPRJ23U3B
#define CALIB_DEFAULT_OB   50

#elif SP5K_SENSOR_TG_AD9000_SONY675
#define CALIB_DEFAULT_OB   50

#elif SP5K_SENSOR_TG_PANX12081A_MN34510
#define CALIB_DEFAULT_OB   50

#elif SP5K_SENSOR_MI9130 /* A900 */
#define CALIB_DEFAULT_OB   50

#elif SP5K_SENSOR_OV5610
#define CALIB_DEFAULT_OB   50

#elif SP5K_SENSOR_OV5620 /* A531*/
#define CALIB_DEFAULT_OB   50

#elif SP5K_SENSOR_OV9810
#define CALIB_DEFAULT_OB   50

#elif SP5K_SENSOR_OV5633
#define CALIB_DEFAULT_OB   50

#elif SP5K_SENSOR_MT9J003
#define CALIB_DEFAULT_OB   38

#else
#define CALIB_DEFAULT_OB   50

#endif


typedef enum {
	CAL_PV_OB_SAVE_INFO,
	CAL_PV_OB_INIT_EXP,
	CAL_PV_OB_MIN_EXP,
	CAL_PV_OB_AGC_IDX,
	CAL_PV_OB_TAG_LUMA,
} calPvOb_t;


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appCalibData_s{
	UINT8 ObSign;
	UINT8 ShutterSign;
	UINT8 IsoSign;
	UINT8 WbSign;
	UINT8 PvDPCSign;
	UINT8 CapDPCSign;
	UINT8 PvRawSign;
	UINT8 StillRawSign;
	UINT8 CIPASign;
   	UINT8 capSatGainSign;
	UINT8 rawBatchEnable;
	UINT8 prefhStamp;
	UINT8 afFullScan;
	UINT8 IsoObSign;
	UINT8 pvOBSign;
	UINT8 pcApiTestSign;
	UINT8 videoRecDateStamp;
	UINT8 dzoomEnable;
	UINT8 DualSensorSign;	
	UINT8 aedebug;
	UINT8 awbdebug;

	SINT16 Ob[5];
	UINT16 Flash;
	SINT16 PvIso;
	SINT16 CapIso;
	UINT16 ShutterEarly[2];
	UINT16 WbRgain;
	UINT16 WbBgain;
	SINT16 sensDiff;
	SINT16 irisDiff;
	SINT16 capSatGain;
	SINT16 isoOb[8][5];  //ISO:50/100/200/400/800/1600/3200/6400
	UINT16 CIPACapCnt;
	SINT16 pvOB[4];
	UINT16 DualSensorPri[4];
	UINT16 DualSensorSec[4];
}__attribute__((packed)) appCalibData_t;;


typedef struct appAfCalibData_s{
	UINT16	AfPosition[7];
}appAfCalibData_t;
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/


/**************************************************************************
 *          E X T E R N V A R I A B L E S   D E C L A R A T I O N S         *
 **************************************************************************/


/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
#if KIT_CALIB_ENABLE
appCalibData_t* appCalibDataGet();
void appCalibInit(void);
void appCalibAwb();
void appCalibOb();
void appCalibIso();
void appCalibIsoOb();
void appCalibMShutter();
void appCalibViewBp();
void appCalibCaptureBp();
void appCalibDualSensor();
void appCalibViewRaw();
void appCalibStillRaw();
void appCalibLSC(void);
 void appCalibPVOB(void);
void cmdCalib(UINT32 argc,UINT8 *arg[], UINT32 v[]);

UINT8 appCalibUsbSettingGet(void);
UINT8 appCalibRawBatchEnableGet(void);

UINT32 appCalibDataLoad(void);
void appCalibDataSave(void);

UINT32 appCalibRawCb(UINT32 addr, UINT32 fmt, void *imgSize);
void appGetRawAvgRGB(UINT16 *r, UINT16 *g, UINT16 *b);

void appCalibFlashState(UINT32 msg,	UINT32 param);

UINT32 appCalibRawCbAvoidOverflowSet(UINT32 enable);

UINT8 appCalibAPITestSettingGet(void);
void appCalibAPITestSettingSet(UINT8 enable);

UINT8 appCalibVideoStampSettingGet(void);
void appCalibVideoStampSettingSet(UINT8 enable);

UINT8 appCalibDzoomEnableGet(void);
void appCalibDzoomEnableSet(UINT8 enable);
#else

#define appCalibRawBatchEnableGet() FALSE
#define appCalibDataLoad() FAIL
#define appCalibDataSave()
#define appCalibAPITestSettingGet() 0
#define appCalibAPITestSettingSet(a)
#endif
#endif  /* APP_CALIB_API_H */

