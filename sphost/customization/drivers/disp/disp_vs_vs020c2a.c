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

#include "common.h"
#include "middleware/disp_def.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_disp_api.h"
#include "debug_mod/dbg_api.h"
/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/*
  compile option in Makefile:
    DISP_UPS052=1 : use ups052 timing.
*/

/* render dimension. */
#define DEV_PID  0
#define DISP_UPS052	1

#if DISP_UPS052 == 1
#define DEV_NAME     "PANEL - VS0202A"
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_UPS052

#define WIDTH     176
#define HEIGHT    220

#if SPCA5330
#define HSYNCOS      0/*2*/
#define VSYNCOS      0/*2 */
#define HSYNCW       1/*5*/
#define VSYNCW       1/*5*/
#else
#define HSYNCW    2
#define VSYNCW    2
#endif


#define HPIXEL   528 /*390*/
#define VLINE     226 /*360*/

#define PNLH0     2
#define PNLV0     2
#define PNLH1     (PNLH0 + WIDTH - 1)
#define PNLV1     (PNLV0 + HEIGHT - 1 + 1)
#define CLK_SRC   DISP_CLK_SRC_DRAMPLL
#define CLK_1X      (CLK_NX/3)/*2600*/ /* 6136 75000 /3 or /4*/
#define CLK_NX    11700/*7800*/ /*24545*/ /*30000*//* Vsync */
#define SCL_CLK   125000/*117647 148500*/
#define FILTER_EVEN  /*DISP_UPS052_EVEN_BRG */DISP_UPS052_EVEN_RGB
#define FILTER_ODD   /*DISP_UPS052_ODD_BRG*/ DISP_UPS052_ODD_RGB

#endif


/* 3-wire interface. */

#define SCL  (1 << 12)
#define SDA  (1 << 17)
#define CS   (1 << 11)

#define LCD_MSK  (SCL | SDA | CS)

/*******************************************************************************
*                                  M A C R O S
*******************************************************************************/

#define	SCL_LO	sp5kGpioWrite(SP5K_GPIO_GRP_DISPL, SCL, 0)
#define	SCL_HI	sp5kGpioWrite(SP5K_GPIO_GRP_DISPL, SCL, SCL)
#define	SDA_LO	sp5kGpioWrite(SP5K_GPIO_GRP_DISPL, SDA, 0)
#define	SDA_HI	sp5kGpioWrite(SP5K_GPIO_GRP_DISPL, SDA, SDA)
#define	CS_LO	sp5kGpioWrite(SP5K_GPIO_GRP_DISPL, CS, 0)
#define	CS_HI	sp5kGpioWrite(SP5K_GPIO_GRP_DISPL, CS, CS)


#define BL  (1 << 24)

#define	BL_EN   sp5kGpioWrite(SP5K_GPIO_GRP_FML,BL,BL) /* Eric 110723 */
#define	BL_DIS   sp5kGpioWrite(SP5K_GPIO_GRP_FML,BL,0)

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* device open. */
#if DISP_UPS052 == 1
#define dispOpen  dispVsVs020c2aUps520Open

#endif

/* operating function. */
static UINT8* dispOpDevNameGet(void);
static void   dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc, UINT32 descSize);
static UINT32 dispOpPowerOn(void);
static UINT32 dispOpPowerOff(void);

/* serial command. */
static void dispSpiCmdSend(UINT32 val, UINT32 bitsLen);
static void dispSpiDataSend(UINT32 val, UINT32 bitsLen);

/*******************************************************************************
*                            G L O B A L   D A T A
*******************************************************************************/

static dispPara_t dispPwrOnParaTbl[] = {

	/* set clk and type. */
	{DISP_PARA_CLK_SRC, CLK_SRC, 0, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X, CLK_NX, 0, 0, 0},
	{DISP_PARA_CLK_PHASE, 8, 0, 0, 0, 0},  /* for both v4 & v5 ok. */
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0},


	/* set vh timing and format. */
	#if SPCA5330
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL, VLINE, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS, VSYNCOS, HSYNCW, VSYNCW, 0},
	/*{DISP_PARA_DEVICE_SRC, 1 , 0 , 0 , 0 , 0},  */
	#else
	{DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0},
	#endif
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0},
	/*{DISP_PARA_IMG_PV_RENDER_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0},*/
	/*{DISP_PARA_IMG_PV_CROP_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0},*/



	#if DISP_UPS052 == 1
	{DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_DISABLE, 0, 0, 0, 0},
	{DISP_PARA_UPS052_RGB_ONLY_ENABLE, 1, 0, 0, 0, 0},
	{DISP_PARA_UPS_HSD_MODE, DISP_UPS_HSD_DELAY_2T, 0, 0, 0, 0},
/*	{DISP_PARA_UPS_COLOR_FILTER, 0, 0, 0, 0, 0}, */
	{DISP_PARA_UPS_COLOR_FILTER, FILTER_EVEN, FILTER_ODD, 0, 0, 0},	
	/*{DISP_PARA_CCIR_CLAMP_MODE, DISP_CCIR_CLAMP_DISABLE, 0, 0, 0, 0},*/
	{DISP_PARA_INTERLACE_MODE, DISP_PROGRESSIVE, 0, 0, 0, 0},
	{DISP_PARA_LCM_DATA_PACKET, 0, 0, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK, 0, 0, 0, 0}, 
	
	#endif
	
/*	{DISP_PARA_COLOR_MATRIX_ENABLE, 0, 0, 0, 0, 0}, */	
#if 0
    {DISP_PARA_PIN_MUX, 0, 0x077C, 0x077C, 0, 0}, 
    {DISP_PARA_PIN_SELECT, DISP_PIN_SEL_TVGPIO_15TO0, 1, 0, 0, 0}, 
    {DISP_PARA_PIN_SELECT, DISP_PIN_SEL_TVGPIO_27TO16, 1, 0, 0, 0}, 
#endif
	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},
	/* send 3-wire command. */
	{DISP_PARA_GPIO_FUNC_CFG, SCL | SDA | CS, SCL | SDA | CS, 0, 0, 0},
	{DISP_PARA_GPIO_DIR_CFG, SCL | SDA | CS, SCL | SDA | CS, 0, 0, 0},
     
	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},

};



/* serial command table. */

const static UINT16 dispCmdTbl[] = {
0x01,
0x02,                       
0x03 ,                     
0x08,
0x0C,
0x0F,
0x20,
0x21,
/*Delay(10);*/ 
0x10,
0x11,
/*Delay(10);*/
0x30,
0x31,
0x32,
0x33,
0x34,
0x35,
0x36,
0x37,
0x38,
0x39,
0xFF,
0xB0,
0xB1,
0x50,
0x51,
0x52,
0x53,
0x54,
0x55,
0x56,
0x57,
0x58,
0x59,
0xFF,
0x07,
};

const static UINT16 dispDataTbl[] = {
0x111C,
0x0100,
0x1038,
0x0505,
0x0112,
0x0001,
0x0000,
0x0000,
/* Delay(10); */
0x0000,
0x1000,
/* Delay(10); */
0x0000,
0x00DB,
0x0000,
0x0000,
0x00DB,
0x0000,
0x00AF,
0x0000,
0x00DB,
0x0000,
0x0003,
0x1501,
0x0606,
0x0000,
0x0808,
0x0207,
0x0000,
0x0603,
0x0000,
0x0807,
0x0207,
0x0000,
0x0503,
0x0000,
0x1017,
};


/* device name. */
#if SPCA5310
#if DISP_UPS052 == 1
static UINT8 dispDevName[] = "PANEL - VS020C2A_UPS052";
/*static UINT8 dispDevName[] = DEV_NAME; */
#endif
#else
#if DISP_UPS052 == 1
static UINT8 dispDevName[] = "LCD -VS020C2A_UPS052";
#endif
#endif

/* device capability. */
#if 0
static dispCapabDesc_t dispCapabDesc = {
	WIDTH,
	HEIGHT,
	PNLH0,
	PNLV0,
	4,
	3,
};
#endif
static dispCapabDesc_t dispCapabDesc = {
	.width        = WIDTH,
	.height       = HEIGHT,
	.xoff         = PNLH0,
	.yoff         = PNLV0,
	.aspectRatioX = 4,
	.aspectRatioY = 3,
	.logiEn       = 0,
	.frameRate    = 60000,
};

/*******************************************************************************
*
* Function Name: dispOpen
*
* Descriptions:
*
* Arguments:
*
* Returns:
*   SUCCESS or FAIL.
*
* See also:
*
*******************************************************************************/
UINT32
dispOpen(
	void
)
{

	UINT32 ret;
	UINT32 chnlId, devId;
	dispOperatingFunc_t dispOpFunc;


	DBG_FUNC_ENTER(NULL);

	dispOpFunc.pfOpDevNameGet    = dispOpDevNameGet;
	dispOpFunc.pfOpPowerOn       = dispOpPowerOn;
	dispOpFunc.pfOpPowerOff      = dispOpPowerOff;
	dispOpFunc.pfOpCapabilityGet = dispOpCapabilityGet;
	dispOpFunc.pfOpResolutionSet = NULL;
	chnlId = (DEV_PID == 0) ? SP5K_DISP_CHNL_0 : SP5K_DISP_CHNL_1;
	devId  = (DEV_PID == 0) ? SP5K_DISP_CHNL_0_PANEL : SP5K_DISP_CHNL_1_PANEL;
	ret = sp5kDispDevReg(chnlId, devId, &dispOpFunc);

	DBG_FUNC_RETURN(ret, NULL);

}

/*******************************************************************************
*
* Function Name: dispOpDevNameGet
*
* Descriptions:
*
* Arguments:
*
* Returns:
*   return = device name string.
*
* See also:
*
*******************************************************************************/
UINT8 *
dispOpDevNameGet(
	void
)
{
	return dispDevName;
}

/*******************************************************************************
*
* Function Name: dispOpCapabilityGet
*
* Descriptions:
*
* Arguments:
*
* Returns:
*
* See also:
*
*******************************************************************************/
void
dispOpCapabilityGet(
	dispCapabDesc_t* pcapabDesc,
	UINT32 descSize
)
{
	#if 1
	UINT32 frmRate;
	if (DEV_PID == 0) {
		dispInfGet(DISP_CHNL_0, DISP_INF_FRAME_RATE, &frmRate);
	}
	else {
		dispInfGet(DISP_CHNL_1, DISP_INF_FRAME_RATE, &frmRate);
	}
	dispCapabDesc.frameRate = frmRate;
	#endif
	*pcapabDesc = dispCapabDesc;
}

/*******************************************************************************
*
* Function Name: dispOpPowerOn
*
* Descriptions:
*
* Arguments:
*
* Returns:
*
* See also:
*
*******************************************************************************/

#define WRITE8(a,b)        *(volatile UINT8*)(a) = (b) 
#define READ8(a) (*((volatile UINT8*)(a)))

#define WRITE16(a,b)	*(volatile UINT16*)(a) = (b) 
#define READ16(a)		(*((volatile UINT16*)(a)))


void dispspisendcmd(	
	UINT32 val,
	UINT32 bitsLen
)
{
UINT32 outputvalue=val<<8|bitsLen;
/*sp5kGpioCfgSet(SP5K_GPIO_GRP_DISPL, LCD_MSK, LCD_MSK);	*/
dispSpiCmdSend(outputvalue,16);
}



UINT32
dispOpPowerOn(
	void
)
{
	UINT32 count= 0;



	unsigned char finish = 0x22;
	sp5kTimeDelay( SP5K_TIME_DELAY_1MS , 500); 
	dispParaTableSet(DEV_PID, dispPwrOnParaTbl);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISPL, LCD_MSK, LCD_MSK);	

	/* send 3-wire command. */
      CS_HI; 
      SCL_HI;

	dispSpiCmdSend( 0x28 , 8);
	dispSpiDataSend(0x00CE , 16);
	sp5kTimeDelay( SP5K_TIME_DELAY_1MS , 20);


for( count = 0 ;  count <35 ; count++ )
{
	dispSpiCmdSend( dispCmdTbl[count] , 8);
	dispSpiDataSend( dispDataTbl[count] , 16);

	if( count == 7 ||count == 9)
	{
	 
		printf("sp5kTimeDelay 20ms for write table\n");
		sp5kTimeDelay( SP5K_TIME_DELAY_1MS , 20);

	}
	else
		sp5kTimeDelay( SP5K_TIME_DELAY_1MS , 1);
}	

dispSpiCmdSend( finish , 8);


/*BL_EN;*/
	printf("VS020c2a 2.0 start init \n");
   
	return SUCCESS;
}
void testpower(void){
	dispOpPowerOn();
}


/*******************************************************************************
*
* Function Name: dispOpPowerOff
*
* Descriptions:
*
* Arguments:
*
* Returns:
*
* See also:
*
*******************************************************************************/
UINT32
dispOpPowerOff(
	void
)
{
	BL_DIS;

/*	dispParaSet(DISP_PARA_PIN_SELECT, DISP_PIN_SEL_TVGPIO_15TO0, 0, 0, 0, 0 ,0);
	dispParaSet(DISP_PARA_PIN_SELECT, DISP_PIN_SEL_TVGPIO_27TO16, 0, 0, 0, 0 ,0);
*/	
	return SUCCESS;
}

/*******************************************************************************
*
* Function Name: dispSpiCmdSend
*
* Descriptions:
*
* Arguments:
*
* Returns:
*
* See also:
*
*******************************************************************************/
static void
dispSpiDataSend(
	UINT32 val,
	UINT32 bitsLen
)
{


	UINT32	i, j;
	UINT32   mark = 0x1<<(bitsLen-1);
printf("dispSpiDataSend  0x%0x\n",val);

	CS_LO;
	SCL_LO;
	SDA_LO;

/* Isaac add for spi command bit*/
	
	SDA_HI;
	for ( j = 0; j < 3; j++ );
		SCL_HI;
	for ( j = 0; j < 3; j++ );
		SCL_LO;
	for ( j = 0; j < 3; j++ );
		SDA_LO;

/* Isaac add for spi command parameter bit end*/
	
	for ( i = 0; i < bitsLen; i++ ) {
		SCL_LO;
		if ( val & mark ) {
			SDA_HI;
		}
		else {
			SDA_LO;
		}
		for ( j = 0; j < 3; j++ );
		SCL_HI;
		for ( j = 0; j < 3; j++ );
		SCL_LO;
		for ( j = 0; j < 3; j++ );
		SDA_LO;

		val <<= 1;

/* Isaac add for spi command parameter bit*/

		if(i==7&&bitsLen>8)
		{

			for ( j = 0; j < 3; j++ );
				CS_HI;
			for ( j = 0; j < 3; j++ );
				CS_LO;


			SCL_LO;
			SDA_HI;
			for ( j = 0; j < 3; j++ );
				SCL_HI;
			for ( j = 0; j < 3; j++ );
				SCL_LO;
			for ( j = 0; j < 3; j++ );
				SDA_LO;


		}
		
/* Isaac add for spi command parameter bit end*/
	}
	
	CS_HI;

}

static void
dispSpiCmdSend(
	UINT32 val,
	UINT32 bitsLen
)
{


	UINT32	i, j;
	UINT32   mark = 0x1<<(bitsLen-1);
printf("dispSpiCmdSend  0x%0x\n",val);

	CS_LO;
	SCL_LO;
	SDA_LO;

/* Isaac add for spi command bit*/
	SCL_LO;
	SDA_LO;
	for ( j = 0; j < 3; j++ );
		SCL_HI;
	for ( j = 0; j < 3; j++ );
		SCL_LO;
	for ( j = 0; j < 3; j++ );
		SDA_LO;

/* Isaac add for spi command bit end*/

	for ( i = 0; i < bitsLen; i++ ) {
		SCL_LO;
		if ( val & mark ) {
			SDA_HI;
		}
		else {
			SDA_LO;
		}
		for ( j = 0; j < 3; j++ );
		SCL_HI;
		for ( j = 0; j < 3; j++ );
		SCL_LO;
		for ( j = 0; j < 3; j++ );
		SDA_LO;

		val <<= 1;
	}


	CS_HI;

}
/*******************************************************************************
*
* Function Name: dispTest
*
* Descriptions:
*
* Arguments:
*
* Returns:
*
* See also:
*
*******************************************************************************/

#if 0
void RegisterWrite(UINT32 addr, UINT32 value){

	UINT32 val = (addr<<12)|value;

/*	dispSpiCmdSend(val,16); */
}
#endif

