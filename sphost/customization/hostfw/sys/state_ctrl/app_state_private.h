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
#ifndef _APP_STATE_PRIVATE_H_
#define _APP_STATE_PRIVATE_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "common.h"
#include "app_state_ctrl.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


/* state transition phase definition */
typedef enum {
	APP_STATE_PHASE_INIT   = 0x0,    /* state is in initialization phase */
	APP_STATE_PHASE_READY,           /* state is in working phase : actvie */
	APP_STATE_PHASE_CLOSE,     	 /* state is in close phase */
	APP_STATE_PHASE_NUM
} appStatePhase_e;
                                  
typedef struct appStateInfo_s {
	appState_e  prevState;
	appState_e  activeState;
	appState_e  nextState;	
	appStatePhase_e  statePhase;	
	UINT32 stateMsgParam;
} appStateInfo_t;

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appStateCtrlInitial (void);

#endif  /* _APP_STATE_PRIVATE_H_ */

