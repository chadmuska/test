#ifndef APP_TEST_ADAS_LDWS_H
#define APP_TEST_ADAS_LDWS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_test_adas.h"
#include "app_general.h"
#include "middleware/common_types.h"
#include "sp5k_gfx_api.h"


extern SP5K_EVENT_FLAGS_GROUP g_EventAdasStartPlayAudio;
extern eADAS_Sound g_eADAS_SOUND;
//API
void appLdwsStart(const int nCenterX, const int nCenterY, ...);
void appLdwsStop();
void appLdwsDumpSetting();

//Product OSD
void appLdwsDrawProductOSDResult(sp5kGfxPageNum_e eDrawedLayer);

//Debug
void debugLdwsDrawResult(sp5kGfxPageNum_e eDrawedLayer);
void debugLdwsDrawResultToRecord(frameBufInfo_t *pframe);

//Demo
void demoLdwsDrawResultToRecord(frameBufInfo_t *pframe);

#ifdef __cplusplus
}
#endif

#endif

