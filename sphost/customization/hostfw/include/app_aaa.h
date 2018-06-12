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

#ifndef APP_AAA_H
#define APP_AAA_H
#include "api/sp5k_ae_api.h"
#include "api/sp5k_awb_api.h"
#include "api/sp5k_cdsp_api.h"


/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

typedef enum _APPAE_PARA_METERING{
	AE_METERING_CENTER,
	AE_METERING_MULTI,
	AE_METERING_SPOT,
	AE_METERING_MAX,
} APPAE_PARA_METERING;

#define AE_ZOOM_LEVEL_MAX	7
#define AE_BIAS_MAX			13

#define AV_UNIT		1
#define EV_UNIT		16
#define SV_UNIT		EV_UNIT
#define TV_UNIT		EV_UNIT
#define SV_UNIT		EV_UNIT

#define AE_LV10		160
#define AE_LV11		(AE_LV10 + EV_UNIT)
#define AE_LV12		(AE_LV11 + EV_UNIT)
#define AE_LV13		(AE_LV12 + EV_UNIT)
#define AE_LV14		(AE_LV13 + EV_UNIT)
#define AE_LV15		(AE_LV14 + EV_UNIT)

/*****************************************************************
 * STD ISO/EXP, prefix with STD_ISO_ & STD_EXP_
 *****************************************************************/
#define STD_ISO_25		(STD_ISO_50 - SV_UNIT)			// 48
#define STD_ISO_50		(STD_ISO_100 - SV_UNIT)			// 64
#define STD_ISO_100		(5*SV_UNIT)						// 80
#define STD_ISO_200		(STD_ISO_100 + SV_UNIT)			// 96
#define STD_ISO_400		(STD_ISO_200 + SV_UNIT)			// 112
/*s Add by Aries 09/08/11*/
#define STD_ISO_640		(STD_ISO_400 + (SV_UNIT/2))		// 120
/*e Add by Aries 09/08/11*/
#define STD_ISO_800		(STD_ISO_400 + SV_UNIT)			// 128
#define STD_ISO_1600	(STD_ISO_800 + SV_UNIT)			// 144

#define STD_EXP_16_01	(STD_EXP_8_01 - TV_UNIT)
#define STD_EXP_8_01	(STD_EXP_4_01 - TV_UNIT)
#define STD_EXP_4_01	(STD_EXP_2_01 - TV_UNIT)
#define STD_EXP_2_01	(STD_EXP_1_01 - TV_UNIT)

#define STD_EXP_1_01	(STD_EXP_1_02 - TV_UNIT)		// 0
#define STD_EXP_1_02	(STD_EXP_1_04 - TV_UNIT)		//16

#define STD_EXP_1_04	(STD_EXP_1_08 - TV_UNIT) 		//32
#define STD_EXP_1_08	(STD_EXP_1_16 - TV_UNIT)		//48
#define STD_EXP_1_16	(STD_EXP_1_32 - TV_UNIT)		//64
#define STD_EXP_1_24	(STD_EXP_1_32 - (TV_UNIT / 2))	//72
#define STD_EXP_1_32	(5*TV_UNIT)						//80
#define STD_EXP_1_64	(STD_EXP_1_32 + TV_UNIT)		//96
#define STD_EXP_1_128	(STD_EXP_1_64 + TV_UNIT)		//112
#define STD_EXP_1_256	(STD_EXP_1_128 + TV_UNIT)		//128
#define STD_EXP_1_512	(STD_EXP_1_256 + TV_UNIT)		//144
#define STD_EXP_1_1024	(STD_EXP_1_512 + TV_UNIT)		//160
#define STD_EXP_1_2048	(STD_EXP_1_1024 + TV_UNIT)		//176
#define STD_EXP_1_4096	(STD_EXP_1_2048 + TV_UNIT)		//192
#define STD_EXP_1_8192	(STD_EXP_1_4096 + TV_UNIT)		//208

/*****************************************************************
 * VIEW ISO/EXP, prefix with PV_ISO_ & PV_EXP_
 *****************************************************************/

/*****************************************************************
 * CAPTURE ISO/EXP, prefix with CAP_ISO_ & CAP_EXP_
 *****************************************************************/
#define ISO_NUM		6
#define ISO_50		(ISO_100 - SV_UNIT)
#define ISO_75		(ISO_100 - SV_UNIT/2)
#define ISO_100		(80)
#define ISO_200		(ISO_100 + SV_UNIT)
#define ISO_400		(ISO_200 + SV_UNIT)
#define ISO_800		(ISO_400 + SV_UNIT)
#define ISO_1600	(ISO_800 + SV_UNIT)

#define EXP_16_01	(EXP_8_01 - TV_UNIT)
#define EXP_8_01	(EXP_4_01 - TV_UNIT)
#define EXP_4_01	(EXP_2_01 - TV_UNIT)
#define EXP_2_01	(EXP_1_01 - TV_UNIT)

#define EXP_1_01	(EXP_1_02 - TV_UNIT) // 0
#define EXP_1_02	(EXP_1_04 - TV_UNIT) // 16

#define EXP_1_04	(EXP_1_08 - TV_UNIT) // 32
#define EXP_1_08	(EXP_1_16 - TV_UNIT) // 48
#define EXP_1_16	(EXP_1_32 - TV_UNIT) // 64
#define EXP_1_20	(EXP_1_32 - 11)
#define EXP_1_24	(EXP_1_32 - (TV_UNIT / 2))
#define EXP_1_32	(80)
#define EXP_1_64	(EXP_1_32 + TV_UNIT) //96
#define EXP_1_128	(EXP_1_64 + TV_UNIT) //112
#define EXP_1_256	(EXP_1_128 + TV_UNIT) //128
#define EXP_1_512	(EXP_1_256 + TV_UNIT) //144
#define EXP_1_1024	(EXP_1_512 + TV_UNIT) //160
#define EXP_1_2048	(EXP_1_1024 + TV_UNIT) //176
#define EXP_1_4096	(EXP_1_2048 + TV_UNIT) // 192
#define EXP_1_8192	(EXP_1_4096 + TV_UNIT) // 208
#define EXP_1_16384	(EXP_1_8192 + TV_UNIT) // 224

typedef enum _APPAE_SUBACTION_MODE{
	AE_SUBACTION_AUTO,
	AE_SUBACTION_AUTO_LED,
	AE_SUBACTION_LED_MANUAL_START,
	AE_SUBACTION_LED_OFF=AE_SUBACTION_LED_MANUAL_START,
	AE_SUBACTION_LED_1,
	AE_SUBACTION_LED_2,
	AE_SUBACTION_LED_3,
	AE_SUBACTION_LED_NUM=AE_SUBACTION_LED_3,
} APPAE_SUBACTION_MODE;
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appAeStatus_s {
	SINT16 iris;
	SINT16 shutter;
	SINT16 iso;
	SINT16 ev;
	SINT16 av;
	SINT16 tv;
} appAeStatus_t;

typedef struct appAePara_s {
	SINT16 ui_iris;
	UINT8  cap_flash;
} appAePara_t;

typedef struct appAeSet_s {
   UINT8 pasm;
   UINT8 scene;
   UINT8 meter;
   UINT8 evbias;
   UINT8 iris;
   UINT8 shutter;
   UINT8 iso;
   UINT8 aeb;
   UINT8 stabilizer;
   UINT8 dist;
} appAeSet_t;

typedef struct appAeCb_s {
	SINT16 pviso100agc;
	SINT16 capiso100agc;
	UINT16 luma;
	SINT16 lvidx;
	SINT16 expidx;
	SINT16 agcidx;
	SINT16 avidx;
	SINT32 capexp;
	SINT32 capagc;
	SINT16 capav;
	SINT32 capaebexp[3];
	SINT32 capaebagc[3];
	UINT8 zml;
	UINT8 iris_stop;
	appAeSet_t aeset;
} appAeCb_t;


typedef struct appAwbStatus_s {
	UINT32 wbMode;
	UINT32 rGain;
	UINT32 bGain;
} appAwbStatus_t;

extern sp5kEvUnit_t evUnit;
extern sp5kExpLimit_t expLimitPv,expLimitCap;
extern sp5kAgcLimit_t agcLimitPv,agcLimitCap;


appAeCb_t *appAeCbGet(void);
appAePara_t *appAeParaGet(void);
appAeSet_t * appAeSetGet(void);

void appAeInit();
void appAeEnable(BOOL fEnable);
void appAePreviewRestore();
void appAePreviewSet(void);
UINT8 appAeStableGet(void);
void appAeStableSet(void);
void appAeStableReset(void);
void appAeStabilizerSet(UINT8 stabilizer);
SINT32 appPreviewAeBvIdxGet(void);
UINT16	appAeLumaGet(void);
SINT32 appAeMinCapGainGet(void);
void appAeCaptureFlashAeOverrideSet(void);
void appAeGetCurrentExpIsoUiIdx(UINT16* ui_exp_idx, UINT16* ui_iso_idx);
UINT8 appAeFastAdjust(UINT32 param);
void appAeIrisGet(UINT32 *numerator, UINT32 *denominator);
void appAeCapParaGet(SINT16 *piris, SINT16 *pshutter, SINT16 *piso);
UINT32 appAeIrisSet(UINT8 irisStop);
void appAeZoomLevelSet(UINT32 zml);
void appAeCapShutterSet(UINT8 shutidx);
void appAeCapIsoSet(UINT8 isoidx);
void appAeCapIrisSet(UINT8 irisidx);
void appAeMeterSet(UINT8 bMeter);
void appAeEvBiasSet(UINT8 bEvBias);
void appAeTargetFactorSet(UINT8 FactorIdx);
void appAePreviewHighISOBoundarySet(UINT32 SV);
void appAeCaptureHighISOBoundarySet(UINT32 SV);
void appAeAebSet(UINT8 aeb);
void appAeExpFlickerModeSet(UINT8 flickeridx);
void appAeFrameRateSet(UINT32 fps);
void appAeMDEnable(UINT8 bEnable);
void appAeMDSet(UINT8 bDetect);
void appAePvReset(void);
void appAeCaptureSet(UINT8 imgId);
void cmdAE(UINT32 argc, UINT8 *arg[], UINT32 v[]);
void appAeHDRALGEnable(UINT8 bEnable);
void appAeSubActionMode(UINT8 mode);
void appAeHighPriority(UINT8 bEnable);
void appAeReady(void);






void appAwbInit(void);


#endif  /* APP_AAA_H */
