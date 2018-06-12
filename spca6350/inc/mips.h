/**************************************************************************
 *
 *       Copyright (c) 2002-2016 by iCatch Technology, Inc.
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
 *  Author: Timothy Wu                                                    *
 *                                                                        *
 **************************************************************************/
#ifndef _MIPS_H_
#define _MIPS_H_

#ifdef  __cplusplus 
extern "C" {
#endif

/*
 *  Aliases for general purpose registers.
 *  Using the naming convention of MIPS.
 */
#define zero            $0      /* constant zero                       */
#define AT              $1      /* assembler temporaries               */
#define v0              $2      /* function return, value holders      */
#define v1              $3
#define a0              $4      /* incoming arguments                  */
#define a1              $5
#define a2              $6
#define a3              $7
#define t0              $8      /* temporaries, not saved across calls */
#define t1              $9
#define t2              $10
#define t3              $11
#define t4              $12
#define t5              $13
#define t6              $14
#define t7              $15
#define s0              $16     /* temporaries, saved across calls     */
#define s1              $17
#define s2              $18
#define s3              $19
#define s4              $20
#define s5              $21
#define s6              $22
#define s7              $23
#define t8              $24     /* temporaries                         */
#define t9              $25
#define k0              $26     /* exception handling, kernel reg      */
#define k1              $27
#define gp              $28     /* global data pointer                 */
#define sp              $29     /* stack pointer                       */
#define s8              $30     /* saved temporary                     */
#define fp              $30     /* frame pointer                       */
#define ra              $31     /* return address                      */

/*
 *  System Control Coprocessor (CP0) registers
 */
#define CO_BADVA        $8      /* Bad Virtual Address                 */
#define C0_SR           $12     /* Processor Status                    */
#define C0_CAUSE        $13     /* Exception Cause                     */
#define C0_EPC          $14     /* Exception PC                        */
#define C0_PRID         $15     /* Processor Rev Indentifier           */

/*
 *  Floating-Point Control registers
 */
#define FPA_CSR         31      /* Control/Status register             */
#define FPA_IRR         0       /* Implementation/Rev register         */
#define C1_CSR          31
#define C1_FRID         0

/*
 *  MIPS family dependent definitions
 */
#ifndef  _mips3_
	/* Assume R3000 */
#	define GPR_SIZE		4		/* Size of general purpose register    */
#	define LOAD_REG		lw
#	define SAVE_REG		sw
#else
	/* Assume R4000 */
#	define GPR_SIZE		8
#	define LOAD_REG		ld
#	define SAVE_REG		sd
#endif

/*
 *  Stack address for interrupt or context switch.
 */
#define STACK_EPC		(GPR_SIZE *  1)	
#define STACK_CAUSE 	(GPR_SIZE *  2)	
#define STACK_STATUS	(GPR_SIZE *  3)	

#define STACK_AT		(GPR_SIZE *  4)	
#define STACK_V0		(GPR_SIZE *  5)	
#define STACK_V1		(GPR_SIZE *  6)	
#define STACK_A0		(GPR_SIZE *  7)		
#define STACK_A1		(GPR_SIZE *  8)		
#define STACK_A2		(GPR_SIZE *  9)		
#define STACK_A3		(GPR_SIZE * 10)		
#define STACK_T0		(GPR_SIZE * 11)
#define STACK_T1		(GPR_SIZE * 12)
#define STACK_T2		(GPR_SIZE * 13)
#define STACK_T3		(GPR_SIZE * 14)
#define STACK_T4		(GPR_SIZE * 15)
#define STACK_T5		(GPR_SIZE * 16)
#define STACK_T6		(GPR_SIZE * 17)
#define STACK_T7		(GPR_SIZE * 18)
#define STACK_S0		(GPR_SIZE * 19)
#define STACK_S1		(GPR_SIZE * 20)
#define STACK_S2		(GPR_SIZE * 21)
#define STACK_S3		(GPR_SIZE * 22)
#define STACK_S4		(GPR_SIZE * 23)
#define STACK_S5		(GPR_SIZE * 24)
#define STACK_S6		(GPR_SIZE * 25)
#define STACK_S7		(GPR_SIZE * 26)
#define STACK_T8		(GPR_SIZE * 27)
#define STACK_T9		(GPR_SIZE * 28)
#define STACK_GP		(GPR_SIZE * 29)
#define STACK_S8		(GPR_SIZE * 30)
#define STACK_RA		(GPR_SIZE * 31)

#define STACK_LO		(GPR_SIZE * 32)
#define STACK_HI		(GPR_SIZE * 33)

#define NR_REG_IN_STACK	(34)	
#define STACK_SIZE		(NR_REG_IN_STACK * GPR_SIZE)

/*
 *  Memory address mapping
 */
#define K0BASE          0x80000000
#define K0SIZE          0x20000000
#define K1BASE          0xa0000000
#define K1SIZE          0x20000000
#define K2BASE          0xc0000000
#define IS_K0SEG(x)     ((unsigned)(x) >= K0BASE && (unsigned)(x) < K1BASE)
#define IS_K1SEG(x)     ((unsigned)(x) >= K1BASE && (unsigned)(x) < K2BASE)
#define k02k1(a)        (((ulong)(a)) | K1BASE)
#define log2phy(a)      (((ulong)(a)) & 0x1fffffff)
#define phy2k0(a)       ((a) | K0BASE)
#define phy2k1(a)       ((a) | K1BASE)

/*
 *  break instruction codes 0..1023
 */
#define BRK_OVR         6               /* overflow          */
#define BRK_DZERO       7               /* divide by zero    */
#define BRK_TRAP        8               /* trap on condition */

/*
 *  Floating-Point Control register bits
 */
#define CSR_C           0x00800000
#define CSR_EXC         0x0003f000
#define CSR_EE          0x00020000
#define CSR_EV          0x00010000
#define CSR_EZ          0x00008000
#define CSR_EO          0x00004000
#define CSR_EU          0x00002000
#define CSR_EI          0x00001000
#define CSR_TV          0x00000800
#define CSR_TZ          0x00000400
#define CSR_TO          0x00000200
#define CSR_TU          0x00000100
#define CSR_TI          0x00000080
#define CSR_SV          0x00000040
#define CSR_SZ          0x00000020
#define CSR_SO          0x00000010
#define CSR_SU          0x00000008
#define CSR_SI          0x00000004
#define CSR_RM          0x00000003

/*
 *  Status Register
 */
#define SR_CUMASK       0xf0000000      /* Coprocessor usable bits    */
#define SR_CU3          0x80000000      /* Coprocessor 3 usable       */
#define SR_CU2          0x40000000      /* coprocessor 2 usable       */
#define SR_CU1          0x20000000      /* Coprocessor 1 usable       */
#define SR_CU0          0x10000000      /* Coprocessor 0 usable       */
#define SR_BEV          0x00400000      /* Bootstrap Exception Vector */
#define SR_TS           0x00200000      /* TLB shutdown               */
#define SR_PE           0x00100000      /* Parity Error               */
#define SR_CM           0x00080000      /* Cache Miss                 */
#define SR_PZ           0x00040000      /* Parity Zero                */
#define SR_SWC          0x00020000      /* Swap Caches                */
#define SR_ISC          0x00010000      /* Isolate Cache              */

#define SR_IMASK        0x0000ff00      /* Interrupt Mask             */
#define SR_IMASK8       0x00000000      /* Interrupt Mask level=8     */
#define SR_IMASK7       0x00008000      /* Interrupt Mask level=7     */
#define SR_IMASK6       0x0000c000      /* Interrupt Mask level=6     */
#define SR_IMASK5       0x0000e000      /* Interrupt Mask level=5     */
#define SR_IMASK4       0x0000f000      /* Interrupt Mask level=4     */
#define SR_IMASK3       0x0000f800      /* Interrupt Mask level=3     */
#define SR_IMASK2       0x0000fc00      /* Interrupt Mask level=2     */
#define SR_IMASK1       0x0000fe00      /* Interrupt Mask level=1     */
#define SR_IMASK0       0x0000ff00      /* Interrupt Mask level=0     */

#define SR_IBIT8        0x00008000      /* (Intr5)                    */
#define SR_IBIT7        0x00004000      /* (Intr4)                    */
#define SR_IBIT6        0x00002000      /* (Intr3)                    */
#define SR_IBIT5        0x00001000      /* (Intr2)                    */
#define SR_IBIT4        0x00000800      /* (Intr1)                    */
#define SR_IBIT3        0x00000400      /* (Intr0)                    */
#define SR_IBIT2        0x00000200      /* (Software Interrupt 1)     */
#define SR_IBIT1        0x00000100      /* (Software Interrupt 0)     */
#define SR_INT5         0x00008000      /* (Intr5)                    */
#define SR_INT4         0x00004000      /* (Intr4)                    */
#define SR_INT3         0x00002000      /* (Intr3)                    */
#define SR_INT2         0x00001000      /* (Intr2)                    */
#define SR_INT1         0x00000800      /* (Intr1)                    */
#define SR_INT0         0x00000400      /* (Intr0)                    */

#define SR_KUO          0x00000020      /* Kernel/User mode, old      */
#define SR_IEO          0x00000010      /* Interrupt Enable, old      */
#define SR_KUP          0x00000008      /* Kernel/User mode, previous */
#define SR_IEP          0x00000004      /* Interrupt Enable, previous */
#define SR_KUC          0x00000002      /* Kernel/User mode, current  */
#define SR_IEC          0x00000001      /* Interrupt Enable, current  */

/*
 *  Cause Register
 */
#define CAUSE_BD        0x80000000      /* Branch Delay               */
#define CAUSE_CEMASK    0x30000000      /* Coprocessor Error          */
#define CAUSE_CESHIFT   28              /* Right justify CE           */
#define CAUSE_IPMASK    0x0000ff00      /* Interrupt Pending          */
#define CAUSE_IPSHIFT   8               /* Right justify IP           */
#define CAUSE_IP8       0x00008000      /* (Intr5)                    */
#define CAUSE_IP7       0x00004000      /* (Intr4)                    */
#define CAUSE_IP6       0x00002000      /* (Intr3)                    */
#define CAUSE_IP5       0x00001000      /* (Intr2)                    */
#define CAUSE_IP4       0x00000800      /* (Intr1)                    */
#define CAUSE_IP3       0x00000400      /* (Intr0)                    */
#define CAUSE_INT5      0x00008000      /* (Intr5)                    */
#define CAUSE_INT4      0x00004000      /* (Intr4)                    */
#define CAUSE_INT3      0x00002000      /* (Intr3)                    */
#define CAUSE_INT2      0x00001000      /* (Intr2)                    */
#define CAUSE_INT1      0x00000800      /* (Intr1)                    */
#define CAUSE_INT0      0x00000400      /* (Intr0)                    */
#define CAUSE_SW2       0x00000200      /* (Software Int 1)           */
#define CAUSE_SW1       0x00000100      /* (Software Int 0)           */
#define CAUSE_EXCMASK   0x0000003c      /* Exception Code             */
#define CAUSE_EXCSHIFT  2               /* Right justify EXC          */

/*
 *  Exception Code
 */
#define EXC_INT         (0 << 2)        /* External interrupt         */
#define EXC_MOD         (1 << 2)        /* TLB modification           */
#define EXC_TLBL        (2 << 2)        /* TLB miss (Load or Ifetch)  */
#define EXC_TLBS        (3 << 2)        /* TLB miss (Save)            */
#define EXC_ADEL        (4 << 2)        /* Addr err (Load or Ifetch)  */
#define EXC_ADES        (5 << 2)        /* Addr err (Save)            */
#define EXC_IBE         (6 << 2)        /* Bus err (Ifetch)           */
#define EXC_DBE         (7 << 2)        /* Bus err (data load/store)  */
#define EXC_SYS         (8 << 2)        /* System call                */
#define EXC_BP          (9 << 2)        /* Break point                */
#define EXC_RI          (10 << 2)       /* Reserved instruction       */
#define EXC_CPU         (11 << 2)       /* Coprocessor unusable       */
#define EXC_OVF         (12 << 2)       /* Arithmetic overflow        */
#define EXC_TR          (13 << 2)       /* Trap instruction           */

#ifdef  __cplusplus
}
#endif

#endif /* _MIPS_H_ */
