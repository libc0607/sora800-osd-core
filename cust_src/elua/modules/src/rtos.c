/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    rtos.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/3/7
 *
 * Description:
 *          lua.rtos��
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
/*begin\NEW\zhutianhua\2017.2.28 14:38\����rtos.set_trace�ӿڣ��ɿ����Ƿ����Lua��trace*/
#include "platform_conf.h"
/*end\NEW\zhutianhua\2017.2.28 14:38\����rtos.set_trace�ӿڣ��ɿ����Ƿ����Lua��trace*/
#include "platform_rtos.h"
#include "platform_malloc.h"

static void setfieldInt(lua_State *L, const char *key, int value)
{
    lua_pushstring(L, key);
    lua_pushinteger(L, value);
    lua_rawset(L, -3);// ����key,value ���õ�table��
}

static void setfieldBool(lua_State *L, const char *key, int value)
{
    if(value < 0) // invalid value
        return;

    lua_pushstring(L, key);
    lua_pushboolean(L, value);
    lua_rawset(L, -3);// ����key,value ���õ�table��
}

static int handle_msg(lua_State *L, PlatformMessage *pMsg)
{    
    int ret = 1;
    
    switch(pMsg->id)
    {
    case RTOS_MSG_WAIT_MSG_TIMEOUT:
        lua_pushinteger(L, pMsg->id);
        // no error msg data.
        break;
        
    case RTOS_MSG_TIMER:
        lua_pushinteger(L, pMsg->id);
        lua_pushinteger(L, pMsg->data.timer_id);
        ret = 2;
        break;

    case RTOS_MSG_UART_RX_DATA:
    case RTOS_MSG_UART_TX_DONE:
        lua_pushinteger(L, pMsg->id);
        lua_pushinteger(L, pMsg->data.uart_id);
        ret = 2;
        break;

    case RTOS_MSG_KEYPAD:
        /* ��table��ʽ������Ϣ���� */
        lua_newtable(L);    
        setfieldInt(L, "id", pMsg->id);
        setfieldBool(L, "pressed", pMsg->data.keypadMsgData.bPressed);
        setfieldInt(L, "key_matrix_row", pMsg->data.keypadMsgData.data.matrix.row);
        setfieldInt(L, "key_matrix_col", pMsg->data.keypadMsgData.data.matrix.col);
        break;

    case RTOS_MSG_TP:
        /* ��table��ʽ������Ϣ���� */
        lua_newtable(L);    
        setfieldInt(L, "id", pMsg->id);
        setfieldInt(L, "pen_state", pMsg->data.tpData.penState);
        setfieldInt(L, "x", pMsg->data.tpData.x);
        setfieldInt(L, "y", pMsg->data.tpData.y);
        break;
        
/*+\NEW\liweiqiang\2013.4.5\����rtos.tick�ӿ�*/
    case RTOS_MSG_INT:
        /* ��table��ʽ������Ϣ���� */
        lua_newtable(L);    
        setfieldInt(L, "id", pMsg->id);
        setfieldInt(L, "int_id", pMsg->data.interruptData.id);
        setfieldInt(L, "int_resnum", pMsg->data.interruptData.resnum);
        break;
/*-\NEW\liweiqiang\2013.4.5\����rtos.tick�ӿ�*/

/*+\NEW\liweiqiang\2013.7.8\����rtos.pmd��Ϣ*/
    case RTOS_MSG_PMD:
        /* ��table��ʽ������Ϣ���� */
        lua_newtable(L);    
        setfieldInt(L, "id", pMsg->id);
        setfieldBool(L, "present", pMsg->data.pmdData.battStatus);
        setfieldInt(L, "voltage", pMsg->data.pmdData.battVolt);
        setfieldInt(L, "level", pMsg->data.pmdData.battLevel);
        setfieldBool(L, "charger", pMsg->data.pmdData.chargerStatus);
        setfieldInt(L, "state", pMsg->data.pmdData.chargeState);
        break;
/*-\NEW\liweiqiang\2013.7.8\����rtos.pmd��Ϣ*/

/*+\NEW\liweiqiang\2013.11.4\����audio.core�ӿڿ� */
    case RTOS_MSG_AUDIO:
        /* ��table��ʽ������Ϣ���� */
        lua_newtable(L);    
        setfieldInt(L, "id", pMsg->id);
        if(pMsg->data.audioData.playEndInd == TRUE)
            setfieldBool(L,"play_end_ind",TRUE);
        else if(pMsg->data.audioData.playErrorInd == TRUE)
            setfieldBool(L,"play_error_ind",TRUE);
        break;
/*-\NEW\liweiqiang\2013.11.4\����audio.core�ӿڿ� */

/*+\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
    case RTOS_MSG_ALARM:
        lua_pushinteger(L, pMsg->id);
        ret = 1;
        break;
/*-\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/

    default:
        ret = 0;
        break;
    }
    
    return ret;
}

static int l_rtos_receive(lua_State *L) 		/* rtos.receive() */
{
    u32 timeout = luaL_checkinteger( L, 1 );
    PlatformMessage *pMsg = NULL;
/*+\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
    static BOOL firstRecv = TRUE;
    int ret = 0;

    if(firstRecv)
    {
        // ��һ�ν�����Ϣʱ�����Ƿ���Ҫ����ϵͳ
        firstRecv = FALSE;
        platform_poweron_try();
    }
/*-\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */

    if(platform_rtos_receive((void**)&pMsg, timeout) != PLATFORM_OK)
    {
        return luaL_error( L, "rtos.receive error!" );
    }
    
    ret = handle_msg(L, pMsg);

    if(pMsg)
    {
    /*+\NEW\liweiqiang\2013.12.6\libc malloc��dlmallocͨ�� */
        // ����Ϣ�ڴ���ƽ̨�������߳������,����platform_free���ͷ�
        platform_free(pMsg);
    /*-\NEW\liweiqiang\2013.12.6\libc malloc��dlmallocͨ�� */
    }

    return ret;
}

static int l_rtos_sleep(lua_State *L)   /* rtos.sleep()*/
{
    int ms = luaL_checkinteger( L, 1 );

    platform_os_sleep(ms);
    
    return 0;
}

static int l_rtos_timer_start(lua_State *L)
{
    int timer_id = luaL_checkinteger(L,1);
    int ms = luaL_checkinteger(L,2);
    int ret;

    ret = platform_rtos_start_timer(timer_id, ms,FALSE);

    lua_pushinteger(L, ret);

    return 1;
}

static int l_rtos_timer_high_priority_start(lua_State *L)
{
    int timer_id = luaL_checkinteger(L,1);
    int ms = luaL_checkinteger(L,2);
    int ret;

    ret = platform_rtos_start_timer(timer_id, ms,TRUE);

    lua_pushinteger(L, ret);

    return 1;
}

static int l_rtos_timer_stop(lua_State *L)
{
    int timer_id = luaL_checkinteger(L,1);
    int ret;

    ret = platform_rtos_stop_timer(timer_id);

    lua_pushinteger(L, ret);

    return 1;
}

static int l_rtos_init_module(lua_State *L)
{
    int module_id = luaL_checkinteger(L, 1);
    int ret;

    switch(module_id)
    {
    case RTOS_MODULE_ID_KEYPAD:
        {
            PlatformKeypadInitParam param;

            int type = luaL_checkinteger(L, 2);
            int inmask = luaL_checkinteger(L, 3);
            int outmask = luaL_checkinteger(L, 4);

            param.type = type;
            param.matrix.inMask = inmask;
            param.matrix.outMask = outmask;

            ret = platform_rtos_init_module(RTOS_MODULE_ID_KEYPAD, &param);
        }
        break;

/*+\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
    case RTOS_MODULE_ID_ALARM:
    case RTOS_MODULE_ID_TP:
        {
            ret = platform_rtos_init_module(module_id, NULL);
        }
		break;
/*-\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/


    default:
        return luaL_error(L, "rtos.init_module: module id must < %d", NumOfRTOSModules);
        break;
    }

    lua_pushinteger(L, ret);

    return 1;
}

/*+\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
static int l_rtos_poweron_reason(lua_State *L)
{
    lua_pushinteger(L, platform_get_poweron_reason());
    return 1;
}

static int l_rtos_poweron(lua_State *L)
{
    int flag = luaL_checkinteger(L, 1);
    platform_rtos_poweron(flag);
    return 0;
}
/*-\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */

static int l_rtos_poweroff(lua_State *L)
{
	platform_rtos_poweroff();	
	return 0;
}

//9501GPS �ṩ32Kʱ��
static int l_rtos_sys32k_clk_out(lua_State *L)
{
    int enable = luaL_checkinteger(L, 1);
    platform_rtos_sys32k_clk_out(enable);
    return 0;
}


static int l_rtos_send_event(lua_State *L)
{
    u32 event = luaL_checkinteger(L, 1);
    platform_rtos_send_event(event);
    return 0;
}

/*-\NEW\zhuwangbin\2017.2.12\��Ӱ汾��ѯ�ӿ� */
static int l_get_version(lua_State *L)
{
	char *ver;

	ver = platform_rtos_get_version();
	lua_pushlstring(L, ver, strlen(ver));
	
	return 1;
}
/*-\NEW\zhuwangbin\2017.2.12\��Ӱ汾��ѯ�ӿ� */

static int l_get_env_usage(lua_State *L)
{
	lua_pushinteger(L,platform_get_env_usage());	
	return 1;
}

/*+\NEW\liweiqiang\2013.9.7\����rtos.restart�ӿ�*/
static int l_rtos_restart(lua_State *L)
{
	platform_rtos_restart();	
	return 0;
}
/*-\NEW\liweiqiang\2013.9.7\����rtos.restart�ӿ�*/

/*+\NEW\liweiqiang\2013.4.5\����rtos.tick�ӿ�*/
static int l_rtos_tick(lua_State *L)
{
    lua_pushinteger(L, platform_rtos_tick());
    return 1;
}
/*-\NEW\liweiqiang\2013.4.5\����rtos.tick�ӿ�*/

/*+\NEW\liweiqiang\2017.12.6\����rtos.set_time�ӿ�*/
static int l_rtos_set_time(lua_State *L){
    platform_rtos_set_time(luaL_checknumber(L, 1));
    return 0;
}
/*+\NEW\liweiqiang\2017.12.6\����rtos.set_time�ӿ�*/

/*begin\NEW\zhutianhua\2017.2.28 14:12\����rtos.set_trace�ӿڣ��ɿ����Ƿ����Lua��trace*/
static int l_set_trace(lua_State *L)
{
    u32 flag = luaL_optinteger(L, 1, 0);
    if(flag==1)
    {
        platform_set_console_port(luaL_optinteger(L, 2, PLATFORM_PORT_ID_DEBUG));
    }
    else
    {
        platform_set_console_port(NUM_UART);
    }
    lua_pushboolean(L,1);
    return 1;
}
/*end\NEW\zhutianhua\2017.2.28 14:12\����rtos.set_trace�ӿڣ��ɿ����Ƿ����Lua��trace*/

/*begin\NEW\zhutianhua\2017.9.5 30:57\����get_fs_free_size�ӿ�*/
static int l_get_fs_free_size(lua_State *L)
{
	lua_pushinteger(L, platform_fs_get_free_size());	
	return 1;
}
/*end\NEW\zhutianhua\2017.9.5 30:57\����get_fs_free_size�ӿ�*/


/*begin\NEW\zhutianhua\2017.9.26 17:28\����make_dir�ӿ�*/
static int l_make_dir(lua_State *L)
{
    size_t  len = 0;    
    char* pDir = (char*)luaL_checklstring(L, 1, &len);
    
    lua_pushboolean(L, platform_make_dir(pDir, len));	
    return 1;
}

static int l_remove_dir(lua_State *L)
{
    size_t  len = 0;    
    char* pDir = (char*)luaL_checklstring(L, 1, &len);
    
    lua_pushboolean(L, platform_remove_dir(pDir, len));	
    return 1;
}
/*end\NEW\zhutianhua\2017.9.26 17:28\����make_dir�ӿ�*/

/*+\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
static int l_rtos_setalarm(lua_State *L)
{
    PlatformSetAlarmParam alarmparam;
    
    alarmparam.alarmon = luaL_checkinteger(L, 1);
    alarmparam.year = luaL_checkinteger(L, 2);
    alarmparam.month = luaL_checkinteger(L, 3);
    alarmparam.day = luaL_checkinteger(L, 4);
    alarmparam.hour = luaL_checkinteger(L, 5);
    alarmparam.min = luaL_checkinteger(L, 6);
    alarmparam.sec = luaL_checkinteger(L, 7);

    lua_pushboolean(L, platform_rtos_setalarm(&alarmparam));	
    return 1;
}
/*-\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/

#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE rtos_map[] =
{
    { LSTRKEY( "init_module" ),  LFUNCVAL( l_rtos_init_module ) },
/*+\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
    { LSTRKEY( "poweron_reason" ),  LFUNCVAL( l_rtos_poweron_reason ) },
    { LSTRKEY( "poweron" ),  LFUNCVAL( l_rtos_poweron ) },
/*-\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
    { LSTRKEY( "poweroff" ),  LFUNCVAL( l_rtos_poweroff ) },
/*+\NEW\liweiqiang\2013.9.7\����rtos.restart�ӿ�*/
    { LSTRKEY( "restart" ),  LFUNCVAL( l_rtos_restart ) },
/*-\NEW\liweiqiang\2013.9.7\����rtos.restart�ӿ�*/
    { LSTRKEY( "receive" ),  LFUNCVAL( l_rtos_receive ) },
    //{ LSTRKEY( "send" ), LFUNCVAL( l_rtos_send ) }, //�ݲ��ṩsend�ӿ�
    { LSTRKEY( "sleep" ), LFUNCVAL( l_rtos_sleep ) },
    { LSTRKEY( "timer_start" ), LFUNCVAL( l_rtos_timer_start ) },
    { LSTRKEY( "timer_high_priority_start" ), LFUNCVAL( l_rtos_timer_high_priority_start ) },
    { LSTRKEY( "timer_stop" ), LFUNCVAL( l_rtos_timer_stop ) },
/*+\NEW\liweiqiang\2013.4.5\����rtos.tick�ӿ�*/
    { LSTRKEY( "tick" ), LFUNCVAL( l_rtos_tick ) },
/*-\NEW\liweiqiang\2013.4.5\����rtos.tick�ӿ�*/
    { LSTRKEY( "get_env_usage" ), LFUNCVAL( l_get_env_usage ) },
/*-\NEW\zhuwangbin\2017.2.12\��Ӱ汾��ѯ�ӿ� */
    { LSTRKEY( "get_version" ), LFUNCVAL( l_get_version ) },
/*-\NEW\zhuwangbin\2017.2.12\��Ӱ汾��ѯ�ӿ� */
    /*begin\NEW\zhutianhua\2017.2.28 14:4\����rtos.set_trace�ӿڣ��ɿ����Ƿ����Lua��trace*/
    { LSTRKEY( "set_trace" ), LFUNCVAL( l_set_trace ) },
    //9501GPS �ṩ32Kʱ��
    {LSTRKEY( "sys32k_clk_out" ), LFUNCVAL( l_rtos_sys32k_clk_out ) },
    /*end\NEW\zhutianhua\2017.2.28 14:4\����rtos.set_trace�ӿڣ��ɿ����Ƿ����Lua��trace*/
    { LSTRKEY( "sendevent" ), LFUNCVAL( l_rtos_send_event ) },
    /*begin\NEW\zhutianhua\2017.9.5 30:53\����get_fs_free_size�ӿ�*/
    { LSTRKEY( "get_fs_free_size" ), LFUNCVAL( l_get_fs_free_size ) },    
    /*end\NEW\zhutianhua\2017.9.5 30:53\����get_fs_free_size�ӿ�*/
    /*begin\NEW\zhutianhua\2017.9.27 19:41\����make_dir�ӿ�*/
    { LSTRKEY( "make_dir" ), LFUNCVAL( l_make_dir) },
    { LSTRKEY( "remove_dir" ), LFUNCVAL( l_remove_dir) },
    /*end\NEW\zhutianhua\2017.9.27 19:41\����make_dir�ӿ�*/
    /*+\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
    { LSTRKEY( "set_alarm" ), LFUNCVAL( l_rtos_setalarm ) },
    /*-\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
    { LSTRKEY( "set_time" ), LFUNCVAL( l_rtos_set_time ) },
	{ LNILKEY, LNILVAL }
};

int luaopen_rtos( lua_State *L )
{
    luaL_register( L, AUXLIB_RTOS, rtos_map );

    // module id
    MOD_REG_NUMBER(L, "MOD_KEYPAD", RTOS_MODULE_ID_KEYPAD);
    /*+\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
    MOD_REG_NUMBER(L, "MOD_ALARM", RTOS_MODULE_ID_ALARM);
    /*-\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
    MOD_REG_NUMBER(L, "MOD_TP", RTOS_MODULE_ID_TP);

/*+\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
    // ����ԭ��
    #define REG_POWERON_RESON(rEASON) MOD_REG_NUMBER(L, #rEASON, PLATFORM_##rEASON)
    REG_POWERON_RESON(POWERON_KEY);
    REG_POWERON_RESON(POWERON_CHARGER);
    REG_POWERON_RESON(POWERON_ALARM);
    REG_POWERON_RESON(POWERON_RESTART);
    REG_POWERON_RESON(POWERON_OTHER);
    REG_POWERON_RESON(POWERON_UNKNOWN);
/*-\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
    /*+\NewReq NEW\zhuth\2014.6.18\���ӿ���ԭ��ֵ�ӿ�*/
    REG_POWERON_RESON(POWERON_EXCEPTION);
    REG_POWERON_RESON(POWERON_HOST);
    REG_POWERON_RESON(POWERON_WATCHDOG);
    /*-\NewReq NEW\zhuth\2014.6.18\���ӿ���ԭ��ֵ�ӿ�*/

    // msg id
    MOD_REG_NUMBER(L, "WAIT_MSG_TIMEOUT", RTOS_MSG_WAIT_MSG_TIMEOUT);
    MOD_REG_NUMBER(L, "MSG_TIMER", RTOS_MSG_TIMER);
    MOD_REG_NUMBER(L, "MSG_KEYPAD", RTOS_MSG_KEYPAD);
    MOD_REG_NUMBER(L, "MSG_UART_RXDATA", RTOS_MSG_UART_RX_DATA);
    MOD_REG_NUMBER(L, "MSG_UART_TX_DONE", RTOS_MSG_UART_TX_DONE);
/*+\NEW\liweiqiang\2013.4.5\����lua gpio �ж�����*/
    MOD_REG_NUMBER(L, "MSG_INT", RTOS_MSG_INT);
/*-\NEW\liweiqiang\2013.4.5\����lua gpio �ж�����*/
/*+\NEW\liweiqiang\2013.7.8\����rtos.pmd��Ϣ*/
    MOD_REG_NUMBER(L, "MSG_PMD", RTOS_MSG_PMD);
/*-\NEW\liweiqiang\2013.7.8\����rtos.pmd��Ϣ*/
/*+\NEW\liweiqiang\2013.11.4\����audio.core�ӿڿ� */
    MOD_REG_NUMBER(L, "MSG_AUDIO", RTOS_MSG_AUDIO);
/*-\NEW\liweiqiang\2013.11.4\����audio.core�ӿڿ� */
/*+\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
    MOD_REG_NUMBER(L, "MSG_ALARM", RTOS_MSG_ALARM);
/*-\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/

    MOD_REG_NUMBER(L, "MSG_TP", RTOS_MSG_TP);

    //timeout
    MOD_REG_NUMBER(L, "INF_TIMEOUT", PLATFORM_RTOS_WAIT_MSG_INFINITE);

    // ���б�Ҫ�ĳ�ʼ��
    platform_rtos_init();

    return 1;
}

