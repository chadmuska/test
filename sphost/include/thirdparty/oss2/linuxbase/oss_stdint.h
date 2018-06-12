#ifndef __OSS_STDINT_H__
#define __OSS_STDINT_H__

/* Aovid conflicts with stdint.h */
typedef u8_t			uint8_t;
typedef u16_t			uint16_t;
typedef u32_t			uint32_t;
typedef u64_t			uint64_t;
typedef s8_t 			int8_t;
typedef s16_t			int16_t;
typedef s32_t			int32_t;
typedef s64_t			int64_t;

#define UINT64_MAX		((UINT64)~0ULL)

#define INT64_MAX		((SINT64)(UINT64_MAX>>1))
#define INT64_MIN		((SINT64)(-INT64_MAX - 1))

#define UINT32_MAX		((UINT32)~0U)
#define INT32_MAX		((SINT32)(UINT32_MAX>>1))
#define INT32_MIN		((SINT32)(-INT32_MAX - 1))

#define UINT16_MAX		((UINT16)~0U)
#define INT16_MAX		((SINT16)(UINT16_MAX>>1))
#define INT16_MIN		((SINT16)(-INT16_MAX - 1))

#define UINT8_MAX		((UINT8)~0U)
#define INT8_MAX		((SINT8)(UINT8_MAX>>1))
#define INT8_MIN		((SINT8)(-INT8_MAX - 1))

#endif
