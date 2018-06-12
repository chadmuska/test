/**************************************************************************
 *
 *       Copyright (c) 2006-2018 by iCatch Technology, Inc.
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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app_General.h"
#include "customization/dev_init.h"

#define TG_CMD_SAVE

UINT32 sensorCmdportIdGet(UINT32);
UINT32 sensorSerialAsyncWrite(UINT32, UINT32, const UINT8 *);

// tg cmd

void tgCmdDecode(UINT8 *regdata, UINT32 *paddr, UINT32 *pdata)
{
#if SP5K_SENSOR_AFE_AD9000==1 // ad9000=4 byte tg cmd
	if (paddr) *paddr = ((UINT32)regdata[1]<<8) + regdata[0];
	if (pdata) *pdata = ((UINT32)regdata[3]<<8) + regdata[2];
#else // 5 byte tg cmd
	//UINT32 data;
	if (paddr) 
		*paddr = ((((UINT32)regdata[1]&0xf))<<8) + regdata[0];
	if (pdata) {
		UINT32 data;
		data = regdata[4];
		data = (data<<8) + regdata[3];
		data = (data<<8) + regdata[2];\
		data = (data<<4) + (regdata[1]>>4);\
		*pdata = data;
	}
#endif
}

void tgCmdMake(UINT32 addr, UINT32 data, UINT8 *regdata)
{
#if SP5K_SENSOR_AFE_AD9000==1 // ad9000=4 byte tg cmd
	regdata[0] = addr & 0xff;
	regdata[1] = (addr>>8) & 0xff;
	regdata[2] = data & 0xff;
	regdata[3] = (data>>8) & 0xff;
#else // 5 byte tg cmd
	regdata[0] = addr & 0xff;
	regdata[1] = ((addr>>8)&0xf) + ((data<<4)&0xf0);
	regdata[2] = (data>>4) & 0xff;
	regdata[3] = (data>>12) & 0xff;
	regdata[4] = (data>>20) & 0xff;
#endif
}

typedef struct {
	UINT8 reg[5];
	UINT8 used;
} tgcmd_t;

#define TGCMD_MAX	10
tgcmd_t tgCmdView[TGCMD_MAX],tgCmdVideo[TGCMD_MAX],tgCmdSnap[TGCMD_MAX];

// treat all other modes as snap mode
static tgcmd_t *tgCmdBufGet(UINT32 sp5kmode)
{
	return sp5kmode==SP5K_MODE_STILL_PREVIEW ? tgCmdView :
		   sp5kmode==SP5K_MODE_VIDEO_PREVIEW ? tgCmdVideo : tgCmdSnap;
}

char *tgCmdModeStrGet(UINT32 sp5kmode)
{
	return sp5kmode==SP5K_MODE_STILL_PREVIEW ? "view" :
		   sp5kmode==SP5K_MODE_VIDEO_PREVIEW ? "video" : "snap";
}

void tgCmdUpdate(UINT32 sp5kmode, UINT8 *reg)
{
	int i;
	tgcmd_t *tbl=tgCmdBufGet(sp5kmode);
	UINT32 addr1,addr2;
	if (tbl==NULL) return;
	for (i=0 ; i<TGCMD_MAX ; i++) {
		if (tbl[i].used) {
			tgCmdDecode(reg, &addr1, NULL);
			tgCmdDecode(tbl[i].reg, &addr2, NULL);
			if (addr1==addr2)
				break;
		} else {
			break;
		}
	}
	if (i<TGCMD_MAX) {
		tbl[i].used = 1;
		memcpy(tbl[i].reg, reg, 5);
	}
}

void tgCmdDump(UINT32 sp5kmode)
{
	int i;
	UINT32 addr, data;
	tgcmd_t *tbl=tgCmdBufGet(sp5kmode);
	if (tbl==NULL) return;
	printf("tg for %s mode:\n", tgCmdModeStrGet(sp5kmode));
	for (i=0 ; i<TGCMD_MAX ; i++) {
		tgCmdDecode(tbl[i].reg, &addr, &data);
		printf("%2d(%d) %x %x\n", i, tbl[i].used, addr, data);
	}
}

// common tg cmd
void tgCmdClear(UINT32 sp5kmode)
{
	//tgcmd_t *tbl;
	if (sp5kmode==0) { /* clear all */
		memset(tgCmdView, 0, sizeof(tgcmd_t)*TGCMD_MAX);
		memset(tgCmdVideo, 0, sizeof(tgcmd_t)*TGCMD_MAX);
		memset(tgCmdSnap, 0, sizeof(tgcmd_t)*TGCMD_MAX);
	} else {
		tgcmd_t *tbl;
		tbl = tgCmdBufGet(sp5kmode);
		if (tbl==NULL) return;
		memset(tbl, 0, sizeof(tgcmd_t)*TGCMD_MAX);
	}
}

void tgCmdExec(UINT32 sp5kmode)
{
	UINT32 i,id;
	tgcmd_t *tbl=tgCmdBufGet(sp5kmode);
	if (tbl==NULL) return;
	for (i=0 ; i<TGCMD_MAX ; i++) {
		if (tbl[i].used) {
			id = sensorCmdportIdGet(0); // XTG
			sensorSerialAsyncWrite(id, 1, tbl[i].reg);
		}
	}
}

void tgCmdGet(UINT8 *buf)
{
#ifdef TG_CMD_SAVE
	memcpy(buf, tgCmdView, sizeof(tgCmdView));
	memcpy(buf+60, tgCmdVideo, sizeof(tgCmdView));
	memcpy(buf+120, tgCmdSnap, sizeof(tgCmdView));
#else
	memset(buf, 0, 3*sizeof(tgCmdView));
#endif
}

void tgCmdSet(UINT8 *buf)
{
#ifdef TG_CMD_SAVE
	memcpy(tgCmdView, buf, sizeof(tgCmdView));
	memcpy(tgCmdVideo, buf+60, sizeof(tgCmdView));
	memcpy(tgCmdSnap, buf+120, sizeof(tgCmdView));
#else
	memset(tgCmdView, 0, sizeof(tgCmdView));
	memset(tgCmdVideo, 0, sizeof(tgCmdView));
	memset(tgCmdSnap, 0, sizeof(tgCmdView));
#endif
}

