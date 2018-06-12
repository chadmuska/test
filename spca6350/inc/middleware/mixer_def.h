/*******************************************************************************
*
*       Copyright (c) 2010-2011 by iCatch Technology, Inc.
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
*  Author: Matt Wang
*
*******************************************************************************/
#ifndef _MIXER_DEF_H_
#define _MIXER_DEF_H_

/*******************************************************************************
*                              D A T A   T Y P E S
*******************************************************************************/
typedef struct _mixerPlayIni_t {
	UINT8 *pdevName;
	UINT32 devId;
	UINT32 ioId;
	UINT32 usBufLen;
	UINT32 subTrkTot;
} mixerPlayIni_t;

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
UINT32 mixerPlayOpen(mixerPlayIni_t *pini);
UINT32 mixerPlayClose(UINT32 hndl);

#endif  /* _MIXER_DEF_H_ */

