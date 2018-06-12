#ifndef _ASM_BYTEORDER_H
#define _ASM_BYTEORDER_H

#include <asm/types.h>

#ifndef __LITTLE_ENDIAN_BITFIELD
#define __LITTLE_ENDIAN_BITFIELD
#endif

#define le16_to_cpu(x)			((u16)(x))
#define __le16_to_cpu			le16_to_cpu
#define le16_to_cpup(x)			(*((u16 *)(x)))
#define le16_to_cpus(x)			do {} while (0)

#define le32_to_cpu(x)			((u32)(x))
#define __le32_to_cpu			le32_to_cpu
#define le32_to_cpup(x)			(*((u32 *)(x)))

#define le64_to_cpu(x)			((u64)(u64)(x))
#define __le64_to_cpu			le64_to_cpu
#define le64_to_cpup(x)			(*((u64 *)(x)))

#define cpu_to_le16(x)			((u16)(x))
#define cpu_to_le32(x)			((u32)(x))

#define cpu_to_le16p(x)			(*((u16 *)(x)))
#define cpu_to_le16s(x)			do {} while (0)

#define cpu_to_le32p(x)			(*((u32 *)(x)))
#define cpu_to_le64(x)			((u64)(u64)(x))

#define be16_to_cpu(x)			SWAP16(le16_to_cpu(x))
#define be16_to_cpup(x)			SWAP16(le16_to_cpup(x))
#define be32_to_cpu(x)			SWAP32(le32_to_cpu(x))
#define be32_to_cpup(x)			SWAP32(le32_to_cpup(x))

#define cpu_to_be16(x)			SWAP16(cpu_to_le16(x))
#define cpu_to_be16p(x)			SWAP16(cpu_to_le16p(x))
#define cpu_to_be32(x)			SWAP32(cpu_to_le32(x))
#define cpu_to_be32p(x)			SWAP32(cpu_to_le32p(x))

#define cpu_to_be64(x)			SWAP64(x)
#define be64_to_cpu(x)			SWAP64(x)

#define get_unaligned_16(p)		get_unaligned_le16(p)
#define get_unaligned_32(p)		get_unaligned_le32(p)
#define put_unaligned_16(v, p)		put_unaligned_le16(v, p)
#define put_unaligned_32(v, p)		put_unaligned_le32(v, p)
#define put_unaligned(n, v, p)		put_unaligned_le##n(v, p)
#define get_unaligned(n, p)		get_unaligned_le##n(p)

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

static inline const u32 get_unaligned_le64(const void* x)
{
	u64 x64;
	((u8*)&x64)[0] = ((u8*)x)[0];
	((u8*)&x64)[1] = ((u8*)x)[1];
	((u8*)&x64)[2] = ((u8*)x)[2];
	((u8*)&x64)[3] = ((u8*)x)[3];
	((u8*)&x64)[4] = ((u8*)x)[4];
	((u8*)&x64)[5] = ((u8*)x)[5];
	((u8*)&x64)[6] = ((u8*)x)[6];
	((u8*)&x64)[7] = ((u8*)x)[7];
	return x64;
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

static inline void put_unaligned_le64(u64 val, void *p)
{
	((u8*)p)[0] = ((u8*)&val)[0];
	((u8*)p)[1] = ((u8*)&val)[1];
	((u8*)p)[2] = ((u8*)&val)[2];
	((u8*)p)[3] = ((u8*)&val)[3];
	((u8*)p)[4] = ((u8*)&val)[4];
	((u8*)p)[5] = ((u8*)&val)[5];
	((u8*)p)[6] = ((u8*)&val)[6];
	((u8*)p)[7] = ((u8*)&val)[7];
}

static inline void put_unaligned_be16(u32 val, void *p)
{
	((u8*)p)[0] = ((u8*)&val)[1];
	((u8*)p)[1] = ((u8*)&val)[0];
}

static inline void put_unaligned_be32(u32 val, void *p)
{
	((u8*)p)[0] = ((u8*)&val)[3];
	((u8*)p)[1] = ((u8*)&val)[2];
	((u8*)p)[2] = ((u8*)&val)[1];
	((u8*)p)[3] = ((u8*)&val)[0];
}

static inline u16 get_unaligned_be16(const void *p) {
	u16 v;
	((u8*)&v)[0] = ((const u8*)p)[1];
	((u8*)&v)[1] = ((const u8*)p)[0];
	return v;
}

static inline u16 get_unaligned_be32(const void *p) {
	u32 v;
	((u8*)&v)[0] = ((const u8*)p)[3];
	((u8*)&v)[1] = ((const u8*)p)[2];
	((u8*)&v)[2] = ((const u8*)p)[1];
	((u8*)&v)[3] = ((const u8*)p)[0];
	return v;
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

static inline u64 SWAP64(u64 x) {
	return ((x & (u64)0x00000000000000ffULL) << 56) | \
	       ((x & (u64)0x000000000000ff00ULL) << 40) | \
	       ((x & (u64)0x0000000000ff0000ULL) << 24) | \
	       ((x & (u64)0x00000000ff000000ULL) <<  8) | \
	       ((x & (u64)0x000000ff00000000ULL) >>  8) | \
	       ((x & (u64)0x0000ff0000000000ULL) >> 24) | \
	       ((x & (u64)0x00ff000000000000ULL) >> 40) | \
	       ((x & (u64)0xff00000000000000ULL) >> 56);
}

#define __constant_cpu_to_le64(x) ((__u64)(x))
#define __constant_le64_to_cpu(x) ((__u64)(x))
#define __constant_cpu_to_le32(x) ((__u32)(x))
#define __constant_le32_to_cpu(x) ((__u32)(x))
#define __constant_cpu_to_le16(x) ((__u16)(x))
#define __constant_le16_to_cpu(x) ((__u16)(x))

#define ___constant_swab16(x) ((__u16)(				\
	(((__u16)(x) & (__u16)0x00ffU) << 8) |			\
	(((__u16)(x) & (__u16)0xff00U) >> 8)))

#define ___constant_swab32(x) ((__u32)(				\
	(((__u32)(x) & (__u32)0x000000ffUL) << 24) |		\
	(((__u32)(x) & (__u32)0x0000ff00UL) <<  8) |		\
	(((__u32)(x) & (__u32)0x00ff0000UL) >>  8) |		\
	(((__u32)(x) & (__u32)0xff000000UL) >> 24)))

#define ___constant_swab64(x) ((__u64)(				\
	(((__u64)(x) & (__u64)0x00000000000000ffULL) << 56) |	\
	(((__u64)(x) & (__u64)0x000000000000ff00ULL) << 40) |	\
	(((__u64)(x) & (__u64)0x0000000000ff0000ULL) << 24) |	\
	(((__u64)(x) & (__u64)0x00000000ff000000ULL) <<  8) |	\
	(((__u64)(x) & (__u64)0x000000ff00000000ULL) >>  8) |	\
	(((__u64)(x) & (__u64)0x0000ff0000000000ULL) >> 24) |	\
	(((__u64)(x) & (__u64)0x00ff000000000000ULL) >> 40) |	\
	(((__u64)(x) & (__u64)0xff00000000000000ULL) >> 56)))

#define __constant_cpu_to_be64(x) ((__force __be64)___constant_swab64((x)))
#define __constant_be64_to_cpu(x) ___constant_swab64((__force __u64)(__be64)(x))
#define __constant_cpu_to_be32(x) ((__force __be32)___constant_swab32((x)))
#define __constant_be32_to_cpu(x) ___constant_swab32((__force __u32)(__be32)(x))
#define __constant_cpu_to_be16(x) ((__force __be16)___constant_swab16((x)))
#define __constant_be16_to_cpu(x) ___constant_swab16((__force __u16)(__be16)(x))

#endif /*_ASM_BYTEORDER_H */
