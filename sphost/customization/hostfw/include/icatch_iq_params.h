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
 *  Author: Edward Yang                                                   *
 *                                                                         *
 **************************************************************************/
#ifndef _ICATCH_IQ_H
#define _ICATCH_IQ_H

#define IQ_FLOW_VERSION "00.01.06"  /*__DATE__ "-" __TIME__*/

typedef enum {
    OPSTATE_PV_INIT,
    OPSTATE_PV,
    OPSTATE_SNAP,
    OPSTATE_VIDEO,
    OPSTATE_END
} IQ_STATE_t;

typedef enum {
    ID_ISO50,
    ID_ISO100,
    ID_ISO200,
    ID_ISO400,
    ID_ISO800,
    ID_ISO1600,
    ID_ISO3200,
    //ID_ISO6400,
    ID_ISO_MAX
} ISO_t;

#endif /* ICATCH_IQ_H */

