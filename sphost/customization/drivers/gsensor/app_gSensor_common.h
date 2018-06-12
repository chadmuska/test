
#ifndef _APP_GSENSOR_COMMON_H_
#define _APP_GSENSOR_COMMON_H_


 /**************************************************************************
  * 						  C O N S T A N T S 						   *
  **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum _gSensorSelector_e {
	GSENSOR_CHIP_ID,
	GSENSOR_CAPABILITY_IDX,
	GSENSOR_CAPABILITY_DATA,
	GSENSOR_XYZ,
	GSENSOR_ANGULAR_RATE,
} gSensorSelector_e;

typedef struct _gSensorInit_t {
	UINT32 ulAddr;
	UINT32 ulSize;
} gSensorInit_t;

typedef struct _gSensorCfg_t {
	UINT32 ulselector;
	UINT32 ulvalue;
} gSensorCfg_t;

typedef enum _gSensorCfgSelector_e {
	GSENSOR_CFG_NULL,
	GSENSOR_CFG_NBIT_REGADDR,
	GSENSOR_CFG_NBIT_REGDATA,
	GSENSOR_CFG_READBIT,
	GSENSOR_CFG_NBIT_UNIT,
	GSENSOR_CFG_SLAVEADDR,
	GSENSOR_CFG_FERQUENCY_I2C,
	GSENSOR_CFG_ENADDR2BYTE,
	GSENSOR_CFG_READMODE,
	GSENSOR_CFG_CS_GRP_ID,
	GSENSOR_CFG_CS_PIN_MASK,
	GSENSOR_CFG_FERQUENCY_SPI,
	GSENSOR_CFG_MODE_SPI,
} gSensorCfgSelector_e;

typedef struct _gSensorCapab_t {
	UINT32 ucMeasureRange;
	UINT32 ucSensitivity;
} gSensorCapab_t;


#endif  /* _APP_GSENSOR_COMMON_H_ */



