;/* SPCA6350 */

;/* LCD */
PWR_LCD        out dummy 0
PWR_BACKLIGHT  out lmi 18 0
PWR_BACKLIGHT2 out lmi 19 0
PWR_BL_CTL     out dummy 0
LCD_RESET      out dummy 0

;/* LCD Serial Port */
LCD2_SPI_CS  out disp 30 0
LCD_SPI_CS   out disp 11 0
LCD_SPI_SCL  out disp 12 0
LCD_SPI_SDA  out disp 17 0

;/* TV */
TV_DETECT    in tg 7

;/* STROBE */

;/* LENS	*/

;/* LED	*/

;/* SD Card	*/
SD_CARD_DETECT	!in	lmi	20	0
SD_CARD_WP		in  lmi 21  0
SD_CARD_POWER		out	lmi 12

;/* Main POWER control */

;/* BUTTON KEY */
BUTTON_POWER		in	pocu	0
BUTTON_PB		 in	pocu	2
BUTTON_TELE		!in	gen	10
BUTTON_WIDE		!in	lmi	15
BUTTON_SNAP1	!in  tg 2
BUTTON_SNAP2	!in  tg 3

;/* SPEAKER */

;/* USB	*/
USB_DETECT          in usb 0

;/* battery	*/

;/* SENSOR */
TG_CS               out dummy

CMOS_PWR_ANA        out dummy
CMOS_PWR_IO         out dummy
CMOS_PWR_CORE       out dummy
CMOS_RESET          out tg 4 0
CMOS_PWDN           out dummy
CMOS_PWR_ANA_2ND    out dummy
CMOS_PWR_IO_2ND     out dummy
CMOS_PWR_CORE_2ND   out dummy
CMOS_RESET_2ND      out tg 6 0
CMOS_PWDN_2ND       out dummy

; /* nouse pins will be set output and pull low */
;#define MASK_NOUSE_GEN  0
;#define MASK_NOUSE_TGL  0
;#define MASK_NOUSE_DISP 0
;#define MASK_NOUSE_LMI  0
;#define MASK_NOUSE_FML  0
;#define MASK_NOUSE_FMH  0

;/* for OV5633 */
;#if SP5K_SENSOR_OV5633
;#define HW_RGB_IN_EN_MASK 0x3FFFFFF
;#endif

;/* for IMX118 */
;#if SP5K_SENSOR_SONYIMX118
;#define SEN_REMAP_0 0x76543210
;#define SEN_REMAP_1 0xffffff98
;#endif
;
;/* for IMX175 */
;#if SP5K_SENSOR_SONYIMX175
;#define SEN_REMAP_0 0xffffBA10
;#define SEN_REMAP_1 0xffffffff
;#endif
;
;/* for IMX206 */
;#if SP5K_SENSOR_SONYIMX206
;#define SEN_REMAP_0 0xffffBA10
;#define SEN_REMAP_1 0xffffffff
;#endif

;/* for AR0330 */
;#if (SP5K_SENSOR_AR0330CS)||(SP5K_SENSOR_AR0330)
;#define SEN_REMAP_0 0xffffBA10
;#define SEN_REMAP_1 0xffffffff
;#endif
;

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
;/* Button Num */
;/* S1/S2/T/W/TV/PB */
;#define GPIO_BTN_NUM    7
;/* UP/DOWN/LEFT/RIGHT/OK/F1/F2/F3 */
;#define ADC_BTN_NUM     8
