#ifndef _APP_SYS_CFG_H_
#define _APP_SYS_CFG_H_

void appGyroOutputRotate(
	SINT32 xIn,
	SINT32 yIn,
	SINT32 zIn,
	SINT32 *xOut,
	SINT32 *yOut,
	SINT32 *zOut
);


void
cmdAppIs(
	UINT32 argc,
	UINT8 *arg[],
	UINT32 v[]
);


UINT32
appGyroCalParamLoad(
	void *pdata
);

UINT32 appIsLoadEISParam(
	UINT32 sensor_mode,
	int *fl
);


UINT32 appIsQuickEnGet(void);

void appGyroCalibration();
#if SP5K_ICAT_EIS

void appIsDbgModeSet(
	UINT32 cardExist
);

void appIsCfgSet(
	UINT32 fps,
	UINT32 advEis,
	UINT32 modeswMode
);

void appIsModeSet(
    UINT8 en,
    UINT32 eisMode,
    UINT32 videoSize,
    void* pvRaw2YuvCb
);

void isLdcIsDisable(void);

#endif

#endif
