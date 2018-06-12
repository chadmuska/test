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
 * @file sp5k_aud_api.h
 *
 * @author Edward Kuo
 */
#ifndef _SP5K_AUD_API_H_
#define _SP5K_AUD_API_H_

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
#define SP5K_AUD_DEV_REC				0x80000000
#define SP5K_AUD_DEV_PLAY  				0x80000010
#define SP5K_AUD_DEV_REC_VOL_BOOST		0x80000020
#define SP5K_AUD_DEV_REC_VOL_BOOST_DB	0x80000030
#define SP5K_AUD_DEV_PLAY_VOL_GAIN		0x80000040

/* New definition for host to control iCatch's internal digital gain*/
#define SP5K_AUD_DEV_REC_INT			0x80000100
#define SP5K_AUD_DEV_PLAY_INT			0x80000110
/* New definition for host to control external codec digital gain */
#define SP5K_AUD_DEV_REC_EXT			SP5K_AUD_DEV_REC
#define SP5K_AUD_DEV_PLAY_EXT			SP5K_AUD_DEV_PLAY

#define SP5K_AUD_DEV_SPEAKER_ON_PIN  ( 0x00 )

typedef enum _sp5kAudDevRecSrc_e {
	SP5K_AUD_DEV_REC_SRC_NONE    = 0x00,
	SP5K_AUD_DEV_REC_SRC_LINE_IN = 0x10,
	SP5K_AUD_DEV_REC_SRC_MIC_IN  = 0x20,
} sp5kAudDevRecSrc_e;

typedef enum _sp5kAudDevPlaySrc_e {
	SP5K_AUD_DEV_PLAY_SRC_NONE      = 0,
	SP5K_AUD_DEV_PLAY_SRC_SPEAKER   = 1,
	SP5K_AUD_DEV_PLAY_SRC_HEADPHONE = 2,
	SP5K_AUD_DEV_PLAY_SRC_LINEOUT   = 3,
	SP5K_AUD_DEV_PLAY_SRC_SPEAKER_LINEOUT = 4,
} sp5kAudDevPlaySrc_e;

/* sp5kAudActDevSet(). */
typedef enum _sp5kAudDevId_e {
	SP5K_AUD_DEV_PRIMARY   = 0,
	SP5K_AUD_DEV_SECONDARY = 1,
	SP5K_AUD_DEV_TERTIARY  = 2,
	SP5K_AUD_DEV_TOT,
} sp5kAudDevId_e;  /* direct map to audDevId_e. */

typedef enum _sp5kAuRecdChnl_e {
	SP5K_AUD_REC_CH_LEFT  = 0x01,
	SP5K_AUD_REC_CH_RIGHT = 0x02,
	SP5K_AUD_REC_CH_ALL   = 0x03,
} sp5kAudRecChnl_e;  /* direct map to audRecChnl_e. */

typedef enum _sp5kAudPlayChnl_e {
	SP5K_AUD_PLAY_CH_LEFT  = 0x01,
	SP5K_AUD_PLAY_CH_RIGHT = 0x02,
	SP5K_AUD_PLAY_CH_ALL   = 0x03,
} sp5kAudPlayChnl_e;  /* direct map to audPlayChnl_e. */

typedef enum _sp5kAudFilterChnl_e {
	SP5K_AUD_FILTER_FIRST  = 0x01,
	SP5K_AUD_FILTER_SECOND = 0x02,
	SP5K_AUD_FILTER_ALL    = 0x03,
} sp5kAudFilterChnl_e; /* direct map to audFilterChnl_e. */

typedef enum _sp5kAudHPFilter_e {
	SP5K_HP_FILTER_DISABLE    = 0x00,
	SP5K_HP_FILTER_1ST_ORDER  = 0x01,
	SP5K_HP_FILTER_2ND_ORDER  = 0x02,
} sp5kAudHPFilter_e ;  /* direct map to audHPFilter_e. */

typedef enum {
	SP5K_AUD_STATUS_UNKNOWN    = 0,
	SP5K_AUD_STATUS_PLAY_STOP  = (1 << 0),
	SP5K_AUD_STATUS_PLAY_START = (1 << 1),
	SP5K_AUD_STATUS_PLAY_PAUSE = (1 << 2),
	SP5K_AUD_STATUS_REC_STOP   = (1 << 4),
	SP5K_AUD_STATUS_REC_START  = (1 << 5),
	SP5K_AUD_STATUS_REC_PAUSE  = (1 << 6),
} sp5kAudDevStatus_t;

typedef enum {
	SP5K_AUD_REC_LEVEL_WIN_SIZE_CFG,
	SP5K_AUD_REC_LEVEL_DETECT_EN,
	SP5K_AUD_REC_LEVEL_VALUE_GET,
} sp5kAudRecLevel_t;

/* argument devId in sp5kAudMixerPlayCtrlSet(). */
typedef enum _sp5kAudMixerPlayDevId_e {
	SP5K_AUD_MIXER_PLAY_DEV_0 = 0,
} sp5kAudMixerPlayDevId_e;

/* argument trkId in sp5kAudMixerPlayCtrlSet(). */
typedef enum _sp5kAudMixerPlayTrkId_e {
	SP5K_AUD_MIXER_PLAY_TRK_SOUND = 0,
	SP5K_AUD_MIXER_PLAY_TRK_MEDIA = 1,
} sp5kAudMixerPlayTrkId_e;

/* argument cfgId in sp5kAudMixerPlayCtrlSet(). */
typedef enum _sp5kAudMixerPlayCfgId_e {
	/*
	  trkId = not used, fill 0.
	  para1 = 0: direct output to device, 1: multi track mixing.
	*/
	SP5K_AUD_MIXER_PLAY_MIX_ENABLE,
	/*
	  para1 = volume, 0~256.
	*/
	SP5K_AUD_MIXER_PLAY_VOLUME,
} sp5kAudMixerPlayCfgId_e;

typedef enum _sp5kAudCfgId_e {
	SP5K_AUD_SPEAKER_AUTO_PWR_OFF,  /**< 0: disable, 1: enable speaker auto power off when playback done. */
	SP5K_AUD_AD_KEEP_ON,            /**< 0: disable, 1: enable On-chip audio ADC will not turn off. */
	SP5K_AUD_RAW_IN_CB_REG,			
	SP5K_AUD_I2S_KEEP_ON,
} sp5kAudCfgId_e;

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/
/* Equalizer shelf filter parameter */
typedef struct sp5kAudFilterEqShelf_s {
	UINT32 Fcentre;  /**< Centre frequency in unit of Hz. */
	float  gain;     /**< Gain value in unit of dB. */
} sp5kAudFilterEqShelf_t;  /* direct map to audFilterEqShelf_t. */

/* Equalizer peak filter parameter */
typedef struct sp5kAudFilterEqPeak_s {
	UINT32 Fcentre;  /**< Centre frequency in unit of Hz. */
	float  gain;     /**< Gain value in unit of dB. */
	float  Qfactor;  /**< Quality factor. */
} sp5kAudFilterEqPeak_t;  /* direct map to audFilterEqPeak_t. */

/* High-pass filter parameter */
typedef struct sp5kAudFilterHp_s {
	UINT32 type;       /**< SP5K_HP_FILTER_DISABLE: disable, SP5K_HP_FILTER_1ST_ORDER: enable 1st order high-pass filter, SP5K_HP_FILTER_2ND_ORDER: enable 2nd order high-pass filter. */
	UINT32 Fcentre;    /**< Centre frequency in unit of Hz. */
	float  Dfactor;    /**< Damping factor. */
} sp5kAudFilterHp_t;  /* direct map to audFilterHp_t. */

/* Notch filter parameter */
typedef struct sp5kAudFilterNotch_s {
	UINT32 en;       /**< 0: disable, 1: enable notch filter. */
	UINT32 Fcentre;  /**< Centre frequency in unit of Hz. */
	float  Qfactor;  /**< Quality factor. */
} sp5kAudFilterNotch_t;  /* direct map to audFilterNotch_t. */

/* argument pcfg in sp5kAudFilterEqCfg(). */
typedef struct sp5kAudFilterEqCfg_s {
	UINT32                 chnlId;      /**< Channel ID, one of sp5kAudFilterChnl_e. */
	UINT32                 en;          /**< 0: disable, 1: enable equalizer filter. */
	UINT32                 Fsample;     /**< Sampling frequency in unit of Hz. */
	sp5kAudFilterEqShelf_t lowShelf;    /**< Low shelf filter parameter. */
	sp5kAudFilterEqPeak_t  firstPeak;   /**< First peaking filter parameter. */
	sp5kAudFilterEqPeak_t  secondPeak;  /**< Second peaking filter parameter. */
	sp5kAudFilterEqPeak_t  thirdPeak;   /**< Third peaking filter parameter. */
	sp5kAudFilterEqShelf_t highShelf;   /**< High shelf filter parameter. */
} sp5kAudFilterEqCfg_t;  /* direct map to audFilterEqCfg_t. */

/* argument pcfg in sp5kAudFilterHpCfg(). */
typedef struct sp5kAudFilterHpCfg_s {
	UINT32            chnlId;       /**< Channel ID, one of sp5kAudFilterChnl_e. */
	UINT32            Fsample;      /**< Sampling frequency in unit of Hz. */
	sp5kAudFilterHp_t filter;       /**< high-pass filter parameter. */
} sp5kAudFilterHpCfg_t;  /* direct map to audFilterHpCfg_t. */

/* argument pcfg in sp5kAudFilterNotchCfg(). */
typedef struct sp5kAudFilterNotchCfg_s {
	UINT32               chnlId;       /**< Channel ID, one of sp5kAudFilterChnl_e. */
	UINT32               Fsample;      /**< Sampling frequency in unit of Hz. */
	sp5kAudFilterNotch_t firstNotch;   /**< First notch filter parameter. */
	sp5kAudFilterNotch_t secondNotch;  /**< Second notch filter parameter. */
	sp5kAudFilterNotch_t thirdNotch;   /**< Third notch filter parameter. */
	sp5kAudFilterNotch_t fourthNotch;  /**< Fourth notch filter parameter. */
	sp5kAudFilterNotch_t fifthNotch;   /**< Fifth notch filter parameter. */
} sp5kAudFilterNotchCfg_t;  /* direct map to audFilterNotchCfg_t. */

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
UINT32 sp5kAudActDevSet(UINT32 selector, UINT32 devId);
UINT32 sp5kAudDevSrcSet(UINT32 selector, UINT32 ioSrcType);
UINT32 sp5kAudDevSrcGet(UINT32 selector, UINT32 *pioSrcType);
UINT32 sp5kAudDevVolumeSet(UINT32 selector, UINT32 vol);
UINT32 sp5kAudDevVolumeGet(UINT32 selector, UINT32 *pvol);
void   sp5kAudDevStatusGet(UINT32 *pstatus);
void   sp5kAudDevVrefSet(UINT32 mask, UINT32 on);
UINT32 sp5kAudCfgSet(UINT32 cfgId, ...);

UINT32 sp5kAudDevRecLevelOp(UINT32 selector, void *val);

UINT32 sp5kAudMixerPlayCtrlSet(UINT32 devId, UINT32 trkId, UINT32 cfgId, ...);

/* FIXME: for 5110 host compatibility */
UINT32 sp5kAudDevSpeakerCtrl(UINT32 enable);
UINT32 sp5kAudDevPinCfg(UINT32 funcPin, UINT32 grp, UINT32 pinId, UINT32 polarity);

UINT32 sp5kAudFilterEqCfg(sp5kAudFilterEqCfg_t *pcfg);
UINT32 sp5kAudFilterHpCfg(sp5kAudFilterHpCfg_t *pcfg);
UINT32 sp5kAudFilterNotchCfg(sp5kAudFilterNotchCfg_t *pcfg);
void sp5kAudRecPreSet(void);
#endif  /* _SP5K_AUD_API_H_ */

