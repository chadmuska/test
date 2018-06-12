/**************************************************************************
 *
 *         Copyright (c) 2002-2012 by iCatch Technology Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology Inc. All rights are reserved by iCatch Technology Inc.
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology Inc.
 *
 *  iCatch Technology Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 *  Author: Anmin Deng
 *
 **************************************************************************/
#include "mips.h"
#include "customization/dev_init.h"
#include GPIO_INC

#define BKDOOR_ENTRY_TOO_LARGE         1
#define USE_FRM_DRAM_BOOT_PARAM        0
#define USE_CPU_TIMER5_AS_TIMESTAMPS   1

#define TIMESTAMPS_CLK_HZ  1000000
#define TIMER_BASE_CLK_HZ 27000000
#define TIMESTAMPS_TIMER_BASE  (TIMER_BASE_CLK_HZ / TIMESTAMPS_CLK_HZ - 1)


	.extern	__init
	.extern	__main

	.set	noreorder
	.set	noat

	.section .exceptRam, "ax"
	#ifdef RAM_PROGRAM
	.ascii	"DRAM CODE"
	#else
	.ascii	"ROM CODE "
	#endif

	/* New feature here: it allows modification of registers after the first
	 * block is read onto DRAM!
	 * NOTE! backdoor code text end can not exceed one block size
	 */
	/* in backdoor code all branches must be PC-related or PC-region
	 * accessing any static linked variable out of 0th block is not allowed
	 */
	.section .exceptRom, "ax"
	.globl	_backdoorEntry
	.ent	_backdoorEntry
_backdoorEntry:

#if USE_FRM_DRAM_BOOT_PARAM /* dram parameter set by FRM */
	/*FIXME*/ /****TODO*DRAM param workaround*/
	/*workaround by CS: change DRAM read parameter for VC83*/
	lui	t2, 0xa000
	lw	t1, 0x100(t2)
	lui	t0, 0xb000
	andi	t3, t1, 0xff
	beqz	t3, $$defaultDramAdj
	srl	t1, t1, 8
	sb	t1, 0x7051(t0)
	srl	t1, t1, 8
	sb	t1, 0x7056(t0)
	lw	t2, 0x104(t2)
	srl	t1, t1, 8
	sb	t1, 0x7057(t0)
	sb	t2, 0x7058(t0)
	srl	t2, t2, 8
	sb	t2, 0x7073(t0)
	srl	t2, t2, 8
	sb	t2, 0x7080(t0)
	srl	t2, t2, 8
	j	$$dramAdjDone
	sb	t2, 0x00d1(t0)

$$defaultDramAdj:
	/* using default parameter */
	li	t1, 0x06
	sb	t1, 0x7051(t0)
	li	t1, 0xb5
	sb	t1, 0x7056(t0)
	li	t1, 0x14
	sb	t1, 0x7057(t0)
	li	t1, 0x1b
	sb	t1, 0x7058(t0)
	li	t1, 0x2a
	sb	t1, 0x7073(t0)
	sb	$0, 0x7080(t0)
	li	t1, 0xa8
	sb	t1, 0x00d1(t0)
$$dramAdjDone:
#else
	lui	t0, 0xb000
#endif

  #if RTC_WAKEUP_SUPPORT
	/*no shutdown if RTC IRQ on*/
	lhu	t1, 0x1000(t0)
	nop
	andi	t1, t1, 0x1000
	bnez	t1, $$noShutDown
	nop
  #endif

	/*no shutdown if any power key */
	lbu	t1, 0x1304(t0)
	nop
	bnez	t1, $$noShutDown
	nop

	/* while (1) { globReg->pwren_cpu = 8; } */
$$EMERGENCY_SHUTDOWN:
	li	t1, 8
	j	$$EMERGENCY_SHUTDOWN
	sb	t1, 0x00c0(t0)

$$noShutDown:
	/* clear latch to avoid unexpected power on after quick battery removal & insert */
	li	t1, 0xa
	sb	t1, 0x00c0(t0)

#if BKDOOR_ENTRY_TOO_LARGE
	b	_backdoorEntryLarge
	nop
	.end	_backdoorEntry

	.section .exceptRomLarge, "ax"
	.globl	_backdoorEntryLarge
	.ent	_backdoorEntryLarge
_backdoorEntryLarge:

#endif /*#if BKDOOR_ENTRY_TOO_LARGE*/

#ifndef RAM_PROGRAM
	li	t1, 0x0c
	sb	t1, (0x1055)(t0)/* ROM cycle */
#endif
	li	t1, NAND_ACT_RCV_TIME
	sb	t1, (0x4061)(t0)
#ifdef NAND_ACT_RCV_TIME_192MHZ
	li	t1, NAND_ACT_RCV_TIME_192MHZ
	sb	t1, (0x1305)(t0)
#endif

#ifdef POCU_SAR0_LOWBAT_CHECK_WORKAROUND_THRESHOLD_VALUE
	/*t0 should be untouched and left as 0xb0000000*/
	/*FIXME*/ /****TODO*low battery check pouc workaround*/
	/*workaround by CS: config ADC SAR0 for checking battery*/
	/*workaround by CS: read ADC SAR0 value later after loaded and boot*/

	li	t1, 1
	sb	t1, 0x0074(t0)
	lbu	t1, 0x0060(t0)
	lbu	t2, 0x0065(t0)
	ori	t1, t1, 3
	sb	t1, 0x0060(t0)
	ori	t2, t2, 0xd1
	sb	t1, 0x0065(t0)

	li	t1, 0x20
	sb	t1, 0x6450(t0)	# paudReg->tadda = 0x20
	li	t1, 0x05
	sb	t1, 0x6454(t0)	# paudReg->envref = 5, strong mode
	li	t1, 1
	sb	t1, 0x6468(t0)	# paudReg->sarclksel = 1
	sb	$0, 0x6464(t0)	# paudReg->sars = 0
	sb	t1, 0x6460(t0)	# paudReg->ensar = 1
#endif

#if 0 /*spca6350 boot test*/
#if USE_CPU_TIMER5_AS_TIMESTAMPS
	sb	$0, 0x1190(t0)	# timer5 disable
	li	t1, TIMESTAMPS_TIMER_BASE
	sh	t1, 0x1192(t0)
	li	t1, TIMESTAMPS_CLK_HZ
	sw	t1, 0x1194(t0)
	li	t1, 1
	sb	t1, 0x1190(t0)
#endif
#endif /*#if 0*/ /*spca6350 boot test*/

	/*FIXME*/ /****TODO*fill a signature for confirming backdoor run*/
	li	t1, 0x12345678

#if MULTI_BOOT_STAGE
	/* using long calls to engage icache */
	la	t2, bkdoorLoadFW
	jr	t2
	sw	t1, (0x1300)(t0)
#else  /*#if MULTI_BOOT_STAGE*/
	jr	ra
	sw	t1, (0x1300)(t0)
#endif /*#if MULTI_BOOT_STAGE*/

#if BKDOOR_ENTRY_TOO_LARGE
	.end	_backdoorEntryLarge
#else  /*#if BKDOOR_ENTRY_TOO_LARGE*/
	.end	_backdoorEntry
#endif /*#if BKDOOR_ENTRY_TOO_LARGE*/


	/*_____________________________________________________________________

	  Reset Handler:
	    1. Detect and initialize DRAM (by reading I/O trap)
	    2. Initialize address varialbes (referenced by __init())
	    3. Disable timers and clear timer's interrupt
	  _____________________________________________________________________
	*/
	.section .boot, "ax"
	.align	2
	.globl	__resetTrampoline
	.ent	__resetTrampoline
__resetTrampoline:

	lui	t0, 0xb000

  /* config for driving: temporary add for 6330 romter + evb */
  #ifndef RAM_PROGRAM
	li	t1, 0x55555555
	sw	t1, (0x0140)(t0)
	sw	t1, (0x0144)(t0)
	sw	t1, (0x0148)(t0)
	sw	t1, (0x014C)(t0)
  #endif

	/* LDO enable */
	li	t1, 1
	sb	t1, 0x00c2(t0)

	/* PLL for Display */
	li	t1, 1
	sb	t1, 0x0278(t0)

  #ifdef POCU_SAR0_LOWBAT_CHECK_WORKAROUND_THRESHOLD_VALUE
	/*t0 should be untouched and left as 0xb0000000*/
	/*FIXME*/ /****TODO*low battery check pouc workaround*/
	/*workaround by CS: config ADC SAR0 for checking battery*/
	/*workaround by CS: read ADC SAR0 value later after loaded and boot*/
	/*if bat sar0 value too low, shut down POCU directly*/
	/* paudReg->snapsardata = 1 */
	li	t1, 1
	sb	t1, 0x646c(t0)
	nop
	/* val = paudReg->saro, with 10-bit 2's complement signed-extend */
	lhu	t1, 0x6470(t0)
	nop
	xori	t1, t1, 0x200	# val += 512
/*TEST*/
	sw	t1, (0x1300)(t0)

	/* if (val < THRESHOLD) shutdown() */
	sltu	t1, t1, POCU_SAR0_LOWBAT_CHECK_WORKAROUND_THRESHOLD_VALUE
	bnez	t1, $$EMERGENCY_SHUTDOWN
	nop
  #endif

	/* Enable DC-DC conversion */
	li	t1, 0x02
	sb	t1, 0x00c0(t0)

	li	t1, -1
	sb	zero, 0x1018(t0)  /* Disable all interrupts */
	sw	zero, 0x1008(t0)  /* Disable hardware interrupts */
	sw	zero, 0x100c(t0)  /* Disable peripheral interrupts */
	sw	t1,   0x1004(t0)  /* Clear peripheral interrupt */

#if 0 /* move enable clock to main.c */
	/* Enable clock */
	/*FIXME* only turn on essential clocks and turn on other clocks
	 * in individual device modules */
	li	t1, -1
	sb	t1, (0x0060)(t0)
	sw	t1, (0x0064)(t0)
#else
	/* 5330 FM48mclk, Phase clock setting */
	li	t1, 0x02    /* [2]USB, [1]Peri, [0]Aud, only 3 bits */
	sb	t1, (0x0060)(t0)
	li	t1, 0x08c30247
	sw	t1, (0x0064)(t0) /* 0xb0000064 ~ 0xb0000067 = 0x088b0247 */
#endif

#ifdef FRONT_RGB_IN_PIN_MASK /* parallel sensor needs this */
	li	t1, FRONT_RGB_IN_PIN_MASK
	sw	t1, 0x02b4(t0)
#endif
#ifdef FRONT_RGB_IN_PULL_EN
	li	t1, FRONT_RGB_IN_PULL_EN
	sw	t1, 0x0118(t0)
#endif

#ifdef HW_LMI16_17_SCHIMITT_TRIGGER_WINDOW
	li	t1, HW_LMI16_17_SCHIMITT_TRIGGER_WINDOW
	sb	t1, 0x00ed(t0)
#endif
#ifdef HW_LMI18_19_SCHIMITT_TRIGGER_WINDOW
	li	t1, HW_LMI18_19_SCHIMITT_TRIGGER_WINDOW
	sb	t1, 0x00ee(t0)
#endif
#ifdef HW_LMI20_21_SCHIMITT_TRIGGER_WINDOW
	li	t1, HW_LMI20_21_SCHIMITT_TRIGGER_WINDOW
	sb	t1, 0x00ef(t0)
#endif

/* GPIO pull & strength set */
#ifdef SP5K_GPIO_GEN_PULL_MIN
	li	t1, SP5K_GPIO_GEN_PULL_MIN
 	sw	t1, 0x0114(t0)
#endif
#ifdef SP5K_GPIO_TGL_PULL_MIN
	li	t1, SP5K_GPIO_TGL_PULL_MIN
 	sw	t1, 0x0108(t0)
#endif
#ifdef SP5K_GPIO_DISP_PULL_MIN
	li	t1, SP5K_GPIO_DISP_PULL_MIN
 	sw	t1, 0x0104(t0)
#endif
#ifdef SP5K_GPIO_LMI_PULL_MIN
	li	t1, SP5K_GPIO_LMI_PULL_MIN
 	sw	t1, 0x0100(t0)
#endif
#ifdef SP5K_GPIO_FML_PULL_MIN
	li	t1, SP5K_GPIO_FML_PULL_MIN
 	sw	t1, 0x010c(t0)
#endif
#ifdef SP5K_GPIO_FMH_PULL_MIN
	li	t1, SP5K_GPIO_FMH_PULL_MIN
 	sw	t1, 0x0110(t0)
#endif

#if defined(SP5K_GPIO_GEN_DS0_MIN) && defined(SP5K_GPIO_GEN_DS1_MIN)
	li	t1, SP5K_GPIO_GEN_DS0_MIN
	sw	t1, 0x0138(t0)
	li	t1, SP5K_GPIO_GEN_DS1_MIN
	sw	t1, 0x013c(t0)
#endif
#if defined(SP5K_GPIO_TGL_DS0_MIN) && defined(SP5K_GPIO_TGL_DS1_MIN)
	li	t1, SP5K_GPIO_TGL_DS0_MIN
	sw	t1, 0x0130(t0)
	li	t1, SP5K_GPIO_TGL_DS1_MIN
	sw	t1, 0x0134(t0)
#endif
#if defined(SP5K_GPIO_DISP_DS0_MIN) && defined(SP5K_GPIO_DISP_DS1_MIN)
	li	t1, SP5K_GPIO_DISP_DS0_MIN
	sw	t1, 0x0128(t0)
	li	t1, SP5K_GPIO_DISP_DS1_MIN
	sw	t1, 0x012c(t0)
#endif
#if defined(SP5K_GPIO_LMI_DS0_MIN) && defined(SP5K_GPIO_LMI_DS1_MIN)
	li	t1, SP5K_GPIO_LMI_DS0_MIN
	sw	t1, 0x0120(t0)
	li	t1, SP5K_GPIO_LMI_DS1_MIN
	sw	t1, 0x0124(t0)
#endif
#if defined(SP5K_GPIO_FML_DS0_MIN) && defined(SP5K_GPIO_FML_DS1_MIN)
	li	t1, SP5K_GPIO_FML_DS0_MIN
	sw	t1, 0x0140(t0)
	li	t1, SP5K_GPIO_FML_DS1_MIN
	sw	t1, 0x0144(t0)
#endif
#if defined(SP5K_GPIO_FMH_DS0_MIN) && defined(SP5K_GPIO_FMH_DS1_MIN)
	li	t1, SP5K_GPIO_FMH_DS0_MIN
	sw	t1, 0x0148(t0)
	li	t1, SP5K_GPIO_FMH_DS1_MIN
	sw	t1, 0x014c(t0)
#endif

#if !RAM_PROGRAM /*spca6350 boot tests*/
#if USE_CPU_TIMER5_AS_TIMESTAMPS
	sb	$0, 0x1190(t0)	# timer5 disable
	li	t1, TIMESTAMPS_TIMER_BASE
	sh	t1, 0x1192(t0)
	li	t1, TIMESTAMPS_CLK_HZ
	sw	t1, 0x1194(t0)
	li	t1, 1
	li	t2, 1 << 5
	sb	t2, 0x1004(t0)  /* clearing timer5 status */
	sb	t1, 0x1190(t0)
# if 0 /* testing timer5 interrupt */
	sb	t2, 0x100c(t0)  /* enabling timer5 interrupt */
	sb	t1, 0x1008(t0)  /* enabling timer interrupts */
	sb	t1, 0x1018(t0)  /* enabling interrupt master */
# endif
#endif
#endif /*#if !RAM_PROGRAM*/ /*spca6350 boot tests*/

	/* Enable LDO AD/DA */
	li      t1, 0x01
	sb      t1, (0x00c2)(t0)

#if 0 /* fill the dmem with 0xefefefef to sys stack top to check dmem usage */
# if (SYS_STACK_BOT-SYS_STACK_SIZE) >> 16 == 0xd000
	lui	t3, %hi(SYS_STACK_BOT-SYS_STACK_SIZE)
	addiu	t2, t3, %lo(SYS_STACK_BOT-SYS_STACK_SIZE)
	li	t1, 0xefefefef

.L1:
	addiu	t3, t3, 4
	bne	t2, t3, .L1
	sw	t1, -4(t3)
# endif /*#if (SYS_STACK_BOT-SYS_STACK_SIZE) >> 16 == 0xd000*/
#endif /*spca6350 boot tests*/


	/* SDRAM initialization */
#if !RAM_PROGRAM
	jal	sdramInit
	nop
#endif

	/* Move and clear data area */
	jal	__init
	nop

#if 0 /*spca6350 boot test*/
	jal	QUARTInit
	nop
#if 1
	mfc0	a0, $12, 2           /*srsctl*/
	jal	QUARTXnum
	li	a1, 8

	jal	QUARTChar
	li	a0, '\n'

	mfc0	a0, $16, 0           /*config*/
	jal	QUARTXnum
	li	a1, 8

	jal	QUARTChar
	li	a0, '\n'

	mfc0	a0, $16, 1           /*config1*/
	jal	QUARTXnum
	li	a1, 8

	jal	QUARTChar
	li	a0, '\n'

	mfc0	a0, $16, 7           /*config7*/
	jal	QUARTXnum
	li	a1, 8

	jal	QUARTChar
	li	a0, '\n'

	la	a0, __clz_tab
	jal	QUARTDump
	li	a1, 256
#endif
#endif /*spca6350 boot test*/

/* anmin-20080910
 * there must be an absolute addr long call, "see start"
 * o. before the call the control could be in ROM in .boot, __main is in .text
 * o. or boot loader jumped in uncache jump uncache->cache if all text in RAM
 */
	/*_____________________________________________________________________

	  jump to __main() (never back again)
	  _____________________________________________________________________
	*/
	/* moved to tx_ill.S */

#if RAM_BOOT
	la	t3, __main
	jr	t3               /* Never return */
#else  /*#if RAM_BOOT*/
	j	__main           /* Never return */
#endif /*#if RAM_BOOT*/
#if defined(CPU_CLK)
	li	a0, CPU_CLK
#else
	move	a0, zero
#endif

	.end	__resetTrampoline

#if !RAM_PROGRAM
/* This is DRAM run bootcode, only for SD/SPI boot test use */
	.ent	sdramInit
sdramInit:
	lui	t0, 0xb000
	/* Enable DRAM parameter */
	li		t1, 0x01
	sb		t1, (0x00c9)(t0)
	li		t1, 0x20
	sb		t1, (0x00ca)(t0)
	li		t1, 0x34
	sb		t1, (0x00cf)(t0)
	li		t1, 0x01
	sb		t1,	(0x0291)(t0)
	sb		$0,	(0x00cd)(t0)
	sb		t1, (0x0293)(t0)
	sb		t1, (0x0292)(t0)
	sb		$0,	(0x0074)(t0)
	li		t1, 0x03
	sb		t1, (0x00c8)(t0)
	/* disable regabort isr for delay */
	lbu		t3, (0x100c)(t0) /* 100c[7]: internal bus error interrupt */
	ori		t2, t0, 0x9050
	andi	t1, t3, 0x7f
	sb		t1, (0x100c)(t0)
	/* delay */
	sb		$0, (0x0000)(t2) /* w 9050 and trigger regabort for delay */

	li		t1, 0x05
	sb		t1, (0x008a)(t0) /* regclk, 72M */
	li		t1, 0x08
	sb		t1, (0x008e)(t0) /* clk48m, 48M */
	sb		t1, (0x0092)(t0) /* fm,     48M */
	li		t1, 0x01
	sb		t1, (0x0070)(t0)
	/* delay */
	sb		$0, (0x0000)(t2)

#if (CPU_CLK==9) /* 216MHz */
	sb		$0, (0x0078)(t0)
	li		t1, 0x20
	sb		t1, (0x007a)(t0)
	li		t1, 0x01
	sb		t1, (0x007b)(t0)
	li		t1, 0x07
	sb		t1, (0x0079)(t0)
	li		t1, 0x01
	sb		t1, (0x0078)(t0)

	li		t1, 0x06
	sb		t1, (0x008a)(t0)
	li		t1, 0x08
	sb		t1, (0x008e)(t0)
	li		t1, 0x04
	sb		t1, (0x0092)(t0)
	li		t1, 0xa0
	sb		t1, (0x1104)(t0)
	li		t1, 0x01
	sb		t1, (0x1105)(t0)
#elif (CPU_CLK==8) /* 360MHz */
	sb		$0, (0x0078)(t0)
	li		t1, 0xe0
	sb		t1, (0x007a)(t0)
	li		t1, 0x01
	sb		t1, (0x007b)(t0)
	li		t1, 0x03
	sb		t1, (0x0079)(t0)
	li		t1, 0x20
	sb		t1, (0x007c)(t0)
	sb		t1, (0x026c)(t0)
	li		t1, 0x04
	sb		t1, (0x0298)(t0)
	li		t1, 0x03
	sb		t1, (0x0299)(t0)
	li		t1, 0x01
	sb		t1, (0x0078)(t0)

	li		t1, 0x0a
	sb		t1, (0x008a)(t0)
	li		t1, 0x0e
	sb		t1, (0x008e)(t0)
	li		t1, 0x07
	sb		t1, (0x0092)(t0)
	li		t1, 0xa0
	sb		t1, (0x1104)(t0)
	li		t1, 0x01
	sb		t1, (0x1105)(t0)
#else /* CPU_CLK == 8 */
#error CPU_CLK selection error!
#endif /* CPU_CLK == 9 */

	/* delay */
	sb		$0, (0x0000)(t2)
	sb		$0, (0x0070)(t0)

	sb		$0, (0x00dc)(t0) /*mem io type*/
	sb		$0, (0x7122)(t0) /*mem io oen*/
	sb		$0, (0x7121)(t0) /*dram reset_n pin driving low*/
	/* delay */
	sb		$0, (0x0000)(t2)
	li		t1, 0x01
	sb		t1, (0x7121)(t0) /*dram reset_n pin driving high*/
	/* delay */
	sb		$0, (0x0000)(t2)
	li      t1, 0xc0
	sb      t1, (0x7120)(t0) /*mcke_trig*/
#if (CPU_CLK==9) /* 216MHz */
	li      t1, 0xa0
	sb      t1, (0x7050)(t0) /*dramtype, the size will be calibrated ater in init.c*/
	li      t1, 0x0a
	sb      t1, (0x7081)(t0) /*[7]ddrphy_testen [3]ddrphy_pdn [1:0]frange*/
	li      t1, 0x7d
	sb      t1, (0x7054)(t0) /*refintval*/
	li      t1, 0x06
	sb      t1, (0x7055)(t0)
	li      t1, 0x03
	sb      t1, (0x7056)(t0) /*zqcsintvl*/
	li      t1, 0x40
	sb      t1, (0x7057)(t0)
	li      t1, 0x09
	sb      t1, (0x7058)(t0) /*trp[4:0]}*/
	li      t1, 0x1a
	sb      t1, (0x7059)(t0) /*txsnr[7:0]*/
	li      t1, 0x09
	sb      t1, (0x705a)(t0) /*tfaw[4:0]*/
	li      t1, 0x0a
	sb      t1, (0x705b)(t0) /*trc[4:0]*/
	li      t1, 0x17
	sb      t1, (0x705d)(t0) /*trfc[7:0]*/
	li      t1, 0x11
	sb      t1, (0x705e)(t0) /*twrp[5:0]*/
	li      t1, 0x31
	sb      t1, (0x705f)(t0) /*{1'h0,trrd[2:0],trcd[3:0]}*/
	li      t1, 0x10
	sb      t1, (0x7064)(t0) /*mrscaslat*/
	li      t1, 0x03
	sb      t1, (0x7065)(t0) /*{tWR,resetdll}*/
	li      t1, 0x12
	sb      t1, (0x7066)(t0) /*mddr123emr[9:0]*/
	sb      $0, (0x7067)(t0)
	sb      $0, (0x7068)(t0) /*ddr23emr2[10:0]*/
	sb      $0, (0x7069)(t0)
	li      t1, 0x3
	sb      t1, (0x706e)(t0) /*tmrd[15:0]*/
	sb      $0, (0x706f)(t0)
	li      t1, 0x08
	sb      t1, (0x7072)(t0) /*wl_lat_sel*/
	li      t1, 0x99
	sb      t1, (0x7073)(t0) /*{rp_lat_sel[3:0],rd_lat_sel[3:0]}*/
	li      t1, 0x0c
	sb      t1, (0x7074)(t0) /*tw2r[4:0]*/
	li      t1, 0x07
	sb      t1, (0x7075)(t0) /*tr2w[4:0]*/
	sb		$0, (0x7080)(t0) /*rdata_sel0*/
	li		t1, 0x55
	sb		t1, (0x7082)(t0) /*dqs delay*/

	/* delay */
	sb		$0, (0x0000)(t2)
	sb		$0, (0x7120)(t0) /*mcs_n*/
	/* delay */
	sb		$0, (0x0000)(t2)
	li      t1, 0x02
	sb      t1, (0x70e6)(t0) /*initsdram*/
	/* delay */
	sb		$0, (0x0000)(t2)
	sb		$0, (0x7051)(t0) /*[5]sstl2_pwd_off [4]sstl2_pwd_on [3]disrefresh [2]diszqcs [0]sdclkmode*/
	/* delay */
	sb		$0, (0x0000)(t2)
#elif (CPU_CLK==8) /* 360MHz */
	li      t1, 0xa0
	sb      t1, (0x7050)(t0) /*dramtype, the size will be calibrated ater in init.c*/
	li      t1, 0x2a
	sb      t1, (0x7054)(t0) /*refintval*/
	li      t1, 0x09
	sb      t1, (0x7055)(t0)
	li		t1, 0x03
	sb      t1, (0x7056)(t0) /*zqcsintvl*/
	li		t1, 0x40
	sb      t1, (0x7057)(t0)
	li      t1, 0x0a
	sb      t1, (0x7058)(t0) /*trp[4:0]}*/
	li      t1, 0x34
	sb      t1, (0x7059)(t0) /*txsnr[7:0]*/
	li      t1, 0x0d
	sb      t1, (0x705a)(t0) /*tfaw[4:0]*/
	li      t1, 0x0f
	sb      t1, (0x705b)(t0) /*trc[4:0]*/
	li      t1, 0x30
	sb      t1, (0x705d)(t0) /*trfc[7:0]*/
	li      t1, 0x14
	sb      t1, (0x705e)(t0) /*twrp[5:0]*/
	li      t1, 0x31
	sb      t1, (0x705f)(t0) /*{1'h0,trrd[2:0],trcd[3:0]}*/
	li      t1, 0x10
	sb      t1, (0x7064)(t0) /*mrscaslat*/
	li      t1, 0x03
	sb      t1, (0x7065)(t0) /*{tWR,resetdll}*/
	li      t1, 0x12
	sb      t1, (0x7066)(t0) /*mddr123emr[9:0]*/
	sb		$0, (0x7067)(t0)
	sb      $0, (0x7068)(t0) /*ddr23emr2[10:0]*/
	sb		$0, (0x7069)(t0)
	li      t1, 0x3
	sb      t1, (0x706e)(t0) /*tmrd[15:0]*/
	sb		$0, (0x706f)(t0)
	li      t1, 0x08
	sb      t1, (0x7072)(t0) /*wl_lat_sel*/
	li      t1, 0x9a
	sb      t1, (0x7073)(t0) /*{rp_lat_sel[3:0],rd_lat_sel[3:0]}*/
	li      t1, 0x0d
	sb      t1, (0x7074)(t0) /*tw2r[4:0]*/
	li      t1, 0x08
	sb      t1, (0x7075)(t0) /*tr2w[4:0]*/
	li		t1, 0
	sb		t1, (0x7080)(t0) /*rdata_sel0*/
	li      t1, 0x09
	sb      t1, (0x7081)(t0) /*[7]ddrphy_testen [3]ddrphy_pdn [1:0]frange*/
	li		t1, 0x77
	sb		t1, (0x7082)(t0) /*dqs delay*/
	li		t1, 0x22222222
	sw		t1, 0x7090(t0)
	sw		t1, 0x7094(t0)

	li		t1, 0xa4
	sb		t1, (0x7782)(t0) /*driving sdclk other*/
	/* delay */
	sb		$0, (0x0000)(t2)
	sb		$0, (0x7120)(t0) /*mcs_n*/
	/* delay */
	sb		$0, (0x0000)(t2)
	li      t1, 0x02
	sb      t1, (0x70e6)(t0) /*initsdram*/
	/* delay */
	sb		$0, (0x0000)(t2)
	sb		$0, (0x7051)(t0) /*[5]sstl2_pwd_off [4]sstl2_pwd_on [3]disrefresh [2]diszqcs [0]sdclkmode*/
	/* delay */
	sb		$0, (0x0000)(t2)
#else /* CPU_CLK == 8 */
#error CPU_CLK selection error!
#endif /* CPU_CLK == 9 */
	/* reset front to avoid regabort caused front fail */
	li		t1, 0x01
	sb		t1, 0x0020(t0)
	sb		$0, 0x0020(t0)
	/* clear lbuserr_sts 7001[4] to let codesentry work well */
	li		t1, 0x10
	sb		t1, 0x7001(t0)
	/* enable regabort */
	li		t1, 0x80
	sb		t1, (0x1004)(t0) /* clear regabort evt */
	sb		t3, (0x100c)(t0)
	li		t1, 0x03
	sb		t1, 0x0282(t0) /* gpeclk must < 3 */
	li		t1, -1
	sw		t1, (0x0140)(t0)
	sw		t1, (0x0144)(t0)
	sw		t1, (0x0148)(t0)
	sb		t1, (0x014c)(t0)
	jr	ra
	nop
	.end	sdramInit

/* This is DRAM run bootcode, only for SD/SPI boot test use */
#endif /*#if !RAM_PROGRAM*/
