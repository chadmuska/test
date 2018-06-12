/*--------------------------------------------------------------------------*
 *	File name	: app_test_adas_lpd.c
 *	Function	: Test ADAS LPD function
 *	First editor: Weikai Hsu
 *	First date	: 
 *	History		: 	
 *	            : 2018.06.01  Weikai Hsu "Added File Header"
 *	
 *	
 *	
 *--------------------------------------------------------------------------*/
#include "app_test_adas_lpd.h"
#include "app_test_adas_api.h"
#include "app_test_adas.h"
#include "app_test_adas_fcws.h"
#include "sp5k_ftrack_api.h"
#include "sp5k_media_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_utility_api.h"
#include "app_pal.h"
#include "app_osd_api.h"

#include "app_dbg_api.h"
#include <stdio.h>
#include <string.h>
#if defined(HOST_DBG)
#undef HOST_DBG
#define HOST_DBG 0
#endif

extern sp5kPatternTrackCarRes_t g_fcwsResult;
extern unsigned int g_nFcwsCenterX;
extern unsigned int g_nFcwsCenterY;
extern void CovertToOSDCoordinate(int pInX, int pInY, int pInSize, int* pOutX, int* pOutY, int* pOutSize);
extern void CovertToYUVCoordinate(const UINT32* pInFrameWidth, const UINT32* pInFrameHeight, int pInX, int pInY, int pInSize, int* pOutX, int* pOutY, int* pOutSize);

//LPD algo W:H = 1: 0.45
#define LPD_WH_Ratio 0.45f

static sp5kAdasLpdResult_t g_lpdResult;
static UINT32 g_nDstLPDWidth = 160;  //must align 16, for bleing Lpd to yuv frame 
static UINT32 g_nDstLPDHight = 80;   //must align 16, for bleing Lpd to yuv frame 
static void *g_pLpdDstYuvBuf = 0;
static void *g_pPreLpdDstYuvBuf = 0;
static sp5kGfxObj_t g_LpdGfxObj, g_preLpdGfxObj;

static void init();
static void start();
static void stop();

static void AdasLPDResultCallback(UINT32 param, UINT32 param2);


/*--------------------------------------------------------------------------*
* Function name : appLpdStart 
* Function		: lpd function start
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appLpdStart(){
    init();
    start();
}
/*--------------------------------------------------------------------------*
* Function name : appLpdStop 
* Function		: lpd function stop
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appLpdStop(){
    stop();
}
/*--------------------------------------------------------------------------*
* Function name : appIsLpdFound 
* Function		: Is lpd found
* Return value	: bool 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appIsLpdFound(){
    BOOL bIsFound = FALSE;
    (SP5K_LPD_RESULT_FOUND == g_lpdResult.result) ? (bIsFound = TRUE) : (bIsFound = FALSE);
    return bIsFound;
}
/*--------------------------------------------------------------------------*
* Function name : appLpdDrawProductOSDResult 
* Function		: lpd draw product osd result
* Return value	: void 
* Parameter1	: UINT8 eDrawedLayer
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appLpdDrawProductOSDResult(UINT8 eDrawedLayer){
    sp5kGfxPageCapabDesc_t cap;
    sp5kGfxPageCapabilityGet(eDrawedLayer , &cap);

    if(SP5K_LPD_RESULT_FOUND == g_lpdResult.result){
        sp5kGfxObj_t srcObj, dstObj;
        memset(&srcObj, 0, sizeof(sp5kGfxObj_t));
        memset(&dstObj, 0, sizeof(sp5kGfxObj_t));

        if(1 == g_lpdResult.update_flag){
            if(!g_pLpdDstYuvBuf){
                return;
            }

            srcObj.pbuf = g_pLpdDstYuvBuf;
            srcObj.bufW = g_nDstLPDWidth;
            srcObj.bufH = g_nDstLPDHight;
            srcObj.roiX = g_LpdGfxObj.roiX;
            srcObj.roiY = g_LpdGfxObj.roiY;
            srcObj.fmt = SP5K_GFX_FMT_YUV422;
            srcObj.roiW = g_LpdGfxObj.roiW;
            srcObj.roiH = g_LpdGfxObj.roiH;

            dstObj.pbuf = cap.pbuf;
            dstObj.bufW = cap.frmW;
            dstObj.bufH = cap.frmH;
            dstObj.fmt  = cap.fmt;
            dstObj.roiW = g_LpdGfxObj.roiW >> 2; 
            dstObj.roiH = g_LpdGfxObj.roiH >> 2;
            dstObj.roiX = (cap.frmW >> 1) - (dstObj.roiW >> 1);
            dstObj.roiY = (cap.frmH >> 3) + (cap.frmH >> 4);
            sp5kGfxObjectScale(&g_LpdGfxObj, &dstObj);
        }
        else{
            if(!g_pPreLpdDstYuvBuf){
                return;
            }
            
            srcObj.pbuf = g_pPreLpdDstYuvBuf;
            srcObj.bufW = g_nDstLPDWidth;
            srcObj.bufH = g_nDstLPDHight,
                srcObj.roiX = g_preLpdGfxObj.roiX;
            srcObj.roiY = g_preLpdGfxObj.roiY;
            srcObj.fmt = SP5K_GFX_FMT_YUV422;
            srcObj.roiW = g_preLpdGfxObj.roiW;
            srcObj.roiH = g_preLpdGfxObj.roiH;

            dstObj.pbuf = cap.pbuf;
            dstObj.bufW = cap.frmW;
            dstObj.bufH = cap.frmH;
            dstObj.fmt  = cap.fmt;
            dstObj.roiW = g_preLpdGfxObj.roiW >> 2; 
            dstObj.roiH = g_preLpdGfxObj.roiH >> 2; 
            dstObj.roiX = (cap.frmW >> 1) - (dstObj.roiW >> 1);
            dstObj.roiY = (cap.frmH >> 3) + (cap.frmH >> 4);
            sp5kGfxObjectScale(&g_preLpdGfxObj, &dstObj);
        }
    }
}
/*--------------------------------------------------------------------------*
* Function name : debugLpdDrawResult 
* Function		: lpd draw result for debug
* Return value	: void 
* Parameter1	: UINT8 eDrawedLayer
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void debugLpdDrawResult(UINT8 eDrawedLayer){
    sp5kGfxPageCapabDesc_t cap;
    sp5kGfxPageCapabilityGet(eDrawedLayer , &cap);

    sp5kGfxObj_t srcObj, dstObj;
    memset(&srcObj, 0, sizeof(sp5kGfxObj_t));
    memset(&dstObj, 0, sizeof(sp5kGfxObj_t));

    if(SP5K_LPD_RESULT_FOUND == g_lpdResult.result){
        if(1 == g_lpdResult.update_flag){       //update LPD
            if(!g_pLpdDstYuvBuf){
                return;
            }
            
            srcObj.pbuf = g_pLpdDstYuvBuf;
            srcObj.bufW = g_nDstLPDWidth;
            srcObj.bufH = g_nDstLPDHight;
            srcObj.roiX = g_LpdGfxObj.roiX;
            srcObj.roiY = g_LpdGfxObj.roiY;
            srcObj.fmt = SP5K_GFX_FMT_YUV422;
            srcObj.roiW = g_LpdGfxObj.roiW;
            srcObj.roiH = g_LpdGfxObj.roiH;
            
            dstObj.pbuf = cap.pbuf;
            dstObj.bufW = cap.frmW;
            dstObj.bufH = cap.frmH;
            dstObj.fmt  = cap.fmt;
            dstObj.roiW = g_LpdGfxObj.roiW >> 2; 
            dstObj.roiH = g_LpdGfxObj.roiH >> 2;
            dstObj.roiX = (cap.frmW >> 1) - (dstObj.roiW >> 1);
            dstObj.roiY = (cap.frmH >> 3) + (cap.frmH >> 4);
            sp5kGfxObjectScale(&g_LpdGfxObj, &dstObj);
        }
        else{                                   //use previous LPD
            if(!g_pPreLpdDstYuvBuf){
                return;
            }

            srcObj.pbuf = g_pPreLpdDstYuvBuf;
            srcObj.bufW = g_nDstLPDWidth;
            srcObj.bufH = g_nDstLPDHight,
            srcObj.roiX = g_preLpdGfxObj.roiX;
            srcObj.roiY = g_preLpdGfxObj.roiY;
            srcObj.fmt = SP5K_GFX_FMT_YUV422;
            srcObj.roiW = g_preLpdGfxObj.roiW;
            srcObj.roiH = g_preLpdGfxObj.roiH;
            
            dstObj.pbuf = cap.pbuf;
            dstObj.bufW = cap.frmW;
            dstObj.bufH = cap.frmH;
            dstObj.fmt  = cap.fmt;
            dstObj.roiW = g_preLpdGfxObj.roiW >> 2; 
            dstObj.roiH = g_preLpdGfxObj.roiH >> 2; 
            dstObj.roiX = (cap.frmW >> 1) - (dstObj.roiW >> 1);
            dstObj.roiY = (cap.frmH >> 3) + (cap.frmH >> 4);
            sp5kGfxObjectScale(&g_preLpdGfxObj, &dstObj);
        }
    }
}
/*--------------------------------------------------------------------------*
* Function name : lpdDrawResultToRecord 
* Function		: adas lpd draw result to record
* Return value	: void 
* Parameter1	: frameBufInfo_t *pframe
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void lpdDrawResultToRecord(frameBufInfo_t *pframe){
    if(!pframe){
        return;
    }


    if(SP5K_LPD_RESULT_FOUND == g_lpdResult.result){
        sp5kGfxObj_t srcObj;
        sp5kGfxObj_t dstObj;
        memset(&srcObj, 0, sizeof(sp5kGfxObj_t));
        memset(&dstObj, 0, sizeof(sp5kGfxObj_t));
        
        if(1 == g_lpdResult.update_flag){
            if(!g_pLpdDstYuvBuf){
                return;
            }
            srcObj.pbuf = g_pLpdDstYuvBuf;
            srcObj.bufW = g_nDstLPDWidth;
            srcObj.bufH = g_nDstLPDHight;
            srcObj.roiX = g_LpdGfxObj.roiX;
            srcObj.roiY = g_LpdGfxObj.roiY;
            srcObj.fmt = SP5K_GFX_FMT_YUV422;
            srcObj.roiW = g_LpdGfxObj.roiW;
            srcObj.roiH = g_LpdGfxObj.roiH;
            
            dstObj.pbuf = pframe->pbuf;
            dstObj.bufW = pframe->width;
            dstObj.bufH = pframe->height;
            dstObj.fmt  = SP5K_GFX_FMT_YUV422;
            //dstObj.roiW = g_LpdGfxObj.roiW << 2; 
            //dstObj.roiH = g_LpdGfxObj.roiH << 2;
            dstObj.roiW = g_LpdGfxObj.roiW; 
            dstObj.roiH = g_LpdGfxObj.roiH;
            dstObj.roiX = (pframe->width >> 1) - (dstObj.roiW >> 1);
            dstObj.roiY = (pframe->height >> 3) + (pframe->height >> 4);
            sp5kGfxObjectScale(&g_LpdGfxObj, &dstObj);
        }
        else{
            if(!g_pPreLpdDstYuvBuf){
                return;
            }

            srcObj.pbuf = g_pPreLpdDstYuvBuf;
            srcObj.bufW = g_nDstLPDWidth;
            srcObj.bufH = g_nDstLPDHight,
            srcObj.roiX = g_preLpdGfxObj.roiX;
            srcObj.roiY = g_preLpdGfxObj.roiY;
            srcObj.fmt = SP5K_GFX_FMT_YUV422;
            srcObj.roiW = g_preLpdGfxObj.roiW;
            srcObj.roiH = g_preLpdGfxObj.roiH;
            
            dstObj.pbuf = pframe->pbuf;
            dstObj.bufW = pframe->width;
            dstObj.bufH = pframe->height;
            dstObj.fmt  = SP5K_GFX_FMT_YUV422;
            //dstObj.roiW = g_preLpdGfxObj.roiW << 2; 
            //dstObj.roiH = g_preLpdGfxObj.roiH << 2; 
            dstObj.roiW = g_preLpdGfxObj.roiW; 
            dstObj.roiH = g_preLpdGfxObj.roiH; 
            dstObj.roiX = (pframe->width >> 1) - (dstObj.roiW >> 1);
            dstObj.roiY = (pframe->height >> 3) + (pframe->height >> 4);
            sp5kGfxObjectScale(&g_preLpdGfxObj, &dstObj);
        }
    }
}
/*--------------------------------------------------------------------------*
* Function name : init 
* Function		: adas lpd init
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void init(){
    //register LPD result callback
    sp5kAdasCfgSet(SP5K_ADAS_CFG_RESULT_CB, SP5K_ADAS_ALGO_LPD, AdasLPDResultCallback);

    /* Setting sensor buffer */ 
    sp5kAdasInputBufCfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.input = SP5K_ADAS_INPUTBUF_CFG_INPUT_MAIN;
    cfg.nSensor = 1;
    cfg.sen[0].id = SP5K_ADAS_INPUTBUF_CFG_SENSOR_FRONT;
    cfg.sen[0].roi.elem.x = 0;
    cfg.sen[0].roi.elem.y = 0;
    cfg.sen[0].roi.elem.w = ADAS_COORD_HSIZE-1;
    cfg.sen[0].roi.elem.h = ADAS_COORD_VSIZE-1;
    sp5kAdasCfgSet(SP5K_ADAS_CFG_INPUTBUF,&cfg);    
}
/*--------------------------------------------------------------------------*
* Function name : start 
* Function		: adas lpd start
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void start(){
    appFcwsStart(g_nFcwsCenterX, g_nFcwsCenterY);
    adasSetFcws(TRUE);
    
    sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_ENABLE, SP5K_ADAS_ALGO_LPD, 1);	
    adasSetLpd(TRUE);
    hostAdasStart();
}
/*--------------------------------------------------------------------------*
* Function name : stop 
* Function		: adas lpd stop
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void stop(){
    if(g_pLpdDstYuvBuf){
        sp5kYuvBufferFree((void*)g_pLpdDstYuvBuf);
        g_pLpdDstYuvBuf = 0;
    }

    if(g_pPreLpdDstYuvBuf){
        sp5kYuvBufferFree((void*)g_pPreLpdDstYuvBuf);
        g_pPreLpdDstYuvBuf = 0;
    }

    sp5kAdasCfgSet(SP5K_ADAS_CFG_ALGO_ENABLE, SP5K_ADAS_ALGO_LPD, 0);	
    adasSetLpd(FALSE);
    hostAdasStop();
}
/*--------------------------------------------------------------------------*
* Function name : AdasLPDResultCallback 
* Function		: adas lpd result callback
* Return value	: static void 
* Parameter1	: UINT32 param
* Parameter2	: UINT32 param2
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void AdasLPDResultCallback(UINT32 param, UINT32 param2){
	sp5kAdasResultGet(SP5K_ADAS_CFG_ALGO_LPD, &g_lpdResult);
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s AdasLPDResultCallback result [(%d,%d)(%dx%d)]", LPD_STR, g_lpdResult.lp_x, g_lpdResult.lp_y, g_lpdResult.lp_w, g_lpdResult.lp_h);

    //take base fw LPD postion, and copy to g_pLpdDstYuvBuf for blending to yuv frame(rec)
    if(!g_pLpdDstYuvBuf){
        g_pLpdDstYuvBuf = sp5kYuvBufferAlloc(g_nDstLPDWidth, g_nDstLPDHight);
    }

    switch(g_lpdResult.result){
        case SP5K_LPD_RESULT_FOUND:     //Update Flag
            if(1 == g_lpdResult.update_flag){
                //convert lpd algo. cooridanate to 
                int x = (g_lpdResult.lp_x * g_lpdResult.lpdbuf->yuvw + ADAS_ALGO_CENTER_X) DIVISION_ADAS_ALGO_W;
                int y = (g_lpdResult.lp_y * g_lpdResult.lpdbuf->yuvh + ADAS_ALGO_CENTER_Y) DIVISION_ADAS_ALGO_H;
                int w = (g_lpdResult.lp_w * g_lpdResult.lpdbuf->yuvw + ADAS_ALGO_CENTER_X) DIVISION_ADAS_ALGO_W; 
                int h = (g_lpdResult.lp_h * g_lpdResult.lpdbuf->yuvh + ADAS_ALGO_CENTER_Y) DIVISION_ADAS_ALGO_H;
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "\n%s updateFlag:%d debugLpdDrawResult ori(x,y,w,h)(%3d,%3d,%3d,%3d) convert x,y,w,h (%3d,%3d,%3d,%3d)\n", LPD_STR, g_lpdResult.update_flag, g_lpdResult.lp_x, g_lpdResult.lp_y, g_lpdResult.lp_w, g_lpdResult.lp_h, x, y, w, h);
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s %s frame(%d,%d) result f:(%d,%d,%d,%d)] -> result screen(%d,%d,%d,%d)] DST OSD (w,h)(%d,%d), DST LPD(w.h)(%d,%d)]\n", 
                                                LPD_STR,
                                                __FUNCTION__,
                                                g_lpdResult.lpdbuf->yuvw, g_lpdResult.lpdbuf->yuvh,
                                                g_lpdResult.lp_x,g_lpdResult.lp_y,
                                                g_lpdResult.lp_w,g_lpdResult.lp_h,
                                                x, y, w, h,
                                                PRODUCT_OSD_W, PRODUCT_OSD_H,
                                                (g_lpdResult.lp_w * PRODUCT_OSD_W + ADAS_ALGO_CENTER_X) DIVISION_ADAS_ALGO_W,
                                                (g_lpdResult.lp_h * PRODUCT_OSD_H + ADAS_ALGO_CENTER_Y) DIVISION_ADAS_ALGO_H)
                    
                if( 0 == g_lpdResult.lp_w){
                    x = y = w = h = 0;
                    return;
                }

                sp5kGfxObj_t srcObj;
                srcObj.pbuf = g_lpdResult.lpdbuf->yuvaddr;
                srcObj.bufW = g_lpdResult.lpdbuf->yuvw;
                srcObj.bufH = g_lpdResult.lpdbuf->yuvh,
                srcObj.roiX = x;
                srcObj.roiY = y;
                srcObj.fmt = SP5K_GFX_FMT_YUV422;
                srcObj.roiW = w;
                srcObj.roiH = h;
            
                //Copy lpd from basefw yuv buffer to product lpd yuv bufffer
                g_LpdGfxObj.pbuf = g_pLpdDstYuvBuf;
                g_LpdGfxObj.bufW = g_nDstLPDWidth;
                g_LpdGfxObj.bufH = g_nDstLPDHight;
                g_LpdGfxObj.fmt  = SP5K_GFX_FMT_YUV422;
                g_LpdGfxObj.roiX = 0;
                g_LpdGfxObj.roiY = 0;
                g_LpdGfxObj.roiW = g_nDstLPDWidth; 
                //g_LpdGfxObj.roiH = g_nDstLPDHight;
                g_LpdGfxObj.roiH = (UINT32)(LPD_WH_Ratio * g_nDstLPDWidth);
                sp5kGfxObjectScale(&srcObj, &g_LpdGfxObj);

                //reserve current lpd for paste previous lpd, when update flag is 0
                if(!g_pPreLpdDstYuvBuf){
                    g_pPreLpdDstYuvBuf = sp5kYuvBufferAlloc(g_nDstLPDWidth, g_nDstLPDHight);
                }

                g_preLpdGfxObj.pbuf = g_pPreLpdDstYuvBuf;
                g_preLpdGfxObj.bufW = g_LpdGfxObj.bufW;
                g_preLpdGfxObj.bufH = g_LpdGfxObj.bufH;
                g_preLpdGfxObj.fmt  = SP5K_GFX_FMT_YUV422;
                g_preLpdGfxObj.roiX = g_LpdGfxObj.roiX;
                g_preLpdGfxObj.roiY = g_LpdGfxObj.roiY;
                g_preLpdGfxObj.roiW = g_LpdGfxObj.roiW;
                g_preLpdGfxObj.roiH = g_LpdGfxObj.roiH;

                //save previous lpd dstObj to preDstObj
                sp5kGfxObjectCopy(&g_LpdGfxObj, &g_preLpdGfxObj);
            }
            break;
        
        case SP5K_LPD_RESULT_NOT_FOUND:            
        case SP5K_LPD_RESULT_LESS_THAN_SIX_DIGITS:
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s %s LPD Not Found", LPD_STR, __FUNCTION__);
            break;
        default:
            break;
    }
}

