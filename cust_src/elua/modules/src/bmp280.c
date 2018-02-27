// bmp280 Driver.
// Reference: 	http://blog.csdn.net/liaokesen168/article/details/50930834
//				http://www.cnblogs.com/leptonation/p/5210331.html
// Thanks to @Leptonation and @liaokesen168.

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "auxmods.h"
#include "lrotable.h"
#include "lrodefs.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>

#include "platform.h"
#include "platform_conf.h"
#include "platform_i2c.h"
#include "platform_rtos.h"
#include "platform_malloc.h"

static u8 bmp280_i2c_addr = 0x76;	// default when SDO_PIN == 0. 	0x77 When SDO_PIN == 1.
static u8 bmp280_i2c_id = 2;		// default i2c 2
static u16 bmp280_tdata[3] = {0};	// t calibration data
static u16 bmp280_pdata[9] = {0};	// p calibration data
//static s32 t_fine;				// temperature for pressure calculate

int bmp280_i2c_setup(u8 slave_addr)
{
    PlatformI2CParam i2cParam;
	
	i2cParam.speed = PLATFORM_I2C_SPEED_FAST;
	i2cParam.slaveAddr = slave_addr;	

    return platform_i2c_setup( bmp280_i2c_id, &i2cParam);
}

int bmp280_i2c_close() 
{
    return platform_i2c_close( bmp280_i2c_id );
}

void bmp280_i2c_write_byte(u8 reg_addr, u8 dat)
{
	u16 slave_addr = I2C_NULL_SLAVE_ADDR;
	u8 buf[1];
	buf[0] = dat;
	platform_i2c_send_data( bmp280_i2c_id, slave_addr, &reg_addr, buf, 1);
}

void bmp280_i2c_read_bytes(u8 reg_addr, int len, u8 * dat) 
{
	u16 slave_addr = I2C_NULL_SLAVE_ADDR;
	platform_i2c_recv_data( bmp280_i2c_id, slave_addr, &reg_addr, dat, len);
	return;
}

u8 bmp280_i2c_read_byte(u8 reg_addr)
{
	u8 buf[1];
	bmp280_i2c_read_bytes(reg_addr, 1, buf);
	return buf[0];
}

void bmp280_init() 
{
	bmp280_i2c_write_byte(0xf4,0x33);
	bmp280_i2c_write_byte(0xf5,0x44);
	
	bmp280_tdata[0] = (bmp280_i2c_read_byte(0x89) << 8) | bmp280_i2c_read_byte(0x88);
	bmp280_tdata[1] = (bmp280_i2c_read_byte(0x8b) << 8) | bmp280_i2c_read_byte(0x8a);
	bmp280_tdata[2] = (bmp280_i2c_read_byte(0x8d) << 8) | bmp280_i2c_read_byte(0x8c);
	
	bmp280_pdata[0] = (bmp280_i2c_read_byte(0x8f) << 8) | bmp280_i2c_read_byte(0x8e);
	bmp280_pdata[1] = (bmp280_i2c_read_byte(0x91) << 8) | bmp280_i2c_read_byte(0x90);
	bmp280_pdata[2] = (bmp280_i2c_read_byte(0x93) << 8) | bmp280_i2c_read_byte(0x92);
	bmp280_pdata[3] = (bmp280_i2c_read_byte(0x95) << 8) | bmp280_i2c_read_byte(0x94);
	bmp280_pdata[4] = (bmp280_i2c_read_byte(0x97) << 8) | bmp280_i2c_read_byte(0x96);
	bmp280_pdata[5] = (bmp280_i2c_read_byte(0x99) << 8) | bmp280_i2c_read_byte(0x98);
	bmp280_pdata[6] = (bmp280_i2c_read_byte(0x9b) << 8) | bmp280_i2c_read_byte(0x9a);
	bmp280_pdata[7] = (bmp280_i2c_read_byte(0x9d) << 8) | bmp280_i2c_read_byte(0x9c);
	bmp280_pdata[8] = (bmp280_i2c_read_byte(0x9f) << 8) | bmp280_i2c_read_byte(0x9e);
	
}

void bmp280_reset() 
{
	bmp280_i2c_write_byte(0xe0, 0x86);
	platform_os_sleep(50);
}
  
double bmp280_temperature() 
{
	s32 adc_t;
	double var1, var2, temperature;
	
	adc_t = (bmp280_i2c_read_byte(0xfa)<<12)|(bmp280_i2c_read_byte(0xfb)<<4)|(bmp280_i2c_read_byte(0xfc)>>4);
	  
    var1 = (((double) adc_t) / 16384.0 - ((double) bmp280_tdata[0]) / 1024.0)  
            * ((double) bmp280_tdata[1]);  
    var2 = ((((double) adc_t) / 131072.0 - ((double) bmp280_tdata[0]) / 8192.0)  
            * (((double) adc_t) / 131072.0 - ((double) bmp280_tdata[0]) / 8192.0))  
            * ((double) bmp280_tdata[2]); 
	//t_fine = (s32) (var1 + var2);
    temperature = (var1 + var2) / 5120.0;  
  
    return temperature;  
}

double bmp280_pressure(double temperature) 
{
	s32 adc_p;
	double var1, var2, pressure, t_fine; 
	
	t_fine =  temperature * 5120.0;
	adc_p = (bmp280_i2c_read_byte(0xf7)<<12)|(bmp280_i2c_read_byte(0xf8)<<4)|(bmp280_i2c_read_byte(0xf9)>>4);
	     
    var1 = ((double) t_fine / 2.0) - 64000.0;  
    var2 = var1 * var1 * ((double) bmp280_pdata[5]) / 32768.0;  
    var2 = var2 + var1 * ((double) bmp280_pdata[4]) * 2.0;  
    var2 = (var2 / 4.0) + (((double) bmp280_pdata[3]) * 65536.0);  
    var1 = (((double) bmp280_pdata[2]) * var1 * var1 / 524288.0  
            + ((double) bmp280_pdata[1]) * var1) / 524288.0;  
    var1 = (1.0 + var1 / 32768.0) * ((double) bmp280_pdata[0]);  
  
    if (var1 == 0.0) {  
        return 0; // avoid exception caused by division by zero  
    }  
  
    pressure = 1048576.0 - (double) adc_p;  
    pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;  
    var1 = ((double) bmp280_pdata[8]) * pressure * pressure / 2147483648.0;  
    var2 = pressure * ((double) bmp280_pdata[7]) / 32768.0;  
    pressure = pressure + (var1 + var2 + ((double) bmp280_pdata[6])) / 16.0;  
  
    return pressure;  
}


// Lua: bmp280.init(i2c_id, i2c_addr)
static int lua_bmp280_init(lua_State *L) 
{
	//int ret;
	bmp280_i2c_id = luaL_checkinteger(L, 1);
	bmp280_i2c_addr = luaL_checkinteger(L, 2);
	bmp280_i2c_setup(bmp280_i2c_addr);
	MOD_CHECK_ID( i2c, bmp280_i2c_id );
	
	bmp280_init();
	
	bmp280_i2c_close(); 
	return 0;
}

// Lua: bmp280.reset()
static int lua_bmp280_reset(lua_State *L) 
{
	//int ret;
	bmp280_i2c_setup(bmp280_i2c_addr);
	MOD_CHECK_ID( i2c, bmp280_i2c_id );
	
	bmp280_reset();
	
	bmp280_i2c_close(); 
	return 0;
}

// Lua: pressure = bmp280.pressure()
static int lua_bmp280_pressure(lua_State *L) 
{
	//int ret;
	int pressure, temperature;
	bmp280_i2c_setup(bmp280_i2c_addr);
	MOD_CHECK_ID( i2c, bmp280_i2c_id );
	
	temperature = bmp280_temperature();
	pressure = bmp280_pressure(temperature);
	
	bmp280_i2c_close(); 
	lua_pushinteger(L, (s32)pressure);
	return 1;
}

// Lua: temperature = bmp280.temperature()
static int lua_bmp280_temperature(lua_State *L) 
{
	int temperature;
	
	bmp280_i2c_setup(bmp280_i2c_addr);
	MOD_CHECK_ID( i2c, bmp280_i2c_id );
	
	temperature = bmp280_temperature();
	
	bmp280_i2c_close(); 
	lua_pushinteger(L, (s32)temperature);
	return 1;
}

const LUA_REG_TYPE bmp280_map[] = 
{
	{ LSTRKEY( "init" ),  				LFUNCVAL( lua_bmp280_init ) },
	{ LSTRKEY( "reset" ),  				LFUNCVAL( lua_bmp280_reset ) },
	{ LSTRKEY( "pressure" ),  			LFUNCVAL( lua_bmp280_pressure ) },
	{ LSTRKEY( "temperature" ), 		LFUNCVAL( lua_bmp280_temperature ) },

	{ LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_bmp280( lua_State *L )
{
  luaL_register( L, AUXLIB_BMP280, bmp280_map );
  
  return 1;
}
