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
 * @file lc_page.h
 * @brief
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_PAGE_H_
#define _LINUXCOMPAT_PAGE_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "lc_io.h"

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define PAGE_SHIFT	12
#define PAGE_SIZE	(1<<PAGE_SHIFT)
#define PAGE_MASK	(~((1 << PAGE_SHIFT) - 1))

#define UNCAC_ADDR(addr)	_to_uncache(addr)
#define CAC_ADDR(addr)		_to_cache(addr)

#endif /* _LINUXCOMPAT_PAGE_H_ */
