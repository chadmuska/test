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
 *	File name    : App_custom_cmd.c
 *	Function     : Provide test command and sample code.
                   Also, you can test your design in this page.
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.04  Edward.Lu  "Add description and modify code"
 **************************************************************************/
#define HOST_DBG 0
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"
#include "api/cmd_api.h"
#include "api/sp5k_cdsp_api.h"
#include "api/sp5k_disp_api.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_fs_api.h"
#include "api/sp5k_os_api.h"
#include "api/sp5k_cal_api.h"
#include "api/sp5k_modesw_api.h"
#include "api/sp5k_ae_api.h"
#include "api/sp5k_awb_api.h"
#include "api/sp5k_gfx_api.h"
#include "api/sp5k_dcf_api.h"
#include "api/sp5k_media_api.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_aud_api.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_sensor_api.h"
#include "api/sp5k_usb_api.h"
#include "api/sp5k_dbg_api.h"
#include "api/sp5k_ftrack_api.h"
#include "api/sp5k_eis_rsc_algo_api.h"

#if SPCA5310
#include "api/sp5k_pip_api.h"
#endif
#include "middleware/proflog.h"

#include "api/sp5k_rsvblk_api.h"
#include "customization/dev_init.h"
#include "customization/app_init.h"
#include "app_tgcmd.h"
#include "../../hostfw/include/app_view_param.h"
#include "../../hostfw/include/gpio_custom.h"
#include "../../hostfw/include/app_key_def.h"
#include "../../hostfw/include/app_res_def.h"
#include "../../hostfw/include/app_playback_def.h"
#include "../../hostfw/include/app_audio.h"
#include "../../hostfw/include/app_spca7002_utility.h"
#include "../../hostfw/include/app_video.h"
#include "../../hostfw/include/app_aaa.h"
#include "../../hostfw/include/app_script.h"
#include "../../hostfw/include/app_still.h"
#include "../../hostfw/include/app_state_ctrl.h"
#include "../../hostfw/include/app_sound.h"
#include "app_util.h"
#include "app_face_detect_ex.h"
#include "app_sio_filter.h"
#include "app_zoom_api.h"
#include "app_com_api.h"

#if SP5K_SENSOR_DUAL_MODE
#include "../../hostfw/include/app_i2c_util.h"
#endif
#include "../../hostfw/include/app_icon_def.h"
#include "../../hostfw/include/app_osd_api.h"
#include "../../hostfw/include/app_sensor.h"

#include "../../hostfw/include/app_watch_dog.h"
#include "jet_utility.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum {
	DRAM_TEST_PATTERN_BIT_CHANGE,
	DRAM_TEST_PATTERN_CUSTOM1,
	DRAM_TEST_PATTERN_MAX
};

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define CMD_ARGC_MAX            36

#define READ8(a)                (*((volatile UINT8*)(a)))
#define WRITE8(a,b)             *(volatile UINT8*)(a) = (b)
#define READ16(a)               (*((volatile UINT16*)(a)))
#define WRITE16(a,b)            *(volatile UINT16*)(a) = (b)
#define READ32(a)               (*((volatile UINT32*)(a)))
#define WRITE32(a,b)            *(volatile UINT32*)(a) = (b)
#define COUNT_OF(array)	   (sizeof(array)/sizeof(array[0]))
#define GPIO_MASK(PinNo)    	(0x00000001L << (PinNo))
#define SIZE					1024*1024

#define CP_BUF_SIZE             256*1024

#define AV_UNIT                 1
#define EV_UNIT                 16
#define SV_UNIT                 EV_UNIT
#define TV_UNIT                 EV_UNIT
#define SV_UNIT                 EV_UNIT

#define DUAL_SEN_RAW            1
#define DUAL_SEN_RAW_YUV        3
#define DUAL_SEN_PRI            4
#define DUAL_SEN_SEC            8
#define DUAL_PIP_RATION         30
#define ROUND_DOWN_TO_DIVISIBLE(num,div) ( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div) ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef UINT32 (*bkfunc_t)(UINT32);

typedef struct CustomerCmd_s {
	char *cmd;
	void (*pfunc)(UINT32 argc, UINT8 *argv[], UINT32 v[]);
} CustomerCmd_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 v[CMD_ARGC_MAX];

UINT8 tgCmdMode=SP5K_MODE_STILL_PREVIEW;
UINT32 xdata[10]={0,0,0,0,0,0,0,0,0,0};

static UINT32 urgentCbCnt = 0;
static UINT32 waitCnt = 0;
static UINT32 pvurgentCbInited;
static frameBufInfo_t pvmain[2];
static frameBufInfo_t pv2nd[2];
static UINT32 dualsenmode;
static UINT32 dualsenmodePlace;

static UINT8 *pdstBuf = NULL;
static UINT8 *pdstBuf1 = NULL;
static UINT32 usbDispYuvBuf[6] = {0};
static UINT32 usbDispWidth = 0;
static UINT32 usbDispHeight = 0;
static UINT32 Mthfile, Mthfile1;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern int getch(void);
extern void cmdScript( UINT32 argc, UINT8 *arg[], UINT32 v[] );
extern void cmdMSH(UINT32 argc, 	UINT8 *arg[], UINT32 v[]);
extern void cmdAE(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdAWB(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdCDFS(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdSnap(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdVideo(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdFlash(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdCalib(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdScript(UINT32 argc, UINT8 *arg[], UINT32 v[]);
#ifdef JET_FEATURE
extern void cmdI2ctool(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdGsensor(UINT32 argc, UINT8 *arg[], UINT32 v[]);
extern void cmdJetCmd(UINT32 argc, UINT8 *arg[], UINT32 v[]);
#endif
extern void appPanelInit(void);
extern void appBtnInit(void);
extern void aaaLabDo(UINT32 fun, UINT32 subfunc, UINT32 sensormode_pv, UINT32 sensormode_snap);
#if SP5K_SENSOR_DUAL_MODE
    #if SP5K_SENSOR_AR0330_SERDES_SPCA7002
extern void SERDES_7002_WrSPIFlash(UINT32 size);
extern void serdes_stream_off_set(UINT8 a_ucSerdesSel);
extern void serdes_spca7002_i2c_data_write(UINT32 a_udwAddr, UINT32 a_udwData);
extern UINT32 serdes_spca7002_i2c_data_read(UINT32 a_udwAddr);
    #endif
#endif

extern void appStateCtrlInfoPrint(char *title);

#if defined(PRJ_JETDVR)  || defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern void appInfoFileReName(UINT8 * srcPath  , UINT8 * dstPath , UINT8 * srcFileName , UINT8 *dstFileName);
extern void appInfoSetParkingMode(void);
extern UINT32 appInfoSecTimeBaseGet(tmx_t * time);
extern void appInfoSecTimeToRealGet(UINT32 totalsec ,tmx_t * time);
extern  BOOL appInfoFWUPDateVerGet(UINT8 * filename);
extern void halPwrcEnable(UINT32 en);
/* paul 2018.04.16 Add for modify compiler warning*/
extern void appInfoSWWDTWakeup(UINT32);
extern void PlayPosCountStop(void);
extern void RegularCountStop(void);
extern void ConnectDevCountStop(void);

extern UINT32 PlayPos_TimerCount;
extern UINT32 PosReq_TimerCount ;
extern UINT32 ConnectDev_TimerCount ;
extern UINT32 Regular_TimerCount ;
extern BOOL NewAVNModel;
#include "app_infohandlerComm.h"
#include "sp5k_dbg_api.h"
#include "app_buzzer.h"
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/* TEST_ADAS */
#if TEST_ADAS
#include "../../hostfw/service/adas/app_test_adas_api.h"

#if TEST_ADAS_FCWS
#include "../../hostfw/service/adas/app_test_adas_fcws.h"
#endif

#if TEST_ADAS_ROD
#include "../adas/app_test_adas_rod.h"
#endif
/*for iCatch test only -Start-*/
enum {
    LDWS_SENSOR_UNSUPPORT = 0,
    LDWS_SENSOR_OV4689,
    LDWS_SENSOR_AR0330CS,
    LDWS_SENSOR_TOTAL
};

enum {
    FCWS_SENSOR_UNSUPPORT = 0,
    FCWS_SENSOR_OV4689,
    FCWS_SENSOR_AR0330CS,
    FCWS_SENSOR_AR0330,
    FCWS_SENSOR_AR0330_SERDES_SPCA7002,
    FCWS_SENSOR_OV4689_SERDES_SPCA7002,
    FCWS_SENSOR_TOTAL
};

/*typedef struct _LdwsSupportSensor_t{
    UINT32 nSensorType;
    UINT32 nDistanceToCenterThr;
} LdwsSupportSensor_t;*/ /*cppcheck Removed never used*/

/*typedef struct _FcwsSupportSensor_t{
    int nSensorType;
    UINT32 nSensorFocalLen;
    UINT32 nSensorPixelUnit;
} FcwsSupportSensor_t;*/ /*cppcheck Removed never used*/

//iCath LDWS sensor lens parameter
static int g_nLdwsSensorType = LDWS_SENSOR_UNSUPPORT;
static LdwsSupportSensor_t g_LdwsSupportSensor[LDWS_SENSOR_TOTAL - 1] = {
                                                                             {LDWS_SENSOR_OV4689, 29},
                                                                             {LDWS_SENSOR_AR0330CS, 40}
                                                                        };

//iCatch FCWS sensor lens parameter
static int g_nFcwsSensorType = FCWS_SENSOR_UNSUPPORT;
static FcwsSupportSensor_t g_FcwsSupportSensor[FCWS_SENSOR_TOTAL - 1] = {
                                                 {FCWS_SENSOR_OV4689, (2.5*10000), 158},
                                                 {FCWS_SENSOR_AR0330CS, (3.49*10000), 158},
                                                 {FCWS_SENSOR_AR0330, (3.6*10000), 158},
                                                 {FCWS_SENSOR_AR0330_SERDES_SPCA7002, (3.6*10000), 158},
                                                 {FCWS_SENSOR_OV4689_SERDES_SPCA7002, (2.5*10000), 158},
                                                };
static int g_nFcwsPeriod = 2;

static void decideLdwsSensorType(int *pInOutSensorType);
static void decideFcwsSensorType(int *pInOutSensorType);

static void decideLdwsSensorType(int *pInOutSensorType){
#if SP5K_SENSOR_OV4689
    *pInOutSensorType = LDWS_SENSOR_OV4689;
#elif SP5K_SENSOR_AR0330CS
    *pInOutSensorType = LDWS_SENSOR_AR0330CS;
#else
    HOST_ASSERT_MSG(0,"%s current sensor is not supported\n" , LDWS_STR);
#endif
}

static void decideFcwsSensorType(int *pInOutSensorType){
#if SP5K_SENSOR_OV4689
    *pInOutSensorType = FCWS_SENSOR_OV4689;
#elif SP5K_SENSOR_AR0330CS
    *pInOutSensorType = FCWS_SENSOR_AR0330CS;
#elif SP5K_SENSOR_AR0330
    *pInOutSensorType = FCWS_SENSOR_AR0330;
#elif SP5K_SENSOR_AR0330_SERDES_SPCA7002
    *pInOutSensorType = FCWS_SENSOR_AR0330_SERDES_SPCA7002;
#elif SP5K_SENSOR_OV4689_SERDES_SPCA7002
    *pInOutSensorType = FCWS_SENSOR_OV4689_SERDES_SPCA7002;
#else
    HOST_ASSERT_MSG(0,"%s current sensor is not supported\n" , FCWS_STR);
#endif
}

extern void appLdwsStart(const int nCenterX, const int nCenterY, ...);
extern void appLdwsStop();
extern void appLpdStart();
extern void appLpdStop();
extern void appLdwsDumpSetting();
extern void appFcwsDumpSetting();
#endif

static CustomerCmd_t CustomerCmd[] =
{
    { "ae"      , cmdAE},
    { "awb"     , cmdAWB},
    { "cdfs"    , cmdCDFS},
    { "snap"    , cmdSnap},
    { "video"   , cmdVideo},
#if KIT_FLASH_ENABLE
    { "flash"   , cmdFlash},
#endif
#if KIT_CALIB_ENABLE
    { "calib"   , cmdCalib},
#endif
    { "script"  , cmdScript},
#ifdef JET_FEATURE
    { "i2ctool" , cmdI2ctool},
    { "gsensor" , cmdGsensor},
    { "jetcmd"  , cmdJetCmd},
#endif
    { NULL, NULL },
};

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCharge_SetStopCBFRegister
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.04  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	
static void hostCmdHandle(void)
{
	/* Key Definition */
	#define KEY_DOWN_PRESS          0x00
	#define KEY_DEL_PRESS           0x06
	#define KEY_LEFT_PRESS          0x02
	#define KEY_RIGHT_PRESS         0x03
	#define KEY_SET_PRESS           0x04
	#define KEY_TELE_PRESS          0x01
	#define KEY_WIDE_PRESS          0x00
	#define KEY_UP_PRESS            0x01
	#define KEY_MENU_PRESS          0x01
	#define KEY_PLAYBACK_PRESS      0x05
	#define KEY_SNAP2_RELEASE       0x11
	#define KEY_SNAP1_RELEASE       0x12
	#define MSG_GPIO_BUTTON            0x0100
	#define MSG_ADC_BUTTON            0x0103

	UINT8 ch = 0;
	UINT32 key = 0xffffffff;

	printf("\n");
	printf("Enter Host Key Control !!\n");
	printf("\n");
	printf(" -------------------------------------\n");
	printf("| 7 = (MENU) 8 = ( UP   ) 9 =( MODE ) |\n");
	printf("| 4 = (LEFT) 5 = ( SET  ) 6 =( RIGHT) |\n");
	printf("| 1 = (WIDE) 2 = ( DOWN ) 3 =( TEL  ) |\n");
	printf("| 0 = (MENU)(DEL )  A =S1 RELEASE     |\n");
	printf("| B = S2 release C=S1 press D:S2 PRESS|\n");
	printf("| F = PB|\n");
	printf(" -------------------------------------\n");
	printf("\n");
	printf("Quit by press ESC ..\n");
	printf("\n");
	while (ch != 0x1B) {/* if ESC, quit */
		ch = (UINT8 )getch();
		printf("ch=0x%x\n",ch);
		switch ( ch ) {
		case 0x30:/* ASM/Delete key */
			key = APP_KEY_PRESS_DEL;
			break;
		case 0x31:/* WIDE key */
			key = APP_KEY_PRESS_WIDE;
			break;
		case 0x32:/* Down key */
			key = APP_KEY_PRESS_DOWN;
			break;
		case 0x33:/* TEL key */
			key = APP_KEY_PRESS_TELE;
			break;
		case 0x34:/* LEFT Key */
			key = APP_KEY_PRESS_LEFT;
			break;
		case 0x35:/* SET key */
			key = APP_KEY_PRESS_SET;
			break;
		case 0x36:/* RIGHT key */
			key = APP_KEY_PRESS_RIGHT;
			break;
		case 0x37:/* MENU key */
			key = APP_KEY_PRESS_MENU;
			break;
		case 0x38:/* UP key */
			key = APP_KEY_PRESS_UP;
			break;
		case 0x39:/* MODE key */
			key = APP_KEY_PRESS_MODE;
			break;
		case 0x61:/* MODE key A */
			key = APP_KEY_RELEASE_S2;
			break;
		case 0x62:/* MODE key B*/
			key = APP_KEY_RELEASE_S1;
			break;
		case 0x63:/* MODE key C*/
			key = APP_KEY_PRESS_S1;
			break;
		case 0x64:/* MODE key D*/
			key = APP_KEY_PRESS_S2;
			break;
		case 0x65:/* MODE key E*/
			key = 0xffffffff;
			break;
		case 0x66:
			key = APP_KEY_PRESS_PB;

			break;
		default:
			key = 0xffffffff;
			break;
		}
		if ( key != 0xffffffff ) {
			printf("key=%x\n",key);
			sp5kHostMsgSend(key,0);
		}
		printf("\n");
		printf(" -------------------------------------\n");
		printf("| 7 = (MENU) 8 = ( UP   ) 9 =( MODE ) |\n");
		printf("| 4 = (LEFT) 5 = ( SET  ) 6 =( RIGHT) |\n");
		printf("| 1 = (WIDE) 2 = ( DOWN ) 3 =( TEL  ) |\n");
		printf("| 0 = (MENU)(DEL )  A =S1 RELEASE     |\n");
		printf("| B = S2 release C=S1 press D:S2 PRESS|\n");
		printf("| F = PB|\n");
		printf(" -------------------------------------\n");
		printf("\n");
	}

	printf("Quit Host Key Control !!\n");
	printf("\n");
}

/*-------------------------------------------------------------------------
 *  Function Name : appFileCopy
 *  Description :
 *------------------------------------------------------------------------*/
void appFileCopy(UINT8 *src, UINT8 *dst)
{
	UINT32 fdSrc,fdDst;
	UINT8 *buf;
	UINT32 l;
	printf("cp [%s] [%s]\n", src, dst);
	fdSrc = sp5kFsFileOpen(src, SP5K_FS_OPEN_RDONLY);
	if (fdSrc==0) {
		printf("Cannot open src:%s\n", src);
		return;
	}
	fdDst = sp5kFsFileOpen(dst, SP5K_FS_OPEN_CREATE);
	if (fdDst==0) {
		printf("Cannot open dst:%s\n", dst);
		sp5kFsFileClose(fdSrc);
		return;
	}
	l = sp5kFsFileSizeGet(fdSrc);
	buf = sp5kMalloc(CP_BUF_SIZE);
	if (buf==NULL) {
		printf("malloc err\n");
		goto cp_closefile;
	}
	while (l) {
		UINT32 size=l>=CP_BUF_SIZE ? CP_BUF_SIZE : l;
		sp5kFsFileRead(fdSrc, buf, size);
		sp5kFsFileWrite(fdDst, buf, size);
		l -= size;
		printf(".");
	}
	printf("\n");
	sp5kFree(buf);
cp_closefile:
	sp5kFsFileClose(fdDst);
	sp5kFsFileClose(fdSrc);
}

/*-------------------------------------------------------------------------
 *  Function Name : pvUrgentCb_RawYuv_Pip
 *  Description :
 *------------------------------------------------------------------------*/
static UINT32 pvUrgentCb_RawYuv_Pip(
	UINT32 streamId,        /* nth stream, from 0 */
	UINT32 fid,             /* frame id */
	frameBufInfo_t *pframe, /* buffer information */
	UINT8 *pbufdup          /* duplicated buffer address */
)
{
	UINT32 srcRoiw, srcRoih, srcRoix, srcRoiy;
	UINT32 dstRoiw, dstRoih, dstRoix, dstRoiy;
	static sp5kGfxObj_t imgSrc, imgDst;
	static sp5kGfxMapProjectParam_t  proPrm;
	profLogPrintf(2, "urcb 0");

	/*   PIP Sec to Pri,  Src B,  Dst A*/
	if( pvurgentCbInited == 1){
		sp5kUrgentCallbackYuvGet( &pvmain[0], 0, 0);
		sp5kUrgentCallbackYuvGet( &pv2nd[1], 1, 1);
	}
	else if( pvurgentCbInited == 2) {
		sp5kDispUrgentCallbackYuvGet( &pvmain[0], 0, 0);
		sp5kDispUrgentCallbackYuvGet( &pv2nd[1], 1, 1);
	}
#if 1
	if(urgentCbCnt > waitCnt){
		sp5kGfxObjectMappingAsyncWait(-1);
		urgentCbCnt--;
	}
#else
	proPrm.mapAttr.waitMode= SP5K_GFX_MAP_MODE_DIRECT_WAIT;
#endif

	if(streamId == 1){
		/* dst is in 2nd buf,  sensor[1]  */
		imgDst.pbuf = pframe->pbuf;
		imgDst.bufW = pframe->width;
		imgDst.bufH = pframe->height;

		dstRoiw = pv2nd[1].roiW;
		dstRoih = pv2nd[1].roiH;
		dstRoix = pv2nd[1].roiX;
		dstRoiy = pv2nd[1].roiY;

		/* src is in main buf, sensor[0] */
		imgSrc.pbuf = pvmain[0].pbuf;
		imgSrc.bufW = pvmain[0].width;
		imgSrc.bufH = pvmain[0].height;
		imgSrc.fmt	= SP5K_GFX_FMT_YUV422;

		srcRoix = imgSrc.roiX = pvmain[0].roiX;
		srcRoiy = imgSrc.roiY = pvmain[0].roiY;
		srcRoiw = imgSrc.roiW = pvmain[0].roiW;
		srcRoih = imgSrc.roiH = pvmain[0].roiH;
	}
	else {
		/* dst is in main buf,  sensor[0] */
		imgDst.pbuf = pframe->pbuf;
		imgDst.bufW = pframe->width;
		imgDst.bufH = pframe->height;

		dstRoiw = pvmain[0].roiW;
		dstRoih = pvmain[0].roiH;
		dstRoix = pvmain[0].roiX;
		dstRoiy = pvmain[0].roiY;

		/* src is in 2nd buf, sensor[1] */
		imgSrc.pbuf = pv2nd[1].pbuf;
		imgSrc.bufW = pv2nd[1].width;
		imgSrc.bufH = pv2nd[1].height;
		imgSrc.fmt	= SP5K_GFX_FMT_YUV422;

		srcRoix = imgSrc.roiX = pv2nd[1].roiX;
		srcRoiy = imgSrc.roiY = pv2nd[1].roiY;
		srcRoiw = imgSrc.roiW = pv2nd[1].roiW;
		srcRoih = imgSrc.roiH = pv2nd[1].roiH;
	}

	proPrm.vec.src[0].x = srcRoix;
	proPrm.vec.src[0].y = srcRoiy;
	proPrm.vec.src[1].x = srcRoix+srcRoiw-1;
	proPrm.vec.src[1].y = srcRoiy;
	proPrm.vec.src[2].x = srcRoix+srcRoiw-1;
	proPrm.vec.src[2].y = srcRoiy+srcRoih-1;
	proPrm.vec.src[3].x = srcRoix;
	proPrm.vec.src[3].y = srcRoiy+srcRoih-1;

	/*set sen B as dst */
	imgDst.fmt  = SP5K_GFX_FMT_YUV422;
	dstRoiw = ROUND_DOWN_TO_DIVISIBLE((dstRoiw*DUAL_PIP_RATION/100)&(~0x0001), 16);
	dstRoih = ROUND_DOWN_TO_DIVISIBLE((dstRoih*DUAL_PIP_RATION/100)&(~0x0001), 16);
	dstRoix = 0;
	dstRoiy = 0;

	imgDst.roiX = dstRoix;
	imgDst.roiY = dstRoiy;
	imgDst.roiW = dstRoiw;
	imgDst.roiH = dstRoih;

	proPrm.mapAttr.itpAlg = SP5K_GFX_MAP_ITP_BILINEAR; /*IME_ITP_BILINEAR*/
	proPrm.mapAttr.bgClrY = 0x00;
	proPrm.mapAttr.bgClrU = 0x80;
	proPrm.mapAttr.bgClrV = 0x80;
	proPrm.mapAttr.ctMode = SP5K_GFX_MAP_COLOR_TRANS_BYPASS; /*IME_COLOR_TRANS_BYPASS*/
	proPrm.mapAttr.ctLut  = 0;
	proPrm.mapAttr.pctTblY= NULL;
	proPrm.mapAttr.pctTblU= NULL;
	proPrm.mapAttr.pctTblV= NULL;
	proPrm.mapAttr.waitMode= SP5K_GFX_MAP_MODE_ASYNC_WAIT;

	proPrm.vec.dst[0].x = dstRoix;
	proPrm.vec.dst[0].y = dstRoiy;
	proPrm.vec.dst[1].x = dstRoix+dstRoiw-1;
	proPrm.vec.dst[1].y = dstRoiy;
	proPrm.vec.dst[2].x = dstRoix+dstRoiw-1;
	proPrm.vec.dst[2].y = dstRoiy+dstRoih-1;
	proPrm.vec.dst[3].x = dstRoix;
	proPrm.vec.dst[3].y = dstRoiy+dstRoih-1;

	sp5kGfxObjectMapping(
		&imgSrc,
		&imgDst,
		SP5K_GFX_MAP_MODE_STILL,
		SP5K_GFX_MAPPING_ID_PROJECT,
		(UINT32)&proPrm
	);

	profLogPrintf(2,"urcb src buf=%x",(uint)(imgSrc.pbuf));
	urgentCbCnt += 1;

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : sample_PipStart
 *  Description :
 *------------------------------------------------------------------------*/
UINT32 sample_PipStart(UINT32 dualSenMode)
{
	UINT32 pmode;
	sp5kModeGet(&pmode);
	sp5kPrevUrgentCallback_t cbs[2];
	cbs[0].fp = cbs[1].fp = NULL;

	pvurgentCbInited = 0;
	if(dualSenMode == 4){
		cbs[0].fp = pvUrgentCb_RawYuv_Pip;
		cbs[1].fp = NULL;
	}
	else if(dualSenMode == 8){
		cbs[0].fp = NULL;
		cbs[1].fp = pvUrgentCb_RawYuv_Pip;
	}

	if (pmode==SP5K_MODE_STILL_PREVIEW ){
		pvurgentCbInited = 1;
		sp5kPreviewUrgentCallbackSet(1, 0, cbs, 2);
	}
	else if (pmode==SP5K_MODE_VIDEO_PREVIEW ){
		pvurgentCbInited = 2;
		sp5kDispUrgentCallbackSet(1, 0, cbs, 2);
	}
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : sample_PipStop
 *  Description :
 *------------------------------------------------------------------------*/
UINT32 sample_PipStop(void)
{
	UINT32 pmode;
	sp5kPrevUrgentCallback_t cbs[2];
	cbs[0].fp = NULL;
	cbs[1].fp = NULL;
	sp5kModeGet(&pmode);

	if (pmode==SP5K_MODE_STILL_PREVIEW){
		sp5kPreviewUrgentCallbackSet(1, 0, cbs, 2);
	}
	else if (pmode==SP5K_MODE_VIDEO_PREVIEW){
		sp5kDispUrgentCallbackSet(1, 0, cbs, 2);
	}

	if(urgentCbCnt > waitCnt){
		sp5kGfxObjectMappingAsyncWait(-1);
		urgentCbCnt--;
	}

	pvurgentCbInited = 0;
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : pvUrgentCb_UvcDispYuv
 *  Description :
 *------------------------------------------------------------------------*/
static UINT32 pvUrgentCb_UvcDispYuv(
    UINT32 streamId,        /* nth stream, from 0 */
    UINT32 fid,             /* frame id */
    frameBufInfo_t *pframe, /* buffer information */
    UINT8 *pbufdup          /* duplicated buffer address */
)
{
    sp5kGfxObj_t uvcDispSrc1, uvcDispSrc2, uvcDispSrc3, uvcDispDst;
    static UINT8 outIdx = 0;
    UINT32 ret;

    profLogPrintf(0, "pvUrgentCb_UvcDispYuv s");
    if (pdstBuf1 == NULL) {
        pdstBuf1 = sp5kYuvBufferAlloc(usbDispWidth, usbDispHeight);
    }

    /* blend */
    uvcDispSrc1.pbuf = pdstBuf;
    uvcDispSrc1.bufW = usbDispWidth;
    uvcDispSrc1.bufH = usbDispHeight;
    uvcDispSrc1.roiX = 0;
    uvcDispSrc1.roiY = 0;
    uvcDispSrc1.roiW = usbDispWidth;
    uvcDispSrc1.roiH = usbDispHeight;
    uvcDispSrc1.fmt	= SP5K_GFX_FMT_YUV422;
    uvcDispSrc2.pbuf = pframe->pbuf;
    uvcDispSrc2.bufW = pframe->width;
    uvcDispSrc2.bufH = pframe->height;
    uvcDispSrc2.roiX = 0;
    uvcDispSrc2.roiY = 0;
    uvcDispSrc2.roiW = pframe->width;
    if (pframe->height == 1088) {
        uvcDispSrc2.roiH = 1080;
    }
    else if (pframe->height == 544) {
        uvcDispSrc2.roiH = 540;
    }
    else {
        uvcDispSrc2.roiH = pframe->height;
    }
    uvcDispSrc2.fmt	= SP5K_GFX_FMT_YUV422;
    uvcDispSrc3.pbuf = pdstBuf1;
    uvcDispSrc3.bufW = usbDispWidth;
    uvcDispSrc3.bufH = usbDispHeight;
    uvcDispSrc3.roiX = 0;
    uvcDispSrc3.roiY = 0;
    uvcDispSrc3.roiW = usbDispWidth;
    uvcDispSrc3.roiH = usbDispHeight;
    uvcDispSrc3.fmt	= SP5K_GFX_FMT_YUV422;
    uvcDispDst.pbuf = (UINT8 *)usbDispYuvBuf[outIdx];
    uvcDispDst.bufW = usbDispWidth;
    uvcDispDst.bufH = usbDispHeight;
    uvcDispDst.roiX = 0;
    uvcDispDst.roiY = 0;
    uvcDispDst.roiW = usbDispWidth;
    uvcDispDst.roiH = usbDispHeight;
    uvcDispDst.fmt	= SP5K_GFX_FMT_YUV422;
    sp5kGfxObjectScale(&uvcDispSrc2, &uvcDispSrc3);
    sp5kGfxObjectBlend(&uvcDispSrc1, &uvcDispSrc3, &uvcDispDst, BLEND_ALPHA);

    /* put yuv data to dispbuf */
    ret = sp5kUsbDispYuvSend((UINT32)uvcDispDst.pbuf);
    profLogPrintf(0, "pvUrgentCb_UvcDispYuv yuv idx=%d ret=%d", outIdx, ret);
    if (ret == SUCCESS) {
        outIdx++;
        if (outIdx >= sp5kUsbInfoGet(SP5K_USB_DISP_YUV_BUF_NUM_GET)) {
            outIdx = 0;
        }
    }

    profLogPrintf(0, "pvUrgentCb_UvcDispYuv e");
    return ret;
}

/*-------------------------------------------------------------------------
 *  Function Name : sample_UvcDispCbStart
 *  Description :
 *------------------------------------------------------------------------*/
UINT32 sample_UvcDispCbStart(
    UINT16 Width,
    UINT16 Height,
    UINT32 fps
)
{
    sp5kGfxPageCapabDesc_t cap;
    sp5kGfxObj_t gfxObjSrc, gfxObjDst;
    UINT8 i;

    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_MOV);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, 48000);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, 16);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CHANNELS, 2);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_PCM);
    sp5kAudDevSrcSet(SP5K_AUD_DEV_REC, SP5K_AUD_DEV_REC_SRC_MIC_IN|SP5K_AUD_REC_CH_ALL);
    sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC, 31);
    /* Setup width, height, type for 1st stream */
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, 1920);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, 1080);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, 18000000);
    /* Setup width, height, type for 2nd stream */
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_WIDTH, 1920);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_HEIGHT, 1080);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_AVG_BITRATE, 18000000);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_FRAME_RATE, 30);
    sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_INPUT_FRAME_RATE, 30);

    sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
    sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

    printf("sample_UvcDispCbStart\n");
    usbDispWidth = Width;
    usbDispHeight = Height;
    sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_DISP_YUV_BUF_NUM_SET, 6);
    for (i = 0; i < sp5kUsbInfoGet(SP5K_USB_DISP_YUV_BUF_NUM_GET); i++) {
        usbDispYuvBuf[i] = (UINT32)sp5kYuvBufferAlloc(Width, Height);
    }

    printf("\n######### %dx%d fps:%u buf num:%d\n", Width, Height, fps, sp5kUsbInfoGet(SP5K_USB_DISP_YUV_BUF_NUM_GET));

    /* clear OSD layer */
    appOsdLib_OsdLayerClearDraw();
    /* draw video and battery icon on OSD layer */
    appOsdLib_OsdLayIconDraw(4, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MODE_VIDEO);
    appOsdLib_OsdLayIconDraw(52, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_QUALITY_FINE);
    appOsdLib_OsdLayIconDraw(187, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MOVIE_SIZE_1080P);
    appOsdLib_OsdLayIconDraw(2, 204, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BATTERY_FULL);

    /* prepare OSD buffer */
    if (pdstBuf == NULL) {
        pdstBuf = sp5kYuvBufferAlloc(Width, Height);
    }

    sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0, &cap);
    printf("\n######### cap:%d, %d\n", cap.frmW, cap.frmH);
    gfxObjSrc.pbuf = cap.pbuf;
    gfxObjSrc.bufW = cap.frmW;
    gfxObjSrc.bufH = cap.frmH;
    gfxObjSrc.roiX = 0;
    gfxObjSrc.roiY = 0;
    gfxObjSrc.roiW = cap.frmW;
    gfxObjSrc.roiH = cap.frmH;
    gfxObjSrc.fmt  = SP5K_GFX_FMT_IDX8;
    gfxObjDst.pbuf = pdstBuf;
    gfxObjDst.bufW = Width;
    gfxObjDst.bufH = Height;
    gfxObjDst.roiX = 0;
    gfxObjDst.roiY = 0;
    gfxObjDst.roiW = Width;
    gfxObjDst.roiH = Height;
    gfxObjDst.fmt = SP5K_GFX_FMT_YUV422;
    sp5kGfxObjectScale(&gfxObjSrc, &gfxObjDst);

    /* video urgent cb register */
    sp5kPrevUrgentCallback_t cbs[2];
    cbs[0].fp = cbs[1].fp = NULL;
    cbs[1].fp = pvUrgentCb_UvcDispYuv;
    sp5kVideoUrgentCallbackSet(1, 0, cbs, 2);

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : sample_UvcDispCbStop
 *  Description :
 *------------------------------------------------------------------------*/
UINT32 sample_UvcDispCbStop(
    void
)
{
    printf("sample_UvcDispCbStop\n");

    if (pdstBuf != NULL) {
        sp5kYuvBufferFree(pdstBuf);
        pdstBuf = NULL;
    }

    if (pdstBuf1 != NULL) {
        sp5kYuvBufferFree(pdstBuf1);
        pdstBuf1 = NULL;
    }

    /* video urgent cb unregister */
    sp5kPrevUrgentCallback_t cbs[2];
    cbs[0].fp = cbs[1].fp = NULL;
    sp5kVideoUrgentCallbackSet(1, 0, cbs, 2);

    return SUCCESS;
}

#if IMG_EFFECT
#include "app_ie_api.h"
static UINT32 opt_digital_effect = 0, opt_brightness = 0, opt_contrast = 0, opt_hue=0;
UINT32
myImgEditProc(
	UINT32 addr, UINT32 w, UINT32 h
)
{
    printf("[%s] s %d\n",__FUNCTION__,__LINE__);
/*Iq setting*/
	sp5kIqModeSet(SP5K_MODE_STILL_PLAYBACK);
#if 0/*V33*/
	sp5kIqCfgSet(SP5K_IQ_CFG_DIGI_EFF_U_CONTRAST, 0x40);
	sp5kIqCfgSet(SP5K_IQ_CFG_DIGI_EFF_V_CONTRAST, 0x40);
#else/*V35*/
    if(opt_digital_effect == UI_EFFECT_BRIGHTNESS_CONTRAST){
        appIqEffectSet(opt_digital_effect, opt_brightness, opt_contrast);
    }   
    else if(opt_digital_effect == UI_EFFECT_HUE){
        appIqEffectSet(opt_digital_effect, opt_hue);
    }
    else{
        appIqEffectSet(opt_digital_effect);
    }
#endif
	sp5kIqModeSetDone(SP5K_MODE_STILL_PLAYBACK);

/*create a new buffer*/
	sp5kGfxObj_t o1 = { (UINT8 *)addr, NULL, w, h, 0, 0, SP5K_GFX_FMT_YUV422, w, h };
	/* SP5K_GFX_OBJECT_EDIT_CDSP src/dst must be different so we need a copy */ 
	sp5kGfxObj_t o2 = o1;  
	o2.pbuf = sp5kYuvBufferAlloc(w, h);
	sp5kGfxObjectCopy(&o1, &o2);

/*CDSP iq effect*/
    sp5kGfxObjectEdit(&o2, &o1, SP5K_GFX_OBJECT_EDIT_CDSP, 0);      /*CDSP effect o1*/
    fsSimpleWrite("D:\\Edit_001.yuv", o1.pbuf , w*h*2);

#if 0
	sp5kYuvBufferFree(o1.pbuf); /*NG*/
#else
    if (o2.pbuf != NULL) {
       sp5kYuvBufferFree(o2.pbuf);
       o2.pbuf = NULL;
    }
#endif

    printf("[%s] e %d\n",__FUNCTION__,__LINE__);
	return SUCCESS;
}
	
void flow_edit(UINT32 opt1, UINT32 opt2, UINT32 opt3)
{
	printf("[imgHue]%s %d s ----------\n",__FUNCTION__,__LINE__);

    opt_digital_effect = opt1;
    if(opt_digital_effect == UI_EFFECT_BRIGHTNESS_CONTRAST){
        opt_brightness = opt2;
        opt_contrast = opt3;
    }
    else if(opt_digital_effect == UI_EFFECT_HUE){
        opt_hue = opt2;
    }

    sp5kImageEditCallbackSet(myImgEditProc);
    sp5kImageEdit(SP5K_IMGEDIT_CUSTOMIZE|SP5K_IMGEDIT_ATTR_PREVIEW, 0, 0, 0, 0);

    /* un-register callback function */
    sp5kImageEditCallbackSet(NULL);
    printf("[imgHue]%s %d e ----------\n",__FUNCTION__,__LINE__);
}
#endif

#if (SP5K_ICAT_EIS && APP_RTMP)

void EIS_STREAMING()
{

	{
		printf("MEDIA_REC_STREAM_2ND\n");
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_BRC_TYPE, SP5K_MEDIA_VBR);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_WIDTH, 1920);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_HEIGHT, 1080);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_INPUT_FRAME_RATE,15);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_FRAME_RATE, 15);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_MAX_BITRATE, 1200000);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_CODEC, MEDIA_VIDEO_H264);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_VLC_BUF_CNT, 4);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_VLC_BUF_SIZE, 1400000);/*SHEN: bitrate -> byterate*/
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_H264_GOP_NO, 15);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_H264_GOP_STRUCTURE, SP5K_MEDIA_H264_GOP_IPPP);

		{
			UINT32 h264Qp;
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_H264_QP, &h264Qp);
			h264Qp = h264Qp & 0xFF000000;
			h264Qp = h264Qp | (20 << 16) | (38 << 8) | (31 << 0);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_H264_QP, h264Qp);
		}
		{
			UINT32 h264QpI;
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_H264_QP_I, &h264QpI);
			h264QpI = h264QpI & 0xFF000000;
			h264QpI = h264QpI | (33 << 8) | ( 43<< 0);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_H264_QP_I, h264QpI);
		}
	}

	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CODEC, VIDEO_AUDIO_CODEC );
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, 44100/*VIDEO_AUDIO_SAMPLE_RATE*/ );
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, 16/*VIDEO_AUDIO_SAMPLE_BIT*/ );
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CHANNELS, 1/*VIDEO_AUDIO_CHANNEL*/ );

	{
		appAeFrameRateSet(30);
		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, 0x36/*SENSOR_MODE_1080P_30FPS*/);
	}


/****************************	REC 	*************************************************/
	sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

	appVideo_SetRscEis(1, 1, SP5K_EIS_ALGO_COMP_TYPE_IME);

	sp5kSystemCfgSet(SP5K_CDSP_SYS_CFG,CDSP_SYS_CFG_DIGI_EFF_Y_REMAP,1);

    sp5kModeSet(SP5K_MODE_VIDEO_RECORD);
    sp5kModeWait(SP5K_MODE_VIDEO_RECORD);


}
#endif

  /*--------------------------------------------------------------------------*
 * Function name	: appFileMove
 * Function	       	: test  command for rename the file
 * Return value   	: void 
 * Parameter1    	: src file pointer
 * Parameter2    	: dst file pointer
 * History	   :
 *                :  2018.05.09  Paul.Chang     "First Add "
 *--------------------------------------------------------------------------*/    
void appFileMove(UINT8 *src, UINT8 *dst)
{
	BOOL exist ;
	printf("fsmv [%s] [%s]\n", src, dst);

	exist = appFileExistCheck(src);
	if(exist )
	{
		UINT32 ret;
		printf("file exist \n");
		ret = sp5kFsRename(src, dst);
		if(ret == SUCCESS)
			printf("Rename SUCCESS\n");
		else
			printf("Rename FAILED\n");
	
	}
	else
		printf("File not exist\n");
	
	
}

/*-------------------------------------------------------------------------
 *  Function Name : appCustomerCmd
 *  Description :
 *------------------------------------------------------------------------*/
void
appCustomerCmd(
	UINT32 argc,
	char *arg[],
	UINT32 v[]
)
{
	char *param=arg[0];
	static UINT8 paraInited=0;

	if (param[0] == '.') {
		int i;
		for (i=0 ; i<argc-1 ; i++) {
			*(arg[i]+strlen(arg[i])) = ' ';
		}
		appScriptQuickRun((char *)(param+1));
		return;
	}

	/* custom command */
    if (strcmp(param, "host")==0) {
		hostCmdHandle();
	}
	else if (strcmp(param, "zoom")==0) {
		appDZoomEnableCtrl(v[1]);
	}
#if IMG_EFFECT
    else if (strcmp(param, "iedit")==0) {
        printf("iedit s\n");
        flow_edit(v[1], v[2],v[3]);
        printf("iedit e\n");
    }
#endif
#if HW_FACETRACK
	else if (strcmp(param, "ftrack") == 0) {
		UINT32 mode;
		sp5kModeGet(&mode);
		if (mode != SP5K_MODE_STANDBY) {
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
		}
		if (v[1] == 1) {
			
			appFdCreateEx(FACE_DETECT_ONLY);
			sp5kFaceTrackModeSet(SP5K_FACE_TRACK_MODE_PV_ON);
		}
		else {
			sp5kFaceTrackModeSet(SP5K_FACE_TRACK_MODE_OFF);
		}
		if (mode != SP5K_MODE_STANDBY) {
			sp5kModeSet(mode);
			sp5kModeWait(mode);
		}
	}
#endif

#if APP_RTMP
	  else if (!strcmp(param, "rtmp")) {
				char cmd[16];
				
				UINT32 cur_mode;
				if (argc > 1) {
					static UINT32 scHandle;
					if (!strcmp(arg[1], "init")) {
						FwInit();
					}
	
					if (!strcmp(arg[1], "uninit")) {
						FwUninit();
					}
					else if (!strcmp(arg[1], "eis"))
					{
						static UINT32  curMode;
	
						sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAM_EN, 1); /* enable the multi stream mode */
	
						scHandle = sp5kMediaRecControl(SP5K_MEDIA_REC_SC_CREATE, SP5K_MEDIA_REC_SC_TYPE_FRAMEWORK, SP5K_MEDIA_REC_STREAM_2ND, SP5K_MEDIA_AUDIO_AAC);
						if (!scHandle)
						{
							printf("ERROR  HANDLE!!\n");
							return;
						}
						sp5kMediaRecControl(SP5K_MEDIA_REC_SC_ATTACH, scHandle);
						sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAMS, SP5K_MEDIA_REC_STREAM_2ND);
	
				#if SP5K_ICAT_EIS
						EIS_STREAMING();
				#endif
						FwPushRtmp(arg[2],arg[3]);
					}
	
					else if (!strcmp(arg[1], "start")) {
						if(argc == 4){
							UINT32 h264Qp;
	
							sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, 1280);
							sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, 720);
							sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);
	
							/* ini:36 min:default max:51 */
							sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_H264_QP, &h264Qp);
							h264Qp = h264Qp & 0xFF000000;
							h264Qp = h264Qp | (20 << 16) | (51 << 8)| (36 << 0);
							sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_QP, h264Qp);
							sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_NO, 15);
	
							sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, 500000);
							sp5kMediaRecCfgSet(SP5K_MEDIA_REC_INPUT_FRAME_RATE, 30);
							sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, 30);
							sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_STRUCTURE, SP5K_MEDIA_H264_GOP_IPPP);
							sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, VIDEO_AUDIO_CODEC);
	
							sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAM_EN, 1); /* enable the multi stream mode */
							sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAMS, SP5K_MEDIA_REC_STREAM_1ST);
	
							scHandle = sp5kMediaRecControl(SP5K_MEDIA_REC_SC_CREATE, SP5K_MEDIA_REC_SC_TYPE_FRAMEWORK, SP5K_MEDIA_REC_STREAM_1ST, SP5K_MEDIA_AUDIO_AAC);
							sp5kMediaRecControl(SP5K_MEDIA_REC_SC_ATTACH, scHandle);
	
							/* Modesw */
	
							sp5kModeGet(&cur_mode);
							if (cur_mode == SP5K_MODE_VIDEO_RECORD) {
								printf("cur mode is SP5K_MODE_VIDEO_RECORD\n");
							}
							else {
							/* Modesw */
								sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW); /* need to enter once for record size active */
								sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
								sp5kModeSet(SP5K_MODE_VIDEO_RECORD);
								sp5kModeWait(SP5K_MODE_VIDEO_RECORD);
								/*sp5kMediaRecControl(SP5K_MEDIA_REC_STREAM_START, SP5K_MEDIA_REC_STREAM_1ST);*/
								printf("set SP5K_MODE_VIDEO_RECORD mode\n");
							}
	
							FwPushRtmp(arg[2],arg[3]);
						}
						else{
							printf("usage: fw rtmp serverpath cfg\n");
						}
					}
					else if (!strcmp(arg[1], "stop"))
					{
						FwPushRtmpStop();
						
						sp5kModeGet(&cur_mode);
						printf("[rtmp stop]cur mode %x\n", cur_mode);
						if(cur_mode == SP5K_MODE_VIDEO_RECORD){
							printf("modesw from recode to pv\n");
							sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
							sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
						}
						else{
							printf("[rtmp stop]cur mode 0x%x error\n", cur_mode);
						}
						/*sp5kMediaRecControl(SP5K_MEDIA_REC_STREAM_STOP, SP5K_MEDIA_REC_STREAM_1ST);*/
						sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DETACH, scHandle);
						sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DESTROY, scHandle);
					}
				}
			}
#endif

    else if (strcmp(param, "osall")==0){
        extern void sp5kCmdExec(char *cmd);

        /*v[1] 0:Disable , else adjust watch dog assert time*/
        if(v[1]==0)
        {
            printf("[host]Disable SW WDG !\n");
            appWatchDogModify(0);
        }
        else
        {
            printf("[host]SW WDG set time(%u sec).\n",v[1]);
            appWatchDogModify(v[1]*100);
        }

        printf("[host]Execute os all...\n");
        sp5kCmdExec("os all");

        printf("\n[host]Enable SW WDG !(%d sec)\n",(WATCHDOG_TIMEOUT_ASSERT*TICK_UNIT)/1000);
        appWatchDogModify(watchDog_timeout_setting);
    }
    else if (strcmp(param, "wdgon")==0) {
        /*v[1] 0:rollback , else adjust watch dog assert time*/
        if(v[1]==0)
        {
            printf("[host]Enable SW WDG !(%d sec)\n",(WATCHDOG_TIMEOUT_ASSERT*TICK_UNIT)/1000);
            appWatchDogRollBack();
        }
        else
        {
            printf("[host]SW WDG set time(%u sec).\n",v[1]);
            appWatchDogModify(v[1]*100);
        }
    }
    else if (strcmp(param, "wdgoff")==0) {
        printf("[host]Disable SW WDG !\n");
        appWatchDogModify(0);
    } else if ( strcmp(param, "poweroff" ) == 0 ) {
	/*	sp5kHostMsgSend( SP5K_MSG_POWER_OFF, 0 );*/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		NewAVNModel = TRUE;
		if (argc == 2)
			appInfoSWWDTWakeup(v[1]);
#endif
		appPowerOffUiMsgSend(APP_POWER_OFF_USB_PLUG);
	}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	else if (strcmp(param, "connstop")==0) {
		PlayPosCountStop();
		RegularCountStop();
		ConnectDevCountStop();
	}
#endif
	/*paul 20180509 add for test*/
	else if (strcmp(param, "fsmv")==0) {		
		if (argc==3)
		{
			appFileMove((UINT8 *)arg[1], (UINT8 *)arg[2]);
		}
		else
		{
			printf("fsmv cmd error\n");
		}
	}
	else if (strcmp(param, "dev")==0) {
		if (strcmp(arg[1], "gpioInit")==0) {
			printf("gpio Init...\n");
			gpioInitialCfgSet();
		}
		else if (strcmp(arg[1], "lcdInit")==0) {
			printf("panel Init...\n");
			appPanelInit();
		}
		else if (strcmp(arg[1], "blOn")==0) {
			printf("BackLight On...\n");
			LCD_BACKLIGHT_ON;
		}
		else if (strcmp(arg[1], "blOff")==0) {
			printf("BackLight Off...\n");
			LCD_BACKLIGHT_OFF;
		}
		else if (strcmp(arg[1], "btnInit")==0) {
			printf("button Init...\n");
			appBtnInit();
		}
	}
	else if (strcmp(param, "aeoff")==0) {
		sp5kAeModeSet(0);
		sp5kAwbModeSet(0);
		printf("AE/AEB Off\n");
	}
	else if (strcmp(param, "aeon")==0) {
		sp5kAeModeSet(2);
		sp5kAwbModeSet(1);
		printf("AE/AWB On\n");
	}
	else if (strcmp(param, "log")==0) {
        printf("0:off , 1:on\n");
        sp5kLogControl(v[1]);    /* 0:SP5K_LOG_DISABLE, 1:SP5K_LOG_ENABLE*/
	}
	else if (strcmp(param, "sio")==0) {
        printf("Please use log replace sio cmd.\n");
        sioCtrl(v[1]);
	}
	else if (strcmp(param, "fcopy")==0) {
		if (argc==3)
            appFileCopy((UINT8 *)arg[1], (UINT8 *)arg[2]);
	}
	else if (strcmp(param, "ad")==0) {
		UINT32 i,ch,val[8],min[8],max[8],avg[8],cnt[8];
		if (v[1]==0)
			v[1] = 100;

		memset(min, 0xff, sizeof(min));
		memset(max, 0, sizeof(max));
		memset(avg, 0, sizeof(avg));
		memset(cnt, 0, sizeof(cnt));
		for (i=0 ; i<v[1] ; i++) {
			for (ch=0 ; ch<8 ; ch++) {
				if (sp5kAdcRead(ch, &val[ch])==SUCCESS) {
					if (min[ch]>val[ch]) min[ch] = val[ch];
					if (max[ch]<val[ch]) max[ch] = val[ch];
					avg[ch] += val[ch];
					cnt[ch] ++;
				}
			}
			if (v[1]!=100) printf("%4u %4u %4u %4u %4u %4u %4u %4u\n", val[0], val[1], val[2], val[3],val[4],val[5],val[6],val[7]);
		}
		if (v[1]==100) {
			printf("ch times  avg (8b)  min  max\n");
			for (ch=0 ; ch<8 ; ch++) {
				avg[ch] /= cnt[ch];
				printf("%2u %5u %4u(%3d) %4u %4u\n", ch, cnt[ch], avg[ch], avg[ch]/4, min[ch], max[ch]);
			}
		}
	}
	else if (strcmp(param, "modeget")==0) {
		UINT32 mode ;
		sp5kModeGet(&mode);
		printf("sp5kModeGet:0x%x\n",mode);
	}
	else if (strcmp(param, "modeset")==0) {
		printf("sp5kModeSet(0x%x)\n",v[1]);
		if ((v[1]==0x11)||(v[1]==0x13)||(v[1]==0x21)||(v[1]==0x23)||(v[1]==0x88)) {
			sp5kModeSet(v[1]);
			printf("sp5kModeWait(0x%x)\n",v[1]);
			sp5kModeWait(v[1]);
			printf("sp5kModeWait(0x%x) complete\n",v[1]);
		}
		else {
			printf("mode != (0x11 | 0x13 | 0x21 | 0x23 | 0x88)\n");
		}
	}
	else if (strcmp(param, "smode")==0) {
		if ((v[1]&0x30)==0x30)
			sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW, v[1]);
		else
			sp5kSensorModeCfgSet(SP5K_MODE_STILL_SNAP, v[1]);

		sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_FIT, 1);
		sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
		sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
		printf("[modesw] stillpv -> stillpv 0x%x\n",v[1]);
	}
#if SP5K_SENSOR_DUAL_MODE
	else if (strcmp(param, "serdessize")==0) {
       		printf("serdes size report\n");
	}
#endif
	else if (strcmp(param, "s2s")==0) {
		/*need open #if S2STEST & close QV & pvqv*/
		sp5kHostMsgSend(APP_KEY_PRESS_S1,0);
	}
	else if (strcmp(param, "diskinfoget")==0){
		appDiskCapability_t* diskCapB;
		diskCapB = appSdInfoGet();
		appCardSpeedGet(SP5K_DRIVE_SD);
		printf("sdMode[0x%x], sdType[0x%x]\n", diskCapB->sdMode, diskCapB->sdType);
	}
	else if (strcmp(param, "state")==0) {
		appStateChange(v[1],v[2]);
	}
	else if (strcmp(param, "profinit")==0) {
		profLogInit(v[1],v[2]);
	}
	else if (strcmp(param, "usbon")==0) {
		if ((v[1]>0)&&(v[1]<=4)) {
			sp5kUsbSwCon(0);
			sp5kUsbModeSet(v[1]);
			if (v[1] ==1) {
				//SHEN: UVC+UAC bandwidth about 10MBps, so video can set  7MBps for smooth.
				//      UVC only, can set 12MBps.
				sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_MJPG_BYTERATE_SET, 7*1024*1024);
				// H264 bitrate
				sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, 7*1024*1024);

				/* can refer to cmd_usb.c cmd: usb pc */
				/*sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_VER_SET,1);*/
				/*sp5kResourceFileLoad( SP5K_RES_USB_CONF_BIN, RES_PLATFORM"USBUVC15.BIN" );*/

				sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_VER_SET,0);
				sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_IF_EXCHANGE_SET, 1);
				sp5kResourceFileLoad( SP5K_RES_USB_CONF_BIN, RES_PLATFORM"USBUVC.BIN" );
				printf("usbon PCCAM: IF1 mapping to EP1\n");
			}
			else if (v[1] ==2) {
				printf("usbon MSDC\n");
			}
			else if (v[1] ==3) {
				printf("usbon SIDC\n");
			}
			else {
				printf("usbon APITEST\n");
			}
			sp5kUsbSwCon(1);
		}
		else {
			printf("usbon Fail\n");
		}
	}
    else if (strcmp(param, "usbuvcdisp") == 0) {
        sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_DISP_CB, sample_UvcDispCbStart, sample_UvcDispCbStop);
    }
    else if (strcmp(param, "usbuvcosd") == 0) {
        if (v[1] == 1) {
            if (v[2] == 0) {
                printf("USB UVC clear video icon !\n");
                appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 4, 6, 36, 32, 0, 0);
            }
            else {
                printf("USB UVC draw video icon !\n");
                appOsdLib_OsdLayIconDraw(4, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MODE_VIDEO);
            }
        }
        else if (v[1] == 2) {
            if (v[2] == 0) {
                printf("USB UVC clear quality icon !\n");
                appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 52, 6, 36, 32, 0, 0);
            }
            else {
                printf("USB UVC draw quality icon !\n");
                appOsdLib_OsdLayIconDraw(52, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_QUALITY_FINE);
            }
        }
        else if (v[1] == 3) {
            if (v[2] == 0) {
                printf("USB UVC clear video size icon !\n");
                appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, 187, 6, 36, 32, 0, 0);
            }
            else {
                printf("USB UVC draw video size icon !\n");
                appOsdLib_OsdLayIconDraw(187, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MOVIE_SIZE_1080P);
            }
        }
        else if (v[1] == 4) {
            if (v[2] == 0) {
                printf("USB UVC clear battery icon !\n");
                appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 2, 204, 36, 32, 0, 0);
            }
            else {
                printf("USB UVC draw battery icon !\n");
                appOsdLib_OsdLayIconDraw(2, 204, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BATTERY_FULL);
            }
        }
        else {
            if (v[2] == 0) {
                printf("USB UVC clear all icon !\n");
                appOsdLib_OsdLayerClearDraw();
            }
            else {
                printf("USB UVC draw all icon !\n");
                appOsdLib_OsdLayIconDraw(4, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MODE_VIDEO);
                appOsdLib_OsdLayIconDraw(52, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_QUALITY_FINE);
                appOsdLib_OsdLayIconDraw(187, 6, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MOVIE_SIZE_1080P);
                appOsdLib_OsdLayIconDraw(2, 204, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BATTERY_FULL);
            }
        }

        sp5kGfxPageCapabDesc_t cap;
        sp5kGfxObj_t gfxObjSrc, gfxObjDst;
        sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0, &cap);
        gfxObjSrc.pbuf = cap.pbuf;
        gfxObjSrc.bufW = cap.frmW;
        gfxObjSrc.bufH = cap.frmH;
        gfxObjSrc.roiX = 0;
        gfxObjSrc.roiY = 0;
        gfxObjSrc.roiW = cap.frmW;
        gfxObjSrc.roiH = cap.frmH;
        gfxObjSrc.fmt  = SP5K_GFX_FMT_IDX8;
        gfxObjDst.pbuf = pdstBuf;
        gfxObjDst.bufW = usbDispWidth;
        gfxObjDst.bufH = usbDispHeight;
        gfxObjDst.roiX = 0;
        gfxObjDst.roiY = 0;
        gfxObjDst.roiW = usbDispWidth;
        gfxObjDst.roiH = usbDispHeight;
        gfxObjDst.fmt = SP5K_GFX_FMT_YUV422;
        sp5kGfxObjectScale(&gfxObjSrc, &gfxObjDst);
    }
    else if (strcmp(param, "usbuvrecset") == 0) {
        if (v[1] == 2) { //dual stream
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, 30);
            sp5kMediaRecCfgSet(SP5K_MEDIA_REC_INPUT_FRAME_RATE, 30);
            sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, SENSOR_MODE_1080P_30FPS);
        }
        else { //single stream
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, 60);
            sp5kMediaRecCfgSet(SP5K_MEDIA_REC_INPUT_FRAME_RATE, 60);
            sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, SENSOR_MODE_1080P_60FPS);
        }

        sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
        sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
    }
    else if (strcmp(param, "usbuvrecstart") == 0) {
        printf("\n@@start rec\n");
        if (v[1] == 2) { //dual stream
            sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAM_EN, 1);
            sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAMS, SP5K_MEDIA_REC_STREAM_1ST|SP5K_MEDIA_REC_STREAM_2ND);
            Mthfile = sp5kMediaRecControl(SP5K_MEDIA_REC_SC_CREATE, MEDIA_REC_SC_TYPE_FILE, SP5K_MEDIA_REC_STREAM_1ST, SP5K_MEDIA_AUDIO_PCM);
            sp5kMediaRecControl(SP5K_MEDIA_REC_SC_ATTACH, Mthfile);
            Mthfile1 = sp5kMediaRecControl(SP5K_MEDIA_REC_SC_CREATE, MEDIA_REC_SC_TYPE_FILE, SP5K_MEDIA_REC_STREAM_2ND, SP5K_MEDIA_AUDIO_PCM);
            sp5kMediaRecControl(SP5K_MEDIA_REC_SC_ATTACH, Mthfile1);
        }
        else { //single stream
            sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAM_EN, 0);
        }
        #if SUPPORT_RUNTIME_SWITCH_DRAM_CLK
        appSysClkSwitch(1);
        #endif
        sp5kModeSet(SP5K_MODE_VIDEO_RECORD);
        sp5kModeWait(SP5K_MODE_VIDEO_RECORD);
    }
    else if (strcmp(param, "usbuvrecstop") == 0) {
        printf("\n@@stop rec\n");
        sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
        sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
        if (v[1] == 2) { //dual stream
            sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DETACH, Mthfile);
            sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DETACH, Mthfile1);
            sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DESTROY, Mthfile);
            sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DESTROY, Mthfile1);
        }
        #if SUPPORT_RUNTIME_SWITCH_DRAM_CLK
        appSysClkSwitch(0);
        #endif
    }
	else if (!strcmp(param, "raw")) {
		sp5kStillCapCfgSet(SP5K_CAPTURE_SAVE_RAW, v[1]?1:0);
		sp5kStillCapCfgSet(SP5K_CAPTURE_SAVE_RAW_OUT_PATH, v[2]);
		printf("%s raw capture\n", v[1]?"Enable":"Disable");
	}
	else if (strcmp(param, "speaker")==0) {
		printf("speaker out\n\n");
		appBeep(TRUE, FALSE);
	}
	else if (strcmp(param, "hdmi")==0) {
		if (v[1] ==1) {
			sp5kHostMsgSend(SP5K_MSG_HDMI_PLUG,0);
			printf("send HDMI in message\n");
		}
		else if (v[1] ==0) {
			sp5kHostMsgSend(SP5K_MSG_HDMI_UNPLUG,0);
			printf("send HDMI out message\n");
		}
	}
	else if (strcmp(param, "tv")==0) {
		if (v[1] ==1) {
			sp5kHostMsgSend(APP_KEY_TV_IN,0);
			printf("send TV in message\n");
		}
		else if (v[1] ==0) {
			sp5kHostMsgSend(APP_KEY_TV_OUT,0);
			printf("send TV out message\n");
		}
	}
	else if (strcmp(param, "notch") == 0) { /* notch filter */
		sp5kAudFilterNotchCfg_t filter;
		filter.chnlId = SP5K_AUD_FILTER_ALL;
		filter.Fsample = v[1];

		filter.firstNotch.en = v[2];
		filter.firstNotch.Fcentre = v[3];
		filter.firstNotch.Qfactor = v[4]; /* suggest 10~20 */

		filter.secondNotch.en = v[5];
		filter.secondNotch.Fcentre = v[6];
		filter.secondNotch.Qfactor = v[7];

		filter.thirdNotch.en = v[8];
		filter.thirdNotch.Fcentre = v[9];
		filter.thirdNotch.Qfactor = v[10];

		filter.fourthNotch.en = v[11];
		filter.fourthNotch.Fcentre = v[12];
		filter.fourthNotch.Qfactor = v[13];

		filter.fifthNotch.en = v[14];
		filter.fifthNotch.Fcentre = v[15];
		filter.fifthNotch.Qfactor = v[16];

		printf("notch sample: %d\n", filter.Fsample);
		printf("1st(en,F,Q): (%d,%d,%f)\n", filter.firstNotch.en, filter.firstNotch.Fcentre, filter.firstNotch.Qfactor);
		printf("2nd(en,F,Q): (%d,%d,%f)\n", filter.secondNotch.en, filter.secondNotch.Fcentre, filter.secondNotch.Qfactor);
		printf("3rd(en,F,Q): (%d,%d,%f)\n", filter.thirdNotch.en, filter.thirdNotch.Fcentre, filter.thirdNotch.Qfactor);
		printf("4th(en,F,Q): (%d,%d,%f)\n", filter.fourthNotch.en, filter.fourthNotch.Fcentre, filter.fourthNotch.Qfactor);
		printf("5th(en,F,Q): (%d,%d,%f)\n", filter.fifthNotch.en, filter.fifthNotch.Fcentre, filter.fifthNotch.Qfactor);
		sp5kAudFilterNotchCfg(&filter);
	}
	else if (strcmp(param, "equalizer") == 0) { /* equalizer filter */
		sp5kAudFilterEqCfg_t filter;
		float lowShelfGain = 0, firstPeakGain = 0, secondPeakGain = 0, thirdPeakGain = 0, highShelfGain = 0;

		lowShelfGain =   v[4];
		firstPeakGain =  v[6];
		secondPeakGain = v[9];
		thirdPeakGain =  v[12];
		highShelfGain =  v[15];
		if (lowShelfGain > 200 || firstPeakGain > 200 || secondPeakGain > 200 || thirdPeakGain > 200 || highShelfGain > 200) {
			printf("Please input gain value 0~200, 101 is -100, 200 is -1\n");
			return;
		}

		filter.chnlId = SP5K_AUD_FILTER_ALL;
		filter.en = v[1];
		filter.Fsample = v[2];

		filter.lowShelf.Fcentre = v[3];
		if (lowShelfGain > 100) {
			filter.lowShelf.gain = lowShelfGain - 201;
		}
		else {
			filter.lowShelf.gain = lowShelfGain;
		}

		filter.firstPeak.Fcentre = v[5];
		if (firstPeakGain > 100) {
			filter.firstPeak.gain = firstPeakGain - 201;
		}
		else {
			filter.firstPeak.gain = firstPeakGain;
		}
		filter.firstPeak.Qfactor = v[7];

		filter.secondPeak.Fcentre = v[8];
		if (secondPeakGain > 100) {
			filter.secondPeak.gain = secondPeakGain - 201;
		}
		else {
			filter.secondPeak.gain = secondPeakGain;
		}
		filter.secondPeak.Qfactor = v[10];

		filter.thirdPeak.Fcentre = v[11];
		if (thirdPeakGain > 100) {
			filter.thirdPeak.gain = thirdPeakGain - 201;
		}
		else {
			filter.thirdPeak.gain = thirdPeakGain;
		}
		filter.thirdPeak.Qfactor = v[13];

		filter.highShelf.Fcentre = v[14];
		if (highShelfGain > 100) {
			filter.highShelf.gain = highShelfGain - 201;
		}
		else {
			filter.highShelf.gain = highShelfGain;
		}

		printf("equalizer en : %d  sample: %d\n", filter.en, filter.Fsample);
		printf("low shelf(F,G): (%d,%f)\n", filter.lowShelf.Fcentre, filter.lowShelf.gain);
		printf("1st peak(F,G,Q): (%d,%f,%f)\n", filter.firstPeak.Fcentre, filter.firstPeak.gain, filter.firstPeak.Qfactor);
		printf("2nd peak(F,G,Q): (%d,%f,%f)\n", filter.secondPeak.Fcentre, filter.secondPeak.gain, filter.secondPeak.Qfactor);
		printf("3rd peak(F,G,Q): (%d,%f,%f)\n", filter.thirdPeak.Fcentre, filter.thirdPeak.gain, filter.thirdPeak.Qfactor);
		printf("high shelf(F,G): (%d,%f)\n", filter.highShelf.Fcentre, filter.highShelf.gain);

		sp5kAudFilterEqCfg(&filter);
	}
	else if (strcmp(param, "ALC") == 0) { /* ALC setting */
		extern UINT8 gAudioAlcEnable;
		gAudioAlcEnable = v[1];
		appAudioRecVolumeConfig();
	}
	else if(!strcmp(param, "dualmode")){
		UINT32 pmode, dual_sensor_mode=0;

		sp5kModeGet(&pmode);

		dual_sensor_mode = v[1];

		sp5kModeCfgSet(SP5K_MODE_CFG_PV_SENSOR_OPT,dual_sensor_mode);
		profLogPrintf(0,"dualmode %d %d %x",__LINE__,dual_sensor_mode,pmode);
		sp5kSensorModeCfgSet(pmode, 0x30);

		profLogPrintf(0,"dualmode %d %d %x %x",__LINE__,dual_sensor_mode,pmode ,SP5K_MODE_SEC_SENSOR_SEL|pmode);
		sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|pmode, 0x30);

		profLogPrintf(0,"dualmode %d %d %x",__LINE__,dual_sensor_mode,pmode);

		sp5kModeSet(pmode);
		sp5kModeWait(pmode);

		profLogPrintf(0,"dualmode %d %d %x",__LINE__,dual_sensor_mode,pmode);
	}
	else if(!strcmp(param, "dualpip")) {
		UINT32 pmode, dual_sensor_mode=0,ret;
		static UINT32 standby_dup_enable=0;
		dual_sensor_mode = v[1];

		sp5kModeGet(&pmode);
		#if 1
		if(standby_dup_enable==0){
			standby_dup_enable = 1;
			sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP,1);
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
		}
		#endif

		sp5kSensorModeCfgSet(pmode, 0x30);
		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_RECORD, 0x30);
		sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|pmode, 0x30);
		sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_VIDEO_RECORD, 0x30);

		sp5kModeCfgSet(SP5K_MODE_CFG_PV_SENSOR_OPT,dual_sensor_mode);

		sp5kModeSet(pmode);
		sp5kModeWait(pmode);

		ret = sp5kModeCfgSet(SP5K_MODE_CFG_PV_SENSOR_OPT, -1);

		dualsenmode = ret&0xf;
		dualsenmodePlace = (ret>>4)&0xf;

		printf("ret=%x dualsenmode=%x dualsenmodePlace=%x \n",ret,dualsenmode,dualsenmodePlace);

		if(dual_sensor_mode == 4 || dual_sensor_mode == 8  ) {
			sample_PipStart(dual_sensor_mode);
		}
		else{
			sample_PipStop();
		}

		sp5kModeSet(pmode);
		sp5kModeWait(pmode);
	}
	else if (!strcmp(param, "dualstrm")) {
		UINT32 hFile, hRtp, dual_sensor_mode;
		dual_sensor_mode = v[1];

		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAM_EN, 1); /* enable the multi stream mode */

		/* Audio hardware is shared by audio codecs */
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, 48000);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, 16);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CHANNELS, 2);

		/* Setup width, height, type for 1st video stream */
		#if 0
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_STRUCTURE, GOP_IIII);
		#endif
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH,  1920);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, 1080);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);

		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_AVI);

		/* Setup width, height, type for 2nd video stream */
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_WIDTH,  640);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_HEIGHT, 480);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);

		/* Create file stream controllers */
		/* Enable two streams at the same time when video recording mode. */
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAMS,SP5K_MEDIA_REC_STREAM_1ST|SP5K_MEDIA_REC_STREAM_2ND);

		hRtp  = sp5kMediaRecControl(SP5K_MEDIA_REC_SC_CREATE, SP5K_MEDIA_REC_SC_TYPE_FILE, SP5K_MEDIA_REC_STREAM_1ST, SP5K_MEDIA_AUDIO_PCM);
		sp5kMediaRecControl(SP5K_MEDIA_REC_SC_ATTACH, hRtp);

		hFile = sp5kMediaRecControl(SP5K_MEDIA_REC_SC_CREATE, SP5K_MEDIA_REC_SC_TYPE_FILE, SP5K_MEDIA_REC_STREAM_2ND, SP5K_MEDIA_AUDIO_PCM);
		sp5kMediaRecControl(SP5K_MEDIA_REC_SC_ATTACH, hFile);

		/* Modesw */
		sp5kModeCfgSet(SP5K_MODE_CFG_PV_SENSOR_OPT,dual_sensor_mode);

		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, 0x30);
		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_RECORD, 0x30);
		sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_VIDEO_PREVIEW, 0x30);
		sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_VIDEO_RECORD, 0x30);

		sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
		sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

		if(dual_sensor_mode == 4 || dual_sensor_mode == 8  ) {
			sample_PipStart(dual_sensor_mode);
		}
		else{
			sample_PipStop();
		}

		sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW); /* need to enter once for record size active */
		sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

		sp5kModeSet(SP5K_MODE_VIDEO_RECORD);
		sp5kModeWait(SP5K_MODE_VIDEO_RECORD);

		profLogInit(2,0x10000);
		profLogModeSet(2,0);
		/* recording two files now */
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 5000);
		/* User logged in to check 1st */

		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAM_EN, 1); /* enable the multi stream mode */

		sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
		sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

		sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DETACH, hFile);
		sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DETACH, hRtp);
		sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DESTROY, hFile);
		sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DESTROY, hRtp);

		printf("end \n");
	}
#if !defined(PRJ_19DVR)
    #if ((SP5K_SENSOR_DUAL_MODE&&!(defined(HW_SBC))) || (SP5K_SENSOR_FAKE&&(defined(HW_SBC))) || \
		(SP5K_SENSOR_DUAL_MODE&&!(defined(HW_JETDVR))) || (SP5K_SENSOR_FAKE&&(defined(HW_JETDVR))) || \
		(SP5K_SENSOR_DUAL_MODE&&!(defined(HW_PDVR))) || (SP5K_SENSOR_FAKE&&(defined(HW_PDVR)))|| \
		(SP5K_SENSOR_DUAL_MODE&&!(defined(HW_17DVR))) || (SP5K_SENSOR_FAKE&&(defined(HW_17DVR))))
	else if (!strcmp(param, "burnspi")) {
		profLogAdd(0,"spiflashw s");
        #if SP5K_SENSOR_AR0330_SERDES_SPCA7002
        serdes_stream_off_set(2);
		SERDES_7002_WrSPIFlash(0);
        #else
        sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_ENABLE, 0, 0);
		BB_WrSPIFlash(0);
        sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_ENABLE, 1, 0);
        #endif
		profLogAdd(0,"spiflashw e");
		printf("spiflashw\n");
	}
	else if (!strcmp(param, "readspi")) {
		profLogAdd(0,"spiflashr s");
        #if SP5K_SENSOR_AR0330_SERDES_SPCA7002
        #else
        sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_ENABLE, 0, 0);
		BB_RdSPIFlash(0,512*1024);
        sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_ENABLE, 1, 0);
        #endif
		profLogAdd(0,"spiflashr e");
		printf("spiflashr\n");
	}
	else if (!strcmp(param, "yuvsnap")) {
		printf("yuvsnap=%x\n",v[1]);
            	appSetYUVSaveStatus(0);
		appYUVCapture(v[1]);
	}


    #endif
#endif
	/* param manager:
		param op: get, set, get info
		mode op: first power on, power on/off, switch mode */
	else if (strcmp(param, "p")==0) {
		if (strcmp(arg[1], "on")==0) {
			if (paraInited) return;
			UiParamViewInit();
			UiParamViewChangeMode(v[2]);
			paraInited = 1;
		}
		if (paraInited==0) return;
		if (strcmp(arg[1], "off")==0) {
			UiParamViewFini();
			paraInited = 0;
		} else if (strcmp(arg[1], "flush")==0) {
			UiParamViewFlush();
		} else if (strcmp(arg[1], "reset")==0) {
			UiParamViewReset();
		} else if (strcmp(arg[1], "mode")==0) {
			UiParamViewChangeMode(v[2]);
		} else if (strcmp(arg[1], "d")==0) {
			param_info_t info;
			UINT32 i;
			for (i=0 ; i<VIEW_PRM_COUNT ; i++) {
				UiParamViewGetInfo(i, &info); /* you can use VIEW_PRM_ID(af) instead of i */
				printf("%2u: %d(%d) mask=%04x flag=%s(%2x)\n", i, ((UINT8*)pViewParam)[i], info.defVal, info.availMask,
					(info.flag & PRM_SAVE) ? "Save" : (info.flag & PRM_NOUSE) ? "NA" : (info.flag & PRM_NOSEL) ? "Fixed" : "None", info.flag);
			}
		} else if (strcmp(arg[1], "s")==0) {
			((UINT8 *)pViewParam)[v[2]] = v[3];
		}
	}
    #if SP5K_SENSOR_DUAL_MODE
	else if (strcmp(param, "i2cw") == 0) {
        #if SP5K_SENSOR_AR0330_SERDES_SPCA7002
        serdes_spca7002_i2c_data_write(v[1], v[2]);
        #else
		appSPCA7002_I2CDataWrite(v[1], v[2]);
        #endif
	}
	else if (strcmp(param, "i2cr") == 0) {
        #if SP5K_SENSOR_AR0330_SERDES_SPCA7002
        UINT32 regdata;
        regdata = serdes_spca7002_i2c_data_read(v[1]);
        printf("reg 0x%x = 0x%2x\n", v[1], regdata);
        #else
		appSPCA7002_I2CDataRead(v[1]);
        #endif
	}
	#if SP5K_SENSOR_AR0330_SERDES_SPCA7002
    else if (strcmp(param, "rearid") == 0)
    {
        UINT32 id_sensor, id_7002;
        id_7002 = serdes_spca7002_i2c_data_read(0x72cf)<<8 |
                  serdes_spca7002_i2c_data_read(0x72ce);
        id_sensor = serdes_spca7002_i2c_data_read(0x72c7)<<8 |
                    serdes_spca7002_i2c_data_read(0x72c6);
        printf("7002   ID = 0x%4x\n", id_7002);
        printf("Sensor ID = 0x%4x\n", id_sensor);
        if(id_7002 == 0x7002 && id_sensor == 0x2604)
            printf("Success\n");
        else
            printf("Fail\n");
    }
	#endif
    #endif
	/* TG cmd */
	else if (strcmp(param, "tgset")==0) {
		UINT8 regdata[5];
		tgCmdMake(v[1], v[2], regdata);
		printf("set %s tg: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", tgCmdModeStrGet(tgCmdMode),
			regdata[0], regdata[1], regdata[2], regdata[3], regdata[4] );
		if (tgCmdMode==SP5K_MODE_STILL_PREVIEW || tgCmdMode==SP5K_MODE_VIDEO_PREVIEW) {
			UINT32 id;
			extern UINT32 sensorCmdportIdGet(UINT32 port);
			extern UINT32 sensorSerialAsyncWrite(UINT32 cmdportid,UINT32 nreg,const UINT8 *pdata);

			id = sensorCmdportIdGet(0);
			sensorSerialAsyncWrite(id, 1, regdata);
			}
			tgCmdUpdate(tgCmdMode, regdata);
		}
		else if (strcmp(param, "tgdump")==0) {
			tgCmdDump(tgCmdMode);
		} else if (strcmp(param, "tgclr")==0) {
			tgCmdClear(tgCmdMode);
			tgCmdDump(tgCmdMode);
		} else if (strcmp(param, "tgsave")==0) {
			((UINT8 *)pViewParam)[v[2]] = v[3];
			printf("tgcmd saved\n");
		} else if (strcmp(param, "tgmode")==0) {
			tgCmdMode = v[1]==0 ? SP5K_MODE_STILL_PREVIEW :
			v[2]==1 ? SP5K_MODE_VIDEO_PREVIEW : SP5K_MODE_STILL_SNAP;
			printf("set tgcmd mode to %s\n", tgCmdModeStrGet(tgCmdMode));
	}
	else if (strcmp(param, "xdata")==0 || strcmp(param, "x")==0) {
		UINT32 i;
		if (argc==1) {
			for (i=0 ; i<COUNT_OF(xdata) ; i++)
				printf("x[%u]=%u ", i, xdata[i]);
			printf("\n");
		} else {
			if (argc>COUNT_OF(xdata)) argc = COUNT_OF(xdata);
			for (i=1 ; i<argc ; i++)
				xdata[i-1] = v[i];
		}
	}
	else if (strcmp(param, "audio")==0) {
		printf("audio test\n\n");
		UINT32 hwAGCGain = 0x80000001;

		sp5kAudDevSrcSet(SP5K_AUD_DEV_REC, SP5K_AUD_DEV_REC_SRC_MIC_IN|SP5K_AUD_REC_CH_ALL);

		sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_WAV );
		sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_UNKNOWN );
		sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_PCM );
		sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, v[1]);
		sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, v[2]);
		sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CHANNELS, 2 );
		/*volume */
		sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC|SP5K_AUD_REC_CH_ALL, 32);
		sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST|SP5K_AUD_REC_CH_ALL, hwAGCGain);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_OFF);
		/* ALC off
		UINT16 AudAlcHb=800;
		UINT16 AudAlcLb=300;
		UINT8 AudAlcMaxVol=22;
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_MAX_VOL, AudAlcMaxVol);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_HB, AudAlcHb);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_LB, AudAlcLb);
		*/

		sp5kModeSet( SP5K_MODE_AUDIO_RECORD );
		sp5kModeWait(SP5K_MODE_AUDIO_RECORD);
		UINT32 i = 0;
		while(1){
			sp5kTimeDelay(3, 500);
			i++;
			if( i > 10 ){
				break;
			}
		}

		sp5kModeSet( SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		return;
	}
	/*weikai added 180524*/
#if defined(PRJ_JETDVR)  || defined( PRJ_17DVR)|| defined( PRJ_19DVR)/*Add for UART data recdvicer and handler command to do  Isaac 20170322 */
	else if(strcmp(param, "halpoweroff")==0){

		appInfoSWWDTWakeup(0);
		halPwrcEnable(0);
		while(1);
		
	 }	
	else if(strcmp(param, "versionGet")==0){


		appInfoFWUPDateVerGet((UINT8*) "D:\\xxx.BRN");
		
	 }	

       else if(strcmp(param, "secbase")==0){
		tmx_t time , back;

		sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &time);
		appInfoSecTimeToRealGet(appInfoSecTimeBaseGet(&time) , &back);
		
	 }
	else if (strcmp(param, "audpb")==0){    	
		UINT32 ret;    	UINT32 duration = 0;        
		printf("L=%d\n",__LINE__);		
		sp5kModeSet(SP5K_MODE_AUDIO_PLAYBACK);		
		sp5kModeWait(SP5K_MODE_AUDIO_PLAYBACK);        
		printf("L=%d\n",__LINE__);        
		printf("L=%d\n",__LINE__);        
		printf("fname=%s\n",(UINT8 *)arg[1]);    	/* set the filename to be playback */        
		sp5kMediaPlayAttrSet(SP5K_MEDIA_ATTR_FILE_NAME, (UINT32)arg[1]);       
		printf("L=%d\n",__LINE__);    	
		ret = sp5kMediaPlayAttrParse();    
		if (ret == SUCCESS) {    		
			sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_DURATION, &duration);    	
		}    	
		else {    	    
			printf("parse fail\n");    	        	
		}    	
		printf("dur=%u\n",duration);    	
		if (duration) {    		
			/* set sound volume */    		
			sp5kAudDevVolumeSet(SP5K_AUD_DEV_PLAY, 20);         		
			/* start to playback */    		
			sp5kMediaPlayControl(SP5K_MEDIA_PLAY_START, 0, 0);    	
			}    
		}
	else if (strcmp(param, "sdhealthcheck")==0){

		UINT32 ret = 0, sd_health = 0;

            ret = sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_DISK_HEALTH_QUERY, &sd_health);
            if (ret == 0) {
                /* Success, Toshiba spec Max Health = 0xFF */
                printf("SD card Health = %u%%\n", sd_health);
            }
            else {
                /* Fail, Maybe read error or this card did not support health command */
                printf("Fail to get SD Health. Error = %#X\n", sd_health);
            }
	}
	else if (strcmp(param, "TVGANTSC")==0){
		UINT32 dispHeight, dispWidth;

			#if 1
			sp5kResourceFileLoad(SP5K_RES_DISP_GAMMA , (char *)RES_DISP_DISPGAM0);
			//initial display color matrix
			sp5kResourceFileLoad(SP5K_RES_DISP_COLOR_MATRIX, (char *)RES_DISP_DISPMAX0); //TV
			#endif
			/* for better TV out display quality, window width must be multiple of osd buffer(320)	*/
			sp5kGfxInitCfgSet(SP5K_GFX_AB_FRAME_SIZE, 320, 240, 0, 0);
			sp5kDispAttrGet(SP5K_DISP_CHNL_1, SP5K_DISP_DEV_DIMENSION, (SINT32 *)&dispWidth, (SINT32 *)&dispHeight);


			sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_IMG_WINDOW, 0, 0, dispWidth, dispHeight);  /*shrink image layer too*/
			sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_OSD_WINDOW, 0, 0, dispWidth, dispHeight);  /*shrink osd layer too*/

			profLogAdd(0, "tvlcd: Osd TvMode :s");
			appOsdInit_TVModeSet(0);
			profLogAdd(0, "tvlcd: Osd TvMode :e");
	}
	else if(strcmp(param, "TVGA")==0){ 
		UINT32 val=0;/*200*/
		if (argc >= 2){
	       		val = v[1];
			sp5kResourceFileLoad(SP5K_RES_DISP_GAMMA ,  (const char *)arg[1]);
	
		}
		printf("GAMA     %u\n" ,val);		
	}
	else if(strcmp(param, "TVGet")==0){
		printf("SP5K_DISP_TV_HUE    %d\n" ,sp5kDispAttrGet(SP5K_DISP_CHNL_1   , SP5K_DISP_TV_HUE ));		
		printf("SP5K_DISP_TV_SATURATION     %d\n" ,sp5kDispAttrGet(SP5K_DISP_CHNL_1   , SP5K_DISP_TV_SATURATION  ));	
		printf("SP5K_DISP_TV_BLACK_LEVEL      %d\n" ,sp5kDispAttrGet(SP5K_DISP_CHNL_1   , SP5K_DISP_TV_BLACK_LEVEL));	
	}
	else if(strcmp(param, "Contrast")==0){ 
		if (argc >= 2){
			UINT32 val;/*200*/
	       		val = v[1];
			sp5kDispAttrSet(SP5K_DISP_CHNL_1 , SP5K_DISP_CONTRAST,val);
		}
		printf("SP5K_DISP_TV_HUE    %d\n" ,sp5kDispAttrGet(SP5K_DISP_CHNL_1   , SP5K_DISP_CONTRAST ));		
	}
	else if(strcmp(param, "TVHUESet")==0){ 
		if (argc >= 2){
			UINT32 val;
	       		val = v[1];
			sp5kDispAttrSet(SP5K_DISP_CHNL_1 , SP5K_DISP_TV_HUE,val);
		}
		printf("SP5K_DISP_TV_HUE    %d\n" ,sp5kDispAttrGet(SP5K_DISP_CHNL_1   , SP5K_DISP_TV_HUE ));		
	}
	else if(strcmp(param, "TVHUESATURATION")==0){	
		if (argc >= 2){
			UINT32 val;
	       		val = v[1];
			sp5kDispAttrSet(SP5K_DISP_CHNL_1 ,SP5K_DISP_TV_SATURATION, val);
		}
		printf("SP5K_DISP_TV_SATURATION     %d\n" ,sp5kDispAttrGet(SP5K_DISP_CHNL_1   , SP5K_DISP_TV_SATURATION  ));	

	}
	else if(strcmp(param, "TVBlackSet")==0){		
		if (argc >= 2){
			UINT32 val;
		        val = v[1];
			sp5kDispAttrSet(SP5K_DISP_CHNL_1 , SP5K_DISP_TV_BLACK_LEVEL ,val);
		}
		printf("SP5K_DISP_TV_BLACK_LEVEL      %d\n" ,sp5kDispAttrGet(SP5K_DISP_CHNL_1   , SP5K_DISP_TV_BLACK_LEVEL));	
	}
	else if(strcmp(param, "adcinit")==0)
	{
		sp5kAdcInit();
	}
	else if(strcmp(param, "adcen")==0)
	{
		sp5kAdcEnable(1);
	}
	else if(strcmp(param, "adcread")==0)
	{
		UINT32 pvalADC2= 0xffffffff;
		sp5kAdcRead(2,&pvalADC2);
		printf("ADC channel 2 %u\n",pvalADC2);
	}
	else if(strcmp(param, "hwadcver")==0)
	{
		UINT32 pvalADC2= 0xffffffff;
		sp5kAdcRead(4,&pvalADC2);
		printf("ADC channel 4 %u\n",pvalADC2);
	}

	else if(strcmp(param, "tvtest")==0){
	sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_SCALE_FACTOR, 640, 480,0x10010001);
	}
	else if (strcmp(param, "rawcwrite") == 0) {	    
		UINT8 *pbuf;		
		pbuf = (UINT8 *)sp5kMalloc(512);		
		//printf("pbuf=%x\n",pbuf);		
		memset(pbuf,0xff,512);		
		*pbuf = v[1];		
		*(pbuf+1) = v[2];        
		sp5kRawSegmentWrite(0, 512, pbuf);        
		//printf("@1,2=%x,%x",*pbuf, *(pbuf+1));        
		sp5kFree(pbuf);    
	}	
	else if (strcmp(param, "rawcread") == 0) 
		{	    
		UINT8 *pbuf;		
		pbuf = (UINT8 *)sp5kMalloc(512);		
		memset(pbuf,0x55,512);        
		//printf("@1,2=%x,%x",*pbuf, *(pbuf+1));		
		//printf("pbuf=%x\n",pbuf);        
		sp5kRawSegmentRead(0, 512,pbuf);        
		//printf("@1,2=%x,%x",*pbuf, *(pbuf+1));        
		sp5kFree(pbuf);    
	}
	else if (strcmp(param, "event") == 0) 
	{
	    int val = 1;

	    if (argc >= 2)
	        val = v[1];
	    extern SP5K_EVENT_FLAGS_GROUP appGsensor_sig;
	    sp5kOsEventFlagsSet(&appGsensor_sig, val, TX_INT_ENABLE);
	}
	else if (strcmp(param, "AVNinfo") == 0)
	{
	    extern void appInfoHandler(UINT8 *);

	    if (argc == 2)
		    appInfoHandler((UINT8 *)arg[1]);
	}


#endif

	#if SP5K_FW2_SUPPORT
	else if (strcmp(param, "fw2sup")==0) {
		
		#define FW_FILE_NAME   	"D:\\SPHOST.BRN"
		#define FW2_DBG   	0
		UINT32 curMode;
		UINT8 *pbuf1, *pbuf2;
		#if FW2_DBG
		UINT8 *pbuf3;
		#endif
		UINT32 i;
		printf("------------------------>fw2sup\n");
		
		sp5kModeGet(&curMode);
		if(curMode != SP5K_MODE_STANDBY){
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
		}

		pbuf1 = sp5kMalloc(1024);
		pbuf2 = pbuf1 + 512;
		for (i = 0; i < 512; i++) {
			pbuf1[i] = i;
			pbuf2[i] = 0;
		}
		#if FW2_DBG
		pbuf3 = sp5kMalloc(512);
		for (i = 0; i < 512; i++) {
			pbuf3[i] = 0;
		}
		#endif
		
		sp5kRawSegmentWrite(0, 512, pbuf2);/* reset rawC */
		#if FW2_DBG
		sp5kRawSegmentRead(0, 512, pbuf3);
		memdump(pbuf3, 512);
		for (i = 0; i < 512; i++) {
			pbuf3[i] = 0;
		}
		#endif

		printf("start FW update...\n");
		if (sp5kDscFirmwareUpdate( (UINT8 *)FW_FILE_NAME,0 )==SUCCESS)
		{
			printf("FW update OK!\n");
			sp5kRawSegmentWrite(0, 512, pbuf1);/* write the signature to rawC */
			#if FW2_DBG
			sp5kRawSegmentRead(0, 512, pbuf3);
			memdump(pbuf3, 512);
			for (i = 0; i < 512; i++) {
				pbuf3[i] = 0;
			}
			#endif
		}
		sp5kFree(pbuf1);
		#if FW2_DBG
		sp5kFree(pbuf3);
		#endif
	}
	else if (strcmp(param, "fw2test")==0) {
		printf("------------------------>fw2test\n");
		UINT8 *pbuf1, *pbuf2;
		UINT32 i;

		pbuf1 = sp5kMalloc(1024);
		pbuf2 = pbuf1 + 512;
		for (i = 0; i < 512; i++) {
			pbuf1[i] = i;
			pbuf2[i] = 0;
		}
		
		#if 0
		spiRawCWrite(pbuf1, 512, 0);
		spiRawCRead(pbuf2, 512, 0);
		#else
		sp5kRawSegmentWrite(0, 512, pbuf1);
		sp5kRawSegmentRead(0, 512, pbuf2);
		#endif
		if (memcmp(pbuf1, pbuf2, 512) == 0) {
			printf("rawC compare ok\n");
		}
		else {
			printf("rawC compare ng\n");
		}
		sp5kFree(pbuf1);
	}
	#endif
	else if (strcmp(param, "i2cinit")==0) {
        /*i2cinit [port] [pin] [SLAVE] [freq] [addr2byte] [readmode]*/

        UINT8 slaveAddr, freq, readMode;
        BOOL enAddr2Byte;
		UINT32  Parameter, PORT, PIN, ret;

        PORT = v[1];		/*SAMPLE: SP5K_SSC_PORT_6*/
        PIN = v[2];			/*SAMPLE: SP5K_SSC_CFG_PIN0*/
		slaveAddr = v[3]&0xFF;	/*8bit format*/
		freq = v[4]&0xFF;		/*I2C clock = (REGclk/2) / (4*freqDiv+4) */
		enAddr2Byte = (v[5])?1:0;	/* Use 16bit register address or NOT*/
		readMode = v[6] & 0x3;	/*  0: I2C read without dummy write (I2C normal read) 
						        1: I2C read with dummy write (I2C read with assigned register address) 
						        2: I2C read with dummy write, with stop bit omission (I2C read with assigned register address in one communication)					        */
		Parameter = ((slaveAddr<<16) | (freq<<8) | (enAddr2Byte<<5) | readMode<<1);
		printf("Initial SSC PORT=0x%X PIN=%u slaveAddr=0x%X freq=0x%X enAddr2Byte=%d, readMode=%d,  Parameter=0x%X \n", PORT, PIN, slaveAddr, freq, enAddr2Byte, readMode, Parameter);

		ret  = sp5kSscCfgSet(PORT, SP5K_SSC_CFG_MODE, SP5K_SSC_MODE_I2C);
		ret |= sp5kSscCfgSet(PORT, SP5K_SSC_CFG_PIN, PIN);
		ret |= sp5kSscCfgSet(PORT, SP5K_I2C_CFG_PHY, Parameter);
        printf("Initial complete, ret=0x%X\n", ret);
	}
	else if (strcmp(param, "i2cread")==0) {
        /*i2cread [port] [addr] [cnt]*/
		UINT32 subAddr, PORT, ret;
		UINT8 I2C_data[8]={0}, cnt, i;
        
        PORT = 		v[1];
		subAddr  = 	v[2]&0xFFFF;
		cnt = 		(v[3]>8)? 8:v[3];
		printf("I2C Read PORT=0x%X Addr=0x%X cnt=%d\n\n", PORT, subAddr, cnt);
        
		ret = sp5kI2cRead(PORT, subAddr, I2C_data, cnt);
		printf("\n\nret=0x%X, cnt=%d\n", ret, cnt);

        for ( i=0 ; i<cnt ; i++)
			printf("Data[%d] = 0x%x\n", i, I2C_data[i]);
	}
	else if (strcmp(param, "i2cwrite")==0) {
        /*i2cwrite [port] [addr] [wdata...] [cnt]*/
		UINT32 subAddr, cnt, PORT, ret;
		UINT8 I2C_data[8]={0}, i;
        
        PORT = 		v[1];
        subAddr = 	v[2]&0xFFFF;
		cnt = 		(v[4]>8)? 8:v[4];
		for ( i=0 ; i<cnt ; i++)
        {
			v[3] = v[3]>>8;
            I2C_data[i] = v[3]&0xFF;
        }
		printf("I2C Write PORT=0x%X Addr=0x%X cnt=%u\n\n", PORT, subAddr, cnt);

		ret = sp5kI2cWrite(PORT, subAddr, I2C_data, cnt);
		printf("I2C write, ret= 0x%X\n", ret);
	}
    else if (!strcmp(param, "lcdrotate")) {
        if(argc >= 2) {
            UINT32 curMode;

            sp5kModeGet(&curMode);
            if(curMode == SP5K_MODE_VIDEO_PREVIEW || curMode == SP5K_MODE_STILL_PREVIEW) {
                UINT32 rotate=v[1]<<12;

                sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_SCALE_FACTOR, -1, -1, 0<<16|3<<8|rotate|1);
                sp5kModeSet(curMode);
                sp5kModeWait(curMode);
            }
        }
        else {
            printf("lcdrotate usage:\n");
            printf("0:         0\n");
            printf("1:       180\n");
            printf("2:    H flip\n");
            printf("3:    V flip\n");
            printf("4:        90\n");
            printf("5:       270\n");
            printf("6: H flip 90\n");
            printf("7: V flip 90\n");
        }
    }
    else if (!strcmp(param, "osdtest")) {
        if(argc >= 2) {
    		if (strcmp(arg[1], "on") == 0) {
                sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_ACTIVE, 1);
            }
            else if (strcmp(arg[1], "off") == 0) {
                sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_ACTIVE, 0);
            }
            else
                printf("Please use osdtest to check osdtest usage\n");
        }
        else{
            printf("osdtest usage:\n");
            printf("osdtest on : Enable OSD layer\n");
            printf("osdtest off: Disable OSD layer\n");
        }
    }
    else if (!strcmp(param, "prescale")) {
        if(argc >= 2){
            if(v[1] >= 1 && v[1] <= 16){
                UINT8 nPresclEn = 1;
                UINT8 nPresclFac = v[1];
                UINT8 nUpdate = 1;      //update ime
                UINT8 nPreScaleSrc = 1; //Main
                UINT8 nScaleEngine = 3; //IME
                printf("prescale:%d %d\n", nPresclEn, nPresclFac);
                sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_SCALE_FACTOR, -1, -1, (nUpdate << 28)|(nPreScaleSrc << 24)|(nPresclEn << 21)|(nScaleEngine << 8)|(nPresclFac << 4)| 1);
                //sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
                //sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
            }
        }
    }
    #if TEST_ADAS_LDWS
    else if (!strcmp(param, "ldwsstart")) {
        if(argc >= 2){
            if(strcmp(arg[1], "--help") == 0){
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " Usage: ldwsstart [OPTION]...");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " Example:");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " \t ldwsstart x y distanceToCenterThr period");
            }
            else if(argc == 5){
                HOST_PROF_LOG_PRINT(LEVEL_INFO, "x:%d y:%d distance center threshold:%d period:%d \n", v[1], v[2], v[3], v[4]);
                appLdwsStart(v[1], v[2], v[3], v[4]);
            }
            else{
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " ldwsstart: invalid option");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " Try 'ldwsstart --help' for more information");
            }
        }
        else{
            if (argc == 1)
	            appLdwsStart(512, 512, 32, 1); /*parameter get by wiki*/
            else
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " Try 'ldwsstart --help' for more information");
        }
    }
    else if (!strcmp(param, "ldwsstop")) {
        HOST_PROF_LOG_PRINT(LEVEL_INFO, " stop ldws");
        appLdwsStop();
    }
    #endif
    #if TEST_ADAS_FCWS
    else if (!strcmp(param, "fcwsstart")) {
        if(argc >= 2){
            if(strcmp(arg[1], "--help") == 0){
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " Usage: fcwsstart [OPTION]...");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " Example:");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " \t fcwsstart x y night/day focalLength pixelUnit [period]");
            }
            else if(argc == 7){
                HOST_PROF_LOG_PRINT(LEVEL_INFO, "x:%d y:%d, night|day:%d focal lenght:%d pixel unit:%d period:%d", v[1], v[2], v[3], v[4], v[5], v[6]);
                HOST_ASSERT_MSG(v[3] >= NIGHT && v[3] < TIME_TOTAL, "error night|day value:%d, support range: [%d..%d]\n", v[3], NIGHT, TIME_TOTAL -1);
                appFcwsStart(v[1], v[2], v[3], v[4], v[5], v[6]);
            }
            else if(argc == 6){
                HOST_PROF_LOG_PRINT(LEVEL_INFO, "x:%d y:%d, night|day:%d focal lenght:%d pixel unit:%d", v[1], v[2], v[3], v[4], v[5]);
                HOST_ASSERT_MSG(v[3] >= NIGHT && v[3] < TIME_TOTAL, "error night|day value:%d, support range: [%d..%d]\n", v[3], NIGHT, TIME_TOTAL -1);
                appFcwsStart(v[1], v[2], v[3], v[4], v[5]);
            }
            else{
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " fcwsstart: invalid option");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " Try 'fcwsstart --help' for more information");
            }
        }
        else{
            if (argc == 1)
                appFcwsStart(512, 512, 1, 25000, 158);  /*parameter get by wiki*/
            else
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " Try 'fcwsstart --help' for more information");
        }
    }
    else if (!strcmp(param, "fcwsstop")) {
        HOST_PROF_LOG_PRINT(LEVEL_INFO, " stop fcws");
        appFcwsStop();
    }
    #endif
    #if TEST_ADAS_LPD
    else if (!strcmp(param, "lpdstart")) {
        appLpdStart();
    }
    else if (!strcmp(param, "lpdstop")) {
        appLpdStop();
    }
    #endif
    #if TEST_ADAS_ROD
    else if (!strcmp(param, "rodstart")){
        appRodStart();
    }
    else if (!strcmp(param, "rodstop")){
        appRodStop();
    }
    else if(!strcmp(param, "rodset")){
        if(argc >= 2){
            if(strcmp(arg[1], "input") == 0) {
                if(argc >= 3){
                    if(strcmp(arg[2], "main") == 0) {
                        appRodCfgSet(ROD_CFG_INPUT_MAIN);
                    }
                    else if(strcmp(arg[2], "sec") == 0) {
                        appRodCfgSet(ROD_CFG_INPUT_SEC);
                    }
                    else if(strcmp(arg[2], "pbvdo") == 0) {
                        appRodCfgSet(ROD_CFG_INPUT_PBVDO);
                    }
                }
            }
            else if(strcmp(arg[1], "srcw") == 0) {
                appRodCfgSet(ROD_CFG_SRC_W, v[2]);
            }
            else if(strcmp(arg[1], "srch") == 0) {
                appRodCfgSet(ROD_CFG_SRC_H, v[2]);
            }
            else if(strcmp(arg[1], "osdw") == 0) {
                appRodCfgSet(ROD_CFG_OSD_W, v[2]);
            }
            else if(strcmp(arg[1], "osdh") == 0) {
                appRodCfgSet(ROD_CFG_OSD_H, v[2]);
            }
            else if(strcmp(arg[1], "offsettoph") == 0) {
                appRodCfgSet(ROD_CFG_OSD_OFFSET_TOP_H, v[2]);
            }
            else if(strcmp(arg[1], "uph") == 0) {
                appRodCfgSet(ROD_CFG_UP_H, v[2]);
            }
            else if(strcmp(arg[1], "midh") == 0) {
                appRodCfgSet(ROD_CFG_MID_H, v[2]);
            }
            else if(strcmp(arg[1], "lowh") == 0) {
                appRodCfgSet(ROD_CFG_LOW_H, v[2]);
            }
        }}
#endif


    else if(!strcmp(param, "adasdump")){
        if(argc >= 2) {
    		if (strcmp(arg[1], "ldws") == 0) {
                #if TEST_ADAS_LDWS
                appLdwsDumpSetting();
                #endif
            }
            else if (strcmp(arg[1], "fcws") == 0) {
                #if TEST_ADAS_FCWS
                appFcwsDumpSetting();
                #endif
            }
            else{
                printf("Please use adasdump to check adasdump usage\n");
            }
        }
        else{
            printf("adasdump usage:\n");
            printf("adasdump ldws : dump ldws setting\n");
            printf("adasdump fcws: dump fcws setting\n");
        }
    }
    else if (!strcmp(param, "adastest")) {  //Ony for iCath testing  !
        if(argc >= 2){
            if(strcmp(arg[1], "--help") == 0){
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " Usage: adastest [OPTION]...");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " Description:");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " \t sensortype for sensor ADV7480 (HDMI Input) only");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " Example:");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " \t 1.LDWS:");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " \t\t adastest ldwsstart x y [sensortype] period");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " \t\t adastest ldwsstop");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " \t 2.FCWS:");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " \t\t adastest fcwsstart x y night|day [sensortype] period");
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " \t\t adastest fcwsstop");
            }
            #if TEST_ADAS_LDWS
            else if (strcmp(arg[1], "ldwsstart") == 0) {              
                #if SP5K_SENSOR_ADV7480
                if(argc == 6){
		    int nPeriod;		
                    HOST_PROF_LOG_PRINT(LEVEL_INFO," x:%d y:%d sensor type:%d period:%d \n", v[2], v[3], v[4], v[5]);
                    int nSensorType = v[4];
                    HOST_ASSERT_MSG(nSensorType > LDWS_SENSOR_UNSUPPORT && nSensorType < LDWS_SENSOR_TOTAL, "error sensor type value:%d, support sensor type's range: [%d..%d]\n", nSensorType, LDWS_SENSOR_OV4689, LDWS_SENSOR_TOTAL -1);
                    g_nLdwsSensorType = nSensorType;
                    nPeriod = v[5];
                #else
                if(argc == 5){
		    int nPeriod;
                    HOST_PROF_LOG_PRINT(LEVEL_INFO," x:%d y:%d period:%d \n", v[2], v[3], v[4]);
                    decideLdwsSensorType(&g_nLdwsSensorType);
                    nPeriod = v[4];
                #endif
                    int nSensorIndex = g_nLdwsSensorType - 1;
                    appLdwsStart(v[2], v[3], g_LdwsSupportSensor[nSensorIndex].nDistanceToCenterThr, nPeriod);
                }
                else{
                    HOST_PROF_LOG_PRINT(LEVEL_INFO, " adastest: invalid option");
                    HOST_PROF_LOG_PRINT(LEVEL_INFO, " Try 'adastest --help' for more information");
                }
            }
            else if (!strcmp(arg[1], "ldwsstop")) {
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " stop ldws");
                appLdwsStop();
            }
            #endif

            #if TEST_ADAS_FCWS
            else if (strcmp(arg[1], "fcwsstart") == 0) {
                #if SP5K_SENSOR_ADV7480
                if(argc == 7){
                    HOST_PROF_LOG_PRINT(LEVEL_INFO," x:%d y:%d night/day:%d sensor type:%d period:%d \n", v[2], v[3], v[4], v[5], v[6]);
                    int nSensorType = v[5];
                    HOST_ASSERT_MSG(nSensorType > 0 && nSensorType < FCWS_SENSOR_TOTAL, "error sensor type value:%d, support sensor type's range: [%d..%d]\n", nSensorType, FCWS_SENSOR_OV4689, FCWS_SENSOR_TOTAL -1);
                    g_nFcwsSensorType = nSensorType;
                #else
                if(argc == 6){
                    HOST_PROF_LOG_PRINT(LEVEL_INFO," x:%d y:%d night|day:%d period:%d \n", v[2], v[3], v[4], v[5]);
                    decideFcwsSensorType(&g_nFcwsSensorType);
                #endif

                    HOST_ASSERT_MSG(v[4] >= NIGHT && v[4] < TIME_TOTAL, "error night|day value:%d, support range: [%d..%d]\n", v[4], NIGHT, TIME_TOTAL -1);
                    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t use iCatch sensor parameters: 1.focal length 2. pixel unit");
                    switch(g_nFcwsSensorType){

                        case FCWS_SENSOR_OV4689:
                            HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t sensor:FCWS_SENSOR_OV4689");
                            break;
                        case FCWS_SENSOR_AR0330CS:
                            HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t sensor:FCWS_SENSOR_AR0330CS");
                            break;
                        case FCWS_SENSOR_AR0330:
                            HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t sensor:FCWS_SENSOR_AR0330");
                            break;
                        case FCWS_SENSOR_AR0330_SERDES_SPCA7002:
                            HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t sensor:FCWS_SENSOR_AR0330_SERDES_SPCA7002");
                            break;
                        case FCWS_SENSOR_OV4689_SERDES_SPCA7002:
                            HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t sensor:FCWS_SENSOR_OV4689_SERDES_SPCA7002");
                            break;
                        case FCWS_SENSOR_UNSUPPORT:
                            HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t sensor:FCWS_SENSOR_UNSUPPORT");
                            break;
                        default:
                            break;
                    }

                    int nFcwsSensorIndex = g_nFcwsSensorType - 1;

                    #if SP5K_SENSOR_ADV7480
                    g_nFcwsPeriod = v[6];
                    #else
                    g_nFcwsPeriod = v[5];
                    #endif

                    appFcwsStart(v[2], v[3], v[4], g_FcwsSupportSensor[nFcwsSensorIndex].nSensorFocalLen, g_FcwsSupportSensor[nFcwsSensorIndex].nSensorPixelUnit, g_nFcwsPeriod);
                }
                else{
                    HOST_PROF_LOG_PRINT(LEVEL_INFO, " adastest: invalid option");
                    HOST_PROF_LOG_PRINT(LEVEL_INFO, " Try 'adastest --help' for more information");
                }
            }
            else if (!strcmp(arg[1], "fcwsstop")) {
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " stop fcws");
                appFcwsStop();
            }
            #endif
            else{
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " adastest: invalid option '%s'", arg[1]);
                HOST_PROF_LOG_PRINT(LEVEL_INFO, " Try 'adastest --help' for more information");
            }
        }
        else{
            HOST_PROF_LOG_PRINT(LEVEL_INFO, " Try 'adastest --help' for more information");
        }
    }


#if 0
	else if (strcmp(param, "uartr")==0) {
		UINT32 i=0;
		UINT32 *uartR = NULL;
		uartR = malloc(SIZE);
		memset(uartR, 0, SIZE);

		while(1){
			sp5kUartRead(1, uartR+i);
			if(i == SIZE){
				i=0;
				for(i=0; i<=SIZE; i++){
					printf("r=%d..\t", *(uartR+i));
				}
				printf("Read finish\n");
				break;
			}else{
				i++;
			}
		}
	}
	else if (strcmp(param, "uartw")==0) {
		UINT32 i, ret;
		UINT8 *pUartData = NULL;
		pUartData = malloc(SIZE);

		for(i=0; i<=SIZE; i++){
			*(pUartData+i) = i%255;
		}

		i=0;
		while(1){
			sp5kGpioRead(SP5K_GPIO_GRP_FMH, GPIO_MASK(61-32), &ret);	/* RTS check */
			if(!ret){
				sp5kUartWrite(1, pUartData+i, 16);
				if(i == SIZE){
					printf("Write finish\n");
					i=0;
					break;
				}else{
					i+=16;
				}
			}
		}
	}
	else if (strcmp(param, "uartinit")==0) {
		/* 1. Set Uart0 Baud Rate :115200
		     2. 0x4051(UART0 engine enable)
		     3. Set FMGIPIO as function pin: 0x4048 start */
		sp5kUartInit(1, 4194304);		/* UART1: port = 0, UART2: port = 1, UART3: port = 2 */
		sp5kUartModeSet(1, 0);		/* param2: DMA transmit: 1, PIO: 0 */
		WRITE8(0xb000010f, 0xf0);	/* FMIO pull high */
		WRITE8(0xb00042a9, 0x04);	/* UART0 flow Contrl [2]=0 sw flow ctrl, 1 hw flow ctrl; */
	}
#endif
    else{
        #if (!SP5K_CVR_MODE) && (!SP5K_SPC_MODE) /*not support cvr/SPC mode*/
		#if 1
		extern void cmdParamInitEx(UINT8 argc, UINT8 *arg[], UINT32 v[]);
		extern void utility_cmd_dispatch(void);

		cmdParamInitEx(argc, (UINT8 **)arg, v);
		utility_cmd_dispatch();
		#endif
        #endif
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appMonitorCmd
 *  Description :
 *------------------------------------------------------------------------*/
void
appMonitorCmd(
	int argc,
	char *argv[]
)
{
	UINT32 i = 0;
	UINT32 j = 0;
	memset(v, 0, sizeof(v));

    /*parse*/
	for (j = 0; j < argc && j < CMD_ARGC_MAX; j++) {
		v[j] = strtoul(argv[j], NULL, 0);
	}

    /*check cmd function point*/
	void (*pfunc)(UINT32 argc, UINT8 *argv[], UINT32 v[]);
	while(CustomerCmd[i].cmd != NULL)
	{
		if (!strcmp(CustomerCmd[i].cmd, *argv))
		{
			pfunc = CustomerCmd[i].pfunc;
			if (pfunc != NULL)
			{
				pfunc((UINT32)argc, (UINT8 **)argv, v);
				return;
			}
		}
		i++;
	}

    appCustomerCmd(argc,argv, v);
}