/*--------------------------------------------------------------------------*
 *	File name	: app_test_adas_rod.c
 *	Function	: Test ADAS ROD function
 *	First editor: Weikai Hsu
 *	First date	: 
 *	History		: 	
 *	            : 2018.06.01  Weikai Hsu "Added File Header"
 *	
 *	
 *	
 *--------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "app_test_adas_rod.h"
#include "app_test_adas.h"
#include "app_test_adas_api.h"

#include "app_com_api.h"
#include "app_osd_api.h"
#include "sp5k_dbg_api.h"

//#include "debug_mod/dbg_api.h"
#include "app_dbg_api.h"
#if defined(HOST_DBG)
#undef HOST_DBG
#define HOST_DBG 1 
#endif

//deafult ROD src W,H
#define ROD_SRC_BUF_W 1920
#define ROD_SRC_BUF_H 1080

//draw ROD OSD related
#define ROD_OSD_W 320
#define ROD_OSD_H 180
#define ROD_OSD_OFFSET_TOP_H 30
#define LINE_NUMS 3


typedef struct RodCfg{
    UINT8 nInput;
    UINT32 nSrcW;
    UINT32 nSrcH;
    UINT32 nOsdW;
    UINT32 nOsdH;
    UINT32 nOsdOffsetTopH;
    UINT32 nUpH;
    UINT32 nMidH;
    UINT32 nLowH;
}tRodcfg;
/*Removed:never used*/
/*typedef struct RodWarnInfo{
    UINT16 ndisance;
    UINT16 nSec;
    UINT16 nTimes;
}tRodWarnInfo;*/

eADAS_Sound g_ePrevADAS_SOUND;
static UINT32 g_nStartTimePlayRODSound = 0;
static tRodcfg g_RodCfg = {.nInput = SP5K_ADAS_INPUTBUF_CFG_INPUT_MAIN,
                           .nSrcW = ROD_SRC_BUF_W,
                           .nSrcH = ROD_SRC_BUF_H,
                           .nOsdW = ROD_OSD_W,
                           .nOsdH = ROD_OSD_H,
                           .nOsdOffsetTopH = ROD_OSD_OFFSET_TOP_H,
                           .nUpH = 298,
                           .nMidH = 488,
                           .nLowH = 888
                          };

static sp5kAdasRodMap_t g_Rodmap;
static sp5kAdasRodResult_t g_RodResult;

static void init(const unsigned int *pInUpY, const unsigned int *pInMidY, const unsigned int *pInLowY);
static void start();
static void stop();
static void rodResultCallback(UINT32 param);
static void drawRodProductOSD(sp5kGfxPageNum_e eDrawedLayer, const sp5kAdasRodResult_t *pInRodResult);
static void decidePlayWarningSound(const sp5kAdasRodResult_t *pInRodResult);
static void rodWarnProc(const sp5kAdasRodResult_t *pInRodResult);

/*--------------------------------------------------------------------------*
* Function name : init 
* Function		: adas rod init
* Return value	: static void 
* Parameter1	: const unsigned int *pInUpY
* Parameter2	: const unsigned int *pInMidY
* Parameter3	: const unsigned int *pInLowY
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void init(const unsigned int *pInUpY, const unsigned int *pInMidY, const unsigned int *pInLowY){
    HOST_ASSERT_MSG(pInUpY && pInMidY && pInLowY, "please set UpY, MidY and LowY\n");
    /*Removed: Unsigned variable can't be negative so it is unnecessary to test it */
    HOST_ASSERT_MSG(/* *pInUpY >= 0 && */  *pInUpY <= 1024, "UpY:%d ,The UpY's range is [0..1024]\n", *pInUpY);
    HOST_ASSERT_MSG(/* *pInMidY >= 0 && */ *pInMidY <= 1024, "MidY:%d ,The MidY's range is [0..1024]\n", *pInMidY);
    HOST_ASSERT_MSG(/* *pInLowY >= 0 &&*/ *pInLowY <= 1024, "LowY:%d ,The LowY's range is [0..1024]\n", *pInLowY);
    
    //init global data
    memset(&g_Rodmap, 0, sizeof(g_Rodmap));
    memset(&g_RodResult, 0, sizeof(g_RodResult));

    /* Set callback parameter */
    sp5kAdasCfgSet(SP5K_ADAS_CFG_RESULT_CB, SP5K_ADAS_ALGO_ROD, rodResultCallback);

    /* Set rod parameter */
    g_Rodmap.image_width = g_RodCfg.nSrcW;
    g_Rodmap.image_Height = g_RodCfg.nSrcH;
    g_Rodmap.rod_up_coord_y = *pInUpY;
    g_Rodmap.rod_mid_coord_y = *pInMidY;
    g_Rodmap.rod_low_coord_y = *pInLowY;
    sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_ROD, SP5K_ADAS_ROD_MAP_PARAM, &g_Rodmap);

    /* Set algorithm parameters of ADAS */
    sp5kAdasInputBufCfg_t adasCfg;
    memset(&adasCfg,0,sizeof(adasCfg));
    adasCfg.input = g_RodCfg.nInput;
    adasCfg.nSensor = 1;
    adasCfg.sen[0].id = SP5K_ADAS_INPUTBUF_CFG_SENSOR_REAR;         //for ROD algorithm
    adasCfg.sen[0].roi.elem.x = 0;
    adasCfg.sen[0].roi.elem.y = 0;
    adasCfg.sen[0].roi.elem.w = ADAS_COORD_HSIZE-1;
    adasCfg.sen[0].roi.elem.h = ADAS_COORD_VSIZE-1;
    sp5kAdasCfgSet(SP5K_ADAS_CFG_INPUTBUF,&adasCfg);

    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s [%d,%d,%d][%d][%d][%d][%d]", ROD_STR,
            adasCfg.input,
            adasCfg.nSensor,
            adasCfg.sen[0].id,
            adasCfg.sen[0].roi.elem.x,
            adasCfg.sen[0].roi.elem.y,
            adasCfg.sen[0].roi.elem.w,
            adasCfg.sen[0].roi.elem.h);

    //output host rod settings
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "%s parameters", ROD_STR);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t input: %d", g_RodCfg.nInput);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t src(%d,%d)", g_RodCfg.nSrcW, g_RodCfg.nSrcH);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t osd(%d,%d)", g_RodCfg.nOsdW, g_RodCfg.nOsdH);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t osd offset top h:%d", g_RodCfg.nOsdOffsetTopH);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t (upH,midH,lowH)=(%d,%d,%d)", g_RodCfg.nUpH, g_RodCfg.nMidH, g_RodCfg.nLowH);
}
/*--------------------------------------------------------------------------*
* Function name : start 
* Function		: adas rod start
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void start(){
    sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_ENABLE, SP5K_ADAS_ALGO_ROD, 1);
    adasSetRod(TRUE);
    hostAdasStart();
}
/*--------------------------------------------------------------------------*
* Function name : stop 
* Function		: adas rod stop
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void stop(){
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] s", __FUNCTION__);
    sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_ENABLE, SP5K_ADAS_ALGO_ROD, 0);
    adasSetRod(FALSE);
    hostAdasStop();
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] e", __FUNCTION__);
}
/*--------------------------------------------------------------------------*
* Function name : rodResultCallback 
* Function		: rod result callback
* Return value	: static void 
* Parameter1	: UINT32 param
* History	    :
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void rodResultCallback(UINT32 param){
    UINT32 mode;
    sp5kModeGet(&mode);
    HOST_ASSERT_MSG(mode == SP5K_MODE_VIDEO_PREVIEW || mode == SP5K_MODE_VIDEO_PLAYBACK, "[%s]  :ROD must be executed in the video preview mode", __FUNCTION__);

    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] %d", __FUNCTION__, param);
    sp5kAdasResultGet(SP5K_ADAS_CFG_ALGO_ROD, &g_RodResult);
    decidePlayWarningSound(&g_RodResult);
}
/*--------------------------------------------------------------------------*
* Function name : drawRodProductOSD 
* Function		: draw rod product
* Return value	: static void 
* Parameter1	: sp5kGfxPageNum_e eDrawedLayer
* Parameter2	: const sp5kAdasRodResult_t *pInRodResult
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void drawRodProductOSD(sp5kGfxPageNum_e eDrawedLayer, const sp5kAdasRodResult_t *pInRodResult){
    if(!pInRodResult){
        return;
    }
    
    sp5kAdasRodCoord_t prodCoord[LINE_NUMS];
    memcpy(&prodCoord[0], &(pInRodResult->warning_coord[0]), sizeof(pInRodResult->warning_coord));

    /*Clear */
    appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
    appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 80, 100, 100, 0, 0);

    /*------Line Draw-----*/
    sp5kGfxAttrSet(SP5K_GFX_FILL_ENABLE, 1, 0, 0, 0);
    sp5kGfxAttrSet(SP5K_GFX_PEN_ENABLE, 1, 0, 0, 0);
    sp5kGfxAttrSet(SP5K_GFX_PEN_WIDTH, 1, 0, 0, 0);
    UINT16 LX,LY,RX,RY;
    LX = LY = RX = RY = 0;

    int i = 0;
    for(i = 0; i < LINE_NUMS; ++i){
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] ori xy %d--L:[%d %d] R[%d %d]\n", __FUNCTION__ , i, prodCoord[i].Lx, prodCoord[i].Ly, prodCoord[i].Rx, prodCoord[i].Ry);
        //fixme
        LX = (prodCoord[i].Lx * g_RodCfg.nOsdW) DIVISION_ADAS_ALGO_W;
        LY = ((prodCoord[i].Ly * g_RodCfg.nOsdH) DIVISION_ADAS_ALGO_H) + g_RodCfg.nOsdOffsetTopH;
        RX = (prodCoord[i].Rx * g_RodCfg.nOsdW) DIVISION_ADAS_ALGO_W;;
        RY = ((prodCoord[i].Ry * g_RodCfg.nOsdH) DIVISION_ADAS_ALGO_H) + g_RodCfg.nOsdOffsetTopH;

        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] %d--ori L:[%d %d] R[%d %d]\n", __FUNCTION__, i, prodCoord[i].Lx, prodCoord[i].Ly, prodCoord[i].Rx, prodCoord[i].Ry);
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] %d--L:[%d %d] R[%d %d]\n", __FUNCTION__, i, LX, LY, RX, RY);
        if (LX > 0 && RX < g_RodCfg.nOsdW && LY > g_RodCfg.nOsdOffsetTopH && LY < (g_RodCfg.nOsdH + g_RodCfg.nOsdOffsetTopH) && RY > g_RodCfg.nOsdOffsetTopH && RY < (g_RodCfg.nOsdH + g_RodCfg.nOsdOffsetTopH)){
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_YELLOW_1, 0, 0, 0);
            sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, PAL_YELLOW_1, 0, 0, 0);
            sp5kGfxLineDraw(eDrawedLayer, LX, LY, RX, RY);
        }
    }

    /*------Warning Line Draw-----*/
    UINT16 W_LX, W_LY, W_RX, W_RY;
    W_LX = W_LY = W_RX = W_RY = 0;

    int nStopIndex = 0;
    if(pInRodResult->warning_flag_far){
        nStopIndex = 1;
    }
    else if(pInRodResult->warning_flag_near){
        nStopIndex = 0;
    }
    else{
        nStopIndex = LINE_NUMS;
    }

    if(pInRodResult->warning_flag_far || pInRodResult->warning_flag_near){
        for(i = LINE_NUMS - 1 ; i >= nStopIndex; i--){
            W_LX = (prodCoord[i].Lx * g_RodCfg.nOsdW) DIVISION_ADAS_ALGO_W;
            W_LY = ((prodCoord[i].Ly * g_RodCfg.nOsdH) DIVISION_ADAS_ALGO_H) + g_RodCfg.nOsdOffsetTopH;
            W_RX = (prodCoord[i].Rx * g_RodCfg.nOsdW) DIVISION_ADAS_ALGO_W;
            W_RY = ((prodCoord[i].Ry * g_RodCfg.nOsdH) DIVISION_ADAS_ALGO_H) + g_RodCfg.nOsdOffsetTopH;

            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] %d--Warning ori L:[%d %d] R[%d %d]\n", __FUNCTION__, i, prodCoord[i].Lx, prodCoord[i].Ly, prodCoord[i].Rx, prodCoord[i].Ry);
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] %d--Warning L:[%d %d] R[%d %d]\n", __FUNCTION__, i, W_LX, W_LY, W_RX, W_RY);
            if (W_LX > 0 && W_RX < g_RodCfg.nOsdW && W_LY > g_RodCfg.nOsdOffsetTopH && W_LY < (g_RodCfg.nOsdH + g_RodCfg.nOsdOffsetTopH) && W_RY > g_RodCfg.nOsdOffsetTopH && W_RY < (g_RodCfg.nOsdH + g_RodCfg.nOsdOffsetTopH)){
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_RED, 0, 0, 0);
                sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, PAL_RED, 0, 0, 0);
                sp5kGfxLineDraw(eDrawedLayer, W_LX, W_LY, W_RX, W_RY);
            }
        }
    }
}
/*--------------------------------------------------------------------------*
* Function name : decidePlayWarningSound 
* Function		: warning sound
* Return value	: static void 
* Parameter1	: const sp5kAdasRodResult_t *pInRodResult
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void decidePlayWarningSound(const sp5kAdasRodResult_t *pInRodResult){
    rodWarnProc(pInRodResult);

}
/*--------------------------------------------------------------------------*
* Function name : rodWarnProc 
* Function		: rod warn process
* Return value	: static void 
* Parameter1	: const sp5kAdasRodResult_t *pInRodResult
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void rodWarnProc(const sp5kAdasRodResult_t *pInRodResult){
    if(pInRodResult->warning_flag_far == 1){
        g_eADAS_SOUND = E_ADAS_SOUND_ROD_BEEP_1TIMES_PERSEC;
    }
    else if(pInRodResult->warning_flag_near == 1){
        g_eADAS_SOUND = E_ADAS_SOUND_ROD_BEEP_2TIMES_PERSEC;
    }
    else{
        g_eADAS_SOUND = E_ADAS_SOUND_UNDEFINED;
    }

#define WARN_CHECK_TIME_UNIT 1       //1: 1sec
#define WARN_CHECK_GAP_TIME 100      //unit:1sec
    static BOOL bFirstBeep = TRUE;
    static UINT32 nGapRodTime = 0; 

    if(bFirstBeep){
        bFirstBeep = FALSE;
        g_ePrevADAS_SOUND = g_eADAS_SOUND;
        sp5kOsEventFlagsSet(&g_EventAdasStartPlayAudio, 1, TX_OR);
        g_nStartTimePlayRODSound = sp5kOsTimeGet();
    }
    else{
        if(g_eADAS_SOUND == g_ePrevADAS_SOUND){
            nGapRodTime = sp5kOsTimeGet() - g_nStartTimePlayRODSound;
            if(nGapRodTime >= (WARN_CHECK_TIME_UNIT * WARN_CHECK_GAP_TIME)){
                g_ePrevADAS_SOUND = g_eADAS_SOUND;
                sp5kOsEventFlagsSet(&g_EventAdasStartPlayAudio, 1, TX_OR);
                g_nStartTimePlayRODSound = sp5kOsTimeGet();
            }
        }
        else{
            nGapRodTime = sp5kOsTimeGet() - g_nStartTimePlayRODSound;
            if(nGapRodTime >= (WARN_CHECK_TIME_UNIT * WARN_CHECK_GAP_TIME)){
                bFirstBeep = TRUE;
                g_ePrevADAS_SOUND = E_ADAS_SOUND_UNDEFINED;
            }
        }
    }
}
/*--------------------------------------------------------------------------*
* Function name : appRodStart 
* Function		: rod start
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appRodStart(){
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] s", __FUNCTION__);
    //Only run in the video preview mdoe
    UINT32 mode;
    sp5kModeGet(&mode);
    HOST_ASSERT_MSG(mode == SP5K_MODE_VIDEO_PREVIEW  || mode == SP5K_MODE_VIDEO_PLAYBACK, "[%s]  :ROD must be started in the video preview mode", __FUNCTION__);

    appRodCfgSet(ROD_CFG_UP_H, g_RodCfg.nUpH);
    appRodCfgSet(ROD_CFG_MID_H, g_RodCfg.nMidH);
    appRodCfgSet(ROD_CFG_LOW_H, g_RodCfg.nLowH);
    
    init(&g_RodCfg.nUpH, &g_RodCfg.nMidH, &g_RodCfg.nLowH);
    start();
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] e", __FUNCTION__);
}
/*--------------------------------------------------------------------------*
* Function name : appRodStop 
* Function		: rod stop
* Return value	: void 
* Parameter1	: void
* History	    :
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appRodStop(){
    stop();
}
/*--------------------------------------------------------------------------*
* Function name : appRodCfgSet 
* Function		: rod config set
* Return value	: void 
* Parameter1	: eRODCfg eCfgOption
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appRodCfgSet(eRODCfg eCfgOption, ...){
    va_list args;
    va_start(args, eCfgOption);
    UINT32 nValue = va_arg(args, UINT32);

    switch(eCfgOption){
        case ROD_CFG_INPUT_MAIN:
            g_RodCfg.nInput = SP5K_ADAS_INPUTBUF_CFG_INPUT_MAIN;        //front
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] set input main %d", __FUNCTION__, g_RodCfg.nInput);
            break;
        case ROD_CFG_INPUT_SEC:
            g_RodCfg.nInput = SP5K_ADAS_INPUTBUF_CFG_INPUT_SEC;         //rear
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] set input sec %d", __FUNCTION__, g_RodCfg.nInput);
            break;
        case ROD_CFG_INPUT_PBVDO:
            g_RodCfg.nInput = SP5K_ADAS_INPUTBUF_CFG_INPUT_PBVDO;       //playback video
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] set input pbvdo %d", __FUNCTION__, g_RodCfg.nInput);
            break;
        case ROD_CFG_SRC_W:
            g_RodCfg.nSrcW = nValue;
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] set src w:%d", __FUNCTION__, g_RodCfg.nSrcW);
            break;
        case ROD_CFG_SRC_H:
            g_RodCfg.nSrcH = nValue;
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] set src h:%d", __FUNCTION__, g_RodCfg.nSrcH);
            break;
        case ROD_CFG_OSD_W:
            g_RodCfg.nOsdW = nValue;
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] set osd w:%d", __FUNCTION__, g_RodCfg.nOsdW);
            break;
        case ROD_CFG_OSD_H:
            g_RodCfg.nOsdH = nValue;
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] set osd h:%d", __FUNCTION__, g_RodCfg.nOsdH);
            break;
        case ROD_CFG_OSD_OFFSET_TOP_H:
            g_RodCfg.nOsdOffsetTopH = nValue;
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] set osd offset top h:%d", __FUNCTION__, g_RodCfg.nOsdOffsetTopH);
            break;
        case ROD_CFG_UP_H:
            g_RodCfg.nUpH = nValue;
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] set up h:%d", __FUNCTION__, g_RodCfg.nUpH);
            break;
        case ROD_CFG_MID_H:
            g_RodCfg.nMidH = nValue;
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] set mid h:%d", __FUNCTION__, g_RodCfg.nMidH);
            break;
        case ROD_CFG_LOW_H:
            g_RodCfg.nLowH = nValue;
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] set low h:%d", __FUNCTION__, g_RodCfg.nLowH);
            break;
        default:
            HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s] command parameter is not support", __FUNCTION__);
            break;
    }
    va_end(args);  
}

//Product OSD
/*--------------------------------------------------------------------------*
* Function name : appRodDrawProductOSDResult 
* Function		: rod draw product osd result
* Return value	: void 
* Parameter1	: sp5kGfxPageNum_e eDrawedLayer
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appRodDrawProductOSDResult(sp5kGfxPageNum_e eDrawedLayer){
    drawRodProductOSD(eDrawedLayer, &g_RodResult);
}

//Debug
/*--------------------------------------------------------------------------*
* Function name : debugRodDrawResult 
* Function		: rod draw result for debug
* Return value	: void 
* Parameter1	: sp5kGfxPageNum_e eDrawedLayer
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void debugRodDrawResult(sp5kGfxPageNum_e eDrawedLayer){
    drawRodProductOSD(eDrawedLayer, &g_RodResult);
}
/*--------------------------------------------------------------------------*
* Function name : debugRodDrawResultToRecord 
* Function		: rod draw result to record
* Return value	: void 
* Parameter1	: frameBufInfo_t *pframe
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void debugRodDrawResultToRecord(frameBufInfo_t *pframe){

}
/*--------------------------------------------------------------------------*
* Function name : demoRodDrawResultToRecord 
* Function		: demo rod draw result to record
* Return value	: void 
* Parameter1	: frameBufInfo_t *pframe
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
//Demo
void demoRodDrawResultToRecord(frameBufInfo_t *pframe){

}

