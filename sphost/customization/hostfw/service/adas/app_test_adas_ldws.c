/*--------------------------------------------------------------------------*
 *	File name	: app_test_adas_ldws.c
 *	Function	: Test ADAS LDWS function
 *	First editor: Weikai Hsu
 *	First date	: 
 *	History		: 	
 *	            : 2018.06.01  Weikai Hsu "Added File Header"
 *	
 *	
 *	
 *--------------------------------------------------------------------------*/
#include "app_test_adas_ldws.h"
#include "app_test_adas.h"

#include "app_test_adas_api.h"
#include "app_i2c_lib_icatch.h"
#include "app_dbg_api.h"
#include "app_pal.h"
#include "app_osd_api.h"
#include "app_com_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_media_api.h"
#include "app_dbg_api.h"
#include "sp5k_adas_api.h"
#include "sp5k_dbg_api.h"

#include <stdio.h>
#include <string.h>
#if defined(HOST_DBG)
#undef HOST_DBG
#define HOST_DBG 0
#endif

#define LDWS_PERFORMANCE 0 
#define LDWS_PROCESS_TIME_PER_CALLBACK 0

typedef enum{
    E_ADAS_LDWS_WARN_DRAW_NO = 0,
    E_ADAS_LDWS_WARN_DRAW_LEFT,
    E_ADAS_LDWS_WARN_DRAW_RIGHT,
    E_ADAS_LDWS_WARN_DRAW_BOTH,
    E_ADAS_LDWS_WARN_DRAW_TOTAL
}eADAS_LDWS_WARN_TYPE;

typedef enum{
    E_ADAS_LDWS_WARN_SOUND_NO = 0, 
    E_ADAS_LDWS_WARN_SOUND_LEFT,
    E_ADAS_LDWS_WARN_SOUND_RIGHT,
    E_ADAS_LDWS_WARN_SOUND_TOTAL
}eADAS_LDWS_WARN_SOUND_TYPE;

typedef enum{
    E_ADAS_LDWS_WARN_STRATEGY_NORMAL = 0,
    E_ADAS_LDWS_WARN_STRATEGY_DEPARTURE_SECS,
    E_ADAS_DLWS_WARN_STRATEGY_UNDIFINED
}eADAS_LDWS_WARN_STRATEGY_TYPE;

typedef struct WarnPerSecsInfo{
    UINT16 nTriggerCnt;
    UINT16 nFrameCnt;
    UINT16 nConditionOverCnt;
}tWarnPerSecsInfo;

typedef struct WarnStrategySecs{
    UINT8 bFirstTrigger;
    UINT8 nOverConditionSecs;
    UINT32 nStarTime;
    UINT32 nEndTime;
    UINT32 nPreserveWarnType;
    UINT8 nSatisfiedSecs;
    tWarnPerSecsInfo *pWarnPerSecsInfo;
}tWarnStragetySecs;

//prototype
static void init(const int* pInCenterX, const int* pInCenterY, ...);
static void start();
static void stop();
static void warnOverSecsProc(UINT32 *pCurWarnType, UINT8 nSecs);
static void warnOverSecInit(tWarnStragetySecs* pInOutKeepSec, UINT8* pSecs);
static void warnOverSecRun(tWarnStragetySecs* pInOutKeepSec, UINT32 *pInCurWarnType);
static void decidePlayWarningSound(const UINT8 pWarnStrategyType, const UINT32 *pInWarnFlag);
static void drawProductOSDLane(sp5kGfxPageNum_e eDrawedLayer);
static void drawLane(sp5kGfxPageNum_e eDrawedLayer, const sp5kAdasLdwsResult_t* pInLdwsResult);
static void drawDetectingArea(sp5kGfxPageNum_e eDrawedLayer, const int* pInCenterY);
static void ldwsResultCallback();
//ref. app_adas_ldws.c 
sp5kAdasLdwsResult_t g_LdwsResult;
static sp5kAdasLdwsResult_t g_LdwsResultCoverted;
static UINT32 g_LdwsWarnDrawFlag = 0;      //LdwsWarnFlage & 0xF
static UINT8 g_ldwsLaneDraw = 1;
static int g_nLdwsCenterX = LDWS_ADJUST_ALGO_CENTER_X;
static int g_nLdwsCenterY = LDWS_ADJUST_ALGO_CENTER_Y;
static tWarnStragetySecs g_stWarnKeepSec = {FALSE, 5, 0, 0, 0, 0, 0};

/*default parameter, the customer must decide it by themselves
    1. distanceToCenterThr
    2. period
*/
static int g_nDistanceToCenterThr = 32; 
static int g_nLdwsPeriod = 1;

//ref. app_adas_ldws.c function
static void ldwsProcess(UINT32 param);
static void ldwsCoordTranslate(const sp5kAdasLdwsResult_t* pInLdwsResult, sp5kAdasLdwsResult_t* pOutLdwsResult);

extern void drawLdwsStampOnLayerYUV();
extern void blendLdwsStampToYUV(frameBufInfo_t *pInOutFrame);
/*--------------------------------------------------------------------------*
* Function name : appLdwsStart 
* Function		: ldws function start
* Return value	: void 
* Parameter1	: const int nCenterX
* Parameter2	: const int nCenterY
* Parameter3	: C - Variable Arguments
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appLdwsStart(const int nCenterX, const int nCenterY, ...){
#if LDWS_PERFORMANCE
    //_dbg_proc_ps_en(3);
    _dbg_proc_ps_en(2);
#endif
    g_nLdwsCenterX = nCenterX;
    g_nLdwsCenterY = nCenterY;
    
    va_list args;
    va_start(args, nCenterY);

    g_nDistanceToCenterThr = va_arg(args, int); 
    g_nLdwsPeriod = va_arg(args, int);

    init(&g_nLdwsCenterX, &g_nLdwsCenterY, g_nDistanceToCenterThr, g_nLdwsPeriod);
    start();
    va_end(args);  
}
/*--------------------------------------------------------------------------*
* Function name : appLdwsStop 
* Function		: ldws function stop
* Return value	: void 
* Parameter1	: void
* History	    :
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appLdwsStop(){
    stop();

#if LDWS_PERFORMANCE
    _dbg_proc_ps_dump(0);
#endif
}
/*--------------------------------------------------------------------------*
* Function name : appLdwsDumpSetting 
* Function		: draw dump setting
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appLdwsDumpSetting(){
    sp5kAdasLdwsCfg_t ldws_cfg_get;
    sp5kAdasCfgGet(SP5K_ADAS_CFG_ALGO_LDWS, &ldws_cfg_get);

    HOST_PROF_LOG_PRINT(LEVEL_INFO, "%s Setting", LDWS_STR);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t angle_thr:%d", ldws_cfg_get.angle_thr);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t distance2center_thr:%d", ldws_cfg_get.distance2center_thr);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t center point(%d,%d)", ldws_cfg_get.center_x, ldws_cfg_get.center_y);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t img_scale_factor:%d", ldws_cfg_get.img_scale_factor);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t warn_lock_time:%d", ldws_cfg_get.warn_lock_time);
}
/*--------------------------------------------------------------------------*
* Function name : appLdwsDrawProductOSDResult 
* Function		: ldws draw product osd result
* Return value	: void 
* Parameter1	: sp5kGfxPageNum_e eDrawedLayer
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appLdwsDrawProductOSDResult(sp5kGfxPageNum_e eDrawedLayer){
    drawProductOSDLane(eDrawedLayer);
}
/*--------------------------------------------------------------------------*
* Function name : debugLdwsDrawResult 
* Function		: ldws draw result for debug
* Return value	: void 
* Parameter1	: sp5kGfxPageNum_e eDrawedLayer
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void debugLdwsDrawResult(sp5kGfxPageNum_e eDrawedLayer){
    drawLane(eDrawedLayer, &g_LdwsResultCoverted);
    drawDetectingArea(eDrawedLayer, &g_nLdwsCenterY);

    #if 0
    char buf[16] = {};
    static int i = 0; 
    ++i;
    sprintf(buf, "LDWS:%d", i);
    sp5kGfxStringDraw(eDrawedLayer, 0, 50, SP5K_GFX_ALIGN_LEFT, SP5K_GFX_ENCODE_ASCII, (UINT8*)buf);
    #endif
}
/*--------------------------------------------------------------------------*
* Function name : debugLdwsDrawResultToRecord 
* Function		: ldws draw result to record
* Return value	: void 
* Parameter1	: frameBufInfo_t *pframe
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void debugLdwsDrawResultToRecord(frameBufInfo_t *pframe){
    if(!pframe){
        return;
    }

    //base on algo team LDWS v2.17
    if ((g_LdwsResultCoverted.info.ftx1 == 0 && g_LdwsResultCoverted.info.ftx2 == 0 && g_LdwsResultCoverted.info.fty1 == 0 && g_LdwsResultCoverted.info.fty2 == 0) 
        || (g_LdwsResultCoverted.info.ftx3 == 0 && g_LdwsResultCoverted.info.ftx4 == 0 && g_LdwsResultCoverted.info.fty3 == 0 && g_LdwsResultCoverted.info.fty4 == 0)){
        g_ldwsLaneDraw = 0;
    }
    else{
        g_ldwsLaneDraw = 1;
    }

    static sp5kAdasLdwsResult_t ldwsInfors;

    //pframe must be 16:9 based on algo 16:9
    ldwsInfors.info.ftx1 = (g_LdwsResult.info.ftx1 * pframe->width) DIVISION_ADAS_ALGO_W;
    ldwsInfors.info.ftx2 = (g_LdwsResult.info.ftx2 * pframe->width) DIVISION_ADAS_ALGO_W;
    ldwsInfors.info.ftx3 = (g_LdwsResult.info.ftx3 * pframe->width) DIVISION_ADAS_ALGO_W;
    ldwsInfors.info.ftx4 = (g_LdwsResult.info.ftx4 * pframe->width) DIVISION_ADAS_ALGO_W;
    
    ldwsInfors.info.fty1 = (g_LdwsResult.info.fty1 * pframe->height) DIVISION_ADAS_ALGO_H;
    ldwsInfors.info.fty2 = (g_LdwsResult.info.fty2 * pframe->height) DIVISION_ADAS_ALGO_H;
    ldwsInfors.info.fty3 = (g_LdwsResult.info.fty3 * pframe->height) DIVISION_ADAS_ALGO_H;
    ldwsInfors.info.fty4 = (g_LdwsResult.info.fty4 * pframe->height) DIVISION_ADAS_ALGO_H;
    
    /* draw lane line in the video */
    #define LDWS_LANE_WIDTH 5
    if(g_ldwsLaneDraw){ 
        switch(g_LdwsWarnDrawFlag){
            case E_ADAS_LDWS_WARN_DRAW_NO:
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx1, ldwsInfors.info.fty1, ldwsInfors.info.ftx2, ldwsInfors.info.fty2, YUV_COLOR_YELLOW, LDWS_LANE_WIDTH);
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx3, ldwsInfors.info.fty3, ldwsInfors.info.ftx4, ldwsInfors.info.fty4, YUV_COLOR_YELLOW, LDWS_LANE_WIDTH);
                break;
            case E_ADAS_LDWS_WARN_DRAW_LEFT:
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx1, ldwsInfors.info.fty1, ldwsInfors.info.ftx2, ldwsInfors.info.fty2, YUV_COLOR_RED, LDWS_LANE_WIDTH);
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx3, ldwsInfors.info.fty3, ldwsInfors.info.ftx4, ldwsInfors.info.fty4, YUV_COLOR_YELLOW, LDWS_LANE_WIDTH);
                break;
            case E_ADAS_LDWS_WARN_DRAW_RIGHT:
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx1, ldwsInfors.info.fty1, ldwsInfors.info.ftx2, ldwsInfors.info.fty2, YUV_COLOR_YELLOW, LDWS_LANE_WIDTH);
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx3, ldwsInfors.info.fty3, ldwsInfors.info.ftx4, ldwsInfors.info.fty4, YUV_COLOR_RED, LDWS_LANE_WIDTH);
                break;
            case E_ADAS_LDWS_WARN_DRAW_BOTH:
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx1, ldwsInfors.info.fty1, ldwsInfors.info.ftx2, ldwsInfors.info.fty2, YUV_COLOR_RED, LDWS_LANE_WIDTH);
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx3, ldwsInfors.info.fty3, ldwsInfors.info.ftx4, ldwsInfors.info.fty4, YUV_COLOR_RED, LDWS_LANE_WIDTH);
                break;
            default:
                break;
        }
    }
}

/*--------------------------------------------------------------------------*
* Function name : demoLdwsDrawResultToRecord 
* Function		: demo ldws draw result to record
* Return value	: void 
* Parameter1	: frameBufInfo_t *pframe
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void demoLdwsDrawResultToRecord(frameBufInfo_t *pframe){
    if(!pframe){
        return;
    }

    //base on algo team LDWS v2.17
    if ((g_LdwsResultCoverted.info.ftx1 == 0 && g_LdwsResultCoverted.info.ftx2 == 0 && g_LdwsResultCoverted.info.fty1 == 0 && g_LdwsResultCoverted.info.fty2 == 0) 
        || (g_LdwsResultCoverted.info.ftx3 == 0 && g_LdwsResultCoverted.info.ftx4 == 0 && g_LdwsResultCoverted.info.fty3 == 0 && g_LdwsResultCoverted.info.fty4 == 0)){
        g_ldwsLaneDraw = 0;
    }
    else{
        g_ldwsLaneDraw = 1;
    }

    static sp5kAdasLdwsResult_t ldwsInfors;

    //pframe must be 16:9 based on algo 16:9
    ldwsInfors.info.ftx1 = (g_LdwsResult.info.ftx1 * pframe->width) DIVISION_ADAS_ALGO_W;
    ldwsInfors.info.ftx2 = (g_LdwsResult.info.ftx2 * pframe->width) DIVISION_ADAS_ALGO_W;
    ldwsInfors.info.ftx3 = (g_LdwsResult.info.ftx3 * pframe->width) DIVISION_ADAS_ALGO_W;
    ldwsInfors.info.ftx4 = (g_LdwsResult.info.ftx4 * pframe->width) DIVISION_ADAS_ALGO_W;
    
    ldwsInfors.info.fty1 = (g_LdwsResult.info.fty1 * pframe->height) DIVISION_ADAS_ALGO_H;
    ldwsInfors.info.fty2 = (g_LdwsResult.info.fty2 * pframe->height) DIVISION_ADAS_ALGO_H;
    ldwsInfors.info.fty3 = (g_LdwsResult.info.fty3 * pframe->height) DIVISION_ADAS_ALGO_H;
    ldwsInfors.info.fty4 = (g_LdwsResult.info.fty4 * pframe->height) DIVISION_ADAS_ALGO_H;
            
    /* draw lane line in the video */
    #define LDWS_LANE_WIDTH 5
    if(g_ldwsLaneDraw){
        switch(g_LdwsWarnDrawFlag){
            case E_ADAS_LDWS_WARN_DRAW_NO:
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx1, ldwsInfors.info.fty1, ldwsInfors.info.ftx2, ldwsInfors.info.fty2, YUV_COLOR_YELLOW, LDWS_LANE_WIDTH);
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx3, ldwsInfors.info.fty3, ldwsInfors.info.ftx4, ldwsInfors.info.fty4, YUV_COLOR_YELLOW, LDWS_LANE_WIDTH);
                break;
            case E_ADAS_LDWS_WARN_DRAW_LEFT:
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx1, ldwsInfors.info.fty1, ldwsInfors.info.ftx2, ldwsInfors.info.fty2, YUV_COLOR_RED, LDWS_LANE_WIDTH);
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx3, ldwsInfors.info.fty3, ldwsInfors.info.ftx4, ldwsInfors.info.fty4, YUV_COLOR_YELLOW, LDWS_LANE_WIDTH);
                drawLdwsStampOnLayerYUV();
                blendLdwsStampToYUV(pframe);
                break;
            case E_ADAS_LDWS_WARN_DRAW_RIGHT:
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx1, ldwsInfors.info.fty1, ldwsInfors.info.ftx2, ldwsInfors.info.fty2, YUV_COLOR_YELLOW, LDWS_LANE_WIDTH);
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx3, ldwsInfors.info.fty3, ldwsInfors.info.ftx4, ldwsInfors.info.fty4, YUV_COLOR_RED, LDWS_LANE_WIDTH);
                drawLdwsStampOnLayerYUV();
                blendLdwsStampToYUV(pframe);
                break;
            case E_ADAS_LDWS_WARN_DRAW_BOTH:
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx1, ldwsInfors.info.fty1, ldwsInfors.info.ftx2, ldwsInfors.info.fty2, YUV_COLOR_RED, LDWS_LANE_WIDTH);
                sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, ldwsInfors.info.ftx3, ldwsInfors.info.fty3, ldwsInfors.info.ftx4, ldwsInfors.info.fty4, YUV_COLOR_RED, LDWS_LANE_WIDTH);
                drawLdwsStampOnLayerYUV();
                blendLdwsStampToYUV(pframe);
                break;
            default:
                break;
        }
    }
}
/*--------------------------------------------------------------------------*
* Function name : init 
* Function		: ldws function init
* Return value	: static void 
* Parameter1	: const int* pInCenterX
* Parameter2	: const int* pInCenterY
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void init(const int* pInCenterX, const int* pInCenterY, ...){
    //init global data
    memset(&g_LdwsResult, 0, sizeof(g_LdwsResult));
    memset(&g_LdwsResultCoverted, 0, sizeof(g_LdwsResultCoverted));

    /* 1. Set enable parameter */ 
    //sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_ENABLE,SP5K_ADAS_ALGO_LDWS,1);  //move to start()

    /* 2. Set callback parameter */
    sp5kAdasCfgSet(SP5K_ADAS_CFG_RESULT_CB,SP5K_ADAS_ALGO_LDWS, ldwsResultCallback);

    va_list args;
    va_start(args, *pInCenterY);
    g_nDistanceToCenterThr = va_arg(args, int);

    /* 3. Set warning and angle threshold parameter */
    sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_LDWS,SP5K_ADAS_LDWS_THRESHOLD_SET, LDWS_ANGLE_THR, g_nDistanceToCenterThr); 

    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s %d", LDWS_STR,*pInCenterY);
    /* 4. Set center point */
    sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_LDWS,SP5K_ADAS_LDWS_CENTRAL_POINT_SET, *pInCenterX, *pInCenterY); 
    /* 5. Set image scale factor base on sensor mode */
    sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_LDWS, SP5K_ADAS_LDWS_IMAGE_SCALE_FACTOR_SET, 1000);
    /* 6. Set Warnning interval*/
    sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_LDWS, SP5K_ADAS_LDWS_WARN_MSG_INTERVAL, LDWS_SOUND_WARNING_INTERVAL);  /* uint :ms*/

    /* 7.Set LDWS Period */
    g_nLdwsPeriod = va_arg(args, int);
	sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_PERIOD, SP5K_ADAS_ALGO_LDWS, g_nLdwsPeriod);

    /* Set algorithm parameters of ADAS */
    sp5kAdasInputBufCfg_t adasCfg;
    memset(&adasCfg,0,sizeof(adasCfg));
    adasCfg.input = SP5K_ADAS_INPUTBUF_CFG_INPUT_MAIN;
    adasCfg.nSensor = 1;
    adasCfg.sen[0].id = SP5K_ADAS_INPUTBUF_CFG_SENSOR_FRONT;
    adasCfg.sen[0].roi.elem.x = 0;
    adasCfg.sen[0].roi.elem.y = 0;
    adasCfg.sen[0].roi.elem.w = ADAS_COORD_HSIZE-1;
    adasCfg.sen[0].roi.elem.h = ADAS_COORD_VSIZE-1;
    sp5kAdasCfgSet(SP5K_ADAS_CFG_INPUTBUF,&adasCfg);


    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s [%d,%d,%d][%d][%d][%d][%d]", LDWS_STR,
                                                                    adasCfg.input,
                                                                    adasCfg.nSensor,
                                                                    adasCfg.sen[0].id,
                                                                    adasCfg.sen[0].roi.elem.x,
                                                                    adasCfg.sen[0].roi.elem.y,
                                                                    adasCfg.sen[0].roi.elem.w,
                                                                    adasCfg.sen[0].roi.elem.h);

    //output ldws parameter info
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "%s parameters", LDWS_STR);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t center point (%d,%d)", *pInCenterX, *pInCenterY);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t distance to center thr:%d", g_nDistanceToCenterThr);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t angle thr:%d", LDWS_ANGLE_THR);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t period:%d", g_nLdwsPeriod);

    va_end(args);  
}

//if other adas feature use the same task, move out to ADAS.c part
/*--------------------------------------------------------------------------*
* Function name : start 
* Function		: ldws function start
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void start(){
    sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_ENABLE,SP5K_ADAS_ALGO_LDWS,1);
    adasSetLdws(TRUE);
    hostAdasStart();
}
/*--------------------------------------------------------------------------*
* Function name : stop 
* Function		: ldws function stop
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void stop(){
    sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_ENABLE,SP5K_ADAS_ALGO_LDWS,0);
    sp5kFree(g_stWarnKeepSec.pWarnPerSecsInfo);
    g_stWarnKeepSec.pWarnPerSecsInfo = 0;    
    adasSetLdws(FALSE);
    hostAdasStop();
}
/*--------------------------------------------------------------------------*
* Function name : ldwsResultCallback 
* Function		: ldws result callback
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void ldwsResultCallback(){
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s]", __FUNCTION__);
#if LDWS_PROCESS_TIME_PER_CALLBACK
    static UINT32 nSumCount = 100;
    static UINT32 nCurrent = 0;
    static UINT32 timeStart = 0;
    static UINT32 timeEnd = 0;
    static UINT32 timeGap = 0;
    static UINT32 timeSum = 0;
    static BOOL bFirstTime = TRUE;
    timeStart = tmrMsTimeGet();

    if(!bFirstTime){
        timeGap = timeStart - timeEnd;
        timeSum += timeGap;
        ++nCurrent;
    }
#endif

    sp5kAdasResultGet(SP5K_ADAS_CFG_ALGO_LDWS, &g_LdwsResult);
    ldwsProcess(g_LdwsResult.warn_flag);

    //verify distance_center_to_lane_l, distance_center_to_lane_r
    //printf("%s disL:%d disR:%d\n" , __FUNCTION__, g_LdwsResult.distance_center_to_lane_l, g_LdwsResult.distance_center_to_lane_r);

#if LDWS_PROCESS_TIME_PER_CALLBACK
    timeEnd = tmrMsTimeGet();
    bFirstTime = FALSE;
    if(nCurrent == nSumCount){
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "\n ============ LDWS Process average time per %d times ResultCallback ===========\n" , nSumCount);
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "LDWS av process time:   \t %d ms\n", ( timeSum / nSumCount) );
        nCurrent = 0;
        timeSum = 0;
        bFirstTime = TRUE;
    }
#endif
}

//ref app_adas_ldws.c 's appLdwsProc
/*--------------------------------------------------------------------------*
* Function name : ldwsProcess 
* Function		: ldws process
* Return value	: static void 
* Parameter1	: UINT32 param
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void ldwsProcess(UINT32 param){
    ldwsCoordTranslate(&g_LdwsResult, &g_LdwsResultCoverted);
    g_LdwsWarnDrawFlag = param & 0x0F;
    decidePlayWarningSound(E_ADAS_LDWS_WARN_STRATEGY_DEPARTURE_SECS, &param);
}
/*--------------------------------------------------------------------------*
* Function name : ldwsCoordTranslate 
* Function		: ldws process
* Return value	: static void 
* Parameter1	: const sp5kAdasLdwsResult_t* pInLdwsResult
* Parameter2	: sp5kAdasLdwsResult_t* pOutLdwsResult
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void ldwsCoordTranslate(const sp5kAdasLdwsResult_t* pInLdwsResult, sp5kAdasLdwsResult_t* pOutLdwsResult){
    if(0 == pInLdwsResult || 0 == pOutLdwsResult){
        return;
    }

    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s (ftx1,fty2)(%d,%d) (ftx2,fty2)(%d,%d) (ftx3,fty3)(%d,%d) (ftx4,fty4)(%d,%d)- before covert", LDWS_STR, 
                                        pInLdwsResult->info.ftx1, pInLdwsResult->info.fty1,
                                        pInLdwsResult->info.ftx2, pInLdwsResult->info.fty2,
                                        pInLdwsResult->info.ftx3, pInLdwsResult->info.fty3,
                                        pInLdwsResult->info.ftx4, pInLdwsResult->info.fty4);

    pOutLdwsResult->info.ftx1 = (pInLdwsResult->info.ftx1 * ADAS_OSD_W) DIVISION_ADAS_ALGO_W;
    pOutLdwsResult->info.ftx2 = (pInLdwsResult->info.ftx2 * ADAS_OSD_W) DIVISION_ADAS_ALGO_W;
    pOutLdwsResult->info.ftx3 = (pInLdwsResult->info.ftx3 * ADAS_OSD_W) DIVISION_ADAS_ALGO_W;
    pOutLdwsResult->info.ftx4 = (pInLdwsResult->info.ftx4 * ADAS_OSD_W) DIVISION_ADAS_ALGO_W;

    pOutLdwsResult->info.fty1 = ((pInLdwsResult->info.fty1 * ADAS_OSD_H) DIVISION_ADAS_ALGO_H) + ADAS_OSD_OFFSET_H;
    pOutLdwsResult->info.fty2 = ((pInLdwsResult->info.fty2 * ADAS_OSD_H) DIVISION_ADAS_ALGO_H) + ADAS_OSD_OFFSET_H;
    pOutLdwsResult->info.fty3 = ((pInLdwsResult->info.fty3 * ADAS_OSD_H) DIVISION_ADAS_ALGO_H) + ADAS_OSD_OFFSET_H;
    pOutLdwsResult->info.fty4 = ((pInLdwsResult->info.fty4 * ADAS_OSD_H) DIVISION_ADAS_ALGO_H) + ADAS_OSD_OFFSET_H;
    
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s (ftx1,fty2)(%d,%d) (ftx2,fty2)(%d,%d) (ftx3,fty3)(%d,%d) (ftx4,fty4)(%d,%d)", LDWS_STR, 
                                        pOutLdwsResult->info.ftx1, pOutLdwsResult->info.fty1,
                                        pOutLdwsResult->info.ftx2, pOutLdwsResult->info.fty2,
                                        pOutLdwsResult->info.ftx3, pOutLdwsResult->info.fty3,
                                        pOutLdwsResult->info.ftx4, pOutLdwsResult->info.fty4);
}
/*--------------------------------------------------------------------------*
* Function name : warnOverSecsProc 
* Function		: warn over sec process
* Return value	: static void 
* Parameter1	: UINT32 *pCurWarnType
* Parameter2	: UINT8 nSecs
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void warnOverSecsProc(UINT32 *pCurWarnType, UINT8 nSecs){
    warnOverSecInit(&g_stWarnKeepSec, &nSecs);
    warnOverSecRun(&g_stWarnKeepSec, pCurWarnType);
}
/*--------------------------------------------------------------------------*
* Function name : warnOverSecInit 
* Function		: warn over sec init
* Return value	: static void 
* Parameter1	: tWarnStragetySecs* pInOutKeepSec
* Parameter2	: UINT8* pSecs
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void warnOverSecInit(tWarnStragetySecs* pInOutKeepSec, UINT8* pSecs){
    if(!pInOutKeepSec || !pSecs){
        return;    
    }

    if(!pInOutKeepSec->pWarnPerSecsInfo){
        pInOutKeepSec->pWarnPerSecsInfo = (tWarnPerSecsInfo*)sp5kMalloc(*pSecs * sizeof(tWarnPerSecsInfo));
        memset(pInOutKeepSec->pWarnPerSecsInfo, 0, *pSecs * sizeof(tWarnPerSecsInfo));
    }
}
/*--------------------------------------------------------------------------*
* Function name : warnOverSecRun 
* Function		: warn over sec run
* Return value	: static void 
* Parameter1	: tWarnStragetySecs* pInOutKeepSec
* Parameter2	: UINT32 *pInCurWarnType
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void warnOverSecRun(tWarnStragetySecs* pInOutKeepSec, UINT32 *pInCurWarnType){
    if(!pInOutKeepSec || !pInCurWarnType){
        return;
    }

    static UINT8 nSecIndex = 0;
    nSecIndex = pInOutKeepSec->nSatisfiedSecs;

    //record data
    switch(*pInCurWarnType){
        case E_ADAS_LDWS_WARN_SOUND_NO:
            break;
        case E_ADAS_LDWS_WARN_SOUND_LEFT:
            if(!pInOutKeepSec->bFirstTrigger){
                pInOutKeepSec->nPreserveWarnType = E_ADAS_LDWS_WARN_SOUND_LEFT;
                ++(pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nTriggerCnt);
                pInOutKeepSec->bFirstTrigger = TRUE;
                pInOutKeepSec->nStarTime = sp5kOsTimeGet();
            }
            else{
                if(pInOutKeepSec->nPreserveWarnType == *pInCurWarnType){
                    ++(pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nTriggerCnt);
                }
                else{
                    //reset
                    pInOutKeepSec->nStarTime = 0;
                    pInOutKeepSec->nEndTime = 0;
                    pInOutKeepSec->bFirstTrigger = FALSE;
                    pInOutKeepSec->nPreserveWarnType = 0;
                    pInOutKeepSec->nSatisfiedSecs = 0;
                    memset(pInOutKeepSec->pWarnPerSecsInfo, 0, pInOutKeepSec->nOverConditionSecs * sizeof(tWarnPerSecsInfo));
                }
            }
            break;
        case E_ADAS_LDWS_WARN_SOUND_RIGHT:
            if(!pInOutKeepSec->bFirstTrigger){
                pInOutKeepSec->nPreserveWarnType = *pInCurWarnType;
                ++(pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nTriggerCnt);
                pInOutKeepSec->bFirstTrigger = TRUE;
                pInOutKeepSec->nStarTime = sp5kOsTimeGet();
            }
            else{
                if(pInOutKeepSec->nPreserveWarnType == *pInCurWarnType){
                    ++(pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nTriggerCnt);
               }
               else{
                    //reset
                    pInOutKeepSec->nStarTime = 0;
                    pInOutKeepSec->nEndTime = 0;
                    pInOutKeepSec->bFirstTrigger = FALSE;
                    pInOutKeepSec->nPreserveWarnType = 0;
                    pInOutKeepSec->nSatisfiedSecs = 0;
                    memset(pInOutKeepSec->pWarnPerSecsInfo, 0, pInOutKeepSec->nOverConditionSecs * sizeof(tWarnPerSecsInfo));
               }
            }
            break;
        default:
            break;
    }

    //decide warn or not
    #define CONDICTION_PERCENTAGE 0.5f
    
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "bFirstTrigger:%d cnt:%d frmcnt:%d PreType:%d CurType:%d nGap:%d \n", pInOutKeepSec->bFirstTrigger, pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nTriggerCnt, pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nFrameCnt, pInOutKeepSec->nPreserveWarnType, *pInCurWarnType, pInOutKeepSec->nEndTime);
    if(pInOutKeepSec->bFirstTrigger){
        ++(pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nFrameCnt);

        if(0 != pInOutKeepSec->nStarTime){
	    static UINT8 nCheckGapTime = 1;		
            pInOutKeepSec->nEndTime = sp5kOsTimeGet() - pInOutKeepSec->nStarTime;

            if(pInOutKeepSec->nEndTime > (nCheckGapTime * 100)){    //sp5kOsTime unit: 10ms, every 1 sec to check condition
                pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nConditionOverCnt = (UINT16) (CONDICTION_PERCENTAGE * pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nFrameCnt);
                if( pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nTriggerCnt > pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nConditionOverCnt){
                    ++(pInOutKeepSec->nSatisfiedSecs);
                    pInOutKeepSec->nStarTime = sp5kOsTimeGet();
                }
                else{
                    //reset
                    pInOutKeepSec->nStarTime = 0;
                    pInOutKeepSec->nEndTime = 0;
                    pInOutKeepSec->bFirstTrigger = FALSE;
                    pInOutKeepSec->nPreserveWarnType = 0;
                    pInOutKeepSec->nSatisfiedSecs = 0;
                    memset(pInOutKeepSec->pWarnPerSecsInfo, 0, pInOutKeepSec->nOverConditionSecs * sizeof(tWarnPerSecsInfo));
                }

                if(pInOutKeepSec->nSatisfiedSecs >= pInOutKeepSec->nOverConditionSecs){
                    //play sound    
                    switch(pInOutKeepSec->nPreserveWarnType){
                        case E_ADAS_LDWS_WARN_SOUND_LEFT:
                            g_eADAS_SOUND = E_ADAS_SOUND_LDWS_BEEP;
                            HOST_PROF_LOG_PRINT(LEVEL_DEBUG,"\n play Sound bFirstTrigger:%d cnt:%d frmcnt:%d nCodiCnt:%d nGap:%d \n", pInOutKeepSec->bFirstTrigger, pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nTriggerCnt, pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nFrameCnt, pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nConditionOverCnt, pInOutKeepSec->nEndTime);
                            sp5kOsEventFlagsSet(&g_EventAdasStartPlayAudio, 1, TX_OR);
                            break;
                        case E_ADAS_LDWS_WARN_SOUND_RIGHT:
                            g_eADAS_SOUND = E_ADAS_SOUND_LDWS_BEEP;
                            HOST_PROF_LOG_PRINT(LEVEL_DEBUG,"\n play Sound bFirstTrigger:%d cnt:%d frmcnt:%d nCodiCnt:%d nGap:%d \n", pInOutKeepSec->bFirstTrigger, pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nTriggerCnt, pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nFrameCnt, pInOutKeepSec->pWarnPerSecsInfo[nSecIndex].nConditionOverCnt, pInOutKeepSec->nEndTime);
                            sp5kOsEventFlagsSet(&g_EventAdasStartPlayAudio, 1, TX_OR);
                            break;    
                        default:
                            break;
                    }
                
                    //reset
                    pInOutKeepSec->nStarTime = 0;
                    pInOutKeepSec->nEndTime = 0;
                    pInOutKeepSec->bFirstTrigger = FALSE;
                    pInOutKeepSec->nPreserveWarnType = 0;
                    pInOutKeepSec->nSatisfiedSecs = 0;
                    memset(pInOutKeepSec->pWarnPerSecsInfo, 0, pInOutKeepSec->nOverConditionSecs * sizeof(tWarnPerSecsInfo));
                }
            }
        }
    }
}
/*--------------------------------------------------------------------------*
* Function name : decidePlayWarningSound 
* Function		: play warning sound
* Return value	: static void 
* Parameter1	: const UINT8 nWarnStrategyType
* Parameter2	: const UINT32 *pInWarnFlag
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void decidePlayWarningSound(const UINT8 nWarnStrategyType, const UINT32 *pInWarnFlag){
    if(!pInWarnFlag){
        return;
    }
     UINT32 pure_raw = 0xF;
    //E_ADAS_LDWS_WARN_STRATEGY_NORMAL
    static UINT32 nSoundFlag = 0;
    nSoundFlag = (*pInWarnFlag) >> 4;         //normal warning sound 

    //E_ADAS_LDWS_WARN_STRATEGY_DEPARTURE_SECS
    static UINT32 nCurrentWarnType = 0;
    nCurrentWarnType = *pInWarnFlag && pure_raw;  //pure raw data
    #define OVER_SECS 5

    switch(nWarnStrategyType){
        case E_ADAS_LDWS_WARN_STRATEGY_NORMAL:
            switch(nSoundFlag){
                case E_ADAS_LDWS_WARN_SOUND_LEFT:
                case E_ADAS_LDWS_WARN_SOUND_RIGHT:
                    g_eADAS_SOUND = E_ADAS_SOUND_LDWS_BEEP;
                    sp5kOsEventFlagsSet(&g_EventAdasStartPlayAudio, 1, TX_OR);
                    break;    
                default:
                    break; 
            }
            break;
        case E_ADAS_LDWS_WARN_STRATEGY_DEPARTURE_SECS:
            warnOverSecsProc(&nCurrentWarnType, OVER_SECS);
            break;
        default:
            break;
    }
}
/*--------------------------------------------------------------------------*
* Function name : drawProductOSDLane 
* Function		: play warning sound
* Return value	: static void 
* Parameter1	: sp5kGfxPageNum_e eDrawedLayer
* History	    :
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void drawProductOSDLane(sp5kGfxPageNum_e eDrawedLayer){
    //vanishing point line
    //sp5kGfxAttrSet(SP5K_GFX_FILL_ENABLE, 1, 0, 0, 0);
    //sp5kGfxAttrSet(SP5K_GFX_PEN_ENABLE, 1, 0, 0, 0);
    sp5kGfxAttrSet(SP5K_GFX_PEN_WIDTH, 1, 0, 0, 0);
    sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_VANISHINGPT_LINE_COLOR, 0, 0, 0);
    sp5kGfxLineDraw(eDrawedLayer, 0, VANISHING_PT_Y, PRODUCT_OSD_W, VANISHING_PT_Y);

    //set default left lane / right lane point
    static int leftLane_x1, leftLane_y1, leftLane_x2, leftLane_y2, rightLane_x1, rightLane_y1, rightLane_x2, rightLane_y2;
    static int baseLine_Y1 = 140;
    static int bsaeLine_Y2 = 180;

    leftLane_x2 = PRODUCT_OSD_W >> 2;
    leftLane_y2 = bsaeLine_Y2;
    rightLane_x2 = PRODUCT_OSD_W - (PRODUCT_OSD_W >> 2);
    rightLane_y2 = bsaeLine_Y2;

    //y = ax + b
    static float fLeftLaneSlope = 0.f;
    static float fRightLaneSlope = 0.f;
    fLeftLaneSlope = (float)(VANISHING_PT_Y - leftLane_y2) / (VANISHING_PT_X - leftLane_x2);
    fRightLaneSlope = -fLeftLaneSlope;
    //b = y - ax
    static float fLeftLane_b = 0.f;
    static float fRightLane_b = 0.f;
    fLeftLane_b = VANISHING_PT_Y - fLeftLaneSlope * VANISHING_PT_X;
    fRightLane_b = VANISHING_PT_Y - fRightLaneSlope * VANISHING_PT_X;
    //x = (y - b) / a
    leftLane_x1 = (baseLine_Y1 - fLeftLane_b) / fLeftLaneSlope;
    leftLane_y1 = baseLine_Y1;
    rightLane_x1 = (baseLine_Y1 - fRightLane_b) / fRightLaneSlope;
    rightLane_y1 = baseLine_Y1;

    
    switch(g_LdwsWarnDrawFlag){
        case E_ADAS_LDWS_WARN_DRAW_NO:
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%d]%s No Shift", sp5kMsTimeGet(), LDWS_STR);
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
            sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
            sp5kGfxLineDraw(eDrawedLayer, leftLane_x1, leftLane_y1, leftLane_x2, leftLane_y2);
            sp5kGfxLineDraw(eDrawedLayer, rightLane_x1, rightLane_y1, rightLane_x2, rightLane_y2);
            break;
        case E_ADAS_LDWS_WARN_DRAW_LEFT:
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%d]%s Left Shift", sp5kMsTimeGet(), LDWS_STR);
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
            sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
            sp5kGfxLineDraw(eDrawedLayer, leftLane_x1, leftLane_y1, leftLane_x2, leftLane_y2);
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
            sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
            sp5kGfxLineDraw(eDrawedLayer, rightLane_x1, rightLane_y1, rightLane_x2, rightLane_y2);
            break;
        case E_ADAS_LDWS_WARN_DRAW_RIGHT:
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%d]%s Right Shift", sp5kMsTimeGet(), LDWS_STR);
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
            sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
            sp5kGfxLineDraw(eDrawedLayer, leftLane_x1, leftLane_y1, leftLane_x2, leftLane_y2);
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
            sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
            sp5kGfxLineDraw(eDrawedLayer, rightLane_x1, rightLane_y1, rightLane_x2, rightLane_y2);
            break;
        case E_ADAS_LDWS_WARN_DRAW_BOTH:
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%d]%s Both Shift", sp5kMsTimeGet(), LDWS_STR);
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
            sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
            sp5kGfxLineDraw(eDrawedLayer, leftLane_x1, leftLane_y1, leftLane_x2, leftLane_y2);
            sp5kGfxLineDraw(eDrawedLayer, rightLane_x1, rightLane_y1, rightLane_x2, rightLane_y2);
            break;
        default:
            break;
    }
}
/*--------------------------------------------------------------------------*
* Function name : drawLane 
* Function		: play warning sound
* Return value	: static void 
* Parameter1	: sp5kGfxPageNum_e eDrawedLayer
* Parameter2	: const sp5kAdasLdwsResult_t* pInLdwsResult
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void drawLane(sp5kGfxPageNum_e eDrawedLayer, const sp5kAdasLdwsResult_t* pInLdwsResult){
    if(pInLdwsResult == 0){
        return;
    }

    //base on algo team v2.17 spec
    if ((pInLdwsResult->info.ftx1 == 0 && pInLdwsResult->info.ftx2 == 0 && pInLdwsResult->info.fty1 == 0 && pInLdwsResult->info.fty2 == 0)
        || (pInLdwsResult->info.ftx3 == 0 && pInLdwsResult->info.ftx4 == 0 && pInLdwsResult->info.fty3 == 0 && pInLdwsResult->info.fty4 == 0)){
        g_ldwsLaneDraw = 0;
    }
    else{
        g_ldwsLaneDraw = 1;
    }

    sp5kGfxAttrSet(SP5K_GFX_FILL_ENABLE, 1, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PEN_ENABLE, 1, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PEN_WIDTH, 1, 0, 0, 0);

    if (g_ldwsLaneDraw){
        switch(g_LdwsWarnDrawFlag){
            case E_ADAS_LDWS_WARN_DRAW_NO:
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%d]%s No Shift", sp5kMsTimeGet(), LDWS_STR);
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
                sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
                sp5kGfxLineDraw(eDrawedLayer, pInLdwsResult->info.ftx1, pInLdwsResult->info.fty1, pInLdwsResult->info.ftx2, pInLdwsResult->info.fty2);
                sp5kGfxLineDraw(eDrawedLayer, pInLdwsResult->info.ftx3, pInLdwsResult->info.fty3, pInLdwsResult->info.ftx4, pInLdwsResult->info.fty4);
                break;
            case E_ADAS_LDWS_WARN_DRAW_LEFT:
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%d]%s Left Shift", sp5kMsTimeGet(), LDWS_STR);
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
                sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
                sp5kGfxLineDraw(eDrawedLayer, pInLdwsResult->info.ftx1, pInLdwsResult->info.fty1, pInLdwsResult->info.ftx2, pInLdwsResult->info.fty2);
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
                sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
                sp5kGfxLineDraw(eDrawedLayer, pInLdwsResult->info.ftx3, pInLdwsResult->info.fty3, pInLdwsResult->info.ftx4, pInLdwsResult->info.fty4);
                break;
            case E_ADAS_LDWS_WARN_DRAW_RIGHT:
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%d]%s Right Shift", sp5kMsTimeGet(), LDWS_STR);
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
                sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
                sp5kGfxLineDraw(eDrawedLayer, pInLdwsResult->info.ftx1, pInLdwsResult->info.fty1, pInLdwsResult->info.ftx2, pInLdwsResult->info.fty2);
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
                sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
                sp5kGfxLineDraw(eDrawedLayer, pInLdwsResult->info.ftx3, pInLdwsResult->info.fty3, pInLdwsResult->info.ftx4, pInLdwsResult->info.fty4);
                break;
            case E_ADAS_LDWS_WARN_DRAW_BOTH:
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%d]%s Both Shift", sp5kMsTimeGet(), LDWS_STR);
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
                sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
                sp5kGfxLineDraw(eDrawedLayer, pInLdwsResult->info.ftx1, pInLdwsResult->info.fty1, pInLdwsResult->info.ftx2, pInLdwsResult->info.fty2);
                sp5kGfxLineDraw(eDrawedLayer, pInLdwsResult->info.ftx3, pInLdwsResult->info.fty3, pInLdwsResult->info.ftx4, pInLdwsResult->info.fty4);
                break;
            default:
                break;
        }
    }
}
/*--------------------------------------------------------------------------*
* Function name : drawDetectingArea 
* Function		: draw detect area
* Return value	: static void 
* Parameter1	: sp5kGfxPageNum_e eDrawedLayer
* Parameter2	: const int* pInCenterY
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void drawDetectingArea(sp5kGfxPageNum_e eDrawedLayer, const int* pInCenterY){
    //based on algorithm team information
    #define LDWS_DETECT_START_H_RATIO 0.6f
    #define LDWS_DETECT_END_H_RATIO 0.76f
    short nStartH = LDWS_DETECT_START_H_RATIO * ADAS_OSD_H + ADAS_OSD_OFFSET_H;
    short nEndH = LDWS_DETECT_END_H_RATIO * ADAS_OSD_H + ADAS_OSD_OFFSET_H;

    //vanishing point line
    short nAdjustedVptY = ((*pInCenterY * ADAS_OSD_H) DIVISION_ADAS_ALGO_H) + ADAS_OSD_OFFSET_H;
    sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_GREEN, 0, 0, 0);
    sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, PAL_GREEN, 0, 0, 0);
    sp5kGfxLineDraw(eDrawedLayer, 0, nAdjustedVptY, ADAS_OSD_W, nAdjustedVptY);
    
    //adjust vanishing point y
    if( ADAS_ALGO_CENTER_Y != (*pInCenterY)){
        short nAdjustedOffsetY = nAdjustedVptY - LDWS_OSD_DEFAULT_CENTER_Y;
        //Adjustted detecting area
        nStartH += nAdjustedOffsetY;
        nEndH += nAdjustedOffsetY;
        sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_WHITE, 0, 0, 0);
        sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, PAL_WHITE, 0, 0, 0);
        sp5kGfxLineDraw(eDrawedLayer, 0, nStartH, ADAS_OSD_W, nStartH);
        sp5kGfxLineDraw(eDrawedLayer, 0, nEndH, ADAS_OSD_W, nEndH);
    }
    else{
        //default detecting area
        sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_WHITE, 0, 0, 0);
        sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, PAL_WHITE, 0, 0, 0);
        sp5kGfxLineDraw(eDrawedLayer, 0, nStartH, ADAS_OSD_W, nStartH);
        sp5kGfxLineDraw(eDrawedLayer, 0, nEndH, ADAS_OSD_W, nEndH);
    }
}

