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
#ifndef _APP_GPS_H_
#define _APP_GPS_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum { 
	GPS_STOP	= 0x0,	
	GPS_RUN,	
} GPS_STATUS_e;

typedef enum { 
	GPS_INFO_LATITUDE = 0x0,	
	GPS_INFO_LATITUDE_IND,	
	GPS_INFO_LONGITUDE,	
	GPS_INFO_LONGITUDE_IND,
	GPS_INFO_PFI,
	GPS_INFO_MSLALTITUDE,
	GPS_INFO_SPEED,
} GPS_INFO_e;

typedef struct GPRMC_s {
	UINT8 gprmcTime[6+1]; 	/* GPS time */
	UINT8 gprmcSts;			/* GPS status*/
	UINT8 gprmcLatitude[8+1]; /* GPS Latitude */
	UINT8 gprmcLatitudeInd; /* GPS Latitude Indicator*/
	UINT8 gprmcLongitude[9+1];/* GPS Longitude */
	UINT8 gprmcLongitudeInd;/* GPS Longitude  Indicator*/
	UINT8 gprmcReserved;	/* GPS Reserved  */
	UINT8 gprmcDate[8+1];		/* GPS date */
} GPRMC_s;

typedef struct GPGGA_s {
	UINT8 gpggaTime[6+1]; 	/* GPS time */
	UINT8 gpggaLatitude[8+1]; /* GPS Latitude */
	UINT8 gpggaLatitudeInd; /* GPS Latitude Indicator*/
	UINT8 gpggaLongitude[9+1];/* GPS Longitude */
	UINT8 gpggaLongitudeInd;/* GPS Longitude  Indicator*/
	UINT8 gpggaPFI;	/* GPS PositionFixIndicator  */
	UINT8 gpggaMSLAltitude[5+1]; /*MSLAltitude*/	
} GPGGA_t;

typedef struct GPVTG_s {
	UINT8 gpvtgSpeed[3+1]; 	/* GPS speed */
	UINT8 gpvtgReserved;	/* GPS Reserved  */
} GPVTG_t;



#if 1 /*add  by feng*/
typedef struct GPGSV_s {
	UINT8 ggsvtotalnumber; 	/* Number ofMessage */
	UINT8 ggsvMessageNumber;			/* Message Number*/
	UINT8 ggsvSatellites[2+1] ; /* Satellites in View */
	UINT8 gpgsvSatelliteID0[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR0[2+1];	/* SNR(C/NO)  */

	UINT8 gpgsvSatelliteID1[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR1[2+1];	/* SNR(C/NO)  */


	UINT8 gpgsvSatelliteID2[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR2[2+1];	/* SNR(C/NO)  */

	UINT8 gpgsvSatelliteID3[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR3[2+1];	/* SNR(C/NO)  */
#if 1
	UINT8 gpgsvSatelliteID4[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR4[2+1];	/* SNR(C/NO)  */

	UINT8 gpgsvSatelliteID5[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR5[2+1];	/* SNR(C/NO)  */
	
	UINT8 gpgsvSatelliteID6[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR6[2+1];	/* SNR(C/NO)  */

	UINT8 gpgsvSatelliteID7[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR7[2+1];	/* SNR(C/NO)  */
#endif	

#if 1
	UINT8 gpgsvSatelliteID8[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR8[2+1];	/* SNR(C/NO)  */

	UINT8 gpgsvSatelliteID9[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR9[2+1];	/* SNR(C/NO)  */
	
	UINT8 gpgsvSatelliteID10[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR10[2+1];	/* SNR(C/NO)  */

	UINT8 gpgsvSatelliteID11[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR11[2+1];	/* SNR(C/NO)  */
#endif	

#if 1
	UINT8 gpgsvSatelliteID12[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR12[2+1];	/* SNR(C/NO)  */

	UINT8 gpgsvSatelliteID13[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR13[2+1];	/* SNR(C/NO)  */
	
	UINT8 gpgsvSatelliteID14[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR14[2+1];	/* SNR(C/NO)  */

	UINT8 gpgsvSatelliteID15[3+1]; /* Satellite ID*/
	UINT8 gpgsvSNR15[2+1];	/* SNR(C/NO)  */
#endif	

} GPGSV_s;

#endif

typedef struct gpsInfo_s{
	UINT8 gpsStatus;
	struct GPRMC_s rmcInfo;
	struct GPGGA_s ggaInfo;
	struct GPVTG_s vtgInfo;
	struct GPGSV_s gsvInfo;
}gpsInfo_t;

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void  appUart2ndInit(void);
void appGpathSave(void);
void appUsrDataWrite(void);
void  appUsrDataWriteEnd(void);
void appGpsInfoParse(void);
void appGpsModeSwitch(void);
UINT8 appGpsInfoGet(UINT8 id,UINT8 buf[]);

#endif  /* _APP_GPS_H_ */


