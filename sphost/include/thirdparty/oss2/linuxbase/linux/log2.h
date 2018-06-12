/* Integer base 2 logarithm calculation
 *
 * Copyright (C) 2006 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _LINUX_LOG2_H
#define _LINUX_LOG2_H

#ifdef __cplusplus
extern "C" {
#endif

static inline __attribute__((const))
bool is_power_of_2(unsigned long n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}

/* low efficiency implemention */
extern unsigned long __roundup_pow_of_two(unsigned long n);
#define roundup_pow_of_two __roundup_pow_of_two

#ifdef __cplusplus
}
#endif

#endif
