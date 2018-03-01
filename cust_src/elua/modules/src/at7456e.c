// at7456e osd Driver.

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "auxmods.h"
#include "lrotable.h"
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
#include "at7456e.h"

// IO define
static int at7456e_pin_ncs = 0;
static int at7456e_pin_sck = 0;
static int at7456e_pin_sdi = 0;
static int at7456e_pin_sdo = 0;

// ����io�ڵĺ���
void at7456e_set_pinval(int pin, int val) 
{
	platform_pio_op( PLATFORM_IO_GET_PORT( pin ) , 1 << (PLATFORM_IO_GET_PIN( pin )) , (val == 1 ? PLATFORM_IO_PIN_SET : PLATFORM_IO_PIN_CLEAR ) );
}

int at7456e_get_pinval(int pin)
{
	return platform_pio_op( PLATFORM_IO_GET_PORT( pin ) , 1 << (PLATFORM_IO_GET_PIN( pin )) , PLATFORM_IO_PIN_GET );
}

int at7456e_verify_pin_lua(lua_State* L, int pin) 
{
	int port_id, pin_id;
	port_id = PLATFORM_IO_GET_PORT( pin );
	pin_id = PLATFORM_IO_GET_PIN( pin );
	if( PLATFORM_IO_IS_PORT( pin ) || !platform_pio_has_port( port_id ) || !platform_pio_has_pin( port_id, pin_id ) )
		return luaL_error( L, "invalid pin" );
	return 0;
}

// ioģ��spi��
// ������Ҫ��Ӻ��ʵ� nop ָ�������ʱ�������� SPI ʱ������������ 10MHz
// rda8955�����ƵӦ����100MHz������ �����ֻҪ����10^1����Ļ������ڼ��ɣ�
// д��10�����ԣ�
void delay_spi() 
{
	for (int i=0; i<10; i++);
}

// ��ָ���Ĵ���д 8 λ��
void write_at7456_addr_data(unsigned char addr, unsigned char dat)
{ 
	unsigned char i;
	
	at7456e_set_pinval(at7456e_pin_sck, 0);
	at7456e_set_pinval(at7456e_pin_ncs, 0);
	delay_spi(); 
	
	// д��ַ
	for (i=0; i<8; i++) { 
		at7456e_set_pinval(at7456e_pin_sdi, ((addr & 0x80) == 0)? 0: 1);
		//SDIN = ((addr & 0x80) == 0)? 0: 1;
		addr = addr << 1;
		at7456e_set_pinval(at7456e_pin_sck, 1);
		delay_spi(); 
		at7456e_set_pinval(at7456e_pin_sck, 0);
	} 
	
	// д����
	for (i=0; i<8; i++) { 
		at7456e_set_pinval(at7456e_pin_sdi, ((dat & 0x80) == 0)? 0: 1);
		// SDIN = ((dat & 0x80) == 0)? 0: 1;
		dat = dat << 1;
		at7456e_set_pinval(at7456e_pin_sck, 1);
		delay_spi();
		at7456e_set_pinval(at7456e_pin_sck, 0);
	} 
		at7456e_set_pinval(at7456e_pin_ncs, 1);
		at7456e_set_pinval(at7456e_pin_sck, 1);
		at7456e_set_pinval(at7456e_pin_sdi, 1);
}

// ��ָ���Ĵ�����ȡ 8 λ����
unsigned char read_at7456_addr_data(unsigned char addr) 
{
	unsigned char i, r; 
	at7456e_set_pinval(at7456e_pin_sck, 0); 
	r = 0;
	at7456e_set_pinval(at7456e_pin_ncs, 0);
	addr |= 0x80; // D7 = 1 Ϊ���Ĵ���
	
	for (i=0; i<8; i++) { // д��ַ
		at7456e_set_pinval(at7456e_pin_sdi, ((addr & 0x80) == 0)? 0: 1);
		//SDIN = ((addr & 0x80) == 0)? 0: 1;
		addr = addr << 1;
		at7456e_set_pinval(at7456e_pin_sck, 1);
		delay_spi(); 
		at7456e_set_pinval(at7456e_pin_sck, 0);
	} 
	for (i=0; i<8; i++) { // ������
		r = r << 1; 
		//if (SDOUT)
		if (at7456e_get_pinval(at7456e_pin_sdo) == 1)	
			r |= 1; 
		at7456e_set_pinval(at7456e_pin_sck, 1); 
		delay_spi(); 
		at7456e_set_pinval(at7456e_pin_sck, 0); 
	} 
	
	at7456e_set_pinval(at7456e_pin_ncs, 1); 
	at7456e_set_pinval(at7456e_pin_sck, 1); 
	at7456e_set_pinval(at7456e_pin_sdi, 1); 
	
	return r; 
}

void write_at7456_data(unsigned char dat)
{ 
	unsigned char i;
	at7456e_set_pinval(at7456e_pin_sck, 0);
	at7456e_set_pinval(at7456e_pin_ncs, 0);
	delay_spi();
	for (i=0; i<8; i++) {  // д��ַ
		at7456e_set_pinval(at7456e_pin_sdi, ((dat & 0x80) == 0)? 0: 1);
		//SDIN = ((dat & 0x80) == 0)? 0: 1;
		dat = dat << 1;
		at7456e_set_pinval(at7456e_pin_sck, 1);
		delay_spi();
		at7456e_set_pinval(at7456e_pin_sck, 0);
	} 
	
	at7456e_set_pinval(at7456e_pin_ncs, 1);
	at7456e_set_pinval(at7456e_pin_sck, 1);
	at7456e_set_pinval(at7456e_pin_sdi, 1);
} 

unsigned char read_at7456_data(void)
{ 
	unsigned char i, r;
	at7456e_set_pinval(at7456e_pin_sck, 0);
	r = 0;
	at7456e_set_pinval(at7456e_pin_ncs, 0);
	delay_spi();
	
	for (i=0; i<8; i++) { // ������
		r = r << 1;
		if (at7456e_get_pinval(at7456e_pin_sdo) == 1)
			r |= 1;
		at7456e_set_pinval(at7456e_pin_sck, 1);
		delay_spi();
		at7456e_set_pinval(at7456e_pin_sck, 0);
	}
	
	at7456e_set_pinval(at7456e_pin_ncs, 1);
	at7456e_set_pinval(at7456e_pin_sck, 1);
	at7456e_set_pinval(at7456e_pin_sdi, 1);
	
	return r;
} 

VER7456 at7456e_check_chip_version(void)
{
	unsigned char r1, r2;
	r1 = read_at7456_addr_data(VM0);
	r2 = (r1&~(1<<1))|0x88; 		// VM0.1(Software Reset Bit) = 0��ͬʱ�� VM0.3 ��λ 1
	write_at7456_addr_data(VM0, r2); // д VM0
	platform_os_sleep(1);
	r2 = read_at7456_addr_data(VM0) & 0x88;
	write_at7456_addr_data(VM0, r1); // �ָ� VM0
	platform_os_sleep(1);
	
	if (r2 == 0x88)
		return NEW7456; // �°� 7456
	else if (r2 == 0x08)
		return OLD7456; // �ϰ� 7456
	else
		return BAD7456; // SPI �ӿ��쳣
}
 
void at7456e_reset(void)
{
	// �������λ����Ҫ�����û������ nRESET ���ⲿ 7456
	write_at7456_addr_data(VM0, 2);
	//delay_ms(60); // ��λʱ������Ϊ 50ms
}

// ��ʾ���ַ���Ϊ�ַ��������Լ��ո�
void at7456e_display_string(unsigned char row, unsigned char col, unsigned char *s)
{
	unsigned int kk;
	unsigned char c;
	
	kk = row * 30 + col;
	write_at7456_addr_data(DMAH, kk / 256); // address
	write_at7456_addr_data(DMAL, kk % 256);
	write_at7456_addr_data(DMM, 0x01); // Auto Inc
	
	c = *s++;
	while (c != 0) { 
		if ((c >= '0') && (c <= '9'))
			write_at7456_data ((c == '0')? 10: c-'1'+1);
		else if ((c >= 'A') && (c <= 'Z'))
			write_at7456_data(c - 'A' + 11);
		else if  ((c >= 'a') &&(c <= 'z'))
			write_at7456_data(c - 'a'+37);
		else if (c == ' ')
			write_at7456_data(0x00);
		else if (c == '(')
			write_at7456_data(0x3f);
		else if (c == ')')
			write_at7456_data(0x40);
		else if (c == '.')
			write_at7456_data(0x41);
		else if (c == '?')
			write_at7456_data(0x42);
		else if (c == ';')
			write_at7456_data(0x43);
		else if (c == ':')
			write_at7456_data(0x44);
		else if (c == ',')
			write_at7456_data(0x45);
		else if (c == '\'')
			write_at7456_data(0x46);
		else if (c == '/')
			write_at7456_data(0x47);
		else if (c == '"')
			write_at7456_data(0x48);
		else if (c == '-')
			write_at7456_data(0x49);
		else if (c == '<')
			write_at7456_data(0x4a);
		else if (c == '>')
			write_at7456_data(0x4b);
		else if (c == '@')
			write_at7456_data(0x4c);
		else
			write_at7456_data(c);
		c = *s++;
		kk++;
	}
	write_at7456_data(0xff); // Exit Auto Inc
}

// OSD 1��0��
void at7456e_osd(int enable)
{
	unsigned char k;
	k = read_at7456_addr_data(VM0);
	if (enable) {
		write_at7456_addr_data(VM0, k|(1<<3)); // VM0[3]=1���� OSD
	}
	else {
		write_at7456_addr_data(VM0, k&~(1<<3)); // VM0[3]=0����ֹ OSD
	}
	platform_os_sleep(1);
}

// ������0 ��ַΪ�ո�
void at7456e_clear_screen(void)
{
	unsigned int i;
	write_at7456_addr_data(DMAH, 0x00); // address
	write_at7456_addr_data(DMAL, 0);
	write_at7456_addr_data(DMM, 0x01); // ��ַ�Զ�����
	for (i=0; i <(16*30); i++) 
		write_at7456_data(0);
	
	write_at7456_data(0xff); // �˳��Զ�����ģʽ
}

// ͨ��д��/���� VM0.7 ���ж� AT7456 �İ汾���⣬�°���Զ�д VM0.7 λ
// ��ֹ SPI �ӿڿ�·���·����Ҫ�� 0��1 ����״̬
VER7456 at7456e_check_version(void)
{
	unsigned char r1, r2;
	r1 = read_at7456_addr_data(VM0);
	r2 = (r1&~(1<<1))|0x88; 		// VM0.1(Software Reset Bit) = 0��ͬʱ�� VM0.3 ��λ 1
	write_at7456_addr_data(VM0, r2); // д VM0
	platform_os_sleep(1);
	r2 = read_at7456_addr_data(VM0) & 0x88;
	write_at7456_addr_data(VM0, r1); // �ָ� VM0
	platform_os_sleep(1);
	
	if (r2 == 0x88)
		return NEW7456; // �°� 7456
	else if (r2 == 0x08)
		return OLD7456; // �ϰ� 7456
	else
		return BAD7456; // SPI �ӿ��쳣
}

void at7456e_osd_autoblacklevel(int enable)
{
	unsigned char k;
	k = read_at7456_addr_data(OSDBL);
	
	if (enable)
		k &= ~(1<<4);		// ��λ�� OSDBL.4 ǿ������ --- ǿ�ƴ��Զ���ƽ����
	else
		k |= (1<<4);		// ��λ�� OSDBL.4 ǿ���� 1 --- ǿ�ƹر��Զ���ƽ����
	
	write_at7456_addr_data(OSDBL, k);
}



// =============== Lua ==========================

// Lua: at7456e.init(ncs, sck, sdi, sdo)   
static int lua_at7456e_init(lua_State* L) 
{
	at7456e_pin_ncs = luaL_checkinteger(L, 1);
	at7456e_pin_sck = luaL_checkinteger(L, 2);
	at7456e_pin_sdi = luaL_checkinteger(L, 3);
	at7456e_pin_sdo = luaL_checkinteger(L, 4);
	
	lua_pushinteger(L, (at7456e_check_chip_version() == NEW7456)? 1: 0);
	return 1;
}

// Lua: at7456e.reset()
// always return success.
// needs an external delay for at least 60ms.
static int lua_at7456e_reset(lua_State* L) 
{
	at7456e_reset();
	lua_pushinteger(L, 1);
	return 1;
}

static int lua_at7456e_osd(lua_State* L) 
{
	int enable = luaL_checkinteger(L, 1);
	at7456e_osd(enable);
	lua_pushinteger(L, 1);
	return 1;
}

static int lua_at7456e_clear(lua_State* L) 
{
	at7456e_clear_screen();
	lua_pushinteger(L, 1);
	return 1;
}

// Lua: at7456e.puttext(x, y, str)
static int lua_at7456e_puttext(lua_State* L) 
{
	int put_x, put_y, length;
	char * pBuf;
	const char * pBuf_const;
	
	put_x = luaL_checkinteger(L, 1);
	put_y = luaL_checkinteger(L, 2);
	pBuf_const = lua_tolstring(L, 3, &length);
	
	pBuf = (char*)platform_malloc(length + 1);
	memset(pBuf, 0, length + 1);
	memcpy(pBuf, pBuf_const, length);
	
	at7456e_display_string(put_y, put_x, pBuf); 
	
	platform_free(pBuf);
	lua_pushinteger(L, 1);
	return 1;
}



/*
  at7456e.init(ncs, sck, sdi, sdo)   
  at7456e.reset()
  at7456e.osd()
  at7456e.clear()
  at7456e.puttext(x, y, str)
//To-do
//at7456e.automode()
//at7456e.putimage(sx,sy,px,py,dx,dy[,l,b,i])
//at7456e.writefont(uart_id)
*/
#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE at7456e_map[] = 
{
	{ LSTRKEY( "init" ),  				LFUNCVAL( lua_at7456e_init ) },
	{ LSTRKEY( "reset" ),  				LFUNCVAL( lua_at7456e_reset ) },
	{ LSTRKEY( "osd" ), 				LFUNCVAL( lua_at7456e_osd ) },
	{ LSTRKEY( "clear" ), 				LFUNCVAL( lua_at7456e_clear ) },
	{ LSTRKEY( "puttext" ), 			LFUNCVAL( lua_at7456e_puttext ) },
//	{ LSTRKEY( "automode" ),  			LFUNCVAL( lua_at7456e_automode ) },
//	{ LSTRKEY( "putimage" ), 			LFUNCVAL( lua_at7456e_putimage ) },
//	{ LSTRKEY( "writefont" ),  			LFUNCVAL( lua_at7456e_writefont ) },

	{ LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_at7456e( lua_State *L )
{
	luaL_register( L, AUXLIB_AT7456E, at7456e_map );

	// Set it as its own metatable
	lua_pushvalue( L, -1 );
	lua_setmetatable( L, -2 );

	MOD_REG_NUMBER( L, "SUCCESS", 1 );
	MOD_REG_NUMBER( L, "FAILED", 0 );

	return 1;
}