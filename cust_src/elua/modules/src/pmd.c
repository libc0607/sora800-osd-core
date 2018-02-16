/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    pmd.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/3/28
 *
 * Description:
 *          lua.pmd�� ��Դ�����
 **************************************************************************/

#include <stdlib.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "platform.h"
#include "lrotable.h"
#include "platform_conf.h"
#include "platform_pmd.h"

/*+\NEW\liweiqiang\2013.9.8\����pmd.init���ó������ӿ� */
static int getFiledInt(lua_State *L, int index, const char *key, int defval)
{
    lua_getfield(L, index, key);
    return luaL_optint(L, -1, defval);
}

// pmd.init
static int pmd_init(lua_State *L) {
    #define GET_FIELD_VAL(fIELD, dEFault) pmdcfg.fIELD = getFiledInt( L, 1, #fIELD, dEFault)

    PlatformPmdCfg pmdcfg;

    luaL_checktype(L, 1, LUA_TTABLE);
    
/*+\NEW\liweiqiang\2014.2.8\���Ƶ�Դ�������ýӿ� */
    GET_FIELD_VAL(battFullLevel, PMD_CFG_INVALID_VALUE);
    GET_FIELD_VAL(battRechargeLevel, PMD_CFG_INVALID_VALUE);
    GET_FIELD_VAL(poweronLevel, PMD_CFG_INVALID_VALUE);
    GET_FIELD_VAL(poweroffLevel, PMD_CFG_INVALID_VALUE);
    GET_FIELD_VAL(currentFirst, PMD_CFG_INVALID_VALUE);
    GET_FIELD_VAL(battlevelFirst, PMD_CFG_INVALID_VALUE);
    GET_FIELD_VAL(currentSecond, PMD_CFG_INVALID_VALUE);
    GET_FIELD_VAL(battlevelSecond, PMD_CFG_INVALID_VALUE);
    GET_FIELD_VAL(currentThird, PMD_CFG_INVALID_VALUE);
    /*+\NEW\zhuth\2015.2.28\֧�ֳ��ʱ������������*/
    GET_FIELD_VAL(intervaltimeFirst, PMD_CFG_INVALID_VALUE);
    GET_FIELD_VAL(intervaltimeSecond, PMD_CFG_INVALID_VALUE);
    GET_FIELD_VAL(intervaltimeThird, PMD_CFG_INVALID_VALUE);
    /*-\NEW\zhuth\2015.2.28\֧�ֳ��ʱ������������*/
    /*+\NEW\zhuth\2014.11.6\��Դ�������ò���������Ƿ����ص�����*/
    GET_FIELD_VAL(batdetectEnable, PMD_CFG_INVALID_VALUE);
    /*-\NEW\zhuth\2014.11.6\��Դ�������ò���������Ƿ����ص�����*/
    /*+\NEW\zhuth\2015.2.26\֧���������������*/
    GET_FIELD_VAL(pluschgctlEnable, PMD_CFG_INVALID_VALUE);
    GET_FIELD_VAL(pluschgonTime, PMD_CFG_INVALID_VALUE);
    GET_FIELD_VAL(pluschgoffTime, PMD_CFG_INVALID_VALUE);
    /*-\NEW\zhuth\2015.2.26\֧���������������*/
/*-\NEW\liweiqiang\2014.2.8\���Ƶ�Դ�������ýӿ� */

    lua_pushinteger(L, platform_pmd_init(&pmdcfg));

    return 1;
}
/*-\NEW\liweiqiang\2013.9.8\����pmd.init���ó������ӿ� */

// pmd.ldoset
static int pmd_ldo_set(lua_State *L) {
    int total = lua_gettop(L);
    int level = luaL_checkinteger(L, 1);
    int i;
    int ldo;

    for(i = 2; i <= total; i++)
    {
        ldo = luaL_checkinteger(L, i);
        platform_ldo_set(ldo, level);
    }

    return 0; 
}

/*
present  battStatus BOOL
voltage ��ص�ѹbattVolt  INT
level ��صȼ� battLevel INT 
charger ������Ƿ�����chargerStatus BOOL
state ���״̬chargeState INT
*/
/*+NEW\zhuwangbin\2017.2.10\��ӳ�������ѯ�ӿ�*/
static int pmd_chg_param_get(lua_State *L)
{
	BOOL    battStatus;
    BOOL    chargerStatus;
    u8      chargeState;
    u8      battLevel;
    u16     battVolt;

	platform_pmd_get_chg_param(&battStatus, &battVolt, &battLevel, &chargerStatus, &chargeState);
	lua_pushboolean(L, battStatus);
	lua_pushinteger(L, battVolt);
	lua_pushinteger(L, battLevel);
	lua_pushboolean(L, chargerStatus);
	lua_pushinteger(L, chargeState);
	
	return 5;
}
/*+NEW\zhuwangbin\2017.2.10\��ӳ�������ѯ�ӿ�*/


// pmd.sleep(sleepornot)
static int pmd_deepsleep(lua_State *L) {    
    int sleep = luaL_checkinteger(L,1);

    platform_pmd_powersave(sleep);
    return 0; 
}

/*+\NEW\liweiqiang\2014.2.13\����pmd.charger��ѯ�����״̬�ӿ� */
//pmd.charger()
static int pmd_charger(lua_State *L) {
    lua_pushboolean(L, platform_pmd_get_charger());
    return 1;
}
/*-\NEW\liweiqiang\2014.2.13\����pmd.charger��ѯ�����״̬�ӿ� */

#define MIN_OPT_LEVEL 2
#include "lrodefs.h"  

// Module function map
const LUA_REG_TYPE pmd_map[] =
{ 
/*+\NEW\liweiqiang\2013.9.8\����pmd.init���ó������ӿ� */
  { LSTRKEY( "init" ),  LFUNCVAL( pmd_init ) },
/*-\NEW\liweiqiang\2013.9.8\����pmd.init���ó������ӿ� */
  { LSTRKEY( "ldoset" ),  LFUNCVAL( pmd_ldo_set ) },
  { LSTRKEY( "sleep" ),  LFUNCVAL( pmd_deepsleep ) },
  /*+\NEW\liweiqiang\2014.2.13\����pmd.charger��ѯ�����״̬�ӿ� */
  { LSTRKEY( "charger" ),  LFUNCVAL( pmd_charger ) },
  /*-\NEW\liweiqiang\2014.2.13\����pmd.charger��ѯ�����״̬�ӿ� */
/*+NEW\zhuwangbin\2017.2.10\��ӳ�������ѯ�ӿ�*/
  { LSTRKEY( "param_get" ),  LFUNCVAL( pmd_chg_param_get ) },
/*+NEW\zhuwangbin\2017.2.10\��ӳ�������ѯ�ӿ�*/

  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_pmd( lua_State *L )
{
    luaL_register( L, AUXLIB_PMD, pmd_map );

    MOD_REG_NUMBER(L, "LDO_KEYPAD", PLATFORM_LDO_KEYPAD);
    MOD_REG_NUMBER(L, "LDO_LCD", PLATFORM_LDO_LCD);

/*+\NEW\liweiqiang\2013.5.8\����KP_LEDR,G,B����*/
    MOD_REG_NUMBER(L, "KP_LEDR", PLATFORM_LDO_KP_LEDR);
    MOD_REG_NUMBER(L, "KP_LEDG", PLATFORM_LDO_KP_LEDG);
    MOD_REG_NUMBER(L, "KP_LEDB", PLATFORM_LDO_KP_LEDB);
/*-\NEW\liweiqiang\2013.5.8\����KP_LEDR,G,B����*/

/*+\NEW\liweiqiang\2013.6.1\����LDO_VIB���ƽӿ�*/
    MOD_REG_NUMBER(L, "LDO_VIB", PLATFORM_LDO_VIB);
/*-\NEW\liweiqiang\2013.6.1\����LDO_VIB���ƽӿ�*/

/*+\NEW\liweiqiang\2013.10.10\����LDO_VLCD����POWER_VLCD*/
    MOD_REG_NUMBER(L, "LDO_VLCD", PLATFORM_LDO_VLCD);
/*-\NEW\liweiqiang\2013.10.10\����LDO_VLCD����POWER_VLCD*/

/*+\NEW\liweiqiang\2013.11.8\����LDO_VASW,VMMC����*/
    MOD_REG_NUMBER(L, "LDO_VASW", PLATFORM_LDO_VASW);
    MOD_REG_NUMBER(L, "LDO_VMMC", PLATFORM_LDO_VMMC);
/*-\NEW\liweiqiang\2013.11.8\����LDO_VASW,VMMC����*/

/*+\new\liweiqiang\2014.5.9\����LDO_VCAM���� */
    MOD_REG_NUMBER(L, "LDO_VCAM", PLATFORM_LDO_VCAM);
/*-\new\liweiqiang\2014.5.9\����LDO_VCAM���� */
    return 1;
}  
