#ifndef _ASM_BYTEORDER_H
#define _ASM_BYTEORDER_H

#include <asm/types.h>

#ifndef __LITTLE_ENDIAN_BITFIELD
#define __LITTLE_ENDIAN_BITFIELD
#endif

#define le16_to_cpu(x)	((u16)(x))
#define le16_to_cpup(x)	(*((u16 *)(x)))
#define le16_to_cpus(x) do {} while (0)
#define le32_to_cpu(x)	((u32)(x))
#define le32_to_cpup(x)	(*((u32 *)(x)))
#define le64_to_cpu(x) ((u64)(u64)(x))

#define cpu_to_le16(x)	((u16)(x))
#define cpu_to_le16p(x)	(*((u16 *)(x)))
#define cpu_to_le16s(x) do {} while (0)

#define cpu_to_le32(x)	((u32)(x))
#define cpu_to_le32p(x)	(*((u32 *)(x)))
#define cpu_to_le64(x) ((u64)(u64)(x))

static inline const u16 get_unaligned_le16(const void* x)
{
	u16 x16;
	((u8*)&x16)[0] = ((u8*)x)[0];
	((u8*)&x16)[1] = ((u8*)x)[1];
	return x16;
}

static inline const u32 get_unaligned_le32(const void* x)
{
	u32 x32;
	((u8*)&x32)[0] = ((u8*)x)[0];
	((u8*)&x32)[1] = ((u8*)x)[1];
	((u8*)&x32)[2] = ((u8*)x)[2];
	((u8*)&x32)[3] = ((u8*)x)[3];
	return x32;
}

static inline void put_unaligned_le16(u16 val, void *p)
{
	((u8*)p)[0] = ((u8*)&val)[0];
	((u8*)p)[1] = ((u8*)&val)[1];
}

static inline void put_unaligned_le32(u32 val, void *p)
{
	((u8*)p)[0] = ((u8*)&val)[0];
	((u8*)p)[1] = ((u8*)&val)[1];
	((u8*)p)[2] = ((u8*)&val)[2];
	((u8*)p)[3] = ((u8*)&val)[3];
}

#ifdef SPCA6350
static inline u16 SWAP16(u16 n)
{
	__asm__(
		" wsbh %0, %1" : "=r" (n) : "r" (n));
	return n;
}

static inline u32 SWAP32(u32 n)
{
	__asm__(
		" wsbh %0, %1 \n"
		" rotr %0, %0, 16 \n"
		: "=r" (n)
		: "r" (n));
	return n;
}
#else
#define SWAP16(_x_)                                        \
	({ u16 _x = (_x_); (u16)((_x << 8) | (_x >> 8)); })

#define SWAP32(_x_)                        \
	({ u32 _x = (_x_);                   \
	   ((_x << 24) |                            \
	   ((0x0000FF00UL & _x) <<  8) |            \
	   ((0x00FF0000UL & _x) >>  8) |            \
	   (_x  >> 24)); })
#endif

#define be16_to_cpu(x)	SWAP16(le16_to_cpu(x))
#define be16_to_cpup(x)	SWAP16(le16_to_cpup(x))
#define be32_to_cpu(x)	SWAP32(le32_to_cpu(x))

#define cpu_to_be16(x)	SWAP16(cpu_to_le16(x))
#define cpu_to_be16p(x)	SWAP16(cpu_to_le16p(x))
#define cpu_to_be32(x)	SWAP32(cpu_to_le32(x))
#define cpu_to_be32p(x)	SWAP32(cpu_to_le32p(x))

#define __constant_cpu_to_le64(x) ((__u64)(x))
#define __constant_le64_to_cpu(x) ((__u64)(x))
#define __constant_cpu_to_le32(x) ((__u32)(x))
#define __constant_le32_to_cpu(x) ((__u32)(x))
#define __constant_cpu_to_le16(x) ((__u16)(x))
#define __constant_le16_to_cpu(x) ((__u16)(x))


#endif /*_ASM_BYTEORDER_H */
