#ifndef APP_TEST_ADAS_ROD_H
#define APP_TEST_ADAS_ROD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_test_adas.h"
#include "app_general.h"
#include "middleware/common_types.h"
#include "sp5k_gfx_api.h"

typedef enum{
    ROD_CFG_INPUT_MAIN = 0,
    ROD_CFG_INPUT_SEC,
    ROD_CFG_INPUT_PBVDO,
    ROD_CFG_SRC_W,
    ROD_CFG_SRC_H,
    ROD_CFG_OSD_W,
    ROD_CFG_OSD_H,
    ROD_CFG_OSD_OFFSET_TOP_H,
    ROD_CFG_UP_H,
    ROD_CFG_MID_H,
    ROD_CFG_LOW_H,
    ROD_CFG_MAX
} eRODCfg;

extern SP5K_EVENT_FLAGS_GROUP g_EventAdasStartPlayAudio;
extern eADAS_Sound g_eADAS_SOUND;
extern eADAS_Sound g_ePrevADAS_SOUND;
//API
void appRodStart();
void appRodStop();
void appRodCfgSet(eRODCfg eCfgOption, ...);

//Product OSD
void appRodDrawProductOSDResult(sp5kGfxPageNum_e eDrawedLayer);

//Debug
void debugRodDrawResult(sp5kGfxPageNum_e eDrawedLayer);
void debugRodDrawResultToRecord(frameBufInfo_t *pframe);

//Demo
void demoRodDrawResultToRecord(frameBufInfo_t *pframe);

#ifdef __cplusplus
}
#endif

#endif
