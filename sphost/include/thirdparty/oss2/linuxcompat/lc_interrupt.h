/**************************************************************************
 *
 *       Copyright (c) 2013-2013 by iCatch Technology, Inc.
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
 *  Hsin-Chu, Taiwan.
 *
 **************************************************************************/
/**
 * @file lc_interrupt.h
 * @brief
 * @author LiJun
 *
 * The detail information about this file.
 */

#ifndef _LINUXCOMPAT_INTERRUPT_H_
#define _LINUXCOMPAT_INTERRUPT_H_

#include "common.h"
/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
/**
 * enum irqreturn
 * @IRQ_NONE		interrupt was not from this device
 * @IRQ_HANDLED		interrupt was handled by this device
 * @IRQ_WAKE_THREAD	handler requests to wake the handler thread
 */
enum irqreturn {
	IRQ_NONE		= (0 << 0),
	IRQ_HANDLED		= (1 << 0),
	IRQ_WAKE_THREAD		= (1 << 1),
};

typedef enum irqreturn irqreturn_t;
#define IRQ_RETVAL(x)	((x) ? IRQ_HANDLED : IRQ_NONE)

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
struct pt_regs {};
typedef int(*isr_func_t)(int, void *, struct pt_regs *);
typedef void (*irqIsr_t)(void);

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
extern unsigned long __local_irq_save(void);
extern void __local_irq_restore(unsigned long x);
extern void local_irq_enable(void);
extern void local_irq_disable(void);

#define local_irq_save(x)		((x) = __local_irq_save())
#define local_irq_restore(x)	(__local_irq_restore(x))

extern int request_irq(int irq, isr_func_t handler, unsigned long irqflags,
					const char *devname, void *dev_id);
extern void free_irq(int irq, void *dev_id);

extern void halUsbhostIrqReg(void (*isr)(void));
extern void halUsbhostIrqUnreg(void);
extern void irqIsrReg(UINT32, irqIsr_t);
extern void irqIsrUnreg(UINT32);
extern unsigned long __ros_local_irq_save(void);
extern void __ros_local_irq_restore(unsigned long x);

extern BOOL oss_in_interrupt();

#endif /* _LINUXCOMPAT_INTERRUPT_H_ */
