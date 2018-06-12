/**************************************************************************
 *
 *       Copyright (c) 2014 -2018 by iCatch Technology, Inc.
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
 *  History:
 *  SamWang 20141112  create  basefw ver:12555  
 *
 **************************************************************************/
 
#ifndef _SENSOR_TEMPLATE_H_
#define _SENSOR_TEMPLATE_H_

#ifdef __cplusplus
extern "C" {
#endif
#define SNAP_NOR_RO     0
#define SNAP_FRM_RO     2
#define SNAP_SKIPEXP_RO 4

#ifndef SEN_MACRO_TG_MASTER_EN
#define SEN_MACRO_TG_MASTER_EN 0
#endif
#ifndef SEN_MACRO_AUTOOB_EN
#define SEN_MACRO_AUTOOB_EN    0
#endif
#ifndef SEN_MACRO_PARALLEL_EN
#define SEN_MACRO_PARALLEL_EN  0
#endif

#ifndef SEN_MACRO_LVDS_SIZE_EN
#define SEN_MACRO_LVDS_SIZE_EN  0
#endif

#ifndef SEN_MACRO_FRAMERATE_EN
#define SEN_MACRO_FRAMERATE_EN  0
#endif


#define SEN_MACRO_BINCORR(desc) \
.pbincorr =&( const sensorTgBinCor_t){						\
	.iBinMode			= desc##_BINCORR_BINMODE, 	/*: 0~1 	*/		\
	.htrend				= desc##_BINCORR_HTREND,	/*: 0~1 	*/		\
	.vtrend				= desc##_BINCORR_VTREND,	/*: 0~1 	*/		\
	.iHBinFac0			= desc##_BINCORR_HBINFAC0,	/*: 0~2048 */	\
	.iHBinFac1			= desc##_BINCORR_HBINFAC1,	/*: 0~2048 */	\
	.iHBinFac2			= desc##_BINCORR_HBINFAC2,	/*: 0~2048 */	\
	.iHBinFac3			= desc##_BINCORR_HBINFAC3,	/*: 0~2048 */	\
	.iVBinFac0			= desc##_BINCORR_VBINFAC0,	/*: 0~1024 */	\
	.iVBinFac1			= desc##_BINCORR_VBINFAC1,	/*: 0~1024 */	\
	.iBinCorrHen		= desc##_BINCORR_HEN,		/*: 0~ 3	 */		\
},														


/*========Macro definition===================================>>>>*/
#if SEN_MACRO_AUTOOB_EN
#define SEN_MACRO_OBWIN(desc) \
.pobwin[0] = &( const sensorTgWin_t ) { 					\
	.hoffset			= desc##_OB_HOFFSET,				\
	.voffset			= desc##_OB_VOFFSET,				\
	.hsize				= desc##_OB_HSIZE,					\
	.vsize				= desc##_OB_VSIZE,					\
},															
#else
#define SEN_MACRO_OBWIN(desc) 
#endif

#if SEN_MACRO_FRAMERATE_EN
#define SEN_MACRO_FRAMERATE(desc) \
	.fps_mhz				=(desc##_FRAMERATE_KFPS*1000),					
#else
#define SEN_MACRO_FRAMERATE(desc) 
#endif


#define SEN_MACRO_PREV_TG_CFG(desc) 							\
{																\
	.fpdo = &(const sensorPreviewDo_t) {						\
		.previewBegin = sensorDoFuncPreviewBegin,				\
		.previewShortExp = sensorDoFuncPreviewShortExp, 		\
		.previewLongExpBegin = NULL,							\
		.previewLongExpFollow = NULL,							\
		.previewLongExpEnd = NULL,								\
		.previewExpEnd = sensorDoFuncPreviewExpEnd, 			\
	},															\
	.crop = &( const sensorTgCrop_t ) { 						\
		.xOffset			= desc##_CROP_HOFFSET,				\
		.yOffset			= desc##_CROP_VOFFSET,				\
		.xSize				= desc##_CROP_HSIZE,				\
		.ySize				= desc##_CROP_VSIZE,				\
	},															\
	SEN_MACRO_OBWIN(desc)\
	SEN_MACRO_FRAMERATE(desc)\
	.hsize					= desc##_HSIZE ,					\
	.vsize					= desc##_VSIZE,						\
	.sensor_model			= SENSORMODEL,						\
	.model_info 			= desc##_MODEL_INFO,				\
	.cfa_pattern			= desc##_CFAPATTERN,				\
	.hratio 				= desc##_HRATIO,					\
	.vratio 				= desc##_VRATIO,					\
	.sensor_pclk			= desc##_PCLK1X,					\
	.sensor_htotal			= desc##_LINETOTAL,					\
	.pix_100khz 			= desc##_CDSPCLK, 					\
	.bayerhs_size			= desc##_BAYERHS_SIZE,				\
}																

#define SEN_MACRO_PREV_TG_BIN_CFG(desc) 						\
{																\
	.fpdo = &(const sensorPreviewDo_t) {						\
		.previewBegin = sensorDoFuncPreviewBegin,				\
		.previewShortExp = sensorDoFuncPreviewShortExp, 		\
		.previewLongExpBegin = NULL,							\
		.previewLongExpFollow = NULL,							\
		.previewLongExpEnd = NULL,								\
		.previewExpEnd = sensorDoFuncPreviewExpEnd, 			\
	},															\
	.crop = &( const sensorTgCrop_t ) { 						\
		.xOffset			= desc##_CROP_HOFFSET,				\
		.yOffset			= desc##_CROP_VOFFSET,				\
		.xSize				= desc##_CROP_HSIZE,				\
		.ySize				= desc##_CROP_VSIZE,				\
	},															\
	SEN_MACRO_OBWIN(desc)\
	SEN_MACRO_FRAMERATE(desc)\
	.hsize					= desc##_HSIZE ,					\
	.vsize					= desc##_VSIZE, 					\
	.sensor_model			= SENSORMODEL,						\
	.model_info 			= desc##_MODEL_INFO,				\
	.cfa_pattern			= desc##_CFAPATTERN,				\
	.hratio 				= desc##_HRATIO,					\
	.vratio 				= desc##_VRATIO,					\
	.sensor_pclk			= desc##_PCLK1X,					\
	.sensor_htotal			= desc##_LINETOTAL, 				\
	.pix_100khz 			= desc##_CDSPCLK,					\
	.bayerhs_size			= desc##_BAYERHS_SIZE,				\
	SEN_MACRO_BINCORR(desc)\
}																


#define SEN_MACRO_SNAP_TG_CFG(desc,opt)							\
{																\
	.fpdo = &(const sensorSnapDo_t) {							\
		.psnapEarly = (sensorDoFunc_t []){						\
			sensorDoFuncSnapEarly0, 							\
		},														\
		.snapExpBegin = sensorDoFuncSnapExpBegin,				\
		.snapBshutterBegin = sensorDoFuncSnapBshutterBegin, 	\
		.snapBshutterFollow = sensorDoFuncSnapBshutterFollow,	\
		.snapBshutterEnd = sensorDoFuncSnapBshutterEnd, 		\
		.snapLongExpBegin = NULL,								\
		.snapLongExpFollow = NULL,								\
		.snapLongExpEnd = NULL, 								\
		.snapShortExp = sensorDoFuncSnapShortExp,				\
		.snapExpEnd = sensorDoFuncSnapExpEnd,					\
		.psnapDummy = NULL, 									\
		.psnapReadoutField = (sensorDoFunc_t []) {				\
			sensorDoFuncSnapReadoutField0,						\
		},														\
		.snapFieldEnd = sensorDoFuncSnapFieldEnd,				\
		.snapReadoutEnd = sensorDoFuncSnapReadoutEnd,			\
		.snapNext = sensorDoFuncSnapNext,						\
	},															\
	.crop = NULL, /****TODO***/ 								\
	.hsize = desc##_HSIZE,										\
	.vsize = desc##_VSIZE,										\
	SEN_MACRO_OBWIN(desc)\
	.sensor_model 			= SENSORMODEL,						\
	.model_info 			= desc##_MODEL_INFO,				\
	.cfa_pattern	  		= desc##_CFAPATTERN,				\
	.hratio 				= desc##_HRATIO,					\
	.vratio 				= desc##_VRATIO,					\
	.n_dummy				= SNAP_DUMMYVD_NUM, 				\
	.n_readout				= SNAP_READOUTVD_NUM,				\
	.n_snap_early			= SNAP_EARLY_NUM,					\
	.skip_snap_readout_vd 	= opt,								\
	.fldsel_start			= FLDSEL_START_NUM, 				\
	.fldsel_end 			= FLDSEL_END_NUM,					\
	.sensor_pclk			= desc##_PCLK1X,					\
	.sensor_htotal			= desc##_LINETOTAL, 				\
	.pix_100khz 	  		= desc##_CDSPCLK,					\
}																

/*SyncSigGen*/

#if (SEN_MACRO_SSG_PCLK_EN)
#define SEN_MACRO_SSG_CLK(desc)   .ssg_pclk        = desc##_SSG_PCLK, 
#else
#define SEN_MACRO_SSG_CLK(desc)   .ssg_pclk        = 0, 
#endif

#if SEN_MACRO_TG_MASTER_EN
#define SEN_MACRO_SSGEN_SIZE(desc)								\
	.ssg_hblank 	 = desc##_SSG_HBLANK, 						\
	.ssg_htotal 	 = desc##_SSG_HTOTAL, 						\
	.ssg_vblank 	 = desc##_SSG_VBLANK, 						\
	.ssg_vtotal 	 = desc##_SSG_VTOTAL, 						\
	.ssg_vblank_base = desc##_SSG_VBASE,	                    \
	SEN_MACRO_SSG_CLK(desc)                                     
#else
#define SEN_MACRO_SSGEN_SIZE(desc)
#endif

#if SEN_MACRO_PARALLEL_EN
#define SEN_MACRO_LVDS_EN 0
#define SEN_MACRO_PARALLEL_CFG(desc)									\
	.conf_hdpol 	 = desc##_HDPOL,						\
	.conf_vdpol 	 = desc##_VDPOL,						\
	.conf_hdpclkedge = desc##_PCLKEDGE,					   	\
	.conf_hvldbypass = desc##_HREF,					   	   	\
	.conf_vvldbypass = desc##_VREF,					   	   	\
	.conf_yuvhdpol	 = desc##_HDORGPOL,				   	   	\
	.conf_yuvvdpol	 = desc##_VDORGPOL,
#else
#define SEN_MACRO_LVDS_EN 1
#define SEN_MACRO_PARALLEL_CFG(desc)
#endif

#if SEN_MACRO_LVDS_EN
#define SEN_MACRO_LVDS_CFG(desc)									\
	.pLVDScfg		= &desc##_LVDS_CFG,
#else
#define SEN_MACRO_LVDS_CFG(desc)									
#endif


#if SEN_MACRO_LVDS_SIZE_EN
#define SEN_MACRO_LVDS_SIZE_CFG(desc)									\
	.lvds_hsize 	 = desc##_LVDSHSIZE,					        \
	.lvds_vsize 	 = desc##_LVDSVSIZE,					
#else
#define SEN_MACRO_LVDS_SIZE_CFG(desc)
#endif

   
#define SEN_MACRO_SIG_CFG(desc)								\
{															\
	.linetotal		 = desc##_LINETOTAL,					\
	.frametotal 	 = desc##_FRAMETOTAL, 				   	\
	.hoffset		 = desc##_HOFFSET,					   	\
	.voffset		 = desc##_VOFFSET,					   	\
	.hsize			 = desc##_HSIZE , 					   	\
	.vsize			 = desc##_VSIZE,						\
	.lastlinepix	 = desc##_HSIZE,						\
	SEN_MACRO_LVDS_SIZE_CFG(desc)							\
	SEN_MACRO_PARALLEL_CFG(desc)							\
	.hdnum_cmdport	 = desc##_HDNUM_CMDPORT,				\
	.ext_hdnum		 = desc##_EXT_HDNUM,			   		\
	.fpclk			 = desc##_PCLKNX, 			   		   	\
	.pclk1x 		 = desc##_PCLK1X, 			   		   	\
	SEN_MACRO_SSGEN_SIZE(desc)								\
  SEN_MACRO_LVDS_CFG(desc)								\
}
/*========Macro definition===================================<<<<*/


#define SEN_MACRO_MODE_ENTRY(id,t,s,c,n,e) [id]={&t	, &s	, c[0] , CMD_LEN( c ), n ,e}


#ifdef __cplusplus
}
#endif

#endif  /* _HAL_FRONT_H_ */
