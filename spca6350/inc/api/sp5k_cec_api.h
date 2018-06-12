/**************************************************************************
 *
 *       Copyright (c) 2011-2014 by iCatch Technology, Inc.
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file sp5k_cec_api.h
 *
 * @author Matt Wang
 */
#ifndef _SP5K_CEC_API_H_
#define _SP5K_CEC_API_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/disp_def.h"

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
#define SP5K_CEC_INST_TOT  1
#define SP5K_DDC_CHNL_TOT  1
#define SP5K_CEC_CHNL_TOT  1

#define SP5K_CEC_OPERAND_TBL_SIZE  14

/** @brief Control ID for sp5kCecControl(). */
typedef enum _sp5kCecCtrlId_e {
	/**
	 * @brief Open a CEC instance.
	 *
	 * <pre>
	 * UINT32
	 * sp5kCecControl(
	 *     UINT32 instId,
	 *     SP5K_CEC_CTRL_OPEN,
	 *     sp5kCecInstCfg_t *pcfg
	 * );
	 * </pre>
	 *
	 * @param [in] instId  Instance ID, 0~(SP5K_CEC_INST_TOT-1).
	 * @param [in] *pcfg   Configuration of instance, must use SP5K_CEC_INST_CFG_CLEAR() to clear whole data structure before setting parts of members for future extension.
	 * <pre>
	 * pcfg->ddcChnlId      DDC channel ID, 0~(SP5K_DDC_CHNL_TOT-1).
	 * pcfg->cecChnlId      CEC channel ID, 0~(SP5K_CEC_CHNL_TOT-1).
	 * pcfg->rxMsgCallback  Callback for recieving message.
	 * </pre>
	 *
	 * @return Instance handle or NULL if fail.
	 *
	 * @see sp5kCecInstCfg_t, sp5kCecRecieveMsgCallback_t
	 */
	SP5K_CEC_CTRL_OPEN,

	/**
	 * @brief Close a CEC instance.
	 *
	 * <pre>
	 * UINT32
	 * sp5kCecControl(
	 *     UINT32 instId,
	 *     SP5K_CEC_CTRL_CLOSE
	 * );
	 * </pre>
	 *
	 * @param [in] instId  Instance ID, 0~(SP5K_CEC_INST_TOT-1).
	 *
	 * @return SUCCESS or FAIL.
	 */
	SP5K_CEC_CTRL_CLOSE,

	/**
	 * @brief Start a CEC instance.
	 *
	 * <pre>
	 * UINT32
	 * sp5kCecControl(
	 *     UINT32 instId,
	 *     SP5K_CEC_CTRL_START
	 * );
	 * </pre>
	 *
	 * @param [in] instId  Instance ID, 0~(SP5K_CEC_INST_TOT-1).
	 *
	 * @return SUCCESS or FAIL.
	 */
	SP5K_CEC_CTRL_START,

	/**
	 * @brief Stop a CEC instance.
	 *
	 * <pre>
	 * UINT32
	 * sp5kCecControl(
	 *     UINT32 instId,
	 *     SP5K_CEC_CTRL_STOP
	 * );
	 * </pre>
	 *
	 * @param [in] instId  Instance ID, 0~(SP5K_CEC_INST_TOT-1).
	 *
	 * @return SUCCESS or FAIL.
	 */
	SP5K_CEC_CTRL_STOP,

	/**
	 * @brief Send message to CEC channel with waiting for ACK/NAK.
	 *
	 * <pre>
	 * UINT32
	 * sp5kCecControl(
	 *     UINT32 instId,
	 *     SP5K_CEC_CTRL_MSG_SEND_WAIT,
	 *     UINT32 iniAddr,
	 *     UINT32 flwAddr,
	 *     UINT32 opCode,
	 *     SINT32 oprNum,
	 *     UINT8 *poprTbl
	 * );
	 * </pre>
	 *
	 * @param [in] instId   Instance ID, 0~(SP5K_CEC_INST_TOT-1).
	 * @param [in] iniAddr  Initiator address, one of sp5kCecLogiAddr_e.
	 * @param [in] flwAddr  Follower address, one of sp5kCecLogiAddr_e.
	 * @param [in] opCode   Operation code, one of sp5kCecMsgId_e.
	 * @param [in] oprNum   Operand number, 0~SP5K_CEC_OPERAND_TBL_SIZE, or -1 for <Polling Message>.
	 * @param [in] poprTbl  Operand table, maximum size is SP5K_CEC_OPERAND_TBL_SIZE.
	 *
	 * @return One of sp5kCecErrCode_e. If not (SP5K_CEC_ERR_SENT_ACK or SP5K_CEC_ERR_SENT_NAK), the out value is undefined.
	 */
	SP5K_CEC_CTRL_MSG_SEND_WAIT,

	/**
	 * @brief Get discovered CEC device physical address.
	 *
	 * <pre>
	 * UINT32
	 * sp5kCecControl(
	 *     UINT32 instId,
	 *     SP5K_CEC_CTRL_PHYSICALL_ADDR_GET,
	 *     UINT32 *pphyAddr
	 * );
	 * </pre>
	 *
	 * @note cecPhysicalAddrDiscover() must be called first before calling cecCtrlPhysicalAddrGet().
	 *
	 * @param [in]  instId     Instance ID, 0~(SP5K_CEC_INST_TOT-1).
	 * @param [out] *pphyAddr  Physical address in A.B.C.D format, where bit[15:12]=A, [11:8]=B, [7:4]=C, [3:0]=D.
	 *
	 * @return SUCCESS or FAIL. If FAIL, the out value is undefined.
	 */
	SP5K_CEC_CTRL_PHYSICALL_ADDR_GET,

	/**
	 * @brief Allocate CEC device logical address.
	 *
	 * <pre>
	 * UINT32
	 * sp5kCecControl(
	 *     UINT32 instId,
	 *     SP5K_CEC_CTRL_LOGICAL_ADDR_ALLOC,
	 *     UINT32 devType,
	 *     UINT32 *plogiAddr
	 * );
	 * </pre>
	 *
	 * @note cecCtrlStart() must be called first before calling cecCtrlLogicalAddrAlloc().
	 *
	 * @param [in]  instId      Instance ID, 0~(SP5K_CEC_INST_TOT-1).
	 * @param [in]  devType     Device type, one of sp5kCecDevType_e, except SP5K_CEC_DEV_TYPE_TV and SP5K_CEC_DEV_TYPE_RESERVED.
	 * @param [out] *plogiAddr  Allocated logical address, one of sp5kCecLogiAddr_e.
	 *
	 * @return SUCCESS or FAIL. If FAIL, the out value is undefined.
	 */
	SP5K_CEC_CTRL_LOGICAL_ADDR_ALLOC,

	/**
	 * @brief Get allocated CEC device logical address.
	 *
	 * <pre>
	 * UINT32
	 * sp5kCecControl(
	 *     UINT32 instId,
	 *     SP5K_CEC_CTRL_LOGICAL_ADDR_GET,
	 *     UINT32 devType,
	 *     UINT32 *plogiAddr
	 * );
	 * </pre>
	 *
	 * @note cecCtrlLogicalAddrAlloc() must be called first before calling cecCtrlLogicalAddrGet().
	 *
	 * @param [in]  instId      Instance ID, 0~(SP5K_CEC_INST_TOT-1).
	 * @param [in]  devType     Device type, one of sp5kCecDevType_e, except SP5K_CEC_DEV_TYPE_TV and SP5K_CEC_DEV_TYPE_RESERVED.
	 * @param [out] *plogiAddr  Allocated logical address for expected device type, one of sp5kCecLogiAddr_e.
	 *
	 * @return SUCCESS or FAIL. If FAIL, the out value is undefined.
	 */
	SP5K_CEC_CTRL_LOGICAL_ADDR_GET,

	/**
	 * @brief Get TV supported video format (disp resolution) via DDC channel.
	 *
	 * <pre>
	 * UINT32
	 * sp5kCecControl(
	 *     UINT32 instId,
	 *     SP5K_CEC_EDID_VIDEO_FMT_GET,
	 *     UINT32 *pfmtNum,
	 *     sp5kCecVideoFmt_t **ppfmtTbl
	 * );
	 * </pre>
	 *
	 * @param [in]  instId     Instance ID, 0~(SP5K_CEC_INST_TOT-1).
	 * @param [out] *pfmtNum   Video format number.
	 * @param [out] *ppfmtTbl  Pointer to video format table, size is *pfmtNum, and each byte in table is one of sp5kEdidVideoFormat_e.
	 *
	 * @return SUCCESS or FAIL. If FAIL, the out value is undefined.
	 */
	SP5K_CEC_CTRL_VIDEO_FMT_GET,

	/**
	 * @brief Enable / disable message log (by printf) for all allocated logical addresses for debugging.
	 *
	 * <pre>
	 * UINT32
	 * sp5kCecControl(
	 *     UINT32 instId,
	 *     SP5K_CEC_CTRL_DBG_MSG_LOG_ENABLE_SET,
	 *     UINT32 en
	 * );
	 * </pre>
	 *
	 * @param [in] instId  Instance ID, 0~(SP5K_CEC_INST_TOT-1).
	 * @param [in] en      0: disable, 1: enable.
	 *
	 * @return SUCCESS or FAIL.
	 */
	SP5K_CEC_CTRL_DBG_MSG_LOG_ENABLE_SET,

	/**
	 * @brief Enable / disable monitoring all RX message on CEC pin.
	 *
	 * <pre>
	 * UINT32
	 * sp5kCecControl(
	 *     UINT32 instId,
	 *     SP5K_CEC_CTRL_RX_MSG_MON_ENABLE_SET,
	 *     UINT32 en
	 * );
	 * </pre>
	 *
	 * @param [in] instId  Instance ID, 0~(SP5K_CEC_INST_TOT-1).
	 * @param [in] en      0: disable, 1: enable.
	 *
	 * @return SUCCESS or FAIL.
	 */
	SP5K_CEC_CTRL_RX_MSG_MON_ENABLE_SET,
} sp5kCecCtrlId_e;

/** @brief CEC error code. */
typedef enum _sp5kCecErrCode_e {
	SP5K_CEC_ERR_NOT_PLUGGED   = 1,
	SP5K_CEC_ERR_INVALID_PARAM,
	SP5K_CEC_ERR_TX_BUSY,
	SP5K_CEC_ERR_TX_ERR,
	SP5K_CEC_ERR_TX_TIMEOUT,
	SP5K_CEC_ERR_SENT_ACK,
	SP5K_CEC_ERR_SENT_NAK,
} sp5kCecErrCode_e;

/** @brief CEC messages (opcodes), refer to spec HDMI 1.4 CEC Table 8~26. */
typedef enum _sp5kCecMsgId_e {
	SP5K_CEC_MSG_FEATURE_ABORT                 = 0x00,
	SP5K_CEC_MSG_IMAGE_VIEW_ON                 = 0x04,
	SP5K_CEC_MSG_TUNER_STEP_INCREMENT          = 0x05,
	SP5K_CEC_MSG_TUNER_STEP_DECREMENT          = 0x06,
	SP5K_CEC_MSG_TUNER_DEVICE_STATUS           = 0x07,
	SP5K_CEC_MSG_GIVE_TUNER_DEVICE_STATUS      = 0x08,
	SP5K_CEC_MSG_RECORD_ON                     = 0x09,
	SP5K_CEC_MSG_RECORD_STATUS                 = 0x0A,
	SP5K_CEC_MSG_RECORD_OFF                    = 0x0B,
	SP5K_CEC_MSG_TEXT_VIEW_ON                  = 0x0D,
	SP5K_CEC_MSG_RECORD_TV_SCREEN              = 0x0F,
	SP5K_CEC_MSG_GIVE_DECK_STATUS              = 0x1A,
	SP5K_CEC_MSG_DECK_STATUS                   = 0x1B,
	SP5K_CEC_MSG_SET_MENU_LANGUAGE             = 0x32,
	SP5K_CEC_MSG_CLEAR_ANALOGUE_TIMER          = 0x33,
	SP5K_CEC_MSG_SET_ANALOGUE_TIMER            = 0x34,
	SP5K_CEC_MSG_TIMER_STATUS                  = 0x35,
	SP5K_CEC_MSG_STANDBY                       = 0x36,
	SP5K_CEC_MSG_PLAY                          = 0x41,
	SP5K_CEC_MSG_DECK_CONTROL                  = 0x42,
	SP5K_CEC_MSG_TIMER_CLEARED_STATUS          = 0x43,
	SP5K_CEC_MSG_USER_CONTROL_PRESSED          = 0x44,  /* Remote Control Passthrough. */
	SP5K_CEC_MSG_USER_CONTROL_RELEASED         = 0x45,  /* Remote Control Passthrough. */
	SP5K_CEC_MSG_GIVE_OSD_NAME                 = 0x46,
	SP5K_CEC_MSG_SET_OSD_NAME                  = 0x47,
	SP5K_CEC_MSG_SET_OSD_STRING                = 0x64,
	SP5K_CEC_MSG_SET_TIMER_PROGRAM_TITLE       = 0x67,
	SP5K_CEC_MSG_SYSTEM_AUDIO_MODE_REQUEST     = 0x70,
	SP5K_CEC_MSG_GIVE_AUDIO_STATUS             = 0x71,
	SP5K_CEC_MSG_SET_SYSTEM_AUDIO_MODE         = 0x72,
	SP5K_CEC_MSG_REPORT_AUDIO_STATUS           = 0x7A,
	SP5K_CEC_MSG_GIVE_SYSTEM_AUDIO_MODE_STATUS = 0x7D,
	SP5K_CEC_MSG_SYSTEM_AUDIO_MODE_STATUS      = 0x7E,
	SP5K_CEC_MSG_ROUTING_CHANGE                = 0x80,
	SP5K_CEC_MSG_ROUTING_INFORMATION           = 0x81,
	SP5K_CEC_MSG_ACTIVE_SOURCE                 = 0x82,
	SP5K_CEC_MSG_GIVE_PHYSICAL_ADDRESS         = 0x83,
	SP5K_CEC_MSG_REPORT_PHYSICAL_ADDRESS       = 0x84,
	SP5K_CEC_MSG_REQUEST_ACTIVE_SOURCE         = 0x85,
	SP5K_CEC_MSG_SET_STREAM_PATH               = 0x86,
	SP5K_CEC_MSG_DEVICE_VENDOR_ID              = 0x87,
	SP5K_CEC_MSG_VENDOR_COMMAND                = 0x89,
	SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_DOWN     = 0x8A,
	SP5K_CEC_MSG_VENDOR_REMOTE_BUTTON_UP       = 0x8B,
	SP5K_CEC_MSG_GIVE_DEVICE_VENDOR_ID         = 0x8C,
	SP5K_CEC_MSG_MENU_REQUEST                  = 0x8D,
	SP5K_CEC_MSG_MENU_STATUS                   = 0x8E,
	SP5K_CEC_MSG_GIVE_DEVICE_POWER_STATUS      = 0x8F,
	SP5K_CEC_MSG_REPORT_POWER_STATUS           = 0x90,
	SP5K_CEC_MSG_GET_MENU_LANGUAGE             = 0x91,
	SP5K_CEC_MSG_SELECT_ANALOGUE_SERVICE       = 0x92,
	SP5K_CEC_MSG_SELECT_DIGITAL_SERVICE        = 0x93,
	SP5K_CEC_MSG_SET_DIGITAL_TIMER             = 0x97,
	SP5K_CEC_MSG_CLEAR_DIGITAL_TIMER           = 0x99,
	SP5K_CEC_MSG_SET_AUDIO_RATE                = 0x9A,
	SP5K_CEC_MSG_INACTIVE_SOURCE               = 0x9D,
	SP5K_CEC_MSG_CEC_VERSION                   = 0x9E,
	SP5K_CEC_MSG_GET_CEC_VERSION               = 0x9F,
	SP5K_CEC_MSG_VENDOR_COMMAND_WITH_ID        = 0xA0,
	SP5K_CEC_MSG_CLEAR_EXTERNAL_TIMER          = 0xA1,
	SP5K_CEC_MSG_SET_EXTERNAL_TIMER            = 0xA2,
	SP5K_CEC_MSG_INITIATE_ARC                  = 0xC0,
	SP5K_CEC_MSG_REPORT_ARC_INITIATED          = 0xC1,
	SP5K_CEC_MSG_REPORT_ARC_TERMINATED         = 0xC2,
	SP5K_CEC_MSG_REQUEST_ARC_INITIATION        = 0xC3,
	SP5K_CEC_MSG_REQUEST_ARC_TERMINATION       = 0xC4,
	SP5K_CEC_MSG_TERMINATE_ARC                 = 0xC5,
	SP5K_CEC_MSG_CDC_MESSAGE                   = 0xF8,
	SP5K_CEC_MSG_ABORT                         = 0xFF,
} sp5kCecMsgId_e;

/** @brief Logical addresses, refer to spec HDMI 1.4 CEC Table 5. */
typedef enum _sp5kCecLogiAddr_e {
	SP5K_CEC_LOGI_ADDR_TV                 =  0,
	SP5K_CEC_LOGI_ADDR_RECORDING_DEVICE_1 =  1,
	SP5K_CEC_LOGI_ADDR_RECORDING_DEVICE_2 =  2,
	SP5K_CEC_LOGI_ADDR_TUNER_1            =  3,
	SP5K_CEC_LOGI_ADDR_PLAYBACK_DEVICE_1  =  4,
	SP5K_CEC_LOGI_ADDR_AUDIO_SYSTEM       =  5,
	SP5K_CEC_LOGI_ADDR_TUNER_2            =  6,
	SP5K_CEC_LOGI_ADDR_TUNER_3            =  7,
	SP5K_CEC_LOGI_ADDR_PLAYBACK_DEVICE_2  =  8,
	SP5K_CEC_LOGI_ADDR_RECORDING_DEVICE_3 =  9,
	SP5K_CEC_LOGI_ADDR_TUNER_4            = 10,
	SP5K_CEC_LOGI_ADDR_PLAYBACK_DEVICE_3  = 11,
	SP5K_CEC_LOGI_ADDR_RESERVED_1         = 12,
	SP5K_CEC_LOGI_ADDR_RESERVED_2         = 13,
	SP5K_CEC_LOGI_ADDR_SPECIFIC_USE       = 14,
	SP5K_CEC_LOGI_ADDR_UNREGISTERED       = 15,  /* as initiator address. */
	SP5K_CEC_LOGI_ADDR_BROADCAST          = 15,  /* as follower address. */
} sp5kCecLogiAddr_e;

/** @brief Reason for a <Feature Abort> response, refer to spec HDMI 1.4 CEC Table 29 [Abort Reason]. */
typedef enum _sp5kCecAbortReason_e {
	SP5K_CEC_ABORT_UNRECOGNIZED_OPCODE           = 0,
	SP5K_CEC_ABORT_NOT_INCORRECT_MODE_TO_RESPOND = 1,
	SP5K_CEC_ABORT_CANNOT_PROVIDE_SOURCE         = 2,
	SP5K_CEC_ABORT_INVALID_OPERAND               = 3,
	SP5K_CEC_ABORT_REFUSED                       = 4,
	SP5K_CEC_ABORT_UNABLE_TO_DETERMINE           = 5,
} sp5kCecAbortReason_e;

/** @brief Used in <Deck Control>, refer to spec HDMI 1.4 CEC Table 29 [Deck Control Mode]. */
typedef enum _sp5kCecDeckCtrlMode_e {
	SP5K_CEC_DECK_CTRL_SKIP_FORWARD = 1,
	SP5K_CEC_DECK_CTRL_SKIP_REVERSE = 2,
	SP5K_CEC_DECK_CTRL_STOP         = 3,
	SP5K_CEC_DECK_CTRL_REJECT       = 4,
} sp5kCecDeckCtrlMode_e;

/** @brief Indicate the current status of a tape or disk deck, refer to spec HDMI 1.4 CEC Table 29 [Deck Info]. */
typedef enum _sp5kCecDeckInfo_e {
	SP5K_CEC_DECK_INFO_PLAY                 = 0x11,
	SP5K_CEC_DECK_INFO_RECORD               = 0x12,
	SP5K_CEC_DECK_INFO_PLAY_REVERSE         = 0x13,
	SP5K_CEC_DECK_INFO_STILL                = 0x14,
	SP5K_CEC_DECK_INFO_SLOW                 = 0x15,
	SP5K_CEC_DECK_INFO_SLOW_REVERSE         = 0x16,
	SP5K_CEC_DECK_INFO_FAST_FORWARD         = 0x17,
	SP5K_CEC_DECK_INFO_FAST_REVERSE         = 0x18,
	SP5K_CEC_DECK_INFO_NO_MEDIA             = 0x19,
	SP5K_CEC_DECK_INFO_STOP                 = 0x1A,
	SP5K_CEC_DECK_INFO_SKIP_FORWARD         = 0x1B,
	SP5K_CEC_DECK_INFO_SKIP_REVERSE         = 0x1C,
	SP5K_CEC_DECK_INFO_INDEX_SEARCH_FORWARD = 0x1D,
	SP5K_CEC_DECK_INFO_INDEX_SEARCH_REVERSE = 0x1E,
	SP5K_CEC_DECK_INFO_OTHER_STATUS         = 0x1F,
} sp5kCecDeckInfo_e;

/** @brief Used by a device to indicate its device type, refer to spec HDMI 1.4 CEC Table 29 [Device Type]. */
typedef enum _sp5kCecDevType_e {
	SP5K_CEC_DEV_TYPE_TV               = 0,
	SP5K_CEC_DEV_TYPE_RECORDING_DEVICE = 1,
	SP5K_CEC_DEV_TYPE_RESERVED         = 2,
	SP5K_CEC_DEV_TYPE_TUNER            = 3,
	SP5K_CEC_DEV_TYPE_PLAYBACK_DEVICE  = 4,
	SP5K_CEC_DEV_TYPE_AUDIO_SYSTEM     = 5,
	SP5K_CEC_DEV_TYPE_PURE_CEC_SWITCH  = 6,
	SP5K_CEC_DEV_TYPE_VIDEO_PROCESSOR  = 7,
} sp5kCecDevType_e;

/** @brief Used in <Menu Requesr>, refer to spec HDMI 1.4 CEC Table 29 [Menu Request Type]. */
typedef enum _sp5kCecMenuRequestType_e {
	SP5K_CEC_MENU_ACTIVATE   = 0,
	SP5K_CEC_MENU_DEACTIVATE = 1,
	SP5K_CEC_MENU_QUERY      = 2,
} sp5kCecMenuRequestType_e;

/** @brief Used in <Menu Status>, refer to spec HDMI 1.4 CEC Table 29 [Menu State]. */
typedef enum _sp5kCecMenuState_e {
	SP5K_CEC_MENU_ACTIVATED   = 0,
	SP5K_CEC_MENU_DEACTIVATED = 1,
} sp5kCecMenuState_e;

/** @brief The mode in which to play media, refer to spec HDMI 1.4 CEC Table 29 [Play Mode]. */
typedef enum _sp5kCecPlayMode_e {
	SP5K_CEC_PLAY_MODE_PLAY_FORWARD              = 0x24,
	SP5K_CEC_PLAY_MODE_PLAY_REVERSE              = 0x20,
	SP5K_CEC_PLAY_MODE_PLAY_STILL                = 0x25,
	SP5K_CEC_PLAY_MODE_FAST_FORWARD_MIN_SPEED    = 0x05,
	SP5K_CEC_PLAY_MODE_FAST_FORWARD_MEDIUM_SPEED = 0x06,
	SP5K_CEC_PLAY_MODE_FAST_FORWARD_MAX_SPEED    = 0x07,
	SP5K_CEC_PLAY_MODE_FAST_REVERSE_MIN_SPEED    = 0x09,
	SP5K_CEC_PLAY_MODE_FAST_REVERSE_MEDIUM_SPEED = 0x0A,
	SP5K_CEC_PLAY_MODE_FAST_REVERSE_MAX_SPEED    = 0x0B,
	SP5K_CEC_PLAY_MODE_SLOW_FORWARD_MIN_SPEED    = 0x15,
	SP5K_CEC_PLAY_MODE_SLOW_FORWARD_MEDIUM_SPEED = 0x16,
	SP5K_CEC_PLAY_MODE_SLOW_FORWARD_MAX_SPEED    = 0x17,
	SP5K_CEC_PLAY_MODE_SLOW_REVERSE_MIN_SPEED    = 0x19,
	SP5K_CEC_PLAY_MODE_SLOW_REVERSE_MEDIUM_SPEED = 0x1A,
	SP5K_CEC_PLAY_MODE_SLOW_REVERSE_MAX_SPEED    = 0x1B,
} sp5kCecPlayMode_e;

/** @brief Used to indicate the current power status of a device, refer to spec HDMI 1.4 CEC Table 29 [Power Status]. */
typedef enum _sp5kCecPowerStatus_e {
	SP5K_CEC_POWER_ON            = 0x00,
	SP5K_CEC_POWER_STANDBY       = 0x01,
	SP5K_CEC_POWER_STANDBY_TO_ON = 0x02,
	SP5K_CEC_POWER_ON_TO_STANDBY = 0x03,
} sp5kCecPowerStatus_e;

/** @brief User control codes, refer to spec HDMI 1.4 CEC Table 30. */
typedef enum _sp5kCecUserCtrlCode_e {
	SP5K_CEC_USER_CTRL_SELECT                       = 0x00,
	SP5K_CEC_USER_CTRL_UP                           = 0x01,
	SP5K_CEC_USER_CTRL_DOWN                         = 0x02,
	SP5K_CEC_USER_CTRL_LEFT                         = 0x03,
	SP5K_CEC_USER_CTRL_RIGHT                        = 0x04,
	SP5K_CEC_USER_CTRL_RIGHT_UP                     = 0x05,
	SP5K_CEC_USER_CTRL_RIGHT_DOWN                   = 0x06,
	SP5K_CEC_USER_CTRL_LEFT_UP                      = 0x07,
	SP5K_CEC_USER_CTRL_LEFT_DOWN                    = 0x08,
	SP5K_CEC_USER_CTRL_ROOT_MENU                    = 0x09,
	SP5K_CEC_USER_CTRL_SETUP_MENU                   = 0x0A,
	SP5K_CEC_USER_CTRL_CONTENTS_MENU                = 0x0B,
	SP5K_CEC_USER_CTRL_FAVORITE_MENU                = 0x0C,
	SP5K_CEC_USER_CTRL_EXIT                         = 0x0D,
	SP5K_CEC_USER_CTRL_MEDIA_TOP_MENU               = 0x10,
	SP5K_CEC_USER_CTRL_MEDIA_CONTEXT_SENSITIVE_MENU = 0x11,
	SP5K_CEC_USER_CTRL_NUMBER_ENTRY_MODE            = 0x1D,
	SP5K_CEC_USER_CTRL_NUMBER_11                    = 0x1E,
	SP5K_CEC_USER_CTRL_NUMBER_12                    = 0x1F,
	SP5K_CEC_USER_CTRL_NUMBER_0                     = 0x20,
	SP5K_CEC_USER_CTRL_NUMBER_1                     = 0x21,
	SP5K_CEC_USER_CTRL_NUMBER_2                     = 0x22,
	SP5K_CEC_USER_CTRL_NUMBER_3                     = 0x23,
	SP5K_CEC_USER_CTRL_NUMBER_4                     = 0x24,
	SP5K_CEC_USER_CTRL_NUMBER_5                     = 0x25,
	SP5K_CEC_USER_CTRL_NUMBER_6                     = 0x26,
	SP5K_CEC_USER_CTRL_NUMBER_7                     = 0x27,
	SP5K_CEC_USER_CTRL_NUMBER_8                     = 0x28,
	SP5K_CEC_USER_CTRL_NUMBER_9                     = 0x29,
	SP5K_CEC_USER_CTRL_DOT                          = 0x2A,
	SP5K_CEC_USER_CTRL_ENTER                        = 0x2B,
	SP5K_CEC_USER_CTRL_CLEAR                        = 0x2C,
	SP5K_CEC_USER_CTRL_NEXT_FAVORITE                = 0x2F,
	SP5K_CEC_USER_CTRL_CHANNEL_UP                   = 0x30,
	SP5K_CEC_USER_CTRL_CHANNEL_DOWN                 = 0x31,
	SP5K_CEC_USER_CTRL_PREVIOUS_CHANNEL             = 0x32,
	SP5K_CEC_USER_CTRL_SOUND_SELECT                 = 0x33,
	SP5K_CEC_USER_CTRL_INPUT_SELECT                 = 0x34,
	SP5K_CEC_USER_CTRL_DISPLAY_INFORMATION          = 0x35,
	SP5K_CEC_USER_CTRL_HELP                         = 0x36,
	SP5K_CEC_USER_CTRL_PAGE_UP                      = 0x37,
	SP5K_CEC_USER_CTRL_PAGE_DOWN                    = 0x38,
	SP5K_CEC_USER_CTRL_POWER                        = 0x40,
	SP5K_CEC_USER_CTRL_VOLUME_UP                    = 0x41,
	SP5K_CEC_USER_CTRL_VOLUME_DOWN                  = 0x42,
	SP5K_CEC_USER_CTRL_MUTE                         = 0x43,
	SP5K_CEC_USER_CTRL_PLAY                         = 0x44,
	SP5K_CEC_USER_CTRL_STOP                         = 0x45,
	SP5K_CEC_USER_CTRL_PAUSE                        = 0x46,
	SP5K_CEC_USER_CTRL_RECORD                       = 0x47,
	SP5K_CEC_USER_CTRL_REWIND                       = 0x48,
	SP5K_CEC_USER_CTRL_FAST_FORWARD                 = 0x49,
	SP5K_CEC_USER_CTRL_EJECT                        = 0x4A,
	SP5K_CEC_USER_CTRL_FORWARD                      = 0x4B,
	SP5K_CEC_USER_CTRL_BACKWARD                     = 0x4C,
	SP5K_CEC_USER_CTRL_STOP_RECORD                  = 0x4D,
	SP5K_CEC_USER_CTRL_PAUSE_RECORD                 = 0x4E,
	SP5K_CEC_USER_CTRL_RESERVED                     = 0x4F,
	SP5K_CEC_USER_CTRL_ANGLE                        = 0x50,
	SP5K_CEC_USER_CTRL_SUB_PICTURE                  = 0x51,
	SP5K_CEC_USER_CTRL_VIDEO_ON_DEMAND              = 0x52,
	SP5K_CEC_USER_CTRL_ELECTRONIC_PROGRAM_GUIDE     = 0x53,
	SP5K_CEC_USER_CTRL_TIMER_PROGRAMMING            = 0x54,
	SP5K_CEC_USER_CTRL_INITIAL_CONFIGURATION        = 0x55,
	SP5K_CEC_USER_CTRL_PLAY_FUNCTION                = 0x60,
	SP5K_CEC_USER_CTRL_PAUSE_PLAY_FUNCTION          = 0x61,
	SP5K_CEC_USER_CTRL_RECORD_FUNCTION              = 0x62,
	SP5K_CEC_USER_CTRL_PAUSE_RECORD_FUNCTION        = 0x63,
	SP5K_CEC_USER_CTRL_STOP_FUNCTION                = 0x64,
	SP5K_CEC_USER_CTRL_MUTE_FUNCTION                = 0x65,
	SP5K_CEC_USER_CTRL_RESTORE_VOLUME_FUNCTION      = 0x66,
	SP5K_CEC_USER_CTRL_TUNE_FUNCTION                = 0x67,
	SP5K_CEC_USER_CTRL_SELECT_MEDIA_FUNCTION        = 0x68,
	SP5K_CEC_USER_CTRL_SELECT_AV_INPUT_FUNCTION     = 0x69,
	SP5K_CEC_USER_CTRL_SELECT_AUDIO_INPUT_FUNCTION  = 0x6A,
	SP5K_CEC_USER_CTRL_POWER_TOGGLE_FUNCTION        = 0x6B,
	SP5K_CEC_USER_CTRL_POWER_OFF_FUNCTION           = 0x6C,
	SP5K_CEC_USER_CTRL_POWER_ON_FUNCTION            = 0x6D,
	SP5K_CEC_USER_CTRL_F1_BLUE                      = 0x71,
	SP5K_CEC_USER_CTRL_F2_RED                       = 0x72,
	SP5K_CEC_USER_CTRL_F3_GREEN                     = 0x73,
	SP5K_CEC_USER_CTRL_F4_YELLOW                    = 0x74,
	SP5K_CEC_USER_CTRL_F5                           = 0x75,
	SP5K_CEC_USER_CTRL_DATA                         = 0x76,
} sp5kCecUserCtrlCode_e;

/** @brief Used in <CEC Version>, refer to spec HDMI 1.4 CEC Table 29 [CEC Version]. */
typedef enum _sp5kCecVersion_e {
	SP5K_CEC_VER_1_3A = 0x04,
	SP5K_CEC_VER_1_4  = 0x05,
	SP5K_CEC_VER_1_4A = 0x05,
	SP5K_CEC_VER_1_4B = 0x05,
} sp5kCecVersion_e;

/** @brief EDID video format, refer to spec CEA-861-F Table 3 and HDMI 1.4 Table H-7. */
typedef enum _sp5kEdidVideoFormat_e {
	SP5K_EDID_VIC_MSK                      = DISP_EDID_VIC_MSK,
	SP5K_EDID_640X480P_60HZ_4T3            = DISP_EDID_640X480P_60HZ_4T3,
	SP5K_EDID_720X480P_60HZ_4T3            = DISP_EDID_720X480P_60HZ_4T3,
	SP5K_EDID_720X480P_60HZ_16T9           = DISP_EDID_720X480P_60HZ_16T9,
	SP5K_EDID_1280X720P_60HZ_16T9          = DISP_EDID_1280X720P_60HZ_16T9,
	SP5K_EDID_1920X1080I_60HZ_16T9         = DISP_EDID_1920X1080I_60HZ_16T9,
	SP5K_EDID_720X480I_60HZ_4T3            = DISP_EDID_720X480I_60HZ_4T3,
	SP5K_EDID_720X480I_60HZ_16T9           = DISP_EDID_720X480I_60HZ_16T9,
	SP5K_EDID_720X240P_60HZ_4T3            = DISP_EDID_720X240P_60HZ_4T3,
	SP5K_EDID_720X240P_60HZ_16T9           = DISP_EDID_720X240P_60HZ_16T9,
	SP5K_EDID_2880X480I_60HZ_4T3           = DISP_EDID_2880X480I_60HZ_4T3,
	SP5K_EDID_2880X480I_60HZ_16T9          = DISP_EDID_2880X480I_60HZ_16T9,
	SP5K_EDID_2880X240P_60HZ_4T3           = DISP_EDID_2880X240P_60HZ_4T3,
	SP5K_EDID_2880X240P_60HZ_16T9          = DISP_EDID_2880X240P_60HZ_16T9,
	SP5K_EDID_1440X480P_60HZ_4T3           = DISP_EDID_1440X480P_60HZ_4T3,
	SP5K_EDID_1440X480P_60HZ_16T9          = DISP_EDID_1440X480P_60HZ_16T9,
	SP5K_EDID_1920X1080P_60HZ_16T9         = DISP_EDID_1920X1080P_60HZ_16T9,
	SP5K_EDID_720X576P_50HZ_4T3            = DISP_EDID_720X576P_50HZ_4T3,
	SP5K_EDID_720X576P_50HZ_16T9           = DISP_EDID_720X576P_50HZ_16T9,
	SP5K_EDID_1280X720P_50HZ_16T9          = DISP_EDID_1280X720P_50HZ_16T9,
	SP5K_EDID_1920X1080I_50HZ_16T9         = DISP_EDID_1920X1080I_50HZ_16T9,
	SP5K_EDID_720X576I_50HZ_4T3            = DISP_EDID_720X576I_50HZ_4T3,
	SP5K_EDID_720X576I_50HZ_16T9           = DISP_EDID_720X576I_50HZ_16T9,
	SP5K_EDID_720X288P_50HZ_4T3            = DISP_EDID_720X288P_50HZ_4T3,
	SP5K_EDID_720X288P_50HZ_16T9           = DISP_EDID_720X288P_50HZ_16T9,
	SP5K_EDID_2880X576I_50HZ_4T3           = DISP_EDID_2880X576I_50HZ_4T3,
	SP5K_EDID_2880X576I_50HZ_16T9          = DISP_EDID_2880X576I_50HZ_16T9,
	SP5K_EDID_2880X288P_50HZ_4T3           = DISP_EDID_2880X288P_50HZ_4T3,
	SP5K_EDID_2880X288P_50HZ_16T9          = DISP_EDID_2880X288P_50HZ_16T9,
	SP5K_EDID_1440X576P_50HZ_4T3           = DISP_EDID_1440X576P_50HZ_4T3,
	SP5K_EDID_1440X576P_50HZ_16T9          = DISP_EDID_1440X576P_50HZ_16T9,
	SP5K_EDID_1920X1080P_50HZ_16T9         = DISP_EDID_1920X1080P_50HZ_16T9,
	SP5K_EDID_1920X1080P_24HZ_16T9         = DISP_EDID_1920X1080P_24HZ_16T9,
	SP5K_EDID_1920X1080P_25HZ_16T9         = DISP_EDID_1920X1080P_25HZ_16T9,
	SP5K_EDID_1920X1080P_30HZ_16T9         = DISP_EDID_1920X1080P_30HZ_16T9,
	SP5K_EDID_2880X480P_60HZ_4T3           = DISP_EDID_2880X480P_60HZ_4T3,
	SP5K_EDID_2880X480P_60HZ_16T9          = DISP_EDID_2880X480P_60HZ_16T9,
	SP5K_EDID_2880X576P_50HZ_4T3           = DISP_EDID_2880X576P_50HZ_4T3,
	SP5K_EDID_2880X576P_50HZ_16T9          = DISP_EDID_2880X576P_50HZ_16T9,
	SP5K_EDID_1920X1080I_1250TOT_50HZ_16T9 = DISP_EDID_1920X1080I_1250TOT_50HZ_16T9,
	SP5K_EDID_1920X1080I_100HZ_16T9        = DISP_EDID_1920X1080I_100HZ_16T9,
	SP5K_EDID_1280X720P_100HZ_16T9         = DISP_EDID_1280X720P_100HZ_16T9,
	SP5K_EDID_720X576P_100HZ_4T3           = DISP_EDID_720X576P_100HZ_4T3,
	SP5K_EDID_720X576P_100HZ_16T9          = DISP_EDID_720X576P_100HZ_16T9,
	SP5K_EDID_720X576I_100HZ_4T3           = DISP_EDID_720X576I_100HZ_4T3,
	SP5K_EDID_720X576I_100HZ_16T9          = DISP_EDID_720X576I_100HZ_16T9,
	SP5K_EDID_1920X1080I_120HZ_16T9        = DISP_EDID_1920X1080I_120HZ_16T9,
	SP5K_EDID_1280X720P_120HZ_16T9         = DISP_EDID_1280X720P_120HZ_16T9,
	SP5K_EDID_720X480P_120HZ_4T3           = DISP_EDID_720X480P_120HZ_4T3,
	SP5K_EDID_720X480P_120HZ_16T9          = DISP_EDID_720X480P_120HZ_16T9,
	SP5K_EDID_720X480I_120HZ_4T3           = DISP_EDID_720X480I_120HZ_4T3,
	SP5K_EDID_720X480I_120HZ_16T9          = DISP_EDID_720X480I_120HZ_16T9,
	SP5K_EDID_720X576P_200HZ_4T3           = DISP_EDID_720X576P_200HZ_4T3,
	SP5K_EDID_720X576P_200HZ_16T9          = DISP_EDID_720X576P_200HZ_16T9,
	SP5K_EDID_720X576I_200HZ_4T3           = DISP_EDID_720X576I_200HZ_4T3,
	SP5K_EDID_720X576I_200HZ_16T9          = DISP_EDID_720X576I_200HZ_16T9,
	SP5K_EDID_720X480P_240HZ_4T3           = DISP_EDID_720X480P_240HZ_4T3,
	SP5K_EDID_720X480P_240HZ_16T9          = DISP_EDID_720X480P_240HZ_16T9,
	SP5K_EDID_720X480I_240HZ_4T3           = DISP_EDID_720X480I_240HZ_4T3,
	SP5K_EDID_720X480I_240HZ_16T9          = DISP_EDID_720X480I_240HZ_16T9,
	SP5K_EDID_1280X720P_24HZ_16T9          = DISP_EDID_1280X720P_24HZ_16T9,
	SP5K_EDID_1280X720P_25HZ_16T9          = DISP_EDID_1280X720P_25HZ_16T9,
	SP5K_EDID_1280X720P_30HZ_16T9          = DISP_EDID_1280X720P_30HZ_16T9,
	SP5K_EDID_1920X1080P_120HZ_16T9        = DISP_EDID_1920X1080P_120HZ_16T9,
	SP5K_EDID_1920X1080P_100HZ_16T9        = DISP_EDID_1920X1080P_100HZ_16T9,
	SP5K_EDID_1280X720P_24HZ_64T27         = DISP_EDID_1280X720P_24HZ_64T27,
	SP5K_EDID_1280X720P_25HZ_64T27         = DISP_EDID_1280X720P_25HZ_64T27,
	SP5K_EDID_1280X720P_30HZ_64T27         = DISP_EDID_1280X720P_30HZ_64T27,
	SP5K_EDID_1280X720P_50HZ_64T27         = DISP_EDID_1280X720P_50HZ_64T27,
	SP5K_EDID_1280X720P_60HZ_64T27         = DISP_EDID_1280X720P_60HZ_64T27,
	SP5K_EDID_1280X720P_100HZ_64T27        = DISP_EDID_1280X720P_100HZ_64T27,
	SP5K_EDID_1280X720P_120HZ_64T27        = DISP_EDID_1280X720P_120HZ_64T27,
	SP5K_EDID_1920X1080P_24HZ_64T27        = DISP_EDID_1920X1080P_24HZ_64T27,
	SP5K_EDID_1920X1080P_25HZ_64T27        = DISP_EDID_1920X1080P_25HZ_64T27,
	SP5K_EDID_1920X1080P_30HZ_64T27        = DISP_EDID_1920X1080P_30HZ_64T27,
	SP5K_EDID_1920X1080P_50HZ_64T27        = DISP_EDID_1920X1080P_50HZ_64T27,
	SP5K_EDID_1920X1080P_60HZ_64T27        = DISP_EDID_1920X1080P_60HZ_64T27,
	SP5K_EDID_1920X1080P_100HZ_64T27       = DISP_EDID_1920X1080P_100HZ_64T27,
	SP5K_EDID_1920X1080P_120HZ_64T27       = DISP_EDID_1920X1080P_120HZ_64T27,
	SP5K_EDID_1680X720P_24HZ_64T27         = DISP_EDID_1680X720P_24HZ_64T27,
	SP5K_EDID_1680X720P_25HZ_64T27         = DISP_EDID_1680X720P_25HZ_64T27,
	SP5K_EDID_1680X720P_30HZ_64T27         = DISP_EDID_1680X720P_30HZ_64T27,
	SP5K_EDID_1680X720P_50HZ_64T27         = DISP_EDID_1680X720P_50HZ_64T27,
	SP5K_EDID_1680X720P_60HZ_64T27         = DISP_EDID_1680X720P_60HZ_64T27,
	SP5K_EDID_1680X720P_100HZ_64T27        = DISP_EDID_1680X720P_100HZ_64T27,
	SP5K_EDID_1680X720P_120HZ_64T27        = DISP_EDID_1680X720P_120HZ_64T27,
	SP5K_EDID_2560X1080P_24HZ_64T27        = DISP_EDID_2560X1080P_24HZ_64T27,
	SP5K_EDID_2560X1080P_25HZ_64T27        = DISP_EDID_2560X1080P_25HZ_64T27,
	SP5K_EDID_2560X1080P_30HZ_64T27        = DISP_EDID_2560X1080P_30HZ_64T27,
	SP5K_EDID_2560X1080P_50HZ_64T27        = DISP_EDID_2560X1080P_50HZ_64T27,
	SP5K_EDID_2560X1080P_60HZ_64T27        = DISP_EDID_2560X1080P_60HZ_64T27,
	SP5K_EDID_2560X1080P_100HZ_64T27       = DISP_EDID_2560X1080P_100HZ_64T27,
	SP5K_EDID_2560X1080P_120HZ_64T27       = DISP_EDID_2560X1080P_120HZ_64T27,
	SP5K_EDID_3840X2160P_24HZ_16T9         = DISP_EDID_3840X2160P_24HZ_16T9,
	SP5K_EDID_3840X2160P_25HZ_16T9         = DISP_EDID_3840X2160P_25HZ_16T9,
	SP5K_EDID_3840X2160P_30HZ_16T9         = DISP_EDID_3840X2160P_30HZ_16T9,
	SP5K_EDID_3840X2160P_50HZ_16T9         = DISP_EDID_3840X2160P_50HZ_16T9,
	SP5K_EDID_3840X2160P_60HZ_16T9         = DISP_EDID_3840X2160P_60HZ_16T9,
	SP5K_EDID_4096X2160P_24HZ_256T135      = DISP_EDID_4096X2160P_24HZ_256T135,
	SP5K_EDID_4096X2160P_25HZ_256T135      = DISP_EDID_4096X2160P_25HZ_256T135,
	SP5K_EDID_4096X2160P_30HZ_256T135      = DISP_EDID_4096X2160P_30HZ_256T135,
	SP5K_EDID_4096X2160P_50HZ_256T135      = DISP_EDID_4096X2160P_50HZ_256T135,
	SP5K_EDID_4096X2160P_60HZ_256T135      = DISP_EDID_4096X2160P_60HZ_256T135,
	SP5K_EDID_3840X2160P_24HZ_64T27        = DISP_EDID_3840X2160P_24HZ_64T27,
	SP5K_EDID_3840X2160P_25HZ_64T27        = DISP_EDID_3840X2160P_25HZ_64T27,
	SP5K_EDID_3840X2160P_30HZ_64T27        = DISP_EDID_3840X2160P_30HZ_64T27,
	SP5K_EDID_3840X2160P_50HZ_64T27        = DISP_EDID_3840X2160P_50HZ_64T27,
	SP5K_EDID_3840X2160P_60HZ_64T27        = DISP_EDID_3840X2160P_60HZ_64T27,

	/* alias. */
	SP5K_EDID_1440X480I_60HZ_4T3           = DISP_EDID_720X480I_60HZ_4T3,
	SP5K_EDID_1440X480I_60HZ_16T9          = DISP_EDID_720X480I_60HZ_16T9,
	SP5K_EDID_1440X240P_60HZ_4T3           = DISP_EDID_720X240P_60HZ_4T3,
	SP5K_EDID_1440X240P_60HZ_16T9          = DISP_EDID_720X240P_60HZ_16T9,
	SP5K_EDID_1440X576I_50HZ_4T3           = DISP_EDID_720X576I_50HZ_4T3,
	SP5K_EDID_1440X576I_50HZ_16T9          = DISP_EDID_720X576I_50HZ_16T9,
	SP5K_EDID_1440X288P_50HZ_4T3           = DISP_EDID_720X288P_50HZ_4T3,
	SP5K_EDID_1440X288P_50HZ_16T9          = DISP_EDID_720X288P_50HZ_16T9,
	SP5K_EDID_1440X576I_100HZ_4T3          = DISP_EDID_720X576I_100HZ_4T3,
	SP5K_EDID_1440X576I_100HZ_16T9         = DISP_EDID_720X576I_100HZ_16T9,
	SP5K_EDID_1440X480I_120HZ_4T3          = DISP_EDID_720X480I_120HZ_4T3,
	SP5K_EDID_1440X480I_120HZ_16T9         = DISP_EDID_720X480I_120HZ_16T9,
	SP5K_EDID_1440X576I_200HZ_4T3          = DISP_EDID_720X576I_200HZ_4T3,
	SP5K_EDID_1440X576I_200HZ_16T9         = DISP_EDID_720X576I_200HZ_16T9,
	SP5K_EDID_1440X480I_240HZ_4T3          = DISP_EDID_720X480I_240HZ_4T3,
	SP5K_EDID_1440X480I_240HZ_16T9         = DISP_EDID_720X480I_240HZ_16T9,

	SP5K_EDID_3D_STRUCT_MSK                = DISP_EDID_3D_STRUCT_MSK,
	SP5K_EDID_3D_FP                        = DISP_EDID_3D_FP,
	SP5K_EDID_3D_FA                        = DISP_EDID_3D_FA,
	SP5K_EDID_3D_LA                        = DISP_EDID_3D_LA,
	SP5K_EDID_3D_SBSF                      = DISP_EDID_3D_SBSF,
	SP5K_EDID_3D_LD                        = DISP_EDID_3D_LD,
	SP5K_EDID_3D_LDGGD                     = DISP_EDID_3D_LDGGD,
	SP5K_EDID_3D_TAB                       = DISP_EDID_3D_TAB,
	SP5K_EDID_3D_SBSHHSS                   = DISP_EDID_3D_SBSHHSS,
	SP5K_EDID_3D_SBSHAQSS                  = DISP_EDID_3D_SBSHAQSS,
} sp5kEdidVideoFormat_e;

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
/** @brief CEC video format data type. */
typedef UINT16 sp5kCecVideoFmt_t;

/**
 * @brief CEC recieve message callback.
 *
 * @param [in] instId   Instance ID, 0~(SP5K_CEC_INST_TOT-1).
 * @param [in] iniAddr  Initiator address, one of sp5kCecLogiAddr_e.
 * @param [in] flwAddr  Follower address, one of sp5kCecLogiAddr_e.
 * @param [in] opCode   Operation code, one of sp5kCecMsgId_e.
 * @param [in] oprNum   Operand number, 0~SP5K_CEC_OPERAND_TBL_SIZE, or -1 for <Polling Message>.
 * @param [in] poprTbl  Operand table, maximum size is SP5K_CEC_OPERAND_TBL_SIZE.
 */
typedef void (*sp5kCecRecieveMsgCallback_t)(UINT32 instId, UINT32 iniAddr, UINT32 flwAddr,
                                            UINT32 opCode, SINT32 oprNum, UINT8 *poprTbl);

/** @brief CEC instance configuration. */
typedef struct _sp5kCecInstCfg_t {
	UINT32 ddcChnlId;                           /**< DDC channel ID, 0~(SP5K_DDC_CHNL_TOT-1). */
	UINT32 cecChnlId;                           /**< CEC channel ID, 0~(SP5K_CEC_CHNL_TOT-1). */
	sp5kCecRecieveMsgCallback_t rxMsgCallback;  /**< Recieve message callback. Set to NULL if using host message. */
} sp5kCecInstCfg_t;

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
/** @brief Clear variable in data type of sp5kCecInstCfg_t. */
#define SP5K_CEC_INST_CFG_CLEAR(pcfg)  CFG_CLEAR(pcfg, sizeof(sp5kCecInstCfg_t))

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* cec.c */
UINT32 sp5kCecControl(UINT32 instId, UINT32 ctrlId, ...);

#endif  /* _SP5K_CEC_API_H_ */

