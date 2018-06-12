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
#ifndef _APP_STILL_IQ_H_
#define _APP_STILL_IQ_H_




#if 0 /* to print the process of preview state init */
#define VIEW_PERF_LOG  profLogAdd
#else
#define VIEW_PERF_LOG(...) 
#endif

#if 0 /* to print each icons's draw time. */
#define VIEW_OSD_PERF_LOG  profLogAdd
#else
#define VIEW_OSD_PERF_LOG(...) 
#endif


/* for IQ tuning */
typedef struct {
		UINT64 OBC_SWH		:1; /* LSB */
		UINT64 BPC_SWH		:1;
		UINT64 LSC_SWH		:1;
		UINT64 ARD_SWH		:1;
		UINT64 ARD_DP_R_SWH	:1;
		UINT64 ARD_DP_G_SWH	:1;
		UINT64 ARD_DP_B_SWH	:1;
		UINT64 CA_SWH		:1;
        
		UINT64 RDN_SWH	    :1;
        UINT64 GDN_SWH	    :1;
        UINT64 BDN_SWH	    :1;
		UINT64 WHTC1_SWH	:1;
		UINT64 WHTC2_SWH	:1;
		UINT64 DRKC1_SWH	:1;
		UINT64 DRKC2_SWH	:1;
		UINT64 RGBWDR_SWH	:1;
        
		UINT64 CCM_SWH		:1;		
#if SPCA6330        
		UINT64 CCM2_SWH		:1;        
#endif
		UINT64 RGBGAMMA_SWH	:1;
		UINT64 LUT3D_SWH	:1;
		UINT64 HUECOR_SWH	:1;
		UINT64 YGAMMA_SWH	:1;
		UINT64 YUVWDR_SWH	:1;
		UINT64 DE_SWH		:1;
        
		UINT64 COR_SWH		:1; /* no use*/
		UINT64 EDGE_SWH		:1;        
		UINT64 CEDGE_SWH	:1;
		UINT64 AA_SWH		:1;
		UINT64 UVLPFEG_SWH	:1;
    	UINT64 UVLPFAR_SWH	:1;
		UINT64 YEF_SWH      :1;
    	UINT64 YNF_SWH      :1;
        
		UINT64 DN3D_SWH		:1;        
		UINT64 LDC_SWH		:1;        
		UINT64 pre_AA_SWH   :1;
		UINT64 preYEF_SWH   :1; 
        UINT64 preYNF_SWH   :1; 
		UINT64 preEDGE_SWH  :1;	
		UINT64 preUVLPFEG_SWH :1;
   		UINT64 preUVLPFAR_SWH :1;
        
		UINT64 AHD_SWH		:1;		
		UINT64 AHD_AA_SWH	:1;
		UINT64 AHD_UVLPFEG_SWH:1;
     	UINT64 AHD_UVLPFAR_SWH:1;
} BLOCK_SWH_t;

typedef union {
	UINT64 val;
	BLOCK_SWH_t iq_swh;
} IQ_SWH_t;

typedef enum {
	OBC_BLKIDX = 0x01, /* 0 indicates all block off */
	BPC_BLKIDX,
	LSC_BLKIDX,
	ARD_BLKIDX,
	ARD_DP_BLKIDX,
	CA_BLKIDX,
	RGBDN_BLKIDX,
	WHTC_BLKIDX,
	RGBWDR_BLKIDX,
	CCM_BLKIDX,
	RGBGAMMA_BLKIDX,
	LUT3D_BLKIDX,
	HUECOR_BLKIDX,
	YGAMMA_BLKIDX,
	YUVWDR_BLKIDX,
	DE_BLKIDX,
	COR_BLKIDX,
	EDGE_BLKIDX,
	AA_BLKIDX,
	UVLPF_BLKIDX,
	DN3D_BLKIDX,
	LDC_BLKIDX,
	AHD_BLKIDX,
	AHD_COR_BLKIDX,
	AHD_AA_BLKIDX,
	END_BLKIDX /* END of enum */
} IQ_BLKIDX_t;


typedef enum {
	IQ_NORMAL_MODE,
	IQ_TEST_MODE,
	IQ_MODE_END
} IQ_MODE_t;

typedef enum {
    OPSTATE_PV_INIT,
    OPSTATE_PV,
    OPSTATE_SNAP,
    OPSTATE_VIDEO,
    OPSTATE_END
} IQ_STATE_t;

extern IQ_SWH_t 	gIQ_swh;
extern IQ_MODE_t	gIQ_mode;
extern IQ_BLKIDX_t  gIQ_blkidx;

typedef struct {
	const char *cus_packet;
} cus_tablename_t;
#endif /* _APP_STILL_IQ_H_ */
