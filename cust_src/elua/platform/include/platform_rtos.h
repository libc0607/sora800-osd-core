/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    platform_rtos.h
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/3/7
 *
 * Description:
 *          luaƽ̨��rtos��ӿ�
 **************************************************************************/

#ifndef _PLATFORM_RTOS_H_
#define _PLATFORM_RTOS_H_

/*+\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
typedef enum PlatformPoweronReasonTag
{
    PLATFORM_POWERON_KEY,
    PLATFORM_POWERON_CHARGER,
    PLATFORM_POWERON_ALARM,
    PLATFORM_POWERON_RESTART,
    PLATFORM_POWERON_OTHER,
    PLATFORM_POWERON_UNKNOWN,
    /*+\NewReq NEW\zhuth\2014.6.18\���ӿ���ԭ��ֵ�ӿ�*/
    PLATFORM_POWERON_EXCEPTION,
    PLATFORM_POWERON_HOST,
    PLATFORM_POWERON_WATCHDOG,
    /*-\NewReq NEW\zhuth\2014.6.18\���ӿ���ԭ��ֵ�ӿ�*/

}PlatformPoweronReason;    
/*-\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */

// ��ʼ����ģ��ID
typedef enum PlatformRtosModuleTag
{
    RTOS_MODULE_ID_KEYPAD,
    /*+\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
    RTOS_MODULE_ID_ALARM,
    /*-\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
    // touch screen...
    RTOS_MODULE_ID_TP,
    NumOfRTOSModules
}PlatformRtosModule;

typedef struct KeypadMatrixDataTag
{
    unsigned char       row;
    unsigned char       col;
}KeypadMatrixData;

typedef struct KeypadMsgDataTag
{
    u8   type;  // keypad type
    BOOL bPressed; /* �Ƿ��ǰ�����Ϣ */
    union {
        struct {
            u8 row;
            u8 col;
        }matrix, gpio;
        u16 adc;
    }data;
}KeypadMsgData;

/*+\NEW\liweiqiang\2013.4.5\����lua gpio �ж�����*/
typedef struct PlatformIntDataTag
{
    elua_int_id             id;
    elua_int_resnum         resnum;
}PlatformIntData;
/*-\NEW\liweiqiang\2013.4.5\����lua gpio �ж�����*/

/*+\NEW\liweiqiang\2013.7.8\����rtos.pmd��Ϣ*/
#define PLATFORM_BATT_NOT_CHARGING      0
#define PLATFORM_BATT_CHARING           1
#define PLATFORM_BATT_CHARGE_STOP       2

typedef struct PlatformPmdDataTag
{
    BOOL    battStatus;
    BOOL    chargerStatus;
    u8      chargeState;
    u8      battLevel;
    u16     battVolt;
}PlatformPmdData;
/*-\NEW\liweiqiang\2013.7.8\����rtos.pmd��Ϣ*/

/*+\NEW\liweiqiang\2013.11.4\����audio.core�ӿڿ� */
typedef struct PlatformAudioDataTag
{
    BOOL    playEndInd;
    BOOL    playErrorInd;
}PlatformAudioData;
/*-\NEW\liweiqiang\2013.11.4\����audio.core�ӿڿ� */
typedef struct PlatformTpDataTag
{
    u8 penState;  //��ǰ����״̬
    u16 x;
    u16 y;
}PlatformTpData;

#define PLATFORM_RTOS_WAIT_MSG_INFINITE         (0)

typedef enum PlatformMsgIdTag
{
    RTOS_MSG_WAIT_MSG_TIMEOUT, // receive message timeout
    RTOS_MSG_TIMER,
    RTOS_MSG_UART_RX_DATA,
    RTOS_MSG_UART_TX_DONE,
    RTOS_MSG_KEYPAD,
/*+\NEW\liweiqiang\2013.4.5\����lua gpio �ж�����*/
    RTOS_MSG_INT,             
/*-\NEW\liweiqiang\2013.4.5\����lua gpio �ж�����*/
/*+\NEW\liweiqiang\2013.7.8\����rtos.pmd��Ϣ*/
    RTOS_MSG_PMD,
/*-\NEW\liweiqiang\2013.7.8\����rtos.pmd��Ϣ*/
/*+\NEW\liweiqiang\2013.11.4\����audio.core�ӿڿ� */
    RTOS_MSG_AUDIO,
/*-\NEW\liweiqiang\2013.11.4\����audio.core�ӿڿ� */
/*+\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
    RTOS_MSG_ALARM,
/*-\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/

    RTOS_MSG_TP,

    NumOfMsgIds
}PlatformMsgId;

typedef union PlatformMsgDataTag
{
    int                 timer_id;
    int                 uart_id;
    KeypadMsgData       keypadMsgData;
/*+\NEW\liweiqiang\2013.4.5\����lua gpio �ж�����*/
    PlatformIntData     interruptData;
/*-\NEW\liweiqiang\2013.4.5\����lua gpio �ж�����*/
/*+\NEW\liweiqiang\2013.7.8\����rtos.pmd��Ϣ*/
    PlatformPmdData     pmdData;
/*-\NEW\liweiqiang\2013.7.8\����rtos.pmd��Ϣ*/
/*+\NEW\liweiqiang\2013.11.4\����audio.core�ӿڿ� */
    PlatformAudioData   audioData;
/*-\NEW\liweiqiang\2013.11.4\����audio.core�ӿڿ� */

    PlatformTpData      tpData;
}PlatformMsgData;

typedef struct PlatformMessageTag
{
    PlatformMsgId       id;
    PlatformMsgData     data;
}PlatformMessage;

typedef struct PlatformKeypadInitParamTag
{
    int type;
    struct{
        int inMask;         /* active key in mask */
        int outMask;        /* active key out mask */        
    }matrix;
}PlatformKeypadInitParam;

/*+\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/
typedef struct PlatformSetAlarmParamTag
{
    BOOL alarmon;
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 min;
    u8 sec;
}PlatformSetAlarmParam;

int platform_rtos_setalarm(void *pParam);
/*-\NEW\shenyuanyuan\2017.10.10\������ӽӿ�*/

int platform_rtos_init(void);

int platform_rtos_poweroff(void);

/*+\NEW\liweiqiang\2013.9.7\����rtos.restart�ӿ�*/
int platform_rtos_restart(void);
/*-\NEW\liweiqiang\2013.9.7\����rtos.restart�ӿ�*/

int platform_rtos_init_module(int module, void *pParam);

int platform_rtos_receive(void **ppMessage, u32 timeout);

int platform_rtos_send(PlatformMessage *pMsg);

int platform_rtos_send_high_priority(PlatformMessage *pMsg);

int platform_rtos_start_timer(int timer_id, int milliSecond, BOOL high);

int platform_rtos_stop_timer(int timer_id);

/*+\NEW\liweiqiang\2013.4.5\����rtos.tick�ӿ�*/
int platform_rtos_tick(void);
/*-\NEW\liweiqiang\2013.4.5\����rtos.tick�ӿ�*/

/*-\NEW\zhuwangbin\2017.2.12\��Ӱ汾��ѯ�ӿ� */
char *platform_rtos_get_version(void);
/*-\NEW\zhuwangbin\2017.2.12\��Ӱ汾��ѯ�ӿ� */

/*+\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
int platform_get_poweron_reason(void);

int platform_rtos_poweron(int flag);

void platform_poweron_try(void);
/*-\NEW\liweiqiang\2013.12.12\���ӳ�翪��ʱ���û����о����Ƿ�����ϵͳ */
/*begin\NEW\zhutianhua\2017.9.6 22:35\����get_fs_free_size�ӿ�*/
int platform_fs_get_free_size(void);
/*end\NEW\zhutianhua\2017.9.6 22:35\����get_fs_free_size�ӿ�*/

void platform_tp_sleep(BOOL sleep);

/*+\NEW\liweiqiang\2017.12.6\����rtos.set_time�ӿ�*/
void platform_rtos_set_time(u32 timestamp);
/*-\NEW\liweiqiang\2017.12.6\����rtos.set_time�ӿ�*/

#endif/*_PLATFORM_RTOS_H_*/
