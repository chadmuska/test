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
#define HOST_DBG 0
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "app_key_def.h"
#include "app_util.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/* todo: how to make sure all key msg is included */
/* crosscheck app_btn_def.h */
/* to add an entry, add name & key press id (if available) */
struct {
	char *name;
	UINT32 key;
} KeyTable[]={
	{ "s1",		APP_KEY_PRESS_S1 },
	{ "s2",		APP_KEY_PRESS_S2 },
	{ "wide",	APP_KEY_PRESS_WIDE },
	{ "tele",	APP_KEY_PRESS_TELE },
	{ "up",		APP_KEY_PRESS_UP },
	{ "down",	APP_KEY_PRESS_DOWN },
	{ "left",	APP_KEY_PRESS_LEFT },
	{ "right",	APP_KEY_PRESS_RIGHT },
	{ "menu",	APP_KEY_PRESS_MENU },
	{ "del",	APP_KEY_PRESS_DEL },
	{ "set",	APP_KEY_PRESS_SET },
	{ "mode",	APP_KEY_PRESS_MODE },
};

#define KEY_MAX		COUNT_OF(KeyTable)

UINT32
appKeyLookup(
	char * keyname
)
{
	int i;
	if (keyname==NULL) 
	{
		return 0xffffffff;
	}
	//printf("%s\n", keyname);
	for (i=0 ; i<KEY_MAX ; i++) 
	{
		if (strcmp(keyname, KeyTable[i].name)==0)
		{
			return KeyTable[i].key;
	}
	}
	return 0xffffffff;
}

#if 0
char *
appKeyNameGet(
	UINT16 key
)
{
	int i;
	key = KEY_CODE(key);
	for (i=0 ; i<KEY_MAX ; i++) {
		if (KEY_CODE(KeyTable[i].key)==key)
			return KeyTable[i].name;
	}
	return NULL;
}
#endif

