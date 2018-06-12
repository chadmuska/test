#ifndef APP_ICATCH_EIS_LIB_H
#define APP_ICATCH_EIS_LIB_H

#include "api/sp5k_media_api.h"

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define APP_GYRO_X_OFS		0
#define APP_GYRO_Y_OFS		1

#define APP_GYRO_DLPF_250HZ	0	// delay 0.97ms
#define APP_GYRO_DLPF_184HZ	1	// delay 2.9ms
#define APP_GYRO_DLPF_92HZ	2	// delay 3.9ms
#define APP_GYRO_DLPF_41HZ	4	// delay 5.9ms
#define APP_GYRO_DLPF_20HZ	4	// delay 9.9ms
#define APP_GYRO_DLPF_10HZ	5	// delay 17.85ms
#define APP_GYRO_DLPF_5HZ	6	// delay 33.48ms

#define APP_GYRO_SCALE_31_DPS	0 // Degree per second
#define APP_GYRO_SCALE_63_DPS	1
#define APP_GYRO_SCALE_125_DPS	2
#define APP_GYRO_SCALE_250_DPS	3

#define GYRO_T_GAINX10		3214 // IDG2020 product spec : 1 degree(Celsius) = 321.4 LSB
#define GYRO_TILE_UNIT		16384

#define GYRO_ERR_VAL		99999 // 99999 is out of range for SINT16 gyro value

#define LOG_GMV	1

#define EIS_LOG	0
#define EIS_OSD	0

enum {
	EIS_STATE_IDLE,
	EIS_STATE_TRACK,
	EIS_STATE_STABLIZE,
	EIS_STATE_MAX
};

enum {
	EIS_VID_MODE_1080P30,
	EIS_VID_MODE_720P60,
	EIS_VID_MODE_720P30,
	EIS_VID_MODE_MAX,
};

enum {
	EIS_CFG_NONE,
	EIS_CFG_DIST, // set only
	EIS_CFG_DIS_COMP_RANGE, // get only
	EIS_CFG_CROP_POS, // EIS_CROP_POS_XXXX
	EIS_CFG_RSC_RANGE_Y,
	EIS_CFG_MAX,
};

enum {
	EIS_CROP_POS_CDSP,
	EIS_CROP_POS_DISP,
	EIS_CROP_POS_MAX,
};
#define APP_GYRO_DLPF_FREQ		APP_GYRO_DLPF_250HZ
#define APP_GYRO_SCALE			APP_GYRO_SCALE_250_DPS

#define APP_GYRO_CLK_NORMAL		SP5K_SPI_PORT2_CLK_400KHZ
#define APP_GYRO_CLK_FAST		SP5K_SPI_PORT2_CLK_12MHZ

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct {
	SINT16 t,x,y;
	SINT32 fx,fy;
	//SINT16 frx,fry;
} gyro_t;

typedef struct {
	SINT32 t,x,y,ox,oy;
	UINT32 gyroIdx;
	#if LOG_GMV
	SINT16 gmvx,gmvy;
	#endif
	SINT32 xc,yc;
	SINT32 gyroCnt;
	UINT32 expMs;
} eis_alg_t;

typedef struct {
	UINT32 cropPos;
	SINT32 fps;
	SINT32 gyroRatio;
	SINT32 rscRatioX,rscRatioY;
	SINT32 eisCntMax; // eisTimerCounterThreshold <= eisCntMax so EIS always sets before EISTrigCB (and CDSP SOF for CDSP crop)
	SINT32 eisCntOfs; // eisTimerCounterThreshold offset by experiment, should be 0 as possible
	SINT32 rscCntMax; // eisTimerCounterRscThreshold <= rscCntMax so RSC always sets before GMV CB
	SINT32 rscCntNum; // RSC sample numbers to represent RSC motion, (EIS_FREQ/30) * (1080/1296) * ((1 - .disCompRange)%
	SINT32 rscCntOfs; // eisTimerCounterRscThreshold offset by experiment, should be frame size as possible
	UINT32 disCompRange;
	UINT32 calibStableCntMax;
	SINT32 dispCropRangeY;
	UINT32 (*eisThrCalc)(UINT32 expMs);
	void (*eisTimerProc)(void);
	void (*eisSet)(SINT32 xofs, SINT32 yofs);
	void (*gmvUpdate)(SINT32 *pmvx, SINT32 *pmvy, sp5kDistGmvCbEx_t *dis);
	void (*osdDraw)(UINT8 *pbuf, UINT32 w, UINT32 h);
} eis_param_t;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
extern UINT32 gyroDataIdx,eisDataIdx;
extern gyro_t *gyroData;
extern eis_alg_t *eisData;

extern UINT32 eisCreated;

extern SINT32 gyroXC, gyroYC;
extern SINT32 gyroCos, gyroSin;
extern SINT32 gyroT0, gyroT1;
extern SINT32 gyroX_T0, gyroX_T1;
extern SINT32 gyroY_T0, gyroY_T1;

extern SINT32 gyroTmin,gyroTmax;
extern SINT32 gyroXmin,gyroXmax;
extern SINT32 gyroYmin,gyroYmax;

extern eis_param_t *eisParam;

// basic gyro functions
void gyroInit(UINT32 freq, UINT32 dlpf);
void gyroFini(void);
UINT8 gyroRead8(UINT32 addr);
void gyroWrite(UINT32 addr, UINT32 data);
void gyroReadXY(UINT8* w);
void gyroReadTXY(UINT8* w);

// extended gyro functions
UINT32 gyroCalibCenter(UINT32 fMsg);
UINT32 gyroCalibCenterByT(UINT32 sec, UINT32 fMsg);
//UINT32 gyroCalibTilt(UINT32 fMsg);
void gyroCalibLoad(void);
void gyroCalibSave(void);
UINT32 gyroDetect(void);
void gyroAutoCalibCenterFormulaSet(SINT32 t, SINT32 x, SINT32 y);
void gyroAutoCalibCenterFromT(SINT16 T);

// mv test with urgent callback
#if EIS_OSD
void pvUrgentUpdateEisIdx(UINT32 idx);
void pvUrgentUpdateMv(SINT32 mx, SINT32 my);
void pvUrgentUpdateRsc(SINT32 rscx, SINT32 rscy);
void pvUrgentUpdateCrop(UINT32 cropX, UINT32 cropY, UINT32 cropW, UINT32 cropH, UINT32 w, UINT32 h);
void pvUrgentUpdateEisOfs(SINT32 sx, SINT32 sy);
void pvUrgentUpdateGyro(SINT32 ox, SINT32 oy);
void pvUrgentUpdateLastGmv(SINT32 gmvx, SINT32 gmvy);
#else
#define pvUrgentUpdateEisIdx(idx)
#define pvUrgentUpdateMv(mx, my)
#define pvUrgentUpdateRsc(x, y)
#define pvUrgentUpdateCrop(cx, cy, cw, ch, w, h)
#define pvUrgentUpdateEisOfs(x, y)
#define pvUrgentUpdateGyro(x, y)
#define pvUrgentUpdateLastGmv(x, y)
//#define pvUrgentCbInit(mode)
//#define pvUrgentCbUnReg(mode)
#endif

void pvUrgentCbInit(UINT32 mode);
void pvUrgentCbUnReg(UINT32 mode);

/*log*/
#if EIS_LOG
void eisLogEnable(BOOL en);
void eisLogSaveToCard(void);
#else
#define eisLogEnable(param)
#define eisLogSaveToCard()
#endif

// eis functions
void eisCreate(UINT32 eisInitState);
void eisDestroy(void);
void eisReset(void);
UINT32 eisVidModeSet(UINT32 vidMode); // EIS_VID_MODE_*
UINT32 eisStateSet(UINT32 newState);
void EIStrigCB(void);
UINT32 eisCfgSet(UINT32 cfgId, UINT32 val);
UINT32 eisCfgGet(UINT32 cfgId);

UINT32 eisGmvUpdCb(UINT32 unit, SINT32 *pmvx, SINT32 *pmvy, sp5kDistGmvCbEx_t *dis);

#endif  /* APP_ICATCH_EIS_LIB_H */

