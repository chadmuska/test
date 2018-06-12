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
/* Created at January,08,2018 05:42:37 */
#include "app_menu_event_dual2.h"
#include "app_icon_def_dual2.h"


UINT16 CustomIconPtr[] = 
{
	ID_ICON_MODE_STILL,	ID_ICON_NONE,	ID_ICON_RESOLUTION_4M,	ID_ICON_NONE,
	ID_ICON_QUALITY_SUPER_FINE,	ID_ICON_QUALITY_FINE,	ID_ICON_QUALITY,	ID_ICON_NONE,
	ID_ICON_OFF,	ID_ICON_PRINT_DATE,	ID_ICON_PRINT_DATEANDTIME,	ID_ICON_MODE_VIDEO,
	ID_ICON_NONE,	ID_ICON_MOVIE_SIZE_1080P,	ID_ICON_MOVIE_SIZE_1080P,	ID_ICON_MOVIE_SIZE_1080P,
	ID_ICON_NONE,	ID_ICON_QUALITY_SUPER_FINE,	ID_ICON_QUALITY_FINE,	ID_ICON_QUALITY,
	ID_ICON_NONE,	ID_ICON_OFF,	ID_ICON_1MIN,	ID_ICON_3MIN,
	ID_ICON_5MIN,	ID_ICON_NONE,	ID_ICON_OFF,	ID_ICON_PRINT_DATE,
	ID_ICON_PRINT_DATEANDTIME,	ID_ICON_MODE_PLAYBACK,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_MODE_SETUP,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_FREQUENCY_50,
	ID_ICON_FREQUENCY_60,	ID_ICON_NONE,	ID_ICON_OFF,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_OFF,	ID_ICON_ON,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_LANGUAGE_EN,	ID_ICON_LANGUAGE_CS,	ID_ICON_LANGUAGE_CT,	ID_ICON_NONE,
	ID_ICON_TV_MODE_NTSC,	ID_ICON_TV_MODE_PAL,	ID_ICON_NONE,	ID_ICON_USB_COMPUTER,
	ID_ICON_USB_CAMERA,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,
};

UINT32 CustomMenuIndex[] = 
{
	0x01000000,	0x01010000,	0x01010100,	0x01020000,	0x01020100,	0x01020200,	0x01020300,	0x01030000,
	0x01030100,	0x01030200,	0x01030300,	0x02000000,	0x02010000,	0x02010100,	0x02010200,	0x02010300,
	0x02020000,	0x02020100,	0x02020200,	0x02020300,	0x02030000,	0x02030100,	0x02030200,	0x02030300,
	0x02030400,	0x02040000,	0x02040100,	0x02040200,	0x02040300,	0x03000000,	0x03010000,	0x03010100,
	0x03010101,	0x03010102,	0x03010200,	0x03010201,	0x03010202,	0x03010300,	0x03010301,	0x03010302,
	0x03020000,	0x03020100,	0x03020101,	0x03020102,	0x03020200,	0x03020201,	0x03020202,	0x04000000,
	0x04010000,	0x04010100,	0x04010101,	0x04010102,	0x04010200,	0x04010201,	0x04010202,	0x04010300,
	0x04010301,	0x04010302,	0x04010400,	0x04010401,	0x04010402,	0x04010403,	0x04010404,	0x04010500,
	0x04010501,	0x04010502,	0x04020000,	0x04020100,	0x04020200,	0x04030000,	0x04030100,	0x04030200,
	0x04040000,	0x04040100,	0x04040200,	0x04050000,	0x04050100,	0x04050101,	0x04050102,	0x04050103,
	0x04050104,	0x04050105,	0x04050106,	0x04050107,	0x04050108,	0x04050109,	0x0405010a,	0x0405010b,
	0x0405010c,	0x0405010d,	0x0405010e,	0x0405010f,	0x04050110,	0x04050111,	0x04050112,	0x04050113,
	0x04050114,	0x04050115,	0x04050116,	0x04050117,	0x04050118,	0x04050119,	0x0405011a,	0x0405011b,
	0x0405011c,	0x0405011d,	0x0405011e,	0x0405011f,	0x04050120,	0x04050121,	0x04050122,	0x04050123,
	0x04050124,	0x04050125,	0x04050126,	0x04050127,	0x04050128,	0x04050129,	0x0405012a,	0x0405012b,
	0x0405012c,	0x0405012d,	0x0405012e,	0x0405012f,	0x04050130,	0x04050131,	0x04050132,	0x04050133,
	0x04050200,	0x04050201,	0x04050202,	0x04050203,	0x04050204,	0x04050205,	0x04050206,	0x04050207,
	0x04050208,	0x04050209,	0x0405020a,	0x0405020b,	0x0405020c,	0x04050300,	0x04050301,	0x04050302,
	0x04050303,	0x04050304,	0x04050305,	0x04050306,	0x04050307,	0x04050308,	0x04050309,	0x0405030a,
	0x0405030b,	0x0405030c,	0x0405030d,	0x0405030e,	0x0405030f,	0x04050310,	0x04050311,	0x04050312,
	0x04050313,	0x04050314,	0x04050315,	0x04050316,	0x04050317,	0x04050318,	0x04050319,	0x0405031a,
	0x0405031b,	0x0405031c,	0x0405031d,	0x0405031e,	0x0405031f,	0x04050400,	0x04050401,	0x04050402,
	0x04050403,	0x04050404,	0x04050405,	0x04050406,	0x04050407,	0x04050408,	0x04050409,	0x0405040a,
	0x0405040b,	0x0405040c,	0x0405040d,	0x0405040e,	0x0405040f,	0x04050410,	0x04050411,	0x04050412,
	0x04050413,	0x04050414,	0x04050415,	0x04050416,	0x04050417,	0x04050418,	0x04050500,	0x04050501,
	0x04050502,	0x04050503,	0x04050504,	0x04050505,	0x04050506,	0x04050507,	0x04050508,	0x04050509,
	0x0405050a,	0x0405050b,	0x0405050c,	0x0405050d,	0x0405050e,	0x0405050f,	0x04050510,	0x04050511,
	0x04050512,	0x04050513,	0x04050514,	0x04050515,	0x04050516,	0x04050517,	0x04050518,	0x04050519,
	0x0405051a,	0x0405051b,	0x0405051c,	0x0405051d,	0x0405051e,	0x0405051f,	0x04050520,	0x04050521,
	0x04050522,	0x04050523,	0x04050524,	0x04050525,	0x04050526,	0x04050527,	0x04050528,	0x04050529,
	0x0405052a,	0x0405052b,	0x0405052c,	0x0405052d,	0x0405052e,	0x0405052f,	0x04050530,	0x04050531,
	0x04050532,	0x04050533,	0x04050534,	0x04050535,	0x04050536,	0x04050537,	0x04050538,	0x04050539,
	0x0405053a,	0x0405053b,	0x0405053c,	0x04050600,	0x04050601,	0x04050602,	0x04050603,	0x04060000,
	0x04060100,	0x04060200,	0x04060300,	0x04070000,	0x04070100,	0x04070200,	0x04080000,	0x04080100,
	0x04080200,	0x04090000,	0x04090100,	0x04090200,	0x040a0000,	0x040a0100,	0x040a0200,	0x05000000,
	0x05010000,	0x05020000,	0x05030000,	0x05040000,	0x05050000,	0x05060000,	0x05070000,	0x05080000,
	0x05090000,	0x050a0000,	0x050b0000,	0x050c0000,	0x050d0000,	0x050e0000,	0x050f0000,	0x05100000,
	0x05110000,	0x05120000,	0x05130000,	0x05140000,	0x06000000,	0x06010000,	0x06010100,	0x06010101,
	0x06010102,	0x06010200,	0x06010201,	0x06010202,	0x06010300,	0x06010301,	0x06010302,	0x06010400,
	0x06010401,	0x06010402,	0x06010500,	0x06010501,	0x06010502,	0x06010503,	0x06010600,	0x06010601,
	0x06010602,	0x06010603,	0x07000000,	0x07010000,	0x07020000,	0x07030000,	0x08000000,	0x08010000,
	0x08020000,	0x08030000,	0x08040000,	0x08050000,	0x08060000,	0x08060100,	0x08060200,	0x08060300,
	0x08060400,	0x08060500,	0x08070000,	0x08080000,	0x08080100,	0x08080200,	0x08080300,	0x08080400,
	0x08090000,	0x080a0000,	0x080b0000,	0x080c0000,	0x080c0100,	0x080c0200,	0x080c0300,	0x080c0400,
	0x080c0500,	0x080c0600,	0x080c0700,	0x080d0000,	0x080d0100,	0x080d0200,	0x080d0201,	0x080d0202,
	0x080d0203,	0x080d0204,	0x080d0205,	0x080d0206,	0x080d0300,	0x080d0400,	0x080d0500,	0x080d0600,
	0x080d0700,	0x080d0701,	0x080d0702,	0x080d0703,	0x080d0704,	0x080d0705,	0x080d0706,	0x080d0800,
	0x080d0801,	0x080d0802,	0x080d0803,	0x080d0804,	0x080d0900,	0x080d0a00,	0x080d0b00,	0x080e0000,
	0x080f0000,	0x08100000,	0x08110000,	0x08120000,	0x08130000,
};

UINT8 CustomMenuNodeItem[] = 
{
	0x08,	0x03,	0x01,	0x03,	0x03,	0x03,	0x03,	0x03,
	0x03,	0x03,	0x03,	0x08,	0x04,	0x03,	0x03,	0x03,
	0x04,	0x03,	0x03,	0x03,	0x04,	0x04,	0x04,	0x04,
	0x04,	0x04,	0x03,	0x03,	0x03,	0x08,	0x02,	0x03,
	0x02,	0x02,	0x03,	0x02,	0x02,	0x03,	0x02,	0x02,
	0x02,	0x02,	0x02,	0x02,	0x02,	0x02,	0x02,	0x08,
	0x0a,	0x05,	0x02,	0x02,	0x05,	0x02,	0x02,	0x05,
	0x02,	0x02,	0x05,	0x04,	0x04,	0x04,	0x04,	0x05,
	0x02,	0x02,	0x0a,	0x02,	0x02,	0x0a,	0x02,	0x02,
	0x0a,	0x02,	0x02,	0x0a,	0x06,	0x33,	0x33,	0x33,
	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,
	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,
	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,
	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,
	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,
	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,
	0x06,	0x0c,	0x0c,	0x0c,	0x0c,	0x0c,	0x0c,	0x0c,
	0x0c,	0x0c,	0x0c,	0x0c,	0x0c,	0x06,	0x1f,	0x1f,
	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,
	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,
	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,
	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x06,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,	0x06,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x06,	0x03,	0x03,	0x03,	0x0a,
	0x03,	0x03,	0x03,	0x0a,	0x02,	0x02,	0x0a,	0x02,
	0x02,	0x0a,	0x02,	0x02,	0x0a,	0x02,	0x02,	0x08,
	0x14,	0x14,	0x14,	0x14,	0x14,	0x14,	0x14,	0x14,
	0x14,	0x14,	0x14,	0x14,	0x14,	0x14,	0x14,	0x14,
	0x14,	0x14,	0x14,	0x14,	0x08,	0x01,	0x06,	0x02,
	0x02,	0x06,	0x02,	0x02,	0x06,	0x02,	0x02,	0x06,
	0x02,	0x02,	0x06,	0x03,	0x03,	0x03,	0x06,	0x03,
	0x03,	0x03,	0x08,	0x03,	0x03,	0x03,	0x08,	0x13,
	0x13,	0x13,	0x13,	0x13,	0x13,	0x05,	0x05,	0x05,
	0x05,	0x05,	0x13,	0x13,	0x04,	0x04,	0x04,	0x04,
	0x13,	0x13,	0x13,	0x13,	0x07,	0x07,	0x07,	0x07,
	0x07,	0x07,	0x07,	0x13,	0x0b,	0x0b,	0x06,	0x06,
	0x06,	0x06,	0x06,	0x06,	0x0b,	0x0b,	0x0b,	0x0b,
	0x0b,	0x06,	0x06,	0x06,	0x06,	0x06,	0x06,	0x0b,
	0x04,	0x04,	0x04,	0x04,	0x0b,	0x0b,	0x0b,	0x13,
	0x13,	0x13,	0x13,	0x13,	0x13,
};

UINT8 CustomMenuSubItem[] = 
{
	0x03,	0x01,	0x00,	0x03,	0x00,	0x00,	0x00,	0x03,
	0x00,	0x00,	0x00,	0x04,	0x03,	0x00,	0x00,	0x00,
	0x03,	0x00,	0x00,	0x00,	0x04,	0x00,	0x00,	0x00,
	0x00,	0x03,	0x00,	0x00,	0x00,	0x02,	0x03,	0x02,
	0x00,	0x00,	0x02,	0x00,	0x00,	0x02,	0x00,	0x00,
	0x02,	0x02,	0x00,	0x00,	0x02,	0x00,	0x00,	0x0a,
	0x05,	0x02,	0x00,	0x00,	0x02,	0x00,	0x00,	0x02,
	0x00,	0x00,	0x04,	0x00,	0x00,	0x00,	0x00,	0x02,
	0x00,	0x00,	0x02,	0x00,	0x00,	0x02,	0x00,	0x00,
	0x02,	0x00,	0x00,	0x06,	0x33,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x0c,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x1f,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x18,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x3c,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x03,	0x00,	0x00,	0x00,	0x03,
	0x00,	0x00,	0x00,	0x02,	0x00,	0x00,	0x02,	0x00,
	0x00,	0x02,	0x00,	0x00,	0x02,	0x00,	0x00,	0x14,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x01,	0x06,	0x02,	0x00,
	0x00,	0x02,	0x00,	0x00,	0x02,	0x00,	0x00,	0x02,
	0x00,	0x00,	0x03,	0x00,	0x00,	0x00,	0x03,	0x00,
	0x00,	0x00,	0x03,	0x00,	0x00,	0x00,	0x13,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x05,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x04,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x07,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x0b,	0x00,	0x06,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x06,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x04,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,
};


menuexe CustomMenuExe[] = 
{
	appMenuEvt_ModeStill,	appMenuEvt_FisrstItemProcess,	appMenuEvt_ImageSizeSet,	NULL,
	appMenuEvt_ImageQualitySet,	appMenuEvt_ImageQualitySet,	appMenuEvt_ImageQualitySet,	appMenuEvt_LastItemProcess,
	appMenuEvt_StillStampModeSet,	appMenuEvt_StillStampModeSet,	appMenuEvt_StillStampModeSet,	appMenuEvt_ModeVideo,
	appMenuEvt_FisrstItemProcess,	appMenuEvt_VideoSizeSet,	appMenuEvt_VideoSizeSet,	appMenuEvt_VideoSizeSet,
	NULL,	appMenuEvt_VideoQualitySet,	appMenuEvt_VideoQualitySet,	appMenuEvt_VideoQualitySet,
	NULL,	appMenuEvt_SeamlessSet,	appMenuEvt_SeamlessSet,	appMenuEvt_SeamlessSet,
	appMenuEvt_SeamlessSet,	appMenuEvt_LastItemProcess,	appMenuEvt_VideoStampModeSet,	appMenuEvt_VideoStampModeSet,
	appMenuEvt_VideoStampModeSet,	appMenuEvt_ModePlayback,	appMenuEvt_FisrstItemProcess,	appMenuEvt_PbFileProtect,
	NULL,	NULL,	appMenuEvt_PbFileProtect,	NULL,
	NULL,	appMenuEvt_PbFileProtect,	NULL,	NULL,
	appMenuEvt_LastItemProcess,	appMenuEvt_PbFileDelete,	NULL,	NULL,
	appMenuEvt_PbFileDelete,	NULL,	NULL,	appMenuEvt_ModeSetup,
	appMenuEvt_SoundInit,	appMenuEvt_SoundSet,	NULL,	NULL,
	appMenuEvt_SoundSet,	NULL,	NULL,	appMenuEvt_SoundSet,
	NULL,	NULL,	appMenuEvt_SoundSet,	NULL,
	NULL,	NULL,	NULL,	appMenuEvt_SoundSet,
	NULL,	NULL,	NULL,	appMenuEvt_LightFreqSet,
	appMenuEvt_LightFreqSet,	NULL,	appMenuEvt_SleepTimeSet,	appMenuEvt_SleepTimeSet,
	NULL,	appMenuEvt_powerOnRecordSet,	appMenuEvt_powerOnRecordSet,	appMenuEvt_RTCInit,
	appMenuEvt_RTCSet,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	appMenuEvt_RTCSet,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	appMenuEvt_RTCSet,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	appMenuEvt_RTCSet,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	appMenuEvt_RTCSet,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	appMenuEvt_RTCSet,
	NULL,	NULL,	NULL,	NULL,
	appMenuEvt_LanguageSet,	appMenuEvt_LanguageSet,	appMenuEvt_LanguageSet,	NULL,
	appMenuEvt_TVModeSet,	appMenuEvt_TVModeSet,	NULL,	appMenuEvt_USBModeSet,
	appMenuEvt_USBModeSet,	NULL,	appMenuEvt_Format,	appMenuEvt_Format,
	appMenuEvt_LastItemProcess,	appMenuEvt_ResetAll,	appMenuEvt_ResetAll,	NULL,
	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,
	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,
	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,
	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,
	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	appMenuEvt_USBModeSet,
	appMenuEvt_USBModeSet,	appMenuEvt_USBModeSet,	NULL,	appMenuEvt_FileTypeSet,
	appMenuEvt_FileTypeSet,	appMenuEvt_FileTypeSet,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,
};


menufunc CustomMenuFunc[] = 
{
	NULL,	appMenuAct_ImageSize,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	appMenuAct_VideoSizeSet,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	appMenuAct_TVMode,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,
};

UINT16 CustomMenuParaTab[] = 
{
	0x00,	0x01,	0x01,	0x02,
	0x02,	0x02,	0x02,	0x03,
	0x03,	0x03,	0x03,	0x00,
	0x04,	0x04,	0x04,	0x04,
	0x05,	0x05,	0x05,	0x05,
	0x06,	0x06,	0x06,	0x06,
	0x06,	0x07,	0x07,	0x07,
	0x07,	0x00,	0x00,	0x08,
	0x00,	0x00,	0x08,	0x00,
	0x00,	0x08,	0x00,	0x00,
	0x00,	0x09,	0x00,	0x00,
	0x09,	0x00,	0x00,	0x00,
	0x00,	0x0a,	0x0a,	0x0a,
	0x0b,	0x0b,	0x0b,	0x0c,
	0x0c,	0x0c,	0x0d,	0x0d,
	0x0d,	0x0d,	0x0d,	0x0e,
	0x0e,	0x0e,	0x0f,	0x0f,
	0x0f,	0x10,	0x10,	0x10,
	0x11,	0x11,	0x11,	0x00,
	0x12,	0x12,	0x12,	0x12,
	0x12,	0x12,	0x12,	0x12,
	0x12,	0x12,	0x12,	0x12,
	0x12,	0x12,	0x12,	0x12,
	0x12,	0x12,	0x12,	0x12,
	0x12,	0x12,	0x12,	0x12,
	0x12,	0x12,	0x12,	0x12,
	0x12,	0x12,	0x12,	0x12,
	0x12,	0x12,	0x12,	0x12,
	0x12,	0x12,	0x12,	0x12,
	0x12,	0x12,	0x12,	0x12,
	0x12,	0x12,	0x12,	0x12,
	0x12,	0x12,	0x12,	0x12,
	0x13,	0x13,	0x13,	0x13,
	0x13,	0x13,	0x13,	0x13,
	0x13,	0x13,	0x13,	0x13,
	0x13,	0x14,	0x14,	0x14,
	0x14,	0x14,	0x14,	0x14,
	0x14,	0x14,	0x14,	0x14,
	0x14,	0x14,	0x14,	0x14,
	0x14,	0x14,	0x14,	0x14,
	0x14,	0x14,	0x14,	0x14,
	0x14,	0x14,	0x14,	0x14,
	0x14,	0x14,	0x14,	0x14,
	0x14,	0x15,	0x15,	0x15,
	0x15,	0x15,	0x15,	0x15,
	0x15,	0x15,	0x15,	0x15,
	0x15,	0x15,	0x15,	0x15,
	0x15,	0x15,	0x15,	0x15,
	0x15,	0x15,	0x15,	0x15,
	0x15,	0x15,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x16,
	0x16,	0x16,	0x16,	0x17,
	0x17,	0x17,	0x17,	0x18,
	0x18,	0x18,	0x18,	0x19,
	0x19,	0x19,	0x1a,	0x1a,
	0x1a,	0x00,	0x1b,	0x1b,
	0x00,	0x00,	0x00,	0x00,
	0x1c,	0x1c,	0x1c,	0x1c,
	0x1c,	0x1c,	0x1c,	0x1c,
	0x1c,	0x1c,	0x1c,	0x1c,
	0x1c,	0x1c,	0x1c,	0x1c,
	0x1c,	0x1c,	0x1c,	0x1c,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1d,	0x1d,
	0x1d,	0x1d,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,
};

UINT32 CustomMenuRootStart[] = 
{
	0x0000,
	0x000b,
	0x001d,
	0x002f,
	0x0117,
	0x012c,
	0x0142,
	0x0146,
	0x0185,
};

