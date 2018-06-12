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
#ifndef _APP_FLASH_API_H_
#define _APP_FLASH_API_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define DEBUG_PREFH_USE_DATE_STAMP	0	/* shall be off @ release */

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct {
	/*-- preview paramters backup */
	SINT32	pvExp;
	UINT32	pvAgc;

	/*-- preflah strobe parameters */
	SINT32	prefhExp;
	UINT32	prefhAgc;
	UINT32	hPos;
	UINT32	pulseWidth;
	UINT32	refDiffLuma;	/* the default luma while expidx/agcidx/hpos/width were applied */

	/*-- preflash result */
	UINT32	sceneLuma;		/*-- the scene luma prior preflash w/t predefined exp/agc */
	UINT32	prefhLuma;		/*-- the resulted scene luma with preflash contributed */
	UINT32	prefhDiffLuma;	/*-- the delta luma contributed by preflash */
	float	flRebound;		/*-- refDiffLuma/prefhDiffLuma@refISO */
	UINT32	fillFlashPercent; /*-- show how many flash/ambient % */

	/*-- snapflash parameters */
	UINT32	snapTarget;		/*-- the target level of capture image */
	UINT32	snapHPos;
	UINT32	snapWidth;
	SINT32	capfhExp;
	UINT32	capfhAgc;

	UINT32	lookupDiffLuma;

	UINT8	snapIsoIdx;		/* selected SNAP ISO */
	UINT8	snapMaxIsoIdx;
	UINT8	isAutoIso;
	UINT8	zm_seg;

	UINT32	trueRefDiffLuma;

	UINT8	isDummyFlash;
	UINT8	curvTblNum;
	UINT8	whichCurvTbl;
	SINT8	whichEntry;

	UINT32	snapFlashMode;

	UINT32	hPosLimit;
	#if DEBUG_PREFH_USE_DATE_STAMP
	UINT8	isStampDbg;
	UINT8	dbgRsvdU8;
	#endif

	/*-- flash ae */
	SINT32	ae_exp;
	UINT32	ae_agc;
	UINT32	ShutterLimmit; /*current setting allowed shortest exp*/
} appPrefhCb_t;

typedef struct {
	//SINT32 pfhCfgExpIdx;
	//UINT32 pfhCfgAgcIdx;
	SINT32 viewExpIdx;
	UINT32 viewAgcIdx;
	SINT32 snapExpIdx;
	UINT32 snapAgcIdx;
	UINT32 hPos;
	UINT32 pulseW;
	UINT32 lookupWidth;
	UINT16 pfhCfgLuma;
	UINT16 pfhLuma;
	UINT16 viewLuma;
	UINT16 pfhDiffLuma;
	UINT16 snapLuma;
	SINT32 viewSv;
	SINT32 capSv;
} pfhDbgInfoSt_t;
/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

#if KIT_FLASH_ENABLE
appPrefhCb_t* appPrefhCbGet(void);
void *appPreflashDebugInfoGet(void);
UINT32 appPreflashLuma2WidthLookup(UINT32 inLuma);
UINT32 appPreflashWidth2LumaLookup(UINT32 inWidth);
UINT32 appPreflashCfgHPosGet(void);
UINT32 appPreflashCfgWidthGet(void);
void appPreflashCfgExpAgcGet(SINT32 *pfExp, UINT32 *pfAgc);
void appPreflashCfgExpAgcSet(UINT32 pfExp, UINT32 pfAgc);
UINT32 appPreflashCfgLumaGet(UINT32 diffLuma);
void appSnapflashTargetSet(UINT32 luma);
UINT32 appSnapflashTargetGet(void);
UINT32 appSnapflashCfgHPosGet(void);
void appPreflashInit(void);
void appPreflashConfig(UINT32 sp5kFlashMode);
void appPreflashTrig(void);
UINT32 appSnapFlashCalculate(UINT16 snapLuma);
void appSnapFlashInstall(UINT32 mode, UINT32 pulseW, UINT32 hPos);
void appSnapFlashDismiss(void);
UINT32 appPreflashStrobeCharge(BOOL forceChg);
void appPreflashResultParamGet(UINT16 *envLuma, UINT16 *pfhLuma);
void appRedEyeFlashInit(void);
void appSnapFlashUiModeSet(UINT16 ui_mode);
#else
#define appPreflashInit()
#define appPreflashConfig(a)
#define appPreflashTrig()
#define appSnapflashTargetGet() 0
#define appSnapFlashCalculate(a) 0
#define appSnapflashCfgHPosGet() 0
#define appSnapFlashInstall(a,b,c)
#define appSnapFlashDismiss()
#define appSnapFlashUiModeSet(a)


#endif
#endif  /* _APP_FLASH_API_H_ */


