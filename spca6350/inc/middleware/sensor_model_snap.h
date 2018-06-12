/**************************************************************************
 *
 *       Copyright (c) 2009-2016 by iCatch Technology, Inc.
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
 *                                                                        *
 *  Author: Shengju wang                                                  *
 *      0.00.01, 20091110 created                                         * 
 *                                                                        *
 **************************************************************************/
#ifndef _CUSTOM_SENSOR_MODEL_SNAP_H_
#define _CUSTOM_SENSOR_MODEL_SNAP_H_
UINT32 sensorDsenExtSet(void);
UINT32 sensorSyncSigSnapExtSet(UINT32 cfg,UINT32 val1,UINT32 val2,UINT32 val3,UINT32 val4);
UINT32 sensorFRONTExtSet(UINT32 sel,UINT32 val);
UINT32 sensorCDSPExtSet(UINT32 sel,UINT32 val);
UINT32 sensorCDSPExtGet(UINT32 sel);


typedef enum {
  SENSOR_SNAP_EXT_HOFFSET2  =  0x00000000, /*UINT32 hoffset2,UINT32 hoffset3,UINT32 hoffset4*/
  SENSOR_SNAP_EXT_FLDOEINC  ,              /*UINT32 Fldsel,UINT32 fldoinc,UINT32 fldeinc*/ 
  SENSOR_SNAP_EXT_HDIVMODE  ,              /*UINT32 hdivmode,UINT32 svd,UINT32 clrmode,UINT32 vcntsft*/
  SENSOR_SNAP_EXT_PIXEL_SEQ ,              /*UINT32 pos0,UINT32 pos1,UINT32 pos2,UINT32 pos3*/
  SENSOR_SNAP_EXT_PIXEL_SEQ2,			   /*UINT32 pos4,UINT32 pos5,UINT32 pos6,UINT32 pos7*/
  SENSOR_SNAP_EXT_DESEN_CFG ,              /*UINT32 sensormode,UINT32 hsize,UINT32 vsize,UINT32 htotal*/
  SENSOR_SNAP_EXT_TGPLL_EN  ,              /*UINT32 TGPLLEN, UINT32 TGPLLDRV*/
  SENSOR_SNAP_EXT_TGPLL_DIV ,              /*UINT32 TGPLLK,UINT32 TGPLLM,UINT32 TGPLLN*/
  SENSOR_SNAP_EXT_LINESW,				   /*UINT32 CFAPattern (First Field ),UINT32 Field sequence (Nth Field) ,UINT32 fixline,UIN32 svd (0:imm  1:vd	2:zfupdate )	*/
  SENSOR_SNAP_EXT_LINESW_AUTO,			   /*UINT32 en, UINT32 svd*/
  SENSOR_SNAP_EXT_ONE_FIELD,               /*UINT32 Hsize,UINT32 Vsize,UINT32 Field_Num*/
} sensorSyncSigSnapExtSel_t;

typedef enum {
  SENSOR_CDSP_EXT_DESENOR =  0x00000000, 
  SENSOR_CDSP_EXT_RAWMAP_START  ,
  SENSOR_CDSP_EXT_RAWMAP_END,
  SENSOR_CDSP_EXT_LINESW, 
  SENSOR_CDSP_EXT_DESENSOR_VIEW_MODE9,
  SENSOR_CDSP_EXT_DESENSOR_SNAP_MODE9,
} sensorCDSPExtSel_t;

typedef enum {
  SENSOR_FRONT_EXT_DIV =	0x00000000, 
  SENSOR_FRONT_EXT_DIV_SHIFT	,
} sensorFRONTExtSel_t;


typedef enum {
  SENSOR_TGPLL_DRV_2MA    = 0,
  SENSOR_TGPLL_DRV_4MA    = 1,
  SENSOR_TGPLL_DRV_6MA    = 2,  
  SENSOR_TGPLL_DRV_8MA    = 3,  
  SENSOR_TGPLL_DRV_10MA   = 4,  
  SENSOR_TGPLL_DRV_12MA   = 5,  
  SENSOR_TGPLL_DRV_14MA   = 6,  
  SENSOR_TGPLL_DRV_16MA   = 7,  
} sensorTGPLLDrvCap_t;

typedef enum {
  HDIVSVLDEN_IMM   = 0,
  HDIVSVLDEN_VVAILD,
  HDIVSVLDEN_VRESH,
} sensorHDIVENSynCap_t;

typedef enum {
  HDIVCNTCLRMODE_VDORG  = 0,
  HDIVCNTCLRMODE_VRESH,
} sensorHDIVCNTCTRModeCap_t;
#endif
