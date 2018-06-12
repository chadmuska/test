/*
 *	crc16.h - CRC-16 routine
 *
 * Implements the standard CRC-16:
 *   Width 16
 *   Poly  0x8005 (x^16 + x^15 + x^2 + 1)
 *   Init  0
 *
 * Copyright (c) 2005 Ben Gardner <bgardner@wabtec.com>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 2. See the file COPYING for more details.
 */

#ifndef __CRC16_H
#define __CRC16_H

#include <ndk_types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern UINT16 const crc16_table[256];

extern UINT16 crc16(UINT16 crc, const UINT8 *buffer, size_t len);

static inline UINT16 crc16_byte(UINT16 crc, const UINT8 data)
{
	return (crc >> 8) ^ crc16_table[(crc ^ data) & 0xff];
}

#ifdef __cplusplus
}
#endif

#endif /* __CRC16_H */
