/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    platform_rtos.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/3/7
 *
 * Description:
 *          lua rtosƽ̨�ӿ�ʵ��
 **************************************************************************/

#include "rda_pal.h"
#include "assert.h"

#include "platform.h"
#include "platform_rtos.h"
#include "time.h"

#define DEBUG_RTOS

#if defined(DEBUG_RTOS)
#define DEBUG_RTOS_TRACE(fmt, ...)      PUB_TRACE(fmt, ##__VA_ARGS__)
#else
#define DEBUG_RTOS_TRACE(fmt, ...)
#endif

#define MAX_LUA_TIMERS              40

typedef struct LuaTimerParamTag
{
    HANDLE              hOsTimer;
    int                 luaTimerId;
}LuaTimerParam;

static LuaTimerParam luaTimerParam[MAX_LUA_TIMERS];
static HANDLE hLuaTimerSem = 0;

static HANDLE           hRTOSWaitMsgTimer;

extern HANDLE g_LuaShellTaskHandle;

int platform_rtos_send(PlatformMessage *pMsg)
{
    IVTBL(send_message)(g_LuaShellTaskHandle, (PVOID)pMsg);

    return PLATFORM_OK;
}

int platform_rtos_send_high_priority(PlatformMessage *pMsg)
{
    IVTBL(send_high_priority_message)(g_LuaShellTaskHandle, (PVOID)pMsg);

    return PLATFORM_OK;
}


int platform_rtos_receive(void **ppMessage, u32 timeout)
{
    BOOL ret;

    if(timeout == PLATFORM_RTOS_WAIT_MSG_INFINITE)
    {
        IVTBL(stop_timer)(hRTOSWaitMsgTimer);
    }
    else
    {
        // start timer�ӿڻ��Զ�������ʱ�� �ʲ���Ҫstop_timer
        IVTBL(start_timer)(hRTOSWaitMsgTimer, timeout);
    }
    
    ret = IVTBL(wait_message)(g_LuaShellTaskHandle, ppMessage, 0);

    return ret == TRUE ? PLATFORM_OK : PLATFORM_ERR;
}

void platform_rtos_timer_callback(T_AMOPENAT_TIMER_PARAMETER *pParameter)
{
    int timer_id = (int)(pParameter->pParameter);
    u8 index;    
    PlatformMessage *pMsg = IVTBL(malloc)(sizeof(PlatformMessage));

    pMsg->id = RTOS_MSG_TIMER;
    pMsg->data.timer_id = timer_id;

    platform_rtos_send(pMsg);
    
    IVTBL(wait_semaphore)(hLuaTimerSem, 0);

    for(index = 0; index < MAX_LUA_TIMERS; index++)
    {
        if(luaTimerParam[index].hOsTimer == pParameter->hTimer)
        {
            if(timer_id != luaTimerParam[index].luaTimerId)
            {
                DEBUG_RTOS_TRACE("[platform_rtos_timer_callback]: warning dismatch timer %d <-> %d", timer_id, luaTimerParam[index].luaTimerId);
            }
            
            IVTBL(delete_timer)(luaTimerParam[index].hOsTimer);
            luaTimerParam[index].hOsTimer = OPENAT_INVALID_HANDLE;
            break;
        }
    }
    
    IVTBL(release_semaphore)(hLuaTimerSem);
}

void platform_rtos_timer_high_priority_callback(T_AMOPENAT_TIMER_PARAMETER *pParameter)
{
    int timer_id = (int)(pParameter->pParameter);
    u8 index;    
    PlatformMessage *pMsg = IVTBL(malloc)(sizeof(PlatformMessage));

    pMsg->id = RTOS_MSG_TIMER;
    pMsg->data.timer_id = timer_id;

    platform_rtos_send_high_priority(pMsg);
    
    IVTBL(wait_semaphore)(hLuaTimerSem, 0);

    for(index = 0; index < MAX_LUA_TIMERS; index++)
    {
        if(luaTimerParam[index].hOsTimer == pParameter->hTimer)
        {
            if(timer_id != luaTimerParam[index].luaTimerId)
            {
                DEBUG_RTOS_TRACE("[platform_rtos_timer_high_priority_callback]: warning dismatch timer %d <-> %d", timer_id, luaTimerParam[index].luaTimerId);
            }
            
            IVTBL(delete_timer)(luaTimerParam[index].hOsTimer);
            luaTimerParam[index].hOsTimer = OPENAT_INVALID_HANDLE;
            break;
        }
    }
    
    IVTBL(release_semaphore)(hLuaTimerSem);
}

int platform_rtos_start_timer(int timer_id, int milliSecond, BOOL high)
{
    u8 index;
    HANDLE hTimer = OPENAT_INVALID_HANDLE;
    
    if(!hLuaTimerSem)
    {
        hLuaTimerSem = IVTBL(create_semaphore)(1);
    }

    for(index = 0; index < MAX_LUA_TIMERS; index++)
    {
        if(luaTimerParam[index].hOsTimer != OPENAT_INVALID_HANDLE &&
            luaTimerParam[index].luaTimerId == timer_id)
        {
            platform_rtos_stop_timer(timer_id);
            break;
        }
    }

    IVTBL(wait_semaphore)(hLuaTimerSem, 0);

    for(index = 0; index < MAX_LUA_TIMERS; index++)
    {
        if(luaTimerParam[index].hOsTimer == NULL)
        {
            break;
        }
    }

    if(index >= MAX_LUA_TIMERS)
    {
        DEBUG_RTOS_TRACE("[platform_rtos_start_timer]: no timer resource.");
        goto start_timer_error;
    }

    hTimer = IVTBL(create_timer)(high ? platform_rtos_timer_high_priority_callback : platform_rtos_timer_callback, (PVOID)timer_id);

    if(OPENAT_INVALID_HANDLE == hTimer)
    {
        DEBUG_RTOS_TRACE("[platform_rtos_start_timer]: create timer failed.");
        goto start_timer_error;
    }

    if(!IVTBL(start_timer)(hTimer, milliSecond))
    {
        DEBUG_RTOS_TRACE("[platform_rtos_start_timer]: start timer failed.");
        goto start_timer_error;
    }
    
    luaTimerParam[index].hOsTimer = hTimer;
    luaTimerParam[index].luaTimerId = timer_id;
    
    IVTBL(release_semaphore)(hLuaTimerSem);
    
    return PLATFORM_OK;

start_timer_error:
    IVTBL(release_semaphore)(hLuaTimerSem);

/*+\NEW\liweiqiang\2014.7.22\����������ʱ��ʧ�ܰѶ�ʱ����Դ�ľ������� */
    if(OPENAT_INVALID_HANDLE != hTimer)
/*-\NEW\liweiqiang\2014.7.22\����������ʱ��ʧ�ܰѶ�ʱ����Դ�ľ������� */
    {
        IVTBL(delete_timer)(hTimer);
    }
    
    return PLATFORM_ERR;
}

int platform_rtos_stop_timer(int timer_id)
{
    u8 index;
    
    if(OPENAT_INVALID_HANDLE == hLuaTimerSem)
    {
        hLuaTimerSem = IVTBL(create_semaphore)(1);
    }

    IVTBL(wait_semaphore)(hLuaTimerSem, 0);

    for(index = 0; index < MAX_LUA_TIMERS; index++)
    {
        if(luaTimerParam[index].hOsTimer != OPENAT_INVALID_HANDLE &&
            luaTimerParam[index].luaTimerId == timer_id)
        {
            IVTBL(stop_timer)(luaTimerParam[index].hOsTimer);
            IVTBL(delete_timer)(luaTimerParam[index].hOsTimer);

            luaTimerParam[index].hOsTimer = OPENAT_INVALID_HANDLE;
            break;
        }
    }
    
    IVTBL(release_semaphore)(hLuaTimerSem);
    
    return PLATFORM_OK;
}

void platform_keypad_message(T_AMOPENAT_KEYPAD_MESSAGE *pKeypadMessage)
{
    PlatformMessage *pMsg = IVTBL(malloc)(sizeof(PlatformMessage));
    
    /*
    DEBUG_RTOS_TRACE("[platform_keypad_message]: p(%d) r(%d) c(%d)", 
                    pKeypadMessage->bPressed, 
                    pKeypadMessage->data.matrix.r, 
                    pKeypadMessage->data.matrix.c);
    */
    
    pMsg->id = RTOS_MSG_KEYPAD;
    pMsg->data.keypadMsgData.bPressed = pKeypadMessage->bPressed;
    pMsg->data.keypadMsgData.data.matrix.row = pKeypadMessage->data.matrix.r;
    pMsg->data.keypadMsgData.data.matrix.col = pKeypadMessage->data.matrix.c;

    platform_rtos_send(pMsg);
}

/*+\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
void platform_alarm_message(T_AMOPENAT_ALARM_MESSAGE *pAlarmMessage)
{
    PlatformMessage *pMsg = IVTBL(malloc)(sizeof(PlatformMessage));

    IVTBL(print)("platform_alarm_message");

    pMsg->id = RTOS_MSG_ALARM;
    
    platform_rtos_send(pMsg);
}

void platform_tp_message(T_AMOPENAT_TOUCHSCREEN_MESSAGE *pTpMessage)
{
    PlatformMessage *pMsg = IVTBL(malloc)(sizeof(PlatformMessage));

    pMsg->id = RTOS_MSG_TP;
    pMsg->data.tpData.penState = pTpMessage->penState;
    pMsg->data.tpData.x        = pTpMessage->x;
    pMsg->data.tpData.y        = pTpMessage->y;
    
    platform_rtos_send(pMsg);
}

void platform_tp_sleep(BOOL sleep)
{
#ifdef AM_TP_SUPPORT
    IVTBL(touchScreenSleep)(sleep);
#endif
}

int platform_rtos_setalarm(void *pParam)
{
    PlatformSetAlarmParam *platformalarm = (PlatformSetAlarmParam *)pParam;
    T_AMOPENAT_ALARM_PARAM pAlarmSet;
    
    pAlarmSet.alarmIndex = 0;
    pAlarmSet.alarmRecurrent = 1;
    pAlarmSet.alarmOn = platformalarm->alarmon;
    pAlarmSet.alarmTime.nYear = platformalarm->year;
    pAlarmSet.alarmTime.nMonth = platformalarm->month;
    pAlarmSet.alarmTime.nDay = platformalarm->day;
    pAlarmSet.alarmTime.nHour = platformalarm->hour;
    pAlarmSet.alarmTime.nMin = platformalarm->min;
    pAlarmSet.alarmTime.nSec = platformalarm->sec;

    IVTBL(print)("platform_rtos_setalarm pAlarmSet.alarmOn %d %d", pAlarmSet.alarmOn,platformalarm->min);

    return IVTBL(set_alarm)(&pAlarmSet);    
}
/*-\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/

int platform_rtos_init_module(int module, void *pParam)
{
    int ret = PLATFORM_OK;
    
    switch(module)
    {
    case RTOS_MODULE_ID_KEYPAD:
        {
            T_AMOPENAT_KEYPAD_CONFIG keypadConfig;
            PlatformKeypadInitParam *pKeypadParam = (PlatformKeypadInitParam *)pParam;

            keypadConfig.type = OPENAT_KEYPAD_TYPE_MATRIX;

            keypadConfig.pKeypadMessageCallback = platform_keypad_message;
            
            keypadConfig.config.matrix.keyInMask = pKeypadParam->matrix.inMask;
            keypadConfig.config.matrix.keyOutMask = pKeypadParam->matrix.outMask;

            IVTBL(init_keypad)(&keypadConfig);
        }
        break;

/*+\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
    case RTOS_MODULE_ID_ALARM:
        {
            T_AMOPENAT_ALARM_CONFIG  alarmConfig;

            alarmConfig.pAlarmMessageCallback = platform_alarm_message;
            IVTBL(init_alarm)(&alarmConfig);
        }
        break;
/*-\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/

    case RTOS_MODULE_ID_TP:
        {
            IVTBL(init_touchScreen)(platform_tp_message);
            break;
        }
    
    default:
        DEBUG_RTOS_TRACE("[platform_rtos_init_module]: unknown module(%d)", module);
        ret = PLATFORM_ERR;
        break;
    }
    
    return ret;
}

static void rtos_wait_message_timeout(T_AMOPENAT_TIMER_PARAMETER *pParameter)
{
    PlatformMessage *pMsg = IVTBL(malloc)(sizeof(PlatformMessage));

    pMsg->id = RTOS_MSG_WAIT_MSG_TIMEOUT;
    
    platform_rtos_send(pMsg);
}

int platform_rtos_init(void)
{
    hRTOSWaitMsgTimer = IVTBL(create_timer)(rtos_wait_message_timeout, NULL);
    
    return PLATFORM_OK;
}

/*+\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
extern int cust_get_poweron_reason(void);
extern void cust_poweron_system(void);

int platform_get_poweron_reason(void)
{
    /*+\NEW\zhuth\2014.7.25\�޸Ŀ���ԭ��ֵʵ��*/
    return (int)IVTBL(get_poweronCasue)();
    /*-\NEW\zhuth\2014.7.25\�޸Ŀ���ԭ��ֵʵ��*/
}

static int poweron_flag = -1;/*-1: δ���� 0: ������ 1:����*/
/*+\NEW\zhuth\2014.2.14\��翪�������û�û������Э��ջ�������ʹ��shutdown�ػ����������ʹ��poweroff_system�ػ�*/
static BOOL cust_sys_flag = FALSE; 
/*-\NEW\zhuth\2014.2.14\��翪�������û�û������Э��ջ�������ʹ��shutdown�ػ����������ʹ��poweroff_system�ػ�*/

int platform_rtos_poweron(int flag)
{
    poweron_flag = flag;

    if(1 == poweron_flag)
    {
        cust_poweron_system();
        /*+\NEW\zhuth\2014.2.14\��翪�������û�û������Э��ջ�������ʹ��shutdown�ػ����������ʹ��poweroff_system�ػ�*/
        cust_sys_flag = TRUE;
        /*-\NEW\zhuth\2014.2.14\��翪�������û�û������Э��ջ�������ʹ��shutdown�ػ����������ʹ��poweroff_system�ػ�*/
    }
    
    return PLATFORM_OK;
}

void platform_poweron_try(void)
{
    // Ϊ���ݾɽű�,��δ���ÿ�����־ʱ�Զ�����ϵͳ
    if(-1 == poweron_flag)
    {
        cust_poweron_system();    
        /*+\NEW\zhuth\2014.2.14\��翪�������û�û������Э��ջ�������ʹ��shutdown�ػ����������ʹ��poweroff_system�ػ�*/
        cust_sys_flag = TRUE;
        /*-\NEW\zhuth\2014.2.14\��翪�������û�û������Э��ջ�������ʹ��shutdown�ػ����������ʹ��poweroff_system�ػ�*/
    }
}
/*-\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */



int platform_get_env_usage(void)
{
    return IVTBL(get_env_usage)();
}

int platform_rtos_poweroff(void)
{
    /*+\NEW\zhuth\2014.2.14\��翪�������û�û������Э��ջ�������ʹ��shutdown�ػ����������ʹ��poweroff_system�ػ�*/
    if((platform_get_poweron_reason() == OPENAT_PM_POWERON_BY_CHARGER)
        && !cust_sys_flag)
    {
        IVTBL(shut_down)();
    }
    else
    {
        IVTBL(poweroff_system)();
    }
    /*-\NEW\zhuth\2014.2.14\��翪�������û�û������Э��ջ�������ʹ��shutdown�ػ����������ʹ��poweroff_system�ػ�*/
    
    return PLATFORM_OK;
}

/*+\NEW\liweiqiang\2013.9.7\����rtos.restart�ӿ�*/
int platform_rtos_restart(void)
{
    IVTBL(restart)();
    return PLATFORM_OK;
}
/*-\NEW\liweiqiang\2013.9.7\����rtos.restart�ӿ�*/

/*+\NEW\liweiqiang\2013.4.5\����rtos.tick�ӿ�*/
int platform_rtos_tick(void)
{
    return (int)IVTBL(get_system_tick)();
}
/*-\NEW\liweiqiang\2013.4.5\����rtos.tick�ӿ�*/

/*-\NEW\zhuwangbin\2017.2.12\��Ӱ汾��ѯ�ӿ� */
extern char * cust_luaInfo_version(void);
char *platform_rtos_get_version(void)
{
	return cust_luaInfo_version();
}

//9501GPS �ṩ32Kʱ��
void platform_rtos_sys32k_clk_out(bool enable)
{
    IVTBL(sys32k_clk_out)(enable);
	IVTBL(print)("platform_rtos_sys32k_clk_out enable %d", enable);
}

void platform_rtos_send_event(u32 event)
{
    IVTBL(send_event)(event);
}

/*-\NEW\zhuwangbin\2017.2.12\��Ӱ汾��ѯ�ӿ� */

/*begin\NEW\zhutianhua\2017.9.5 31:2\����get_fs_free_size�ӿ�*/
int platform_fs_get_free_size(void)
{
     T_AMOPENAT_FILE_INFO info = {0};
    if(IVTBL(get_fs_info)(E_AMOPENAT_FS_INTERNAL,&info)==0)
    {
        UINT64 freeSize = info.totalSize-info.usedSize;
        //DEBUG_RTOS_TRACE("platform_fs_get_free_size %l %l %l\n", info.totalSize, info.usedSize, freeSize);
        return freeSize;
    }
    else
    {
        return 0;
    }
}
/*end\NEW\zhutianhua\2017.9.5 31:2\����get_fs_free_size�ӿ�*/

/*begin\NEW\zhutianhua\2017.9.27 19:37\����make_dir�ӿ�*/
int platform_make_dir(char *pDir, int len)
{
    char* sysDir[] = {"/", "/lua", "/ldata", "/luazip", "/RecDir"};
    u8 i = 0;
    int length = len;

    if(len == 0)
    {
        IVTBL(print)("platform_make_dir dir invalid");
        return 0;
    }

    if(len>1 && *(pDir+len-1)=='/')
    {
        length = len-1;
        *(pDir+len-1)=0;
    }

    for(i=0; i<sizeof(sysDir)/sizeof(u8*); i++)
    {
        if(memcmp(pDir, sysDir[i], strlen(sysDir[i]))==0 && strlen(sysDir[i])==length)
        {
            IVTBL(print)("platform_make_dir no authority to make %s", sysDir[i]);
            return 0;
        }
    }

    if(IVTBL(make_dir)(pDir, 0) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int platform_remove_dir(char *pDir, int len)
{
    char* sysDir[] = {"/", "/lua", "/ldata", "/luazip", "/RecDir"};
    u8 i = 0;
    int length = len;

    if(len == 0)
    {
        IVTBL(print)("platform_make_dir dir invalid");
        return 0;
    }

    if(len>1 && *(pDir+len-1)=='/')
    {
        length = len-1;
        *(pDir+len-1)=0;
    }

    for(i=0; i<sizeof(sysDir)/sizeof(u8*); i++)
    {
        if(memcmp(pDir, sysDir[i], strlen(sysDir[i]))==0 && strlen(sysDir[i])==length)
        {
            IVTBL(print)("platform_make_dir no authority to make %s", sysDir[i]);
            return 0;
        }
    }
    
    return (IVTBL(remove_dir)(pDir) == 0);
}

u32 platform_enter_sc()
{
    return IVTBL(enter_critical_section)();
}

void platform_exit_sc(u32 sc)
{
    IVTBL(exit_critical_section)(sc);
}

/*end\NEW\zhutianhua\2017.9.27 19:37\����make_dir�ӿ�*/

/*+\NEW\liweiqiang\2017.12.6\����rtos.set_time�ӿ�*/
void platform_rtos_set_time(u32 timestamp){
    T_AMOPENAT_SYSTEM_DATETIME datetime;
    struct tm *stm = localtime(&timestamp);

    datetime.nYear = stm->tm_year + 1900;
    datetime.nMonth = stm->tm_mon + 1;
    datetime.nDay = stm->tm_mday;
    datetime.nHour = stm->tm_hour;
    datetime.nMin = stm->tm_min;
    datetime.nSec = stm->tm_sec;
        
    IVTBL(set_system_datetime)(&datetime);
}
/*-\NEW\liweiqiang\2017.12.6\����rtos.set_time�ӿ�*/

