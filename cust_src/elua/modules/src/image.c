/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    rtos.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/3/7
 *
 * Description:
 *          lua.rtos¿â
 **************************************************************************/

#include <ctype.h>
#include <string.h>
#include <malloc.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "auxmods.h"
#include "lrotable.h"
#include "platform.h"
#include "platform_conf.h"
#include "platform_malloc.h"
#include "platform_image.h"

#define META_NAME                 "image"
#define image_check( L )      ( image_t* )luaL_checkudata( L, 1, META_NAME )


static int l_image_load(lua_State *L)
{
    int ret;
    image_t image_info;  
    image_t* p_image_info;
    const char *filename = luaL_checkstring(L,1);
  
    memset(&image_info,0,sizeof(image_t));

    ret = platform_image_decode_jpeg(filename,&image_info);

    printf("l_image_load: %d \n", ret);

    if (ret == 0)
    {
        p_image_info = (image_t*)lua_newuserdata(L, sizeof(image_t));
        p_image_info->width = image_info.width;
        p_image_info->height= image_info.height;
        p_image_info->format= image_info.format;
        p_image_info->buffer= image_info.buffer;
    }else
    {
        lua_pushnil(L);
    }

    luaL_getmetatable( L, META_NAME );
    lua_setmetatable( L, -2 );
    return 1;
}

static int l_image_info(lua_State *L)
{
    image_t* image_info = (image_t*)image_check(L);

    printf("l_image_info image_info:%d\n",image_info);

    printf("l_image_info wd:%d\n",image_info->width);
    printf("l_image_info height:%d\n",image_info->height);
    printf("l_image_info fm:%d\n",image_info->format);

    if(image_info)
    {
        lua_pushinteger( L, image_info->width );
        lua_pushinteger( L, image_info->height );
        lua_pushinteger( L, image_info->format);
    }else
    {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 3;
}

static int l_image_buffer(lua_State *L)
{
    image_t* image_info = (image_t*)image_check(L);

    if(image_info)
    {
        lua_pushlightuserdata(L, image_info->buffer);
    }else
    {
        lua_pushnil(L);
    }

    return 1;
}

static int l_image_destory(lua_State *L)
{
    image_t* image_info = (image_t*)image_check(L);

    if (image_info && image_info->buffer)
    {
        platform_free_image_jpeg_decodedata(image_info->buffer);//free(image_info->buffer);
    }

    return 0;
}




#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE image_map[] =
{
    { LSTRKEY( "load" ),  LFUNCVAL( l_image_load ) },
    
	{ LNILKEY, LNILVAL }
};

const LUA_REG_TYPE image_mt_map[] =
{
    { LSTRKEY( "info" ), LFUNCVAL( l_image_info ) },
    { LSTRKEY( "buffer" ), LFUNCVAL( l_image_buffer ) },
    { LSTRKEY( "destory" ), LFUNCVAL( l_image_destory ) },
	{ LNILKEY, LNILVAL }
};

int luaopen_image( lua_State *L )
{
    luaL_newmetatable( L, META_NAME );
    lua_pushvalue(L,-1);
    lua_setfield(L, -2, "__index");
    luaL_register( L, NULL, image_mt_map );
    luaL_newmetatable( L, META_NAME );
    luaL_register( L, AUXLIB_IMAGE, image_map );

    return 1;
}

