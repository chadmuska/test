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
#include "common.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_global_api.h"
#include "app_gSensor_common.h"
#include "../../hostfw/include/app_user_setting.h"
#include "customization/dev_init.h"
#include "api/sp5k_media_api.h"
#include "api/sp5k_os_api.h"
#include "../../hostfw/include/app_key_def.h"
#include "../../hostfw/include/app_infohandlerComm.h"
#include "api/sp5k_dbg_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

#ifndef INT16
#define INT16   short
#endif

#ifndef INT8
#define INT8    char
#endif

#define NBIT_REGADDR    8
#define NBIT_REGDATA    8
#define READBIT         0x80    /* SPI INTERFACE sync NBIT_REGADDR */
#define NBIT_UNIT       8

#define SLAVEADDR   0x18
#define FERQUENCY_I2C 21 /*SP5K_I2C_CLK_DIVISOR_1024  Isaac 20161004*/
#define ENADDR2BYTE 0
#define READMODE    0x01

#define MODE_MASK   0x0f
#define LEVEL_MASK  0xf0

#define CS_GRP_ID   SP5K_GPIO_GRP_LMI   /* SPI CHIP SELECT GPIO GROUP ID */
#define CS_PIN_MASK 10
#define FERQUENCY_SPI SP5K_SPI_PORT2_CLK_1500KHZ /*SP5K_SPI_PORT2_CLK_1_5MHZ Isaac 20161004 */
#define MODE_SPI    0x03

#define GSENSOR_ACTIVE_LOW      0xFF
#define GSENSOR_ACTIVE_MEDIUM       0xBB
#define GSENSOR_ACTIVE_HIGH     0x99

#define APP_GSENSOR_EVENT_THREAD_PRIORITY 31

#define DEBUG_ON 0

extern PowerDetect appInfoPowerDETStateGet(void);

#if 0
/* Register Initialization Table    */
UINT8 regInitTbl_2g[] = { /* {ADDR, DATA} */
    0x20,0x47,
    0x21,0x00,
    0x22,0x1f,
    0x30,0x6a,
    0x32,0x04,
    0x33,0x03,
    0x34,0x6a,
    0x36,0x04,
    0x37,0x03,
    0x38,0x55,
    0x39,0x55,
    0x3b,0xff,
    0x3c,0x0f
};

UINT8 regInitTbl_4g[] = { /* {ADDR, DATA} */
    0x20,0x47,
    0x21,0x00,
    0x22,0x1f,
    0x30,0x6a,
    0x32,0x04,
    0x33,0x03,
    0x34,0x6a,
    0x36,0x04,
    0x37,0x03,
    0x38,0x55,
    0x39,0x55,
    0x3b,0xff,
    0x3c,0x0f
};

UINT8 regInitTbl_8g[] = { /* {ADDR, DATA} */
    0x20,0x47,
    0x21,0x00,
    0x22,0x1f,
    0x30,0x6a,
    0x32,0x04,
    0x33,0x03,
    0x34,0x6a,
    0x36,0x04,
    0x37,0x03,
    0x38,0x55,
    0x39,0x55,
    0x3b,0xff,
    0x3c,0x0f
};

UINT8 regInitTbl_16g[] = { /* {ADDR, DATA} */
    0x20,0x47,
    0x21,0x00,
    0x22,0x1f,
    0x30,0x6a,
    0x32,0x04,
    0x33,0x03,
    0x34,0x6a,
    0x36,0x04,
    0x37,0x03,
    0x38,0x55,
    0x39,0x55,
    0x3b,0xff,
    0x3c,0x0f
};
#else
static UINT8 reg_init_tbl[] =
    {
    0x14, 0xb6, /* soft reset */
    0x20, 0x04, /* INT1, and INT2 pins push-pull, INT2 active high, INT1 low*/
    0x21, 0x0e, /* Latch 50ms */
    0x17, 0x00, /* Disable low-g, x high g, y high g, and z high g */
    0x16, 0x7,	/* Enable source of slope IRQ all from X, Y, and Z */
    0x27, 0x3,	/* Slope duration 4 times */
    0x10, 0x0e, /* Data filter for 500 Hz (1ms) */
    0x13, 0x00, /* enable filter */
#if 0
    0x28, 0x1,	/* Slope threshold */
    0x38, 0,	/* reset X axis compensation */
    0x39, 0,	/* reset Y axis compensation */
    0x3a, 0x80,	/* reset Z axis compensation */
#endif
    };

#define CONF_LEVE 5
struct conf_item {
    UINT8 reg;
    UINT8 conf[CONF_LEVE];
};

const static struct conf_item normal_mode[] =
    {
        { 0x1b, { 4, 4, 4, 4, 4 } },
        { 0x19, { 0, 0, 0, 0, 0 } },
        { 0xFF, { 7, 6, 5, 4, 3 } },
    };

const static struct conf_item parking_mode[] =
    {
	{ 0x10, { 0x0f, 0x0f, 0x0f, 0x0f, 0x0f } },/* Data filter for 1000 Hz (1ms) */
        { 0x17, { 0x07, 0x07, 0x07, 0x07, 0x07 } },
        { 0x19, { 2, 2, 2, 2, 2 } },
        { 0x1b, { 0, 0, 0, 0, 0 } },
        { 0x26, { 38, 32, 26, 19, 13 } },
        { 0x25, { 12, 9, 6, 3, 1} },
        { 0x24, { 1, 1, 1, 1, 1 } },
        { 0x21, { 0x85, 0x85, 0x85, 0x85, 0x85} }, /* 4 sec pluse of PIN 2 */
    };

const static struct conf_item disable_mode[] =
    {
        { 0x19, { 0, 0, 0, 0, 0 } },
        { 0x1b, { 0, 0, 0, 0, 0 } }
    };

const static struct conf_item power_mode[] =
        {
        {0x20, { 5, 5, 5, 5, 5 } },
        {0x19, { 0, 0, 0, 0, 0 } },
        {0x1b, { 0, 0, 0, 0, 0 } },
        {0x12, {0, 0, 0, 0, 0} },
        {0x11, {0, 0x40, (0xc) << 1 | 0x40,
                        0x80, 0x20},
        }
    };

#define SIZE(x) (sizeof(x) / sizeof(x[0]))

SP5K_EVENT_FLAGS_GROUP appGsensor_sig;

static UINT32 accThreshold = 0;
static BOOL _disable = 0;
static UINT32 lastSend = 0;

int acceleratorLPF = 1;
#define ACC_LPF_BASE (10l)

static SP5K_MUTEX __g_sensor;

#define APP_PARKING_MODE 0x11

static struct {
	INT16 x, y, z;
	UINT32 acc;
} G_log[20];

static int G_log_index;

static UINT32 G_lastTime = 0;

#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void appGsensorLpfSet(int lpf)
{
	if (lpf < 0 || lpf > ACC_LPF_BASE) {
		printf("LPF(%d) value is invalid\n", lpf);
		return;
	}

	acceleratorLPF = lpf;
}



BOOL appGsensorCal(INT16 *acc, UINT8 *off);

void appGsensorZero()
{
    INT16 acc[3] =
        { 0, 0, 0 };

    appGsensorCal(acc, NULL);
}

void appGsensorClearIRQ(void)
{
    UINT8 data;

    sp5kOsMutexGet(&__g_sensor, TX_WAIT_FOREVER);
    sp5kI2cRead(SP5K_SSC_PORT_6, 0x21, &data, 1);
    data |= 0x80;
    sp5kI2cWrite(SP5K_SSC_PORT_6, 0x21, &data, 1);
    sp5kOsMutexPut(&__g_sensor);
}

void appGsensorModeSave(int mode, int level)
{
    UINT8 _mode = (mode & MODE_MASK) | ((level << 4) & LEVEL_MASK);

    printf("%s(%d) mode 0x%02x\n", __func__, __LINE__, _mode);
    sp5kRtcExtraSet(&_mode, APP_PARKING_MODE, sizeof(_mode));
}

void appGsensorLevelCtrl(int mode, int level)
{
    int i;
    const struct conf_item *t;
    const static struct {
        const struct conf_item *conf;
        int size;
        BOOL be_zero;
	BOOL disable;
    } mode_list[] =
        {
            { disable_mode, SIZE(disable_mode), 0, 1 },
            { normal_mode, SIZE(normal_mode), 0, 0 },
            { parking_mode, SIZE(parking_mode), 1, 0 },
            { power_mode, SIZE(power_mode), 0, 0 }, };

    if (mode >= SIZE(mode_list) || level >= CONF_LEVE)
        return;

    /* check disable mode */

    printf("mode %d, %d, %d\nlevel %d\n", (int)SIZE(mode_list), mode,
            mode_list[mode].size, level);

    if (mode_list[mode].be_zero)
        appGsensorZero();
    _disable = mode_list[mode].disable;

    t = mode_list[mode].conf;
    for (i = 0; i < mode_list[mode].size; i++) {
        UINT8 reg, data;

        reg = t[i].reg;
        data = t[i].conf[level];

        printf("conf %d -> 0x%02hhx, 0x%02hhx\n", i, reg, data);
        if (reg == 0xFF) {
            accThreshold = (UINT32) data * (UINT32) data * 10000;
            printf("acc (g) threshold %u\n", accThreshold);
        } else {
            sp5kOsMutexGet(&__g_sensor, TX_WAIT_FOREVER);
            sp5kI2cWrite(SP5K_SSC_PORT_6, reg, &data, 1);
            sp5kOsMutexPut(&__g_sensor);
        }
    }
    lastSend = sp5kOsTimeGet();
}

BOOL appGsensorModeRestore()
{
	int mode, level;

	UINT8 _mode;

	sp5kRtcExtraGet(&_mode, APP_PARKING_MODE, sizeof(_mode));

	printf("%s: g sensor mode 0x%02x\n", __func__, _mode);

	if (_mode == 0 || _mode == 0xff)
		return FAIL;
	printf("%s(%d)\n", __func__, __LINE__);
	mode = _mode & MODE_MASK;
	level = (_mode & LEVEL_MASK) >> 4;

	printf("%s(%d) %d %d\n", __func__, __LINE__, mode, level);
	appGsensorLevelCtrl(mode, level);

	_mode = 0xff;	/* start parkink mode */
	sp5kRtcExtraSet(&_mode, APP_PARKING_MODE, sizeof(_mode));
	return SUCCESS;
}

void appGsensorFilter(BOOL on)
{
    UINT8 data;

    sp5kOsMutexGet(&__g_sensor, TX_WAIT_FOREVER);
    sp5kI2cRead(SP5K_SSC_PORT_6, 0x13, &data, 1);
    if (on)
        data |= 0x80;
    else
        data &= ~0x80;
    sp5kI2cWrite(SP5K_SSC_PORT_6, 0x13, &data, 1);
    sp5kOsMutexPut(&__g_sensor);
}

static void appGsensorIsrCallBack(void)
{
	/*UINT32 err;*/
#if DEBUG_ON
    printf("Gsensor ISR2 Rise\n");
    printf("Event Trigger Interrupt Mode\n");
#endif
#if defined(PRJ_DVR)
    sp5kHostMsgSend(APP_KEY_GSENSOR_TRIGGER,0);
#elif defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 err;
    err = sp5kOsEventFlagsSet(&appGsensor_sig, 1, TX_INT_ENABLE);
	HOST_ASSERT(err == SP5K_SUCCESS);
#endif
}

static void appGsensorIsrInit(void)
{
#if 0
    sp5kGpioCfgSet(SP5K_GPIO_GRP_FML, 1 << PIN_24, 0); /*Input*/
    sp5kGpioIsrReg(SP5K_GPIO_INT_FM24, SP5K_TRIG_LVL_RISE,
            appGsensorIsrCallBack);
    sp5kGpioIntEnable(SP5K_GPIO_INT_FM24, SP5K_TRIG_LVL_RISE);
#else
    sp5kTimerUsIsrReg(SP5K_TIMER_US_ISR_ID_1, -1, 50000, appGsensorIsrCallBack);
#endif
}

static void appGsensorCMDInterfaceInit(void)
{
    UINT32 param1;
    param1 = ((SLAVEADDR << 1) << 16) | (FERQUENCY_I2C << 8)
            | (ENADDR2BYTE << 5) | (READMODE << 1);
    sp5kSscCfgSet(SP5K_SSC_PORT_6, SP5K_SSC_CFG_MODE, SP5K_SSC_MODE_I2C);
    sp5kSscCfgSet(SP5K_SSC_PORT_6, SP5K_SSC_CFG_PIN, SP5K_SSC_CFG_PIN0);
    sp5kSscCfgSet(SP5K_SSC_PORT_6, SP5K_I2C_CFG_PHY, param1);
}

static INT16 convert2S16(UINT8 lsb, UINT8 msb)
{
    return (INT16) (((((INT8) msb) << 8) | lsb) >> 6);
}

static void appGsensorAccXYZPush(UINT32 acc, INT16 x, INT16 y, INT16 z)
{
	G_log[G_log_index].x = x;
	G_log[G_log_index].y = y;
	G_log[G_log_index].z = z;
	G_log[G_log_index].acc = acc;

	G_log_index++;
	G_log_index %= SIZE(G_log);
}

static void appGsensorAccXYZTimePush(UINT32 acc, INT16 x, INT16 y, INT16 z)
{
	/*int i;*/
	UINT32 now = sp5kOsTimeGet();

	if (G_lastTime != 0) {
		int i;
		i = (now - G_lastTime) / 50 - 1;	/* how long no update */
		if (i >= SIZE(G_log)) {
			memset(G_log, 0, sizeof(G_log));
			G_log_index = 0;
		} else {
			while(i > 0) {
				i--;
				appGsensorAccXYZPush(0, 0, 0, 0);
			}
		}
	}
	appGsensorAccXYZPush(acc, x, y, z);
	G_lastTime = now;
}

void appGsensorAccXYZMax(INT16 *x, INT16 *y, INT16 *z)
{
	int i;
	int max_index;
	UINT32 max_acc;

	for (i = 0, max_index = 0, max_acc = 0; i < SIZE(G_log); i++) {
		if (max_acc < G_log[i].acc) {
			max_index = i;
			max_acc = G_log[i].acc;
		}
	}

	*x = G_log[max_index].x;
	*y = G_log[max_index].y;
	*z = G_log[max_index].z;
#if DEBUG_ON
        printf("max %u = %d, %d, %d\n", max_acc, *x, *y, *z);
#endif
}

void appGsensorAccXYZ(INT16 *x, INT16 *y, INT16 *z)
{
	int i = G_log_index - 1;
	if (i < 0)
		i = SIZE(G_log) - 1;

	*x = G_log[i].x;
	*y = G_log[i].y;
	*z = G_log[i].z;
}

extern void hex_dump(UINT8 *buf, UINT32 len);

void _appGsensorAccXYZ(INT16 *x, INT16 *y, INT16 *z)
{
    UINT8 buf[6];

    sp5kOsMutexGet(&__g_sensor, TX_WAIT_FOREVER);
    sp5kI2cRead(SP5K_SSC_PORT_6, 2, buf, sizeof(buf));
    sp5kOsMutexPut(&__g_sensor);

#if 0
    printf("read XYZ (%d) : ", sizeof(buf));
    hex_dump(buf, sizeof(buf));
#endif
    *x = convert2S16(buf[0], buf[1]);
    *y = convert2S16(buf[2], buf[3]);
    *z = convert2S16(buf[4], buf[5]);
}

#define LPF(x, y)	(((x) * acceleratorLPF / ACC_LPF_BASE) + \
		((y) * (ACC_LPF_BASE - acceleratorLPF) / ACC_LPF_BASE))

void appGsensorEventThread(ULONG param)
{
    ULONG flags;
    /*UINT32 err;*/

    static INT16 px, py, pz;

    lastSend = sp5kOsTimeGet();
    while (1) {
        UINT32 acc;
		UINT32 err;
        INT16 x, y, z;
        int x1, y1, z1;
#if 0
        UINT8 data = 0x8f; /* clear and latch */
#endif
        err = sp5kOsEventFlagsGet(&appGsensor_sig, 1, TX_OR_CLEAR, &flags,
                TX_WAIT_FOREVER);
        HOST_ASSERT(err == SP5K_SUCCESS);

	_appGsensorAccXYZ(&x, &y, &z);

	x = LPF(x, px);	/* assign result of LPF */
	y = LPF(y, py);
	z = LPF(z, pz);

        x1 = (x * 391) / 100;	/* convert to G(unit: mg) space from raw data space */
        y1 = (y * 391) / 100;
        z1 = (z * 391) / 100;

	z1 -= 1000;	/* ?“ã??§é??›å??Ÿåº¦?†ã??Šé™¤?™ã???*/
        acc = x1 * x1 + y1 * y1 + z1 * z1;
#if DEBUG_ON
        printf("X %d  Y %d  Z %d\nX %fG y %fG z %fG\n", x, y, z,
                (float) x * 0.00391, (float) y * 0.00391,
                (float) z * 0.00391);
	printf("acc %d, X = %d %d, Y = %d %d, Z = %d %d\n", acc,
			x, x1,
			y, y1,
			z, z1 + 1000);
#endif
	appGsensorAccXYZTimePush(acc, x, y, z);
	px = x; py = y; pz = z;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 now;
	now = sp5kOsTimeGet();
        if (acc >= accThreshold && (lastSend + 250) < now) {
#if DEBUG_ON
                printf("%u - %u = X %d\tY %d\tZ %d\n", acc, accThreshold,
				x1, y1, z1);
#endif
		if (_disable == 0)
	                sp5kHostMsgSend(INFO_QUEUEMSG_EVENTTIGGER, 0);
		lastSend = now;
        }
#endif
#if 0
        sp5kOsMutexGet(&__g_sensor, TX_WAIT_FOREVER);
        sp5kI2cWrite(SP5K_SSC_PORT_6, 0x21, &data, 1);
	sp5kOsMutexPut(&__g_sensor);
#endif
    }
	
}

static UINT8 IntFlag = 0;

/**
 * @brief get g sensor parking status
 * @param none
 * @return true - parking event, false - none
 */
BOOL appGsesnorParkingEvent()
{
	UINT8 mode;

	sp5kRtcExtraGet(&mode, APP_PARKING_MODE, sizeof(mode));
	printf("%s:G sensor mode 0x%02x\n", __func__, mode);

	return IntFlag & (1 << 1) && mode == 0xff;
}

void appGsensorInit(void)
{
    int i;
    UINT32 err;
    UINT8 t, s;
    SP5K_THREAD *ptrdInfoHandler;

    printf("G sensor initial\n");
    sp5kOsMutexCreate(&__g_sensor, "G sensor BMA250E", TX_INHERIT);

    appGsensorCMDInterfaceInit();

    sp5kOsMutexGet(&__g_sensor, TX_WAIT_FOREVER);
    /* disbale deep suspend mode */
    t = 0;
    sp5kI2cWrite(SP5K_SSC_PORT_6, 0x11, &t, 1);
    /* Read Int_STATUS_0 */
    sp5kI2cRead(SP5K_SSC_PORT_6, 0x09, &t, 1);
    for (i = 0; i < 10; i++) {
        sp5kI2cRead(SP5K_SSC_PORT_6, 0x09, &s, 1);
	printf("INT_STATUS_O(%d) 0x%hhx - 0x%hhx\n", i, t, s);
	if (t == s && s != 0xFF)
		break;
	t = s;
    }
    IntFlag = t;

    for (i = 0; i < sizeof(reg_init_tbl) / 2; i++) {
        UINT32 reg;
        UINT8 data;

        reg = reg_init_tbl[2 * i];
        data = reg_init_tbl[2 * i + 1];
        sp5kI2cWrite(SP5K_SSC_PORT_6, reg, &data, 1);
        /* reset need more time to finish */
        if (i == 0)
            sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 2);
    }
    sp5kOsMutexPut(&__g_sensor);

    err = sp5kOsEventFlagsCreate(&appGsensor_sig, "GsensorEvent");
    HOST_ASSERT(err == SP5K_SUCCESS);

    ptrdInfoHandler = sp5kOsThreadCreate("GsensorEventThread",
            appGsensorEventThread, 0,
            APP_GSENSOR_EVENT_THREAD_PRIORITY,
            APP_GSENSOR_EVENT_THREAD_PRIORITY,
            0,
            TX_AUTO_START);
    HOST_ASSERT(ptrdInfoHandler);
    appGsensorIsrInit();
}

void appGsensorUpdateCompensation(UINT8 *offset)
{
    int i;

    printf("Update compensation -");
    for (i = 0; i < 3; i++, offset++) {
        sp5kOsMutexGet(&__g_sensor, TX_WAIT_FOREVER);
	sp5kI2cWrite(SP5K_SSC_PORT_6, (0x38 + i), offset, 1);
        sp5kOsMutexPut(&__g_sensor);
        printf(" 0X%02hhX", *offset);
    }
    printf("\n");
}

BOOL appGsensorCal(INT16 *acc, UINT8 *off)
{
    int i;
    UINT8 data;
    BOOL ret = SUCCESS;
    int timeout;

    for (i = 0; i < 3; i++) {
        int target = 0;

        if (abs(acc[i]) > 200) {
            target = acc[i] > 0 ? 1 : -1;
        }
        if (target > 0)
            target = 1;
        else if (target < 0)
            target = 2;

        sp5kOsMutexGet(&__g_sensor, TX_WAIT_FOREVER);
        /* set compensation target for this axis */
        data = target << (i * 2 + 1);
        sp5kI2cWrite(SP5K_SSC_PORT_6, 0x37, &data, 1);

        data = ((i + 1) << 5 );
        sp5kI2cWrite(SP5K_SSC_PORT_6, 0x36, &data, 1);

	timeout = 10;
        do {
            sp5kI2cRead(SP5K_SSC_PORT_6, 0x36, &data, 1);
		sp5kOsThreadSleep(50);
        } while ((data & 0x10) == 0 && timeout--);

        if (timeout == 0) {
	    printf("%s(%d) timeout\n", __func__, __LINE__);
            ret = FAIL;
	    goto out;
	}

        if (off) {
            sp5kI2cRead(SP5K_SSC_PORT_6, (0x38 + i),
                    &data, 1);
	    off[i] = data;
	}
        sp5kOsMutexPut(&__g_sensor);
    }
out:
    return ret;
}

int appGsensorCalSave()
{
    UINT8 data;

    sp5kOsMutexGet(&__g_sensor, TX_WAIT_FOREVER);
    data = 0x1; /* unlock NVM non-volatile memory */
    sp5kI2cWrite(SP5K_SSC_PORT_6, 0x36, &data, 1);
    data |= 1 << 1; /* trigger NVM write */
    sp5kI2cWrite(SP5K_SSC_PORT_6, 0x36, &data, 1);

    do {
        sp5kI2cRead(SP5K_SSC_PORT_6, 0x36, &data, 1);
    } while ((data & 0x04) == 0);
    sp5kOsMutexPut(&__g_sensor);

    return data >> 4; /* remain write cycles */
}

void appGsensorClean(void)
{
    UINT8 byte;

    sp5kOsMutexGet(&__g_sensor, TX_WAIT_FOREVER);
    sp5kI2cRead(SP5K_SSC_PORT_6, 0x39, &byte, 0x01);
    sp5kOsMutexPut(&__g_sensor);
    printf("Gsensor I2c read: %x\n", byte);
}
