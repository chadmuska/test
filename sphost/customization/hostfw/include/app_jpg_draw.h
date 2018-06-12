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
 **************************************************************************/
#ifndef APP_JPG_DRAW_H
#define APP_JPG_DRAW_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 typedef enum {
	JPG_DRAW_IMG_LAYER  = 0,  
	JPG_DRAW_PIP_LAYER,         
	JPG_DRAW_PF_LAYER,
	JPG_DRAW_LAYER_MAX
} appJpgDrawLayer_e;

 typedef enum {
	AVI_DRAW_IMG_LAYER  = 0,  
	AVI_DRAW_PIP_LAYER,         
	AVI_DRAW_PF_LAYER,
	AVI_DRAW_LAYER_MAX
} appAviDrawLayer_e;
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appJpgFileDraw(UINT32 drawLayer, UINT8 *filename,UINT32 x0,	UINT32 y0,UINT32 sizeW,UINT32 sizeH);
UINT32 appAviFileDraw(UINT32 drawLayer, UINT8 *filename,UINT32 x0,	UINT32 y0,UINT32 sizeW,UINT32 sizeH);

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

#define appJpgDraw_ImgLayer(file,x,y,w,h)  appJpgFileDraw(JPG_DRAW_IMG_LAYER,file,x,y,w,h)
#define appJpgDraw_PipLayer(file,x,y,w,h)	  appJpgFileDraw(JPG_DRAW_PIP_LAYER,file,x,y,w,h)
#define appJpgDraw_PfLayer(file,x,y,w,h)      appJpgFileDraw(JPG_DRAW_PF_LAYER,file,x,y,w,h)


#endif  /* APP_JPG_DRAW_H*/

