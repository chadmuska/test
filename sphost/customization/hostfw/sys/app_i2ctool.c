
 /*--------------------------------------------------------------------------*
 *  File name	: 	App_i2ctool.c
 *  Function 	: 	for the i2c command to read/write i2c data		 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#include "common.h"
#include <stdio.h>
#include <string.h>

#include "api/sp5k_utility_api.h"

#define REG_CLK (66666666)

static UINT32 i2c_port;
static UINT32 freq_div;   // I2C clock = (REGclk/2) / (4*freqDiv+4); REGclk = 66.666M
static UINT32 slave_addr;
static UINT32 read_mode;

  /*--------------------------------------------------------------------------*
 * Function name	: hex_dump
 * Function	       	: Dump the hex data
 * Return value   	: void
 * Parameter1    	: buffer pointer (--)
 * Parameter2    	: buffer length (UINT32)
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
void hex_dump(UINT8 *buf, UINT32 len)
{
    UINT32 i;

    for (i = 0; i < len; i++) {
        printf("0x%02x ", buf[i]);
    }
    printf("\n");
}

  /*--------------------------------------------------------------------------*
 * Function name	: i2ctool_init
 * Function	       	: Initial the i2c I/O setting
 * Return value   	: void
 * Parameter1    	: i2c port number (UINT32)
 * Parameter2    	: i2c pin number (UINT32)
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
static inline void i2ctool_init(UINT32 port, UINT32 pin)
{
#if 0
    appGsensorInit();
#else
    i2c_port = (port << 4) & 0xf0;
    printf("I2C port 0X%u\n", i2c_port);
    sp5kSscCfgSet(i2c_port, SP5K_SSC_CFG_MODE, SP5K_SSC_MODE_I2C);
    sp5kSscCfgSet(i2c_port, SP5K_SSC_CFG_PIN, pin + SP5K_SSC_CFG_PIN0);
#endif
}

  /*--------------------------------------------------------------------------*
 * Function name	: i2ctool_slave
 * Function	       	: Set the i2c to slave mode
 * Return value   	: void
 * Parameter1    	: i2c slave address (UINT32)
 * Parameter2    	: i2c speed to set the clock frequence (UINT32)
 * Parameter3    	: i2c mode (UINT32)
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
static inline void i2ctool_slave(UINT32 addr, UINT32 speed, UINT32 mode)
{
    freq_div = (REG_CLK / (8 * speed) + 1) & 0xff;
    slave_addr = (addr & 0xff) << 1;
    read_mode = mode;

    printf("I2C slave address 0X%u(0X%u) - clock div %u - "
                "read mode %u\n",
            addr, slave_addr, freq_div, read_mode);
}

  /*--------------------------------------------------------------------------*
 * Function name	: i2ctool_read
 * Function	       	: Read i2c data 
 * Return value   	: void
 * Parameter1    	: the i2c data register (UINT32)
 * Parameter2    	: count for the i2c response data length (UINT32)
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
static inline void i2ctool_read(UINT32 reg, UINT32 cnt)
{
    UINT32 val;
    UINT8 buf[8];

    if (cnt > sizeof(buf))
        cnt = sizeof(buf);

    val = (slave_addr << 16) | (freq_div << 8) | (read_mode << 1);
    sp5kSscCfgSet(i2c_port, SP5K_I2C_CFG_PHY, val);
    val = sp5kI2cRead(i2c_port, reg, buf, cnt);
    printf("Read REG 0x%0x(%u) - %u byte : ", reg, reg, cnt);
    if (val == 0) {
        hex_dump(buf, cnt);
    } else {
        printf("Failure (%u)\n", val);
    }
}


  /*--------------------------------------------------------------------------*
 * Function name	: i2ctool_write
 * Function	       	: Write i2c data 
 * Return value   	: void
 * Parameter1    	: the i2c data register (UINT32)
 * Parameter2    	: data buffer array (UINT32)
 * Parameter3    	: count for the i2c data length (UINT32) 
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
static inline void i2ctool_write(UINT32 reg, UINT32 data[], UINT32 cnt)
{
    UINT32 val;
    UINT8 buf[8];
    int i;

    if (cnt > sizeof(buf))
        cnt = sizeof(buf);

    for (i = 0; i < cnt; i++)
        buf[i] = data[i];

    printf("write %u byte(s) : ", cnt);
    hex_dump(buf, cnt);

    val = (slave_addr << 16) | (freq_div << 8) | (read_mode << 1);
    sp5kSscCfgSet(i2c_port, SP5K_I2C_CFG_PHY, val);
    sp5kI2cWrite(i2c_port, reg, buf, cnt);
}

  /*--------------------------------------------------------------------------*
 * Function name	: cmdI2ctool
 * Function	       	: Read or write i2c data by input command
 * Return value   	: void
 * Parameter1    	: argument count (UINT32)
 * Parameter2    	: argument pointer (--)
 * Parameter3    	: value array (UINT32) 
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
  
void cmdI2ctool(UINT32 argc, UINT8 *argv[], UINT32 v[])
{
    if (argc == 1) {
        printf("i2ctool usage:\n"
                "\ti2ctool init port pin : initial I2C \"port\", "
                    "and \"pin\" assignment. port is 0,1,2...\n"
                "\ti2ctool slave address speed [mode]: set slave \"address\", "
                    "and clock \"speed\", read \"mode\"\n"
                "\ti2ctool write address data... : write \"data...\" to"
                    " \"address\"\n"
                "\ti2ctool read address [cnt] : read \"cnt\" byte(s) data "
                    "on \"address\"\n"
                );
     } else if (argc == 4 && strcmp((char *)argv[1], "init") == 0) {
        i2ctool_init(v[2], v[3]);
    } else if (strcmp((char *)argv[1], "slave") == 0 && argc <= 5) {
        UINT32 mode = 0;

        if (argc == 5)
            mode = v[4];
        i2ctool_slave(v[2], v[3], mode);
    } else if (strcmp((char *)argv[1], "read") == 0 && argc >= 3 && argc <= 4) {
        UINT32 cnt = 1;

        if (argc == 4)
            cnt = v[3];

        i2ctool_read(v[2], cnt);
    } else if (strcmp((char *)argv[1], "write") == 0 && argc > 3) {
        i2ctool_write(v[2], &v[3], argc - 3);
    }
}
