/**************************************************************************
 *
 *         Copyright (c) 2005-2014 by iCatch Technology Co., Ltd.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology Co., Ltd. All rights are reserved by iCatch Technology
 *  Co., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology Co., Ltd.
 *
 *  iCatch Technology Co., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  iCatch Technology Co., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 *  Author: Anmin Deng
 *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 20474 $
$Date:: 2015-02-16 11:16:00#$
 */
#ifndef _OS_DEF_H_
#define _OS_DEF_H_

#ifdef   __cplusplus
extern   "C" {
#endif

#include "common.h"

#if !defined(__linux__) || defined(__KERNEL__)
/* --------- mips24k core DSP option ------------ */
/** \brief
 * as this OS design, mips24k core DSP accelerators and status register are
 * only available to threads.
 * any DSP instruction using accelerator or status register is not allowed
 * in ISR or its callback.
 * any thread wants to use DSP accelerator or status register must
 * explicitly calls "tx_thread_dsp_get();"
 * and after using DSP, it must call "tx_thread_dsp_put();"
 * UNLESS it is guaranteed that there is no other thread will ever use
 * DSP accelerator or status register at the same time.
 */
/**
 * \brief tx_thread_dsp_get();
 * a thread is to use mips24k core DSP resource: accelerator or status register
 * \return TX_SUCCESS(0): ok;
 * \return TX_CALLER_ERROR(0x13): not called in a thread;
 */
UINT32 tx_thread_dsp_get(void);

/**
 * \brief tx_thread_dsp_put();
 * a thread is no longer to use mips24k core DSP resource
 * \return TX_SUCCESS(0): ok;
 * \return TX_CALLER_ERROR(0x13): not called in a thread;
 * \return TX_NOT_OWNED(0x1e): called in a thread not previously called
 * tx_thread_dsp_get();
 */
UINT32 tx_thread_dsp_put(void);


/* -------- hibernate callbacks -------- */
#define HIBERNATE_CB_ID_INVALID   0
#define HIBERNATE_VCORE_FUN \
	__attribute__((section(".text.hibernate.vcore"),__noinline__))

/**
 * \param pri, the hibernate callback prioriy,
 * the lower value has higher priority,
 * the higher priority is called *LATER* at entering hibernate,
 * and caller *EARLIER* at wakeup.
 * \param fp_enter, fp_wakeup, the callbacks provided by the caller.
 * the hibernate, wakeup callbacks.
 * \note fp_enter callback returns SUCCESS, o/w failed.
 * \return HIBERNATE_CB_ID_INVALID: callback register failed;
 * o/w the callback id used to pass to hibernate_cb_unreg if needed.
 */
UINT32 hibernate_cb_reg(UINT32 pri,
		UINT32 (*fp_enter)(void), void (*fp_wakeup)(void));

/**
 * \param id, the return value of the corresponding "hibernate_cb_reg".
 * \return SUCCESS; o/w.
 */
UINT32 hibernate_cb_unreg(UINT32 id);
#endif /* #if !defined(__linux__) || defined(__KERNEL__) */

#ifdef   __cplusplus
}
#endif

#endif /* _OS_DEF_H_ */
