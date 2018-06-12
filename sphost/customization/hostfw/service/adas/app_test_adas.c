/*--------------------------------------------------------------------------*
 *	File name	: app_test_adas.c
 *	Function	: Test ADAS function
 *	First editor: Weikai Hsu
 *	First date	: 
 *	History		: 	
 *	            : 2018.06.01  Weikai Hsu "Added File Header"
 *	
 *	
 *	
 *--------------------------------------------------------------------------*/
#include "app_test_adas.h"
#include "app_test_adas_api.h"
#include "app_osd.h"
#include "app_osd_api.h"
#include "app_com_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_os_api.h"
#include "sp5k_adas_api.h"
#include "sp5k_media_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_utility_api.h"

#include "app_res_def.h"

#include "app_dbg_api.h"
#include <stdio.h>
#include <string.h>
#if defined(HOST_DBG)
#undef HOST_DBG
#define HOST_DBG 1
#endif

#include "../../service/adas/app_test_adas_ldws.h"
#include "../../service/adas/app_test_adas_fcws.h"
#include "../../service/adas/app_test_adas_lpd.h"
#include "../../service/adas/app_test_adas_rod.h"

#define FCWS_SUM_CARS 3
#define ADAS_SUM_WARNING 2
#define ADAS_THREAD_TIME_SLICE    100
#define ADAS_AUDIO_THREAD_PRIORITY      17       //baseFW ADAS thread priority
#define ADAS_DRAW_STAMP_THREAD_PRIORITY 17

typedef enum{
    ADAS_LDWS = 0,
    ADAS_FCWS,
    ADAS_LPD,
    ADAS_ROD,
    ADAS_FEATURE_MAX,
}eADAS_FEATURE;


//video stamp
typedef enum{
    ADAS_LAYER_FCWS_CARS = (FCWS_SUM_CARS - 1),
    ADAS_LAYER_LDWS_WARNING,
    ADAS_LAYER_FCWS_WARNING,
    ADAS_LAYER_TOTAL
}eADAS_STAMP_LAYER;

typedef enum{
    ADAS_LAYER_FCWS_CARS_W = 64,
    ADAS_LAYER_LDWS_WARNING_W = 128,
    ADAS_LAYER_FCWS_WARNING_W = 128,
    ADAS_LAYER_TOTAL_W
}eADAS_STAMP_LAYER_W;

typedef enum{
    ADAS_LAYER_FCWS_CARS_H = 32,
    ADAS_LAYER_LDWS_WARNING_H = 48,
    ADAS_LAYER_FCWS_WARNING_H = 48,
    ADAS_LAYER_TOTAL_H
}eADAS_STAMP_LAYER_H;


UINT32 g_TimerAdasProductOSD                    = TIMER_NULL;

BOOL g_bAdasFeature[ADAS_FEATURE_MAX]           = {};
SP5K_THREAD *g_pAdasAudioTherad                 = 0;
SP5K_EVENT_FLAGS_GROUP g_EventAdasStartPlayAudio;
eADAS_Sound g_eADAS_SOUND;

static BOOL g_bIsStartADAS      = FALSE;
static int g_nCarSpeed          = 0;          //km/h
static UINT32 videoStampBufW    = 64;         /* >=scaled string width, aligned to 16 */
static UINT32 videoStampBufH    = 32;         /* >=scaled string height, aligned to 16 */
static UINT32 videoStampDrawRatioW  = 0;
static UINT32 videoStampDrawRatioH  = 0;
static UINT8 *videoStampFcwsCarDistanceBuf[FCWS_SUM_CARS] = {};     /* for string buffer */
static UINT8 *videoStampWarningBuf[ADAS_SUM_WARNING] = {};
static sp5kGfxLayer_t layer[ADAS_LAYER_TOTAL];

extern void CovertToYUVCoordinate(const UINT32* pInFrameWidth, const UINT32* pInFrameHeight, int pInX, int pInY, int pInSize, int* pOutX, int* pOutY, int* pOutSize);
extern void *memset(void *address, int value, size_t size);

static void start(const BOOL *pInFeature);
static void stop(const BOOL *pInFeature);
static void debugAdasDrawResult(sp5kGfxPageNum_e eDrawedLayer);
static void demoAdasDrawResult(sp5kGfxPageNum_e eDrawedLayer);
static void threadAdasAudioProc();
static void initStringStamp(BOOL bInitOnce);
static void deleteStringStamp();

static BOOL isAdasLdwsEnabled();
static BOOL isAdasFcwsEnabled();
BOOL isAdasLpdEnabled();
static BOOL isAdasRodEnabled();
void drawLdwsStampOnLayerYUV();
void drawFcwsStampOnLayerYUV(sp5kPatternTrackCarRes_t *pFcwsResult);
void blendLdwsStampToYUV(frameBufInfo_t *pInOutFrame);
void blendFcwsStampToYUV(frameBufInfo_t *pInOutFrame, const sp5kPatternTrackCarRes_t *pInFcwsResult);

/*--------------------------------------------------------------------------*
* Function name : hostAdasStart 
* Function		: adas start
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void hostAdasStart(){
   start(g_bAdasFeature); //start BaseFW ADAS task
}
/*--------------------------------------------------------------------------*
* Function name : hostAdasStop 
* Function		: adas stop
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void hostAdasStop(){
    stop(g_bAdasFeature); //stop BaseFW ADAS task
}
/*--------------------------------------------------------------------------*
* Function name : debugAdasDrawResult 
* Function		: adas draw result for debug
* Return value	: void 
* Parameter1	: sp5kGfxPageNum_e eDrawedLayer
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void debugAdasDrawResult(sp5kGfxPageNum_e eDrawedLayer){
    sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE,	0, 0, 0, 0);     //don' refresh
    sp5kGfxPageClear(LD_FC_ROD_OSD_PAGE, 0);
    sp5kGfxPageClear(LPD_OSD_PAGE, 0);
    
    //debug information
    static char sFeature[512] = {};
    sprintf(sFeature, "%s%s%s%s%s", ADAS_STR, isAdasLdwsEnabled() ? LDWS_STR : "", isAdasFcwsEnabled() ? FCWS_STR : "", isAdasLpdEnabled() ? LPD_STR : "", isAdasRodEnabled() ? ROD_STR : "");
    appOsd_ColorTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR);
    sp5kGfxStringDraw(eDrawedLayer, 0, 10, SP5K_GFX_ALIGN_LEFT, SP5K_GFX_ENCODE_ASCII, (UINT8*)sFeature);

    sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_YELLOW, 0, 0, 0);
    sp5kGfxLineDraw(eDrawedLayer, OSD_DEFAULT_FRAMSIZE_W >> 2, OSD_DEFAULT_FRAMSIZE_H >> 1, OSD_DEFAULT_FRAMSIZE_W - (OSD_DEFAULT_FRAMSIZE_W >> 2) , OSD_DEFAULT_FRAMSIZE_H >> 1);
    sp5kGfxLineDraw(eDrawedLayer, OSD_DEFAULT_FRAMSIZE_W >> 1, OSD_DEFAULT_FRAMSIZE_H >> 2, OSD_DEFAULT_FRAMSIZE_W >> 1, OSD_DEFAULT_FRAMSIZE_H - (OSD_DEFAULT_FRAMSIZE_H >> 2));

    //LDWS
    if(isAdasLdwsEnabled()){
        debugLdwsDrawResult(eDrawedLayer);
    }

    //FCWS
    if(isAdasFcwsEnabled()){
        debugFcwsDrawResult(eDrawedLayer);
    }

    //LPD
    if(isAdasLpdEnabled()){
        debugLpdDrawResult(LPD_OSD_PAGE);
    }

    if(isAdasRodEnabled()){
        debugRodDrawResult(eDrawedLayer);
    }

    sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE,	1, 0, 0, 0);        //refresh
}
/*--------------------------------------------------------------------------*
* Function name : demoAdasDrawResult 
* Function		: demo adas draw result
* Return value	: void 
* Parameter1	: sp5kGfxPageNum_e eDrawedLayer
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void demoAdasDrawResult(sp5kGfxPageNum_e eDrawedLayer){
    sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE,	0, 0, 0, 0);     //don' refresh
    sp5kGfxPageClear(LD_FC_ROD_OSD_PAGE, 0);
    sp5kGfxPageClear(LPD_OSD_PAGE, 0);
    
    //demo
    sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_YELLOW, 0, 0, 0);
    sp5kGfxLineDraw(eDrawedLayer, OSD_DEFAULT_FRAMSIZE_W >> 2, OSD_DEFAULT_FRAMSIZE_H >> 1, OSD_DEFAULT_FRAMSIZE_W - (OSD_DEFAULT_FRAMSIZE_W >> 2) , OSD_DEFAULT_FRAMSIZE_H >> 1);
    sp5kGfxLineDraw(eDrawedLayer, OSD_DEFAULT_FRAMSIZE_W >> 1, OSD_DEFAULT_FRAMSIZE_H >> 2, OSD_DEFAULT_FRAMSIZE_W >> 1, OSD_DEFAULT_FRAMSIZE_H - (OSD_DEFAULT_FRAMSIZE_H >> 2));

    sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0);     //refresh
}

//Product OSD
void appAdasDrawProductOSD(){
    sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE,	0, 0, 0, 0);   //don' refresh
    sp5kGfxPageClear(LD_FC_ROD_OSD_PAGE, 0); 
    sp5kGfxPageClear(LPD_OSD_PAGE, 0);

    //LDWS
    if(isAdasLdwsEnabled()){
        appLdwsDrawProductOSDResult(LD_FC_ROD_OSD_PAGE);
    }

    //FCWS
    if(isAdasFcwsEnabled()){
        appFcwsDrawProductOsdResult(LD_FC_ROD_OSD_PAGE);
    }
    
    //LPD
    if(isAdasLpdEnabled()){
        appLpdDrawProductOSDResult(LPD_OSD_PAGE);
    }

    //ROD
    if(isAdasRodEnabled()){
        appRodDrawProductOSDResult(LD_FC_ROD_OSD_PAGE);
    }
    
    sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0);        //refresh
}
/*--------------------------------------------------------------------------*
* Function name : appAdasRecResultCb 
* Function		: adas rec result
* Return value	: UINT32 
* Parameter1	: UINT32 streamId
* Parameter2	: UINT32 fid
* Parameter3	: frameBufInfo_t *pframe
* Parameter4	: UINT8 *pbufdup
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appAdasRecResultCb(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup){
    //profLogAdd(6, "appAdasRecResultCb Start");
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s %s", ADAS_STR, __FUNCTION__);

#if CDSP_SKIP_DETECTION
    static void *lastframe = NULL;

    if (lastframe == pframe->pbuf) {
        UINT32 elsTime;
        sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elsTime);
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "\n\n[host][warning] The same main buffer comes again @ %dms\nPlease check dram bandwidth!\n", elsTime);
    }
    lastframe = pframe->pbuf;
#endif

    //confirm in the video record mode

	static UINT32 nMode;
	sp5kModeGet(&nMode);
    if(SP5K_MODE_VIDEO_RECORD != nMode){
        return 0;
    }
    else{
        if(0 == fid){       //init once again
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "%s Into Record mode, ADAS init string stamp again", ADAS_STR);
            //for ADAS had started already, then from video preview to video record without restarting ADAS
            if(g_bIsStartADAS || isAdasFcwsEnabled()){
                initStringStamp(FALSE);
                HOST_PROF_LOG_PRINT(LEVEL_INFO, "%s init string stamp done", ADAS_STR);
            }
        }
    }

    //LPD
    if(isAdasLpdEnabled()){
#if TEST_ADAS_DEBUG_REC_RESULT_VIDEO || TEST_ADAS_DEMO_REC_RESULT_VIDEO
        lpdDrawResultToRecord(pframe);
#endif
    }

    //LDWS
    if(isAdasLdwsEnabled()){
#if TEST_ADAS_DEBUG_REC_RESULT_VIDEO
        debugLdwsDrawResultToRecord(pframe);
#elif TEST_ADAS_DEMO_REC_RESULT_VIDEO
        demoLdwsDrawResultToRecord(pframe);
#endif
    }

    //FCWS
    if(isAdasFcwsEnabled()){
#if TEST_ADAS_DEBUG_REC_RESULT_VIDEO
        debugFcwsDrawResultToRecord(pframe);
#elif TEST_ADAS_DEMO_REC_RESULT_VIDEO
        demoFcwsDrawResultToRecord(pframe);
#endif
    }

    return SUCCESS;
}
/*--------------------------------------------------------------------------*
* Function name : appAdasResultDispCb 
* Function		: adas result display
* Return value	: UINT32 
* Parameter1	: UINT32 streamId
* Parameter2	: UINT32 fid
* Parameter3	: frameBufInfo_t *pframe
* Parameter4	: UINT8 *pbufdup
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appAdasResultDispCb(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup){

#if TEST_ADAS_DEBUG_DISPALY_OSD
    debugAdasDrawResult(LD_FC_ROD_OSD_PAGE);
#endif

#if TEST_ADAS_DEMO_OSD
    demoAdasDrawResult(LD_FC_ROD_OSD_PAGE);
#endif

    return SUCCESS;
}
/*--------------------------------------------------------------------------*
* Function name : appSetAdasCarSpeed 
* Function		: set adas car speed
* Return value	: void 
* Parameter1	: const gpsInfo_t *pInGpsInfo
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appSetAdasCarSpeed(const gpsInfo_t *pInGpsInfo){
    if(pInGpsInfo){
        g_nCarSpeed = (int)(pInGpsInfo->vtgInfo.gpvtgSpeed);
    }
}
/*--------------------------------------------------------------------------*
* Function name : adasSetLdws 
* Function		: set adas ldws
* Return value	: void 
* Parameter1	: BOOL bEnable
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void adasSetLdws(BOOL bEnable){
    g_bAdasFeature[ADAS_LDWS] = bEnable;    
}
/*--------------------------------------------------------------------------*
* Function name : adasSetFcws 
* Function		: set adas fcws
* Return value	: void 
* Parameter1	: BOOL bEnable
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void adasSetFcws(BOOL bEnable){
    g_bAdasFeature[ADAS_FCWS] = bEnable;
}
/*--------------------------------------------------------------------------*
* Function name : adasSetLpd 
* Function		: set adas lpd
* Return value	: void 
* Parameter1	: BOOL bEnable
* History	    :
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void adasSetLpd(BOOL bEnable){
    g_bAdasFeature[ADAS_LPD] = bEnable;
}
/*--------------------------------------------------------------------------*
* Function name : adasSetRod 
* Function		: set adas rod
* Return value	: void 
* Parameter1	: BOOL bEnable
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void adasSetRod(BOOL bEnable){
    g_bAdasFeature[ADAS_ROD] = bEnable;
}
/*--------------------------------------------------------------------------*
* Function name : isAdasLdwsEnabled 
* Function		: is Adas Ldws Enabled
* Return value	: static BOOL 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static BOOL isAdasLdwsEnabled(){
    return g_bAdasFeature[ADAS_LDWS];
}
/*--------------------------------------------------------------------------*
* Function name : isAdasFcwsEnabled 
* Function		: is Adas Fcws Enabled
* Return value	: static BOOL 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static BOOL isAdasFcwsEnabled(){
    return g_bAdasFeature[ADAS_FCWS];
}
/*--------------------------------------------------------------------------*
* Function name : isAdasLpdEnabled 
* Function		: is Adas Lpd Enabled
* Return value	: static BOOL 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static BOOL isAdasLpdEnabled(){
    return g_bAdasFeature[ADAS_LPD];
}
/*--------------------------------------------------------------------------*
* Function name : isAdasRodEnabled 
* Function		: is Adas Rod Enabled
* Return value	: static BOOL 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static BOOL isAdasRodEnabled(){
    return g_bAdasFeature[ADAS_ROD];
}
/*--------------------------------------------------------------------------*
* Function name : start 
* Function		: adas start
* Return value	: static void 
* Parameter1	: const BOOL *pInFeature
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void start(const BOOL *pInFeature){
	g_bIsStartADAS = FALSE;
	
    int i = 0;
    for(; i < ADAS_FEATURE_MAX; ++i){
        if( TRUE == *(pInFeature + i) && (i != ADAS_FCWS)){
            g_bIsStartADAS = TRUE;
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "adas feature i:%d true break", i);
            break;
        }
    }

    HOST_PROF_LOG_PRINT(LEVEL_INFO, "%s IsStart ADAS:%s\n", ADAS_STR, g_bIsStartADAS ? "TRUE" : "FALSE");
    g_bIsStartADAS ? sp5kAdasCtrl(SP5K_ADAS_RUN) : sp5kAdasCtrl(SP5K_ADAS_STOP);
/*
    if(g_bIsStartADAS){
        profLogInit(6,0xF000);
    }
*/
    if(g_bIsStartADAS || isAdasFcwsEnabled()){
        //audio
        if(!g_EventAdasStartPlayAudio && !g_pAdasAudioTherad){
            sp5kOsEventFlagsCreate(&g_EventAdasStartPlayAudio, "AdasPlayAudioStartEvent");
            g_pAdasAudioTherad = sp5kOsThreadCreate("AdasAudioThread", threadAdasAudioProc,
                    0,
                    ADAS_AUDIO_THREAD_PRIORITY,
                    ADAS_AUDIO_THREAD_PRIORITY,
                    ADAS_THREAD_TIME_SLICE,
                    TX_AUTO_START);
        }

        //write string in video file
        static UINT32 nMode;
        sp5kModeGet(&nMode);
        switch(nMode){
            case SP5K_MODE_VIDEO_RECORD:
                initStringStamp(FALSE);
                break;
            default:
                //consider from VIDEO_RECORD to other mode, make sure free stamp buffer
                deleteStringStamp();
                break;
        }

        //set Adas draw product osd timer
        if(TIMER_NULL == g_TimerAdasProductOSD){
            g_TimerAdasProductOSD = appTimerSet(200, "TimerAdasProductOSD");        
        }
    }

    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s start e", ADAS_STR);
}
/*--------------------------------------------------------------------------*
* Function name : stop 
* Function		: adas stop
* Return value	: static void 
* Parameter1	: const BOOL *pInFeature
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void stop(const BOOL *pInFeature){
    int i = 0, nSumEnabled = 0;
    for(i = 0, nSumEnabled = 0; i < ADAS_FEATURE_MAX; ++i){
        if( TRUE == *(pInFeature + i) && ( i != ADAS_FCWS)){
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s i:%d", ADAS_STR, i);
            ++nSumEnabled;
        }
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s i:%d nSumEnabled:%d", ADAS_STR, i, nSumEnabled);
    }

    BOOL bIsStopADAS = FALSE;
    if( 0 == nSumEnabled){
        bIsStopADAS = TRUE;
    }

    if(bIsStopADAS){
        sp5kAdasCtrl(SP5K_ADAS_STOP);
    }

    if(bIsStopADAS && !isAdasFcwsEnabled()){
        UINT32 nRet = 0;
        //delete audio play start event
    	if(g_EventAdasStartPlayAudio){
    		nRet = sp5kOsEventFlagsDelete(&g_EventAdasStartPlayAudio);
    		if(0 == nRet){
                g_EventAdasStartPlayAudio= 0;
    		}
    	}

        //close audio thread
        if(g_pAdasAudioTherad){
        	nRet = sp5kOsThreadTerminate(g_pAdasAudioTherad);
        	if (nRet == 0 ) {
        		nRet = sp5kOsThreadDelete(g_pAdasAudioTherad);
        		if ( nRet == 0 ) {
        			if ( g_pAdasAudioTherad != 0 ) {
        				g_pAdasAudioTherad = 0;
        			}
        		}
        	}
    	}

        //don't draw string in the video file when the ADAS stops
        deleteStringStamp();

        //clean page
        sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0);   //don' refresh
        sp5kGfxPageClear(LD_FC_ROD_OSD_PAGE, 0);
        sp5kGfxPageClear(LPD_OSD_PAGE, 0);
        sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0);  //refresh

        appTimerClear(&g_TimerAdasProductOSD);
    }
}

//Play ADAS warning sound
/*--------------------------------------------------------------------------*
* Function name : threadAdasAudioProc 
* Function		: adas audio thread
* Return value	: static void 
* Parameter1	: void
* History	    :
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void threadAdasAudioProc(){
 //   UINT32 nErr;
    UINT32 flag;

    while(1){
		UINT32 nErr;
        nErr = sp5kOsEventFlagsGet(&g_EventAdasStartPlayAudio, 1, TX_OR_CLEAR, &flag, TX_WAIT_FOREVER);
        if(0 == nErr){
            switch(g_eADAS_SOUND){
                case E_ADAS_SOUND_LDWS_BEEP:		//play back gorund sound
                    appSundVolumeSet(3);
                    //sp5kMediaPlayControl( SP5K_MEDIA_PLAY_ABORT, 0, 0 );	//stop play others sound
                    nErr = sp5kResourceFilePlay(SP5K_RES_TYPE_WAV, (char*)RES_WAV_BEEP, NULL);
                    //appPreloadSoundPlay(SOUND_ID_BEEP);
                    break;
                case E_ADAS_SOUND_FCWS_BEEP:
                    appSundVolumeSet(3);
                    nErr = sp5kResourceFilePlay(SP5K_RES_TYPE_WAV, (char*)RES_WAV_BEEP, NULL);
                    nErr = sp5kResourceFilePlay(SP5K_RES_TYPE_WAV, (char*)RES_WAV_BEEP, NULL);
                    break;
                case E_ADAS_SOUND_LEFT:
                case E_ADAS_SOUND_RIGHT:
                    break;
                case E_ADAS_SOUND_ROD_BEEP_1TIMES_PER2SEC:
                    nErr = sp5kResourceFilePlay(SP5K_RES_TYPE_WAV, (char*)RES_WAV_BEEP, NULL);
                    break;
                case E_ADAS_SOUND_ROD_BEEP_1TIMES_PERSEC:
                    nErr = sp5kResourceFilePlay(SP5K_RES_TYPE_WAV, (char*)RES_WAV_BEEP, NULL);
                    break;
                case E_ADAS_SOUND_ROD_BEEP_2TIMES_PERSEC:
                    nErr = sp5kResourceFilePlay(SP5K_RES_TYPE_WAV, (char*)RES_WAV_BEEP, NULL);
                    nErr = sp5kResourceFilePlay(SP5K_RES_TYPE_WAV, (char*)RES_WAV_BEEP, NULL);
                    break;
                default:
                    break;
            }
        }
        sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
    }
}
/*--------------------------------------------------------------------------*
* Function name : initStringStamp 
* Function		: init string stamp
* Return value	: static void 
* Parameter1	: BOOL bInitOnce
* History	    :
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void initStringStamp(BOOL bInitOnce){
    //allocate YUV buffer for drawing string on it

    if(bInitOnce){
        static BOOL bIsInited = FALSE;
        if(!bIsInited){
            int i = 0;
            for(; i < FCWS_SUM_CARS ; ++i){
                if(videoStampFcwsCarDistanceBuf[i]){
                    sp5kYuvBufferFree(videoStampFcwsCarDistanceBuf[i]);
                    videoStampFcwsCarDistanceBuf[i] = 0;
                }
                videoStampFcwsCarDistanceBuf[i] = (UINT8*)sp5kYuvBufferAlloc(videoStampBufW, videoStampBufH);
            } 

            i = 0;
            for(; i < ADAS_SUM_WARNING; ++i){
                if(videoStampWarningBuf[i]){
                    sp5kYuvBufferFree(videoStampWarningBuf[i]);
                    videoStampWarningBuf[i] = 0;
                }
                videoStampWarningBuf[i] = (UINT8*)sp5kYuvBufferAlloc(ADAS_LAYER_LDWS_WARNING_W, ADAS_LAYER_LDWS_WARNING_H);
            } 
            bIsInited = TRUE;
        }
    }
    else{
        int i = 0;
        for(; i < FCWS_SUM_CARS ; ++i){
            if(videoStampFcwsCarDistanceBuf[i]){
                sp5kYuvBufferFree(videoStampFcwsCarDistanceBuf[i]);
                videoStampFcwsCarDistanceBuf[i] = 0;
            }
            videoStampFcwsCarDistanceBuf[i] = (UINT8*)sp5kYuvBufferAlloc(videoStampBufW, videoStampBufH);
        }

        i = 0;
        for(; i < ADAS_SUM_WARNING; ++i){
            if(videoStampWarningBuf[i]){
                sp5kYuvBufferFree(videoStampWarningBuf[i]);
                videoStampWarningBuf[i] = 0;
            }
            videoStampWarningBuf[i] = (UINT8*)sp5kYuvBufferAlloc(ADAS_LAYER_LDWS_WARNING_W, ADAS_LAYER_LDWS_WARNING_H);
        } 
    }


    UINT32 w, h;
    UINT32 basew, baseh;

    /* Get string scale Ratio*/
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_WIDTH, &w);
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &h);

    //fixme, consider different display device
    basew = 320;
    if (w * 9 == h * 16)  /*16:9*/
        baseh = 180;
    else /*4:3*/
        baseh = 240;

    //250?
    videoStampDrawRatioW = 250* w / basew;
    videoStampDrawRatioH = 250* h / baseh;
}

/*--------------------------------------------------------------------------*
* Function name : deleteStringStamp 
* Function		: delete String Stamp
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void deleteStringStamp(){
    //free string stamp buffer
    int i = 0;
    for(; i < FCWS_SUM_CARS ; ++i){
        if(videoStampFcwsCarDistanceBuf[i]){
            sp5kYuvBufferFree(videoStampFcwsCarDistanceBuf[i]);
            videoStampFcwsCarDistanceBuf[i] = 0;
        }
    }

    i = 0;
    for(; i < ADAS_SUM_WARNING; ++i){
        if(videoStampWarningBuf[i]){
            sp5kYuvBufferFree(videoStampWarningBuf[i]);
            videoStampWarningBuf[i] = 0;
        }
    }
}

/*--------------------------------------------------------------------------*
* Function name : drawLdwsStampOnLayerYUV 
* Function		: draw Ldws Stamp On Layer YUV
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void drawLdwsStampOnLayerYUV(){
    memset(&layer[ADAS_LAYER_LDWS_WARNING], 0x0, sizeof(layer));
    layer[ADAS_LAYER_LDWS_WARNING].pbuf        = videoStampWarningBuf[ADAS_LAYER_LDWS_WARNING - FCWS_SUM_CARS];
    layer[ADAS_LAYER_LDWS_WARNING].frmW        = ADAS_LAYER_LDWS_WARNING_W;
    layer[ADAS_LAYER_LDWS_WARNING].frmH        = ADAS_LAYER_LDWS_WARNING_H;
    layer[ADAS_LAYER_LDWS_WARNING].frmRoiX     = 0;
    layer[ADAS_LAYER_LDWS_WARNING].frmRoiY     = 0;
    layer[ADAS_LAYER_LDWS_WARNING].frmRoiW     = layer[ADAS_LAYER_LDWS_WARNING].frmW;
    layer[ADAS_LAYER_LDWS_WARNING].frmRoiH     = layer[ADAS_LAYER_LDWS_WARNING].frmH;
    layer[ADAS_LAYER_LDWS_WARNING].fmt         = SP5K_GFX_FMT_YUV422;
    
    /* fill BK */
    sp5kGfxPageClear((UINT32)&layer[ADAS_LAYER_LDWS_WARNING], 0x800080);      //transparent 
    
    {
        UINT8 colorTbl[4] = {0};
        colorTbl[0] = PAL_WHITE_1;      /* background color */
        colorTbl[1] = PAL_RED;          /* text color */
        colorTbl[2] = PAL_BLACK;        /* outline color */
        colorTbl[3] = 4;    
        sp5kGfxAttrSet(SP5K_GFX_TEXT_COLOR + SP5K_GFX_OBJ_ATTR_BASE, (UINT32)colorTbl, 0, 4, 0);
    }
    
    sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_GFX_STAMP, 0, 0, 0);
    sp5kGfxAttrSet(SP5K_GFX_PIC_SCALE_FACTOR + SP5K_GFX_OBJ_ATTR_BASE, videoStampDrawRatioW, videoStampDrawRatioH, 0, 0);
    char strAsciiBuf[20] = {};
    sprintf(strAsciiBuf, "%s", "LDWS");

    //draw string on yuv layer 
    sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_GFX_FONT_ID_1, 0, 0, 0);
    sp5kGfxStringDraw((UINT32)&layer[ADAS_LAYER_LDWS_WARNING], 0, 0, SP5K_GFX_ALIGN_TOP_LEFT, STR_ENCODE_ASCII, (UINT8*)strAsciiBuf);
}

/*--------------------------------------------------------------------------*
* Function name : drawFcwsStampOnLayerYUV 
* Function		: draw Ldws Stamp On Layer YUV
* Return value	: void 
* Parameter1	: sp5kPatternTrackCarRes_t *pFcwsResult
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void drawFcwsStampOnLayerYUV(sp5kPatternTrackCarRes_t *pFcwsResult){
    //FCWS Car distance
    if(pFcwsResult){
        char strAsciiBuf[20] = {};                /* for storing distance string */
        int i = 0;
        for(; i < pFcwsResult->num; ++i){
                if(pFcwsResult->num > FCWS_SUM_CARS){       //only draw 3 cars
                    break;
                }
                int x = 0, y = 0, size = 0;

                //fixme
                UINT32 w = 1920;
                UINT32 h = 1088;
                CovertToYUVCoordinate(&w, &h, pFcwsResult->cars[i].x, pFcwsResult->cars[i].y, pFcwsResult->cars[i].size, &x, &y, &size);
                //CovertToYUVCoordinate(&(pInOutFrame->width), &(pInOutFrame->height), pFcwsResult->cars[i].x, pFcwsResult->cars[i].y, pFcwsResult->cars[i].size, &x, &y, &size);

                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, " videoStampFcwsCarDistanceBuf[%d] %p\n", i, videoStampFcwsCarDistanceBuf[i]);
                if(!videoStampFcwsCarDistanceBuf[i]){
                    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s %s  videoStampFcwsCarDistanceBuf[%d]:%p", ADAS_STR, __FUNCTION__, i, videoStampFcwsCarDistanceBuf[i]);
                    deleteStringStamp();
                    initStringStamp(FALSE);
                    continue;
                 }

                //Setp 1. draw on yuv layer
                /* setup layer data (YUV422) */
                memset(&layer[i], 0x0, sizeof(layer));
                layer[i].pbuf        = videoStampFcwsCarDistanceBuf[i];
                layer[i].frmW        = videoStampBufW;
                layer[i].frmH        = videoStampBufH;
                layer[i].frmRoiX     = 0;
                layer[i].frmRoiY     = 0;
                layer[i].frmRoiW     = layer[i].frmW;
                layer[i].frmRoiH     = layer[i].frmH;
                layer[i].fmt         = SP5K_GFX_FMT_YUV422;

                /* fill BK */
                sp5kGfxPageClear((UINT32)&layer[i], 0x800080);      //transparent 

                {
                    UINT8 colorTbl[4] = {0};
                    colorTbl[0] = PAL_NOCOLOR;      /* background color */
                    colorTbl[1] = PAL_YELLOW;       /* text color */
                    colorTbl[2] = PAL_BLACK;        /* outline color */
                    colorTbl[3] = 4;    
                    sp5kGfxAttrSet(SP5K_GFX_TEXT_COLOR + SP5K_GFX_OBJ_ATTR_BASE, (UINT32)colorTbl, 0, 4, 0);
                }
                
                sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_GFX_STAMP, 0, 0, 0);
                sp5kGfxAttrSet(SP5K_GFX_PIC_SCALE_FACTOR + SP5K_GFX_OBJ_ATTR_BASE, videoStampDrawRatioW, videoStampDrawRatioH, 0, 0);
                sprintf(strAsciiBuf, "%dM", pFcwsResult->cars[i].dist);

                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, " %s %s car[%d]:%dM\n", ADAS_STR, __FUNCTION__, i, pFcwsResult->cars[i].dist);
                //draw string on yuv layer 
                sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_GFX_FONT_ID_1, 0, 0, 0);
                sp5kGfxStringDraw((UINT32)&layer[i], 0, 0, SP5K_GFX_ALIGN_TOP_LEFT, STR_ENCODE_ASCII, (UINT8*)strAsciiBuf);
        }

#if TEST_ADAS_DEMO_REC_RESULT_VIDEO
        //WARNING
        memset(&layer[ADAS_LAYER_FCWS_WARNING], 0x0, sizeof(layer));
        layer[ADAS_LAYER_FCWS_WARNING].pbuf        = videoStampWarningBuf[ADAS_LAYER_FCWS_WARNING - FCWS_SUM_CARS];
        layer[ADAS_LAYER_FCWS_WARNING].frmW        = ADAS_LAYER_FCWS_WARNING_W;
        layer[ADAS_LAYER_FCWS_WARNING].frmH        = ADAS_LAYER_FCWS_WARNING_H;
        layer[ADAS_LAYER_FCWS_WARNING].frmRoiX     = 0;
        layer[ADAS_LAYER_FCWS_WARNING].frmRoiY     = 0;
        layer[ADAS_LAYER_FCWS_WARNING].frmRoiW     = layer[ADAS_LAYER_FCWS_WARNING].frmW;
        layer[ADAS_LAYER_FCWS_WARNING].frmRoiH     = layer[ADAS_LAYER_FCWS_WARNING].frmH;
        layer[ADAS_LAYER_FCWS_WARNING].fmt         = SP5K_GFX_FMT_YUV422;
        
        /* fill BK */
        sp5kGfxPageClear((UINT32)&layer[ADAS_LAYER_FCWS_WARNING], 0x800080);      //transparent 
        
        {
            UINT8 colorTbl[4] = {0};
            colorTbl[0] = PAL_WHITE_1;      /* background color */
            colorTbl[1] = PAL_YELLOW;       /* text color */
            colorTbl[2] = PAL_BLACK;        /* outline color */
            colorTbl[3] = 4;    
            sp5kGfxAttrSet(SP5K_GFX_TEXT_COLOR + SP5K_GFX_OBJ_ATTR_BASE, (UINT32)colorTbl, 0, 4, 0);
        }
        
        sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_GFX_STAMP, 0, 0, 0);
        sp5kGfxAttrSet(SP5K_GFX_PIC_SCALE_FACTOR + SP5K_GFX_OBJ_ATTR_BASE, videoStampDrawRatioW, videoStampDrawRatioH, 0, 0);
        sprintf(strAsciiBuf, "%s", "FCWS");
        
        //draw string on yuv layer 
        sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_GFX_FONT_ID_1, 0, 0, 0);
        sp5kGfxStringDraw((UINT32)&layer[ADAS_LAYER_FCWS_WARNING], 0, 0, SP5K_GFX_ALIGN_TOP_LEFT, STR_ENCODE_ASCII, (UINT8*)strAsciiBuf);
#endif
    }
}
/*--------------------------------------------------------------------------*
* Function name : blendLdwsStampToYUV 
* Function		: blend Ldws Stamp To YUV
* Return value	: void 
* Parameter1	: frameBufInfo_t *pInOutFrame
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void blendLdwsStampToYUV(frameBufInfo_t *pInOutFrame){
    if(!pInOutFrame){
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s %s  pInOutFrame:%p return", ADAS_STR, __FUNCTION__, pInOutFrame);
        return;
    }

    static sp5kGfxPicture_t srcPic, dstPic;
    if(layer[ADAS_LAYER_LDWS_WARNING].pbuf){
        srcPic.pbuf = layer[ADAS_LAYER_LDWS_WARNING].pbuf;
        srcPic.frmW = ADAS_LAYER_LDWS_WARNING_W;
        srcPic.frmH = ADAS_LAYER_LDWS_WARNING_H;
        srcPic.roiX = 0;
        srcPic.roiY = 0;
        srcPic.roiW = srcPic.frmW;  
        srcPic.roiH = srcPic.frmH;  
        srcPic.fmt  = SP5K_GFX_FMT_YUV422;
    
        dstPic.pbuf = pInOutFrame->pbuf;
        dstPic.frmW = pInOutFrame->width;
        dstPic.frmH = pInOutFrame->height;
        dstPic.roiX = 72;  
        dstPic.roiY = 48;
        dstPic.roiW = ADAS_LAYER_LDWS_WARNING_W;
        dstPic.roiH = ADAS_LAYER_LDWS_WARNING_H;
        dstPic.fmt  = SP5K_GFX_FMT_YUV422;
        sp5kGfxPictureBlend(&dstPic, 128, 1, 0, &srcPic);
    }
}
/*--------------------------------------------------------------------------*
* Function name : blendFcwsStampToYUV 
* Function		: blend Fcws Stamp To YUV
* Return value	: void 
* Parameter1	: frameBufInfo_t *pInOutFrame
* Parameter2	: const sp5kPatternTrackCarRes_t *pInFcwsResult
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void blendFcwsStampToYUV(frameBufInfo_t *pInOutFrame, const sp5kPatternTrackCarRes_t *pInFcwsResult){
    if(!pInOutFrame){
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s %s  pInOutFrame:%p return", ADAS_STR, __FUNCTION__, pInOutFrame);
        return;
    }

    if(pInFcwsResult){
        //blend FCWS car distance
        static sp5kGfxPicture_t srcPic, dstPic;
        int i = 0;
        for(; i < pInFcwsResult->num; ++i){
            if(!layer[i].pbuf){
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s %s layer[%d]:%p videoStampFcwsCarDistanceBuf[%d]:%p continue, don't blending this layer", ADAS_STR, __FUNCTION__, i, layer[i].pbuf, i, videoStampFcwsCarDistanceBuf[i]);
                break;
            }

            int x = 0, y = 0, size = 0;
            CovertToYUVCoordinate(&(pInOutFrame->width), &(pInOutFrame->height), pInFcwsResult->cars[i].x, pInFcwsResult->cars[i].y, pInFcwsResult->cars[i].size, &x, &y, &size);
            //adjust stampe car distance position
            x = (int)( x + (size / 2.f) - (videoStampBufW >> 1));
            y -= videoStampBufH;
            y -= (videoStampBufH >> 1);
            
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s %s videoStampFcwsCarDistanceBuf[%d]:%p", ADAS_STR, __FUNCTION__, i, videoStampFcwsCarDistanceBuf[i]);


            /* stamp yuv buf and video yuv frame buffer blend */
            srcPic.pbuf = layer[i].pbuf;
            srcPic.frmW = videoStampBufW;
            srcPic.frmH = videoStampBufH;
            srcPic.roiX = 0;
            srcPic.roiY = 0;
            srcPic.roiW = srcPic.frmW;  
            srcPic.roiH = srcPic.frmH;  
            srcPic.fmt  = SP5K_GFX_FMT_YUV422;
            dstPic.pbuf = pInOutFrame->pbuf;
            dstPic.frmW = pInOutFrame->width;
            dstPic.frmH = pInOutFrame->height;
            
            if(x%2 != 0){   //roi X,roi Y must align 2, base firmware limitation
                ++x;            
            }
            
            if(y%2 != 0){
                ++y;
            }
           
            dstPic.roiX = x;  
            dstPic.roiY = y;
            dstPic.roiW = videoStampBufW;
            dstPic.roiH = videoStampBufH;
            dstPic.fmt  = SP5K_GFX_FMT_YUV422;

            //check border X
            if( (dstPic.roiX + dstPic.roiW) > (dstPic.frmW)){
                int nOverX = (dstPic.roiX + dstPic.roiW) - (dstPic.frmW);
                dstPic.roiX -=  nOverX;
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "dist X:%d overX:%d\n", dstPic.roiX, nOverX);
            }
            
            //check border Y
            if( (dstPic.roiY + dstPic.roiH) > (dstPic.frmH)){
                int nOverY = (dstPic.roiY + dstPic.roiH) - (dstPic.frmH);
                dstPic.roiY -=  nOverY;
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "dist Y:%d overY:%d\n", dstPic.roiY, nOverY);
            }

            sp5kGfxPictureBlend(&dstPic, 128, 1, 0, &srcPic);
        }

#if TEST_ADAS_DEMO_REC_RESULT_VIDEO
        //warning string
        if(1 == pInFcwsResult->warn){
            if(layer[ADAS_LAYER_FCWS_WARNING].pbuf){
                srcPic.pbuf = layer[ADAS_LAYER_FCWS_WARNING].pbuf;
                srcPic.frmW = ADAS_LAYER_FCWS_WARNING_W;
                srcPic.frmH = ADAS_LAYER_FCWS_WARNING_H;
                srcPic.roiX = 0;
                srcPic.roiY = 0;
                srcPic.roiW = srcPic.frmW;  
                srcPic.roiH = srcPic.frmH;  
                srcPic.fmt  = SP5K_GFX_FMT_YUV422;

                dstPic.pbuf = pInOutFrame->pbuf;
                dstPic.frmW = pInOutFrame->width;
                dstPic.frmH = pInOutFrame->height;
                dstPic.roiX = 64 + ADAS_LAYER_LDWS_WARNING_W;  
                dstPic.roiY = 48;
                dstPic.roiW = ADAS_LAYER_FCWS_WARNING_W;
                dstPic.roiH = ADAS_LAYER_FCWS_WARNING_H;
                dstPic.fmt  = SP5K_GFX_FMT_YUV422;
                sp5kGfxPictureBlend(&dstPic, 128, 1, 0, &srcPic);
            }
        }
#endif
    }    
}
