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
 *	File name    : App_video_uvc_setting.c
 *	Function     : about video uvc setting	
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.05.31	Edward.Lu  "Add description and modify code"
 **************************************************************************/
 
 /*

 The suggestion video fllow.
 1. set video size.
 2. sp5k mode set to "video_privew." (need to allocate buffer)
 3. sp5k mode set to "video_record" as recording.
 4. set media abort to stop. (to stop and add file name to DCF)
 5. set sp5k mode set to "video_preview."

 */

#include "api/sp5k_usbhost_api.h"
#include <linux/videodev2.h>
#include "oss_usbhost.h"
#include <linux/uvcvideo.h>
#include <linux/uvc_icatchtek.h>
#include <lc_slab.h>


#include "sp5k_global_api.h"
#include "sp5k_media_api.h"
#include "sp5k_usbhost_api.h"
#include "sp5k_rsvblk_api.h"

#include "gpio_custom.h"
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_state_ctrl.h"

#include "app_still.h"
#include "app_video.h"
#include "app_video_uvc.h"
#include "app_view_param.h"
#include "app_view_osd.h"
#include "app_res_def.h"
#include "customization/app_init.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define HOST_DBG 0
 
#define SONIC_ID_PRODUCT    0x0210
#define SONIC_ID_VENDOR     0x1B17
#define SONIC_ID_PRODUCT_1  0x64AB
#define SONIC_ID_VENDOR_1   0x0C45
#define SONIC_ID_PRODUCT_2  0x9230
#define SONIC_ID_VENDOR_2   0x05A3

#define IT_ID_PRODUCT       0x2085
#define IT_ID_VENDOR        0x1BCF
#define IT_ID_PRODUCT_1     0x2085
#define IT_ID_VENDOR_1      0x7119
#define IT_ID_PRODUCT_2     0x0B09
#define IT_ID_VENDOR_2      0x1BCF

#define EU_ADDR             0x1005
#define EU_BUF_SZ           0x04
#define SONIC_EU_ID         0x04
#define IT_EU_ID            0x03
#define EU_SEL              0x01
#define EU_DELAY_CNT        25

#define EU_DEBUG			0

#define EVT_PLUG_IN     (1 << 0)
#define EVT_PLUG_OUT    (1 << 1)
#define	EVT_PLUG_ALL    (EVT_PLUG_IN|EVT_PLUG_OUT)
#define EVT_PLUG_CLR(act_evt,evtstate) {act_evt &= ~(evtstate);}

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UvcCamCfg_t CamCfg, *pCamParam=&CamCfg;
UsbCtrl_t UsbCtrl;
struct usbhost_uvc_ops *uvcops;

static struct {
    UINT16 w,h;
    /* UINT32 frate; */
    UINT32 interval;
} uvcsize[UI_VIDEO_SIZE_MAX]={
    { 1280,  720, VIDEO_30_FRAME_RATE, 33},
    { 1280,  720, VIDEO_30_FRAME_RATE, 33},
    { 640,  480, VIDEO_30_FRAME_RATE, 33},
};

static UINT32 delay_tm;

static SP5K_THREAD *pUvcPlugThr;
static SP5K_EVENT_FLAGS_GROUP pUvcPlugEvt;
static UINT8 rearcamverStr[256];

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void* usbhostUvcDevOpen(int devno);
extern int usbhostUvcDevClose(int devno);
extern int usbhostUvcListSupportFmtDevno(int fmt);
extern int usbhostUvcGetCurrentDevno(void);
extern int usbhostUvcSetCurrentDevno(int devno);
extern int usbhostUvcRequestBuf(void* uh, int num);
extern int usbhostUvcQueryBuf(void* uh, int index, void **pbuf, int *len);
extern int usbhostUvcQueueBuf(void* uh, int index);
extern int usbhostUvcDequeueBuf(void* uh, int *index, int *len);
extern int usbhostUvcStreamOn(void* uh);
extern int usbhostUvcStreamOff(void* uh);
extern int usbhostUvcSetCurrentConfigure(void* uh, int fmt, int width, int height, int interval);
extern int usbhostUvcGetCurrentConfigure(void* uh, int *fmt, int *width, int *height, int *interval);
extern int usbhostUvcListSupportConfigure(void* uh);



/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 
SINT32 appUvcCamCfgSet(UINT32 sel,  ...);
SINT32 appUvcCamCfgGet(UINT32 sel,  ...);

/*--------------------------------------------------------------------------*
* Function name    : appUvcCamSizeScan
* Function         : uvc Scan Size
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static UINT32 appUvcCamSizeScan(void)
{
    UINT32 ret = FAIL;

    if(!UsbCtrl.flag)
    {
        CamCfg.devno = appUvcCamCfgSet(UVC_CAM_CFG_SET_LIST_FMT_DEVNO, CamCfg.fmt);

        if (CamCfg.devno&0x1) {
            CamCfg.devno = 0;
        } else if (CamCfg.devno&0x2) {
            CamCfg.devno = 1;
        } else {
            HOST_PROF_LOG_PRINT(LEVEL_ERROR, "uvc: not support mjpg fmt");
            return FAIL;
        }
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: devno:%d",CamCfg.devno);

        UsbCtrl.handle = (void*)appUvcCamCfgSet(UVC_CAM_CFG_SET_DEV_OPEN, CamCfg.devno);
        if (!UsbCtrl.handle) {
            HOST_PROF_LOG_PRINT(LEVEL_ERROR, "uvc: no invalid uvc device");
            appUvcCamCfgSet(UVC_CAM_CFG_SET_DEV_CLOSE, CamCfg.devno);
            return FAIL;
        }

        #if DEFAULT_720P /*default 720p*/
        appUvcHostCfgSet(UVC_HOST_CFG_SZ_IDX, UI_VIDEO_DUAL_SIZE_FHD_720P);
        appUvcHostCfgSet(UVC_HOST_CFG_FRM_INTVAL, uvcsize[UI_VIDEO_DUAL_SIZE_FHD_720P].interval);
        ret = SUCCESS;
        #else
		UINT32 fmt, width, height, interval;
    	SINT8 i;
		
        for(i=(UI_VIDEO_SIZE_MAX-1); i>=0; i--)
        {
            appUvcCamCfgSet(UVC_CAM_CFG_SET_CUR_CFG, UsbCtrl.handle, CamCfg.fmt, uvcsize[i].w, uvcsize[i].h, 10000000/uvcsize[i].interval);
            appUvcCamCfgGet(UVC_CAM_CFG_GET_CUR_CFG, UsbCtrl.handle, &fmt, &width, &height, &interval);

            if(uvcsize[i].w==width && uvcsize[i].h==height)
            {
                appUvcHostCfgSet(UVC_HOST_CFG_SZ_IDX, i);
                appUvcHostCfgSet(UVC_HOST_CFG_FRM_INTVAL, uvcsize[i].interval);
                HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: support %d:%d x %d\n",ret ,uvcsize[i].w, uvcsize[i].h);
  		  if(i  == UI_VIDEO_DUAL_SIZE_FHD_VGA)
  		      pViewParam->videoSize = UI_VIDEO_DUAL_SIZE_FHD_VGA;
		  ret = SUCCESS;
  		  break;
            }
            else
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: not support %d:%d x %d",ret ,uvcsize[i].w, uvcsize[i].h);
        }
        #endif

        appUvcCamCfgSet(UVC_CAM_CFG_SET_DEV_CLOSE, CamCfg.devno);
        (ret==SUCCESS)?(UsbCtrl.flag = 1):(UsbCtrl.flag = 0);
    }

    return ret;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcModeCfgSet
* Function         : Uvc Mode Config Set
* Return value	   : uvc_save_done (UINT8 0:save done, 1:no save done)
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUvcModeCfgSet(UINT8 plug, UINT8 pvsrc)
{
	UINT32 cfgPv2ndManually  = 2 << 8;
	UINT32 cfgdisplayShowPv2nd = 1;
	UINT32 video2ndSizeCfg = 1U<<31;/*weikai modify 180504*/
	UINT32 encW, encH;
	UINT32 yuvW, yuvH;

	#if 0
	CamCfg.host.width = uvcsize[UI_VIDEO_DUAL_SIZE_FHD_720P].w;
	CamCfg.host.height= uvcsize[UI_VIDEO_DUAL_SIZE_FHD_720P].h;
	#else
	CamCfg.host.width = uvcsize[CamCfg.host.szidx].w;
	CamCfg.host.height= uvcsize[CamCfg.host.szidx].h;
	#endif
	HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: mode cfg - plug: %d" ,plug);

	#if REDUCE_DRAM_BUF
	sp5kModeCfgSet(SP5K_MODE_CFG_VIDEO_PV_SIZE, MAIN_PATH_W_SIZE, MAIN_PATH_H_SIZE, UVC_RING_BUFS);
	#endif

	sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_WIDTH, &encW);
	sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_HEIGHT, &encH);
	yuvW = CamCfg.host.width;
	yuvH = CamCfg.host.height;
	if (encW * yuvH != encH * yuvW) {	/* ratio should be equal */
		yuvH = yuvW * encH / encW;
	}

	sp5kModeCfgSet(SP5K_MODE_CFG_VIDEO_PV_SIZE,
					video2ndSizeCfg |yuvW, video2ndSizeCfg |yuvH, UVC_RING_BUFS);
	sp5kModeCfgSet(SP5K_MODE_CFG_PV_2ND_PATH_ENABLE, cfgPv2ndManually|cfgdisplayShowPv2nd );

	printf("UVC: yuv=%ux%u, enc=%ux%u\n", yuvW, yuvH, encW, encH);

	appUvcImeCbReg();

	UINT32 w_factor=(-1), h_factor=(-1); //If W-factor and H-factor are -1, display playback size will use still preview size.
    // Set preview display sourceas playback. 
    UINT32 setting=(pvsrc<<24)|(appUvcFlipModeGet()<<12)|(3<<8)|1;
	sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_SCALE_FACTOR, w_factor, h_factor, setting);

	sp5kModeCfgSet(SP5K_MODE_CFG_PV_UVC_DEVICE, ENABLE); // enable UVC preview
	sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: mode cfg - plug: e");
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcCamStartDelay
* Function         : Uvc Start Delay
* Return value	   : void
* Parameter1	   : mode (UINT32 SP5K_MODE_VIDEO_RECORD)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUvcCamStartDelay(UINT32 mode)
{
    struct icatchtek_uvc_handle* handle = (struct icatchtek_uvc_handle* )UsbCtrl.handle;
    /*struct uvc_video_chain *chain = handle->chain;*/
    
    if(handle==NULL)
    {
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: %s %d fail.", __FUNCTION__, __LINE__);
        return;
    }

    /* Find the extension unit. */
    if(appUvcHostCfgGet(UVC_HOST_CFG_PLUG_IN) && mode==SP5K_MODE_VIDEO_RECORD)
    {
		struct uvc_video_chain *chain = handle->chain;
		UINT16 pid, vid;
        pid = chain->dev->udev->descriptor.idProduct;
        vid = chain->dev->udev->descriptor.idVendor;

        if((pid==SONIC_ID_PRODUCT && vid==SONIC_ID_VENDOR) ||
           (pid==SONIC_ID_PRODUCT_1 && vid==SONIC_ID_VENDOR_1) ||
           (pid==SONIC_ID_PRODUCT_2 && vid==SONIC_ID_VENDOR_2))
        {
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: cam start delay");
            sp5kTimeDelay(SP5K_TIME_DELAY_1MS, UVC_VIDEO_DELAY*33);
        }
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcCamPlugCtrl
* Function         : Uvc Plug Ctrl
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : sel (UINT8 sel < UVC_PLUG_MAX)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appUvcCamPlugCtrl(UINT8 sel)
{
    UINT32 ret = SUCCESS;

    if(sel >= UVC_PLUG_MAX) return FAIL;

    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: plug control %d", sel);
    appBtnDisable(BTN_ALL);

    if(sel == UVC_PLUG_IN)
    {
		printf("\033[32m^%s: CamCfg.host.width=%d,CamCfg.host.height=%d^\033[0m\n",__FUNCTION__,CamCfg.host.width,CamCfg.host.height);
	 	ret = sp5kUsbhostControl(SP5K_USBHOST_CTRL_UVC_DEVICE_OPEN, CamCfg.host.width, CamCfg.host.height, 30);

	    if(ret == FAIL)
	    	HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: uvc open fail");

		sp5kUsbhostControl(SP5K_USBHOST_CTRL_UVC_DEVICE_ATTACH);

    }
    else if(sel == UVC_PLUG_OUT)
    {
        UsbCtrl.handle = NULL;

		sp5kUsbhostControl(SP5K_USBHOST_CTRL_UVC_DEVICE_DETACH);

		ret = sp5kUsbhostControl(SP5K_USBHOST_CTRL_UVC_DEVICE_CLOSE);

		if(ret == FAIL)
			HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: uvc close again or fail");
    }

    if(sel == UVC_PLUG_IN)
    appUvcHostCfgSet(UVC_HOST_CFG_PLUG_COMPLETE, UVC_PLUG_COMPLETE);

    appBtnEnable(BTN_ALL);

    return ret;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcEnumExtensionUnit
* Function         : Uvc Enum Extension Unit
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*                  : 2018.06.07  Weikai Hsu  "cppcheck uvc_entity *entity no init"
*--------------------------------------------------------------------------*/

UINT32 appUvcEnumExtensionUnit(void)
{
    UINT32 ret = 0, mode = 0;
    struct v4l2_control ctrl;
    /*static UINT8 pre_pwr_freq = 0xff;*/

    sp5kModeGetPeek(&mode);

    if(mode!=SP5K_MODE_VIDEO_PREVIEW && mode!=SP5K_MODE_VIDEO_RECORD)
        return SUCCESS;

    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: %s %x %d %d s",__FUNCTION__ , mode, appUvcRingBufDone(), appUvcHostCfgGet(UVC_HOST_CFG_PLUG_IN));

    (!appUvcHostCfgGet(UVC_HOST_CFG_PLUG_COMPLETE))?(delay_tm=0):(delay_tm++);

	#if UVC_NO_WAIT_FLAG
    if(delay_tm>EU_DELAY_CNT && appUvcHostCfgGet(UVC_HOST_CFG_PLUG_IN))
	#else
    if(delay_tm>EU_DELAY_CNT && appUvcRingBufDone()>UVC_RING_BUFS<<1 && appUvcHostCfgGet(UVC_HOST_CFG_PLUG_IN))
	#endif
    {
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: Discovering xu controls");
        delay_tm = 0;
        struct icatchtek_uvc_handle* handle = (struct icatchtek_uvc_handle* )UsbCtrl.handle;
        UINT16 len;
        UINT8 info = 0, bNumControls, bControlSize;
		/*#if EU_DEBUG
	    UINT8 *data = NULL;
		#else
		UINT8 data[EU_BUF_SZ];
		#endif*/
//        struct uvc_entity *entity;
        

        if(handle==NULL)
        {
            HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: %s %d fail.ret:%d", __FUNCTION__, __LINE__, ret);
            return FAIL;
        }

        /* Find the extension unit. */
        if(appUvcHostCfgGet(UVC_HOST_CFG_PLUG_IN))
        {
			UINT16 vid;
			UINT16 pid;
			UINT32 sel;
			struct uvc_video_chain *chain = handle->chain;
            pid = chain->dev->udev->descriptor.idProduct;
            vid = chain->dev->udev->descriptor.idVendor;
            if((pid!=SONIC_ID_PRODUCT && vid!=SONIC_ID_VENDOR) &&
               (pid!=SONIC_ID_PRODUCT_1 && vid!=SONIC_ID_VENDOR_1) &&
               (pid!=SONIC_ID_PRODUCT_2 && vid!=SONIC_ID_VENDOR_2) &&
               (pid!=IT_ID_PRODUCT && vid!=IT_ID_VENDOR) &&
               (pid!=IT_ID_PRODUCT_1 && vid!=IT_ID_VENDOR_1) &&
               (pid!=IT_ID_PRODUCT_2 && vid!=IT_ID_VENDOR_2))
            {
                HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: %s %d not support uvc camera", __FUNCTION__, __LINE__);
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: pid: %x vid: %x" ,pid ,vid);
                return FAIL;
            }

			/*strbuf must 256byte,len is real length*/
			int RearFWrevlen;
			RearFWrevlen = sp5kUsbHostInfoGet(SP5K_USBHOST_STR_DESC, handle, chain->dev->udev->descriptor.iManufacturer, rearcamverStr);
			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: RearFWrevlen: %x" ,RearFWrevlen);
			//weikai modify for cppcheck 180607
			extern struct uvc_entity *entity;
	    	list_for_each_entry(entity, &chain->entities, chain)
	        {
				UINT8 pre_pwr_freq=0xff;
	            if(UVC_ENTITY_TYPE(entity) == UVC_VC_PROCESSING_UNIT && pre_pwr_freq!=pUiSetting->lightFrequency)
	            {
	                /*pre_pwr_freq = pUiSetting->lightFrequency;*/
	                ctrl.id = V4L2_CID_POWER_LINE_FREQUENCY;

	                if((pid==IT_ID_PRODUCT && vid==IT_ID_VENDOR) ||
						(pid==IT_ID_PRODUCT_1 && vid==IT_ID_VENDOR_1) ||
						(pid==IT_ID_PRODUCT_2 && vid==IT_ID_VENDOR_2))
	                {
	                    (!pUiSetting->lightFrequency)?
	                    (ctrl.value=V4L2_CID_POWER_LINE_FREQUENCY_50HZ):
	                    (ctrl.value=V4L2_CID_POWER_LINE_FREQUENCY_60HZ);
	                }
	                else
	                {
	                    (pUiSetting->lightFrequency)?
	                    (ctrl.value=V4L2_CID_POWER_LINE_FREQUENCY_50HZ):
	                    (ctrl.value=V4L2_CID_POWER_LINE_FREQUENCY_60HZ);
	                }

	                ret = icatchtek_uvc_ops(handle, ICATCHTEK_UVC_S_CTRL, &ctrl);

	                HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: set pwr freq line %d %s"
	                        ,pUiSetting->lightFrequency
	                        ,(ctrl.value==V4L2_CID_POWER_LINE_FREQUENCY_50HZ)?"50HZ":"60HZ");
	            }
	            else if(UVC_ENTITY_TYPE(entity) == UVC_VC_EXTENSION_UNIT)
	            {
					UINT8 guid[16];
					UINT8 bmControls[16];
					UINT32 id;
	            	id = entity->id;
	                ret =  uvc_get_extension_unit_desc(handle, id, guid, &bNumControls, &bControlSize, bmControls);

	                if (!ret) {
	                    //printf("guidExtensionCode:"UVC_GUID_FORMAT"\n", UVC_GUID_ARGS(guid));
	                    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: bNumControls:%d bControlSize:%d", bNumControls, bControlSize);
						UINT32 i;
	                    for (i=0; i<bControlSize;i++)
	                            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: bmControls: 0x%x", bmControls[i]);
	                }
					else{
						HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: %s %d fail.ret:%d\n", __FUNCTION__, __LINE__, ret);
						return FAIL;
					}

					if(guid[3]==0x63 && guid[2]==0x61 && guid[1]==0x06 && guid[0]==0x82)
						id = IT_EU_ID;
					else if(guid[3]==0x28 && guid[2]==0x12 && guid[1]==0xAE && guid[0]==0x3F)
						id = SONIC_EU_ID;

					ret = uvc_extension_unit_get_info(handle, id, sel, &info);
					if (ret) {
						HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: %s %d fail.ret:%d", __FUNCTION__, __LINE__, ret);
						return FAIL;
					}

					if (info & UVC_CONTROL_CAP_SET)
					{
						#if EU_DEBUG
						UINT8 *data = NULL;
						#else
						UINT8 data[EU_BUF_SZ];
						#endif
						ret = uvc_extension_unit_get_len(handle, id, sel, &len);
						if (ret) {
							HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: %s %d fail.ret:%d", __FUNCTION__, __LINE__, ret);
							return FAIL;
						}

						#if EU_DEBUG
						data = kmalloc(len, KMEM_POOL_USB);
				        if(data==NULL)
				        {
					        HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: %s %d fail.ret:%d", __FUNCTION__, __LINE__, ret);
							return FAIL;
				        }
						#else
						if(len!=EU_BUF_SZ)
						{
							HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: <lester> debug other len: id=%u sel=%u len=%d\n", entity->id, sel, len);
							return FAIL;
						}
						len=EU_BUF_SZ;
						#endif

		                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc:**********************************************************");
		                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: selector:%d info:0x%x len:0x%x",sel ,info ,len);
		                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: id=%u sel=%u len=%d\n", entity->id, sel, len);

				        if(len==EU_BUF_SZ)
				        {
		                    data[0] = EU_ADDR & 0xFF;
		                    data[1] = (EU_ADDR & 0xFF00)>>8;
			                data[2] = 0x00;
			                data[3] = 0xFF;
						}
						else
						{
							HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: <lester> debug other len: id=%u sel=%u len=%d\n", entity->id, sel, len);
							return FAIL;
						}

	                	ret = uvc_extension_unit_set_cur(handle, id, sel, len, data);

						#if EU_DEBUG
						if(data != NULL) {kfree(data); data=NULL; }
						#endif

		                if (ret) {
		                    HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: %s %d fail.ret:%d", __FUNCTION__, __LINE__, ret);
		                    return FAIL;
		                }
		                else
		                {
							ret = uvc_extension_unit_get_info(handle, id, sel, &info);
			                if (ret) {
			                    HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: %s %d fail.ret:%d", __FUNCTION__, __LINE__, ret);
			                    return FAIL;
			                }

							if (info & UVC_CONTROL_CAP_GET)
							{
								ret = uvc_extension_unit_get_len(handle, id, sel, &len);
				              	if (ret) {
				                    HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: %s %d fail.ret:%d", __FUNCTION__, __LINE__, ret);
				                    return FAIL;
				                }

								#if EU_DEBUG
								data = kmalloc(len, KMEM_POOL_USB);
								if(data==NULL)
								{
									HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: %s %d fail.ret:%d", __FUNCTION__, __LINE__, ret);
									return FAIL;
								}
								#else
								if(len!=EU_BUF_SZ)
								{
									HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: <lester> debug other len: id=%u sel=%u len=%d\n", entity->id, sel, len);
									return FAIL;
								}
								len=EU_BUF_SZ;
								#endif

			                    ret = uvc_extension_unit_get_cur(handle, id, sel, len, data);
			                    if (ret) {
			                        HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: %s %d fail.ret:%d\n", __FUNCTION__, __LINE__, ret);
									#if EU_DEBUG
									if(data != NULL) {kfree(data); data=NULL; }
									#endif
			                        return FAIL;
								}
								else
								{
									if(len!=EU_BUF_SZ)
									{
										printf("\tcur:");
										UINT32 i;
										for (i=0; i<len; i++)
											printf("0x%x ", data[i]);
										printf("\n");
									}
									else
									{
				                        appUvcCarModeSet(data[2]&0x01);

				                        if((data[2]&0x01) == 0x01)
				                            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: car back %x", data[2]);
				                    }
									#if EU_DEBUG
									if(data != NULL) {kfree(data); data=NULL; }
									#endif

		                    	}
							}
		                }
	                }
	        	}
        	}/*end list_for_each_entry*/
        }/*end UVC_HOST_CFG_PLUG_IN*/
    }
    return ret;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcCamCfgSet
* Function         : Uvc config set
* Return value	   : result (SINT32 SUCCESS/FAIL)
* Parameter1	   : sel (UINT32 sel < UVC_CAM_CFG_MAX)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

SINT32 appUvcCamCfgSet(UINT32 sel,	...)
{
    SINT32 ret = FAIL;
	UINT32 paraTot;
	UINT32 para1, para2, para3, para4 , para5;
    /*para5 = para4 = para3 = para2 = para1 = 0;*/
    /*para5 = para5;*/

	va_list args;

	switch(sel)
    {
        case UVC_CAM_CFG_SET_DEV_OPEN:      paraTot = 1; break;
        case UVC_CAM_CFG_SET_DEV_CLOSE:     paraTot = 1; break;
        case UVC_CAM_CFG_SET_LIST_FMT_DEVNO: paraTot = 1; break;
        case UVC_CAM_CFG_SET_CUR_DEVNO:     paraTot = 1; break;
        case UVC_CAM_CFG_SET_REQEST_BUF:    paraTot = 2; break;
        case UVC_CAM_CFG_SET_QUERY_BUF:     paraTot = 4; break;
        case UVC_CAM_CFG_SET_QUEUE_BUF:     paraTot = 2; break;
        case UVC_CAM_CFG_SET_DEQUEUE_BUF:   paraTot = 3; break;
        case UVC_CAM_CFG_SET_STREAM_ON:     paraTot = 1; break;
        case UVC_CAM_CFG_SET_STREAM_OFF:    paraTot = 1; break;
        case UVC_CAM_CFG_SET_CUR_CFG:       paraTot = 5; break;
        case UVC_CAM_CFG_SET_LIST_CFG:      paraTot = 1; break;
    	default: paraTot = 0; break;
	}

    if(paraTot)
    {
    	va_start(args, sel);
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: cam cfg set sel=%d", sel);
    	para1 = va_arg(args, UINT32);
    	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: cam cfg para1=%d(0x%X)", para1, para1);
    	if ( paraTot >= 2 ) {
    		para2 = va_arg(args, UINT32);
        	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: cam cfg para2=%d(0x%X)", para2, para2);
    		if ( paraTot >= 3 ) {
    			para3 = va_arg(args, UINT32);
    			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: cam cfg para3=%d(0x%X)", para3, para3);
    			if ( paraTot >= 4 ) {
    				para4 = va_arg(args, UINT32);
    				HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: cam cfg para4=%d(0x%X)", para4, para4);
        			if ( paraTot >= 5 ) {
        				para4 = va_arg(args, UINT32);
        				HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: cam cfg para4=%d(0x%X)", para5, para5);
        			}
    			}
    		}
    	}
    }

	switch(sel)
    {
        case UVC_CAM_CFG_SET_DEV_OPEN:
            ret = (SINT32)usbhostUvcDevOpen(para1);
            break;

        case UVC_CAM_CFG_SET_DEV_CLOSE:
            ret = usbhostUvcDevClose(para1);
            break;

        case UVC_CAM_CFG_SET_LIST_FMT_DEVNO:
            ret = usbhostUvcListSupportFmtDevno(para1);
            break;

        case UVC_CAM_CFG_SET_CUR_DEVNO:
            ret = usbhostUvcSetCurrentDevno(para1);
            break;

        case UVC_CAM_CFG_SET_REQEST_BUF:
            ret = usbhostUvcRequestBuf((void*)para1, para2);
            break;

        case UVC_CAM_CFG_SET_QUERY_BUF:
            ret = usbhostUvcQueryBuf((void*)para1, para2, (void**)para3, (int*)para4);
            break;

        case UVC_CAM_CFG_SET_QUEUE_BUF:
            ret = usbhostUvcQueueBuf((void*)para1, para2);
            break;

        case UVC_CAM_CFG_SET_DEQUEUE_BUF:
            ret = usbhostUvcDequeueBuf((void*)para1, (int *)para2, (int *)para3);
            break;

        case UVC_CAM_CFG_SET_STREAM_ON:
            ret = usbhostUvcStreamOn((void*)para1);
            break;

        case UVC_CAM_CFG_SET_STREAM_OFF:
            ret = usbhostUvcStreamOff((void*)para1);
            break;

        case UVC_CAM_CFG_SET_CUR_CFG:
            ret = usbhostUvcSetCurrentConfigure((void*)para1, para2, para3, para4, para5);
            break;

        case UVC_CAM_CFG_SET_LIST_CFG:
            ret = usbhostUvcListSupportConfigure((void*)para1);
            break;

    	default:
    		ret = FAIL;
	}

	if(paraTot) va_end(args);

	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: cam cfg set sel:%d 0x%x",sel ,ret);
    return ret;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcCamCfgGet
* Function         : get uvc config
* Return value	   : result (SINT32 SUCCESS/FAIL)
* Parameter1	   : sel (UINT32 UVC_CAM_CFG_GET_CUR_DEVNO/UVC_CAM_CFG_GET_CUR_CFG)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

SINT32 appUvcCamCfgGet(UINT32 sel,	...)
{
    SINT32 ret = FAIL;
	UINT32 paraTot;
	UINT32 *ppara1, *ppara2, *ppara3, *ppara4, *ppara5;
    ppara5 = ppara4 = ppara3 = ppara2 = ppara1 = NULL;

	va_list args;

	switch(sel)
    {
        case UVC_CAM_CFG_GET_CUR_DEVNO: paraTot = 0; break;
        case UVC_CAM_CFG_GET_CUR_CFG:   paraTot = 5; break;
    	default:                        paraTot = 0; break;
	}

	if(paraTot)
    {
	    va_start(args, sel);
    	ppara1 = va_arg(args, UINT32*);
    	if (paraTot >= 2) {
    		ppara2 = va_arg(args, UINT32*);
    		if (paraTot >= 3) {
    		    ppara3 = va_arg(args, UINT32*);
    			if (paraTot >= 4) {
    				ppara4 = va_arg(args, UINT32*);
                    if (paraTot >= 5) {
        				ppara5 = va_arg(args, UINT32*);
        			}
    			}
    		}
    	}
	}

	switch(sel) {
        case UVC_CAM_CFG_GET_CUR_DEVNO:
            ret = usbhostUvcGetCurrentDevno();
            break;

    	case UVC_CAM_CFG_GET_CUR_CFG:
            ret = usbhostUvcGetCurrentConfigure(
                (void*)ppara1,
                (int *)ppara2,
                (int *)ppara3,
                (int *)ppara4,
                (int *)ppara5);
            break;

    	default:
    		HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: cam cfg INVALID ID");
    		ret = FAIL;
	}

    if(paraTot) va_end(args);
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: cam cfg get sel:%d 0x%x",sel ,ret);

    return ret;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcHostCfgSet
* Function         : Uvc Host Config Set
* Return value	   : result (SINT32 SUCCESS/FAIL)
* Parameter1	   : sel (UINT32 sel < UVC_HOST_CFG_MAX)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

SINT32 appUvcHostCfgSet(UINT32 sel,	...)
{
	SINT32 ret = FAIL;
	UINT32 paraTot;
	
    /*para4 = para3 = para2 = para1 = 0;*/
	va_list args;

	switch(sel)
    {
        case UVC_HOST_CFG_PLUG_OUT:         paraTot = 1; break;
        case UVC_HOST_CFG_PLUG_IN:          paraTot = 1; break;
        case UVC_HOST_CFG_PLUG_COMPLETE:    paraTot = 1; break;
        case UVC_HOST_CFG_PLUG_FAIL:        paraTot = 1; break;
        case UVC_HOST_CFG_SIZE:             paraTot = 2; break;
        case UVC_HOST_CFG_FRM_INTVAL:       paraTot = 1; break;
        case UVC_HOST_CFG_SZ_IDX:           paraTot = 1; break;
    	default: paraTot = 0; break;
	}

	if(paraTot)
    {
		UINT32 para1;
    	va_start(args, sel);
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: host set cfg sel=%d", sel);
    	para1 = va_arg(args, UINT32);
    	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: host cfg para1=%d(0x%X)", para1, para1);
    	if(paraTot >= 2) {
			UINT32 para2;
    		para2 = va_arg(args, UINT32);
        	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: host cfg para2=%d(0x%X)", para2, para2);
    		if(paraTot >= 3) {
				UINT32 para3;
    			para3 = va_arg(args, UINT32);
    			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: host cfg para3=%d(0x%X)", para3, para3);
    			if(paraTot >= 4) {
					UINT32 para4;
    				para4 = va_arg(args, UINT32);
    				HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: host cfg para4=%d(0x%X)", para4, para4);
    			}
    		}
    	}
	}

	switch(sel)
    {
        case UVC_HOST_CFG_PLUG_OUT:
                CamCfg.host.plug = UVC_PLUG_OUT;
                sp5kOsEventFlagsSet(&pUvcPlugEvt, EVT_PLUG_OUT, TX_OR);
            ret = SUCCESS;
            break;

        case UVC_HOST_CFG_PLUG_IN:
            ret = SUCCESS;
            if(CamCfg.host.plug == UVC_PLUG_FAIL)
                ret = FAIL;
            else if(CamCfg.host.plug == UVC_PLUG_OUT)
            {
	            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: EVT_PLUG_IN s");
				sp5kOsEventFlagsSet(&pUvcPlugEvt, EVT_PLUG_IN, TX_OR);
			}
            break;

        case UVC_HOST_CFG_PLUG_COMPLETE:
        case UVC_HOST_CFG_PLUG_FAIL:
            CamCfg.host.plug = para1;
            break;

        case UVC_HOST_CFG_SIZE:
            CamCfg.host.width = para1;
            CamCfg.host.height= para2;
            break;

        case UVC_HOST_CFG_FRM_INTVAL:
            CamCfg.host.frmintval = para1;
            break;

        case UVC_HOST_CFG_SZ_IDX:
            {
                CamCfg.host.szidx = para1;
                CamCfg.host.width = uvcsize[para1].w;
                CamCfg.host.height= uvcsize[para1].h;
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: host cfg set %d size: %dx%d"
                    ,CamCfg.host.szidx ,uvcsize[para1].w ,uvcsize[para1].h);
            }
            ret = SUCCESS;
            break;

    	default:
    		ret = FAIL;
	}

	if(paraTot) va_end(args);

	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: host cfg set sel:%d 0x%x",sel ,ret);
    return ret;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcHostCfgGet
* Function         : Get Uvc Host Config 
* Return value	   : result (SINT32 SUCCESS/FAIL)
* Parameter1	   : sel (UINT32 sel < UVC_HOST_CFG_MAX)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

SINT32 appUvcHostCfgGet(UINT32 sel,	...)
{
	SINT32 ret = FAIL;
	UINT32 paraTot;
	/*UINT32 *ppara1, *ppara2, *ppara3, *ppara4;
    ppara4 = ppara3 = ppara2 = ppara1 = NULL;
    ppara4 = ppara4;*/

	va_list args;

	switch(sel)
    {
        case UVC_HOST_CFG_PLUG_OUT:         paraTot = 0; break;
        case UVC_HOST_CFG_PLUG_IN:          paraTot = 0; break;
        case UVC_HOST_CFG_PLUG_COMPLETE:    paraTot = 0; break;
        case UVC_HOST_CFG_PLUG_FAIL:        paraTot = 0; break;
        case UVC_HOST_CFG_SIZE:             paraTot = 2; break;
        case UVC_HOST_CFG_FRM_INTVAL:       paraTot = 1; break;
        case UVC_HOST_CFG_SZ_IDX:           paraTot = 1; break;
        default: paraTot = 0; break;
	}

	if(paraTot)
    {
    	va_start(args, sel);
		UINT32 *ppara1
    	ppara1 = va_arg(args, UINT32*);
    	if ( paraTot >= 2 ) {
			UINT32 *ppara2;
    		ppara2 = va_arg(args, UINT32*);
    		if ( paraTot >= 3 ) {
				UINT32 *ppara3;
    			ppara3 = va_arg(args, UINT32*);
    			if ( paraTot >= 4 ) {
					UINT32 *ppara4;
    				ppara4 = va_arg(args, UINT32*);
    			}
    		}
    	}
	}

	switch(sel) {
        case UVC_HOST_CFG_PLUG_OUT:
        case UVC_HOST_CFG_PLUG_IN:
			if(CamCfg.host.plug==UVC_PLUG_FAIL)
			{
				HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: host plug fail :%d 0x%x",sel ,ret);
            	ret = 0;
			}
			else
			{
            ret = CamCfg.host.plug;
			}
            break;

        case UVC_HOST_CFG_PLUG_COMPLETE:
            ret = (CamCfg.host.plug==UVC_PLUG_COMPLETE)?1:0;
            break;

        case UVC_HOST_CFG_PLUG_FAIL:
            ret = (CamCfg.host.plug==UVC_PLUG_FAIL)?1:0;
            break;

        case UVC_HOST_CFG_SIZE:
            *ppara1 = CamCfg.host.width;
            *ppara2 = CamCfg.host.height;
            ret = SUCCESS;
            break;

        case UVC_HOST_CFG_FRM_INTVAL:
            *ppara1 = CamCfg.host.frmintval;
            ret = SUCCESS;
            break;

        case UVC_HOST_CFG_SZ_IDX:
            *ppara1 = CamCfg.host.szidx;
            ret = SUCCESS;
            break;

    	default:
    		HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: host cfg INVALID ID");
    		ret = FAIL;
	}

	if (paraTot) va_end(args);

	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: host cfg get sel:%d 0x%x",sel ,ret);
    return ret;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcPlugTask
* Function         : Uvc Plug Task
* Return value	   : void
* Parameter1	   : parameter (ULONG)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static void appUvcPlugTask(ULONG param)
{
	UINT32 flags = 0;

	while (1) {
		sp5kOsEventFlagsGet(&pUvcPlugEvt, EVT_PLUG_ALL, TX_OR_CLEAR, &flags, TX_WAIT_FOREVER);

		if (flags & EVT_PLUG_IN)
		{
			profLogPrintf(0, "uvc: EVT_PLUG_IN s %x", flags);
            EVT_PLUG_CLR(pUvcPlugEvt, EVT_PLUG_IN);
            if(appUvcCamSizeScan()==SUCCESS)
            {
                CamCfg.host.plug = UVC_PLUG_IN;
                if(appUvcCamPlugCtrl(UVC_PLUG_IN) == SUCCESS)
                	appUvcEnumExtensionUnit();
                else
                {
                	HOST_PROF_LOG_PRINT(LEVEL_ERROR, "uvc: EVT_PLUG_IN fail");
                    CamCfg.host.plug = UVC_PLUG_OUT;
            	}
            }
            else
            {
                HOST_PROF_LOG_PRINT(LEVEL_ERROR, "uvc: EVT_PLUG_IN scan size fail");
            }
			profLogPrintf(0, "uvc: EVT_PLUG_IN e");
        }

        if (flags & EVT_PLUG_OUT)
        {
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: EVT_PLUG_OUT s %x", flags);
            appUvcCarModeSet(CAR_REVERSE_DIS);
            appUvcCamPlugCtrl(UVC_PLUG_OUT);
            appUvcCamInit();
            EVT_PLUG_CLR(pUvcPlugEvt, EVT_PLUG_OUT);
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: EVT_PLUG_OUT e");
        }
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcCamCfgInit
* Function         : Uvc plug task thread init
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUvcCamCfgInit(void)
{
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: %s ",__FUNCTION__);
    memset(&CamCfg, 0, sizeof(UvcCamCfg_t));
    memset(&UsbCtrl, 0, sizeof(UsbCtrl_t));
    memcpy(&CamCfg.fmt, "MJPG", sizeof(unsigned int));

    if(!pUvcPlugThr)
        pUvcPlugThr = sp5kOsThreadCreate("uvc_plug_thr", appUvcPlugTask, 0,
        17/*prio*/, 17/*preempt_thr*/, 0/*time_slice*/, TX_AUTO_START/*auto_start*/);

    if(!pUvcPlugEvt)
        sp5kOsEventFlagsCreate(&pUvcPlugEvt, "uvc_plug_evt");
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcCamParmGet
* Function         : Get uvc parameter
* Return value	   : pCamParam (UvcCamCfg_t*)
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UvcCamCfg_t* appUvcCamParmGet(void)
{
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: %s ",__FUNCTION__);
	return pCamParam;
}

