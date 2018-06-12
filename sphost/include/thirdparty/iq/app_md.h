
#ifndef APP_ICATCH_MD_LIB_H
#define APP_ICATCH_MD_LIB_H

#ifdef __cplusplus
extern "C"{
#endif

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define MD_VER			            "00.01.07"

/****************************************************************************
MD_VER: 00.01.07:
    1. support yuv front cam
MD_VER: 00.01.06:
    1. support yuv rear cam


*****************************************************************************/
//for MD_VER: 00.01.06
#define CAM_REAR CAM_REAR_YUV


#define SP5K_MSGX_UI                (SP5K_MSG_HEAD_CUSTOMER+0xB000000)
#define MD_MSG_GRP                  0x800000
#define APP_UI_MSG_MD_START         (SP5K_MSGX_UI|MD_MSG_GRP|0x01)
#define APP_UI_MSG_MD_STOP          (SP5K_MSGX_UI|MD_MSG_GRP|0x02)
#define APP_UI_MSG_MD_REAR_START    (SP5K_MSGX_UI|MD_MSG_GRP|0x03)
#define APP_UI_MSG_MD_REAR_STOP     (SP5K_MSGX_UI|MD_MSG_GRP|0x04)
#define APP_UI_MSG_MD_FRONT_START    (SP5K_MSGX_UI|MD_MSG_GRP|0x05)
#define APP_UI_MSG_MD_FRONT_STOP     (SP5K_MSGX_UI|MD_MSG_GRP|0x06)


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 typedef enum {
    APP_MD_REG = 0,         //reg front
    APP_MD_FRONT_GRID_H_SIZE,
    APP_MD_FRONT_GRID_V_SIZE,
    APP_MD_REAR_GRID_H_SIZE,
    APP_MD_REAR_GRID_V_SIZE,
    APP_MD_EN,              //enable raw front
    APP_MD_EN_FRONT,        //enable yuv front 
    APP_MD_EN_REAR,         //enable yuv rear
    APP_MD_CHKLV_SET,
    APP_MD_CHKLV_FRONT_SET,
    APP_MD_CHKLV_REAR_SET,
    APP_MD_CUSTOM_CHKLV_SET,
    APP_MD_CHKLV_GET,
    APP_MD_CHKLV_FRONT_GET,
    APP_MD_CHKLV_REAR_GET,
    APP_MD_RESET,
    APP_MD_ROI
} appMDOption_e;

 typedef enum {
    APP_MD_LV_CUST = 0,
    APP_MD_LV1 = 0x65,
    APP_MD_LV2,
    APP_MD_LV3,
} appMDChkLV_e;

typedef enum {
    APP_MD_FRONT_LV1 = 0,        //High Sensitivity
    APP_MD_FRONT_LV2,
    APP_MD_FRONT_LV3,
    APP_MD_FRONT_LV_TOTAL
} appMDChkLV_Front_e;

typedef enum {
    APP_MD_REAR_LV1 = 0,        //High Sensitivity
    APP_MD_REAR_LV2,
    APP_MD_REAR_LV3,
    APP_MD_REAR_LV_TOTAL
} appMDChkLV_Rear_e;

typedef enum{
    CAM_FRONT = 0,
    CAM_FRONT_YUV, 
    CAM_REAR_YUV,
    CAM_TOTAL
} e_MDCam;

typedef struct MDInputPara_s{
    UINT32 nFrameID;
    UINT8 nNumVgrid;
    UINT8 nNumHgrid;
    UINT8* pGridYvalue;
} MDInputPara_t;

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 appMotionDetectSet(appMDOption_e cmd, ...);
void appMotionDetectHdr(const UINT32 *pInFrameId, ...);

#ifdef __cplusplus
}
#endif

#endif  /* APP_ICATCH_MD_LIB_H */

