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
/* Created at May,02,2018 08:31:55 */
#include "app_menu_event_17dvr.h"
#include "app_icon_def_17dvr.h"


UINT16 CustomIconPtr[] = 
{
	ID_ICON_MODE_STILL,	ID_ICON_NONE,	ID_ICON_RESOLUTION_4M,	ID_ICON_NONE,
	ID_ICON_QUALITY_SUPER_FINE,	ID_ICON_QUALITY_FINE,	ID_ICON_QUALITY,	ID_ICON_NONE,
	ID_ICON_OFF,	ID_ICON_BURST_3,	ID_ICON_BURST_5,	ID_ICON_NONE,
	ID_ICON_OFF,	ID_ICON_PRINT_DATE,	ID_ICON_PRINT_DATEANDTIME,	ID_ICON_MODE_VIDEO,
	ID_ICON_NONE,	ID_ICON_MOVIE_SIZE_1080P,	ID_ICON_MOVIE_SIZE_1080P,	ID_ICON_MOVIE_SIZE_720P,
	ID_ICON_MODE_VIDEO,	ID_ICON_MODE_VIDEO,	ID_ICON_NONE,	ID_ICON_QUALITY_SUPER_FINE,
	ID_ICON_QUALITY_FINE,	ID_ICON_QUALITY,	ID_ICON_NONE,	ID_ICON_OFF,
	ID_ICON_1MIN,	ID_ICON_3MIN,	ID_ICON_5MIN,	ID_ICON_NONE,
	ID_ICON_OFF,	ID_ICON_PRINT_DATE,	ID_ICON_PRINT_DATEANDTIME,	ID_ICON_NONE,
	ID_ICON_OFF,	ID_ICON_ON,	ID_ICON_MODE_PLAYBACK,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_MODE_SETUP,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_WB_AUTO,	ID_ICON_WB_DAYLIGHT,	ID_ICON_WB_CLOUDY,	ID_ICON_WB_TUNGSTEN,
	ID_ICON_WB_FLUORESCENT_H,	ID_ICON_NONE,	ID_ICON_FREQUENCY_50,	ID_ICON_FREQUENCY_60,
	ID_ICON_NONE,	ID_ICON_OFF,	ID_ICON_NONE,	ID_ICON_NONE,
	ID_ICON_OFF,	ID_ICON_ON,	ID_ICON_NONE,	ID_ICON_OFF,
	ID_ICON_PERIODICAL_CAP_5S,	ID_ICON_PERIODICAL_CAP_10S,	ID_ICON_NONE,	ID_ICON_NONE,
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
	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_NONE,	ID_ICON_LANGUAGE_EN,
	ID_ICON_LANGUAGE_CS,	ID_ICON_LANGUAGE_CT,	ID_ICON_NONE,	ID_ICON_TV_MODE_NTSC,
	ID_ICON_TV_MODE_PAL,	ID_ICON_NONE,	ID_ICON_USB_COMPUTER,	ID_ICON_USB_CAMERA,
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
};

UINT32 CustomMenuIndex[] = 
{
	0x01000000,	0x01010000,	0x01010100,	0x01020000,	0x01020100,	0x01020200,	0x01020300,	0x01030000,
	0x01030100,	0x01030200,	0x01030300,	0x01040000,	0x01040100,	0x01040200,	0x01040300,	0x02000000,
	0x02010000,	0x02010100,	0x02010200,	0x02010300,	0x02010400,	0x02010500,	0x02020000,	0x02020100,
	0x02020200,	0x02020300,	0x02030000,	0x02030100,	0x02030200,	0x02030300,	0x02030400,	0x02040000,
	0x02040100,	0x02040200,	0x02040300,	0x02050000,	0x02050100,	0x02050200,	0x03000000,	0x03010000,
	0x03010100,	0x03010101,	0x03010102,	0x03010200,	0x03010201,	0x03010202,	0x03010300,	0x03010301,
	0x03010302,	0x03020000,	0x03020100,	0x03020200,	0x04000000,	0x04010000,	0x04010100,	0x04010101,
	0x04010102,	0x04010200,	0x04010201,	0x04010202,	0x04010300,	0x04010301,	0x04010302,	0x04010400,
	0x04010401,	0x04010402,	0x04010403,	0x04010404,	0x04010500,	0x04010501,	0x04010502,	0x04020000,
	0x04020100,	0x04020200,	0x04020300,	0x04020400,	0x04020500,	0x04030000,	0x04030100,	0x04030200,
	0x04040000,	0x04040100,	0x04040200,	0x04050000,	0x04050100,	0x04050200,	0x04060000,	0x04060100,
	0x04060200,	0x04060300,	0x04070000,	0x04070100,	0x04070101,	0x04070102,	0x04070103,	0x04070104,
	0x04070105,	0x04070106,	0x04070107,	0x04070108,	0x04070109,	0x0407010a,	0x0407010b,	0x0407010c,
	0x0407010d,	0x0407010e,	0x0407010f,	0x04070110,	0x04070111,	0x04070112,	0x04070113,	0x04070114,
	0x04070115,	0x04070116,	0x04070117,	0x04070118,	0x04070119,	0x0407011a,	0x0407011b,	0x0407011c,
	0x0407011d,	0x0407011e,	0x0407011f,	0x04070120,	0x04070121,	0x04070122,	0x04070123,	0x04070124,
	0x04070125,	0x04070126,	0x04070127,	0x04070128,	0x04070129,	0x0407012a,	0x0407012b,	0x0407012c,
	0x0407012d,	0x0407012e,	0x0407012f,	0x04070130,	0x04070131,	0x04070132,	0x04070133,	0x04070200,
	0x04070201,	0x04070202,	0x04070203,	0x04070204,	0x04070205,	0x04070206,	0x04070207,	0x04070208,
	0x04070209,	0x0407020a,	0x0407020b,	0x0407020c,	0x04070300,	0x04070301,	0x04070302,	0x04070303,
	0x04070304,	0x04070305,	0x04070306,	0x04070307,	0x04070308,	0x04070309,	0x0407030a,	0x0407030b,
	0x0407030c,	0x0407030d,	0x0407030e,	0x0407030f,	0x04070310,	0x04070311,	0x04070312,	0x04070313,
	0x04070314,	0x04070315,	0x04070316,	0x04070317,	0x04070318,	0x04070319,	0x0407031a,	0x0407031b,
	0x0407031c,	0x0407031d,	0x0407031e,	0x0407031f,	0x04070400,	0x04070401,	0x04070402,	0x04070403,
	0x04070404,	0x04070405,	0x04070406,	0x04070407,	0x04070408,	0x04070409,	0x0407040a,	0x0407040b,
	0x0407040c,	0x0407040d,	0x0407040e,	0x0407040f,	0x04070410,	0x04070411,	0x04070412,	0x04070413,
	0x04070414,	0x04070415,	0x04070416,	0x04070417,	0x04070418,	0x04070500,	0x04070501,	0x04070502,
	0x04070503,	0x04070504,	0x04070505,	0x04070506,	0x04070507,	0x04070508,	0x04070509,	0x0407050a,
	0x0407050b,	0x0407050c,	0x0407050d,	0x0407050e,	0x0407050f,	0x04070510,	0x04070511,	0x04070512,
	0x04070513,	0x04070514,	0x04070515,	0x04070516,	0x04070517,	0x04070518,	0x04070519,	0x0407051a,
	0x0407051b,	0x0407051c,	0x0407051d,	0x0407051e,	0x0407051f,	0x04070520,	0x04070521,	0x04070522,
	0x04070523,	0x04070524,	0x04070525,	0x04070526,	0x04070527,	0x04070528,	0x04070529,	0x0407052a,
	0x0407052b,	0x0407052c,	0x0407052d,	0x0407052e,	0x0407052f,	0x04070530,	0x04070531,	0x04070532,
	0x04070533,	0x04070534,	0x04070535,	0x04070536,	0x04070537,	0x04070538,	0x04070539,	0x0407053a,
	0x0407053b,	0x0407053c,	0x04070600,	0x04070601,	0x04070602,	0x04070603,	0x04080000,	0x04080100,
	0x04080200,	0x04080300,	0x04090000,	0x04090100,	0x04090200,	0x040a0000,	0x040a0100,	0x040a0200,
	0x040b0000,	0x040b0100,	0x040b0200,	0x040c0000,	0x040c0100,	0x040c0200,	0x05000000,	0x05010000,
	0x05020000,	0x05030000,	0x05040000,	0x05050000,	0x05060000,	0x05070000,	0x05080000,	0x05090000,
	0x050a0000,	0x050b0000,	0x050c0000,	0x050d0000,	0x050e0000,	0x050f0000,	0x05100000,	0x05110000,
	0x05120000,	0x05130000,	0x05140000,	0x06000000,	0x06010000,	0x06010100,	0x06010101,	0x06010102,
	0x06010200,	0x06010201,	0x06010202,	0x06010300,	0x06010301,	0x06010302,	0x06010400,	0x06010401,
	0x06010402,	0x06010500,	0x06010501,	0x06010502,	0x06010503,	0x06010600,	0x06010601,	0x06010602,
	0x06010603,	0x07000000,	0x07010000,	0x07020000,	0x07030000,	0x08000000,	0x08010000,	0x08020000,
	0x08030000,	0x08040000,	0x08050000,	0x08060000,	0x08060100,	0x08060200,	0x08060300,	0x08060400,
	0x08060500,	0x08070000,	0x08080000,	0x08080100,	0x08080200,	0x08080300,	0x08080400,	0x08090000,
	0x080a0000,	0x080b0000,	0x080c0000,	0x080c0100,	0x080c0200,	0x080c0300,	0x080c0400,	0x080c0500,
	0x080c0600,	0x080c0700,	0x080d0000,	0x080d0100,	0x080d0200,	0x080d0201,	0x080d0202,	0x080d0203,
	0x080d0204,	0x080d0205,	0x080d0206,	0x080d0300,	0x080d0400,	0x080d0500,	0x080d0600,	0x080d0700,
	0x080d0701,	0x080d0702,	0x080d0703,	0x080d0704,	0x080d0705,	0x080d0706,	0x080d0800,	0x080d0801,
	0x080d0802,	0x080d0803,	0x080d0804,	0x080d0900,	0x080d0a00,	0x080d0b00,	0x080e0000,	0x080f0000,
	0x08100000,	0x08110000,	0x08120000,	0x08130000,
};

UINT8 CustomMenuNodeItem[] = 
{
	0x08,	0x04,	0x01,	0x04,	0x03,	0x03,	0x03,	0x04,
	0x03,	0x03,	0x03,	0x04,	0x03,	0x03,	0x03,	0x08,
	0x05,	0x05,	0x05,	0x05,	0x05,	0x05,	0x05,	0x03,
	0x03,	0x03,	0x05,	0x04,	0x04,	0x04,	0x04,	0x05,
	0x03,	0x03,	0x03,	0x05,	0x02,	0x02,	0x08,	0x02,
	0x03,	0x02,	0x02,	0x03,	0x02,	0x02,	0x03,	0x02,
	0x02,	0x02,	0x02,	0x02,	0x08,	0x0c,	0x05,	0x02,
	0x02,	0x05,	0x02,	0x02,	0x05,	0x02,	0x02,	0x05,
	0x04,	0x04,	0x04,	0x04,	0x05,	0x02,	0x02,	0x0c,
	0x05,	0x05,	0x05,	0x05,	0x05,	0x0c,	0x02,	0x02,
	0x0c,	0x02,	0x02,	0x0c,	0x02,	0x02,	0x0c,	0x03,
	0x03,	0x03,	0x0c,	0x06,	0x33,	0x33,	0x33,	0x33,
	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,
	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,
	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,
	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,
	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,
	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x33,	0x06,
	0x0c,	0x0c,	0x0c,	0x0c,	0x0c,	0x0c,	0x0c,	0x0c,
	0x0c,	0x0c,	0x0c,	0x0c,	0x06,	0x1f,	0x1f,	0x1f,
	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,
	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,
	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,	0x1f,
	0x1f,	0x1f,	0x1f,	0x1f,	0x06,	0x18,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,	0x18,	0x06,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,	0x3c,
	0x3c,	0x3c,	0x06,	0x03,	0x03,	0x03,	0x0c,	0x03,
	0x03,	0x03,	0x0c,	0x02,	0x02,	0x0c,	0x02,	0x02,
	0x0c,	0x02,	0x02,	0x0c,	0x02,	0x02,	0x08,	0x14,
	0x14,	0x14,	0x14,	0x14,	0x14,	0x14,	0x14,	0x14,
	0x14,	0x14,	0x14,	0x14,	0x14,	0x14,	0x14,	0x14,
	0x14,	0x14,	0x14,	0x08,	0x01,	0x06,	0x02,	0x02,
	0x06,	0x02,	0x02,	0x06,	0x02,	0x02,	0x06,	0x02,
	0x02,	0x06,	0x03,	0x03,	0x03,	0x06,	0x03,	0x03,
	0x03,	0x08,	0x03,	0x03,	0x03,	0x08,	0x13,	0x13,
	0x13,	0x13,	0x13,	0x13,	0x05,	0x05,	0x05,	0x05,
	0x05,	0x13,	0x13,	0x04,	0x04,	0x04,	0x04,	0x13,
	0x13,	0x13,	0x13,	0x07,	0x07,	0x07,	0x07,	0x07,
	0x07,	0x07,	0x13,	0x0b,	0x0b,	0x06,	0x06,	0x06,
	0x06,	0x06,	0x06,	0x0b,	0x0b,	0x0b,	0x0b,	0x0b,
	0x06,	0x06,	0x06,	0x06,	0x06,	0x06,	0x0b,	0x04,
	0x04,	0x04,	0x04,	0x0b,	0x0b,	0x0b,	0x13,	0x13,
	0x13,	0x13,	0x13,	0x13,
};

UINT8 CustomMenuSubItem[] = 
{
	0x04,	0x01,	0x00,	0x03,	0x00,	0x00,	0x00,	0x03,
	0x00,	0x00,	0x00,	0x03,	0x00,	0x00,	0x00,	0x05,
	0x05,	0x00,	0x00,	0x00,	0x00,	0x00,	0x03,	0x00,
	0x00,	0x00,	0x04,	0x00,	0x00,	0x00,	0x00,	0x03,
	0x00,	0x00,	0x00,	0x02,	0x00,	0x00,	0x02,	0x03,
	0x02,	0x00,	0x00,	0x02,	0x00,	0x00,	0x02,	0x00,
	0x00,	0x02,	0x00,	0x00,	0x0c,	0x05,	0x02,	0x00,
	0x00,	0x02,	0x00,	0x00,	0x02,	0x00,	0x00,	0x04,
	0x00,	0x00,	0x00,	0x00,	0x02,	0x00,	0x00,	0x05,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x02,	0x00,	0x00,
	0x02,	0x00,	0x00,	0x02,	0x00,	0x00,	0x03,	0x00,
	0x00,	0x00,	0x06,	0x33,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x0c,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x1f,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x18,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x3c,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x03,	0x00,	0x00,	0x00,	0x03,	0x00,
	0x00,	0x00,	0x02,	0x00,	0x00,	0x02,	0x00,	0x00,
	0x02,	0x00,	0x00,	0x02,	0x00,	0x00,	0x14,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x01,	0x06,	0x02,	0x00,	0x00,
	0x02,	0x00,	0x00,	0x02,	0x00,	0x00,	0x02,	0x00,
	0x00,	0x03,	0x00,	0x00,	0x00,	0x03,	0x00,	0x00,
	0x00,	0x03,	0x00,	0x00,	0x00,	0x13,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x05,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x04,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x07,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x0b,	0x00,	0x06,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x06,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x04,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
};


menuexe CustomMenuExe[] = 
{
	appMenuEvt_ModeStill,	appMenuEvt_FisrstItemProcess,	appMenuEvt_ImageSizeSet,	NULL,
	appMenuEvt_ImageQualitySet,	appMenuEvt_ImageQualitySet,	appMenuEvt_ImageQualitySet,	NULL,
	appMenuEvt_PhotoBurstSet,	appMenuEvt_PhotoBurstSet,	appMenuEvt_PhotoBurstSet,	appMenuEvt_LastItemProcess,
	appMenuEvt_StillStampModeSet,	appMenuEvt_StillStampModeSet,	appMenuEvt_StillStampModeSet,	appMenuEvt_ModeVideo,
	appMenuEvt_FisrstItemProcess,	appMenuEvt_VideoSizeSet,	appMenuEvt_VideoSizeSet,	appMenuEvt_VideoSizeSet,
	appMenuEvt_VideoSizeSet,	appMenuEvt_VideoSizeSet,	NULL,	appMenuEvt_VideoQualitySet,
	appMenuEvt_VideoQualitySet,	appMenuEvt_VideoQualitySet,	NULL,	appMenuEvt_SeamlessSet,
	appMenuEvt_SeamlessSet,	appMenuEvt_SeamlessSet,	appMenuEvt_SeamlessSet,	NULL,
	appMenuEvt_VideoStampModeSet,	appMenuEvt_VideoStampModeSet,	appMenuEvt_VideoStampModeSet,	appMenuEvt_LastItemProcess,
	appMenuEvt_VideoEISModeSet,	appMenuEvt_VideoEISModeSet,	appMenuEvt_ModePlayback,	appMenuEvt_FisrstItemProcess,
	appMenuEvt_PbFileProtect,	NULL,	NULL,	appMenuEvt_PbFileProtect,
	NULL,	NULL,	appMenuEvt_PbFileProtect,	NULL,
	NULL,	appMenuEvt_LastItemProcess,	appMenuEvt_PbFileDelete,	appMenuEvt_PbFileDelete,
	appMenuEvt_ModeSetup,	appMenuEvt_SoundInit,	appMenuEvt_SoundSet,	NULL,
	NULL,	appMenuEvt_SoundSet,	NULL,	NULL,
	appMenuEvt_SoundSet,	NULL,	NULL,	appMenuEvt_SoundSet,
	NULL,	NULL,	NULL,	NULL,
	appMenuEvt_SoundSet,	NULL,	NULL,	NULL,
	appMenuEvt_VideoWBModeSet,	appMenuEvt_VideoWBModeSet,	appMenuEvt_VideoWBModeSet,	appMenuEvt_VideoWBModeSet,
	appMenuEvt_VideoWBModeSet,	NULL,	appMenuEvt_LightFreqSet,	appMenuEvt_LightFreqSet,
	NULL,	appMenuEvt_SleepTimeSet,	appMenuEvt_SleepTimeSet,	NULL,
	appMenuEvt_powerOnRecordSet,	appMenuEvt_powerOnRecordSet,	NULL,	appMenuEvt_PeriodicalCapSet,
	appMenuEvt_PeriodicalCapSet,	appMenuEvt_PeriodicalCapSet,	appMenuEvt_RTCInit,	appMenuEvt_RTCSet,
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
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	appMenuEvt_RTCSet,	NULL,	NULL,	NULL,
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
	NULL,	NULL,	NULL,	NULL,
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
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	appMenuEvt_RTCSet,	NULL,
	NULL,	NULL,	NULL,	appMenuEvt_LanguageSet,
	appMenuEvt_LanguageSet,	appMenuEvt_LanguageSet,	NULL,	appMenuEvt_TVModeSet,
	appMenuEvt_TVModeSet,	NULL,	appMenuEvt_USBModeSet,	appMenuEvt_USBModeSet,
	NULL,	appMenuEvt_Format,	appMenuEvt_Format,	appMenuEvt_LastItemProcess,
	appMenuEvt_ResetAll,	appMenuEvt_ResetAll,	NULL,	appMenuEvt_SceneModeSet,
	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,
	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,
	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,
	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,
	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	appMenuEvt_SceneModeSet,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	appMenuEvt_USBModeSet,	appMenuEvt_USBModeSet,
	appMenuEvt_USBModeSet,	NULL,	appMenuEvt_FileTypeSet,	appMenuEvt_FileTypeSet,
	appMenuEvt_FileTypeSet,	NULL,	NULL,	NULL,
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
};


menufunc CustomMenuFunc[] = 
{
	NULL,	appMenuAct_ImageSize,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	appMenuAct_VideoSizeSet,	NULL,	NULL,
	appMenuAct_VideoSizeSet,	appMenuAct_VideoSizeSet,	NULL,	NULL,
	NULL,	NULL,	appMenuAct_VideoSeamless,	NULL,
	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	appMenuAct_EISMode,
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
	NULL,	NULL,	appMenuAct_TVMode,	NULL,
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
};

UINT16 CustomMenuParaTab[] = 
{
	0x00,	0x01,	0x01,	0x02,
	0x02,	0x02,	0x02,	0x03,
	0x03,	0x03,	0x03,	0x04,
	0x04,	0x04,	0x04,	0x00,
	0x05,	0x05,	0x05,	0x05,
	0x05,	0x05,	0x06,	0x06,
	0x06,	0x06,	0x07,	0x07,
	0x07,	0x07,	0x07,	0x08,
	0x08,	0x08,	0x08,	0x09,
	0x09,	0x09,	0x00,	0x00,
	0x0a,	0x00,	0x00,	0x0a,
	0x00,	0x00,	0x0a,	0x00,
	0x00,	0x00,	0x0b,	0x0b,
	0x00,	0x00,	0x0c,	0x0c,
	0x0c,	0x0d,	0x0d,	0x0d,
	0x0e,	0x0e,	0x0e,	0x0f,
	0x0f,	0x0f,	0x0f,	0x0f,
	0x10,	0x10,	0x10,	0x11,
	0x11,	0x11,	0x11,	0x11,
	0x11,	0x12,	0x12,	0x12,
	0x13,	0x13,	0x13,	0x14,
	0x14,	0x14,	0x15,	0x15,
	0x15,	0x15,	0x00,	0x16,
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
	0x17,	0x17,	0x17,	0x17,
	0x17,	0x17,	0x17,	0x17,
	0x17,	0x17,	0x17,	0x17,
	0x18,	0x18,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,
	0x18,	0x18,	0x18,	0x18,
	0x19,	0x19,	0x19,	0x19,
	0x19,	0x19,	0x19,	0x19,
	0x19,	0x19,	0x19,	0x19,
	0x19,	0x19,	0x19,	0x19,
	0x19,	0x19,	0x19,	0x19,
	0x19,	0x19,	0x19,	0x19,
	0x19,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1a,	0x1a,
	0x1a,	0x1a,	0x1b,	0x1b,
	0x1b,	0x1b,	0x1c,	0x1c,
	0x1c,	0x1c,	0x1d,	0x1d,
	0x1d,	0x1e,	0x1e,	0x1e,
	0x00,	0x1f,	0x1f,	0x00,
	0x00,	0x00,	0x00,	0x20,
	0x20,	0x20,	0x20,	0x20,
	0x20,	0x20,	0x20,	0x20,
	0x20,	0x20,	0x20,	0x20,
	0x20,	0x20,	0x20,	0x20,
	0x20,	0x20,	0x20,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,
	0x00,	0x1e,	0x1e,	0x1e,
	0x1e,	0x21,	0x21,	0x21,
	0x21,	0x00,	0x00,	0x00,
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
};

UINT32 CustomMenuRootStart[] = 
{
	0x0000,
	0x000f,
	0x0026,
	0x0034,
	0x0126,
	0x013b,
	0x0151,
	0x0155,
	0x0194,
};

