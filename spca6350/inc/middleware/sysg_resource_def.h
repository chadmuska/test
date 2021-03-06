/**************************************************************************
 *
 *       Copyright (c) 2005-2016 by iCatch Technology, Inc.
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
 *  Author:Stephen Hung                                                   *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 26010 $
$Date:: 2016-04-20 16:06:10#$
 */

#ifndef _SYSG_RESOURCE_DEF_H_
#define _SYSG_RESOURCE_DEF_H_

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* -- Resource arrangement of system -- */
/* ID for no free resource file*/


#if !defined(Partner_ApiGen1)
#define SYSG_RES_DECL(D,V,DEF_BIN_NAME) \
	D =V
	
#else
typedef struct tagSysg_res {
    UINT16 sysg_res_id;
	char * str; // selector string
} sysg_res_t;

#define SYSG_RES_DECL(D,V,DEF_BIN_NAME) \
	{ V, DEF_BIN_NAME }

#endif


#if !defined(Partner_ApiGen1)
typedef enum {
#else
sysg_res_t sysg_res_ary[] = {
#endif
	/* 3A/sensor */
	SYSG_RES_DECL(SYSG_RESOURCE_ID_EXPOSURE,0x01,"EXP"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_AGC,0x02,"AGC"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_AE_CFG,0x03,"AECFG"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_AWB_CTT,0x04,"AWBCTT"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_AWB_FILT,0x05,"AWBFILT"),

	/* misc */
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ADF_UDF,0x10,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_FONT,0x11,"FNT"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_STRING,0x12,"STR"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_DATE_STAMP_FONT,0x13,"STSMP"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON,0x14,"ICO"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_2,0x15,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_3,0x16,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_4,0x17,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_5,0x18,""),

	/* cdsp */
	SYSG_RES_DECL(SYSG_RESOURCE_ID_LSC,0x20,"LSC"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_CA,0x21,"CA"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ARD,0x22,"ARD"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ARD_CORNER,0x23,"ARDCN"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_R_GAMMA,0x24,"RGAMMA"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_G_GAMMA,0x25,"GGAMMA"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_B_GAMMA,0x26,"BGAMMA"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_3D_LUT,0x27,"LUT3D"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_BP_VIEW,0x28,"BPX"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_BP_CAPTURE,0x29,"BPX"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_PURPLE_FRINGE,0x2A,"CFC"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_HUE,0x2B,"HCORR"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_Y_GAMMA,0x2C,"YGAMMA"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_WDR,0x2D,"WDR"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_AA_Y,0x2E,"AAY"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_AA_UV,0x2F,"AAUV"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_AHD_AA,0x30,"AHDGRY"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_AHD_CORNER_THR_Y,0x31,"AHDYCN"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_AHD_CORNER_THR_UV,0x32,"AHDUVCN"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_YNF,0x33,"YNFYEF"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_Y_EDGE_GAIN,0x34,"EDG"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_Y_EDGE_OSC,0x35,"EDGOSC"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_Y_CORNER_EDGE,0x36,"CEDG"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_LDC,0x37,"LDC"),

	/* cdsp redo */
	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_PURPLE_FRINGE,0x60,"CFC2"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_HUE,0x61,"HCORR2"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_Y_GAMMA,0x62,"YGAMMA2"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_WDR,0x63,"WDR2"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_AA_Y,0x64,"AAY2"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_AA_UV,0x65,"AAUV2"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_AHD_AA,0x66,"AHDGRY2"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_AHD_CORNER_THR_Y,0x67,"AHDYCN2"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_AHD_CORNER_THR_UV,0x68,"AHDUVCN2"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_YNF,0x69,"YNFYEF2"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_Y_EDGE_GAIN,0x6A,"EDG2"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_Y_EDGE_OSC,0x6B,"EDGOSC2"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_REDO_Y_CORNER_EDGE,0x6C,"CEDG2"),

	/* cdsp Sdo */
	SYSG_RES_DECL(SYSG_RESOURCE_ID_SDO_PURPLE_FRINGE,0x6D,"CFC3"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_SDO_HUE,0x6E,"HCORR3"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_SDO_Y_GAMMA,0x6F,"YGAMMA3"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_SDO_WDR,0x70,"WDR3"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_SDO_AA_Y,0x71,"AAY3"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_SDO_AA_UV,0x72,"AAUV3"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_SDO_YNF,0x73,"YNFYEF3"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_SDO_Y_EDGE_GAIN,0x74,"EDG3"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_SDO_Y_EDGE_OSC,0x75,"EDGOSC3"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_SDO_Y_CORNER_EDGE,0x76,"CEDG3"),

	/* cdsp S2do */
	SYSG_RES_DECL(SYSG_RESOURCE_ID_S2DO_PURPLE_FRINGE,0x77,"CFC4"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_S2DO_HUE,0x78,"HCORR4"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_S2DO_Y_GAMMA,0x79,"YGAMMA4"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_S2DO_WDR,0x7A,"WDR4"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_S2DO_AA_Y,0x7B,"AAY4"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_S2DO_AA_UV,0x7C,"AAUV4"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_S2DO_YNF,0x7D,"YNFYEF4"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_S2DO_Y_EDGE_GAIN,0x7E,"EDG4"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_S2DO_Y_EDGE_OSC,0x7F,"EDGOSC4"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_S2DO_Y_CORNER_EDGE,0x80,"CEDG4"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_DIGI_EFF,0xFF,"DEFF"),
	
	SYSG_RES_DECL(SYSG_RESOURCE_ID_API0,0x100,"API0"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_API1,0x101,"API1"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_OB,0x110,"OB"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_CCM,0x111,"CCM"),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_CCM2,0x112,"CCM2"),

	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_6,0x120,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_7,0x121,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_8,0x122,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_9,0x123,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_10,0x124,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_11,0x125,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_12,0x126,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_13,0x127,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_14,0x128,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_15,0x129,""),
	SYSG_RES_DECL(SYSG_RESOURCE_ID_ICON_16,0x12A,""),
	
	SYSG_RES_DECL(SYSG_RESOURCE_ID_END,0x400,""),
}
#if !defined(Partner_ApiGen1)
sysg_res_e
#endif
;

#undef SYSG_RES_DECL

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#endif /* _SYSG_RESOURCE_DEF_H_ */
