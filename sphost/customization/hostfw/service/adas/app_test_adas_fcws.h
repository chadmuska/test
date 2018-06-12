#ifndef APP_TEST_ADAS_FCWS_H
#define APP_TEST_ADAS_FCWS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_General.h"
#include "middleware/common_types.h"
#include "sp5k_gfx_api.h"
#include "sp5k_ftrack_api.h"

enum {NIGHT= 0, DAY, TIME_TOTAL};

//API
void appFcwsStart(const unsigned int nInCenterX, const unsigned int nInCenterY, ...);
void appFcwsStop();
sp5kPatternTrackCarRes_t* appFcwsGetResult();
void appFcwsDumpSetting();

//Product OSD
void appFcwsDrawProductOsdResult(UINT8 eDrawedLayer);

//Debug
void debugFcwsDrawResult(UINT8 eDrawedLayer);
void debugFcwsDrawResultToRecord(frameBufInfo_t *pframe);

//DEMO
void demoFcwsDrawResultToRecord(frameBufInfo_t *pframe);

#ifdef __cplusplus
}
#endif

#endif

