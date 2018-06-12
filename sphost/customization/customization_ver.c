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
#include <string.h>
#include "common.h"
#include "stdio.h"
#include "sp5k_ver.h"

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
static UINT8 verStr[] = "$unp_1.00.33_app_customization";
static UINT8 buildNoStr[] = "0000";
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
/* -- External function -- */

/* -- External variable -- */

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                                                                        *
 *  Function Name: customizationLibVersionGet                             *
 *                                                                        *
 *  Purposes: Return customization version                                *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *      pbuf: returns version string                                      *
 *                                                                        *
 *  Returns: SUCCESS                                                      *
 *           Error                                                        *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
customizationLibVersionGet(
	UINT8 *pbuf
)
{
	memcpy(pbuf, verStr + 5, sizeof(verStr) - 5);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: customizationBuildNumberGet                            *
 *                                                                        *
 *  Purposes: Return customization build No                               *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *      pbuf: returns build no string                                     *
 *                                                                        *
 *  Returns: void                                                         *
 *                                                                        *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
customizationBuildNumberGet(
	UINT8 *pbuf
)
{
	memcpy(pbuf, buildNoStr, sizeof(buildNoStr));
}

