
#include <stdlib.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "platform.h"
#include "lrotable.h"
#include "platform_conf.h"
#include "platform_pmd.h"
#include "qrencode.h"

#define META_NAME                 "qrencode"
#define qrencode_check( L )      ( QRcode* )luaL_checkudata( L, 1, META_NAME )

extern QRcode* qrencode(const unsigned char *intext, int length);

static int l_qr_create(lua_State *L) {
    size_t  len      = 0;
    int width;
    int disp_height;
    int idx;
    
    char* url_string = (char*)luaL_checklstring(L, 1, &len);
    
    QRcode* code;

    disp_height = luaL_optint(L, 2, 0);
    
    code  = qrencode(url_string, len);

    lua_pushinteger(L, code->width);
    lua_pushinteger(L, code->width);
    lua_newtable(L);

    
    for( idx = 1; idx <= code ->width * code->width; idx ++ )
    {
      lua_pushinteger( L, ((code->data[idx - 1] & 1) ? 0xff : 0x00));
      lua_rawseti( L, -2, idx );
    }
    
    qrencode_free(code);
    
    return 3; 
}

static int l_qr_info(lua_State *L)
{    
    QRcode* qr_info = (QRcode*)qrencode_check(L);  
    
    if(qr_info)    
    {        
        lua_pushinteger( L, qr_info->width ); 
        lua_pushinteger( L, qr_info->version); 
    }else    
    {        
        lua_pushnil(L);  
        lua_pushnil(L); 
    }
    
    return 2;
}

static int l_qr_buffer(lua_State *L)
{    
    QRcode* qr_info = (QRcode*)qrencode_check(L);  
    
    if(qr_info)    
    {        
        lua_pushlightuserdata(L, qr_info->data);    
    }else    
    {        
        lua_pushnil(L);    
    }
    
    return 1;
}

static int l_qr_destory(lua_State *L)
{    
    QRcode* qr_info = (QRcode*)qrencode_check(L);    
    
    if(qr_info)    
    {        
        qrencode_free(qr_info);
    }
    
    return 0;
}

static int l_qr_encode(lua_State *L) {
    size_t len = 0;
    QRcode* code;
    char* s = (char*)luaL_checklstring(L, 1, &len);
    int qr_version = luaL_optint(L, 2, 0);
    int qr_level = luaL_optint(L, 3, 0);
    
    code  = QRcode_encodeData(len, s, qr_version, qr_level);

    lua_pushinteger(L, code->width);
    lua_pushlstring(L, code->data, code->width*code->width);
    QRcode_free(code);
    
    return 2; 
}

#include "lrodefs.h"  

// Module function map
const LUA_REG_TYPE qr_encode_map[] =
{ 
  { LSTRKEY( "build" ),  LFUNCVAL( l_qr_create ) },
  { LSTRKEY("encode"), LFUNCVAL(l_qr_encode)},
  
  { LNILKEY, LNILVAL }
};


int luaopen_qr_encode( lua_State *L )
{
    luaL_register( L, AUXLIB_QRENCODE, qr_encode_map );

    return 1;
}
 

