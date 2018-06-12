/**************************************************************************
 *
 *       Copyright (c) 2012-2012 by iCatch Technology, Inc.
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file hal_usbhost.h
 * @brief The header file of OHCI/EHCI hardware
 * @author LiJun
 */
#ifndef _HAL_USBHOST_H_
#define _HAL_USBHOST_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/* ehci: */
/* HCCAPBASE: */
#define HC_LENGTH(p)		(((p)>>00)&0x00ff)	/* bits 7:0 */
#define HC_VERSION(p)		(((p)>>16)&0xffff)	/* bits 31:16 */

/* HCSPARAMS: */
#define HCS_DEBUG_PORT(p)	(((p)>>20)&0xf)	/* bits 23:20, debug port? */
#define HCS_INDICATOR(p)	((p)&(1 << 16))	/* true: has port indicators */
#define HCS_N_CC(p)		(((p)>>12)&0xf)	/* bits 15:12, #companion HCs */
#define HCS_N_PCC(p)		(((p)>>8)&0xf)	/* bits 11:8, ports per CC */
#define HCS_PORTROUTED(p)	((p)&(1 << 7))	/* true: port routing */
#define HCS_PPC(p)		((p)&(1 << 4))	/* true: port power control */
#define HCS_N_PORTS(p)		(((p)>>0)&0xf)	/* bits 3:0, ports on HC */

/* HCCPARAMS: */
#define HCC_EXT_CAPS(p)		(((p)>>8)&0xff)	/* for pci extended caps */
#define HCC_ISOC_CACHE(p)       ((p)&(1 << 7))	/* true: can cache isoc frame */
#define HCC_ISOC_THRES(p)       (((p)>>4)&0x7)	/* bits 6:4, uframes cached */
#define HCC_CANPARK(p)		((p)&(1 << 2))	/* true: can park on async qh */
#define HCC_PGM_FRAMELISTLEN(p) ((p)&(1 << 1))	/* true: periodic_size changes */
#define HCC_64BIT_ADDR(p)       ((p)&(1))	/* true: can use 64-bit addr */

/* USBCMD: */
/* 23:16 is r/w intr rate, in microframes; default "8" == 1/msec */
#define CMD_PARK	(1<<11)	/* enable "park" on async qh */
#define CMD_PARK_CNT(c)	(((c)>>8)&3)	/* how many transfers to park for */
#define CMD_LRESET	(1<<7)	/* partial reset (no ports, etc) */
#define CMD_IAAD	(1<<6)	/* "doorbell" interrupt async advance */
#define CMD_ASE		(1<<5)	/* async schedule enable */
#define CMD_PSE  	(1<<4)	/* periodic schedule enable */
/* 3:2 is periodic frame list size */
#define CMD_RESET	(1<<1)	/* reset HC not bus */
#define CMD_RUN		(1<<0)	/* start/stop HC */

/* USBSTS: */
#define STS_ASS		(1<<15)	/* Async Schedule Status */
#define STS_PSS		(1<<14)	/* Periodic Schedule Status */
#define STS_RECL	(1<<13)	/* Reclamation */
#define STS_HALT	(1<<12)	/* Not running (any reason) */
/* some bits reserved */
/* these STS_* flags are also intr_enable bits (USBINTR) */
#define STS_IAA		(1<<5)	/* Interrupted on async advance */
#define STS_FATAL	(1<<4)	/* such as some PCI access errors */
#define STS_FLR		(1<<3)	/* frame list rolled over */
#define STS_PCD		(1<<2)	/* port change detect */
#define STS_ERR		(1<<1)	/* "error" completion (overflow, ...) */
#define STS_INT		(1<<0)	/* "normal" completion (short, ...) */

/* CONFIGFLAG: */
#define FLAG_CF		(1<<0)	/* true: we'll support "high speed" */

/* PORTSC: */
/* 31:23 reserved */
#define PORT_WKOC_E	(1<<22)	/* wake on overcurrent (enable) */
#define PORT_WKDISC_E	(1<<21)	/* wake on disconnect (enable) */
#define PORT_WKCONN_E	(1<<20)	/* wake on connect (enable) */
/* 19:16 for port testing */
#define PORT_LED_OFF	(0<<14)
#define PORT_LED_AMBER	(1<<14)
#define PORT_LED_GREEN	(2<<14)
#define PORT_LED_MASK	(3<<14)
#define PORT_OWNER	(1<<13)	/* true: companion hc owns this port */
#define PORT_POWER	(1<<12)	/* true: has power (see PPC) */
#define PORT_USB11(x) (((x)&(3<<10))==(1<<10))	/* USB 1.1 device */
/* 11:10 for detecting lowspeed devices (reset vs release ownership) */
/* 9 reserved */
#define PORT_RESET	(1<<8)	/* reset port */
#define PORT_SUSPEND	(1<<7)	/* suspend port */
#define PORT_RESUME	(1<<6)	/* resume it */
#define PORT_OCC	(1<<5)	/* over current change */
#define PORT_OC		(1<<4)	/* over current active */
#define PORT_PEC	(1<<3)	/* port enable change */
#define PORT_PE		(1<<2)	/* port enable */
#define PORT_CSC	(1<<1)	/* connect status change */
#define PORT_CONNECT	(1<<0)	/* device connected */


/* ohci: */
/*
* HcControl (control) register masks
*/
#define OHCI_CTRL_CBSR	(3 << 0)	/* control/bulk service ratio */
#define OHCI_CTRL_PLE	(1 << 2)	/* periodic list enable */
#define OHCI_CTRL_IE		(1 << 3)	/* isochronous enable */
#define OHCI_CTRL_CLE	(1 << 4)	/* control list enable */
#define OHCI_CTRL_BLE	(1 << 5)	/* bulk list enable */
#define OHCI_CTRL_HCFS	(3 << 6)	/* host controller functional state */
#define OHCI_CTRL_IR		(1 << 8)	/* interrupt routing */
#define OHCI_CTRL_RWC	(1 << 9)	/* remote wakeup connected */
#define OHCI_CTRL_RWE	(1 << 10)/* remote wakeup enable */

/* pre-shifted values for HCFS */
#define OHCI_USB_RESET	(0 << 6)
#define OHCI_USB_RESUME	(1 << 6)
#define OHCI_USB_OPER	(2 << 6)
#define OHCI_USB_SUSPEND	(3 << 6)

/*
 * HcCommandStatus (cmdstatus) register masks
 */
#define OHCI_HCR		(1 << 0)	/* host controller reset */
#define OHCI_CLF  	(1 << 1)	/* control list filled */
#define OHCI_BLF  	(1 << 2)	/* bulk list filled */
#define OHCI_OCR  	(1 << 3)	/* ownership change request */
#define OHCI_SOC  	(3 << 16)	/* scheduling overrun count */

/*
 * masks used with interrupt registers:
 * HcInterruptStatus (intrstatus)
 * HcInterruptEnable (intrenable)
 * HcInterruptDisable (intrdisable)
 */
#define OHCI_INTR_SO	(1 << 0)	/* scheduling overrun */
#define OHCI_INTR_WDH	(1 << 1)	/* writeback of done_head */
#define OHCI_INTR_SF	(1 << 2)	/* start frame */
#define OHCI_INTR_RD	(1 << 3)	/* resume detect */
#define OHCI_INTR_UE	(1 << 4)	/* unrecoverable error */
#define OHCI_INTR_FNO	(1 << 5)	/* frame number overflow */
#define OHCI_INTR_RHSC	(1 << 6)	/* root hub status change */
#define OHCI_INTR_OC	(1 << 30)	/* ownership change */
#define OHCI_INTR_MIE	(1 << 31)	/* master interrupt enable */

/* OHCI ROOT HUB REGISTER MASKS */

/* portstatus bits */
#define RH_PS_CCS            0x00000001	/* current connect status */
#define RH_PS_PES            0x00000002	/* port enable status */
#define RH_PS_PSS            0x00000004	/* port suspend status */
#define RH_PS_POCI           0x00000008	/* port over current indicator */
#define RH_PS_PRS            0x00000010	/* port reset status */
#define RH_PS_PPS            0x00000100	/* port power status */
#define RH_PS_LSDA           0x00000200	/* low speed device attached */
#define RH_PS_CSC            0x00010000	/* connect status change */
#define RH_PS_PESC           0x00020000	/* port enable status change */
#define RH_PS_PSSC           0x00040000	/* port suspend status change */
#define RH_PS_OCIC           0x00080000	/* over current indicator change */
#define RH_PS_PRSC           0x00100000	/* port reset status change */

/* roothub_status bits */
#define RH_HS_LPS	     	0x00000001	/* local power status */
#define RH_HS_OCI	     		0x00000002	/* over current indicator */
#define RH_HS_DRWE	     0x00008000	/* device remote wakeup enable */
#define RH_HS_LPSC	     0x00010000	/* local power status change */
#define RH_HS_OCIC	     0x00020000	/* over current indicator change */
#define RH_HS_CRWE	     0x80000000	/* clear remote wakeup enable */

/* roothub_desc_b masks */
#define RH_B_DR		0x0000ffff	/* device removable flags */
#define RH_B_PPCM	0xffff0000	/* port power control mask */

/* roothub_desc_a masks */
#define RH_A_NDP	(0xff << 0)	/* number of downstream ports */
#define RH_A_PSM	(1 << 8)	/* power switching mode */
#define RH_A_NPS	(1 << 9)	/* no power switching */
#define RH_A_DT		(1 << 10)	/* device type (mbz) */
#define RH_A_OCPM	(1 << 11)	/* over current protection mode */
#define RH_A_NOCP	(1 << 12)	/* no over current protection */
#define RH_A_POTPGT	(0xff << 24)	/* power on to power good time */


/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* ehci: */
UINT32 halEhciStatusGet(void);
void halEhciStatusSet(UINT32 value);
UINT32 halEhciIntEnableGet(void);
void halEhciIntEnableSet(UINT32 value);
UINT32 halEhciCommandGet(void);
void halEhciCommandSet(UINT32 value);
UINT32 halEhciPortStatusGet(UINT32 port);
void halEhciPortStatusSet(UINT32 port, UINT32 value);
void halEhciRegWriteUnBlock(void);
UINT32 halEhciHCHaltHandle(UINT32 usec);
UINT32 halEhciHCResetHandle(UINT32 usec);
UINT32 halEhciHCQuiesceHandle(UINT32 usec);
BOOL halEhciIsIAAHappen(void);
void halEhciIAAClear(void);
void halEhciAllPortOn(void);
void halEhciAllPortOff(void);
UINT32 halEhciHCSParamsGet(void);
UINT32 halEhciHCCParamsGet(void);
UINT32 halEhciCapBaseGet(void);
void halEhciFrameListSet(UINT32 address);
void halEhciAsyncListSet(UINT32 address);
UINT32 halEhciAsyncListGet(void);
void halEhciSegmentSet(UINT32 address);
void halEhciITCSet(UINT32 interval);
void halEhciParkModeSet(BOOL enable, UINT32 count);
void halEhciRun(void);
void halEhciStop(void);
void halEhciAsyncEnable(void);
void halEhciAsyncDisable(void);
UINT32 halEhciWaitAsyncEnableDone(UINT32 usec);
UINT32 halEhciWaitAsyncDisableDone(UINT32 usec);
void halEhciPeriodicEnable(void);
void halEhciPeriodicDisable(void);
UINT32 halEhciWaitPeriodicEnableDone(UINT32 usec);
UINT32 halEhciWaitPeriodicDisableDone(UINT32 usec);
void halEhciIAADEnable(void);
void halEhciIAADDisable(void);
void halEhciFrameSizeSet(UINT32 type);
void halEhciIntrsOn(void);
void halEhciIntrsOff(void);
UINT32 halEhciMicroFrameGet(void);
BOOL halEhciPortIsConnect(UINT32 port);
BOOL halEhciPortIsEnable(UINT32 port);
BOOL halEhciPortIsOverCurrent(UINT32 port);
BOOL halEhciPortIsResume(UINT32 port);
BOOL halEhciPortIsSuspend(UINT32 port);
BOOL halEhciPortIsReset(UINT32 port);
BOOL halEhciPortIsPower(UINT32 port);
BOOL halEhciPortIsOwn(UINT32 port);
BOOL halEhciPortIsUSB11(UINT32 port);
void halEhciPortDisable(UINT32 port);
void halEhciPortResumeStart(UINT32 port);
void halEhciPortResumeStop(UINT32 port);
void halEhciPortSuspend(UINT32 port);
void halEhciPortResetStart(UINT32 port);
void halEhciPortResetStop(UINT32 port);
void halEhciPortPowerOn(UINT32 port);
void halEhciPortPowerOff(UINT32 port);
void halEhciPortOwn(UINT32 port);
void halEhciPortOwnOff(UINT32 port);
BOOL halEhciPortIsConnectChange(UINT32 port);
BOOL halEhciPortIsEnableChange(UINT32 port);
BOOL halEhciPortIsOverCurrentChange(UINT32 port);
void halEhciPortConnectChangeClear(UINT32 port);
void halEhciPortEnableChangeClear(UINT32 port);
void halEhciPortOverCurrentChangeClear(UINT32 port);
void halEhciPortAllWakeOnEnable(UINT32 port);
void halEhciPortAllWakeOnDisable(UINT32 port);
void halEhciRegDump(void);

/* ohci: */
void halOhciRegWriteUnBlock(void);
UINT32 halOhciHCResetHandle(UINT32 usec);
void halOhciResumeFix(void);
UINT32 halOhciControlGet(void);
void halOhciControlSet(UINT32 value);
void halOhciRWCEnable(void);
void halOhciFuncStatReset(void);
BOOL halOhciFuncStatIsResume(void);
UINT32 halOhciFmIntervalGet(void);
void halOhciFmIntervalSet(UINT32 value);
UINT32 halOhciFmNumGet(void);
void halOhciCtrlHeadSet(UINT32 value);
UINT32 halOhciCtrlHeadGet(void);
void halOhciCtrlCurSet(UINT32 value);
void halOhciBulkHeadSet(UINT32 value);
void halOhciBulkCurSet(UINT32 value);
void halOhciHccaSet(UINT32 value);
UINT32 halOhciPeriodicStartGet(void);
void halOhciPeriodicStartSet(UINT32 value);
UINT32 halOhciIntStatusGet(void);
UINT32 halOhciIntEnableGet(void);
void halOhciIntEnable(UINT32 mask);
void halOhciIntDisable(UINT32 mask);
void halOhciIntClear(UINT32 mask);
void halOhciIntEnableMIE(void);
void halOhciIntDisableMIE(void);
void halOhciIntEnableWDH(void);
void halOhciIntDisableWDH(void);
void halOhciIntEnableSF(void);
void halOhciIntDisableSF(void);
void halOhciIntClearSF(void);
void halOhciIntClearAll(void);
UINT32 halOhciPortStatusGet(UINT32 port);
void halOhciPortStatusSet(UINT32 port, UINT32 value);
BOOL halOhciPortIsConnect(UINT32 port);
BOOL halOhciPortIsEnable(UINT32 port);
BOOL halOhciPortIsSuspend(UINT32 port);
BOOL halOhciPortIsReset(UINT32 port);
BOOL halOhciPortIsPower(UINT32 port);
BOOL halOhciPortIsLowSpeed(UINT32 port);
void halOhciPortDisable(UINT32 port);
void halOhciPortEnable(UINT32 port);
void halOhciPortSuspend(UINT32 port);
void halOhciPortResume(UINT32 port);
void halOhciPortReset(UINT32 port);
void halOhciPortPowerOn(UINT32 port);
void halOhciPortPowerOff(UINT32 port);
BOOL halOhciPortIsConnectChange(UINT32 port);
BOOL halOhciPortIsEnableChange(UINT32 port);
BOOL halOhciPortIsSuspendChange(UINT32 port);
BOOL halOhciPortIsResetChange(UINT32 port);
void halOhciPortConnectChangeClear(UINT32 port);
void halOhciPortEnableChangeClear(UINT32 port);
void halOhciPortSuspendChangeClear(UINT32 port);
void halOhciPortOverCurrentChangeClear(UINT32 port);
void halOhciPortResetChangeClear(UINT32 port);
void halOhciScheduleInit(void);
void halOhciCtrlListFilled(void);
void halOhciBulkListFilled(void);
UINT32 halOhciRootHubAGet(void);
void halOhciRootHubASet(UINT32 value);
UINT32 halOhciRootHubPortsNumGet(void);
UINT32 halOhciRootHubPOTPGTGet(void);
UINT32 halOhciRootHubBGet(void);
void halOhciRootHubBSet(UINT32 value);
UINT32 halOhciRootHubStsGet(void);
void halOhciRootHubStsSet(UINT32 value);
void halOhciRootHubOverCurrentClear(void);
void halOhciDump(void);
void halOhciRegDump(void);


/* phy */
extern UINT32 halUsbhostPhyEnter(void);
extern UINT32 halUsbhostPhyExit(void);
extern int halUsbhostPhyGet(void);
extern void halUsbPhySel(UINT8 physel);

/* power */
extern void halUsbhostPwrEnable(void);
extern void halUsbhostPwrDisable(void);

#ifdef __cplusplus
}
#endif

#endif /*_HAL_USBHOST_H_*/

