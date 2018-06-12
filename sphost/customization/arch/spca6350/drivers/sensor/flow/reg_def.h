/**************************************************************************
 *
 *       Copyright (c) 2007-2018 by iCatch Technology, Inc.
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
 *  Author: Anmin Deng
 *
 **************************************************************************/
#ifndef _REG_DEF_H_
#define _REG_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


#define LOGI_ADDR_REG  ((void *)0xb0000000)

#define LOGI_REG_DECLARE(reg_t,preg) reg_t *preg = (reg_t *)LOGI_ADDR_REG
#define LOGI_ADDR_REG_OFFSET(reg_t,reg) ((void *)&((reg_t *)0)->reg - (void *)0)

#ifdef __cplusplus
}
#endif

#endif /* _REG_DEF_H_ */
