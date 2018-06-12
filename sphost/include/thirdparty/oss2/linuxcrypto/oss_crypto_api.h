/**************************************************************************
 *
 *       Copyright (c) 2009-2017 by iCatch Technology, Inc.
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
 **************************************************************************/
#ifndef __OSS_CRYPTO_API_H__
#define __OSS_CRYPTO_API_H__

#include <linux/types.h>

struct OSSCryptoInst;
typedef struct OSSCryptoInst *OSSCryptoHandle;

int oss_crypto_global_init(const char *options);

/* block cipher functions */
BOOL oss_crypto_blkcipher_has_alg(const char *algname);

int oss_crypto_blkcipher_add_alg(const char *algname);

int oss_crypto_blkcipher_alloc(
	OSSCryptoHandle *h,
	const char *algname);

void oss_crypto_blkcipher_free(OSSCryptoHandle h);

/* encryption & decription */
int oss_crypto_setkey(
	OSSCryptoHandle h,
	const u8* key,
	unsigned int len);

int oss_crypto_setiv(
	OSSCryptoHandle h,
	const u8 *iv,
	unsigned int len);

int oss_crypto_encrypt(
	OSSCryptoHandle h,
	const u8 *plaintext,
	unsigned int textlen,
	void (*pf_output)(const u8* ciphertext, unsigned int textlen, void *udata),
	void *udata);

int oss_crypto_decrypt(
	OSSCryptoHandle h,
	const u8 *ciphertext,
	unsigned int textlen,
	void (*pf_output)(const u8* plaintext, unsigned int textlen, void *udata),
	void *udata);

#endif /* __OSS_CRYPTO_API_H__ */

