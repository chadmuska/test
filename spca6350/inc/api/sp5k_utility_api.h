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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file sp5k_utility_api.h
 *
 * @author Eric Su
 */
#ifndef _SP5K_UTILITY_API_H_
#define _SP5K_UTILITY_API_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "middleware/common_types.h"
#include "middleware/speffect_def.h"

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
#define SP5K_SSC_PORT_TIMEOUT	0x80000000
#define SP5K_PORT_MASK_DMA_MODE 0x80000000

typedef enum _sp5kSscPortId_e {
	SP5K_SSC_PORT_0 = 0x00,/*SIF1 front*/
	SP5K_SSC_PORT_1 = 0x10,/*SIF2*/
	SP5K_SSC_PORT_2 = 0x20,/*Embedded SPI*/
	SP5K_SSC_PORT_3 = 0x30,/*GSI 0*/
	SP5K_SSC_PORT_4 = 0x40,/*GSI 1*/
	SP5K_SSC_PORT_5 = 0x50,/*I2C 0*/
	SP5K_SSC_PORT_6 = 0x60,/*I2C 1*/
} sp5kSscPortId_e;

#define SP5K_SSC_PORT_SIF1                  SP5K_SSC_PORT_0
#define SP5K_SSC_PORT_SIF2                  SP5K_SSC_PORT_1
#define SP5K_SSC_PORT_ESPI                  SP5K_SSC_PORT_2
#define SP5K_SSC_PORT_GSI0                  SP5K_SSC_PORT_3
#define SP5K_SSC_PORT_GSI1                  SP5K_SSC_PORT_4
#define SP5K_SSC_PORT_I2C0                  SP5K_SSC_PORT_5
#define SP5K_SSC_PORT_I2C1                  SP5K_SSC_PORT_6

typedef enum _sp5kSscCfg_e {
	SP5K_SSC_CFG_MODE,
	SP5K_SSC_CFG_SYNC,    /* not open? #1:9208 */
	SP5K_SSC_CFG_PIN, /* 0:(lmi10 or lmi11)/tg12/tg13(I2C) or lcdif (SPI), 1:(lmi10 or lmi11)/lmi8/lmi9 */
	SP5K_GSI_CFG_PADDING1, /* avoid all host lib rebuild */
	SP5K_SPI_CFG_CS,  /* obsolete */
	SP5K_SPI_CFG_PHY, /* #1 9200(sifmode) 925c(cs) 9152(ini) 9254(pos) 9150(freq) */
					  /* #3 9200(sifmode) 925c(cs) 9152(ini) 9254(pos) 9150(freq) */
	SP5K_I2C_CFG_PHY,
	SP5K_I2C_CFG_SETUP_HOLD_TIME,
	SP5K_SSC_LOCK,    /* #1, #2 FW lock for sif2, 4050[4:6] */
	SP5K_SPI_WRITE_ONLY,
	SP5K_SSC_DISABLE,  /*de-init*/
} sp5kSscCfg_t;

/* SP5K_SSC_CFG_MODE option */
typedef enum _sp5kSscPortType_e {
	SP5K_SSC_MODE_I2C,
	SP5K_SSC_MODE_SPI,
	SP5K_SSC_MODE_MAX
} sp5kSscPortType_t;


/* SP5K_SSC_CFG_SYNC : N/A */


/* SP5K_SSC_CFG_PIN */
/* most ports support 2 pin only */
typedef enum _sp5kSscPin_e {
	SP5K_SSC_CFG_PIN0,
	SP5K_SSC_CFG_PIN1,
	SP5K_SSC_CFG_PIN2,
	SP5K_SSC_CFG_PIN3,
	SP5K_SSC_CFG_PIN4,
	SP5K_SSC_CFG_PIN5,
	SP5K_SSC_CFG_PIN6,
	SP5K_SSC_CFG_PIN7,
	SP5K_SSC_CFG_PIN8,
	SP5K_SSC_CFG_PIN9,
	SP5K_SSC_CFG_PIN10,
	SP5K_SSC_CFG_PIN11,
	SP5K_SSC_CFG_PIN_MAX,
} sp5kSsc1Pin_t;

#define SP5K_SSC_CFG_PIN_NO_CS 0x80000000

/* SP5K_SPI_CFG_PHY */
typedef enum {
	SP5K_SPI_PORT2_CLK_48MHZ,
	SP5K_SPI_PORT2_CLK_24MHZ,
	SP5K_SPI_PORT2_CLK_12MHZ,
	SP5K_SPI_PORT2_CLK_6MHZ,
	SP5K_SPI_PORT2_CLK_3MHZ,
	SP5K_SPI_PORT2_CLK_1500KHZ,
	SP5K_SPI_PORT2_CLK_375KHZ,
	SP5K_SPI_PORT2_CLK_96MHZ,
	SP5K_SPI_PORT2_CLK_MAX,
} sp5kSpiPort2Clock_t;

typedef enum {
	SP5K_SPI_GSI_CLK_DIVISOR_2, /* ~48MHz */
	SP5K_SPI_GSI_CLK_DIVISOR_4,
	SP5K_SPI_GSI_CLK_DIVISOR_8,
	SP5K_SPI_GSI_CLK_DIVISOR_16,
	SP5K_SPI_GSI_CLK_DIVISOR_32,
	SP5K_SPI_GSI_CLK_DIVISOR_64, /* 1.5MHz */
	SP5K_SPI_GSI_CLK_DIVISOR_128,
	SP5K_SPI_GSI_CLK_DIVISOR_256,
	SP5K_SPI_GSI_CLK_DIVISOR_MAX,
} sp5kSpiGsiClockDivisor_t;
#if 0
typedef enum {
	SP5K_GSI_CLK_48MHZ,
	SP5K_GSI_CLK_24MHZ,
	SP5K_GSI_CLK_12MHZ,
	SP5K_GSI_CLK_6MZ,
	SP5K_GSI_CLK_3MHZ,/*4*/
	SP5K_GSI_CLK_1500KHZ,
	SP5K_GSI_CLK_750KHZ,
	SP5K_GSI_CLK_375KHZ,
} sp5kSpiGsiClock_t;
#endif

#define SP5K_SPI_CLK_IDLE_LOW_LATCH_RISE	0
#define SP5K_SPI_CLK_IDLE_LOW_LATCH_FALL	1
#define SP5K_SPI_CLK_IDLE_HIGH_LATCH_FALL	2
#define SP5K_SPI_CLK_IDLE_HIGH_LATCH_RISE	3
#define SP5K_SPI_CLK_IDLE_LOW_LATCH_RISE_1T	4 /* same as SP5K_SPI_CLK_IDLE_LOW_LATCH_RISE but with 1T delay */
#define SP5K_SPI_SDO_IDLE_HIGH				8


/* SP5K_I2C_CFG_PHY */
#if 0
typedef enum _sp5kI2cClockDivisor_e {
	SP5K_I2C_CLK_DIVISOR_8192,
	SP5K_I2C_CLK_DIVISOR_4096,
	SP5K_I2C_CLK_DIVISOR_2048,
	SP5K_I2C_CLK_DIVISOR_1024,
	SP5K_I2C_CLK_DIVISOR_512,
	SP5K_I2C_CLK_DIVISOR_256,
	SP5K_I2C_CLK_DIVISOR_128,
	SP5K_I2C_CLK_DIVISOR_64,
	SP5K_I2C_CLK_DIVISOR_MAX,
} sp5kI2cClockDivisor_e;
#endif


#define SP5K_I2C_CFG_SLAVE_ADDR_16BIT		0x40
#define SP5K_I2C_CFG_SUB_ADDR_16BIT			0x20
#define SP5K_I2C_CFG_SUB_ADDR_NONE			0x10

#define SP5K_I2C_READ_MODE_NO_DUMMY_WRITE	0
#define SP5K_I2C_READ_MODE_DUMMY_WRITE		1
#define SP5K_I2C_READ_MODE_2				2 /* dummy write + stop bit omission */

#define SP5K_I2C_WRITE_MODE_SEQ				0
#define SP5K_I2C_WRITE_MODE_BURST			1

typedef enum sp5kUartPort_e {
	SP5K_UART_PORT_DEBUG,
	SP5K_UART_PORT_PERI_0,
	SP5K_UART_PORT_PERI_1,
} sp5kUartPort_t;

typedef enum _sp5kUartCfgSelector_e {
	SP5K_UART_BAUDRATE,
	SP5K_UART_TIME_OUT,
	SP5K_UART_FLOWCTRL,			/* 0 for SW, 1 for HW */
	SP5K_UART_TX_TIMEOUT,
	SP5K_UART_RX_TIMEOUT,
	SP5K_UART_PIN_SEL,
	SP5K_UART_PARITY,
	SP5K_UART_CHAR_LEN,
	SP5K_UART_STOP_BIT,
} sp5kUartCfgSelector_e;

typedef enum _sp5kUartParity_e{
	SP5K_UART_PARITY_DISABLE,
	SP5K_UART_PARITY_ODD,
	SP5K_UART_PARITY_EVEN,
}_sp5kUartParity_t;

typedef enum _sp5kYuvScaleFormat_e {
	SP5K_SCALE_FORMAT_YUV = 0,
	SP5K_SCALE_FORMAT_RGB = 1,
} sp5kYuvScaleFormat_e;

/* for sp5kUtilYuvFilter() */
typedef enum _sp5kUtilYuvFilterSel_e {
	SP5K_YUV_FILTER_1D_PERSPECTIVE,
	SP5K_YUV_FILTER_RADIAL_BLUR,
	SP5K_YUV_FILTER_HDR_MARK_NORMAL,
	SP5K_YUV_FILTER_HDR_MARK_UNDER,
	SP5K_YUV_FILTER_HDR_MARK_OVER,
	SP5K_YUV_FILTER_HDR_COMPOSE,
	SP5K_YUV_FILTER_WDR,
	SP5K_YUV_FILTER_TOTAL,
} sp5kUtilYuvFilterSel_e;

typedef enum _peripheralSysCfg_e {
	PERIPHERAL_SYS_CFG_SIF2_PIN, /* obsolete */
	PERIPHERAL_SYS_CFG_TOTAL,
} peripheralSysCfg_e;

typedef enum _sp5kUartModeSelector_e {
	SP5K_UART_MODE_PIO = 0,
	SP5K_UART_MODE_DMA = 1,
} sp5kUartModeSelector_e;

/* for sp5kSuspendResume() */
typedef enum _sp5kSuspendResult_e {
	#if SPCA6350
	SP5K_RESUME_SRC_LMIGPIO08  = (1 <<  0),
	SP5K_RESUME_SRC_LMIGPIO09  = (1 <<  1),
	SP5K_RESUME_SRC_LMIGPIO16  = (1 <<  2),
	SP5K_RESUME_SRC_LMIGPIO17  = (1 <<  3),
	SP5K_RESUME_SRC_LMIGPIO18  = (1 <<  4),
	SP5K_RESUME_SRC_LMIGPIO19  = (1 <<  5),
	SP5K_RESUME_SRC_LMIGPIO20  = (1 <<  6),
	SP5K_RESUME_SRC_LMIGPIO21  = (1 <<  7),
	SP5K_RESUME_SRC_FMGPIO00   = (1 <<  8),
	SP5K_RESUME_SRC_FMGPIO01   = (1 <<  9),
	SP5K_RESUME_SRC_FMGPIO24   = (1 << 10),
	SP5K_RESUME_SRC_FMGPIO25   = (1 << 11),
	SP5K_RESUME_SRC_FMGPIO26   = (1 << 12),
	SP5K_RESUME_SRC_FMGPIO27   = (1 << 13),
	SP5K_RESUME_SRC_FMGPIO28   = (1 << 14),
	SP5K_RESUME_SRC_FMGPIO29   = (1 << 15),
	SP5K_RESUME_SRC_FMGPIO30   = (1 << 16),
	SP5K_RESUME_SRC_FMGPIO31   = (1 << 17),
	SP5K_RESUME_SRC_FMGPIO45   = (1 << 18),
	SP5K_RESUME_SRC_FMGPIO46   = (1 << 19),
	SP5K_RESUME_SRC_DISPGPIO17 = (1 << 21),
	SP5K_RESUME_SRC_GPIO08     = (1 << 22),
	SP5K_RESUME_SRC_GPIO10     = (1 << 23),
	SP5K_RESUME_SRC_TGGPIO02   = (1 << 24),
	SP5K_RESUME_SRC_TGGPIO03   = (1 << 25),
	SP5K_RESUME_SRC_PWRON0     = (1 << 26),
	SP5K_RESUME_SRC_PWRON1     = (1 << 27),
	SP5K_RESUME_SRC_PWRON2     = (1 << 28),
	SP5K_RESUME_SRC_PWRON3     = (1 << 29),
	SP5K_RESUME_SRC_PWRON4     = (1 << 30),
	SP5K_RESUME_SRC_PWRON      = SP5K_RESUME_SRC_PWRON0,
	SP5K_RESUME_SRC_USB        = SP5K_RESUME_SRC_PWRON1,
	SP5K_RESUME_SRC_PLAYBACK   = SP5K_RESUME_SRC_PWRON2,
	#else
	SP5K_RESUME_SRC_DISP_GPIO17_RISE 	= (1<<0),
	SP5K_RESUME_SRC_DISP_GPIO17_FALL 	= (1<<12),
	SP5K_RESUME_SRC_PWRON0_RISE			= (1<<24),
	SP5K_RESUME_SRC_PWRON1_RISE			= (1<<25),
	SP5K_RESUME_SRC_PWRON2_RISE			= (1<<26),
	SP5K_RESUME_SRC_PWRON0_FALL			= (1<<28),
	SP5K_RESUME_SRC_PWRON1_FALL			= (1<<29),
	SP5K_RESUME_SRC_PWRON2_FALL			= (1<<30),
	#endif
} sp5kSuspendResult_e;

typedef enum sp5kYuvHdrCfg_e {
	SP5K_YUV_HDR_CFG_LEVEL,
	SP5K_YUV_HDR_CFG_EM_TBL_M,
	SP5K_YUV_HDR_CFG_EM_TBL_L,
	SP5K_YUV_HDR_CFG_DEGHOST_LEVEL,
	SP5K_YUV_HDR_CFG_DEGHOST_MODE,
	SP5K_YUV_HDR_CFG_TOTAL,
} sp5kYuvHdrCfg_t;

/* for sp5kDscFirmwareBoot */
#define SP5K_BOOT_NORMAL_FW1  ":DEF_FW1"
#define SP5K_BOOT_NORMAL_FW2  ":DEF_FW2"
#define SP5K_BOOT_MBOOT_FW1   ":MB_FW1"
#define SP5K_BOOT_MBOOT_FW2   ":MB_FW2"

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/
/*
 * The physical yuv scale parameters. All the coordinate system is in physical
 * domain.
 */
typedef struct sp5kYuvScale_s {
	UINT8 *psrcAddr;  /*!< The source buffer address to be scaled */
	UINT8 *pdstAddr;  /*!< The output destination address */
	UINT16 srcWidth;  /*!< Width of source buffer, 16x */
	UINT16 srcHeight; /*!< Height of source buffer, 8x */
	UINT16 dstWidth;  /*!< Width of destination buffer, 16x */
	UINT16 dstHeight; /*!< Height of destination buffer, 8x */
	UINT16 cropWidth; /*!< ROI width of source buffer, 16x */
	UINT16 cropHeight;/*!< ROI height of source buffer, 8x */
	UINT16 cropOffX;  /*!< ROI x offset of source buffer, 2x */
	UINT16 cropOffY;  /*!< ROI y offset of source buffer */
} sp5kYuvScale_t;

/*
 * The virtual yuv scale parameters. The source parameters are based on vitual
 * (logical) coordinate system. The destination parameters are still based on
 * physical coordinate system.
 */
typedef struct sp5kYuvScaleVirtual_s {
	UINT8 *psrcAddr;  /*!< The source buffer address to be scaled */
	UINT8 *pdstAddr;  /*!< The output destination address */
	UINT16 srcWidth;  /*!< Virtual width of source buffer */
	UINT16 srcHeight; /*!< Virtual height of source buffer */
	UINT16 dstWidth;  /*!< Width of destination buffer, 16x */
	UINT16 dstHeight; /*!< Height of destination buffer, 8x */
	UINT16 cropWidth; /*!< ROI width of source buffer (Virtual) */
	UINT16 cropHeight;/*!< ROI height of source buffer (Virtual) */
	UINT16 cropOffX;  /*!< ROI x offset of source buffer (Virtual) */
	UINT16 cropOffY;  /*!< ROI y offset of source buffer (Virtual) */
	UINT16 baseWidth; /*!< The real physical width of source buffer */
	UINT16 baseHeight;/*!< The real physical height of source buffer */
} sp5kYuvScaleVirtual_t;

typedef void (*fpsp5kUtilCardIspCallback_t)(UINT32 elapseFileCnt, UINT32 totalFileCnt);
typedef void (*fpsp5kUtilNormalIspCallback_t)(void);

/*
 * The virtual yuv scale parameters. The source parameters are based on vitual
 * (logical) coordinate system. The destination parameters are still based on
 * physical coordinate system.
 */
typedef struct sp5kYuvRotate_s {
	UINT8 *psrcAddr;  /*!< the source buffer address to be rotate, 16X */
	UINT8 *pdstAddr;  /*!< the output destination address, 16X */
	UINT16 srcWidth;  /*!< source width of source buffer, 16X */
	UINT16 srcHeight; /*!< destination height of source buffer, 16X */
	UINT32 rotate;    /*!< rotate degree */
} sp5kYuvRotate_t;

/**
 * \note
 * pbuf: the YUV buffer address, must align to 256
 * bufW: the width of the yuv buffer, align to 16
 * bufH: the height of the yuv buffer, align to 16
 * roiX: the offset x of the ROI, align to 2
 * roiY: the offset y of the ROI
 */
#define sp5kYuvBuf_t	effectYuvBuf_t

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/
UINT32 sp5kSscCfgSet(UINT32,UINT32,UINT32 );
UINT32 sp5kI2cRead(UINT32 ,UINT32 ,UINT8 *, UINT32);
UINT32 sp5kI2cWrite( UINT32 ,UINT32, UINT8 *,UINT32);
UINT32 sp5kSpiRead(UINT32, UINT8 *, UINT32);
UINT32 sp5kSpiWrite(UINT32, const UINT8 *, UINT32 );
UINT32 sp5kUartInit(UINT32 port, UINT32 baud);
UINT32 sp5kUartDeInit(UINT32 port);
UINT32 sp5kUartRead(UINT32 port, UINT32 *ch);
UINT32 sp5kUartBufRead(UINT32 port, UINT8 *buf, UINT32 len);
UINT32 sp5kUartWrite(UINT32 port, UINT8 *ch);
UINT32 sp5kUartBufWrite(UINT32 port, UINT8 *buf, UINT32 len);
UINT32 sp5kUartRxCallbackSet(UINT32 port, int (*pf)(int));
UINT32 sp5kUartModeSet(UINT32 port, UINT32 mode);
UINT8  sp5kUartModeGet(UINT32 port);
UINT32 sp5kUartCfgSet(UINT32 port, sp5kUartCfgSelector_e selector, UINT32 value);
UINT32 sp5kPowerCtrl(UINT32 option, UINT32 ms);
UINT32 sp5kDscFirmwareBoot(UINT8 *pfile, UINT32 addr);
UINT32 sp5kDscFirmwareUpdate(UINT8 *pfilename, fpsp5kUtilCardIspCallback_t pfunc);
UINT32 sp5kDscFirmwareUpdateWithBuf(UINT8* fileBuf, UINT32 bufSize, UINT8 devId, fpsp5kUtilCardIspCallback_t pfunc);
UINT32 sp5kDscFirmwareVerify(UINT32 fwnum, UINT32 rsv);
#if SPCA6350
UINT32 sp5kSuspendResume(UINT32 resumeRiseSrc, UINT32 resumeFallSrc);
#else
UINT32 sp5kSuspendResume(UINT32 resumeSrc);
#endif
#if !SPCA6350
UINT32 sp5kUtilYuvScale(sp5kYuvScale_t * param, sp5kYuvScaleFormat_e format);
UINT32 sp5kUtilYuvScaleVirtual(sp5kYuvScaleVirtual_t *param, sp5kYuvScaleFormat_e format); /* obsolete */
UINT32 sp5kUtilPbBufGet(UINT8 **ppbuf, UINT32 *pwidth, UINT32 *pheight, UINT32 *plinebyte);
#endif
void   sp5kUtilYuvToRgb565Convert(UINT8 y,UINT8 u,UINT8 v,UINT16 *prgb565);
void   sp5kUtilRgb565ToYuvConvert(UINT16 rgb565,UINT8 *py,UINT8 *pu,UINT8 *pv);
#if !SPCA6350
UINT32 sp5kUtilYuvFilter(UINT32 sel,UINT32 opt,UINT8 *p,UINT32 w,UINT32 h);
UINT32 sp5kUtilYuvRotate(sp5kYuvRotate_t *param,UINT32 rotate_op);
#if 0
/**
 * \param[in] effect the ID of effect in speffectID_t
 * \param[in] psrcBuf the source YUV buffer
 * \param[in] pdstBuf the destination YUV buffer
 * \param[in] roiW the width of the ROI to apply the effect
 * \param[in] roiH the height of the ROI to apply the effect
 * \param[in] para1 the 1st input argument
 * \param[in] para2 the 2nd input argument
 * \note
 * if effect == SPE_EFFECT_MOSAIC
 *    para1: the block size of mosaic in pixel
 *    para2: not used
 * othres are not implemented yet
 */
#endif
UINT32 sp5kUtilYuvEffect(UINT32 effect, sp5kYuvBuf_t *psrcBuf, sp5kYuvBuf_t *pdstBuf,
						 UINT32 roiW, UINT32 roiH, UINT32 para1, UINT32 para2);
#endif

/**
 * @brief Allocate a frame buffer in YUV format.
 * @param [in] bufW Frame buffer width (unit in pixel).
 * @param [in] bufH Frame buffer height (uint in pixel).
 * @return Frame buffer address.
 */
void *sp5kYuvBufferAlloc(UINT32 bufW, UINT32 bufH);
/**
 * @brief Free a frame buffer in YUV format.
 * @param [in] ptr Allocated frame buffer address.
 * @return None.
 */
void sp5kYuvBufferFree(void *ptr);
/**
 @brief Switch PLL to specific frequency. Also apply new DRAMPARA which adapt to new DRAM clock.
 @param[in] clk specific clock frequency.
 @retval SUCCESS 0 while success
 @retval FAIL Fail to switch PLL. Usually happened while no DRAMPARA within DMEM.
 */
UINT32 sp5kSystemPllSet(UINT32 clk);

/**
 @brief Playback WAV buffer (PCM only, no suport ADPCM) before whole FW.BIN is loaded if you enable multiboot.
 @warning THIS API control HW directly without middleweare controlling. Please MUST NOT using this API after FW.BIN is loaded.
 @param[in] pWavBuf Pointer to WAV buffer. This buffer MUST contains RIFF WAV tag with format chunk, and start address MUST 4 byte alignment.
 @param[in] bufSz Total size of WAV buffer. If bufSize < RIFF header, it return FAIL. If (bufSize-RIFF header)<(sampleRate*bitsPerSample*channels*mSec), it also return FAIL.
 @param[in] mSec Playback duration for WAV buffer. 0 mean play WHOLE buffer. Others for specified time duration in milli-second.
 @see http://en.wikipedia.org/wiki/WAV#RIFF_WAVE if you have question on RIFF WAV tag
 @see http://www.neurophys.wisc.edu/auditory/riff-format.txt if you have question on RIFF WAV tag, and format chunk
*/
UINT32
sp5kBootWavPlay(UINT8 *pWavBuf, UINT32 bufSz, UINT32 mSec);

UINT32 sp5kHdrYuvDo( frameBufInfo_t *src, frameBufInfo_t *dst, UINT32 num );
UINT32 sp5kHdrYuvCfg( UINT32 select, UINT32 value );
#endif  /* _SP5K_UTILITY_API_H_ */

