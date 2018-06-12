/**************************************************************************
 *
 *       Copyright (c) 2007-2015 by iCatch Technology, Inc.
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
/**
 * @file aud_dev_api.h
 *
 * @author
 */
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 28962 $
$Date:: 2017-08-17 16:42:15#$
 */

#ifndef _AUD_DEV_API_H_
#define _AUD_DEV_API_H_

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
typedef enum _audMode_e {
	AUD_MODE_REC  = (1<<0),
	AUD_MODE_PLAY = (1<<1),
	AUD_MODE_TOT  = (AUD_MODE_REC|AUD_MODE_PLAY),
} halAudMode_e;	/* direct map to halAudDev_e. */		

typedef enum _audCodecMode_e {
	AUD_CODEC_MODE_REC  = 0,
	AUD_CODEC_MODE_PLAY = 1,
	AUD_CODEC_MODE_TOT  = 2,
} audCodecMode_e;

typedef enum _audDevId_e {
	AUD_DEV_PRIMARY   = 0,
	AUD_DEV_SECONDARY = 1,
	AUD_DEV_TERTIARY  = 2,
	AUD_DEV_TOT,
} audDevId_e;

typedef enum _auRecdChnl_e {
	AUD_REC_CH_LEFT  = 0x01,
	AUD_REC_CH_RIGHT = 0x02,
	AUD_REC_CH_ALL   = 0x03,
} audRecChnl_e;  /* direct map to halAudRecChnl_e. */

typedef enum _audPlayChnl_e {
	AUD_PLAY_CH_LEFT  = 0x01,
	AUD_PLAY_CH_RIGHT = 0x02,
	AUD_PLAY_CH_ALL   = 0x03,
} audPlayChnl_e;  /* direct map to halAudPlayChnl_e. */

typedef enum _audFilterChnl_e {
	AUD_FILTER_FIRST  = 0x01,
	AUD_FILTER_SECOND = 0x02,
	AUD_FILTER_ALL    = 0x03,
} audFilterChnl_e;  /* direct map to halAudFilterChnl_e. */

typedef enum _audInputSel_e {
	AUD_IN_NONE = 0x00,
	AUD_IN_LINE = 0x10,
	AUD_IN_MIC  = 0x20,
} audInputSel_e;

typedef enum _audOutputSel_e {
	AUD_OUT_NONE      = 0,
	AUD_OUT_SPEAKER   = 1,
	AUD_OUT_HEADPHONE = 2,
	AUD_OUT_LINEOUT   = 3,
	AUD_OUT_SPEAKER_LINEOUT = 4,
} audOutputSel_e;

#if SPCA6350
/**
 baseFw maintainer MUST KEEP following information sync
 . audDevAttrMsk_e sync with halAudAttrMsk_e
 . audDevAttr_e    sync with halAudAttr_e
 */
#define AUD_DEV_PLAY_PATH_SHF_BIT (0)
#define AUD_DEV_I2S_PCM_MODE_SHF_BIT (8)
#define AUD_DEV_I2S_REC_SHF_SHF_BIT (14)
#define AUD_DEV_I2S_MSB_SHF_BIT (15)
#define AUD_DEV_INT_TRG_MODE_SHF_BIT (18)
#define AUD_DEV_AUD_DAC_AUTO_MUTE_SHF_BIT (21)
#define	AUD_DEV_FILTER_TEST_SHF_BIT (22)
#define AUD_DEV_FILTER_MUX_SHF_BIT (24)
#define AUD_DEV_BYPASS_PLAY_FILTER_SHF_BIT (26)
#define AUD_DEV_I2S_REC_PLAY_MODE_SHF_BIT (30)
#endif

typedef enum _audDevAttrMsk_e {
	/* 0x605c[7:0], extDacPath */
	AUD_DEV_PLAY_PATH_MSK         = (0x11 << (0+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< AUD_DEV_PLAY_PATH_NONE, AUD_DEV_PLAY_PATH_EXTERNAL, AUD_DEV_PLAY_PATH_INTERNAL and AUD_DEV_PLAY_PATH_BOTH. */
	AUD_DEV_REC_PATH_MSK          = (0x11 << (1+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< AUD_DEV_REC_PATH_NONE, AUD_DEV_REC_PATH_EXTERNAL, AUD_DEV_REC_PATH_INTERNAL and AUD_DEV_REC_PATH_BOTH. */
	AUD_DEV_I2S_MODE_MSK          = (0x01 << (2+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< AUD_DEV_I2S_MASTER and AUD_DEV_I2S_SLAVE. */
	AUD_DEV_REC_DUP_L_TO_R_MSK    = (0x01 << (3+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< AUD_DEV_REC_DUP_L_TO_R_DIS and AUD_DEV_REC_DUP_L_TO_R_EN. */
	AUD_DEV_PLAY_PATH_MUX_MSK     = (0x01 << (6+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< AUD_DEV_PLAY_PATH_EXT_MUX_DIS and AUD_DEV_PLAY_PATH_EXT_MUX_EN. */
	AUD_DEV_REC_PATH_MUX_MSK      = (0x01 << (7+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< AUD_DEV_REC_PATH_EXT_MUX_DIS and AUD_DEV_REC_PATH_EXT_MUX_EN. */
	/* 0x605d[5:4][2:0], recPcmMode */
	AUD_DEV_REC_I2S_PCM_MODE_MSK  = (0x01 << (0+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) AUD_DEV_REC_I2S_PCM_MODE_DIS and AUD_DEV_REC_I2S_PCM_MODE_EN. */
	AUD_DEV_PLAY_I2S_PCM_MODE_MSK = (0x01 << (1+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) AUD_DEV_PLAY_I2S_PCM_MODE_DIS and AUD_DEV_PLAY_I2S_PCM_MODE_EN. */
	AUD_DEV_I2S_PCM_MODE_SHF_MSK  = (0x01 << (2+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) AUD_DEV_I2S_PCM_MODE_SHF_0 and AUD_DEV_I2S_PCM_MODE_SHF_1. */
	AUD_DEV_REC_I2S_BCLK_INV_MSK  = (0x01 << (4+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) AUD_DEV_REC_I2S_BCLK_INV_DIS and AUD_DEV_REC_I2S_BCLK_INV_EN. */
	AUD_DEV_PLAY_I2S_BCLK_INV_MSK = (0x01 << (5+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) AUD_DEV_PLAY_I2S_BCLK_INV_DIS and AUD_DEV_PLAY_I2S_BCLK_INV_EN. */
	/* 0x6088[0], i2sRecShft */
	AUD_DEV_I2S_REC_SHF_MSK       = (0x01 << (0+AUD_DEV_I2S_REC_SHF_SHF_BIT)),  /**< AUD_DEV_I2S_REC_SHF_0 and AUD_DEV_I2S_REC_SHF_1. */
	/* 0x608c[2:0], i2sMsb */
	AUD_DEV_I2S_MSB_LSB_MSK       = (0x01 << (0+AUD_DEV_I2S_MSB_SHF_BIT)),		/**< AUD_DEV_I2S_MSB_FIRST and AUD_DEV_I2S_LSB_FIRST. */
	AUD_DEV_REC_LR_MSK            = (0x01 << (1+AUD_DEV_I2S_MSB_SHF_BIT)),		/**< AUD_DEV_REC_LR_NOSWAP and AUD_DEV_REC_LR_SWAP. */
	AUD_DEV_PLAY_LR_MSK           = (0x01 << (2+AUD_DEV_I2S_MSB_SHF_BIT)),		/**< AUD_DEV_PLAY_LR_NOSWAP and AUD_DEV_PLAY_LR_SWAP. */
	/* 0x6170[2][0], audIntMode */
	AUD_DEV_INT_MODE_MSK          = (0x01 << (0+AUD_DEV_INT_TRG_MODE_SHF_BIT)),	/**< AUD_DEV_INT_LEVEL_TRIG and AUD_DEV_INT_EDGE_TRIG. */
	AUD_DEV_DRAM_MODE_MSK         = (0x01 << (2+AUD_DEV_INT_TRG_MODE_SHF_BIT)),	/**< AUD_DEV_DRAM_AUTO_STOP and AUD_DEV_DRAM_ENDLESS_PLAY. */
	/* 0x642c[0], autoMute */
	AUD_DEV_DAC_AUTO_MUTE_MSK     = (0x01 << (0+AUD_DEV_AUD_DAC_AUTO_MUTE_SHF_BIT)),  /**< AUD_DEV_DAC_AUTO_MUTE_DIS and AUD_DEV_DAC_AUTO_MUTE_EN. */
	/**
	 Following setting MAY use for DEBUG ONLY
	*/
	/* 0x60a0[1:0], filterTest */
	AUD_DEV_FILTER_TEST_MSK       = (0x01 << (0+AUD_DEV_FILTER_TEST_SHF_BIT)),	/**< (V35) AUD_DEV_FILTER_TEST_MODE_DIS and AUD_DEV_FILTER_TEST_MODE_EN. */
	AUD_DEV_REC_LOOKBACK_MSK      = (0x01 << (1+AUD_DEV_FILTER_TEST_SHF_BIT)),	/**< (V35) AUD_DEV_REC_LOOPBACK_MODE_DIS and AUD_DEV_REC_LOOPBACK_MODE_EN. */
	/* 0x6178[1:0], filterMux */
	AUD_DEV_FILTER_MUX_MSK        = (0x03 << (0+AUD_DEV_FILTER_MUX_SHF_BIT)),	/**< (V33) AUD_DEV_FILTER_REC_FIRST, AUD_DEV_FILTER_REC_PLAY, AUD_DEV_FILTER_REC_ONLY and AUD_DEV_FILTER_PLAY_ONLY. */
	/* 0x6428[4:3], format */
	AUD_DEV_BYPASS_PLAY_FILTER_MUX =(0x01 << (3+AUD_DEV_BYPASS_PLAY_FILTER_SHF_BIT)),	/**< (V33) AUD_DEV_BYPASS_PLAY_FILTER_DIS and AUD_DEV_BYPASS_PLAY_FILTER_EN. */
	AUD_DEV_PLAY_SRC_SEL_MUX      = (0x01 << (4+AUD_DEV_BYPASS_PLAY_FILTER_SHF_BIT)),	/**< (V33) AUD_DEV_PLAY_SRC_SEL_0 and AUD_DEV_PLAY_SRC_SEL_1. */
	/* 0x60f1[1:0], plyslave */
	AUD_DEV_I2S_REC_MODE_MSK      = (0x03 << (0+AUD_DEV_I2S_REC_PLAY_MODE_SHF_BIT)),
	AUD_DEV_I2S_PLAY_MODE_MSK 	  =  AUD_DEV_I2S_REC_MODE_MSK 
} audDevAttrMsk_e;

typedef enum _audDevAttr_e {
	/* 0x605c[7:0], extDacPath */
	AUD_DEV_PLAY_PATH_NONE        = (0x00 << (0+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< Set reg extdacpath and intdacpath, both path disabled. */
	AUD_DEV_PLAY_PATH_EXTERNAL    = (0x01 << (0+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< Set reg extdacpath and intdacpath, external path enabled. */
	AUD_DEV_PLAY_PATH_INTERNAL    = (0x10 << (0+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< Set reg extdacpath and intdacpath, internal path enabled. */
	AUD_DEV_PLAY_PATH_BOTH        = (0x11 << (0+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< Set reg extdacpath and intdacpath, both path enabled. */
	AUD_DEV_REC_PATH_NONE         = (0x00 << (1+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< Set reg extadcpath and intadcpath, both path disabled. */
	AUD_DEV_REC_PATH_EXTERNAL     = (0x01 << (1+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< Set reg extadcpath and intadcpath, external path enabled. */
	AUD_DEV_REC_PATH_INTERNAL     = (0x10 << (1+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< Set reg extadcpath and intadcpath, internal path enabled. */
	AUD_DEV_REC_PATH_BOTH         = (0x11 << (1+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< Set reg extadcpath and intadcpath, both path enabled. */
	AUD_DEV_I2S_MASTER            = (0x00 << (2+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< Set reg i2sslave, drives clock signals to external I2S device. */
	AUD_DEV_I2S_SLAVE             = (0x01 << (2+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< Set reg i2sslave, clock signals drived by external I2S device. */
	AUD_DEV_REC_DUP_L_TO_R_DIS    = (0x00 << (3+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< Set reg duplicate_lr, not duplicate L-ch to R-ch when stereo recording. */
	AUD_DEV_REC_DUP_L_TO_R_EN     = (0x01 << (3+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< Set reg duplicate_lr, duplicate L-ch to R-ch when stereo recording. */
	AUD_DEV_PLAY_PATH_EXT_MUX_DIS = (0x00 << (6+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< (V35) Set reg extdacpath_sargpio */
	AUD_DEV_PLAY_PATH_EXT_MUX_EN  = (0x01 << (6+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< (V35) Set reg extdacpath_sargpio */
	AUD_DEV_REC_PATH_EXT_MUX_DIS  = (0x00 << (7+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< (V35) Set reg extadcpath_sargpio */
	AUD_DEV_REC_PATH_EXT_MUX_EN   = (0x01 << (7+AUD_DEV_PLAY_PATH_SHF_BIT)),	/**< (V35) Set reg extadcpath_sargpio */
	/* 0x605d[5:4][2:0], recPcmMode */
	AUD_DEV_REC_I2S_PCM_MODE_DIS  = (0 << (0+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) Set reg recPcmMode */
	AUD_DEV_REC_I2S_PCM_MODE_EN   = (1 << (0+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) Set reg recPcmMode */
	AUD_DEV_PLAY_I2S_PCM_MODE_DIS = (0 << (1+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) Set reg plyPcmMode */
	AUD_DEV_PLAY_I2S_PCM_MODE_EN  = (1 << (1+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) Set reg plyPcmMode */
	AUD_DEV_I2S_PCM_MODE_SHF_0    = (0 << (2+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) Set reg pcmShift */
	AUD_DEV_I2S_PCM_MODE_SHF_1    = (1 << (2+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) Set reg pcmShift */
	AUD_DEV_REC_I2S_BCLK_INV_DIS  = (0 << (4+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) Set reg recbclkinv, not inverse input bclk from external CODEC. */
	AUD_DEV_REC_I2S_BCLK_INV_EN   = (1 << (4+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) Set reg recbclkinv, inverse input bclk from external CODEC. */
	AUD_DEV_PLAY_I2S_BCLK_INV_DIS = (0 << (5+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) Set reg plybclkinv, not inverse input bclk from external CODEC. */
	AUD_DEV_PLAY_I2S_BCLK_INV_EN  = (1 << (5+AUD_DEV_I2S_PCM_MODE_SHF_BIT)),	/**< (V35) Set reg plybclkinv, inverse input bclk from external CODEC. */
	/* 0x6088[0], i2sRecShft */
	AUD_DEV_I2S_REC_SHF_0         = (0 << (0+AUD_DEV_I2S_REC_SHF_SHF_BIT)),		/**< Set reg i2srecshft, shift 0 bit. */
	AUD_DEV_I2S_REC_SHF_1         = (1 << (0+AUD_DEV_I2S_REC_SHF_SHF_BIT)),		/**< Set reg i2srecshft, shift 1 bit. if AUD_DEV_REC_LOOKBACK_EN is set, AUD_DEV_I2S_REC_SHF_1 also must be set. */
	/* 0x608c[2:0], i2sMsb */
	AUD_DEV_I2S_MSB_FIRST         = (0 << (0+AUD_DEV_I2S_MSB_SHF_BIT)),			/**< Set reg i2smsb, I2S data is MSB first. */
	AUD_DEV_I2S_LSB_FIRST         = (1 << (0+AUD_DEV_I2S_MSB_SHF_BIT)),			/**< Set reg i2smsb, I2S data is LSB first. */
	AUD_DEV_REC_LR_NOSWAP         = (0 << (1+AUD_DEV_I2S_MSB_SHF_BIT)),			/**< Set reg reclrswap, not swap LR channels in record mode. */
	AUD_DEV_REC_LR_SWAP           = (1 << (1+AUD_DEV_I2S_MSB_SHF_BIT)),			/**< Set reg reclrswap, swap L/R channels in record mode. */
	AUD_DEV_PLAY_LR_NOSWAP        = (0 << (2+AUD_DEV_I2S_MSB_SHF_BIT)),			/**< Set reg plylrswap, not swap LR channels in play mode. */
	AUD_DEV_PLAY_LR_SWAP          = (1 << (2+AUD_DEV_I2S_MSB_SHF_BIT)),			/**< Set reg plylrswap, swap L/R channels in play mode. */
	/* 0x6170[2][0], audIntMode */
	AUD_DEV_INT_LEVEL_TRIG		   = (0 << (0+AUD_DEV_INT_TRG_MODE_SHF_BIT)),	/**< Set reg audintmode, audio interrupt level trigger. */
	AUD_DEV_INT_EDGE_TRIG		   = (1 << (0+AUD_DEV_INT_TRG_MODE_SHF_BIT)),	/**< Set reg audintmode, audio interrupt edge trigger. */
	AUD_DEV_DRAM_AUTO_STOP		   = (0 << (2+AUD_DEV_INT_TRG_MODE_SHF_BIT)),	/**< Set reg drammode, automatic stop when buffer underflow. */
	AUD_DEV_DRAM_ENDLESS_PLAY	   = (1 << (2+AUD_DEV_INT_TRG_MODE_SHF_BIT)),	/**< Set reg drammode, endless play even buffer underflow. */
	/* 0x642c[0], autoMute */
	AUD_DEV_DAC_AUTO_MUTE_DIS	   = (0 << (0+AUD_DEV_AUD_DAC_AUTO_MUTE_SHF_BIT)),	/**< Set reg automute, disable DAC auto mute. */
	AUD_DEV_DAC_AUTO_MUTE_EN	   = (1 << (0+AUD_DEV_AUD_DAC_AUTO_MUTE_SHF_BIT)),	/**< Set reg automute, enable DAC auto mute after about 11 seconds. */
	/**
	 Following setting MAY use for DEBUG ONLY
	*/
	/* 0x60a0[1:0], filterTest */
	AUD_DEV_FILTER_TEST_MODE_DIS   = (0 << (0+AUD_DEV_FILTER_TEST_SHF_BIT)),	/**< (V35) Set reg filterTest */
	AUD_DEV_FILTER_TEST_MODE_EN    = (1 << (0+AUD_DEV_FILTER_TEST_SHF_BIT)),	/**< (V35) Set reg filterTest */
	AUD_DEV_REC_LOOPBACK_MODE_DIS  = (0 << (1+AUD_DEV_FILTER_TEST_SHF_BIT)),	/**< (V35) Set reg recLbTest */
	AUD_DEV_REC_LOOPBACK_MODE_EN   = (1 << (1+AUD_DEV_FILTER_TEST_SHF_BIT)),	/**< (V35) Set reg recLbTest */
	/* 0x6178[1:0], filterMux */
	AUD_DEV_FILTER_REC_FIRST	   = (0 << (0+AUD_DEV_FILTER_MUX_SHF_BIT)), 	/**< (V33) Set reg filterMux, both filter for record, or both filter for play if record is disabled. */
	AUD_DEV_FILTER_REC_PLAY 	   = (1 << (0+AUD_DEV_FILTER_MUX_SHF_BIT)), 	/**< (V33) Set reg filterMux, one filter for record (L-ch), and one filter for play (L-ch). */
	AUD_DEV_FILTER_REC_ONLY 	   = (2 << (0+AUD_DEV_FILTER_MUX_SHF_BIT)), 	/**< (V33) Set reg filterMux, both filter for record. */
	AUD_DEV_FILTER_PLAY_ONLY	   = (3 << (0+AUD_DEV_FILTER_MUX_SHF_BIT)), 	/**< (V33) Set reg filterMux, both filter for play. */
	/* 0x6428[4:3], format */
	AUD_DEV_BYPASS_PLAY_FILTER_DIS = (0 << (3+AUD_DEV_BYPASS_PLAY_FILTER_SHF_BIT)), /**< (V33) Set reg bpFir, play with filter. (DEFAULT) */
	AUD_DEV_BYPASS_PLAY_FILTER_EN  = (1 << (3+AUD_DEV_BYPASS_PLAY_FILTER_SHF_BIT)), /**< (V33) Set reg bpFir, play without filter. (DEBUG ONLY) */
	AUD_DEV_PLAY_SRC_SEL_0		   = (0 << (4+AUD_DEV_BYPASS_PLAY_FILTER_SHF_BIT)), /**< (V33) Set reg lnos, audio render (LineOut or Amp) input from DAC output data. (DEFAULT) */
	AUD_DEV_PLAY_SRC_SEL_1		   = (1 << (4+AUD_DEV_BYPASS_PLAY_FILTER_SHF_BIT)), /**< (V33) Set reg lnos, audio render (LineOut or Amp) input from ADC input data. (DEBUG ONLY) */
	/* 0x60f1[1:0], plyslave */
	AUD_DEV_I2S_PLAY_MASTER 	   = (0 << (0+AUD_DEV_I2S_REC_PLAY_MODE_SHF_BIT)),
	AUD_DEV_I2S_PLAY_SLAVE		   = (1 << (0+AUD_DEV_I2S_REC_PLAY_MODE_SHF_BIT)),	
	AUD_DEV_I2S_REC_MASTER		   = (0 << (1+AUD_DEV_I2S_REC_PLAY_MODE_SHF_BIT)),	
	AUD_DEV_I2S_REC_SLAVE		   = (1 << (1+AUD_DEV_I2S_REC_PLAY_MODE_SHF_BIT)),	
} audDevAttr_e;

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef UINT32 (*pfAudGpioDir_t)(UINT32, UINT32, UINT32);
typedef UINT32 (*pfAudGpioRd_t)(UINT32, UINT32, UINT32 *);
typedef UINT32 (*pfAudGpioWr_t)(UINT32, UINT32, UINT32);

typedef struct audCodecItf_s {
	void (*pf_switch[AUD_CODEC_MODE_TOT])(void);
	void (*pf_init[AUD_CODEC_MODE_TOT])(void);
	void (*pf_vol_set[AUD_CODEC_MODE_TOT])(UINT32, UINT32);
	void (*pf_vol_get[AUD_CODEC_MODE_TOT])(UINT32, UINT32 *);
	void (*pf_output_set)(UINT32);
	void (*pf_output_get)(UINT32 *);
	void (*pf_input_set)(UINT32);
	void (*pf_input_get)(UINT32 *);
} audCodecItf_t;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
void   audCodecRegister(audCodecItf_t *pitfFuncs);
void   audCodecActSet(UINT32 mode, UINT32 devId);
void   audCodecInit(UINT32 mode);
void   audCodecVolumeSet(UINT32 mode, UINT32 chnlId, UINT32 vol);
void   audCodecVolumeGet(UINT32 mode, UINT32 chnlId, UINT32 *pvol);
void   audCodecOutputSet(UINT32 sel);
void   audCodecOutputGet(UINT32 *psel);
void   audCodecInputSet(UINT32 sel);
void   audCodecInputGet(UINT32 *psel);
void   audExtCodecClkEn(UINT32 mode, UINT32 en);
/**
 @brief Set interface related attribute for specified audio device
 @param[in] msk attribute mask. New value will be (old value & ~mask)|(input value & mask).
 @param[in] val attribute value. New value will be (old value & ~mask)|(input value & mask).
 */
void   audDevAttrSet(UINT32 chnlId, UINT32 msk, UINT32 val);

/* FIXME: for 5110 host compatibility */
void   audSpeakerOnPinCfg(pfAudGpioDir_t pfcfg, pfAudGpioWr_t pfwr, UINT32 grp, UINT32 pinId, UINT32 polar);
void   audSpeakerCtrl(UINT32 enable);
UINT32 audSpeakerAutoCtrlEn(UINT32 enable);
UINT32 audOutputDeviceGet(void);

UINT32 audMicVolumeSet(UINT32 chnlId, SINT16 vol);
UINT32 audMicVolumeGet(UINT32 chnlId, SINT16 *vol);
void   audRecLRSwapSet(UINT32 en);
UINT32 audRecLRSwapGet(void);

#endif  /* _AUD_DEV_API_H_ */

