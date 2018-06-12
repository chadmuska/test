/**************************************************************************
 *
 *       Copyright (c) 2006-2018 by iCatch Technology, Inc.
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
#ifndef __HCE_GENERAL_H
#define __HCE_GENERAL_H

#include "common.h"
#include "sp5k_modesw_api.h"
#include "sp5k_os_api.h"


#define ulong 		unsigned long
#define pulong 		unsigned long*

#define uchar 		unsigned char
#define puchar 		unsigned char*

#define ushort 		unsigned short
#define pushort 	unsigned short*

#define pshort 		short*

#define bool 		unsigned char
#define BOOL 		unsigned char

#define INT8     char
#define INT16    short
#define INT32    long
/*#define ULONG    unsigned long*/
#define BYTE     unsigned char
/*#define UINT     unsigned int*/

#define USHORT	unsigned short
#define PUSHORT	unsigned short*
#define PBYTE	unsigned char*


/* alexwu : For VS8320 PASM Porting Transition Phase ONLY !!! */
#define _VS8320_PASM_PORTING_		1


#ifndef SUCCESS
#define SUCCESS 	0
#endif

#ifndef FAIL
#define FAIL    	1
#endif

#define DISABLE   	0
#define ENABLE   	1

/*///////////////////////////////////////////////////////////////////////////////
// To Compatible with Coach V
///////////////////////////////////////////////////////////////////////////////*/

#define CSEG_1
#define CSEG_2
#define CSEG_3
#define CSEG_4

#define __hce(a)


#define BIT_0               0x00000001
#define BIT_1               0x00000002
#define BIT_2               0x00000004
#define BIT_3               0x00000008
#define BIT_4               0x00000010
#define BIT_5               0x00000020
#define BIT_6               0x00000040
#define BIT_7               0x00000080
#define BIT_8               0x00000100
#define BIT_9               0x00000200
#define BIT_10              0x00000400
#define BIT_11              0x00000800
#define BIT_12              0x00001000
#define BIT_13              0x00002000
#define BIT_14              0x00004000
#define BIT_15              0x00008000
#define BIT_16              0x00010000
#define BIT_17              0x00020000
#define BIT_18              0x00040000
#define BIT_19              0x00080000
#define BIT_20              0x00100000
#define BIT_21              0x00200000
#define BIT_22              0x00400000
#define BIT_23              0x00800000
#define BIT_24              0x01000000
#define BIT_25              0x02000000
#define BIT_26              0x04000000
#define BIT_27              0x08000000
#define BIT_28              0x10000000
#define BIT_29              0x20000000
#define BIT_30              0x40000000
#define BIT_31              0x80000000


/*#define BIT(a) 				(1<<(a))*/


#define MakeWord(lo,hi)				( (ushort) ( ((uchar)(lo))  | ( (ushort)((uchar) (hi)) ) << 8 ) ) 	// MAKE_WORD(lo, hi) 	//
#define MakeLong(hi,lo)				( (ulong)  ( ((ushort)(lo)) | ( (ulong) ((ushort)(hi)) ) << 16) )
#define LoWord(L)					((ushort)(L))
#define HiWord(L)					((ushort) (((ulong)(L) >> 16) & 0xFFFF))
#define LoByte(W)					((uchar)(W))
#define HiByte(W)					((uchar)  (((ushort)(W)>>8 ) & 0xFF ))

/* Swaps words and bytes.*/
#define SwapBytes(W) 				(MakeWord( LoByte(W), HiByte(W) ))

/* For example, SWAP_WORDS( 0x33221100 ) = 0x00112233*/
#define SwapWords(L) 				(MakeLong(  MakeWord( LoByte(LoWord(L)), 	\
														  HiByte(LoWord(L))),	\
												MakeWord( LoByte(HiWord(L)),  	\
														  HiByte(HiWord(L))) ))
/*   i=a~N, i++    ==>  i = (i) % (N) + a     */
#define INCREASE_OBJECT(Obj, FirstObj, LastObj)		{Obj=(Obj>=LastObj)?FirstObj:(Obj+1);}//{Obj = (Obj%LastObj) + FirstObj;}
/*   i=a~N, i--    ==>  i = (N+i-2) % (N) + a     */
#define DECREASE_OBJECT(Obj, FirstObj, LastObj)		{Obj=(Obj<=FirstObj)?LastObj:(Obj-1);}//{Obj = ((LastObj+Obj-2)%LastObj) + FirstObj;}

#define CIRCULAR_INCREASE(Obj,FirstObj,LastObj)			{Obj = ((Obj)>=(LastObj) )? FirstObj:(Obj+1);}
#define CIRCULAR_DECREASE(Obj,FirstObj,LastObj)			{Obj = ((Obj)<=(FirstObj))? LastObj :(Obj-1);}



/*  Define Base FW Function*/

/*  Define HCE Function*/

#define Sr1()						QueryStatus(1)
#define Sr2()						QueryStatus(0)


#define Abs(a)					( (a) > 0 ? (a) : -(a) )
#define Sign(a)					( (a) < 0 ? -1 : 1 )

#define Min(a, b)				( (a) < (b) ? (a) : (b) )
#define Max(a, b)				( (a) > (b) ? (a) : (b) )

#define Limit(var,min,max)      {(var) = ((var) < (min)) ? (min) : \
                                        (((var) > (max)) ? (max) : (var)); }

#define CircularLimit(var,min,max)      {(var) = ((var) < (min)) ? (max) : \
                                                 (((var) > (max)) ? (min) : (var)); }

#define COUNT_OF(array)	(sizeof(array)/sizeof(array[0]))

/***************************************************************************************/
#if 0
	#define	DBG_Printf_0(str,args...)   printf(str,##args)
	#define	DBG_Printf_1(str,args...)   printf(str,##args)
#else
	#define	DBG_Printf_0(str,args...)
	#define	DBG_Printf_1(str,args...)	//printf(str,##args)
#endif

#if 0
	#define	DBG_Printf_POWERON(str,args...)   printf(str,##args)
#else
	#define	DBG_Printf_POWERON(str,args...)
#endif

#if 0
	#define DBG_MODE_MSG_TRACE_ENABLE
#endif

#if 0
	#define DBG_HOST_MSG_WAIT
#endif


#ifdef DBG_HOST_MSG_WAIT
	#define	DBG_Printf_WaitMSG(str,args...)   printf(str,##args)
#else
	#define	DBG_Printf_WaitMSG(str,args...)
#endif
/***************************************************************************************/

extern unsigned long ulIRQMask;



ushort QueryStatus(ushort QSr1);
ushort IsCardExist(void);
UINT32 GetStorageFreeSpaceKB(void);


void Idle(void);
void View(void);
void Video_View(void);
void WaitlVD( ushort num );
/*INT32 CircularLimit( INT32 Val, INT32 Min, INT32 Max );*/

void String2Int( UINT8 *pString, UINT32 *IntValue, UINT32 Digit );
UINT8 *Int2String( UINT32 IntValue, UINT8 *pString, UINT32 BufSize );

UINT32 GetInnerDiff( UINT32 dwStartX, UINT32 dwEndX,
                     UINT32 dwStartY, UINT32 dwEndY, UINT32 dwCurX );

void WaitForSr2Bit( ushort Bit);
UINT8 GetSum( UINT8 *Buf, UINT32 Size );
UINT16 GetSum16( UINT16 *Buf, UINT32 Size );
unsigned int Num2Bit(unsigned x);
/*unsigned Bit2Num(unsigned int n);  */


bool	sp5kModeCheck(UINT32 tgtMode);	/* alexwu */

UINT32 profLogPrintf(UINT32 slot, const char *format, ...);

#if 1
extern void osAllInfoDisplay( UINT32 mode );

#define HOST_ASSERT(expr) 	\
	if (!(expr)) { \
		printf("[ERR] Assert %s %d.\n", __FUNCTION__, __LINE__); \
		osAllInfoDisplay(1);\
		while (1);\
	}

#define HOST_ASSERT_MSG(expr, msg) 	\
	if (!(expr)) { \
		printf("[ERR] Assert %s %d: %s\n", __FUNCTION__, __LINE__, msg); \
		osAllInfoDisplay(1);\
		while (1);\
	}

#else
#define HOST_ASSERT(expr)
#define HOST_ASSERT_MSG(expr, msg)
#endif

#endif

