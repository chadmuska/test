/**************************************************************************
 *
 *       Copyright (c) 2005-2014 by iCatch Technology, Inc.
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
 * @file global_def.h
 *
 * @author Eric Su, Matt Wang
 */
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 15702 $
$Date:: 2014-07-28 16:01:53#$
 */

#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_

/**************************************************************************
 *                         H E A D E R   F I L E S
 **************************************************************************/
#include <time.h>

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
/* gpioCfgSet() */
typedef enum _gpioGrp_e {
	GPIO_GRP_TGL = 0,
	GPIO_GRP_LMI,
	GPIO_GRP_FML,
	GPIO_GRP_FMH,
	GPIO_GRP_GEN,
	GPIO_GRP_DISPL,
	GPIO_GRP_DISPH,
	GPIO_GRP_POCU,
	GPIO_GRP_USB,
#if SPCA6350
	GPIO_GRP_SAR,
	GPIO_GRP_AUD_LINEOUT,
#elif SPCA5330
	GPIO_GRP_SAR,
	GPIO_GRP_OIS,
#endif
	GPIO_GRP_CEC,
#if SPCA6350
	GPIO_GRP_RGB,
#endif
	GPIO_GRP_TOT,
	GPIO_GRP_NO_USE = 0xffff,
} gpioGrp_e;

/* for backward compatible. */
#define GPIO_GRP_DISP  GPIO_GRP_DISPL

typedef enum {
	POWER_SAVE_WAKEUP_SRC_TIMER	= 0xFF,
	POWER_SAVE_WAKEUP_SRC_GRP_GPIO	= 0x00,
} powerSaveWakeupSrc;
/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/
/* timeget() callback */
typedef void (*fpTimeCallback_t)(struct tm *);

/* standbyEx.c */
typedef struct powerSaveIndicator_s {
	UINT32 grpId; /**< gpio group */
	UINT32 pinId; /**< gpio pin within specified group */
	UINT32 highPeriod; /**< gpio output high period. Unit: sw count. */
	UINT32 lowPeriod; /**< gpio output high period. Unit: sw count. */
} powerSaveIndicator_t;
enum groupWakeupCondition{
	WAKE_UP_LOW_2_HIGH	= 0x10,
	WAKE_UP_HIGH_2_LOW	= 0x11,
};
#define POWER_SAVE_WAKE_UP_GROUP_MASK	(0x10)
typedef union powerSaveWakeupPin_s {
	struct standby_s {
		UINT32 grpId; /**< gpio group */
		UINT32 pinId; /**< gpio pin within specified group */
		UINT8  polar; /**< 0: low active, 1: high active */
		UINT8  rsvd[3];
		/* for conditional wake-up pins, s */
		UINT32  pre;
		UINT32  cur;
		UINT32 cond;
		/* for conditional wake-up pins, e */
	} standby;
	struct suspend_s {
		UINT32 fall;
		UINT32 rise;
	} suspend;
} powerSaveWakeupPin_t;

typedef struct powerSaveWakeupPinGroup_s{
	UINT32 pinNum;/* total pins */
	powerSaveWakeupPin_t *pins;
}powerSaveWakeupPinGroup_t;

#define TEST_NEW_PWR_SAVE_CFG	(1)
#define POWER_SAVE_STANDBY_RULE_MAX	(4)
#define POWER_SAVE_STANDBY_PIN_MAX	(4)
#define POWER_SAVE_STANDBY_INDI_MAX	(8)
typedef struct powerSaveWakeupCfg_s{
	UINT32 mode;/* sp5kModePowerSaveMode_e */
	UINT32 pinNum;
	powerSaveWakeupPin_t *pins;/* for waked-up by single key */
	
	powerSaveWakeupPinGroup_t group;
	
	UINT32 ruleNum;
	UINT32 *rules;
}powerSaveWakeupCfg_t;
/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
#define GPIO_PACK(grp,pin,dir,active)	((((UINT32)dir&0x1)<<31)|(((UINT32)active&0x1)<<30)|(((UINT32)grp&0xf)<<8)|((UINT32)pin&0xff))
#define GPIO_UNPACK_PIN(pack)			(pack&0xff)
#define GPIO_UNPACK_GRP(pack)			((pack>>8)&0xf)
#define GPIO_UNPACK_ACTIVE(pack)		((pack>>30)&0x1)
#define GPIO_UNPACK_DIR(pack)			((pack>>31)&0x1)

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/
/* gpio.c */
UINT32 gpioCfgSet(UINT32 grpId, UINT32 pinMsk, UINT32 cfg);
UINT32 gpioCfgGet(UINT32 grpId, UINT32 pinMsk);
UINT32 gpioDirSet(UINT32 grpId, UINT32 pinMsk, UINT32 dir);
UINT32 gpioDirGet(UINT32 grpId, UINT32 pinMsk);
UINT32 gpioFuncSet(UINT32 grpId, UINT32 pinMsk, UINT32 val);
UINT32 gpioRead(UINT32 grpId, UINT32 pinMsk, UINT32 *pval);
UINT32 gpioWrite(UINT32 grpId, UINT32 pinMsk, UINT32 val);
UINT32 gpioPullSet(UINT32 grpId, UINT32 pinMsk, UINT32 enable);
UINT32 gpioPullGet(UINT32 grpId, UINT32 pinMsk);
UINT32 gpioDrvSet(UINT32 grpId, UINT32 pinMsk, UINT32 hdw, UINT32 ldw);
UINT32 gpioDrvGet(UINT32 grpId, UINT32 pinMsk, UINT32 *hdw, UINT32 *ldw);
void   gpioPullInit(void);

/* standbyEx.c */
UINT32 standbyExTimerEnSet(UINT32 en, UINT32 timeout);
UINT32 standbyExIndicatorSet(powerSaveIndicator_t *pBlink, UINT32 num);
UINT32 standbyExWakeupSet(powerSaveWakeupCfg_t *pWakeupCfg);
UINT32 standbyExModeEnter(void);

#endif  /* _GLOBAL_DEF_H_ */

