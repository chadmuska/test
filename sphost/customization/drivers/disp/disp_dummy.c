/**************************************************************************
 *
 *       Copyright (c) 2012-2018 by iCatch Technology, Inc.
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
*******************************************************************************/

/*******************************************************************************
*                            H E A D E R   F I L E S
*******************************************************************************/
#include "common.h"
#include "customization/dev_init.h"
#include "debug_mod/dbg_api.h"
#include "api/sp5k_disp_api.h"
/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/*
  compile option in Makefile:
    DISP_ONE=1 : dummy.
*/

/* render dimension. */
#define DEV_PID  0

#define WIDTH   320
#define HEIGHT  240
#define PNLH0   0
#define PNLV0   0
#define PNLH1   (PNLH0 + WIDTH - 1)
#define PNLV1   (PNLV0 + HEIGHT - 1)

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* device open. */
#define dispOpen             dispDummyOpen
#define dispOpDevNameGet     dispDummyOpDevNameGet
#define dispOpPowerOn        dispDummyOpPowerOn
#define dispOpPowerOff       dispDummyOpPowerOff
#define dispOpCapabilityGet  dispDummyOpCapabilityGet

/* operating function. */
static UINT8 *dispOpDevNameGet(void);
static UINT32 dispOpPowerOn(void);
static UINT32 dispOpPowerOff(void);
static void   dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc, UINT32 descSize);

/*******************************************************************************
*                            G L O B A L   D A T A
*******************************************************************************/
/* device name. */
static UINT8 dispDevName[] = "PANEL - DUMMY";

/* device capability. */
static dispCapabDesc_t dispCapabDesc = {
	.width        = WIDTH,
	.height       = HEIGHT,
	.xoff         = PNLH0,
	.yoff         = PNLV0,
	.aspectRatioX = 4,
	.aspectRatioY = 3,
	.logiEn       = 0,
	.frameRate    = 60000,
};

/**
 * @brief Open (register) a device driver.
 *
 * @param None.
 *
 * @return Device handle.
 */
UINT32
dispOpen(
	void
)
{
    UINT32 ret;
	dispOperatingFunc_t dispOpFunc;

    DBG_FUNC_ENTER(NULL);

	dispOpFunc.pfOpDevNameGet    = dispOpDevNameGet;
	dispOpFunc.pfOpPowerOn       = dispOpPowerOn;
	dispOpFunc.pfOpPowerOff      = dispOpPowerOff;
	dispOpFunc.pfOpCapabilityGet = dispOpCapabilityGet;
	dispOpFunc.pfOpResolutionSet = NULL;

	ret = sp5kDispDevReg(SP5K_DISP_CHNL_0, SP5K_DISP_CHNL_0_PANEL, &dispOpFunc);

	DBG_FUNC_RETURN(ret, NULL);
}

/**
 * @brief Get device name string in current resolution of the device.
 *
 * @param None.
 *
 * @return Device name string.
 */
static UINT8 *
dispOpDevNameGet(
	void
)
{
	return dispDevName;
}

/**
 * @brief Power on the device.
 *
 * @param None.
 *
 * @return SUCCESS or FAIL.
 */
static UINT32
dispOpPowerOn(
	void
)
{
	/* set vh timing and format. */
	dispParaSet(DEV_PID, DISP_PARA_GLOB_ENABLE, 0, 0, 0, 0, 2);

	return SUCCESS;
}


/**
 * @brief Power off the device.
 *
 * @param None.
 *
 * @return SUCCESS or FAIL.
 */
static UINT32
dispOpPowerOff(
	void
)
{
	DBG_FUNC_ENTER(NULL);

	DBG_FUNC_RETURN(SUCCESS, NULL);
}


/**
 * @brief Get capability in current resolution of the device.
 *
 * @param [out] *pcapabDesc Capability descriptor.
 * @param [in]  descSize    Data size of the descriptor.
 *
 * @return None.
 */
static void
dispOpCapabilityGet(
	dispCapabDesc_t* pcapabDesc,
	UINT32 descSize
)
{
	*pcapabDesc = dispCapabDesc;
}

