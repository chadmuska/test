/*--------------------------------------------------------------------------*
 *	File name	: app_test_adas_fcws.c
 *	Function	: Test ADAS FCWS function
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


#include "app_test_adas_fcws.h"
#include "app_test_adas_api.h"
#include "app_test_adas.h"
#include "sp5k_media_api.h"
#include "sp5k_gfx_api.h"
#include "app_osd_api.h"
#include "app_pal.h"
#include "app_osd.h"
#include "app_aaa.h"
#include "sp5k_dbg_api.h"

#include "app_dbg_api.h"
#if defined(HOST_DBG)
#undef HOST_DBG
#define HOST_DBG 1 
#endif

extern SP5K_EVENT_FLAGS_GROUP g_EventAdasStartPlayAudio;
extern eADAS_Sound g_eADAS_SOUND;
extern appAeCb_t aeCb;
extern BOOL isAdasLpdEnabled();

sp5kPatternTrackCarRes_t g_fcwsResult;
unsigned int g_nFcwsCenterX = FCWS_ADJUST_ALGO_CENTER_X;
unsigned int g_nFcwsCenterY = FCWS_ADJUST_ALGO_CENTER_Y;

/*
    default parameters, the customer must decide it by  themselves
        1. focal length 
        2. pixel unit. 
 */
static int g_nSensorFocalLen = 3.6*10000; 
static int g_nSensorPixelUnit = 158;

static UINT8 g_nDayNightFlag = DAY;
static int g_nFcwsPeriod = 2;
 
/*Y: 0-255, U: 256-511, V: 512-767 */
static UINT8 FCWS_LUT[768] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

void appFcwsStop();
sp5kPatternTrackCarRes_t* appFcwsGetResult();

static void appTestFcwsDayNightExchange(UINT16 lvIdx);
static void init(const unsigned int *pInCenterX, const unsigned int *pInCenterY, ...);
static void start();
static void stop();
static void fcwsDoneCb(UINT32 param, UINT32 rsvd, void *prsv);
static void decidePlayFcwsWarningSound2ndStage();

//FOR FCWS and LPD
void CovertToOSDCoordinate(int pInX, int pInY, int pInSize, int* pOutX, int* pOutY, int* pOutSize);
void CovertToYUVCoordinate(const UINT32* pInFrameWidth, const UINT32* pInFrameHeight, int pInX, int pInY, int pInSize, int* pOutX, int* pOutY, int* pOutSize);
/*--------------------------------------------------------------------------*
* Function name : appFcwsStart 
* Function		: fcws function start
* Return value	: void 
* Parameter1	: const unsigned int nInCenterX
* Parameter2	: const unsigned int nInCenterY
* Parameter3	: C - Variable Arguments
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appFcwsStart(const unsigned int nInCenterX, const unsigned int nInCenterY, ...){
    g_nFcwsCenterX = nInCenterX;
    g_nFcwsCenterY = nInCenterY;

    va_list args;
    va_start(args, nInCenterY);
    
    g_nDayNightFlag = va_arg(args, UINT32);
    g_nSensorFocalLen = va_arg(args, int);
    g_nSensorPixelUnit = va_arg(args, int);

    UINT32 fr = 0;
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, &fr);
    if(g_nSensorPixelUnit == va_arg(args, int)){  //without period input
        if (fr <= 30) {
            g_nFcwsPeriod = FCWS_30FPS_INTERVAL;
        }
        else if (fr > 30 && fr <= 60) {
            g_nFcwsPeriod = FCWS_60FPS_INTERVAL;
        }
        else {
            g_nFcwsPeriod = FCWS_OTHER_INTERVAL;
        }
    }
    else{
        g_nFcwsPeriod = va_arg(args, int);
    }

    init(&g_nFcwsCenterX, &g_nFcwsCenterY, g_nDayNightFlag, g_nSensorFocalLen, g_nSensorPixelUnit, g_nFcwsPeriod);
    start();
    va_end(args);  
}
/*--------------------------------------------------------------------------*
* Function name : appFcwsStop 
* Function		: fcws function stop
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appFcwsStop(){
    stop();
}
/*--------------------------------------------------------------------------*
* Function name : appFcwsGetResult 
* Function		: fcws get result
* Return value	: sp5kPatternTrackCarRes_t* 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
sp5kPatternTrackCarRes_t* appFcwsGetResult(){
    return &g_fcwsResult;
}
/*--------------------------------------------------------------------------*
* Function name : appFcwsDrawProductOsdResult 
* Function		: fcws draw product osd result
* Return value	: void 
* Parameter1	: UINT8 eDrawedLayer
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appFcwsDrawProductOsdResult(UINT8 eDrawedLayer){
    if( 0 >= g_fcwsResult.num) {
        return;
    }

    switch(eDrawedLayer){
        case SP5K_GFX_PAGE_PIP_0:
        case (SP5K_GFX_PAGE_PIP_0+1):
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PIP_COLOR565_RED, 0, 0, 0);
            break;
        case SP5K_GFX_PAGE_OSD_0:
        case (SP5K_GFX_PAGE_OSD_0+1):
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_RED, 0, 0, 0);
            break;
        default:
            break;
    }

    static char sText[16] = {};
    int i = 0;
    for(; i < g_fcwsResult.num; ++i){
        int x = 0, y = 0, size = 0;
        CovertToOSDCoordinate(g_fcwsResult.cars[i].x, g_fcwsResult.cars[i].y, g_fcwsResult.cars[i].size, &x, &y, &size);

        if(g_fcwsResult.centerCarIdx == i){
            if(1 == g_fcwsResult.warn){
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_RED, 0, 0, 0);
                appOsd_ColorTextColorSet(PAL_RED, PAL_NOCOLOR, PAL_NOCOLOR);
            }
            else{
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_WHITE, 0, 0, 0);
                appOsd_ColorTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR);
            }
        }
        else{
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_WHITE, 0, 0, 0);
            appOsd_ColorTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR);
        }

        //car rectangle
        int nCornerWidth = size >> 2; 
        //left-top corner
        sp5kGfxLineDraw(eDrawedLayer, x, y, x + nCornerWidth , y); 
        sp5kGfxLineDraw(eDrawedLayer, x, y, x, y + nCornerWidth); 
        //right-top corner
        sp5kGfxLineDraw(eDrawedLayer, x + size, y, x + size - nCornerWidth, y);
        sp5kGfxLineDraw(eDrawedLayer, x + size, y, x + size, y + nCornerWidth);
        //left-down corner
        sp5kGfxLineDraw(eDrawedLayer, x, y + size, x + nCornerWidth, y + size);
        sp5kGfxLineDraw(eDrawedLayer, x, y + size, x, y + size - nCornerWidth);
        //right-down corner
        sp5kGfxLineDraw(eDrawedLayer, x + size, y + size, x + size - nCornerWidth, y+size);
        sp5kGfxLineDraw(eDrawedLayer, x + size, y + size, x + size, y + size - nCornerWidth);

        //draw distance
        sprintf(sText, "%dM", g_fcwsResult.cars[i].dist);  
        sp5kGfxStringDraw(eDrawedLayer, x + (size >> 1), y, SP5K_GFX_ALIGN_BOTTOM, SP5K_GFX_ENCODE_ASCII, (UINT8*)sText);
    }
}
/*--------------------------------------------------------------------------*
* Function name : appFcwsDumpSetting 
* Function		: fcws dump setting
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appFcwsDumpSetting(){
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "%s Setting", FCWS_STR);
    //HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t centerpoint:(%d",%d), ldws_cfg_get.angle_thr);

    UINT32 nCenterPoint = 0;
    UINT32 nCenterX = 0;
    UINT32 nCenterY = 0;
    UINT32 tolerfactor = 0;
    UINT32 focal = 0;
    UINT32 pixel_unit = 0;
    UINT32 warn_distance = 0;
    UINT32 crop_factor = 0;
    UINT32 crop_offset = 0;
    UINT32 daynight = 0;

    sp5kPatternTrackCfgGet(SP5K_PATTERN_TRACK_CAR_FC_CENTER_POINT, &nCenterPoint);
    nCenterX = nCenterPoint & 0x0000FFFF;
    nCenterY = (nCenterPoint >> 16) & 0x0000FFFF;
    sp5kPatternTrackCfgGet(SP5K_PATTERN_TRACK_CAR_FC_CENTER_TOLER_FACTOR, &tolerfactor);
    sp5kPatternTrackCfgGet(SP5K_PATTERN_TRACK_CAR_FC_SENSOR_FOCAL,&focal);
    sp5kPatternTrackCfgGet(SP5K_PATTERN_TRACK_CAR_FC_SENSOR_PIXEL_UNIT,&pixel_unit);
    sp5kPatternTrackCfgGet(SP5K_PATTERN_TRACK_CAR_FC_WARN_DISTANCE,&warn_distance);
    sp5kPatternTrackCfgGet(SP5K_PATTERN_TRACK_CAR_FC_ROI_CROP_FACTOR,&crop_factor);
    sp5kPatternTrackCfgGet(SP5K_PATTERN_TRACK_CAR_FC_ROI_CROP_OFFSET,&crop_offset);
    sp5kPatternTrackCfgGet(SP5K_PATTERN_TRACK_CAR_FC_DAYNIGHT,&daynight);

    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t Center Point(%d,%d)", nCenterX, nCenterY);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t tolerfactor:%d", tolerfactor);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t focal:%d", focal);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t pixel_unit:%d", pixel_unit);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t warn_distance:%d", warn_distance);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t crop_factor:%d", crop_factor);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t crop_offset:%d", crop_offset);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t daynight:%d",daynight);
}
/*--------------------------------------------------------------------------*
* Function name : debugFcwsDrawResult 
* Function		: fcws draw result for debug
* Return value	: void 
* Parameter1	: UINT8 eDrawedLayer
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void debugFcwsDrawResult(UINT8 eDrawedLayer){
    //day, night
    static char sText[16] = {};
    sprintf(sText, "%s", ((g_nDayNightFlag == DAY) ? "Day" : "Night"));
    appOsd_ColorTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR);
    sp5kGfxStringDraw(eDrawedLayer, OSD_DEFAULT_FRAMSIZE_W >> 1, 15 * (OSD_DEFAULT_FRAMSIZE_H >> 4) + 10, SP5K_GFX_ALIGN_BOTTOM, SP5K_GFX_ENCODE_ASCII, (UINT8*)sText);

    if( 0 >= g_fcwsResult.num) {
        return;
    }

    sp5kGfxAttrSet(SP5K_GFX_FILL_ENABLE, 1, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PEN_ENABLE, 1, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PEN_WIDTH, 1, 0, 0, 0);

    switch(eDrawedLayer){
        case (SP5K_GFX_PAGE_PIP_0+1):
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PIP_COLOR565_RED, 0, 0, 0);
            break;
        case (SP5K_GFX_PAGE_OSD_0+1):
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_RED, 0, 0, 0);
            break;
        default:
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_RED, 0, 0, 0);
            break;
    }

    int i = 0;
    for(; i < g_fcwsResult.num; ++i){

        int x = 0, y = 0, size = 0;
        CovertToOSDCoordinate(g_fcwsResult.cars[i].x, g_fcwsResult.cars[i].y, g_fcwsResult.cars[i].size, &x, &y, &size);

        if(g_fcwsResult.centerCarIdx == i){
            if(1 == g_fcwsResult.warn){
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_RED, 0, 0, 0);
                appOsd_ColorTextColorSet(PAL_RED, PAL_NOCOLOR, PAL_NOCOLOR);
            }
            else{
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_WHITE, 0, 0, 0);
                appOsd_ColorTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR);
            }
        }
        else{
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_WHITE, 0, 0, 0);
            appOsd_ColorTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR);
        }

        //car rectangle
        int nCornerWidth = size >> 2; 
        //left-top corner
        sp5kGfxLineDraw(eDrawedLayer, x, y, x + nCornerWidth , y); 
        sp5kGfxLineDraw(eDrawedLayer, x, y, x, y + nCornerWidth); 
        //right-top corner
        sp5kGfxLineDraw(eDrawedLayer, x + size, y, x + size - nCornerWidth, y);
        sp5kGfxLineDraw(eDrawedLayer, x + size, y, x + size, y + nCornerWidth);
        //left-down corner
        sp5kGfxLineDraw(eDrawedLayer, x, y + size, x + nCornerWidth, y + size);
        sp5kGfxLineDraw(eDrawedLayer, x, y + size, x, y + size - nCornerWidth);
        //right-down corner
        sp5kGfxLineDraw(eDrawedLayer, x + size, y + size, x + size - nCornerWidth, y+size);
        sp5kGfxLineDraw(eDrawedLayer, x + size, y + size, x + size, y + size - nCornerWidth);

        //draw distance
        sprintf(sText, "%dM", g_fcwsResult.cars[i].dist);  
        sp5kGfxStringDraw(eDrawedLayer, x + (size >> 1), y, SP5K_GFX_ALIGN_BOTTOM, SP5K_GFX_ENCODE_ASCII, (UINT8*)sText);
    }
}
/*--------------------------------------------------------------------------*
* Function name : debugFcwsDrawResultToRecord 
* Function		: fcws draw result to record
* Return value	: void 
* Parameter1	: frameBufInfo_t *pframe
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void debugFcwsDrawResultToRecord(frameBufInfo_t *pframe){
    if(!pframe){
        return;
    }

    UINT32 nCarColor = YUV_COLOR_WHITE;
    #define FCWS_CAR_RECTLINE_WIDTH 5

    int i = 0;
    for(; i < g_fcwsResult.num; ++i){
        int x = 0, y = 0, size = 0;
        CovertToYUVCoordinate(&(pframe->width), &(pframe->height), g_fcwsResult.cars[i].x, g_fcwsResult.cars[i].y, g_fcwsResult.cars[i].size, &x, &y, &size);

        if(g_fcwsResult.centerCarIdx == i){
            if(1 == g_fcwsResult.warn){
                nCarColor = YUV_COLOR_RED;
            }
            else{
                nCarColor = YUV_COLOR_WHITE;
            }
        }
        else{
            nCarColor = YUV_COLOR_WHITE;
        }

        //car rectangle
        int nCornerWidth = size >> 2; 
        //left-top corner
        sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, x, y, x + nCornerWidth, y, nCarColor, FCWS_CAR_RECTLINE_WIDTH); 
        sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, x, y, x, y + nCornerWidth, nCarColor, FCWS_CAR_RECTLINE_WIDTH); 
        //right-top corner
        sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, x + size, y, x + size - nCornerWidth, y, nCarColor, FCWS_CAR_RECTLINE_WIDTH);
        sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, x + size, y, x + size, y + nCornerWidth, nCarColor, FCWS_CAR_RECTLINE_WIDTH);
        //left-down corner
        sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, x, y + size, x + nCornerWidth, y + size, nCarColor, FCWS_CAR_RECTLINE_WIDTH);
        sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, x, y + size, x, y + size - nCornerWidth, nCarColor, FCWS_CAR_RECTLINE_WIDTH);
        //right-down corner
        sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, x + size, y + size, x + size - nCornerWidth, y+size, nCarColor, FCWS_CAR_RECTLINE_WIDTH);
        sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, x + size, y + size, x + size, y + size - nCornerWidth, nCarColor, FCWS_CAR_RECTLINE_WIDTH);
    }

    drawFcwsStampOnLayerYUV(&g_fcwsResult);
    blendFcwsStampToYUV(pframe, &g_fcwsResult);
}
/*--------------------------------------------------------------------------*
* Function name : demoFcwsDrawResultToRecord 
* Function		: demo fcws draw result to record
* Return value	: void 
* Parameter1	: frameBufInfo_t *pframe
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void demoFcwsDrawResultToRecord(frameBufInfo_t *pframe){
    if(!pframe){
        return;
    }

    UINT32 nCarColor = YUV_COLOR_WHITE;
#define FCWS_CAR_RECTLINE_WIDTH 5

    int i = 0;
    for(; i < g_fcwsResult.num; ++i){
        int x = 0, y = 0, size = 0;
        CovertToYUVCoordinate(&(pframe->width), &(pframe->height), g_fcwsResult.cars[i].x, g_fcwsResult.cars[i].y, g_fcwsResult.cars[i].size, &x, &y, &size);

        if(g_fcwsResult.centerCarIdx == i){
            if(1 == g_fcwsResult.warn){
                nCarColor = YUV_COLOR_YELLOW;
            }
            else{
                nCarColor = YUV_COLOR_WHITE;
            }
        }
        else{
            nCarColor = YUV_COLOR_WHITE;
        }

        //car bottom line
        sp5kGfxYuvLineDraw(pframe->pbuf, pframe->width, pframe->height, x, y + size, x + size, y + size, nCarColor, FCWS_CAR_RECTLINE_WIDTH);
    }
    
    drawFcwsStampOnLayerYUV(&g_fcwsResult);
    blendFcwsStampToYUV(pframe, &g_fcwsResult);
}
/*--------------------------------------------------------------------------*
* Function name : appTestFcwsDayNightExchange 
* Function		: demo fcws draw result to record
* Return value	: static void 
* Parameter1	: UINT16 lvIdx
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void appTestFcwsDayNightExchange(UINT16 lvIdx){
    #if SP5K_SENSOR_ADV7480
    return;
    #endif
 
    UINT8 newdayNightFlag;

     if(lvIdx < 96){
        newdayNightFlag = NIGHT; /* night */
    }
    else{
        newdayNightFlag = DAY; /* day */
    }

    if(g_nDayNightFlag != newdayNightFlag){
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s DAYNIGHT_EXCHANGE dayNightFlag=%d newdayNightFlag=%d", FCWS_STR, g_nDayNightFlag, newdayNightFlag);
        g_nDayNightFlag = newdayNightFlag;
        sp5kPatternTrackCfgSet(SP5K_PATTERN_TRACK_CAR_FC_DAYNIGHT, g_nDayNightFlag);
    }
}
/*--------------------------------------------------------------------------*
* Function name : init 
* Function		: fcws function init
* Return value	: static void 
* Parameter1	: const unsigned int *pInCenterX
* Parameter2	: const unsigned int *pInCenterY
* Parameter3	: C - Variable Arguments
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void init(const unsigned int *pInCenterX, const unsigned int *pInCenterY, ...){
    va_list args;
    va_start(args, *pInCenterY);
    g_nDayNightFlag = va_arg(args, UINT32);

    /* 0. Setting Center Point*/
    unsigned int nCenterPoint = (*pInCenterX & 0x0000FFFF) | ((*pInCenterY << 16) & 0xFFFF0000);
    //HOST_PROF_LOG_PRINT(LEVEL_DEBUG, " x:%8X  y:%8X\n", (*pInCenterX & 0x0000FFFF), (*pInCenterY << 16) & 0xFFFF0000);
    //HOST_PROF_LOG_PRINT(LEVEL_DEBUG, " nCenterPoint %8X \n", nCenterPoint);
    sp5kPatternTrackCfgSet(SP5K_PATTERN_TRACK_CAR_FC_CENTER_POINT, nCenterPoint);

    /* 1. Setting camera focal length parameter */
    g_nSensorFocalLen = va_arg(args, int);
    sp5kPatternTrackCfgSet(SP5K_PATTERN_TRACK_CAR_FC_SENSOR_FOCAL, g_nSensorFocalLen);

    /* 2. Setting unit pixel width paramter */
    g_nSensorPixelUnit = va_arg(args, int);
    sp5kPatternTrackCfgSet(SP5K_PATTERN_TRACK_CAR_FC_SENSOR_PIXEL_UNIT, g_nSensorPixelUnit);

    /* 3. Setting warning distance parameter */
    sp5kPatternTrackCfgSet(SP5K_PATTERN_TRACK_CAR_FC_WARN_DISTANCE, FCWS_WARN_DISTANCE);

    /* 4. Setting crop factor and crop offset parameter */
    sp5kPatternTrackCfgSet(SP5K_PATTERN_TRACK_CAR_FC_ROI_CROP_FACTOR, FCWS_ROI_CROP_FACTOR);
    sp5kPatternTrackCfgSet(SP5K_PATTERN_TRACK_CAR_FC_ROI_CROP_OFFSET, FCWS_ROI_CROP_OFFSET);

    /* 5. Setting YUV threshold LUT table for night mode */
    sp5kPatternTrackCfgSet(SP5K_PATTERN_TRACK_CAR_FC_NIGHT_YUV_LUT_TABLE, (UINT32)(&FCWS_LUT[0]));

    /* 6. Setting day or night flag */
    sp5kPatternTrackCfgSet(SP5K_PATTERN_TRACK_CAR_FC_DAYNIGHT, g_nDayNightFlag);

    /* 7. Setting image scale factor parameter */
    UINT32 w = 0, h = 0;
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_WIDTH, &w);
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &h);
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s video w=%d  video h=%d", FCWS_STR, w, h);

    /* 8. Set horizontal decting area */
    sp5kPatternTrackCfgSet(SP5K_PATTERN_TRACK_CAR_FC_CENTER_TOLER_FACTOR, FCWS_CENTER_TOLER_FACTOR);

    switch(w){
        case 1920:
            sp5kPatternTrackCfgSet(SP5K_PATTERN_TRACK_CAR_FC_IMAGE_SCALE_FACTOR, 1000);
            break; 
        case 1280:
            sp5kPatternTrackCfgSet(SP5K_PATTERN_TRACK_CAR_FC_IMAGE_SCALE_FACTOR, 666);
            break;
        default:
            //fixme,other width unsupported?
            HOST_PROF_LOG_PRINT(LEVEL_ERROR, "%s 's frame width is unsupported", FCWS_STR);
            break;
    }

    /* Setting period*/
    g_nFcwsPeriod = va_arg(args, int);
    sp5kPatternTrackCfgSet(SP5K_FACE_TRACK_CFG_ACCUM_PERIOD, g_nFcwsPeriod);

    /* Setting result callback function */    
    sp5kPatternTrackCfgSet(SP5K_FACE_TRACK_CFG_FD_DONE_CB, (UINT32)fcwsDoneCb);

    //output parameters
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "%s parameters", FCWS_STR);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t center point (%d,%d)", *pInCenterX, *pInCenterY);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t day/night: %s", (g_nDayNightFlag == DAY) ? "Day" : "Night");
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t focal length:%d", g_nSensorFocalLen);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t pixel unit:%d", g_nSensorPixelUnit);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t warn distance:%d", FCWS_WARN_DISTANCE);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "\t period:%d", g_nFcwsPeriod);
    va_end(args);  
}
/*--------------------------------------------------------------------------*
* Function name : start 
* Function		: fcws start
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void start(){
    sp5kPatternTrackModeSet(1);
    adasSetFcws(TRUE);
    hostAdasStart();
}
/*--------------------------------------------------------------------------*
* Function name : stop 
* Function		: fcws stop
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void stop(){
    if(!isAdasLpdEnabled()){
        sp5kPatternTrackModeSet(0);
        adasSetFcws(FALSE);
        hostAdasStop();
    }
}
/*--------------------------------------------------------------------------*
* Function name : fcwsDoneCb 
* Function		: fcws done
* Return value	: static void 
* Parameter1	: UINT32 param
* Parameter2	: UINT32 rsvd
* Parameter3	: void *prsv
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void fcwsDoneCb(UINT32 param, UINT32 rsvd, void *prsv){
    appTestFcwsDayNightExchange(aeCb.lvidx);

    UINT32 pnface;
    sp5kPatternTrackResultGet(param, &pnface, &g_fcwsResult);

    UINT32 nCarIndex = g_fcwsResult.centerCarIdx;

    if( 0xFF != nCarIndex) {
        sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_CAR_ROI, g_fcwsResult.cars[nCarIndex].x, g_fcwsResult.cars[nCarIndex].y, g_fcwsResult.cars[nCarIndex].size);
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s][x,y,sz,dis,warn][%d,%d,%d,%d,%d]", __FUNCTION__, g_fcwsResult.cars[nCarIndex].x, g_fcwsResult.cars[nCarIndex].y, g_fcwsResult.cars[nCarIndex].size, g_fcwsResult.cars[nCarIndex].dist, g_fcwsResult.warn);
    }
    else {
        sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_CAR_ROI, 0, 0, 0);
    }

    decidePlayFcwsWarningSound2ndStage();
}
/*--------------------------------------------------------------------------*
* Function name : decidePlayFcwsWarningSound2ndStage 
* Function		: play fcws warning sound
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void decidePlayFcwsWarningSound2ndStage(){
#define FCWS_COUNT_DECREASE_THR 2 
#define FCWS_CHECK_RECENT_SUM_FRAME_CNT 30
#define FIRST_BEEP_DISTANCE 15
#define RELEASE_FIRST_BEEP_DISTANCE 20 
#define SECOND_BEEP_DISTANCE 10
#define RELEASE_SECOND_BEEP_DISTANCE 15

    static UINT32 nObservedCarDistance[FCWS_COUNT_DECREASE_THR+1] = {};
    static UINT32 nPreCenterCarDistance = 0;
    static UINT32 nCountDecrease = 0;
    static int nCountDown = FCWS_CHECK_RECENT_SUM_FRAME_CNT;        //for reset

    static BOOL bCouldPlayFirstSound = TRUE;
    static BOOL bCouldPlaySecondSound = TRUE;

    if(0xFF != g_fcwsResult.centerCarIdx){ //center car existed
        //condition : distance discrease
        if( 0 != nPreCenterCarDistance){
            if( g_fcwsResult.cars[g_fcwsResult.centerCarIdx].dist < nPreCenterCarDistance){
                ++nCountDecrease;
                nObservedCarDistance[nCountDecrease] = g_fcwsResult.cars[g_fcwsResult.centerCarIdx].dist;
                nObservedCarDistance[nCountDecrease-1] = nPreCenterCarDistance;
            }
            else if(g_fcwsResult.cars[g_fcwsResult.centerCarIdx].dist == nPreCenterCarDistance){
                //do nothing, just keep nCountDecrease
            }
            else{
                nCountDecrease = 0;
                memset(nObservedCarDistance, 0, sizeof(UINT32)*(FCWS_COUNT_DECREASE_THR + 1));
            }

            
            if(nCountDecrease > 0){
                --nCountDown;
                if(nCountDown < 0){
                    //reset
                    nPreCenterCarDistance = 0;
                    nCountDecrease = 0;
                    nCountDown = FCWS_CHECK_RECENT_SUM_FRAME_CNT;
                    memset(nObservedCarDistance, 0, sizeof(UINT32)*(FCWS_COUNT_DECREASE_THR + 1));
                    
                    bCouldPlayFirstSound = TRUE;
                    bCouldPlaySecondSound = TRUE;
                }
            }
            //printf("pre:%d cur:%d cnt:%d \n", nPreCenterCarDistance, g_fcwsResult.cars[g_fcwsResult.centerCarIdx].dist, nCountDecrease);
        }

        nPreCenterCarDistance = g_fcwsResult.cars[g_fcwsResult.centerCarIdx].dist;

        if( FCWS_COUNT_DECREASE_THR <= nCountDecrease){
            //for replay 2 times same type sound condition
            if(g_fcwsResult.cars[g_fcwsResult.centerCarIdx].dist >= RELEASE_FIRST_BEEP_DISTANCE){
                bCouldPlayFirstSound = TRUE;
            }

            if(g_fcwsResult.cars[g_fcwsResult.centerCarIdx].dist >= RELEASE_SECOND_BEEP_DISTANCE){
                bCouldPlaySecondSound = TRUE;
            }

            if(bCouldPlaySecondSound && SECOND_BEEP_DISTANCE >= nObservedCarDistance[nCountDecrease]){
                //printf("\n\n ============= Sec ==========\n");
                g_eADAS_SOUND = E_ADAS_SOUND_FCWS_BEEP;
                bCouldPlaySecondSound = FALSE;

                //reset
                nPreCenterCarDistance = 0;
                nCountDecrease = 0;
                nCountDown = FCWS_CHECK_RECENT_SUM_FRAME_CNT;
                memset(nObservedCarDistance, 0, sizeof(UINT32)*(FCWS_COUNT_DECREASE_THR + 1));

                //set play Beep event
                sp5kOsEventFlagsSet(&g_EventAdasStartPlayAudio, 1, TX_OR);
            }
            else if( bCouldPlayFirstSound && FIRST_BEEP_DISTANCE >= nObservedCarDistance[nCountDecrease]){
                //printf("\n\n =========  First ==============\n");
                g_eADAS_SOUND = E_ADAS_SOUND_FCWS_BEEP;
                bCouldPlayFirstSound = FALSE;

                //reset
                nPreCenterCarDistance = 0;
                nCountDecrease = 0;
                nCountDown = FCWS_CHECK_RECENT_SUM_FRAME_CNT;
                memset(nObservedCarDistance, 0, sizeof(UINT32)*(FCWS_COUNT_DECREASE_THR + 1));
                
                //set play Beep event
                sp5kOsEventFlagsSet(&g_EventAdasStartPlayAudio, 1, TX_OR);
            }
        }
    }
    else{
        //without center car, reset data
        nPreCenterCarDistance = 0;
        nCountDecrease = 0;
        nCountDown = FCWS_CHECK_RECENT_SUM_FRAME_CNT;
        memset(nObservedCarDistance, 0, sizeof(UINT32)*(FCWS_COUNT_DECREASE_THR + 1));
        
        bCouldPlayFirstSound = TRUE;
        bCouldPlaySecondSound = TRUE;
    }
}
/*--------------------------------------------------------------------------*
* Function name : CovertToOSDCoordinate 
* Function		: Covert To OSD Coordinate
* Return value	: void 
* Parameter1	: int pInX
* Parameter2	: int pInY
* Parameter3	: int pInSize
* Parameter4	: int* pOutX
* Parameter5	: int* pOutY
* Parameter6	: int* pOutSize
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void CovertToOSDCoordinate(int pInX, int pInY, int pInSize, int* pOutX, int* pOutY, int* pOutSize){
    *pOutX = (pInX * ADAS_OSD_W + ADAS_ALGO_CENTER_X) DIVISION_ADAS_ALGO_W; 
    *pOutY = ((pInY * ADAS_OSD_H + ADAS_ALGO_CENTER_Y) DIVISION_ADAS_ALGO_H) + ADAS_OSD_OFFSET_H; 
    *pOutSize = (pInSize * ADAS_OSD_W + ADAS_ALGO_CENTER_X) DIVISION_ADAS_ALGO_W; 
}
/*--------------------------------------------------------------------------*
* Function name : CovertToYUVCoordinate 
* Function		: Covert To YUV Coordinate
* Return value	: void 
* Parameter1	: const UINT32* pInFrameWidth
* Parameter2	: const UINT32* pInFrameHeight
* Parameter3	: int pInX
* Parameter4	: int pInY
* Parameter5	: int pInSize
* Parameter6	: int* pOutX
* Parameter7	: int* pOutY
* Parameter8	: int* pOutSize
* History	    :
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void CovertToYUVCoordinate(const UINT32* pInFrameWidth, const UINT32* pInFrameHeight, int pInX, int pInY, int pInSize, int* pOutX, int* pOutY, int* pOutSize){
    if(!pInFrameWidth || !pInFrameHeight){
        return;
    }

    int nFrameWidth = *pInFrameWidth;
    int nFrameHeight = *pInFrameHeight;

    *pOutX = (pInX * nFrameWidth + ADAS_ALGO_CENTER_X) DIVISION_ADAS_ALGO_W; 
    *pOutY = (pInY * nFrameHeight + ADAS_ALGO_CENTER_Y) DIVISION_ADAS_ALGO_H; 
    *pOutSize = (pInSize * nFrameWidth + ADAS_ALGO_CENTER_X) DIVISION_ADAS_ALGO_H; 
}

