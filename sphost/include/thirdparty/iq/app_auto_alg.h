
#ifndef _APP_AUTO_ALG_H_
#define _APP_AUTO_ALG_H_

//---------------------------------------------------------------------------

#define AG_ALG_VER			"0.11.00"

#define AG_RES_PATH "A:\\RO_RES\\IQ\\AWDRALG.BIN"

void appAutoWDRLibInit(void);
void appAutoWDRLibOneShot(UINT32 * pHis);
void appAutoWDRLibEable(BOOL bEnable);
void appAutoHDRLibEnable(BOOL bEnable);
#endif

