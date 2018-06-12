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
/* Created at January,08,2018 05:40:35 */
/* This file is created by tool, don't modify it!!! */

#ifndef _MENU_RES_EVB_H_
#define _MENU_RES_EVB_H_

[LANGUAGE]english:

[MARK]MENU_MARK_STILL:
"Still Record",		"mode_still.bmp",		[()appMenuEvt_ModeStill],
	"Resolution",		[appMenuAct_ImageSize(ImageSize)appMenuEvt_FisrstItemProcess],
		"4M",		"resolution_4m.bmp",		[(ImageSize)appMenuEvt_ImageSizeSet],
	"Quality",		[(ImageQuality)],
		"Super Fine",		"quality_super_fine.bmp",		[(ImageQuality)appMenuEvt_ImageQualitySet],
		"Fine",		"quality_fine.bmp",		[(ImageQuality)appMenuEvt_ImageQualitySet],
		"Normal",		"quality.bmp",		[(ImageQuality)appMenuEvt_ImageQualitySet],
	"Photo Burst",		[(PhotoBurst)],
		"Off",		"off.bmp",		[(PhotoBurst)appMenuEvt_PhotoBurstSet],
		"Burst LMT",		"burst_3.bmp",		[(PhotoBurst)appMenuEvt_PhotoBurstSet],
		"Burst SUST",		"burst_5.bmp",		[(PhotoBurst)appMenuEvt_PhotoBurstSet],
	"Date Stamp",		[(StillStampMode)appMenuEvt_LastItemProcess],
		"Off",		"off.bmp",		[(StillStampMode)appMenuEvt_StillStampModeSet],
		"Date",		"print_date.bmp",		[(StillStampMode)appMenuEvt_StillStampModeSet],
		"Date&Time",		"print_dateandtime.bmp",		[(StillStampMode)appMenuEvt_StillStampModeSet],
[MARK]MENU_MARK_VIDEO:
"VideoMenu",		"mode_video.bmp",		[()appMenuEvt_ModeVideo],
	"Movie Size",		[(VideoSize)appMenuEvt_FisrstItemProcess],
		"1080P(60)",		"movie_size_1080p.bmp",		[(VideoSize)appMenuEvt_VideoSizeSet],
		"1080P(30)",		"movie_size_1080p.bmp",		[appMenuAct_VideoSizeSet(VideoSize)appMenuEvt_VideoSizeSet],
		"720P(30)",		"movie_size_720p.bmp",		[(VideoSize)appMenuEvt_VideoSizeSet],
		"4K2K",		"mode_video.bmp",		[(VideoSize)appMenuEvt_VideoSizeSet],
		"2.7K",		"mode_video.bmp",		[(VideoSize)appMenuEvt_VideoSizeSet],
	"Quality",		[(VideoQuality)],
		"Super Fine",		"quality_super_fine.bmp",		[(VideoQuality)appMenuEvt_VideoQualitySet],
		"Fine",		"quality_fine.bmp",		[(VideoQuality)appMenuEvt_VideoQualitySet],
		"Normal",		"quality.bmp",		[(VideoQuality)appMenuEvt_VideoQualitySet],
	"Seamless",		[(Seamless)],
		"Off",		"off.bmp",		[(Seamless)appMenuEvt_SeamlessSet],
		"1 Min",		"1min.bmp",		[(Seamless)appMenuEvt_SeamlessSet],
		"3 Min",		"3min.bmp",		[(Seamless)appMenuEvt_SeamlessSet],
		"5 Min",		"5min.bmp",		[(Seamless)appMenuEvt_SeamlessSet],
	"Date Stamp",		[(VideoStampMode)appMenuEvt_LastItemProcess],
		"Off",		"off.bmp",		[(VideoStampMode)appMenuEvt_VideoStampModeSet],
		"Date",		"print_date.bmp",		[(VideoStampMode)appMenuEvt_VideoStampModeSet],
		"Date&Time",		"print_dateandtime.bmp",		[(VideoStampMode)appMenuEvt_VideoStampModeSet],
[MARK]MENU_MARK_PLAYBACK:
"Playback",		"mode_playback.bmp",		[()appMenuEvt_ModePlayback],
	"Protect",		[()appMenuEvt_FisrstItemProcess],
		"Single",		[(selImgProtect)appMenuEvt_PbFileProtect],
			"Yes",
			"No",
		"Protect All",		[(selImgProtect)appMenuEvt_PbFileProtect],
			"Yes",
			"No",
		"Unprotect All",		[(selImgProtect)appMenuEvt_PbFileProtect],
			"Yes",
			"No",
	"Delete",		[()appMenuEvt_LastItemProcess],
		"Single",		[(selImgDelete)appMenuEvt_PbFileDelete],
			"Yes",
			"No",
		"All",		[(selImgDelete)appMenuEvt_PbFileDelete],
			"Yes",
			"No",
[MARK]MENU_MARK_SETUP:
"Setup",		"mode_setup.bmp",		[()appMenuEvt_ModeSetup],
[MARK]MENU_MARK_SOUND:
	"Sounds",		[()appMenuEvt_SoundInit],
		"Shutter",		[(ShutterSound)appMenuEvt_SoundSet],
			"Off",		[(ShutterSound)],
			"On",		[(ShutterSound)],
		"Start-up",		[(StartupSound)appMenuEvt_SoundSet],
			"None",		[(StartupSound)],
			"1",		[(StartupSound)],
		"Beep",		[(BeepSound)appMenuEvt_SoundSet],
			"Off",		[(BeepSound)],
			"On",		[(BeepSound)],
		"Volume",		[(Volume)appMenuEvt_SoundSet],
			"0",		[(Volume)],
			"1",		[(Volume)],
			"2",		[(Volume)],
			"3",		[(Volume)],
		"Voice Rec",		[(VoiceRec)appMenuEvt_SoundSet],
			"Off",		[(VoiceRec)],
			"On",		[(VoiceRec)],
	"White Balance",		[(WBMode)],
		"Auto",		"wb_auto.bmp",		[(WBMode)appMenuEvt_VideoWBModeSet],
		"Daylight",		"wb_daylight.bmp",		[(WBMode)appMenuEvt_VideoWBModeSet],
		"Cloudy",		"wb_cloudy.bmp",		[(WBMode)appMenuEvt_VideoWBModeSet],
		"Tungsten",		"wb_tungsten.bmp",		[(WBMode)appMenuEvt_VideoWBModeSet],
		"Fluor H",		"wb_fluorescent_h.bmp",		[(WBMode)appMenuEvt_VideoWBModeSet],
[MARK]MENU_MARK_LIGHTFREQ:
	"Light Frequency",		[(LightFreq)],
		"50HZ",		"frequency_50.bmp",		[(LightFreq)appMenuEvt_LightFreqSet],
		"60HZ",		"frequency_60.bmp",		[(LightFreq)appMenuEvt_LightFreqSet],
[MARK]MENU_MARK_SlEEPTIME:
	"Power Save",		[(SleepTime)],
		"OFF",		"off.bmp",		[(SleepTime)appMenuEvt_SleepTimeSet],
		"1min.",		[(SleepTime)appMenuEvt_SleepTimeSet],
[MARK]MENU_MARK_POWERONREC:
	"powerOnRecord",		[(powerOnRecord)],
		"OFF",		"off.bmp",		[(powerOnRecord)appMenuEvt_powerOnRecordSet],
		"ON",		"on.bmp",		[(powerOnRecord)appMenuEvt_powerOnRecordSet],
	"Periodical Capture",		[(PeriodicalCap)],
		"off",		"off.bmp",		[(PeriodicalCap)appMenuEvt_PeriodicalCapSet],
		"5 sec",		"periodical_cap_5s.bmp",		[(PeriodicalCap)appMenuEvt_PeriodicalCapSet],
		"10 sec",		"periodical_cap_10s.bmp",		[(PeriodicalCap)appMenuEvt_PeriodicalCapSet],
[MARK]MENU_MARK_DATETIME:
	"Date & Time",		[()appMenuEvt_RTCInit],
		"Year",		[(rtcYear)appMenuEvt_RTCSet],
			"2000",		[(rtcYear)],
			"2001",		[(rtcYear)],
			"2002",		[(rtcYear)],
			"2003",		[(rtcYear)],
			"2004",		[(rtcYear)],
			"2005",		[(rtcYear)],
			"2006",		[(rtcYear)],
			"2007",		[(rtcYear)],
			"2008",		[(rtcYear)],
			"2009",		[(rtcYear)],
			"2010",		[(rtcYear)],
			"2011",		[(rtcYear)],
			"2012",		[(rtcYear)],
			"2013",		[(rtcYear)],
			"2014",		[(rtcYear)],
			"2015",		[(rtcYear)],
			"2016",		[(rtcYear)],
			"2017",		[(rtcYear)],
			"2018",		[(rtcYear)],
			"2019",		[(rtcYear)],
			"2020",		[(rtcYear)],
			"2021",		[(rtcYear)],
			"2022",		[(rtcYear)],
			"2023",		[(rtcYear)],
			"2024",		[(rtcYear)],
			"2025",		[(rtcYear)],
			"2026",		[(rtcYear)],
			"2027",		[(rtcYear)],
			"2028",		[(rtcYear)],
			"2029",		[(rtcYear)],
			"2030",		[(rtcYear)],
			"2031",		[(rtcYear)],
			"2032",		[(rtcYear)],
			"2033",		[(rtcYear)],
			"2034",		[(rtcYear)],
			"2035",		[(rtcYear)],
			"2036",		[(rtcYear)],
			"2037",		[(rtcYear)],
			"2038",		[(rtcYear)],
			"2039",		[(rtcYear)],
			"2040",		[(rtcYear)],
			"2041",		[(rtcYear)],
			"2042",		[(rtcYear)],
			"2043",		[(rtcYear)],
			"2044",		[(rtcYear)],
			"2045",		[(rtcYear)],
			"2046",		[(rtcYear)],
			"2047",		[(rtcYear)],
			"2048",		[(rtcYear)],
			"2049",		[(rtcYear)],
			"2050",		[(rtcYear)],
		"Month",		[(rtcMonth)appMenuEvt_RTCSet],
			"01",		[(rtcMonth)],
			"02",		[(rtcMonth)],
			"03",		[(rtcMonth)],
			"04",		[(rtcMonth)],
			"05",		[(rtcMonth)],
			"06",		[(rtcMonth)],
			"07",		[(rtcMonth)],
			"08",		[(rtcMonth)],
			"09",		[(rtcMonth)],
			"10",		[(rtcMonth)],
			"11",		[(rtcMonth)],
			"12",		[(rtcMonth)],
		"Day",		[(rtcDay)appMenuEvt_RTCSet],
			"01",		[(rtcDay)],
			"02",		[(rtcDay)],
			"03",		[(rtcDay)],
			"04",		[(rtcDay)],
			"05",		[(rtcDay)],
			"06",		[(rtcDay)],
			"07",		[(rtcDay)],
			"08",		[(rtcDay)],
			"09",		[(rtcDay)],
			"10",		[(rtcDay)],
			"11",		[(rtcDay)],
			"12",		[(rtcDay)],
			"13",		[(rtcDay)],
			"14",		[(rtcDay)],
			"15",		[(rtcDay)],
			"16",		[(rtcDay)],
			"17",		[(rtcDay)],
			"18",		[(rtcDay)],
			"19",		[(rtcDay)],
			"20",		[(rtcDay)],
			"21",		[(rtcDay)],
			"22",		[(rtcDay)],
			"23",		[(rtcDay)],
			"24",		[(rtcDay)],
			"25",		[(rtcDay)],
			"26",		[(rtcDay)],
			"27",		[(rtcDay)],
			"28",		[(rtcDay)],
			"29",		[(rtcDay)],
			"30",		[(rtcDay)],
			"31",		[(rtcDay)],
		"Hour",		[(rtcHour)appMenuEvt_RTCSet],
			"00",		[(rtcHour)],
			"01",		[(rtcHour)],
			"02",		[(rtcHour)],
			"03",		[(rtcHour)],
			"04",		[(rtcHour)],
			"05",		[(rtcHour)],
			"06",		[(rtcHour)],
			"07",		[(rtcHour)],
			"08",		[(rtcHour)],
			"09",		[(rtcHour)],
			"10",		[(rtcHour)],
			"11",		[(rtcHour)],
			"12",		[(rtcHour)],
			"13",		[(rtcHour)],
			"14",		[(rtcHour)],
			"15",		[(rtcHour)],
			"16",		[(rtcHour)],
			"17",		[(rtcHour)],
			"18",		[(rtcHour)],
			"19",		[(rtcHour)],
			"20",		[(rtcHour)],
			"21",		[(rtcHour)],
			"22",		[(rtcHour)],
			"23",		[(rtcHour)],
		"Minute",		[(rtcMinute)appMenuEvt_RTCSet],
			"00",		[(rtcMinute)],
			"01",		[(rtcMinute)],
			"02",		[(rtcMinute)],
			"03",		[(rtcMinute)],
			"04",		[(rtcMinute)],
			"05",		[(rtcMinute)],
			"06",		[(rtcMinute)],
			"07",		[(rtcMinute)],
			"08",		[(rtcMinute)],
			"09",		[(rtcMinute)],
			"10",		[(rtcMinute)],
			"11",		[(rtcMinute)],
			"12",		[(rtcMinute)],
			"13",		[(rtcMinute)],
			"14",		[(rtcMinute)],
			"15",		[(rtcMinute)],
			"16",		[(rtcMinute)],
			"17",		[(rtcMinute)],
			"18",		[(rtcMinute)],
			"19",		[(rtcMinute)],
			"20",		[(rtcMinute)],
			"21",		[(rtcMinute)],
			"22",		[(rtcMinute)],
			"23",		[(rtcMinute)],
			"24",		[(rtcMinute)],
			"25",		[(rtcMinute)],
			"26",		[(rtcMinute)],
			"27",		[(rtcMinute)],
			"28",		[(rtcMinute)],
			"29",		[(rtcMinute)],
			"30",		[(rtcMinute)],
			"31",		[(rtcMinute)],
			"32",		[(rtcMinute)],
			"33",		[(rtcMinute)],
			"34",		[(rtcMinute)],
			"35",		[(rtcMinute)],
			"36",		[(rtcMinute)],
			"37",		[(rtcMinute)],
			"38",		[(rtcMinute)],
			"39",		[(rtcMinute)],
			"40",		[(rtcMinute)],
			"41",		[(rtcMinute)],
			"42",		[(rtcMinute)],
			"43",		[(rtcMinute)],
			"44",		[(rtcMinute)],
			"45",		[(rtcMinute)],
			"46",		[(rtcMinute)],
			"47",		[(rtcMinute)],
			"48",		[(rtcMinute)],
			"49",		[(rtcMinute)],
			"50",		[(rtcMinute)],
			"51",		[(rtcMinute)],
			"52",		[(rtcMinute)],
			"53",		[(rtcMinute)],
			"54",		[(rtcMinute)],
			"55",		[(rtcMinute)],
			"56",		[(rtcMinute)],
			"57",		[(rtcMinute)],
			"58",		[(rtcMinute)],
			"59",		[(rtcMinute)],
		"TimeStyle",		[(DateStyle)appMenuEvt_RTCSet],
			"YY/MM/DD",		[(DateStyle)],
			"MM/DD/YY",		[(DateStyle)],
			"DD/MM/YY",		[(DateStyle)],
[MARK]MENU_MARK_LANGUAGE:
	"Language",		[(Language)],
		"English",		"language_en.bmp",		[(Language)appMenuEvt_LanguageSet],
		"",		"language_cs.bmp",		[(Language)appMenuEvt_LanguageSet],
		"",		"language_ct.bmp",		[(Language)appMenuEvt_LanguageSet],
	"TV Out",		[appMenuAct_TVMode(TVMode)],
		"NTSC",		"tv_mode_ntsc.bmp",		[(TVMode)appMenuEvt_TVModeSet],
		"PAL",		"tv_mode_pal.bmp",		[(TVMode)appMenuEvt_TVModeSet],
	"USB",		[(USBMode)],
		"MSDC",		"usb_computer.bmp",		[(USBMode)appMenuEvt_USBModeSet],
		"UVCMJPG",		"usb_camera.bmp",		[(USBMode)appMenuEvt_USBModeSet],
[MARK]MENU_MARK_FORMAT:
	"Format",
		"Yes",		[(VideoFormat)appMenuEvt_Format],
		"No",		[(VideoFormat)appMenuEvt_Format],
	"Reset All...",		[()appMenuEvt_LastItemProcess],
		"Yes",		[()appMenuEvt_ResetAll],
		"No",		[()appMenuEvt_ResetAll],
[MARK]MENU_MARK_SCENEMODE:
"SceneMode",
[MARK]MENU_MARK_SCENEMODEITEM:
	"Program",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Movie",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Smile Catch",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Shake-free",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Landscape",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Backlight",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Nightscene",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Building",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Kids",		[(SceneMode)appMenuEvt_SceneModeSet],
	"High ISO Portrait",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Food",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Text",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Snow",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Fireworks",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Sunset",		[(SceneMode)appMenuEvt_SceneModeSet],
	"WDR",		[(SceneMode)appMenuEvt_SceneModeSet],
	"HDR",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Panorama",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Blink Detect",		[(SceneMode)appMenuEvt_SceneModeSet],
	"Voice Rec",		[(SceneMode)appMenuEvt_SceneModeSet],
"Card Error",
	"Card Error",
		"Format.",
[MARK]MENU_MARK_FORMAT_YES:
			"Yes",
[MARK]MENU_MARK_FORMAT_NO:
			"No",
		"Delete This File?",
[MARK]MENU_MARK_DELONE_YES:
			"Yes",
[MARK]MENU_MARK_DELONE_NO:
			"No",
		"Detecting Blinking...",
[MARK]MENU_MARK_BLINK_SAVE:
			"Save",
[MARK]MENU_MARK_BLINK_CANCEL:
			"Cancel",
		"Crop this photo?",
[MARK]MENU_MARK_JPG_CROP_YES:
			"Yes",
[MARK]MENU_MARK_JPG_CROP_NO:
			"No",
		"USB mode select",		[(USBMode)],
[MARK]MENU_MARK_USBMODESET:
			"MSDC",		[(USBMode)appMenuEvt_USBModeSet],
			"PCCAM",		[(USBMode)appMenuEvt_USBModeSet],
			"Working",		[(USBMode)appMenuEvt_USBModeSet],
		"Please select Filetype!",		[(fileType)],
[MARK]MENU_MARK_FILETYPE_EVENT:
			"JPG",		[(fileType)appMenuEvt_FileTypeSet],
			"VIDEO",		[(fileType)appMenuEvt_FileTypeSet],
			"EVENT",		[(fileType)appMenuEvt_FileTypeSet],
"RedEyeSave",
[MARK]MENU_MARK_REDEYE_WRITE:
	"OverWrite",
[MARK]MENU_MARK_REDEYE_SAVE:
	"Save as",
[MARK]MENU_MARK_REDEYE_CANCEL:
	"Cancel",
[MARK]MENU_MARK_CALIBRATION:
"Calibration",
	"CALIB INFO",
	"OB CLBT",
	"M_SHUTTER",
	"ISO CLBT(LV11)",
	"Diff ISO OB CLBT",
[MARK]MENU_MARK_WBCLBT:
	"WB CLBT",
		"Back",
		"Calib_Awb",
		"Creat_Ctt",
		"Creat_Lv10_Calibration",
		"Creat_Lv10_Standard",
	"FOCUS",
[MARK]MENU_MARK_FLASH:
	"FLASH",
		"Back",
		"Pre-Analyses",
		"H_POS Scan",
		"Duration Scan",
	"View DPC",
	"Capture DPC",
	"BATTERY",
[MARK]MENU_MARK_CAMCONFIG:
	"+CAM CONFIG",
		"FW Update",
		"Factory Default",
		"PV SaveRaw",
		"CAP SaveRaw",
		"RawBatch Enable",
		"Prefh Stamp",
		"AF Full Scan",
[MARK]MENU_MARK_TESTBENCH:
	"+TEST BENCH",
		"Test Robot",
		"+Sensor",
			"Back",
			"IRIS Diff LV12",
			"Mini Gain LV12",
			"PV LV 4-16",
			"CAP LV 4-16",
			"P2...",
		"CIPA",
		"LCD Test",
		"KEY Test",
		"AE Meter",
		"+FLASH",
			"Back",
			"Curve A0",
			"Curve A1",
			"Curve B0",
			"Curve B1",
			"P2",
		"+AF",
			"Noise Coring Value Test",
			"Noise Coring Theshold Test",
			"Noise Theshoid Test",
			"Back",
		"BATTERY_CURVE",
		"AE debug",
		"AWB debug",
	"USB ISP",
	"Stand_alone LSC",
	"PV OB Cal",
	"VIDEO REC LOG",
	"Digital Zoom",
	"Touch Panel",

[STRING]:
	"Yes",
	"No",
	"Card Error",
	"Format",
	"Unprotect This File?",
	"Unprotect All Files?",
	"Protect This File?",
	"Protect All Files?",
	"Protect/Unprotect File?",
	"Delete This File?",
	"Detecting Blinking...",
	"Save",
	"Crop this photo?",
	"Please select Filetype!",
	"RedEyeSave",
	"OverWrite",
	"Save as",
	"Calibration",
	"CALIB INFO",
	"OB CLBT",
	"M_SHUTTER",
	"ISO CLBT(LV11)",
	"Diff ISO OB CLBT",
	"WB CLBT",
	"Back",
	"Calib_Awb",
	"Creat_Ctt",
	"Creat_Lv10_Calibration",
	"Creat_Lv10_Standard",
	"FOCUS",
	"FLASH",
	"Pre-Analyses",
	"H_POS Scan",
	"Duration Scan",
	"View DPC",
	"Capture DPC",
	"BATTERY",
	"Battery Type",
	"Alkaline",
	"Nimh",
	"Lithium",
	"Oxyride",
	"+CAM CONFIG",
	"FW Update",
	"Factory Default",
	"PV SaveRaw",
	"CAP SaveRaw",
	"RawBatch Enable",
	"Prefh Stamp",
	"AF Full Scan",
	"+TEST BENCH",
	"Test Robot",
	"+Sensor",
	"IRIS Diff LV12",
	"Mini Gain LV12",
	"PV LV 4-16",
	"CAP LV 4-16",
	"P2D",
	"CIPA",
	"LCD Test",
	"KEY Test",
	"AE Meter",
	"+FLASH",
	"Curve A0",
	"Curve A1",
	"Curve B0",
	"Curve B1",
	"P2...",
	"+AF",
	"Noise Coring Value Test",
	"Noise Coring Theshold Test",
	"Noise Theshoid Test",
	"BATTERY_CURVE",
	"AE debug",
	"AWB debug",
	"USB ISP",
	"Stand_alone LSC",
	"PV OB Cal",
	"VIDEO REC LOG",
	"Digital Zoom",
	"Enter",
	"Delete All Files?",
	"Delete Files?",
	"Formatting...",
	"Has no size to resize!",
	"File Locked!",
	"Protect File?",
	"Unprotect File?",
	"Protect All?",
	"Unprotect All?",
	"No File",
	"No Card!",
	"Remove Card!",
	"Waiting...",
	"Card Locked!",
	"PAUSE",
	"REC",
	"File Error!",
	"Please plug out card",
	"Memory Full",
	"Waiting for printer",
	"No Jpeg",
	"Format Fail!",
	"Low Battery",
	"Out of battery",
	"Not Support!",
	"Cancel",
	"Select up to 24",
	"Please select a picture",
	"Print Error",
	"Printer Error",
	"Not Find Printer",
	"PictBridge",
	"Set this photo?",
	"Set all photo?",
	"Review this file?",
	"Save this file?",
	"Delete",
	"All",
	"Select",
	"Copy to card",
	"Copy this file?",
	"Copy all files?",
	"Rotate",
	"Rotate this photo?",
	"Set",
	"Stop",
	"Delete voice memo?",
	"Ready to record",
	"Ready to Play",
	"Recording memo...",
	"Playing audio...",
	"Trim this photo?",
	"Copy error",
	"Capture not enough size",
	"Capture Done",
	"Busy...Please wait",
	"Not available for this file",
	"Image saved",
	"Paper size",
	"Paper error",
	"Load error",
	"Eject error",
	"Paper jam",
	"Ink empty",
	"Ink low",
	"Waste ink",
	"Fatal error",
	"Service call",
	"Printer busy",
	"Lever error",
	"Cover open",
	"Ink cover open",
	"No ink cartridge",
	"Info error",
	"USB error",
	"Connecting",
	"Printing...",
	"Cancelling",
	"No paper",
	"Play",
	"Display File",
	"Completed",
	"Not_Support_C2C",
	"Find start",
	"Find end",
	"and press OK",
	"Writing Video",
	"Writing File Fail",
	"Trim Video is too Short",
	"PB Type",
	"Video",
	"Image",
	"Burst LMT",
	"Burst SUST",
	"MFNR",
	"PreShot",
	"ZeroShot",
	"0.5sec",
	"Memory",
	"Internal",
	"Card",
	"USB",
	"Face Beauty",
	"Super Resolution",
	"USB Dongle",
	"SDIO",
	"WIFI Device",
	"Insufficient Disk Space",
	"Mono",
	"Toycam",
	"Lighttone",
	"Waiting for capture",
	"Waiting for card",
	"FHD+720P",
	"FHD+VGA",
	"FHD+FHD",
	"Time Lapse",
	"Invert Mode",
	"Disconnecting",
	"Card Speed Slow",
	"Power on Record",
	"JPG",
	"EVENT",
	"Protect",
	"Protect All",
	"Unprotect All",
	"Start-up",

#endif /* _MENU_RES_H_ */
