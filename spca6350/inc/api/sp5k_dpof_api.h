/**************************************************************************
 *
 *       Copyright (c) 2002-2016 by iCatch Technology, Inc.
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
 *  Author: Huiling Chou                                                  *
 *                                                                        *
 **************************************************************************/
#include "common.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#ifndef _SP5K_DPOF_API_H_
#define _SP5K_DPOF_API_H_

#define SP5K_DPOF_PRINT_MODE_STD 1
#define SP5K_DPOF_PRINT_MODE_IDX 2
#define SP5K_DPOF_PRINT_MODE_SIZ 3

#define SP5K_DPOF_ROT_NONE 0x00
#define SP5K_DPOF_ROT_90   0x01
#define SP5K_DPOF_ROT_180  0x10
#define SP5K_DPOF_ROT_270  0x11

#define SP5K_DPOF_DEL_CUR 1
#define SP5K_DPOF_DEL_ALL 2

#define SP5K_DPOF_XFER_MODE_IMG		0
#define SP5K_DPOF_XFER_MODE_FRONT	1
#define SP5K_DPOF_XFER_MODE_JOB		2

#define SP5K_DPOF_XFER_IMG_EXIST	1
#define SP5K_DPOF_XFER_FRONT_EXIST	2

enum {
	SP5K_DPOF_PARAMID_GEN_REV = 0x01,
	SP5K_DPOF_PARAMID_GEN_CRT,
	SP5K_DPOF_PARAMID_GEN_DTM,
	SP5K_DPOF_PARAMID_USR_NAM,
	SP5K_DPOF_PARAMID_USR_ADR,
	SP5K_DPOF_PARAMID_USR_TEL,
	SP5K_DPOF_PARAMID_VUQ_RGN,
	SP5K_DPOF_PARAMID_VUQ_VNM,
	SP5K_DPOF_PARAMID_GEN_CRT_REV
};

enum {
	SP5K_DPOF_PARAMID_PRT_PID = 0x01,
	SP5K_DPOF_PARAMID_PRT_TYP,
	SP5K_DPOF_PARAMID_PRT_QTY,
	SP5K_DPOF_PARAMID_PRT_PSL,
	SP5K_DPOF_PARAMID_PRT_FAL,
	SP5K_DPOF_PARAMID_IMG_FMT,
	SP5K_DPOF_PARAMID_IMG_SRC,
	SP5K_DPOF_PARAMID_CFG_DSC_AD, /* ATTR DTM */
	SP5K_DPOF_PARAMID_CFG_DSC_CMT, /* ATTR DTM */
	SP5K_DPOF_PARAMID_CFG_TRM,
	SP5K_DPOF_PARAMID_CFG_ROT,
	SP5K_DPOF_PARAMID_JOB_END
};

enum {
	SP5K_DPOF_IMGFMT_CFIF1 = 0x01,
	SP5K_DPOF_IMGFMT_EXIF1_J,
	SP5K_DPOF_IMGFMT_EXIF1_T,
	SP5K_DPOF_IMGFMT_EXIF2_J,
	SP5K_DPOF_IMGFMT_EXIF2_T,
	SP5K_DPOF_IMGFMT_JFIF,
	SP5K_DPOF_IMGFMT_FPX1,
	SP5K_DPOF_IMGFMT_UNDEF
};

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
UINT32 sp5kDpofInit(UINT8 *pcreator);
UINT32 sp5kDpofFileWrite(void);
UINT32 sp5kDpofClose(void);
UINT32 sp5kDpofFileAttrSet(UINT32 paramId, UINT8 *pdata);
UINT32 sp5kDpofJobAdd(UINT32 modeId);
UINT32 sp5kDpofJobDelete(UINT32 modeId, UINT32 opt);
UINT32 sp5kDpofJobQuery(UINT32 modeId);
UINT32 sp5kDpofJobAttrSet(UINT32 modeId, UINT32 paramId, void *pvoid);
UINT32 sp5kDpofJobAttrGet(UINT32 modeId,UINT32 paramId,	void *pdata);

UINT32 sp5kDpofFirstJobGet(UINT32 mode, UINT32 *pnewJobHandle);
UINT32 sp5kDpofNextJobGet(UINT32 mode, UINT32 jobHandle,UINT32 *pnewJobHandle);
UINT32 sp5kDpofPrevJobGet(UINT32 mode, UINT32 jobHandle, UINT32 *pnewJobHandle);
UINT32 sp5kDpofFirstSrcGet(UINT32 mode, UINT32 jobHandle, UINT32 *psrcHandle);
UINT32 sp5kDpofSrcParamGet(UINT32 modeId, UINT32 srcHandle, UINT32 srcparamId, void *pdata);
UINT32 sp5kDpofJobParamGet(UINT32 modeId, UINT32 jobHandle, UINT32 jobParamId, void *pdata);

/* AUTXFER.MRK */
/**
 * \brief init sp5k dpof xfer
 * \param[in] void
 * \return SUCCESS or FAIL
 * \note it should be called after sp5kDpofInit()
 */
UINT32 sp5kDpofXferInit(void);

/**
 * \brief close sp5k dpof xfer
 * \param[in] void
 * \return SUCCESS or FAIL
 * \note it should be called before sp5kDpofClose()
 */
UINT32 sp5kDpofXferClose(void);

/**
 * \brief write MISC\AUTXFER.MRK file
 * \param[in] void
 * \return SUCCESS or FAIL
 */
UINT32 sp5kDpofXferFileWrite(void);

/**
 * \brief add the image with the current DCF index to the specified job ID 
 * of AUTXFER.MRK
 * \param[in] mode SP5K_DPOF_XFER_MODE_IMG or SP5K_DPOF_XFER_MODE_FRONT
 * \param[in] jobId the ID of job from 0 to 5
 * \return SUCCESS or FAIL
 */
UINT32 sp5kDpofXferImgAdd(UINT32 mode, UINT32 jobId);

/**
 * \brief delete the image with the current DCF index from the specified job 
 * ID of AUTXFER.MRK
 * \param[in] mode SP5K_DPOF_XFER_MODE_IMG or SP5K_DPOF_XFER_MODE_FRONT or SP5K_DPOF_XFER_MODE_JOB
 * \param[in] jobId the ID of job from 0 to 5
 * \return SUCCESS or FAIL
 */
UINT32 sp5kDpofXferImgDelete(UINT32 mode, UINT32 jobId);

/**
 * \brief query if the image with the current DCF index is in the specified job 
 * ID of AUTXFER.MRK
 * \param[in] jobId the ID of job from 0 to 5
 * \param[out] pflag bit0: image is existed in the job, bit1: image is the front cover
 * \return SUCCESS or FAIL
 */
UINT32 sp5kDpofXferImgQuery(UINT32 jobId, UINT32 *pflag);

/**
 * \brief get the first image in the job specified by the index
 * \param[in] jobId the job index from 0 to 5
 * \param[out] pimgHandle the handle of the image
 * \return SUCCESS or FAIL
 */
UINT32 sp5kDpofXferFirstImgGet(UINT32 jobId, UINT32 *pimgHandle);

/**
 * \brief get the next image in the job specified by the index
 * \param[in] jobId the job index from 0 to 5
 * \param[out] pimgHandle the handle of the image
 * \return SUCCESS or FAIL
 */
UINT32 sp5kDpofXferNextImgGet(UINT32 jobId, UINT32 *pimgHandle);

/**
 * \brief get the previous image in the job specified by the index
 * \param[in] jobId the job index from 0 to 5
 * \param[out] pimgHandle the handle of the image
 * \return SUCCESS or FAIL
 */
UINT32 sp5kDpofXferPrevImgGet(UINT32 jobId, UINT32 *pimgHandle);

/**
 * \brief get the front image in the job specified by the index
 * \param[in] jobId the job index from 0 to 5
 * \param[out] pimgHandle the handle of the image
 * \return SUCCESS or FAIL
 */
UINT32 sp5kDpofXferFrontImgGet(UINT32 jobId, UINT32 *pimgHandle);

/**
 * \brief get the parameter value in the job specified by the index and input handle
 * \param[in] jobId the job index from 0 to 5
 * \param[in] imgHandle the handle of the image
 * \param[in] paramId the parameter id can be SP5K_DPOF_PARAMID_IMG_FMT or SP5K_DPOF_PARAMID_IMG_SRC
 * \param[out] pdata the output data
 * \return SUCCESS or FAIL
 */
UINT32 sp5kDpofXferImgParamGet(UINT32 jobId, UINT32 imgHandle, UINT32 paramId, void *pdata);

/**
 * \brief get the value of parameter ID in HDR section
 * \param[in] paramId the parameter ID
 * \param[out] pdata the output data
 * \return SUCCESS or FAIL
 */
UINT32 sp5kDpofXferHdrParamGet(UINT32 paramId, void *pdata);

#endif /* _SP5K_DPOF_API_H_ */

