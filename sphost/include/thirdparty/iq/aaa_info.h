
#ifndef AAA_DEBUGH
#define AAA_DEBUGH

#if defined(_WIN_)
    #include "x5330.h"
#else
    #include "common.h"
#endif

typedef struct tagAaaDebug {
	UINT32 flag;
	UINT32 size;
} aaaDebug_t;

typedef enum tagAaaDebugFlag {
	AAA_INFO_NA			= 0x0,
	
	AAA_INFO_AWB_BIN	= 0xBB,
	AAA_INFO_AWB_TEXT	= AAA_INFO_AWB_BIN+0x10,
	
	AAA_INFO_AE_BIN		= 0xBE,
	AAA_INFO_AE_TEXT	= AAA_INFO_AE_BIN+0x10,
	
	AAA_INFO_AF_BIN		= 0xBF,
	AAA_INFO_AF_TEXT	= AAA_INFO_AF_BIN+0x10,

	AAA_INFO_MASK		= 0xFF,
} aaaDebugFlag_e;

typedef enum tagAaaInfoParam {
	AAA_INFO_PARAM_ENABLED = 0x0,
	AAA_INFO_PARAM_NEWFILE,
	AAA_INFO_PARAM_FRAMENO = 0x1234,
	
	AAA_INFO_PARAM_MAX
} AaaInfoParam_e;

/**************************************************************************
 * to instruct debug operation
 * selector: 
 * val:
 **************************************************************************/
UINT32 aaaInfoParamSet( UINT32 selector /*AaaInfoParam_e*/, UINT32 value );

/**************************************************************************
 * to append data to 3A debug buffer,
 * pDat: data pointer, prefiex with aaaDebug_t
 * WrSize: data write length
 * Type: aaaDebugFlag_e
 **************************************************************************/
UINT32 aaaInfoWrite( UINT8 * pDat, UINT32 nSize, UINT32 Type /*aaaDebugFlag_e*/ );

/**************************************************************************
 * to flush buffer to file
 **************************************************************************/
UINT32 aaaInfoFlush( void );

#endif

