;/* SPCA6350 */

;/* LCD */
PWR_BACKLIGHT  out fm 37 0
PWR_LCD        out dummy 0

PWR_BACKLIGHT2 out lmi 19 0
PWR_BL_CTL     out dummy 0
LCD_RESET      out dummy 0

;/* LCD Serial Port */
LCD_SLOAD    out lmi 15 1
LCD2_SPI_CS  out disp 30 0
LCD_SPI_CS   out disp 11 0
LCD_SCLK     out disp 12 0
LCD_SDATA    out disp 17 0
LCD_SWITCH   out dummy 0
LCDCLK       out dummy 0

;/* HDMI */
PWR_HDMI            out dummy

;/* TV */
;#if (!SP5K_SENSOR_DUAL_MODE)
TV_DETECT    !in tg 7
;#else
TV_DETECT    in dummy /*tmep workaround for RAW+YUV PWDN2 pin*/
;#endif
TV_IN        out dummy 0

;/* Audio */
LINE_OUT_MUTE       out dummy

;/* STROBE */
FLASH_TRIG          out dummy 0
FLASH_CHARGE        out dummy 0
FLASH_COVER			out dummy 0
PWR_FLASH           out dummy 0
FLASH_READY         out dummy 0

;/* LENS */
ZOOM_HP             out dummy 0
ZOOM_CLK            out dummy 0
ZOOM_DIR            out dummy 0
FOCUS_HP            out dummy 0
ZOOM_CLK_EN         out dummy 0
GMT_CLK             out dummy 0
PWR_ZOOM_IC         out dummy 0
ZM_ENC_CA           out dummy 0
ZM_AF_HOME_CA       out dummy 0

MESHUT		    out dummy 0
;/*#define CFG_MESHUT PIN_7*/

;/*IRIS		    fn lmi 7*/
;#define GRP_IRIS GRP_MESHUT
;#define CFG_IRIS CFG_MESHUT

;#define CFG_RENESAS_CLK	0xffff
RENESAS_LENS_CS     out dummy 0
RENESAS_LENS_CLK    out dummy 0
RENESAS_LENS_SDA    out dummy 0
RENESAS_LENS_DATA   out dummy 0
RENESAS_RSTN        out dummy 0

PWR_ZOOM_LED        out dummy 0
PWR_FOCUS_LED       out dummy 0

IN1                 out dummy 0
IN2                 out dummy 0
IN3                 out dummy 0
IN4                 out dummy 0
OIS_PI_POWER        out dummy 0
MOT_SLD             out dummy 0
MOT_SCLK            out dummy 0
MOT_SDAT            out dummy 0
MOT_RESET           out dummy 0
MOTOR_EXT1          out dummy 0
MOB1                out dummy 0

ZOOM_PWM            out dummy 0
;/*#define CFG_ZOOM_PWM	PIN_10     */


;/*OIS*/
OIS_Y_IN_1A         out dummy
OIS_Y_IN_1B         out dummy
OIS_Y_IN_2A         out dummy
OIS_Y_IN_2B         out dummy
OIS_X_IN_1A         out dummy
OIS_X_IN_1B         out dummy
OIS_X_IN_2A         out dummy
OIS_X_IN_2B         out dummy

OIS_MOT_SLD         out dummy
OIS_MOT_SCLK        out dummy
OIS_MOT_SDAT        out dummy
OIS_MOT_RESET       out dummy
OIS_EXT1           out dummy
OIS_EXT2           out  dummy

OIS_X_PI            in dummy
OIS_Y_PI            in dummy

;/*HS_CS            out     fm     24      0	*/
;/*HS_SPCLK         out     fm     27      	*/
;/*HS_DAT_OUT	    out     fm     26      0	*/
;/*HS_DAT_IN        in      fm     25     	*/
HS_SRDYO_N	    in      fm     29


;/*GYRO*/

GYRO_DI             in  dummy
GYRO_POWER          out dummy
GYRO_CS             out dummy
GYRO_CLK            out dummy
GYRO_SDA            out dummy


;/* LED */
LED_POWER           out dummy 0
LED_AF              out     fm     36      0
LED_SELFTIMER       out     gen     8      0

;/* SD Card */
SD_CARD_DETECT		!in	lmi	20	0
SD_CARD_WP		in dummy
SD_CARD_POWER		!out	fm 	33	1 PD=0
SD_CARD_18V             out dummy

;/* Main POWER control */

;/*Matrix GPIO - Button*/
KEY_SCAN1			out	dummy
KEY_SCAN2			out	dummy
KEY_SCAN3			out	dummy
KEY_SCAN4			out	dummy
KEY_SCAN5			out	dummy
KEY_SCAN6			out	dummy

;/* BUTTON KEY */
BUTTON_POWER in  pocu 2
BUTTON_LOCK  !in dummy
BUTTON_PLAY  !in dummy
BUTTON_TELE  !in dummy
BUTTON_WIDE  !in dummy
BUTTON_SNAP1 !in dummy
BUTTON_SNAP2 in  fm   40
BUTTON_ANTI_SHAKE in dummy
MODE_SWITCH_L     in dummy
MODE_SWITCH_R     in dummy

;/* SPEAKER */

;/* DOOR */
DOOR_STAT   in	dummy

;/* USB */
USB_DETECT          in usb 0

;/*SDIO*/
SDIO_SHUTDOWN   !out fm 38 1
SDIO_PWR_EN     out dummy
;#define SDIO_PINMUX	2 /* 0:FM15-20 , 1: FM24-29 , 2: FM41-46 */

;/*BT  BT_SHUTDOWN older is !out fm 27 1 */
BT_SHUTDOWN     !out fm 38 1
BT_PWR_EN       out dummy
;#define BT_UART_PORT 2 /* 2 = UART2 , 3 = UART3 */
;#define BT_UART_PIN  1 /* UART2 0:FMIF2,3,21,22 1:FMIF28,29,30,31 2:FMIF41,42,43,44 */
                        /* UART3 0:FMIF24,25,26,27 */

;/*Ethernet PHY*/
ETHERNET_PHY_PWR_EN out fm 0 0
ETHERNET_PHY_INT        in lmi 19 PD=1

;#define IN_CALIB_AF_LAMP 0

;/* battery */
BATTERY_INT         in   dummy
BATTERY_DETECT_EN   !out dummy

;/* SENSOR */
TG_CS               out dummy

CMOS_PWR_ANA        out dummy
CMOS_PWR_IO         out dummy
CMOS_PWR_CORE       out dummy

;#if (SP5K_SENSOR_AR0330CS)||(SP5K_SENSOR_AR0330)||(SP5K_SENSOR_AR0230)||(SP5K_SENSOR_AR1335)||(SP5K_SENSOR_AR0237)||(SP5K_SENSOR_OV7695)
CMOS_RESET          out tg 5 0
;#else
CMOS_RESET          out tg 4 0
;#endif

;#if (SP5K_SENSOR_DUAL_OV4689)||(SP5K_SENSOR_OV4689)||(SP5K_SENSOR_OV4689_SERDES_SPCA7002)||(SP5K_SENSOR_OV4689_SPCA7002)
CMOS_PWDN           out tg 5 0
;#elif (SP5K_SENSOR_GC4603)||(SP5K_SENSOR_SOIF02) || (SP5K_SENSOR_GC8603)||(SP5K_SENSOR_SOIF22)||(SP5K_SENSOR_SOIF25)
CMOS_PWDN           out tg 5 1
;#else
CMOS_PWDN           out dummy
;#endif

CMOS_PWR_ANA_2ND    out dummy
CMOS_PWR_IO_2ND     out dummy
CMOS_PWR_CORE_2ND   out dummy
CMOS_RESET_2ND      out tg 6 0
CMOS_PWDN_2ND       out dummy


;/* tg clk driving 0=4mA 1=8mA 2=12mA 3=14mA */
;#define TGCLK_DRV 3

;/* HW ver */
HW_VERSION0         in dummy
HW_VERSION1         in dummy

;/* USB charger */
CHG_PGOOD           in dummy
CHG_CHG             in dummy
CHGCTL_VSET         out dummy
CHG_ISET            out dummy
CHGCTL_EN           out dummy
CHGCTL_500mA        out dummy
CHGCTL_USUS         out dummy
G_INT               in dummy
BATT_INTERRUPT      !in dummy

PCB_TEST1_EN        in lmi 2 PD=1
PCB_TEST2_EN        in lmi 18 PD=1

;/* DRAM LDO */
DRAM_LDO_EN          out fm 32 0
DRAM_LDO_CLR         out tg 1 0

; /* nouse pins will be set output and pull low */
;#define MASK_NOUSE_GEN  0
;#define MASK_NOUSE_TGL  0
;#define MASK_NOUSE_DISP 0
;#define MASK_NOUSE_LMI  0
;#define MASK_NOUSE_FML  0
;#define MASK_NOUSE_FMH  0
;#define MASK_NOUSE_SAR  0
;#define MASK_NOUSE_OIS  0
;#define MASK_NOUSE_CEC  0
;#define MASK_NOUSE_RGB  0x03C00000
;#define FRONT_RGB_IN_PIN_MASK 0x00000000
;#define FRONT_RGB_IN_PULL_EN  0x00000000
;#define HW_LMI16_17_SCHIMITT_TRIGGER_WINDOW   0x00
;#define HW_LMI18_19_SCHIMITT_TRIGGER_WINDOW   0x01
;#define HW_LMI20_21_SCHIMITT_TRIGGER_WINDOW   0x11


;/* for OV5633 */
;#if SP5K_SENSOR_OV5633
;#define HW_RGB_IN_EN_MASK 0x3FFFFFF
;#endif

;/* for IMX118 */
;#if SP5K_SENSOR_SONYIMX118
;#define SEN_REMAP_0 0x76543210
;#define SEN_REMAP_1 0xffffff98
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif
;

;/* for IMX078 */
;#if SP5K_SENSOR_SONYIMX078
;#define SEN_REMAP_0 0x3F21F0FF
;#define SEN_REMAP_1 0xffffffFF
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif


;/* for IMX175 */
;#if SP5K_SENSOR_SONYIMX175
;#define SEN_REMAP_0 0xffff3210
;#define SEN_REMAP_1 0xffffff98
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif
;
;/* for FAKE */
;#if SP5K_SENSOR_FAKE
;#define SEN_REMAP_0 0xffffBA10
;#define SEN_REMAP_1 0xffffff98
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif
;
;/* for IMX200 */
;#if SP5K_SENSOR_SONYIMX200
;#define SEN_REMAP_0 0xffffBA10
;#define SEN_REMAP_1 0xffffff98
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif
;
;/* for IMX206 */
;#if SP5K_SENSOR_SONYIMX206
;#define SEN_REMAP_0 0xffff3210
;#define SEN_REMAP_1 0xffffffff
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif
;
;/* for IMX283 */
;#if SP5K_SENSOR_SONYIMX283
;#define SEN_REMAP_0 0xffff3210
;#define SEN_REMAP_1 0xffffffff
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif

;/* for IMX291 */
;#if (SP5K_SENSOR_SONYIMX291)|| (SP5K_SENSOR_GC8603)
;#define SEN_REMAP_0 0xffff3210
;#define SEN_REMAP_1 0xffffffff
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif
;
;/* for OV4689 */
;#if SP5K_SENSOR_OV4689 || SP5K_SENSOR_ADV7480||(SP5K_SENSOR_AR0237)
;#define SEN_REMAP_0 0xffff3210
;#define SEN_REMAP_1 0xffffffff
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif

;/* for AR0330 */
;#if (SP5K_SENSOR_AR0330CS)||(SP5K_SENSOR_AR0330)||(SP5K_SENSOR_AR0230)||(SP5K_SENSOR_AR1335)
;#define SEN_REMAP_0 0xffff3210
;#define SEN_REMAP_1 0xffffffff
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif

;/* for PANAMN34120 */
;#if SP5K_SENSOR_PANAMN34120
;#define SEN_REMAP_0 0xfffffA10
;#define SEN_REMAP_1 0xffffffff
;#define SEN_REMAP_2 0xfffff576
;#define SEN_REMAP_3 0xffffffff
;#endif

;/* for SONYIMX258 */
;#if SP5K_SENSOR_SONYIMX258
;#define SEN_REMAP_0 0xffff3102
;#define SEN_REMAP_1 0xffffffff
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif


;/* for PS5246 */
;#if SP5K_SENSOR_PS5246
;#define SEN_REMAP_0 0xffffff10
;#define SEN_REMAP_1 0xffffffff
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif

;/* for OS08A10 */
;#if SP5K_SENSOR_OS08A10
;#define SEN_REMAP_0 0xffffb10a
;#define SEN_REMAP_1 0xffffffff
;#define SEN_REMAP_2 0xffffffff
;#define SEN_REMAP_3 0xffffffff
;#endif

;/* HDMI phy related settings */
;/* [7:0] cpp, [15:7] cpi, [23:15] bias */
;#define HW_HDMI_PHY_CHARGE_PUMP 0x030703
;/* [7:0] enable, [15:7] voltage control */
;#define HW_HDMI_PHY_PRE_EMPHASIS_50HZ_576P  0
;#define HW_HDMI_PHY_PRE_EMPHASIS_50HZ_720P  0
;#define HW_HDMI_PHY_PRE_EMPHASIS_50HZ_1080I 0
;#define HW_HDMI_PHY_PRE_EMPHASIS_50HZ_1080P 0
;#define HW_HDMI_PHY_PRE_EMPHASIS_60HZ_480P  0
;#define HW_HDMI_PHY_PRE_EMPHASIS_60HZ_720P  0
;#define HW_HDMI_PHY_PRE_EMPHASIS_60HZ_1080I 0
;#define HW_HDMI_PHY_PRE_EMPHASIS_60HZ_1080P 0

;/* custom define */
;/*#define HW_DISP_SPI*/
;#define I2C_DVS_PV    0xcc
;#define I2C_DVS_CAP   0xcd
;#define I2C_DVS_VREC  0xcd

;/* Button Num */
;/* S1/S2/T/W/TV/PB */
;#define GPIO_BTN_NUM    7
;/* UP/DOWN/LEFT/RIGHT/OK/F1/F2/F3 */
;#define ADC_BTN_NUM     8
