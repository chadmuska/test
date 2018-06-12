#ifndef APP_TEST_ADAS_LPD_H
#define APP_TEST_ADAS_LPD_H

#ifdef __cplusplus
extern "C" {
#endif

//License Plate Detection
#include "app_general.h"
#include "middleware/common_types.h"
#include "sp5k_gfx_api.h"

//API
void appLpdStart();
void appLpdStop();
BOOL appIsLpdFound();

//Product OSD
void appLpdDrawProductOSDResult(UINT8 eDrawedLayer);

//Ddebug
void debugLpdDrawResult(UINT8 eDrawedLayer);

void lpdDrawResultToRecord(frameBufInfo_t *pframe);


#ifdef __cplusplus
}
#endif

#endif

