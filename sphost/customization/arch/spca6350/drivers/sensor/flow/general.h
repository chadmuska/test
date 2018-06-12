/**************************************************************************
 *
 *       Copyright (c) 2002-2018 by iCatch Technology, Inc.
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
 *                                                                        *
 *  Author: Timothy Wu                                                    *
 *                                                                        *
 **************************************************************************/
#ifndef _GENERAL_H_
#define _GENERAL_H_

#include "common.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#ifndef NULL
#define NULL               ((void *)0)
#endif

#define SUCCESS            0
#define FAIL               1

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define GNU_PACK __attribute__ ((__packed__))

#define READ8(_reg_)            (*((volatile UINT8  *)(_reg_)))
#define READ16(_reg_)           (*((volatile UINT16 *)(_reg_)))
#define READ32(_reg_)           (*((volatile UINT32 *)(_reg_)))
#define WRITE8(_reg_, _value_)  (*((volatile UINT8  *)(_reg_)) = (_value_))
#define WRITE16(_reg_, _value_) (*((volatile UINT16 *)(_reg_)) = (_value_))
#define WRITE32(_reg_, _value_) (*((volatile UINT32 *)(_reg_)) = (_value_))

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef void (*irqIsr_t)(void);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *                 C O M P I L A T I O N    O P T I O N S                 *
 **************************************************************************/


#endif /* _GENERAL_H_ */

