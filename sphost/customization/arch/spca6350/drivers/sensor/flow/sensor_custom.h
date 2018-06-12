/**************************************************************************
 *
 *       Copyright (c) 2005-2016 by iCatch Technology, Inc.
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/

#ifndef _SENSOR_CUSTOM_H_
#define _SENSOR_CUSTOM_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*sensor_custom_crop_size_cfg_cb_t)(UINT16 a_uwHsize, UINT16 a_uwVsize);
typedef void (*sensor_custom_offset_cfg_cb_t)(UINT8 a_ucSenSel, SINT16 a_swHoffset, SINT16 a_swVoffset);
typedef void (*sensor_custom_wb_gain_reg_cfg_cb_t)(UINT8 a_ucSenSel, UINT32 a_udwRGain, UINT32 a_udwBGain,UINT32 a_udwGrGain, UINT32 a_udwGbGain);
typedef UINT32 (*sensor_custom_wb_gain_get_cb_t)(UINT8 a_ucSenSel, UINT32 *a_udwRGain, UINT32 *a_udwBGain,UINT32 *a_udwGrGain, UINT32 *a_udwGbGain);
typedef void (*sensor_custom_hdr_sen_wb_gain_reg_cfg_cb_t)(UINT32 a_udwRGain, UINT32 a_udwBGain,UINT32 a_udwGrGain, UINT32 a_udwGbGain);
typedef UINT32 (*sensor_custom_hdr_sen_wb_gain_get_cb_t)(UINT32 *a_udwRGain, UINT32 *a_udwBGain,UINT32 *a_udwGrGain, UINT32 *a_udwGbGain);

typedef struct {
	sensor_custom_crop_size_cfg_cb_t fp_customCropSizeCfgCb;
	sensor_custom_offset_cfg_cb_t fp_customOffsetCfgCb;/*for cropped size*/
	sensor_custom_offset_cfg_cb_t fp_customFullSizeOffsetCfgCb;/*for full size (1:1 normally)*/
	sensor_custom_wb_gain_reg_cfg_cb_t fp_customWbgainRegCfgCb;/*for calibration use. Set to sensor WB gain register directly.*/
	sensor_custom_offset_cfg_cb_t fp_customOffsetPvChkCb;/*Cropped size would be effective immediately in preview.*/
	sensor_custom_wb_gain_get_cb_t fp_customWbGainGetCb;/*Get the WB gain configuration value and the vlaue is implemented in sensor or cdsp side could be known from return value.*/
	sensor_custom_hdr_sen_wb_gain_reg_cfg_cb_t fp_customHdrSenWbgainRegCfgCb;/*Set to sensor WB gain register directly.*/
	sensor_custom_hdr_sen_wb_gain_get_cb_t fp_customHdrSenWbGainGetCb;/*Get the WB gain configuration value and the vlaue is implemented in sensor or cdsp side could be known from return value.*/
} sensorCustomCb_t;

void sensorCustomCbInstall(sensorCustomCb_t *psenCustomCb);
void sensorExpAgcPreCfgSet(SINT32 exp,UINT32 agc);
void sensorExpAgcPreCfgGet(SINT32 *exp,UINT32 *agc);
void sensorExpAgcPreSet(UINT32 smode, SINT32 expidx, UINT32 agcidx);

#ifdef __cplusplus
}
#endif

#endif /* _SENSOR_CUSTOM_H_ */
