/**************************************************************************
 *
 *	 Copyright (c) 2015 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
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

#ifndef __NDK_BLUETOOTH_HIDP_H__
#define __NDK_BLUETOOTH_HIDP_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *   Human Input Device
 *****************************************************************************/

/* \description     : Callback function for HIDP key trigger.
 * \param key       : key vlaue.
 * \param key_status: key status, 0=releaseed 1=pressed
 */
typedef void (*NDKBtHidpKeyTriggerCb)(unsigned char key, unsigned int key_status);

/* \description     : Callback function for HIDP Raw Input Data
 * \param key       : input buffer.
 * \param key_status: input size
 */
typedef void (*NDKBtHidpRawInputCb)(const unsigned char *input, unsigned int size, void *user_data);

/* \description       : Register event for key trigger.
 * \param hostcallback: host callback function.
 */
void ndk_bt_hidp_keyevent_register(NDKBtHidpKeyTriggerCb hostcallback);

/* \description : Enable characters print on terminal.
 * \param enable: 1=enable, 0=disable.
 */
void ndk_bt_hidp_keychar_print_en(int enable);

/* \description       : Register event for raw input trigger.
 * \param hostcallback: host callback function.
 */
void ndk_bt_hidp_raw_input_register(NDKBtHidpRawInputCb hostcallback, void *user_data);


#ifdef __cplusplus
}
#endif

#endif

