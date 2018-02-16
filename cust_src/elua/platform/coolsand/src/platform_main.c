
#include "string.h"
#include "stdio.h"

#include "rda_pal.h"
#include "assert.h"

#include "platform.h"
#include "platform_malloc.h"
#include "platform_rtos.h"
#include "platform_pmd.h"

#define CUST_MAIN_TASK_PRIO     OPENAT_CUST_TASKS_PRIORITY_BASE
#define LUA_SHELL_TASK_PRIO     (OPENAT_CUST_TASKS_PRIORITY_BASE+10)

#define CUST_MAIN_TASK_STACK_SIZE       (16*1024)
#define LUA_SHELL_TASK_STACK_SIZE       (32*1024)/*��ջ�Ӵ���16K���������ű���ջ���*/

typedef enum CustMessageIdTag
{
    OPENAT_VAT_MSG_ID,
    
    NumOfCustMsgIds
}CustMessageId;

typedef struct {
    UINT8 type;
    UINT16 len;
    PVOID data;
}CustMessage;

typedef struct{
    UINT32 magic_num; //= 0x11113456
    char   app_ver[32];
    char   base_ver[32];
    UINT32 script_addr;
    UINT32 script_size;
    UINT32 base_addr;
    UINT32 base_size;
    char   reserved[32];
}LUA_INFO;


/*+\NEW\rufei\2013.9.13\����lua�ļ����ܱ��ƻ����³�����������*/
extern void LuaDeleteMainFile(void);
/*-\NEW\rufei\2013.9.13\����lua�ļ����ܱ��ƻ����³�����������*/

extern int LuaAppTask(void);
extern void RILAPI_ReceiveData(void *data, int len);
extern void platform_setup_vat_queue(void);

static VOID cust_task_main(PVOID pParameter);
static VOID lua_shell_main(PVOID pParameter);

static VOID cust_poweron_reason_init(VOID);
static void cust_pm_message(T_AMOPENAT_PM_MSG* pmMessage);
static VOID cust_at_message(UINT8 *pData, UINT16 length);

/*******************************************************
** ATTENTION: For our cust, this part must used. If you 
** have your own main entry file(instead of cust_main.c),
** pls copy this part!
**
** START .. START .. START .. START .. START .. START ..
********************************************************/
/* CUSTOM APP CODE INFOMATION */
UINT8 cust_init(T_AMOPENAT_INTERFACE_VTBL *pVtable);
VOID cust_main(VOID);
extern char _am_openat_ro_lma;
extern char _am_openat_ro_vma;
extern char _am_openat_ro_size;
extern char _am_openat_rw_lma;
extern char _am_openat_rw_vma;
extern char _am_openat_rw_size;
extern char _am_openat_zi_base;
extern char _am_openat_zi_size;
static const volatile T_AMOPENAT_CUST_VTBL __attribute__((section (".am_openat_custvtbl")))
    g_s_AMOpenAtCustVtbl =
{
    0x87654321,
    (UINT32)&_am_openat_ro_lma,
    (UINT32)&_am_openat_ro_vma,
    (UINT32)&_am_openat_ro_size,
    (UINT32)&_am_openat_rw_lma,
    (UINT32)&_am_openat_rw_vma,
    (UINT32)&_am_openat_rw_size,
    (UINT32)&_am_openat_zi_base,
    (UINT32)&_am_openat_zi_size,

    cust_init,  /* cannot changed to other name */
    cust_main,  /* cannot changed to other name */
};


static const LUA_INFO __attribute__((section (".lua_info")))
    g_s_luaInfo = 
{
    0x11113456,
    {EX_VER},
    {PLT_LOD_VERSION},
    LUA_SCRIPT_BASE,
    LUA_SCRIPT_SIZE,
    AM_OPENAT_ROM_BASE,
    AM_OPENAT_ROM_SIZE,
    {0}
};

/*******************************************************
** ATTENTION:
** END .. END .. END .. END .. END .. END .. END ..
********************************************************/

/* Function table from OpenAT platform */
T_AMOPENAT_INTERFACE_VTBL * g_s_InterfaceVtbl = 0;
HANDLE g_CustTaskHandle;
HANDLE g_LuaShellTaskHandle;

struct
{
    HANDLE poweronSyncSemaphore;
    E_AMOPENAT_POWERON_REASON reason;
}g_PowronInfo;

/*-\NEW\zhuwangbin\2017.2.12\��Ӱ汾��ѯ�ӿ� */
char * cust_luaInfo_version(void)
{
	return g_s_luaInfo.app_ver;
}
/*-\NEW\zhuwangbin\2017.2.12\��Ӱ汾��ѯ�ӿ� */
/* The first function called by OpenAT platform */
/* Return value see more E_AMOPENAT_CUST_INIT_RESUL */
UINT8 cust_init(T_AMOPENAT_INTERFACE_VTBL *pVtable)
{
    g_s_InterfaceVtbl = pVtable;
    g_PowronInfo.reason = OPENAT_PM_POWERON_BY_INVALID;
    g_PowronInfo.poweronSyncSemaphore = 0;
    
    return (UINT8)OPENAT_CUST_INIT_RES_OK;
}

/* never removed even if not used */
const T_AMOPENAT_CUST_VTBL* cust_getcustvtbl(VOID)
{
    return (const T_AMOPENAT_CUST_VTBL*)&g_s_AMOpenAtCustVtbl;
}

/* Main function call by OpenAT platform */
VOID cust_main(VOID)
{
    /* ��Կ�Ѿ�׼���� ��Ҫ����֪ͨ */
    IVTBL(set_enc_data_ok)();

    cust_poweron_reason_init();

    /*+\NEW\liweiqiang\2013.9.22\����vat�������ź�������*/
    platform_setup_vat_queue();
    /*-\NEW\liweiqiang\2013.9.22\����vat�������ź�������*/
    
    IVTBL(init_at)(cust_at_message);
    
    /* FOR power on reason and charging messages */
    T_AMOPENAT_PMD_CFG pmdcfg;
    E_AMOPENAT_PM_CHR_MODE pmdmode;

    memset(&pmdcfg, 0, sizeof(T_AMOPENAT_PMD_CFG));
#if 1  /*ģ�����ó�緽��*/
    pmdmode = OPENAT_PM_CHR_BY_DEFAULT;
    /*��Ӳ������й�*/
    pmdcfg.deFault.batdetectEnable = TRUE;
    
    pmdcfg.deFault.tempdetectEnable = FALSE;
    pmdcfg.deFault.templowLevel = 0;
    pmdcfg.deFault.temphighLevel = 0;

/*+\NEW\2013.8.5\A9321�����������,������ѹ4.25V ������150mA ʵ��217mA */
    pmdcfg.deFault.batLevelEnable = TRUE;
    pmdcfg.deFault.batfullLevel = 4200;
    pmdcfg.deFault.batPreChargLevel = 4050;
    pmdcfg.deFault.poweronLevel = 3450;
    pmdcfg.deFault.poweroffLevel = 3400;
    pmdcfg.deFault.batAdc = OPENAT_ADC_7;/*adc_sense  or adc_vbat:OPENAT_ADC_0*/
    pmdcfg.deFault.tempAdc = OPENAT_ADC_1;
    
    /*level:  poweron-----levelFirst-----levelSecond-----levelFull*/
    /*current:----currentFirst----currentSecond---currentThird----*/
    pmdcfg.deFault.currentControlEnable = TRUE; 
    pmdcfg.deFault.currentFirst = OPENAT_PM_CHARGER_500MA;
    pmdcfg.deFault.intervalTimeFirst = 9*60; /*9����*/
    pmdcfg.deFault.batLevelFirst = 4150;
    pmdcfg.deFault.currentSecond = OPENAT_PM_CHARGER_300MA;
    pmdcfg.deFault.intervalTimeSecond = 6*60;/*6����*/
    pmdcfg.deFault.batLevelSecond = 4190;
    pmdcfg.deFault.currentThird = OPENAT_PM_CHARGER_100MA;
    pmdcfg.deFault.intervalTimeThird = 3*60; /*3����*/  
/*-\NEW\2013.8.5\A9321�����������,������ѹ4.25V ������150mA ʵ��217mA */

    pmdcfg.deFault.chargTimeOutEnable = FALSE;
    pmdcfg.deFault.TimeOutMinutes = 240;
#endif
#if 0
    pmdmode = OPENAT_PM_CHR_BY_IC;
    /*��Ӳ������й�*/
    pmdcfg.ic.batdetectEnable = TRUE;
    pmdcfg.ic.tempdetectEnable = FALSE;
    pmdcfg.ic.templowLevel = 0;
    pmdcfg.ic.temphighLevel = 0;
    
    pmdcfg.ic.chrswitchport = OPENAT_GPIO_8;
    pmdcfg.ic.batstatusport = OPENAT_GPIO_1;
    
    pmdcfg.ic.batLevelEnable = FALSE;
    pmdcfg.ic.batfullLevel = 4200;
    pmdcfg.ic.poweronLevel = 3450;
    pmdcfg.ic.poweroffLevel = 3400;
    pmdcfg.ic.batAdc = OPENAT_ADC_7;/*adc_sense  or adc_vbat:OPENAT_ADC_0*/
    pmdcfg.ic.tempAdc = OPENAT_ADC_1;/*adc_battemp*/

    pmdcfg.ic.chargTimeOutEnable = FALSE;
    pmdcfg.ic.TimeOutMinutes = 240;
#endif
    ASSERT(IVTBL(init_pmd)(pmdmode, &pmdcfg, cust_pm_message));

    
    IVTBL(print)("====================================");
    IVTBL(print)("INTR VER :%s", g_s_luaInfo.app_ver);
    IVTBL(print)("BASE VER :%s", g_s_luaInfo.base_ver);
    IVTBL(print)("SCRIPT ADDR :0x%08x", g_s_luaInfo.script_addr);
    IVTBL(print)("SCRIPT SIZE :0x%08x", g_s_luaInfo.script_size);
    IVTBL(print)("BASE   ADDR :0x%08x", g_s_luaInfo.base_addr);
    IVTBL(print)("BASE   SIZE :0x%08x", g_s_luaInfo.base_size);
    IVTBL(print)("====================================");
    
    
    /* ����custom app�߳� */
    g_CustTaskHandle = IVTBL(create_task)((PTASK_MAIN)cust_task_main, 
                                            NULL, 
                                            NULL, 
                                            CUST_MAIN_TASK_STACK_SIZE, 
                                            CUST_MAIN_TASK_PRIO, 
                                            OPENAT_OS_CREATE_DEFAULT, 
                                            0, 
                                            "cust task");

    if(OPENAT_INVALID_HANDLE == g_CustTaskHandle)
    {
        ASSERT(0);
    }
    
    g_LuaShellTaskHandle = IVTBL(create_task)((PTASK_MAIN)lua_shell_main, 
                                                NULL, 
                                                NULL, 
                                                LUA_SHELL_TASK_STACK_SIZE, 
                                                LUA_SHELL_TASK_PRIO, 
                                                OPENAT_OS_CREATE_DEFAULT, 
                                                0, 
                                                "lua shell task");

    if(OPENAT_INVALID_HANDLE == g_LuaShellTaskHandle)
    {
        ASSERT(0);
    }
}

static void cust_pm_message(T_AMOPENAT_PM_MSG* pmMessage)
{
/*+\NEW\liweiqiang\2013.7.8\����rtos.pmd��Ϣ*/
    static PlatformPmdData pmdData = 
    {
        .battStatus = TRUE,
        .chargerStatus = FALSE,
        .chargeState = PLATFORM_BATT_NOT_CHARGING,
        .battLevel = 0xff,
        .battVolt = 0,
    };
    T_AMOPENAT_BAT_STATUS battStatus;
    T_AMOPENAT_CHARGER_STATUS chargerStatus;

    //PUB_TRACE("[cust_pm_message]: event %d",pmMessage->evtId);
    
    switch(pmMessage->evtId)
    {
        case OPENAT_DRV_EVT_PM_POWERON_ON_IND:
            if(OPENAT_PM_POWERON_BY_INVALID == g_PowronInfo.reason)
            {
                IVTBL(get_batteryStatus)(&battStatus);
                IVTBL(get_chargerStatus)(&chargerStatus);

                pmdData.battVolt = battStatus.batteryVolt;
                pmdData.chargerStatus = chargerStatus.chargerStatus == OPENAT_PM_CHARGER_PLUG_IN;
                
                g_PowronInfo.reason = pmMessage->param.poweronind.powerOnReason;
                
                if(0 != g_PowronInfo.poweronSyncSemaphore)
                {
                    IVTBL(release_semaphore)(g_PowronInfo.poweronSyncSemaphore);
                }
            }
            else
            {
                /* received poweron reason message again */
            }
            break;
        case OPENAT_DRV_EVT_CHR_PRESENT_IND:
            pmdData.chargerStatus = pmMessage->param.chrpresentind.present;
            if(pmdData.chargerStatus == FALSE)
                pmdData.chargeState = PLATFORM_BATT_NOT_CHARGING; // ������γ�,��Ϊ���ڳ��״̬
            break;
        case OPENAT_DRV_EVT_BAT_PRESENT_IND:
            pmdData.battStatus = pmMessage->param.batpresentind.present;
            break;
        case OPENAT_DRV_EVT_BAT_LEVEL_IND:
            if(pmdData.battLevel == pmMessage->param.batlevelind.batteryLevel)
            {
                //�������û�б仯������Ϣ��ʾ
                return;
            }
            pmdData.battLevel = pmMessage->param.batlevelind.batteryLevel;
            IVTBL(get_batteryStatus)(&battStatus);
            pmdData.battVolt = battStatus.batteryVolt;
            break;
        case OPENAT_DRV_EVT_BAT_CHARGING:
        /*+\NEW\liweiqiang\2013.7.19\�������ʱ������������µ�����*/
            if(PLATFORM_BATT_CHARING == pmdData.chargeState &&
                pmdData.battLevel == pmMessage->param.chargingind.batteryLevel)
            {
                // ���ʱһֱ���ϱ�����Ϣ ��״̬δ�ı��ҵ������δ�仯����Ƶ����ʾ
                return;
            }
            pmdData.chargeState = PLATFORM_BATT_CHARING;
            pmdData.battLevel = pmMessage->param.chargingind.batteryLevel;
            IVTBL(get_batteryStatus)(&battStatus);
            pmdData.battVolt = battStatus.batteryVolt;
        /*-\NEW\liweiqiang\2013.7.19\�������ʱ������������µ�����*/
            break;
        case OPENAT_DRV_EVT_BAT_CHR_FULL:
        /*+\NEW\liweiqiang\2013.7.19\�������ʱ������������µ�����*/
            pmdData.chargeState = PLATFORM_BATT_CHARGE_STOP;
            pmdData.battLevel = pmMessage->param.chrfullind.batteryLevel;
            IVTBL(get_batteryStatus)(&battStatus);
            pmdData.battVolt = battStatus.batteryVolt;
        /*-\NEW\liweiqiang\2013.7.19\�������ʱ������������µ�����*/
            break;
        case OPENAT_DRV_EVT_BAT_CHR_STOP:
            if(pmMessage->param.chrstopind.chrStopReason == OPENAT_PM_CHR_STOP_NO_CHARGER)
                pmdData.chargeState = PLATFORM_BATT_NOT_CHARGING;
            else
                pmdData.chargeState = PLATFORM_BATT_CHARGE_STOP;
            break;
        case OPENAT_DRV_EVT_BAT_CHR_ERR:
            pmdData.chargeState = PLATFORM_BATT_CHARGE_STOP;
            break;
        default:
            break;
    }
    
    //if(pmdData.battLevel != 0xff) //ȥ��levelδ��ʼ�����ж�,����ֱ�Ӳ���������δ�ϱ������������һֱ�޷��ϱ�pmd��Ϣ
    {
        PlatformMessage *pMsg = platform_malloc(sizeof(PlatformMessage));
        pMsg->id = RTOS_MSG_PMD;
        pMsg->data.pmdData = pmdData;
        platform_rtos_send(pMsg);
    }
/*-\NEW\liweiqiang\2013.7.8\����rtos.pmd��Ϣ*/
}

/* AT message from OpenAT platform */
static VOID cust_at_message(UINT8 *pData, UINT16 length)
{
    CustMessage *pMessage;
    
    pMessage = IVTBL(malloc)(sizeof(CustMessage));
    
    if(NULL != pMessage)
    {
        pMessage->type = OPENAT_VAT_MSG_ID;
        //copy data
        pMessage->data = IVTBL(malloc)(length);
        if(NULL != pMessage->data)
        {
            memcpy(pMessage->data, pData, length);
            pMessage->len = length;
        }
        IVTBL(send_message)(g_CustTaskHandle, pMessage);
    }
}

static VOID cust_poweron_reason_init(VOID)
{
    if(0 == g_PowronInfo.poweronSyncSemaphore)
    {
        g_PowronInfo.poweronSyncSemaphore = IVTBL(create_semaphore)(0);
        ASSERT(0 != g_PowronInfo.poweronSyncSemaphore);
    }
}

static VOID cust_wait_for_poweron_reason(BOOL bDeleteSema)
{
    ASSERT(0 != g_PowronInfo.poweronSyncSemaphore);

    IVTBL(wait_semaphore)(g_PowronInfo.poweronSyncSemaphore, 0);

    if(TRUE == bDeleteSema)
    {
        IVTBL(delete_semaphore)(g_PowronInfo.poweronSyncSemaphore);
        g_PowronInfo.poweronSyncSemaphore = 0;
    }
}

/*+\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
int cust_get_poweron_reason(void)
{
    return (int)g_PowronInfo.reason;
}

void cust_poweron_system(void)
{
    static BOOL hasPoweron = FALSE;

    if(!hasPoweron)
    {
        hasPoweron = TRUE;
        IVTBL(poweron_system)(OPENAT_PM_STARTUP_MODE_DEFAULT, OPENAT_PM_STARTUP_MODE_DEFAULT);
    }
}
/*-\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */

/* custom app main */
static VOID cust_task_main(PVOID pParameter)
{
    CustMessage *pMessage = NULL;

    /* wait for power on message from pm interface */
    cust_wait_for_poweron_reason(TRUE);

/*+\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
    if(g_PowronInfo.reason != OPENAT_PM_POWERON_BY_CHARGER)
    {
        cust_poweron_system();
    }
/*-\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
    
    PUB_TRACE("[cust_task_main]: Enter message loop");
    
    while(1)
    {
        IVTBL(wait_message)(g_CustTaskHandle, (VOID**)&pMessage, 0);
        
        if(NULL != pMessage)
        {
            //dispatch your messge
            switch(pMessage->type)
            {
                case OPENAT_VAT_MSG_ID:
                {
                    RILAPI_ReceiveData(pMessage->data, pMessage->len);
                    break;
                }
                
                default:
                    break;
            }
            
            if(NULL != pMessage->data)
            {
                IVTBL(free)(pMessage->data);
            }
            IVTBL(free)(pMessage);
        }
    }
}

/*+\NEW\liweiqiang\2013.10.25\lua�ű�ͳһ����luaĿ¼��,Ԥ�õķ�lua�ļ�ͳһ����ldata�ļ��� */
#define LUA_DIR "/lua"
#define LUA_DATA_DIR "/ldata"

char *getLuaPath(void)
{
    return LUA_DIR "/?.lua;" LUA_DIR "/?.luae";
}

char *getLuaDir(void)
{
    return LUA_DIR;
}

char *getLuaDataDir(void)
{
    return LUA_DATA_DIR;
}
/*-\NEW\liweiqiang\2013.10.25\lua�ű�ͳһ����luaĿ¼��,Ԥ�õķ�lua�ļ�ͳһ����ldata�ļ��� */

void removeLuaDir(void)
{
    /*+\NEW\rufei\2013.9.13\����lua�ļ����ܱ��ƻ����³�����������*/
    LuaDeleteMainFile();
/*-\NEW\rufei\2013.9.13\����lua�ļ����ܱ��ƻ����³�����������*/
/*+\NEW\liweiqiang\2013.10.25\lua�ű�ͳһ����luaĿ¼��,Ԥ�õķ�lua�ļ�ͳһ����ldata�ļ��� */
    IVTBL(remove_dir)(LUA_DIR); //�쳣�˳�ɾ�����нű�
/*-\NEW\liweiqiang\2013.10.25\lua�ű�ͳһ����luaĿ¼��,Ԥ�õķ�lua�ļ�ͳһ����ldata�ļ��� */
/*+\NEW\liweiqiang\2013.11.28\luadb��ʽԶ������֧�� */
    IVTBL(remove_dir)(LUA_DATA_DIR); //�쳣�˳�ɾ����������
    IVTBL(remove_dir)("/luazip"); //�쳣�˳�ɾ��������
/*-\NEW\liweiqiang\2013.11.28\luadb��ʽԶ������֧�� */
}

/*lua app main*/
static HANDLE luaShellSem = 0;

static VOID lua_shell_main(PVOID pParameter)
{
/*+\NEW\liweiqiang\2013.4.25\����lua�˳�assert���� */
    int luaExitStatus;
    
    if(0 == luaShellSem)
    {
        luaShellSem = IVTBL(create_semaphore)(0);
        ASSERT(0 != luaShellSem);
    }

/*+\NEW\liweiqiang\2013.5.11\�����Խ�ѹluazipĿ¼���ļ�֧��,ѹ���㷨lzma*/
    IVTBL(make_dir)("/luazip", 0);
/*-\NEW\liweiqiang\2013.5.11\�����Խ�ѹluazipĿ¼���ļ�֧��,ѹ���㷨lzma*/
/*+\NEW\liweiqiang\2013.10.25\lua�ű�ͳһ����luaĿ¼��,Ԥ�õķ�lua�ļ�ͳһ����ldata�ļ��� */
    IVTBL(make_dir)(LUA_DIR, 0);
    IVTBL(make_dir)(LUA_DATA_DIR, 0);
/*-\NEW\liweiqiang\2013.10.25\lua�ű�ͳһ����luaĿ¼��,Ԥ�õķ�lua�ļ�ͳһ����ldata�ļ��� */
/*+\NEW\liweiqiang\2013.10.24\����¼��Ŀ¼,�Բ���¼���ļ� */
    IVTBL(make_dir)("/RecDir", 0);
/*-\NEW\liweiqiang\2013.10.24\����¼��Ŀ¼,�Բ���¼���ļ� */
    
    luaExitStatus = LuaAppTask();
    
    removeLuaDir();
    
    PUB_TRACE("[lua_shell_main]: lua exit status %d", luaExitStatus);

    IVTBL(wait_semaphore)(luaShellSem, 3000); // 3�������ϵͳ*/

/*+\NEW\liweiqiang\2013.9.20\lua�쳣�˳�������ϵͳ,�����ļ�δд�뵽flash��*/
    IVTBL(restart)();
/*-\NEW\liweiqiang\2013.9.20\lua�쳣�˳�������ϵͳ,�����ļ�δд�뵽flash��*/
/*-\NEW\liweiqiang\2013.4.25\����lua�˳�assert���� */
}

/*+\NEW\liweiqiang\2013.9.8\����pmd.init���ó������ӿ� */
BOOL cust_pmd_init(PlatformPmdCfg *cfg)
{
/*+\NEW\liweiqiang\2014.2.8\���Ƶ�Դ�������ýӿ� */
#define GET_FILED_VAL(fIELD, dEFault) (cfg->fIELD == PMD_CFG_INVALID_VALUE ? (dEFault) : (cfg->fIELD))
    T_AMOPENAT_PMD_CFG pmdcfg;
    E_AMOPENAT_PM_CHR_MODE pmdmode;

    pmdmode = OPENAT_PM_CHR_BY_DEFAULT;
    /*��Ӳ������й�*/
    /*+\NEW\zhuth\2014.11.6\��Դ�������ò���������Ƿ����ص�����*/
    pmdcfg.deFault.batdetectEnable = (GET_FILED_VAL(batdetectEnable, 1) == 1);
    /*-\NEW\zhuth\2014.11.6\��Դ�������ò���������Ƿ����ص�����*/
    
    pmdcfg.deFault.tempdetectEnable = FALSE;
    pmdcfg.deFault.templowLevel = 0;
    pmdcfg.deFault.temphighLevel = 0;

    pmdcfg.deFault.batLevelEnable = TRUE;
    pmdcfg.deFault.batfullLevel = GET_FILED_VAL(battFullLevel, 4200);
    pmdcfg.deFault.batPreChargLevel = GET_FILED_VAL(battRechargeLevel, 4050);
    pmdcfg.deFault.poweronLevel = GET_FILED_VAL(poweronLevel, 3450);
    pmdcfg.deFault.poweroffLevel = GET_FILED_VAL(poweroffLevel, 3400);
    pmdcfg.deFault.batAdc = OPENAT_ADC_7;/*adc_sense  or adc_vbat:OPENAT_ADC_0*/
    pmdcfg.deFault.tempAdc = OPENAT_ADC_1;
    
    /*level:  poweron-----levelFirst-----levelSecond-----levelFull*/
    /*current:----currentFirst----currentSecond---currentThird----*/
    pmdcfg.deFault.currentControlEnable = TRUE; 
    pmdcfg.deFault.currentFirst = GET_FILED_VAL(currentFirst, OPENAT_PM_CHARGER_500MA);
    pmdcfg.deFault.intervalTimeFirst = GET_FILED_VAL(intervaltimeFirst, 9*60);
    pmdcfg.deFault.batLevelFirst = GET_FILED_VAL(battlevelFirst, 4150);
    pmdcfg.deFault.currentSecond = GET_FILED_VAL(currentSecond, OPENAT_PM_CHARGER_300MA);
    pmdcfg.deFault.intervalTimeSecond = GET_FILED_VAL(intervaltimeSecond, 6*60);
    pmdcfg.deFault.batLevelSecond = GET_FILED_VAL(battlevelSecond, 4190);
    pmdcfg.deFault.currentThird = GET_FILED_VAL(currentThird, OPENAT_PM_CHARGER_100MA);
    pmdcfg.deFault.intervalTimeThird = GET_FILED_VAL(intervaltimeThird, 3*60);
    /*+\NEW\zhuth\2015.2.26\֧���������������*/
    pmdcfg.deFault.plusChgControlEnable = (GET_FILED_VAL(pluschgctlEnable, 0) == 1);
    pmdcfg.deFault.plusChgOnTime = GET_FILED_VAL(pluschgonTime, 5);
    pmdcfg.deFault.plusChgOffTime = GET_FILED_VAL(pluschgoffTime, 1);
    /*-\NEW\zhuth\2015.2.26\֧���������������*/

    pmdcfg.deFault.chargTimeOutEnable = FALSE;
    pmdcfg.deFault.TimeOutMinutes = 240;
/*-\NEW\liweiqiang\2014.2.8\���Ƶ�Դ�������ýӿ� */

    return IVTBL(init_pmd)(pmdmode, &pmdcfg, cust_pm_message);
}
/*-\NEW\liweiqiang\2013.9.8\����pmd.init���ó������ӿ� */

