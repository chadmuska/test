/**************************************************************************
 *
 *       Copyright (c) 2012-2018 by iCatch Technology, Inc.
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

#ifndef APP_AE_PRIVATE_H
#define APP_AE_PRIVATE_H

/**************************************************************************
 * Debug Control                                                          *
 **************************************************************************/
#define AE_DBG_MSG      0
#define AE_OSD_DBG_MSG  0

#if AE_DBG_MSG
#define printf_t printf
#else
__inline__ static void printf_t(char *pstr, ...) {}
#endif

/**************************************************************************
 * Platform AE Configuration                                              *
 **************************************************************************/



/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

#endif  /* APP_AE_PRIVATE_H */
