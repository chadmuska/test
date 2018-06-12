/**************************************************************************
 *
 *       Copyright (c) 2012-2018 by iCatch Technology, Inc.
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
#ifndef APP_BTN_CVR_DEF_H
#define APP_BTN_CVR_DEF_H



/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/*-- DESCRIPTIONS ------------------------------------------------------
 * The pseudo button ID for enable/disable button function is defined :
 * [31:30:29:28] for GRP mask
 * [27:0] for BTN ID/Mask
 *
 * To distinguish between GPIO and ADC (we dont support Matrix BTN at present),
 * [11:0] are used for GPIO buttons
 * [27:12] are used for ADC buttons
 */

#define BTN_SHIFT_GRP_CODE		28
#define BTN_SHIFT_GPIO_MSK		0
#define BTN_SHIFT_ADC_MSK		12

/*-- Pseudo Button Group
 */
#define BTN_GRP_GPIO		(((UINT32)0x1)<<BTN_SHIFT_GRP_CODE)
#define BTN_GRP_ADC			(((UINT32)0x2)<<BTN_SHIFT_GRP_CODE)
#define BTN_GRP_ALL			(((UINT32)0xF)<<BTN_SHIFT_GRP_CODE)


/*-- PORTING Notes !!
 * The following configuration shall be coincident with the BTN ID
 * programmed in app_key.c, especially for ADC buttons. !!!!
 * Please see the descriptions followed.
 */

/*-- GPIO Buttons
 * Notes : The button ID defined below shall be the same as you programmed
 *         in app_key.c where appGpioBtnInit() does.
 */
typedef enum btnCtrlGpioId_s {
	BTN_ID_TELE				= 0,
	BTN_ID_WIDE,
	BTN_ID_S1,
	BTN_ID_S2,
	BTN_ID_TV,
	BTN_ID_PB,
	BTN_ID_POWER,
	BTN_ID_GPIO_TOTAL /* <<== checkup mechanism with app_hw_def.h */
} btnCtrlGpioId_t;

/*----------------- Button pseudo ID for Enable/Disable Buttons -----------------*/
#define BTN_MSK_TELE	(((UINT32)1)<<(BTN_ID_TELE+BTN_SHIFT_GPIO_MSK))
#define BTN_MSK_WIDE	(((UINT32)1)<<(BTN_ID_WIDE+BTN_SHIFT_GPIO_MSK))
#define BTN_MSK_S1		(((UINT32)1)<<(BTN_ID_S1+BTN_SHIFT_GPIO_MSK))
#define BTN_MSK_S2		(((UINT32)1)<<(BTN_ID_S2+BTN_SHIFT_GPIO_MSK))
#define BTN_MSK_PB		(((UINT32)1)<<(BTN_ID_PB+BTN_SHIFT_GPIO_MSK))

#define BTN_TELE	(BTN_GRP_GPIO + BTN_MSK_TELE)
#define BTN_WIDE	(BTN_GRP_GPIO + BTN_MSK_WIDE)
#define BTN_S1		(BTN_GRP_GPIO + BTN_MSK_S1)
#define BTN_S2		(BTN_GRP_GPIO + BTN_MSK_S2)
#define BTN_PB		(BTN_GRP_GPIO + BTN_MSK_PB)
#define BTN_TW		(BTN_TELE | BTN_WIDE)



/*-- ADC Buttons
 * Notes : The button ID defined below shall be the same as you programmed
 *         in app_key.c where appAdcBtnInit() does.
 */
typedef enum btnCtrlAdcId_s {
	BTN_ID_UP,
	BTN_ID_DOWN,
	BTN_ID_LEFT,
	BTN_ID_RIGHT,
	BTN_ID_SET,		/* OK */
	BTN_ID_DEL,		/* F1 */
	BTN_ID_MENU,	/* F2 */
	BTN_ID_MODE,	/* F3 */
	BTN_ID_ADC_TOTAL /* <<== checkup mechanism with app_hw_def.h */
} btnCtrlAdcId_t;

/*----------------- Button pseudo ID for Enable/Disable Buttons -----------------*/
#define BTN_MSK_RIGHT	(((UINT32)1)<<(BTN_ID_RIGHT+BTN_SHIFT_ADC_MSK))
#define BTN_MSK_UP		(((UINT32)1)<<(BTN_ID_UP+BTN_SHIFT_ADC_MSK))
#define BTN_MSK_DOWN	(((UINT32)1)<<(BTN_ID_DOWN+BTN_SHIFT_ADC_MSK))
#define BTN_MSK_LEFT	(((UINT32)1)<<(BTN_ID_LEFT+BTN_SHIFT_ADC_MSK))
#define BTN_MSK_DEL		(((UINT32)1)<<(BTN_ID_DEL+BTN_SHIFT_ADC_MSK))
#define BTN_MSK_MODE	(((UINT32)1)<<(BTN_ID_MODE+BTN_SHIFT_ADC_MSK))
#define BTN_MSK_SET		(((UINT32)1)<<(BTN_ID_SET+BTN_SHIFT_ADC_MSK))
#define BTN_MSK_MENU	(((UINT32)1)<<(BTN_ID_MENU+BTN_SHIFT_ADC_MSK))

#define BTN_RIGHT	(BTN_GRP_ADC + BTN_MSK_RIGHT)
#define BTN_UP		(BTN_GRP_ADC + BTN_MSK_UP)
#define BTN_DOWN	(BTN_GRP_ADC + BTN_MSK_DOWN)
#define BTN_LEFT	(BTN_GRP_ADC + BTN_MSK_LEFT)
#define BTN_DEL		(BTN_GRP_ADC + BTN_MSK_DEL)
#define BTN_MODE	(BTN_GRP_ADC + BTN_MSK_MODE)
#define BTN_SET		(BTN_GRP_ADC + BTN_MSK_SET)
#define BTN_MENU	(BTN_GRP_ADC + BTN_MSK_MENU)

#define BTN_ALL		(BTN_GRP_ALL + ((UINT32)0x0FFFFFFF))
#define BTN_4WAY	(BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT)


/* GPIO button realted */
#define GPIO_RPT		0 /* No Repeat key */
#define GPIO_DBNC		5 /* Debounce ~1 * polling timer */

/* ADC button realted */
#define ADC_RPT			18/* No Repeat key */
#define ADC_DBNC		5/* Debounce ~30 * polling timer */
#define ADC_BTN_A_CH	SP5K_ADC_SARIN1 /* channel 1 for 4 way key */
#define ADC_BTN_B_CH	SP5K_ADC_SARIN0 /* channel 0 for other key */

/* ADC button */
#define ADC_BTN_A_DEV		  10  /* % */
#define ADC_BTN_B_DEV		  10  /* % */

/* ADC setting */
#define ADC_BTN_A_S1        13325   /* UP */
#define ADC_BTN_A_S2         9994   /* DOWN */
#define ADC_BTN_A_S3         6171   /* LEFT */
#define ADC_BTN_A_S4         2785   /* RIGHT */

#define ADC_BTN_B_S1        13325   /* SET/MENU Key  */
#define ADC_BTN_B_S2         2785   /* F3 */
#define ADC_BTN_B_S3         6171   /* F2 */
#define ADC_BTN_B_S4         9994   /* F1 /DEL*/


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/



#endif  /* APP_BTN_CVR_DEF_H */


