/**************************************************************************
 *
 *       Copyright (c) 2015-2018 by iCatch Technology, Inc.
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
#include "app_com_api.h"
#include "app_com_def.h"

#include "app_General.h"
#include "api/sp5k_utility_api.h"

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
enum {
    URGENT_NO = 0,
    URGENT_ST1,
    URGENT_ST2,
    URGENT_STBOTH = URGENT_ST1|URGENT_ST2
};

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef UINT32 (*pfpappURGENT)(
	UINT32 streamId,        /* nth stream, from 0 */
	UINT32 fid,             /* frame id */
	frameBufInfo_t *pframe, /* buffer information */
	UINT8 *pbufdup          /* duplicated buffer address */
);

typedef struct appUrgent_s {
    pfpappURGENT appUrgent1;
    pfpappURGENT appUrgent2;
    UINT8 interval;
    UINT8 enable;
} appUrgent_t;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if SP5K_DIQ_HDR
    extern UINT32 appHDRYuvUrgentCallback(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup);
#endif
    extern UINT32 appVideoRecStampCb(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup);
    extern UINT32 appVideoRecStampCbSub(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup);
    extern UINT32 appVideoRecStampDispCb(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup);
#if SP5K_USB_UVC_SUPPORT
    extern UINT32 appUvcImeTrigCb(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup);
#elif SP5K_SENSOR_DUAL_MODE
    extern UINT32 appRawYuvImeTrigCb(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup);
#endif
extern UINT32 appPIVCb(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup);

#if TEST_ADAS
    extern UINT32 appAdasRecResultCb(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup);
    extern UINT32 appAdasResultDispCb(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup);
#endif
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
appUrgent_t appUrgent[URGENT_TOTNUM] =
{
#if SP5K_DIQ_HDR
    {appHDRYuvUrgentCallback, NULL,                   1, DISABLE},
#endif
    {appVideoRecStampCb,      appVideoRecStampDispCb, 1, DISABLE},
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
    {NULL,                    appVideoRecStampCbSub,  1, DISABLE},
    {appVideoRecStampCb,      appVideoRecStampCbSub,  1, DISABLE},
#endif
    {appPIVCb,      		  NULL,                   1, DISABLE},
#if TEST_ADAS
    {appAdasRecResultCb,      NULL,                   1, DISABLE},
    {              NULL,      appAdasResultDispCb,    1, DISABLE},
    {appAdasRecResultCb,      appAdasResultDispCb,    1, DISABLE},
#endif
};

appUrgent_t appDispUrgent[DISP_URGENT_TOTNUM] =
{
#if SP5K_USB_UVC_SUPPORT
    {appUvcImeTrigCb,      NULL,                   1, DISABLE},
#elif SP5K_SENSOR_DUAL_MODE
    {appRawYuvImeTrigCb,      NULL,                   1, DISABLE},
#endif
};

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/*-------------------------------------------------------------------------
 *  File Name : appUrgentCb
 *------------------------------------------------------------------------*/
UINT32 HostUrgentCb(
    UINT32 streamId,        /* nth stream, from 0 */
    UINT32 fid,             /* frame id */
    frameBufInfo_t *pframe, /* buffer information */
    UINT8 *pbufdup          /* duplicated buffer address */
)
{
    UINT8 i;

    if(streamId == 0)
    {
        for(i=0; i<URGENT_TOTNUM; i++)
        {
            if(appUrgent[i].enable && (appUrgent[i].appUrgent1 != NULL))
                appUrgent[i].appUrgent1(streamId, fid, pframe, pbufdup);
        }
    }
    else if(streamId == 1)
    {
        for(i=0; i<URGENT_TOTNUM; i++)
        {
            if(appUrgent[i].enable && (appUrgent[i].appUrgent2 != NULL))
                appUrgent[i].appUrgent2(streamId, fid, pframe, pbufdup);
        }
    }

    return 0;
}

/*-------------------------------------------------------------------------
 *  File Name : appDispUrgentCb
 *------------------------------------------------------------------------*/
UINT32 HostDispUrgentCb(
    UINT32 streamId,        /* nth stream, from 0 */
    UINT32 fid,             /* frame id */
    frameBufInfo_t *pframe, /* buffer information */
    UINT8 *pbufdup          /* duplicated buffer address */
)
{
    //UINT8 i;

    if(streamId == 0 || streamId == 1)
    {
		UINT8 i;
        for(i=0; i<DISP_URGENT_TOTNUM; i++)
        {
            if(appDispUrgent[i].enable && (appDispUrgent[i].appUrgent1 != NULL))
                appDispUrgent[i].appUrgent1(streamId, fid, pframe, pbufdup);
            }
        }

    return 0;
}

/*-------------------------------------------------------------------------
 *  File Name : appUrgentSet
 *------------------------------------------------------------------------*/
void appUrgentSet(
    UINT8 appUrgentFunc,
    UINT8 enable
)
{
    static UINT8 urgent_st_old = URGENT_NO;
    UINT8 urgent_st = URGENT_NO;
    UINT8 i;
    sp5kPrevUrgentCallback_t urgent_cb[2];

    if(enable)
        appUrgent[appUrgentFunc].enable = ENABLE;
    else
        appUrgent[appUrgentFunc].enable = DISABLE;

    for(i=0; i<URGENT_TOTNUM; i++)
    {
        if(appUrgent[i].enable)
        {
            if(appUrgent[i].appUrgent1 != NULL)
                urgent_st |= URGENT_ST1;
            if(appUrgent[i].appUrgent2 != NULL)
                urgent_st |= URGENT_ST2;
        }
    }

    if(urgent_st != urgent_st_old)
    {
        if(urgent_st == URGENT_ST1)
        {
            urgent_cb[0].fp = HostUrgentCb;
            urgent_cb[1].fp = NULL;
        }
        else if(urgent_st == URGENT_ST2)
        {
            urgent_cb[0].fp = NULL;
            urgent_cb[1].fp = HostUrgentCb;
        }
        else if(urgent_st == URGENT_STBOTH)
        {
            urgent_cb[0].fp = HostUrgentCb;
            urgent_cb[1].fp = HostUrgentCb;
        }
        else
        {
            urgent_cb[0].fp = NULL;
            urgent_cb[1].fp = NULL;
        }
        sp5kVideoUrgentCallbackSet(1, 0, urgent_cb, 2);

        urgent_st_old = urgent_st;
    }
}

/*-------------------------------------------------------------------------
 *  File Name : appDispUrgentSet
 *------------------------------------------------------------------------*/
void appDispUrgentSet(
    UINT8 appUrgentFunc,
    UINT8 enable
)
{
    static UINT8 urgent_st_old = URGENT_NO;
    UINT8 urgent_st = URGENT_NO;
    UINT8 i;
    sp5kPrevUrgentCallback_t urgent_cb[1];

    if(enable)
        appDispUrgent[appUrgentFunc].enable = ENABLE;
    else
        appDispUrgent[appUrgentFunc].enable = DISABLE;

    for(i=0; i<DISP_URGENT_TOTNUM; i++)
    {
        if(appDispUrgent[i].enable)
        {
            if(appDispUrgent[i].appUrgent1 != NULL)
                urgent_st |= URGENT_ST1;
        }
    }

    if(urgent_st != urgent_st_old)
    {
        if(urgent_st == URGENT_ST1)
        {
            urgent_cb[0].fp = HostDispUrgentCb;
        }
        else
        {
            urgent_cb[0].fp = NULL;
        }
        sp5kDispUrgentCallbackSet(1, 0, urgent_cb, 1);

        urgent_st_old = urgent_st;
    }
}

