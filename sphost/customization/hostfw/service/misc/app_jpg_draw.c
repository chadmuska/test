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
 *	File name    : App_jpg_draw.c
 *	Function     : The implementation which draw jpg file.
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.06  Edward.Lu  "Add description and modify code"
 **************************************************************************/

#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_fs_api.h"
#include "app_jpg_draw.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

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

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : appJpgFileDraw
* Function         : Host to play an existing resource jpg file with a full path filename.
* Return value	   : result (UINT32 Success:0, Fail:error code)
* Parameter1	   : drawLayer (UINT32)
                     SP5K_RES_TYPE_JPG    :	Display JPEG resource file on image layer
                     SP5K_RES_TYPE_JPG_PIP: Display JPEG resource file on gfx PIP layer
                     SP5K_RES_TYPE_JPG_PF :	Display JPEG resource file on gfx PF layer
* Parameter2	   : filename (UINT8*)
* Parameter3	   : x0 (UINT32 display start X)
* Parameter4	   : y0 (UINT32 display start Y)
* Parameter5	   : sizeW (UINT32 display width)
* Parameter6	   : sizeH (UINT32 display height)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appJpgFileDraw(
	UINT32 drawLayer, 
	UINT8 *filename,
	UINT32 x0,
	UINT32 y0,
	UINT32 sizeW,
	UINT32 sizeH
)
{
	UINT32 ret, param[4];
	UINT32 layer[JPG_DRAW_LAYER_MAX] = {SP5K_RES_TYPE_JPG, 
										SP5K_RES_TYPE_JPG_PIP, 
										SP5K_RES_TYPE_JPG_PF };
	
	if ((drawLayer >= JPG_DRAW_LAYER_MAX) || (filename == NULL)) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "jpgdraw: %s (%d, %s)",__FUNCTION__, drawLayer,filename);
		return FAIL;
	}
	
	if ((sizeW == 0) && (sizeH == 0)) {
		param[0] = 0; // display start X
		param[1] = 0; // display start Y
		param[2] = 320; // display width
		param[3] = 240; // display height
	}
	else{
		param[0] = x0;
		param[1] = y0;
		param[2] = sizeW;
		param[3] = sizeH;
	}
	// Host to play an existing resource file with a full path filename.
	ret = sp5kResourceFilePlay(layer[drawLayer], (char*)filename, param);	
	return ret;
}

/*--------------------------------------------------------------------------*
* Function name    : appAviFileDraw
* Function         : Host to play an existing resource avi file with a full path filename.
* Return value	   : result (UINT32 Success:0, Fail:error code)
* Parameter1	   : drawLayer (UINT32)
                     SP5K_RES_TYPE_AVI    :	Display AVI resource file on image layer
                     SP5K_RES_TYPE_AVI_PIP: Display AVI resource file on gfx PIP layer
                     SP5K_RES_TYPE_AVI_PF :	Display AVI resource file on gfx PF layer
* Parameter2	   : filename (UINT8*)
* Parameter3	   : x0 (UINT32 display start X)
* Parameter4	   : y0 (UINT32 display start Y)
* Parameter5	   : sizeW (UINT32 display width)
* Parameter6	   : sizeH (UINT32 display height)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appAviFileDraw(
	UINT32 drawLayer, 
	UINT8 *filename,
	UINT32 x0,
	UINT32 y0,
	UINT32 sizeW,
	UINT32 sizeH
)
{
	UINT32 ret, param[4];

	UINT32 layer[AVI_DRAW_LAYER_MAX] = {SP5K_RES_TYPE_AVI, 
										SP5K_RES_TYPE_AVI_PIP, 
										SP5K_RES_TYPE_AVI_PF };
	
	if ((drawLayer >= AVI_DRAW_LAYER_MAX) || (filename == NULL)) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "jpgdraw: %s (%d, %s)",__FUNCTION__, drawLayer,filename);
		return FAIL;
	}
	
	if ((sizeW == 0) && (sizeH == 0)) {
		param[0] = 0;
		param[1] = 0;
		param[2] = 320;
		param[3] = 240;
	}
	else{
		param[0] = x0;
		param[1] = y0;
		param[2] = sizeW;
		param[3] = sizeH;
	}
	// Host to play an existing resource file with a full path filename.
	ret = sp5kResourceFilePlay(layer[drawLayer], (char*)filename, param);	
	return ret;
}
