#ifndef APP_TEST_ADAS_H
#define APP_TEST_ADAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sp5k_capture_api.h"
#include "sp5k_gfx_api.h"

#include "app_gps.h"

//Set feature enabled or not
#if TEST_ADAS_LDWS
#define IS_LDWS_ENABLED TRUE 
#else
#define IS_LDWS_ENABLED FALSE 
#endif

#if TEST_ADAS_FCWS
#define IS_FCWS_ENABLED TRUE
#else
#define IS_FCWS_ENABLED FALSE
#endif

#if TEST_ADAS_LPD
#define IS_LPD_ENABLED TRUE
#else
#define IS_LPD_ENABLED FALSE
#endif

typedef enum {
    E_ADAS_SOUND_LDWS_BEEP = 0,
    E_ADAS_SOUND_FCWS_BEEP,
    E_ADAS_SOUND_LEFT,
    E_ADAS_SOUND_RIGHT,
    E_ADAS_SOUND_ROD_BEEP_1TIMES_PER2SEC,
    E_ADAS_SOUND_ROD_BEEP_1TIMES_PERSEC,
    E_ADAS_SOUND_ROD_BEEP_2TIMES_PERSEC,
	E_ADAS_SOUND_UNDEFINED
} eADAS_Sound;

//Product OSD
void appAdasDrawProductOSD();

//Result
UINT32 appAdasRecResultCb(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup);
UINT32 appAdasResultDispCb(UINT32 streamId, UINT32 fid, frameBufInfo_t *pframe, UINT8 *pbufdup);
//Speed
void appSetAdasCarSpeed(const gpsInfo_t *pInGpsInfo);

void hostAdasStart();
void hostAdasStop();
void adasSetLdws(BOOL bEnable);
void adasSetFcws(BOOL bEnable);
void adasSetLpd(BOOL bEnable);
void adasSetRod(BOOL bEnable);

#ifdef __cplusplus
}
#endif

#endif
