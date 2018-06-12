/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
#include "common.h"
#include <stdio.h>

#include "app_com_def.h"
#include "app_playback_def.h"
#include "app_icon_def.h"
#include "app_ui_para.h"
#include "app_view_param_def.h"
#include "app_video.h"
#include "app_disk.h"
#include "sp5k_disp_api.h"
#include "gpio_custom.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#if 0/*jj move to app_playback_lib ,modify as pbPicSize_t gpbSizeGrade[PB_UI_RESIZE_TOTAL][3]*/
static struct
{
	UINT32 w,h;
	UINT16 icon;
}PicSize[9][3] =
{		/*4:3*/		/*3:2*/		/*16:9*/
	{{3648, 2736, ID_ICON_RESOLUTION_10M},{3648, 2432, ID_ICON_RESOLUTION_10M},{3648, 2048, ID_ICON_RESOLUTION_10M}},/*10M*/
	{{3456, 2592, ID_ICON_RESOLUTION_9M},{3456, 2304, ID_ICON_RESOLUTION_9M},{3456, 1944, ID_ICON_RESOLUTION_9M}},/*9M*/
	{{3264, 2448, ID_ICON_RESOLUTION_8M},{3264, 2176, ID_ICON_RESOLUTION_8M},{3264, 1840, ID_ICON_RESOLUTION_8M}},/*8M*/
	{{3072, 2304, ID_ICON_RESOLUTION_7M},{3072, 2048, ID_ICON_RESOLUTION_7M},{3072, 1728, ID_ICON_RESOLUTION_7M_16_9}},/*7M*/
	/*{{2816, 2112,ID_ICON_RESOLUTION_6M},{2816, 1872,ID_ICON_RESOLUTION_6M},{2816, 1584,ID_ICON_RESOLUTION_6M}},6M*/
	{{2560, 1920, ID_ICON_RESOLUTION_5M},{2560, 1696, ID_ICON_RESOLUTION_5M},{2560, 1440, ID_ICON_RESOLUTION_5M}},/*5M*/
	/*{{2272, 1704,ID_ICON_RESOLUTION_4M},{2272, 1512,ID_ICON_RESOLUTION_4M},{2272, 1280,ID_ICON_RESOLUTION_4M}},4M*/
	{{2048, 1536, ID_ICON_RESOLUTION_3M},{2048, 1360, ID_ICON_RESOLUTION_3M},{2048, 1152, ID_ICON_RESOLUTION_3M}},/*3M*/
	{{1600, 1200, ID_ICON_RESOLUTION_2M},{1600, 1064, ID_ICON_RESOLUTION_2M},{1600, 896, ID_ICON_RESOLUTION_2M_16_9}},/*2M*/
	{{1280,  960, ID_ICON_RESOLUTION_1M},{1280,  832, ID_ICON_RESOLUTION_1M},{1280, 720, ID_ICON_RESOLUTION_1M}},/*1M*/
	{{ 640,  480, ID_ICON_RESOLUTION_VGA},{ 640,  480, ID_ICON_RESOLUTION_VGA},{640, 360, ID_ICON_RESOLUTION_VGA}}/*VGA*/
};
#endif
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
/**
 * @fn		  UINT32 appMenuAct_PbFileIsJpg(UINT8 item)
 * @brief	  appMenuAct_PbFileIsJpg
 * @param	  [in] void
 * @retval	  return = TRUE / FALSE
 * @see 	  NULL
 * @author	  yongsun
 * @since	  2012-11-08
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT32
appMenuAct_PbFileIsJpg(
	void
)
{
	pbImgCb_t* gpbImgCb=appPb_ImageCbPtrGet();

	if (IS_FILETYPE_JPG(gpbImgCb->dcfAttr.fileType)&&appPbTotalFileCountGet()){
		return TRUE;
	}

	return FALSE;
}

/**
 * @fn		UINT32 appMenuAct_PbPhotoEffect(UINT8 item)
 * @brief	appMenuAct_PbPhotoEffect
 * @param	[in] item
 * @retval	return = TRUE / FALSE
 * @see		NULL
 * @author	xu.gao
 * @since	2012-12-06
 * @todo	N/A
 * @bug		N/A
*/

UINT32
appMenuAct_PbPhotoEffect(
UINT8 item
)
{//add for temporary
	if(item == 5)
	{
		return FALSE;
	}

	return TRUE;
}

/**
 * @fn		  UINT32 appMenuAct_PbPhotoResize(UINT8 item)
 * @brief	  appMenuAct_PbPhotoResize
 * @param	  [in] item
 * @retval	  return = TRUE / FALSE
 * @see 	  NULL
 * @author	  xu.gao
 * @since	  2012-12-25
 * @todo	  N/A
 * @bug 	  N/A
*/

UINT32
appMenuAct_PbPhotoResize(
	UINT8 item
)
{
//	item = item;
	pbImgCb_t* gpbImgCb=appPb_ImageCbPtrGet();

	if (IS_FILETYPE_JPG(gpbImgCb->dcfAttr.fileType) && appPbTotalFileCountGet()){
		if(gpbImgCb->imgWidth > 640 && gpbImgCb->imgWidth != 1920)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	return FALSE;
}

/**
 * @fn		  UINT32 appMenuAct_PbPhotoResizeSize(UINT8 item)
 * @brief	  appMenuAct_PbPhotoResizeSize
 * @param	  [in] item
 * @retval	  return = TRUE / FALSE
 * @see 	  NULL
 * @author	  yongsun
 * @since	  2012-11-08
 * @todo	  N/A
 * @bug 	  N/A
*/

UINT32
appMenuAct_PbPhotoResizeSet(
	UINT8 item
)
{
	UINT32 ret=TRUE;
	pbImgCb_t* gpbImgCb=appPb_ImageCbPtrGet();
//	item=item;

	if (appPbTotalFileCountGet())
	{
		if (IS_FILETYPE_JPG(gpbImgCb->dcfAttr.fileType)){
			if ((UINT32)gpbImgCb->imgWidth *3 > (UINT32)gpbImgCb->imgHeight<<2) {//16:9
				if ((gpbImgCb->imgWidth > gpbSizeGrade[item][2][0]) && (gpbImgCb->imgHeight > gpbSizeGrade[item][2][1])&&(UINT32)gpbSizeGrade[item][2][0]*3 > ((UINT32)gpbSizeGrade[item][2][1]<<2))
				{
					return ret;
				}
			}
			else {
				if ((gpbImgCb->imgWidth > gpbSizeGrade[item][0][0]) && (gpbImgCb->imgHeight > gpbSizeGrade[item][0][1])&&((UINT32)gpbSizeGrade[item][0][0]*3 == ((UINT32)gpbSizeGrade[item][0][1]<<2)))
				{
					return ret;
				}
			}
		}
	}

	return FALSE;

}
/**
 * @fn		  UINT32 appMenuAct_PbPhotoVoiceMemo(UINT8 item)
 * @brief	  appMenuAct_PbPhotoVoiceMemo
 * @param	  [in] item
 * @retval	  return = TRUE / FALSE
 * @see 	  NULL
 * @author	  juanjuan.yuan
 * @since	  2012-12-10
 * @todo	  N/A
 * @bug 	  N/A
*/

UINT32
appMenuAct_PbPhotoRotate (
	UINT8 item
)
{
	UINT32 ret=TRUE;
	pbImgCb_t* gpbImgCb=appPb_ImageCbPtrGet();
//	item=item;

	if((IS_FILETYPE_JPG(gpbImgCb->dcfAttr.fileType))&&(gpbImgCb->isProtected == FALSE)){
		return ret;
	}

	return FALSE;
}

/**
 * @fn		  UINT32 appMenuAct_PbPhotoVoiceMemo(UINT8 item)
 * @brief	  appMenuAct_PbPhotoVoiceMemo
 * @param	  [in] item
 * @retval	  return = TRUE / FALSE
 * @see 	  NULL
 * @author	  yongsun
 * @since	  2012-11-08
 * @todo	  N/A
 * @bug 	  N/A
*/

UINT32
appMenuAct_PbPhotoVoiceMemo (
	UINT8 item
)
{
	UINT32 ret=TRUE;
	pbImgCb_t* gpbImgCb=appPb_ImageCbPtrGet();

	if (!appPbTotalFileCountGet())return FALSE;

	if(IS_FILETYPE_JPG(gpbImgCb->dcfAttr.fileType)){
		if (gpbImgCb->dcfAttr.fileType==SP5K_DCF_FILETYPE_JPG_MEMO){
			if(item==0){
				ret=FALSE;
			}else if(item==1){
				ret=TRUE;
			}
		}else{
			if(item==0){
				ret=TRUE;
			}else if(item==1){
				ret=FALSE;
			}
		}
		return ret;
	}

	return FALSE;

}

/**
 * @fn		UINT32 appMenuAct_ImageSizeSet(UINT8 item)
 * @brief		appMenuAct_ImageSizeSet
 * @param	[in] item
 * @retval	return = TRUE / FALSE
 * @see		NULL
 * @author	xu.gao
 * @since		2012-12-26
 * @todo		N/A
 * @bug		N/A
*/

UINT32
appMenuAct_ImageSize(
	UINT8 item
)
{

//	item = item;
	#if 0  /*ez mark*/
	uiPara_t* puiPara = appUiParaGet();
    #if TIMELAPSE_EN
	if(puiPara->FeatureEffect == UI_FEATURE_EFFECT_SPEEDY_CAPTURE \
	|| puiPara ->FeatureEffect == UI_FEATURE_EFFECT_16IN1 \
	|| puiPara ->TimeLapse != UI_TIMELAPSE_OFF)
	#else
    if(puiPara->FeatureEffect == UI_FEATURE_EFFECT_SPEEDY_CAPTURE \
	|| puiPara ->FeatureEffect == UI_FEATURE_EFFECT_16IN1 )
	#endif
	{
		return FALSE;
	}
	else
	#endif
	{
		return TRUE;
	}

}

/**
 * @fn		  UINT32 appMenuAct_PbPhotoVoiceMemo(UINT8 item)
 * @brief	  appMenuAct_PbPhotoVoiceMemo
 * @param	  [in] item
 * @retval	  return = TRUE / FALSE
 * @see 	  NULL
 * @author	  yongsun
 * @since	  2012-11-08
 * @todo	  N/A
 * @bug 	  N/A
*/





/**
 * @fn		  UINT32  appMenuAct_stillFeatureEffect(UINT8 item)
 * @brief	  appMenuAct_stillFeatureEffect
 * @param	  [in] item
 * @retval	  return = TRUE / FALSE
 * @see 	  NULL
 * @author	  junqiong.hu
 * @since	  2013-03-07
 * @todo	  N/A
 * @bug 	  N/A
*/

/*just for PhotoFrame  be nonsupport provisionally when 16 in 1/speed capture */
UINT32
appMenuAct_stillFeatureEffect (
	UINT8 item
)
{
	UINT32 ret=TRUE;
	 /*ez mark*/
	/*uiPara_t* puiPara = appUiParaGet();
	if(puiPara->PhotoFrame != UI_PHOTOFRAME_OFF )
	{
		ret = FALSE;
	}*/

	return ret;
}

/**
 * @fn		  UINT32  appMenuAct_stillVideoSize(UINT8 item)
 * @brief	  appMenuAct_stillVideoSize
 * @param	  [in] item
 * @retval	  return = TRUE / FALSE
 * @see 	  NULL
 * @author	  junqiong.hu
 * @since	  2012-12-17
 * @todo	  N/A
 * @bug 	  N/A
*/

UINT32 appMenuAct_VideoSizeSet(UINT8 item)
{
#if SP5K_ICAT_EIS
{
    uiPara_t* puiPara = appUiParaGet();

    if (puiPara->VideoEISMode != UI_EIS_OFF)
	{
	    if (item == 3 || item==4)	/*4K2K, 2.7K not support EIS*/
	        return FALSE;
    }
}
#endif

 /*ez mark*/
/*
#if SP5K_SENSOR_OV5633
	if((item == UI_VID_SIZE_SVGA)||(item == UI_VID_SIZE_VGA)||(item == UI_VID_SIZE_QVGA))
	{
		return FALSE;
	}
#elif SP5K_SENSOR_OV2715
	if(item == UI_VID_SIZE_QVGA)
	{
		return FALSE;
	}
#else
	if(cardInfo.cardSpeed <=4)
	{
		if(item == UI_VID_SIZE_1080P_60FPS)
		{
			return FALSE;
		}
	}
	if(item == UI_VID_SIZE_SVGA)
	{
		return FALSE;
	}
#endif*/

#if (KIT_WITH_HDMI)
    if(IS_HDMI_IN){
        return FALSE;
    }
#endif

	return TRUE;
}

/**
 * @fn		  UINT32  appMenuAct_VideoFormat(UINT8 item)
 * @brief	  appMenuAct_VideoFormat
 * @param	  [in] item
 * @retval	  return = TRUE / FALSE
 * @see 	  NULL
 * @author	  junqiong.hu
 * @since	  2013-07-16
 * @todo	  N/A
 * @bug 	  N/A
*/

UINT32 appMenuAct_VideoFormat(UINT8 item)
{
	UINT32 ret=TRUE;

    #if TIMELAPSE_EN
	 /*ez mark*/
	//if(appUiParaGet()->TimeLapse != UI_TIMELAPSE_OFF)
	#endif
	{
		ret = FALSE;
	}

	return ret;
}

UINT32 appMenuAct_VideoSeamless (UINT8 item)
{
    uiPara_t* puiPara = appUiParaGet();

    if((puiPara->VideoSize== UI_VIDEO_SIZE_4K2K)||(puiPara->VideoSize == UI_VIDEO_SIZE_27K)){
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

UINT32 appMenuAct_EISMode (UINT8 item)
{
    /* uiPara_t* puiPara = appUiParaGet(); */

#if SP5K_ICAT_EIS
	uiPara_t* puiPara = appUiParaGet();
    if((puiPara->VideoSize== UI_VIDEO_SIZE_4K2K)||(puiPara->VideoSize == UI_VIDEO_SIZE_27K))
	    return FALSE;
    else
        return TRUE;
#else
    return FALSE;
#endif
}

UINT32 appMenuAct_WIFIPassword(UINT8 item)
{
	UINT32 ret;
	ret = FALSE;
	return ret;
}

UINT32 appMenuAct_TVMode(UINT8 item)
{
	UINT32 ret=TRUE;

	if (IS_TV_IN) 
		return FALSE;

	return ret;
}


