#ifndef APP_TEST_ADAS_API_H
#define APP_TEST_ADAS_API_H


#ifdef __cplusplus
extern "C" {
#endif

#include "stdarg.h" //fixme, add to sp5k_adas_api.h

#include "api/sp5k_adas_api.h"
#include "app_test_adas.h"

//ADAS
//for HDMI INPUT, use different sensor video
//#define SP5K_SENSOR_AR0330CS 1

//OSD
#define LD_FC_ROD_OSD_PAGE (SP5K_GFX_PAGE_OSD_0+1)
#define LPD_OSD_PAGE (SP5K_GFX_PAGE_PIP_0+1)

//Product OSD config
#define PRODUCT_OSD_W   320
#define PRODUCT_OSD_H   240
#define VANISHING_PT_X  160
#define VANISHING_PT_Y  120

//ALGO: 1024x1024 based
#define ADAS_ALGO_W             1024
#define ADAS_ALGO_H             1024
#define DIVISION_ADAS_ALGO_W    >> 10
#define DIVISION_ADAS_ALGO_H    >> 10
#define ADAS_ALGO_CENTER_X      (ADAS_ALGO_W >> 1)
#define ADAS_ALGO_CENTER_Y      (ADAS_ALGO_H >> 1)

//PRODUCT: configuration
#define ADAS_OSD_W		        320
#define ADAS_OSD_H		        180
#define ADAS_OSD_OFFSET_H       30

#define ADAS_STR "[ADAS]"
#define CENTRALPOINT_X			160

#define LDWS_STR	"[LDWS]"
#define LDWS_LANE_DRAW_SUPPORT	1
#define LDWS_SOUND_PLAY_SUPPORT	0
#define LDWS_SOUND_WARNING_INTERVAL 3000        //algo team default 3sec

#define LDWS_ANGLE_THR	60
#define LDWS_OSD_DEFAULT_CENTER_Y       ((((ADAS_ALGO_H >> 1) * ADAS_OSD_H) >> 10) + ADAS_OSD_OFFSET_H)
//algorithm team: cooridinate 1024x 1024 , default center x, center y :512
#define LDWS_ADJUST_ALGO_CENTER_X   512
#define LDWS_ADJUST_ALGO_CENTER_Y   512

#define LDWS_SAFE_COLOR		        PAL_YELLOW_1
#define LDWS_WARN_COLOR		        PAL_RED
#define LDWS_VANISHINGPT_LINE_COLOR PAL_GREEN

#define LDWS_WARN_CNT_DBG		15

//FCWS
#define FCWS_STR	"[FCWS]"
#define FCWS_SENSOR_FOCAL		    SENSOR_FOCAL_LEN	/* SENSOR_FOCAL = focal length(mm)*10000 */
#define FCWS_SENSOR_PIXEL_UNIT	    SENSOR_PIXEL_UNIT	/* SENSOR_PIXEL_UNIT = (max_sensor_horizontal_resolution)*(sensor unit size(um))/320*10 */
#define FCWS_WARN_DISTANCE		    15
#define FCWS_CENTER_TOLER_FACTOR    80                  /* default: 100, algo team recommand : 70 ~ 130 */

// FCWS_ROI_CROP_FACTOR+FCWS_ROI_CROP_OFFSET * 2 = 1024 */
#define FCWS_ADJUST_ALGO_CENTER_X   512
#define FCWS_ADJUST_ALGO_CENTER_Y   512
#define FCWS_ROI_CROP_FACTOR	662
#define FCWS_ROI_CROP_OFFSET	181
#define FCWS_30FPS_INTERVAL 2
#define FCWS_60FPS_INTERVAL 4
#define FCWS_OTHER_INTERVAL 10
//LPD
#define LPD_STR	"[LPD]"

//ROD
#define ROD_STR	"[ROD]"

#ifdef __cplusplus
}
#endif

#endif
