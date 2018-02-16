/*********************************************************
  Copyright (C), AirM2M Tech. Co., Ltd.
  Author: brezen
  Description: AMOPENAT ����ƽ̨
  Others:
  History: 
    Version�� Date:       Author:   Modification:
    V0.1      2012.09.24  brezen    �����ļ�
    V0.2      2012.12.17  brezen    ���UART����
    V0.3      2012.12.27  brezen    ���pmd����
    V0.4      2012.12.27  brezen    ���spi���� 
    V0.5      2013.01.08  brezen    ��Ӵ���������
    V0.6      2013.01.11  brezen    ���spi���� 
    V0.7      2013.01.15  brezen    �޸�lcd frequenceΪuint32
    V0.8      2013.01.15  brezen    1�����gpo 2�����lcd gpio ����
    V0.9      2013.01.17  brezen    ���ldo
    V1.0      2013.01.28  brezen    ���psam
    V1.1      2013.02.06  Jack.li   ���camera�ӿ�
    V1.2      2013.02.09  Jack.li   �����Ƶ¼�ƽӿ�
    V1.3      2013.03.01  brezen    ��Ӵ���OPENAT_UART_3
	V1.4      2014.1.17   brezen    �������HFP����
	V1.5      2014.5.22   brezen    �����������̲���ģʽ
	V1.6      2014.6.9    brezen    ��������˯��ģʽ
	V1.7      2014.6.26   brezen    �������spp�ӿ�
*********************************************************/
#ifndef AM_OPENAT_DRV_H
#define AM_OPENAT_DRV_H

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include <am_openat_common.h> 
/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/

 
typedef enum
{
  OPENAT_DRV_EVT_INVALID,
  /* charge plug in/out */
  OPENAT_DRV_EVT_CHR_PRESENT_IND,

  /* battery status */
  OPENAT_DRV_EVT_BAT_PRESENT_IND, //��ʱ��֧��
  OPENAT_DRV_EVT_BAT_LEVEL_IND,
  OPENAT_DRV_EVT_BAT_VOLT_LOW,
  OPENAT_DRV_EVT_BAT_OVERVOLT,    //��ʱ��֧��

  /* charging status */
  OPENAT_DRV_EVT_BAT_CHARGING,
  OPENAT_DRV_EVT_BAT_CHR_FULL,
/*+\BUG WM-1016\rufei\2013.11.19\ �޸�ƽ̨������*/
  OPENAT_DRV_EVT_BAT_RECHARGING,
  OPENAT_DRV_EVT_BAT_RECHR_FULL,
/*-\BUG WM-1016\rufei\2013.11.19\ �޸�ƽ̨������*/
  OPENAT_DRV_EVT_BAT_CHR_STOP,
  OPENAT_DRV_EVT_BAT_CHR_ERR,

  /* power on cause */
  OPENAT_DRV_EVT_PM_POWERON_ON_IND,

  /* GPIO interrupt */
  OPENAT_DRV_EVT_GPIO_INT_IND,
  
  /* LCD event */
  OPENAT_DRV_EVT_LCD_REFRESH_REQ,
  
  /* CAMERA event */
  OPENAT_DRV_EVT_VIDEORECORD_FINISH_IND,

  /* VIDEO event */
  OPENAT_DRV_EVT_VIDEO_PLAY_FINISH_IND,
  OPENAT_DRV_EVT_VIDEO_CURRENT_TIME_IND,

  /* UART event */
  OPENAT_DRV_EVT_UART_RX_DATA_IND,
  OPENAT_DRV_EVT_UART_TX_DONE_IND,
  /* ALARM event */
  OPENAT_DRV_EVT_ALARM_IND,

  /*app �����Զ�У��*/
  OPENAT_SW_AUTO_VERIFY_IND,
  
}E_OPENAT_DRV_EVT;

/*************************************************
* GPIO
*************************************************/

typedef enum E_AMOPENAT_GPIO_PORT_TAG
{
    OPENAT_GPIO_0,
    OPENAT_GPIO_1,
    OPENAT_GPIO_2,
    OPENAT_GPIO_3,
    OPENAT_GPIO_4,
    OPENAT_GPIO_5,
    OPENAT_GPIO_6,
    OPENAT_GPIO_7,
    OPENAT_GPIO_8,
    OPENAT_GPIO_9,
    OPENAT_GPIO_10,
    OPENAT_GPIO_11,
    OPENAT_GPIO_12,
    OPENAT_GPIO_13,
    OPENAT_GPIO_14,
    OPENAT_GPIO_15,
    OPENAT_GPIO_16,
    OPENAT_GPIO_17,
    OPENAT_GPIO_18,
    OPENAT_GPIO_19,
    OPENAT_GPIO_20,
    OPENAT_GPIO_21,
    OPENAT_GPIO_22,
    OPENAT_GPIO_23,
    OPENAT_GPIO_24,
    OPENAT_GPIO_25,
    OPENAT_GPIO_26,
    OPENAT_GPIO_27,
    OPENAT_GPIO_28,
    OPENAT_GPIO_29,
    OPENAT_GPIO_30,
    OPENAT_GPIO_31,
    OPENAT_GPIO_32,
    OPENAT_GPIO_33,
    OPENAT_GPIO_34,
    OPENAT_GPIO_35,
    OPENAT_GPIO_36,
    OPENAT_GPIO_QTY,

    
    OPENAT_GPIO_UNKNOWN = 0xFF
}E_AMOPENAT_GPIO_PORT;


/*********************************************
* ����: GPIO�жϷ�ʽ
* ����: 
*********************************************/
typedef enum
{
    OPENAT_GPIO_NO_INT,
    OPENAT_GPIO_INT_HIGH_LEVEL,
    OPENAT_GPIO_INT_LOW_LEVEL,
    OPENAT_GPIO_INT_BOTH_EDGE,
    OPENAT_GPIO_INT_FALLING_EDGE,
    OPENAT_GPIO_INT_RAISING_EDGE,
}T_OPENAT_GPIO_INT_TYPE;


/*********************************************
* ����: PIN����ģʽ
* ����: 
*********************************************/
typedef enum
{
  OPENAT_GPIO_INVALID_MODE,
  OPENAT_GPIO_INPUT, //as gpio or gpo
  OPENAT_GPIO_OUTPUT,
  OPENAT_GPIO_INPUT_INT,
  OPENAT_GPIO_MODE_UNKNOWN
}T_OPENAT_GPIO_MODE;

/*+\BUG\AMOPENAT-13\brezen\2013.4.13\�޸�Openat������api����������ǿת*/
typedef void (*OPENAT_GPIO_EVT_HANDLE)(E_OPENAT_DRV_EVT evt, E_AMOPENAT_GPIO_PORT gpioPort,unsigned char state);
/*-\BUG\AMOPENAT-13\brezen\2013.4.13\�޸�Openat������api����������ǿת*/



/*********************************************
* ����: GPIO�жϲ���
* ����: 
*********************************************/
typedef struct
{
  unsigned int            debounce; //ms
  T_OPENAT_GPIO_INT_TYPE     intType;
  OPENAT_GPIO_EVT_HANDLE     intCb;
}T_OPENAT_GPIO_INT_CFG;

/*********************************************
* ����: GPIO����
* ����: 
*********************************************/
typedef struct
{
  unsigned char      defaultState;
  T_OPENAT_GPIO_INT_CFG   intCfg;
}T_OPENAT_GPIO_PARAM;

typedef struct T_AMOPENAT_GPIO_CFG_TAG
{
    /// Direction of the GPIO
    T_OPENAT_GPIO_MODE    mode;
    T_OPENAT_GPIO_PARAM   param;
}T_AMOPENAT_GPIO_CFG;

/*************************************************
* ADC
*************************************************/
typedef enum E_AMOPENAT_ADC_CHANNEL_TAG
{
    OPENAT_ADC_0,
    OPENAT_ADC_1,
    OPENAT_ADC_2,
    OPENAT_ADC_3,
    OPENAT_ADC_4,
    OPENAT_ADC_5,
    OPENAT_ADC_6,
    OPENAT_ADC_7,
    OPENAT_ADC_QTY
}E_AMOPENAT_ADC_CHANNEL;

/*************************************************
* PM
*************************************************/
typedef enum
{
  OPENAT_PM_CHR_BY_DEFAULT, /* ϵͳ�Զ����Ƴ�� */
  OPENAT_PM_CHR_BY_CUST,    /* �û��Լ����Ƴ�� */
/*+\NEW WM-746\rufei\2013.3.30\����оƬIC���*/
  OPENAT_PM_CHR_BY_IC,
/*-\NEW WM-746\rufei\2013.3.30\����оƬIC���*/
  OPENAT_PM_CHR_INVALID_MODE
}E_AMOPENAT_PM_CHR_MODE;

/*+\BUG WM-771\rufei\2013.4.11\���Ƴ������*/
typedef enum
{
  OPENAT_PM_LI_BAT,
  OPENAT_PM_NICD_BAT
}E_AMOPENAT_BAT_TYPE;
/*-\BUG WM-771\rufei\2013.4.11\���Ƴ������*/

/*+\BUG WM-771\rufei\2013.5.20\���Ƴ������*/
typedef enum
{
    /// Charge is forced OFF
    OPENAT_PM_CHARGE_OFF,  /* ��֧�ָù���*/
    /// Charge with a  50 mA current
    OPENAT_PM_CHARGER_50MA,
    /// Charge with a 100 mA current
    OPENAT_PM_CHARGER_100MA,
    /// Charge with a 150 mA current
    OPENAT_PM_CHARGER_150MA,
    /// Charge with a 200 mA current
    OPENAT_PM_CHARGER_200MA,
    /// Charge with a 300 mA current
    OPENAT_PM_CHARGER_300MA,
    /// Charge with a 400 mA current
    OPENAT_PM_CHARGER_400MA,
    /// Charge with a 500 mA current
    OPENAT_PM_CHARGER_500MA,
    /// Charge with a 600 mA current
    OPENAT_PM_CHARGER_600MA,
    /// Charge with a 700 mA current
    OPENAT_PM_CHARGER_700MA,
    /// Charge with a 800 mA current
    OPENAT_PM_CHARGER_800MA,

    OPENAT_PM_CHARGE_CURRENT_QTY
} E_OPENAT_CHARGE_CURRENT;
/*-\BUG WM-771\rufei\2013.5.20\���Ƴ������*/


/*+\NEW WM-746\rufei\2013.3.30\����оƬIC���*/
typedef union T_AMOPENAT_PMD_CFG_TAG
{
  struct{
  UINT8 pad;   /*�����壬Ŀǰ��֧�����ֳ��*/
/*+\BUG WM-771\rufei\2013.4.11\���Ƴ������*/
  E_AMOPENAT_BAT_TYPE batType; /*������չʹ��*/
  UINT16 batfullLevel;/*������չʹ�ã�mv*/
/*+\BUG WM-771\rufei\2013.5.20\���Ƴ������*/
  UINT16 batPreChargLevel;
/*-\BUG WM-771\rufei\2013.5.20\���Ƴ������*/
  UINT16 poweronLevel;/*�����Ժ���չʹ�ã�mv*/
  UINT16 poweroffLevel;/*�����Ժ���չʹ�ã�mv*/
/*-\BUG WM-771\rufei\2013.4.11\���Ƴ������*/
  }cust;/*Ŀǰ��֧�����ֳ��*/
  struct{
/*+\BUG OPENAT-20\rufei\2013.4.19\�޸�pmd�����λ�������*/
  BOOL batdetectEnable;
/*-\BUG OPENAT-20\rufei\2013.4.19\�޸�pmd�����λ�������*/

  BOOL tempdetectEnable;
  UINT16 templowLevel;
  UINT16 temphighLevel;
/*+\BUG WM-771\rufei\2013.4.11\���Ƴ������*/

  E_AMOPENAT_BAT_TYPE batType; /*������չʹ��*/

  BOOL batLevelEnable;
  UINT16 batfullLevel;
/*+\BUG WM-771\rufei\2013.5.20\���Ƴ������*/
  UINT16 batPreChargLevel;
/*-\BUG WM-771\rufei\2013.5.20\���Ƴ������*/
  UINT16 poweronLevel;
  UINT16 poweroffLevel;
  E_AMOPENAT_ADC_CHANNEL batAdc;
  E_AMOPENAT_ADC_CHANNEL tempAdc;
  
  BOOL currentControlEnable;
/*+\BUG WM-771\rufei\2013.5.20\���Ƴ������*/
  E_OPENAT_CHARGE_CURRENT  currentFirst;
  UINT32 intervalTimeFirst; /*����Ϊ��λ*/
  UINT16 batLevelFirst; /*������չʹ��*/
  E_OPENAT_CHARGE_CURRENT  currentSecond;
  UINT16 intervalTimeSecond; /*����Ϊ��λ*/
  UINT16 batLevelSecond; /*������չʹ��*/
  E_OPENAT_CHARGE_CURRENT  currentThird;
  UINT32 intervalTimeThird;/*����Ϊ��λ*/
/*+NEW\RUFEI\2015.2.13\�޸����������*/
  BOOL plusChgControlEnable;
  UINT32 plusChgOnTime;/*����Ϊ��λ*/
  UINT32 plusChgOffTime;/*����Ϊ��λ*/
/*-NEW\RUFEI\2015.2.13\�޸����������*/
/*-\BUG WM-771\rufei\2013.5.20\���Ƴ������*/
/*-\BUG WM-771\rufei\2013.4.11\���Ƴ������*/

/*+\BUG WM-771\rufei\2013.5.20\���Ƴ������*/
  BOOL  chargTimeOutEnable;
  UINT16 TimeOutMinutes; /*��ʱ������*/
/*-\BUG WM-771\rufei\2013.5.20\���Ƴ������*/

/*+NEW WM-1049\RUFEI\2014.6.12\����ͨ���йرճ�繦��*/
  BOOL disableCharginCall;
/*-NEW WM-1049\RUFEI\2014.6.12\����ͨ���йرճ�繦��*/

  }deFault;
  struct{
/*+\BUG OPENAT-20\rufei\2013.4.19\�޸�pmd�����λ�������*/
  BOOL batdetectEnable;
/*-\BUG OPENAT-20\rufei\2013.4.19\�޸�pmd�����λ�������*/

  BOOL tempdetectEnable;
  UINT16 templowLevel;
  UINT16 temphighLevel;

  E_AMOPENAT_GPIO_PORT chrswitchport; /*����ڣ����Ƴ�翪��*/
  E_AMOPENAT_GPIO_PORT batstatusport;  /*����ڣ��жϵ���Ƿ����*/
/*+\BUG WM-771\rufei\2013.4.11\���Ƴ������*/
  E_AMOPENAT_BAT_TYPE batType; /*������չʹ��*/
  
  BOOL batLevelEnable;
  UINT16 batfullLevel;
/*+\BUG WM-771\rufei\2013.5.20\���Ƴ������*/
  UINT16 batPreChargLevel;
/*-\BUG WM-771\rufei\2013.5.20\���Ƴ������*/
  UINT16 poweronLevel;
  UINT16 poweroffLevel;
  E_AMOPENAT_ADC_CHANNEL batAdc;
  E_AMOPENAT_ADC_CHANNEL tempAdc;
/*-\BUG WM-771\rufei\2013.4.11\���Ƴ������*/

/*+\BUG WM-771\rufei\2013.5.20\���Ƴ������*/
  BOOL  chargTimeOutEnable;
  UINT16 TimeOutMinutes; /*��ʱ������*/
/*-\BUG WM-771\rufei\2013.5.20\���Ƴ������*/

/*+NEW WM-1049\RUFEI\2014.6.12\����ͨ���йرճ�繦��*/
  BOOL disableCharginCall;
/*-NEW WM-1049\RUFEI\2014.6.12\����ͨ���йرճ�繦��*/
  }ic;
}T_AMOPENAT_PMD_CFG;
/*-\NEW WM-746\rufei\2013.3.30\����оƬIC���*/

typedef enum
{
  OPENAT_PM_BATT_NORMAL,
  OPENAT_PM_BATT_CHARGING,
  OPENAT_PM_BATT_ERR,
  OPENAT_PM_BATT_INVALID
}E_AMOPENAT_BAT_STATUS;

typedef enum
{
  OPENAT_PM_CHARGER_STATUS_ERR,
  OPENAT_PM_CHARGER_PLUG_IN,
  OPENAT_PM_CHARGER_PLUG_OUT,
  OPENAT_PM_CHARGER_STATUS_INVALID
}E_AMOPENAT_CHARGER_STATUS;

typedef enum
{
  OPENAT_PM_CHARGER_WALL,
  OPENAT_PM_CHARGER_USB,
  OPENAT_PM_CHARGER_INVALID
}E_AMOPENAT_CHARGER_TYPE;

typedef enum
{
  OPENAT_LDO_POWER_USB,
  OPENAT_LDO_POWER_LCD,
/*+:\NEW\AMOPENAT-80\brezen\2013.10.9\LDO֧��V_LCD */   
  OPENAT_LDO_POWER_VLCD,
/*-:\NEW\AMOPENAT-80\brezen\2013.10.9\LDO֧��V_LCD */   
  OPENAT_LDO_POWER_KEYPDA,
/*+\NEW AMOPENAT-27\liweiqiang\2013.5.8\����KP_LEDR,G,B���ƽӿ�*/
  OPENAT_LDO_POWER_KP_LEDR,
  OPENAT_LDO_POWER_KP_LEDG,
  OPENAT_LDO_POWER_KP_LEDB,
/*+\NEW AMOPENAT-27\liweiqiang\2013.5.8\����KP_LEDR,G,B���ƽӿ�*/
/*+\NEW\liweiqiang\2013.6.1\����LDO_VIB���ƽӿ�*/
  OPENAT_LDO_POWER_VIB, //LDO_VIBRATOR
/*-\NEW\liweiqiang\2013.6.1\����LDO_VIB���ƽӿ�*/

/*+\NEW\AMOPENAT-89\brezen\2013.11.7\����MMC,VSW���ƽӿ�*/
/*level-voltage(V)*/
/*0-2.66 1-2.75 2-2.85 3-2.96 4-3.0 5-3.17 6-3.28 7-3.3*/
  OPENAT_LDO_POWER_MMC,
/*level-voltage(V)*/  
/*0-1.89 1-1.93 2-1.97 3-2.0 4-2.59 5-2.68 6-2.78 7-2.9*/  
  OPENAT_LDO_POWER_ASW,
/*-\NEW\AMOPENAT-89\brezen\2013.11.7\����MMC,VSW���ƽӿ�*/
/*+\NEW\AMOPENAT-89\brezen\2014.5.5\�ֱ����RF��CAM��ѹ��*/  
  OPENAT_LDO_POWER_CAM,
/*+\NEW\AMOPENAT-89\brezen\2014.5.5\�ֱ����RF��CAM��ѹ��*/  
  

  OPENAT_LDO_POWER_INVALID
}E_AMOPENAT_PM_LDO;

typedef struct
{
  uint16              batteryTemp;        /* ����¶�ADCֵ*/
  uint16              batteryVolt;
/*+\BUG WM-771\rufei\2013.5.22\���Ƴ������3*/
  E_AMOPENAT_ADC_CHANNEL tempChannel;  
  E_AMOPENAT_ADC_CHANNEL batteryChannel;
/*-\BUG WM-771\rufei\2013.5.22\���Ƴ������3*/
  E_AMOPENAT_BAT_STATUS  batteryState;
}T_AMOPENAT_BAT_STATUS;

typedef struct
{
/*+\BUG WM-771\rufei\2013.5.22\���Ƴ������3*/
  uint16                    chargerVolt;
  E_AMOPENAT_ADC_CHANNEL    chargerChannel;
/*-\BUG WM-771\rufei\2013.5.22\���Ƴ������3*/
  E_AMOPENAT_CHARGER_TYPE   chargerType;   /* TODO ��ʱ��֧�� */
  E_AMOPENAT_CHARGER_STATUS chargerStatus;
}T_AMOPENAT_CHARGER_STATUS;

typedef enum
{
  OPENAT_PM_CHR_STOP_NO_REASON,
  OPENAT_PM_CHR_STOP_BAT_FULL,
  OPENAT_PM_CHR_STOP_BAT_ERR,
  OPENAT_PM_CHR_STOP_TIMEOUT,
/*+\BUG WM-771\rufei\2013.8.2\���Ƴ������*/
  OPENAT_PM_CHR_STOP_TEMP,
  OPENAT_PM_CHR_STOP_CHARGER_ERR,
/*-\BUG WM-771\rufei\2013.8.2\���Ƴ������*/
  OPENAT_PM_CHR_STOP_NO_CHARGER,
  OPENAT_PM_CHR_STOP_OTER_REASON,
/*+\BUG WM-1016\rufei\2013.11.19\ �޸�ƽ̨������*/
  OPENAT_PM_RECHR_STOP_BAT_FULL,
/*-\BUG WM-1016\rufei\2013.11.19\ �޸�ƽ̨������*/
  OPENAT_PM_CHR_STOP_INVALID_REASON
}E_AMOPENAT_CHR_STOP_REASON;

typedef enum
{
  OPENAT_PM_POWERON_BY_KEY,     /* �������� */
  OPENAT_PM_POWERON_BY_CHARGER, /* ��翪�� */
  OPENAT_PM_POWERON_BY_ALARM,   /* ���ӿ��� */
  OPENAT_PM_POWERON_BY_RESET,   /* ����������� */
  OPENAT_PM_POWERON_BY_OTHER,   /* ����ԭ���µĿ��� */
  OPENAT_PM_POWERON_BY_INVALID,
/*+NEW OPEANT-104\RUFEI\2014.6.17\ ���ӻ�ȡ����ԭ��ֵ�ӿ�*/
  OPENAT_PM_POWERON_BY_EXCEPTION,
  OPENAT_PM_POWERON_BY_HOST,
  OPENAT_PM_POWERON_BY_WATCHDOG,
/*-NEW OPEANT-104\RUFEI\2014.6.17\ ���ӻ�ȡ����ԭ��ֵ�ӿ�*/
}E_AMOPENAT_POWERON_REASON;

typedef enum
{
  OPENAT_PM_STARTUP_MODE_DEFAULT,              /* ��ϵͳ���� */
  OPENAT_PM_STARTUP_MODE_ON,                   /* ǿ�ƿ��� */
  OPENAT_PM_STARTUP_MODE_OFF                   /* ǿ�Ʋ����� */
}E_AMOPENAT_STARTUP_MODE;

/*+\NEW\RUFEI\2014.2.13\����OPENAT��ѯ�����HW״̬�ӿ�*/
typedef enum
{
    OPENAT_PM_CHR_HW_STATUS_UNKNOWN,
    OPENAT_PM_CHR_HW_STATUS_AC_OFF,
    OPENAT_PM_CHR_HW_STATUS_AC_ON
}E_AMOPENAT_CHR_HW_STATUS;
/*-\NEW\RUFEI\2014.2.13\����OPENAT��ѯ�����HW״̬�ӿ�*/

typedef struct 
{
  E_OPENAT_DRV_EVT evtId;
  union
  {
/*+\BUG WM-771\rufei\2013.4.11\���Ƴ������*/
    struct{
    BOOL present;
    }batpresentind,chrpresentind;
    struct{
    UINT8 pad; /*�գ�������*/
    }chrstartind,chargererrind;
/*+\BUG WM-1016\rufei\2013.11.19\ �޸�ƽ̨������*/
    struct{
    UINT8 batteryLevel;  /*0-100 %*/
    }batlevelind,batovervoltind,chargingind,chrfullind,rechargingind,rechrfullind;
/*-\BUG WM-1016\rufei\2013.11.19\ �޸�ƽ̨������*/
    struct{
    E_AMOPENAT_CHR_STOP_REASON chrStopReason;
    }chrstopind;
    struct{
    E_AMOPENAT_POWERON_REASON powerOnReason;
    }poweronind;
    struct
    {
     BOOL start;  //true :start false:end
     UINT8 mask;  //bit0:sw bit1:key bit2:pw1 bit3:pw2 
    }swAutoVerifyInd;
/*-\BUG WM-771\rufei\2013.4.11\���Ƴ������*/
  }param;
  
}T_AMOPENAT_PM_MSG;

typedef void (*PPM_MESSAGE)(T_AMOPENAT_PM_MSG* pmMessage);

/*************************************************
* KEYPAD
*************************************************/
typedef enum E_AMOPENAT_KEYPAD_TYPE_TAG
{
    OPENAT_KEYPAD_TYPE_MATRIX,      /* ���м��� */
    OPENAT_KEYPAD_TYPE_ADC,         /* ADC���� */
    OPENAT_KEYPAD_TYPE_GPIO,        /* GPIO���� */
    OPENAT_KEYPAD_TYPE_MAX
}E_AMOPENAT_KEYPAD_TYPE;

/*+\NEW WM-718\rufei\2013.3.21\ ����gpio���̼���ģʽ*/
typedef enum E_AMOPENAT_GPIOKEY_MODE_TAG
{
    OPENAT_GPIOKEY_IRQ, /*��ͨģʽ*/
    OPENAT_GPIOKEY_ENCRYPT, /*����ģʽ*/
        
    OPENAT_GPIOKEY_MAX
}E_AMOPENAT_GPIOKEY_MODE;
/*-\NEW WM-718\rufei\2013.3.21\ ����gpio���̼���ģʽ*/

typedef struct T_AMOPENAT_KEYPAD_MESSAGE_TAG
{
    UINT8 nType; /**/
    BOOL bPressed; /* �Ƿ��ǰ�����Ϣ */
    union {
        struct {
            UINT8 r;
            UINT8 c;
        }matrix, gpio;

/*+\NEW OPENAT-771\rufei\2013.8.8\�ϱ�ADC���̲�������*/
        struct{
            UINT16  adcValue;
            UINT16* adcList; /*��T_AMOPENAT_KEYPAD_CONFIG �д�isreportData������Ҫfree  adcList*/
            UINT16  adcCount;
        }adc;
/*-\NEW OPENAT-771\rufei\2013.8.8\�ϱ�ADC���̲�������*/
    }data;
}T_AMOPENAT_KEYPAD_MESSAGE;

typedef VOID (*PKEYPAD_MESSAGE)(T_AMOPENAT_KEYPAD_MESSAGE *pKeypadMessage);

typedef struct T_AMOPENAT_KEYPAD_CONFIG_TAG
{
    E_AMOPENAT_KEYPAD_TYPE type;
    PKEYPAD_MESSAGE pKeypadMessageCallback;
    union {
        struct {
/*+\NEW WM-718\rufei\2013.3.21\ ����gpio���̼���ģʽ*/
            UINT8 keyInMask;
            UINT8 keyOutMask;
/*-\NEW WM-718\rufei\2013.3.21\ ����gpio���̼���ģʽ*/
        }matrix;
/*+\NEW WM-718\rufei\2013.3.21\ ����gpio���̼���ģʽ*/
        struct {
            UINT32 gpioInMask;
            UINT32 gpioOutMask;
            BOOL   gpiofirstcfg;
            E_AMOPENAT_GPIOKEY_MODE mode;
        }gpio;
/*-\NEW WM-718\rufei\2013.3.21\ ����gpio���̼���ģʽ*/

/*+\NEW OPENAT-771\rufei\2013.8.8\�ϱ�ADC���̲�������*/
        struct{
            BOOL isreportData; /*�ڿ��������ڰ������µĴ�����free  adcList*/
        }adc;
/*-\NEW OPENAT-771\rufei\2013.8.8\�ϱ�ADC���̲�������*/
    }config;
}T_AMOPENAT_KEYPAD_CONFIG;

/*************************************************
* TOUCHSCREEN
*************************************************/
typedef enum E_AMOPENAT_TOUCHSCREEN_PEN_STATE_TAG
{
    OPENAT_TOUCHSCREEN_PEN_DOWN,    
    OPENAT_TOUCHSCREEN_PEN_RESSED,  
    OPENAT_TOUCHSCREEN_PEN_UP,

    NumOfOpenatTouchScreenStates
}E_AMOPENAT_TOUCHSCREEN_PEN_STATE;

typedef struct T_AMOPENAT_TOUCHSCREEN_MESSAGE_TAG
{
    UINT8 penState;  //��ǰ����״̬
    UINT16 x;
    UINT16 y;
}T_AMOPENAT_TOUCHSCREEN_MESSAGE;

typedef VOID (*PTOUCHSCREEN_MESSAGE)(T_AMOPENAT_TOUCHSCREEN_MESSAGE *pTouchScreenMessage);


/*************************************************
* UART
*************************************************/
typedef enum
{
  OPENAT_UART_1,
  OPENAT_UART_2,
  OPENAT_UART_3, /*HOST UART�ڣ�ʹ�øÿ������¹涨*/
                /*******************************************************************
                /// 1�����ݰ���ʽ
                //  |-----------------------------------------------------------------
                //  |Start(1Byte) | Size(2Byte) | ID(1Byte) | Data(nByte) | CRC(1Byte)|
                //  |-----------------------------------------------------------------
                // Start     ֻ��Ϊ 0xAD
                // Size Msb  (Size = n(Data) + 1(CRC))
                // Size Lsb
                // ID        ID����Ϊ0xff����������
                // Data 1
                //   .
                //   .
                //   .
                // Data n
                // CRC       ID��Data��ÿ��byte�����


                //Sample     AD0002940094
                             AD000C8A28000100B104000041540D0E
                               
                ///  2�������ʱ���Ϊ 921600��databitΪ7��8����֧����żУ��
                
                //   3����������OPENAT_UART_FLOWCONTROL_HW��֧��XON/XOFF����
                        ͨ�ŶԶ���Ҫ֧��������XON(0x11)/XOFF(0x13)�����øò���
                        ���������Ӳ��FIFO�Զ����������ַ�������Ҫ�������

                        Ŀǰ���������ص�����£������ַ�(0x11,0x13,0x5c)�ᶪʧ�������ݰ����ܲ�����
                        
                     4�����������������ַ�(0x11,0x13,0x5C)��ת�崦��
                        Ϊ�����ݰ������ݺ������ַ�(XON/XOFF)��������ͻ�����ݰ��а�����
                        �����ַ���Ҫͨ��0x5c����ת��
                          > ת��ķ����ǣ�0X5C + Ҫ���͵�������λȡ����
                          > ���Ҫ����0x11�� ����Ҫ����0x5c 0xEE (0xEE = ~0x11)
                          > ���Ҫ����0x13�� ����Ҫ����0x5c 0xEC (0xEC = ~0x13)
                          > ���Ҫ����0x5c�� ����Ҫ����0x5c 0xA3  (0xA3 = ~0x5C)
                        ע��һ: 
                          ת��������ݰ���Ӱ��ԭ�����ݰ��������ʽ����sizeλ��crcλ��ԭʼ����ͬ
                        
                          ԭʼ���ݰ�: ad 00 04 94 00 5c 01 c9    (����5c)
                                                     --      
                          ת�����ݰ�: ad 00 04 94 00 5c A3 01 c9 (ת���󳤶�04��crc���������)
                                                     -----
                        ע���:
                          ģ��Ӳ�����Զ�����ת�壬����Ҫ�������ת�崦�����ͺͽ��ն���ת��õ����ݰ���
                          ��ģ��ͨ�ŶԶ���Ҫ�����ݰ�����ת�崦��

                          modem TX --------->  ad 00 04 94 00 5c 01 c9   (modemӲ���Զ�ת�壬�����������)
                          PC    RX <---------  ad 00 04 94 00 5c A3 01 c9(�Ѿ���ͨ��modem��Ӳ��ת�������ݰ�)

                          PC    TX --------->  ad 00 04 94 00 5c A3 01 c9(pc�����ת��õ����ݰ�)
                          modm  RX <---------  ad 00 04 94 00 5c 01 c9   (modemӲ���Զ�ת�壬�����������)

                        ע����:
                          ���ݰ���ÿ�������ַ�����Ҫת�塣����size��id��data��crc�������ַ�����Ҫת��
                *******************************************************************/
  OPENAT_UART_QTY
}E_AMOPENAT_UART_PORT;

typedef enum
{
  OPENAT_UART_BAUD_1200,
  OPENAT_UART_BAUD_2400,
  OPENAT_UART_BAUD_4800,
  OPENAT_UART_BAUD_9600,
  OPENAT_UART_BAUD_14400,
  OPENAT_UART_BAUD_19200,
  OPENAT_UART_BAUD_28800,
  OPENAT_UART_BAUD_38400,
  OPENAT_UART_BAUD_57600,
  OPENAT_UART_BAUD_76800,
  OPENAT_UART_BAUD_115200,
  OPENAT_UART_BAUD_230400,
  OPENAT_UART_BAUD_460800,
  OPENAT_UART_BAUD_576000,
  OPENAT_UART_BAUD_921600,
  OPENAT_UART_BAUD_1152000,
  OPENAT_UART_BAUD_4000000,
  OPENAT_UART_NUM_OF_BAUD_RATES
}E_AMOPENAT_UART_BAUD;

typedef enum
{
  OPENAT_UART_NO_PARITY,
  OPENAT_UART_ODD_PARITY,
  OPENAT_UART_EVEN_PARITY
}E_AMOPENAT_UART_PARITY;

typedef enum
{
  OPENAT_UART_FLOWCONTROL_NONE,
  OPENAT_UART_FLOWCONTROL_HW,
  OPENAT_UART_FLOWCONTROL_SW,
  OPENAT_UART_FLOWCONTROL_INVALID
} E_AMOPENAT_UART_FLOWCTL;

/*+\NEW\zhutianhua\2018.2.5 16:51\�������uart buf�ӿ�*/
typedef enum
{
  OPENAT_UART_SEND_BUF,
  OPENAT_UART_RECV_BUF,
  OPENAT_UART_BUF_INVALID
} E_AMOPENAT_UART_BUF;
/*-\NEW\zhutianhua\2018.2.5 16:51\�������uart buf�ӿ�*/

#define AMOPENAT_UART_READ_FOREVER  (0xFFFFFFFF)
#define AMOPENAT_UART_READ_TRY      (0)

typedef struct 
{
  E_OPENAT_DRV_EVT evtId;
  union
  {
    uint32  dataLen;
  }param;
  
}T_AMOPENAT_UART_MESSAGE;

typedef void (*PUART_MESSAGE)(T_AMOPENAT_UART_MESSAGE* evt);

typedef struct
{
  E_AMOPENAT_UART_BAUD     baud; 
  uint32                   dataBits; /*6-8*/
  uint32                   stopBits; /*1-2*/
  E_AMOPENAT_UART_PARITY   parity;
  E_AMOPENAT_UART_FLOWCTL  flowControl;
  PUART_MESSAGE            uartMsgHande; /*���ڽ��ܵ����������ϱ�������ΪNULL����ʹ��������ʽ��ȡ*/
  BOOL                     txDoneReport;
}T_AMOPENAT_UART_PARAM;

/*+\NEW\liweiqiang\2013.12.25\���host uart�������ݹ��� */
/************************************************
* HOST
************************************************/
typedef void (*PHOST_MESSAGE)(UINT8 *pData, UINT32 length);
/*-\NEW\liweiqiang\2013.12.25\���host uart�������ݹ��� */

#define OPENAT_LAYER_COUNT 3


/************************************************
* LCD
************************************************/
/*+\NEW\liweiqiang\2013.3.28\���Ӵ��ڲ����ӿ� */
typedef enum 
{
    OPENAT_LCD_SPI4LINE,
    OPENAT_LCD_PARALLEL_8800,

    OPENAT_LCD_BUS_QTY
}E_AMOPENAT_LCD_BUS;
/*-\NEW\liweiqiang\2013.3.28\���Ӵ��ڲ����ӿ� */

typedef struct
{
  uint16  ltX;
  uint16  ltY;
  uint16  rbX;
  uint16  rbY;
}T_AMOPENAT_LCD_RECT_T;

typedef struct
{
  uint16     height;
  uint16     width;
  uint16     xoffset;   /* ˢ����ƫ���� */
  uint16     yoffset;   /* һ������Ϊ0��0x20�����������Ե���ʾʱʹ�� */
  uint32     frequence; /* SPI����Ƶ�� */
  UINT8*     fameBuffer;
  E_AMOPENAT_GPIO_PORT   csPort;  /* LCDƬѡGPIO�� */
  E_AMOPENAT_GPIO_PORT   rstPort; /* LCD��λGPIO�� */
  
  /*+\BUG WM-822\WZQ\2013.5.25\����128*128��4���Ҷ���*/
  uint16     pixelBits;
  /*-\BUG WM-822\WZQ\2013.5.25\����128*128��4���Ҷ���*/
}T_AMOPENAT_MONO_LCD_PARAM;

typedef struct T_AMOPENAT_LCD_REFRESH_REQ_TAG
{
    T_AMOPENAT_LCD_RECT_T rect;
    UINT16 *pFrameBuffer;
}T_AMOPENAT_LCD_REFRESH_REQ;

typedef struct 
{
  E_OPENAT_DRV_EVT evtId;
  union
  {
    T_AMOPENAT_LCD_REFRESH_REQ      refreshReq;
  }param;
}T_AMOPENAT_LCD_MESSAGE;

typedef void (*PLCD_MESSAGE)(T_AMOPENAT_LCD_MESSAGE *pMsg);

typedef struct 
{
    uint16     height;
    uint16     width;
    PLCD_MESSAGE    msgCallback; // ���camera ��Ƶ���ܵ�ˢ�»ص������
/*+\NEW\liweiqiang\2013.3.28\���Ӵ��ڲ����ӿ� */
    E_AMOPENAT_LCD_BUS bus;
    union{
        struct{
            uint32     frequence; /* SPI����Ƶ�� */
            E_AMOPENAT_GPIO_PORT   csPort;  /* LCDƬѡGPIO�� */
            E_AMOPENAT_GPIO_PORT   rstPort; /* LCD��λGPIO�� */
        }spi;
    /*+\NEW\liweiqiang\2013.10.12\���Ӳ��ڲ���cs,rst�ܽ�����*/
        struct{
            E_AMOPENAT_GPIO_PORT   csPort;  /* LCDƬѡGPIO�� */
            E_AMOPENAT_GPIO_PORT   rstPort; /* LCD��λGPIO�� */
        }parallel;
    /*-\NEW\liweiqiang\2013.10.12\���Ӳ��ڲ���cs,rst�ܽ�����*/
    }lcdItf;
/*-\NEW\liweiqiang\2013.3.28\���Ӵ��ڲ����ӿ� */
}T_AMOPENAT_COLOR_LCD_PARAM;


typedef enum E_OPENAT_LCD_LAYER_FORMAT_TAG
{
    OPENAT_LAYER_FORMAT_RGB565,
    OPENAT_LAYER_FORMAT_RGBA,
}E_OPENAT_LCD_LAYER_FORMAT;


typedef struct E_OPENAT_LCD_LAYER_INFO_tag
{   
    E_OPENAT_LCD_LAYER_FORMAT format;
    T_AMOPENAT_LCD_RECT_T     rect;
    uint8*                    buffer;
    BOOL                      used;
    BOOL                      visible;
}E_OPENAT_LCD_LAYER_INFO;




/************************************************
* SPI
*************************************************/
typedef enum
{
  OPENAT_SPI_1,
  OPENAT_SPI_QTY
}E_AMOPENAT_SPI_PORT;

typedef struct T_AMOPENAT_SPI_MESSAGE_TAG
{
  E_OPENAT_DRV_EVT evtId;
  uint32    dataLen;
}T_AMOPENAT_SPI_MESSAGE;

typedef VOID (*PSPI_MESSAGE)(T_AMOPENAT_SPI_MESSAGE* spiMessage);

typedef struct
{
  BOOL          fullDuplex;                  /*TRUE: DI/DO FALSE: DO only*/
  BOOL          cpol;                        /*FALSE: spi_clk idle״̬Ϊ0 TRUE: spi_clk idle״̬Ϊ 1*/
  BOOL          withCS;                      /*��ʱ���ã�����ΪTRUE*/
  uint8         cpha;                        /*0~1 0:��һ��clk�������ش������ݣ�1:�ڶ���clk�����ش������� ...*/
  uint8         dataBits;                    /*4~32*/
  uint32        clock;                       /*110k~13M*/
  PSPI_MESSAGE  spiMessage;                  /*��ʱ��֧��*/
}T_AMOPENAT_SPI_PARAM;

/************************************************
* I2C
*************************************************/
typedef enum E_AMOPENAT_I2C_PORT_TAG
{
    OPENAT_I2C_1,
    OPENAT_I2C_2,
    OPENAT_I2C_3,
    OPENAT_I2C_QTY
}E_AMOPENAT_I2C_PORT;

typedef struct T_AMOPENAT_I2C_MESSAGE_TAG
{
    E_OPENAT_DRV_EVT evtId;
    uint32    dataLen;
}T_AMOPENAT_I2C_MESSAGE;

typedef VOID (*PI2C_MESSAGE)(T_AMOPENAT_I2C_MESSAGE* i2cMessage);

typedef struct
{
    uint32          freq;           /*i2c�������� 100KHz\400KHz*/
    //uint8           slaveAddr;      /*i2c���豸��ַ (7bit) ��read write��ʱ������豸��ַ*/
    uint8           regAddrBytes;    /*i2c����Ĵ�����ַ�ֽ���*/ //�ݲ�֧��������Ϊ0 = 1�ֽ�
    BOOL            noAck;          /*�Ƿ�ȷ��ack*/      //�ݲ�֧��������ΪFALSE
    BOOL            noStop;         /*�Ƿ�ȷ�Ϸ�ֹͣλ*/ //�ݲ�֧��������ΪFALSE
    PI2C_MESSAGE    i2cMessage;     /*��ʱ��֧��*/
}T_AMOPENAT_I2C_PARAM;

/************************************************
* camera
*************************************************/
#define AMOPENAT_CAMERA_DELAY_CMD 0xffff /* 0xffff��ʾ��������һ����ʱָ�� */

#define AMOPENAT_CAMERA_REG_ADDR_8BITS      (0<<0)
#define AMOPENAT_CAMERA_REG_ADDR_16BITS     (1<<0)
#define AMOPENAT_CAMERA_REG_DATA_8BITS      (0<<1)
#define AMOPENAT_CAMERA_REG_DATA_16BITS     (1<<1)

typedef struct T_AMOPENAT_CAMERA_REG_TAG
{
    UINT16      addr;
    UINT16      value;
}AMOPENAT_CAMERA_REG, *PAMOPENAT_CAMERA_REG;

typedef enum E_AMOPENAT_CAMERA_IMAGE_FORMAT_TAG
{
    CAMERA_IMAGE_FORMAT_YUV422,

    NumOfOpenatCameraImageFormats
}E_AMOPENAT_CAMERA_IMAGE_FORMAT;

typedef struct 
{
  E_OPENAT_DRV_EVT evtId;
  union
  {
    UINT16 videorecordFinishResult;
  }param;
}T_AMOPENAT_CAMERA_MESSAGE;

typedef void (*PCAMERA_MESSAGE)(T_AMOPENAT_CAMERA_MESSAGE *pMsg);

typedef struct T_AMOPENAT_CAMERA_PARAM_TAG
{
    /* ����ͷ/��Ƶ��Ϣ������ */
    PCAMERA_MESSAGE messageCallback;
    
    /* ����ͷI2C���� */
    E_AMOPENAT_I2C_PORT i2cPort;            /* ����ͷʹ�õ�i2c����id */
    UINT8       i2cSlaveAddr;               /* ����ͷi2c���ʵ�ַ */
    UINT8       i2cAddrDataBits;            /* ����ͷ�Ĵ�����ַ�Լ�����λ�� */

    /* ����ͷ��Ҫ�ź�����Ч�������� */
    BOOL        RSTBActiveLow;              /* pin RSTB ����Ч */
    BOOL        PWDNActiveLow;              /* pin PWDN ����Ч */
    BOOL        VSYNCActiveLow;             /* pin VSYNC ����Ч */

    /* ����ͷͼ���� */
    UINT16      sensorWidth;
    UINT16      sensorHeight;

    E_AMOPENAT_CAMERA_IMAGE_FORMAT imageFormat; /* ����ͷͼ���ʽ */
    
    PAMOPENAT_CAMERA_REG initRegTable_p;  /* ����ͷ��ʼ��ָ��� */
    UINT16 initRegTableCount;          /* ����ͷ��ʼ��ָ���� */

    AMOPENAT_CAMERA_REG idReg;          /* ����ͷID�Ĵ�����ֵ */
}T_AMOPENAT_CAMERA_PARAM;

// T_AMOPENAT_CAM_PREVIEW_PARAM.encodeQuality video encoding quality
#define OPENAT_VID_REC_QTY_LOW          0
#define OPENAT_VID_REC_QTY_NORMAL       1
#define OPENAT_VID_REC_QTY_HIGH         2
#define OPENAT_VID_REC_QTY_FINE         3

typedef struct T_AMOPENAT_CAM_PREVIEW_PARAM_TAG
{
    UINT16      offsetX;
    UINT16      offsetY;
    
    UINT16      startX;
    UINT16      startY;
    UINT16      endX;
    UINT16      endY;

    /* ��Ƶ¼�ƵĲ���,����Ԥ������������Щ���� */
    UINT16      recordAudio;  /* �Ƿ�¼�� */
    UINT16      filesizePermit;  //��KΪ��λ
    UINT16      timePermit;
    UINT16      encodeQuality;
}T_AMOPENAT_CAM_PREVIEW_PARAM;

typedef struct T_AMOPENAT_CAM_CAPTURE_PARAM_TAG
{
    UINT16      imageWidth;
    UINT16      imageHeight;
}T_AMOPENAT_CAM_CAPTURE_PARAM;

/************************************************
* video
*************************************************/
typedef enum E_AMOPENAT_VIDEO_TYPE_TAG
{
    OPENAT_VIDEO_TYPE_MJPG,
    OPENAT_VIDEO_TYPE_RM,
    OPENAT_VIDEO_TYPE_MP4,
    OPENAT_VIDEO_TYPE_3GP,
    OPENAT_VIDEO_TYPE_AVSTRM,

    NumOfOpenatVideoTypes
}E_AMOPENAT_VIDEO_TYPE;

typedef struct T_AMOPENAT_VIDEO_MESSAGE_TAG
{
    E_OPENAT_DRV_EVT evtId;
    union
    {
      UINT32 playFinishResult;
      UINT32 currentTime;  // ��msΪ��λ
    }param;
}T_AMOPENAT_VIDEO_MESSAGE;

typedef void (*PVIDEO_MESSAGE)(T_AMOPENAT_VIDEO_MESSAGE *pMsg); 

typedef struct T_AMOPENAT_VIDEO_INFO_TAG
{
    UINT16 imageWidth;
    UINT16 imageHeight;
    UINT32 totalTime;
}T_AMOPENAT_VIDEO_INFO;

typedef struct T_AMOPENAT_VIDEO_PARAM_TAG
{
    PVIDEO_MESSAGE                  msgCallback;    /* ��Ƶ��Ϣ�ص����� */    
    INT32                           iFd;            /* ��Ƶ�ļ���� */
    E_AMOPENAT_VIDEO_TYPE           videoType;
}T_AMOPENAT_VIDEO_PARAM;

/************************************************
* AUDIO
*************************************************/
typedef enum
{
  OPENAT_AUD_TONE_DTMF_0,
  OPENAT_AUD_TONE_DTMF_1,
  OPENAT_AUD_TONE_DTMF_2,
  OPENAT_AUD_TONE_DTMF_3,
  OPENAT_AUD_TONE_DTMF_4,
  OPENAT_AUD_TONE_DTMF_5,
  OPENAT_AUD_TONE_DTMF_6,
  OPENAT_AUD_TONE_DTMF_7,
  OPENAT_AUD_TONE_DTMF_8,
  OPENAT_AUD_TONE_DTMF_9,
  OPENAT_AUD_TONE_DTMF_A,
  OPENAT_AUD_TONE_DTMF_B,
  OPENAT_AUD_TONE_DTMF_C,
  OPENAT_AUD_TONE_DTMF_D,
  OPENAT_AUD_TONE_DTMF_HASH,
  OPENAT_AUD_TONE_DTMF_STAR,
  OPENAT_AUD_TONE_DTMF_END
}E_AMOPENAT_DTMF_TYPE;

typedef enum
{
  OPENAT_AUD_TONE_DIAL,
  OPENAT_AUD_TONE_BUSY,
  OPENAT_AUD_TONE_CONGESTION,
  OPENAT_AUD_TONE_PATH_ACKNOWLEGEMENT,
  OPENAT_AUD_TONE_CALL_DROPPED,
  OPENAT_AUD_TONE_SPECIAL_INFO,
  OPENAT_AUD_TONE_CALL_WAITING,
  OPENAT_AUD_TONE_RINGING,
  OPENAT_AUD_TONE_USER_POSITIVE_ACK,
  OPENAT_AUD_TONE_USER_NEGTIVE_ACK,
/*+\BUG WM-901\maliang\2013.8.2\openat����tone������IDLE_INPUT_REMIND*/
  OPENAT_AUD_TONE_IDLE_INPUT_REMIND,
/*-\BUG WM-901\maliang\2013.8.2\openat����tone������IDLE_INPUT_REMIND*/
  OPENAT_AUD_TONE_USER_INFO_RING_BACK,
  OPENAT_AUD_TONE_USER_INFO_DIAL,
  OPENAT_AUD_TONE_END,
}E_AMOPENAT_TONE_TYPE;


typedef enum {
    OPENAT_AUD_SPK_GAIN_MUTE = 0,     /// MUTE
    OPENAT_AUD_SPK_GAIN_0dB,    
    OPENAT_AUD_SPK_GAIN_3dB,    
    OPENAT_AUD_SPK_GAIN_6dB,   
    OPENAT_AUD_SPK_GAIN_9dB,   
    OPENAT_AUD_SPK_GAIN_12dB,
    OPENAT_AUD_SPK_GAIN_15dB,
    OPENAT_AUD_SPK_GAIN_18dB,
    OPENAT_AUD_SPK_GAIN_END,
}E_AMOPENAT_SPEAKER_GAIN;

typedef enum 
{
    OPENAT_AUD_PLAY_ERR_NO,
    OPENAT_AUD_PLAY_ERR_UNKNOWN_FORMAT,
    OPENAT_AUD_PLAY_ERR_BUSY,
    OPENAT_AUD_PLAY_ERR_INVALID_PARAMETER,
    OPENAT_AUD_PLAY_ERR_ACTION_NOT_ALLOWED,
    OPENAT_AUD_PLAY_ERR_OUT_OF_MEMORY,
    OPENAT_AUD_PLAY_ERR_CANNOT_OPEN_FILE,         		           
    OPENAT_AUD_PLAY_ERR_END_OF_FILE,	     
    OPENAT_AUD_PLAY_ERR_TERMINATED,		   
    OPENAT_AUD_PLAY_ERR_BAD_FORMAT,	      
    OPENAT_AUD_PLAY_ERR_INVALID_FORMAT,   
    OPENAT_AUD_PLAY_ERR_ERROR,	               
} E_AMOPENAT_PLAY_ERROR;

typedef enum
{
    OPENAT_AUD_PLAY_MODE_AMR475,
    OPENAT_AUD_PLAY_MODE_AMR515,
    OPENAT_AUD_PLAY_MODE_AMR59,
    OPENAT_AUD_PLAY_MODE_AMR67,
    OPENAT_AUD_PLAY_MODE_AMR74,
    OPENAT_AUD_PLAY_MODE_AMR795,
    OPENAT_AUD_PLAY_MODE_AMR102,
    OPENAT_AUD_PLAY_MODE_AMR122,
    OPENAT_AUD_PLAY_MODE_FR,
    OPENAT_AUD_PLAY_MODE_HR,
    OPENAT_AUD_PLAY_MODE_EFR,
    OPENAT_AUD_PLAY_MODE_PCM,
    OPENAT_AUD_PLAY_MODE_AMR_RING,
    OPENAT_AUD_PLAY_MODE_MP3,
    OPENAT_AUD_PLAY_MODE_AAC,
    OPENAT_AUD_PLAY_MODE_WAV,
    OPENAT_AUD_PLAY_MODE_STREAM_PCM, //for TTS stream play
    /*+\BUG WM-669\lifei\2013.06.09\[OpenAT] ֧��MIDI����*/
    OPENAT_AUD_PLAY_MODE_MIDI,
    /*-\BUG WM-669\lifei\2013.06.09\[OpenAT] ֧��MIDI����*/
    OPENAT_AUD_PLAY_MODE_QTY,
} E_AMOPENAT_PLAY_MODE;
typedef enum
{
    // ¼����ʱֻ֧��amr��ʽ
    OPENAT_REC_MODE_AMR475,
    OPENAT_REC_MODE_AMR515,
    OPENAT_REC_MODE_AMR59,
    OPENAT_REC_MODE_AMR67,
    OPENAT_REC_MODE_AMR74,
    OPENAT_REC_MODE_AMR795,
    OPENAT_REC_MODE_AMR102,
    OPENAT_REC_MODE_AMR122,
    OPENAT_REC_MODE_FR,
    OPENAT_REC_MODE_HR,
    OPENAT_REC_MODE_EFR,
    OPENAT_REC_MODE_PCM,
    OPENAT_REC_MODE_AMR_RING,
    OPENAT_REC_MODE_INVALID,

    OPENAT_REC_MODE_QTY
} OPENAT_REC_MODE_T;

typedef enum
{
    OPENAT_STATUS_MID_BUFFER_REACHED,
    OPENAT_STATUS_END_BUFFER_REACHED,
    OPENAT_STATUS_NO_MORE_DATA,
    OPENAT_STATUS_ERR,

    OPENAT_STATUS_QTY
} OPENAT_REC_STATUS_T;

typedef VOID (*AUDIO_REC_CALLBACK)(UINT8 *,UINT8);

typedef enum
{
    OPENAT_AUD_CHANNEL_HANDSET,     /*�ֱ� mic1+receiver*/
    OPENAT_AUD_CHANNEL_EARPIECE,    /*���� mic2+ear*/
    OPENAT_AUD_CHANNEL_LOUDSPEAKER, /*������mic2+spk*/

	OPENAT_AUD_CHANNEL_BLUETOOTH,   /*����mic1+ear*/
	OPENAT_AUD_CHANNEL_FM,          /*FM����NULL+ear*/
	OPENAT_AUD_CHANNEL_FM_LP,       /*FM����NULL+spk*/
	OPENAT_AUD_CHANNEL_TV,          /*TV  mic1+ear*/

/*+\BUG WM-825\rufei\2013.5.29\����������ֱ�����ͨ��*/
    OPENAT_AUD_CHANNEL_AUX_HANDSET, /*�����ֱ�mic2+receiver*/
    OPENAT_AUD_CHANNEL_AUX_LOUDSPEAKER, /*��������mic1+spk*/
/*+\BUG WM-882\rufei\2013.7.18\����ͨ������*/
    OPENAT_AUD_CHANNEL_AUX_EARPIECE,    /*��������mic1+ear*/
/*-\BUG WM-882\rufei\2013.7.18\����ͨ������*/
/*-\BUG WM-825\rufei\2013.5.29\����������ֱ�����ͨ��*/

/*+\NEW\RUFEI\2014.7.28\����dummyͨ��*/
												/*Ӳ����ͬ��������ת*/
	OPENAT_AUD_CHANNEL_DUMMY_HANDSET,         /*�ֱ� mic1+receiver*/
	OPENAT_AUD_CHANNEL_DUMMY_AUX_HANDSET,     /*�����ֱ�mic2+receiver*/
	OPENAT_AUD_CHANNEL_DUMMY_LOUDSPEAKER,      /*������mic2+spk*/
	OPENAT_AUD_CHANNEL_DUMMY_AUX_LOUDSPEAKER,  /*��������mic1+spk*/
/*-\NEW\RUFEI\2014.7.28\����dummyͨ��*/

    /*+\BUG WM-632\lifei\2013.03.04\[OpenAT] Audio�ӿ�ʵ��������ת���ķ���*/
    OPENAT_AUD_CHANNEL_UNKNOWN,
    /*-\BUG WM-632\lifei\2013.03.04\[OpenAT] Audio�ӿ�ʵ��������ת���ķ���*/
}E_AMOPENAT_AUDIO_CHANNEL;

/*-\BUG\zhuwangbin\2017.7.26\�޸�OPENAT_PlayMusic�ӿ�, ͨ��callback������������ŵĳ���*/
typedef void (*AUD_PLAY_CALLBACK_T)(E_AMOPENAT_PLAY_ERROR result, int *len);
/*-\BUG\zhuwangbin\2017.7.26\�޸�OPENAT_PlayMusic�ӿ�, ͨ��callback������������ŵĳ���*/

/*+\NewReq WM-584\maliang\2013.2.21\[OpenAt]֧��T������MP3*/
typedef struct T_AMOPENAT_PLAY_BUFFER_PARAM_TAG
{
    INT32 *pBuffer;
    UINT32 len;
    UINT8 loop;
    AUD_PLAY_CALLBACK_T callback;
    E_AMOPENAT_PLAY_MODE  format;
}T_AMOPENAT_PLAY_BUFFER_PARAM;

/*+\NewReq WM-702\maliang\2013.3.15\������Ƶ�ļ��Ľӿ�����һ��������������ʾ�ļ�����*/
typedef enum E_AMOPENAT_PLAY_FILE_FORMAT_TAG
{
    OPENAT_AUD_PLAY_FILE_FORMAT_MP3,
    OPENAT_AUD_PLAY_FILE_FORMAT_AMR,
    OPENAT_AUD_PLAY_FILE_FORMAT_MIDI
}E_AMOPENAT_PLAY_FILE_FORMAT;
/*-\NewReq WM-702\maliang\2013.3.15\������Ƶ�ļ��Ľӿ�����һ��������������ʾ�ļ�����*/

typedef struct T_AMOPENAT_PLAY_FILE_PARAM_TAG
{
/*+\BUG WM-719\maliang\2013.3.21\�ļ�ϵͳ�ӿںͲ�����Ƶ�ļ��ӿڵ��ļ�����Ϊunicode little ending����*/
    char* fileName;        /* �ļ���ʹ��unicode���� little endian��ʽ��ʾ */
/*-\BUG WM-719\maliang\2013.3.21\�ļ�ϵͳ�ӿںͲ�����Ƶ�ļ��ӿڵ��ļ�����Ϊunicode little ending����*/
/*+\NewReq WM-702\maliang\2013.3.15\������Ƶ�ļ��Ľӿ�����һ��������������ʾ�ļ�����*/
    E_AMOPENAT_PLAY_FILE_FORMAT  fileFormat;
/*-\NewReq WM-702\maliang\2013.3.15\������Ƶ�ļ��Ľӿ�����һ��������������ʾ�ļ�����*/
    AUD_PLAY_CALLBACK_T callback;
}T_AMOPENAT_PLAY_FILE_PARAM;

typedef struct  T_AMOPENAT_PLAY_PARAM_TAG
{
      BOOL  playBuffer;/*�ǲ���buffer���ǲ����ļ�*/
      union
      {
           T_AMOPENAT_PLAY_BUFFER_PARAM       playBufferParam;
           T_AMOPENAT_PLAY_FILE_PARAM        playFileParam;
      };
}T_AMOPENAT_PLAY_PARAM;
/*-\NewReq WM-584\maliang\2013.2.21\[OpenAt]֧��T������MP3*/

/*+\NewReq WM-710\maliang\2013.3.18\ [OpenAt]���ӽӿ�����MP3���ŵ���Ч*/
typedef enum  E_AMOPENAT_AUDIO_SET_EQ_TAG
{
    OPENAT_AUD_EQ_NORMAL,     
    OPENAT_AUD_EQ_BASS,    
    OPENAT_AUD_EQ_DANCE,
    OPENAT_AUD_EQ_CLASSICAL,
    OPENAT_AUD_EQ_TREBLE,
    OPENAT_AUD_EQ_PARTY,
    OPENAT_AUD_EQ_POP,
    OPENAT_AUD_EQ_ROCK
}E_AMOPENAT_AUDIO_SET_EQ;
/*-\NewReq WM-710\maliang\2013.3.18\ [OpenAt]���ӽӿ�����MP3���ŵ���Ч*/

/*+\NewReq WM-711\maliang\2013.3.18\[OpenAt]���ӽӿڴ򿪻�ر���Ƶ�ػ�����*/
typedef enum  E_AMOPENAT_AUDIO_LOOPBACK_TYPE_TAG
{
    OPENAT_AUD_LOOPBACK_HANDSET,
    OPENAT_AUD_LOOPBACK_EARPIECE,
    OPENAT_AUD_LOOPBACK_LOUDSPEAKER,
/*+\NewReq WM-862\maliang\2013.7.2\ ����at+audlb������Ƶ�ػ�*/
    OPENAT_AUD_LOOPBACK_AUX_HANDSET,
    OPENAT_AUD_LOOPBACK_AUX_LOUDSPEAKER
/*-\NewReq WM-862\maliang\2013.7.2\ ����at+audlb������Ƶ�ػ�*/
}E_AMOPENAT_AUDIO_LOOPBACK_TYPE;
/*-\NewReq WM-711\maliang\2013.3.18\[OpenAt]���ӽӿڴ򿪻�ر���Ƶ�ػ�����*/

/*************************************************
* PSAM
*************************************************/
typedef enum
{
  OPENAT_PSAM_SUCCESS,                      /*�����ɹ�*/
  OPENAT_PSAM_ERR_EXTRA_RXATA,              /*���յ���������*/
  OPENAT_PSAM_ERR_BAT_ATR,                  /*��λATR����*/
  OPENAT_PSAM_ERR_RESET_TIMEOUT,            /*��λӦ��ʱ*/
  OPENAT_PSAM_ERR_PARITY,                   /*������żУ�����*/
  OPENAT_PSAM_ERR_WWT_TIMEOUT,              /*���ݴ���ʧ��*/
  OPENAT_PSAM_ERR_RCV_TIMEOUT,              /*���ݽ��ճ�ʱ*/
  OPENAT_PSAM_ERR_INVALID_PARAM,            /*�ӿڲ�������*/
  OPENAT_PSAM_ERR_DEV_BUSY,                 /*�豸æ*/
  OPENAT_PSAM_ERR_HW_SWITCH,                /*Ӳ���Ѿ��л�����Ŀ��Ϸ���*/
  OPENAT_PSAM_ERR_OTHER                     /*��������*/
}E_AMOPENAT_PSAM_OPER_RESULT;
typedef enum
{
  OPENAT_PSAM_ID_1,                         /* ����SIM0��ΪPSAM���ӿ� */
  OPENAT_PSAM_ID_2,                         /* ����SIM2��ΪPSAM���ӿ� */
  OPENAT_PSAM_ID_INVALID
}E_AMOPENAT_PSAM_ID;
typedef enum
{
  OPENAT_PSAM_VOLT_1_8V,                    /* 1.8V */
  OPENAT_PSAM_VOLT_3V,                      /* 3V */
  OPENAT_PSAM_VOLT_5V,                      /* 5V */
  OPENAT_PSAM_VOLT_INVALID
}E_AMOPENAT_PSAM_VOLT_CLASS;

/*+\BUG WM-690\rufei\2013.3.18\AT+SPWMû��ʵ��PWM1��PWM2*/
/*************************************************
* PWM
*************************************************/
typedef enum
{
    /// 125 ms
    OPENAT_PWM_LPG_PER_125,        // 0:   8
    /// 250ms
    OPENAT_PWM_LPG_PER_250,        // 1:   16
    /// 500ms
    OPENAT_PWM_LPG_PER_500,        // 2:   32
    /// 0.75 sec
    OPENAT_PWM_LPG_PER_750,       // 3:   64
    /// 1 sec
    OPENAT_PWM_LPG_PER_1000,       // 4:   96
    /// 1.25 sec
    OPENAT_PWM_LPG_PER_1250,       // 5:   128
    /// 1.5 sec
    OPENAT_PWM_LPG_PER_1500,       // 6:   160
    /// 1.75 sec
    OPENAT_PWM_LPG_PER_1750,       // 7:   192
    OPENAT_PWM_LPG_PER_QTY
}E_OPENAT_PWM_LPG_PERIOD;

typedef enum 
{
    /// 15.6 ms
    OPENAT_PWM_LPG_ON_01 = 1,      // 1:   2 1/64 s
    /// 31.2 ms
    OPENAT_PWM_LPG_ON_02,          // 2:   3 2/64 s
    /// 46.9 ms
    OPENAT_PWM_LPG_ON_03,          // 3:   4 3/64 s
    /// 62.5 ms
    OPENAT_PWM_LPG_ON_04,          // 4:   5 4/64 s
    /// 78.1 ms
    OPENAT_PWM_LPG_ON_05,          // 5:   6 5/64 s
    /// 93.7 ms
    OPENAT_PWM_LPG_ON_06,          // 6:   7 6/64 s
    /// 0.11 s
    OPENAT_PWM_LPG_ON_07,          // 7:   8 7/64 s
    /// 0.125 s
    OPENAT_PWM_LPG_ON_08,          // 8:   9 8/64 s
    /// 0.141 s
    OPENAT_PWM_LPG_ON_09,          // 9:   10 9/64 s
    /// 0.156 s
    OPENAT_PWM_LPG_ON_10,          // 10:  11 10/64 s
    /// 0.172 s
    OPENAT_PWM_LPG_ON_11,          // 11:  12 11/64 s
    /// 0.187 s
    OPENAT_PWM_LPG_ON_12,          // 12:  13 12/64 s
    /// 0.203 s
    OPENAT_PWM_LPG_ON_13,          // 13:  14 13/64 s
    /// 0.219 s
    OPENAT_PWM_LPG_ON_14,          // 14:  15 14/64 s
    /// 0.234 s
    OPENAT_PWM_LPG_ON_15,          // 15:  16 15/64 s
    
    OPENAT_PWM_LPG_ON_QTY
}E_OPENAT_PWM_LPG_ON;



typedef enum
{
  OPENAT_PWM_PWT,//�ݲ�֧��
  OPENAT_PWM_LPG,//�ݲ�֧��
  OPENAT_PWM_PWL,
  
  OPENAT_PWM_QTY
}E_AMOPENAT_PWM_PORT;

typedef struct
{
    E_AMOPENAT_PWM_PORT port;
    union
    {
        struct pwm_pwt{
        UINT8  level;
        UINT16 freq;
        }pwt,pwl;/*ע��:ʹ��PWLʵ�ַ�����ʱ��freqʵ����Ч��Χ:0-0xff*/
        struct pwm_lpg{
        E_OPENAT_PWM_LPG_PERIOD period;
        E_OPENAT_PWM_LPG_ON     onTime;
        }lpg;
    }cfg;
}T_AMOPENAT_PWM_CFG;
/*-\BUG WM-690\rufei\2013.3.18\AT+SPWMû��ʵ��PWM1��PWM2*/


typedef struct T_AMOPENAT_ALARM_MESSAGE_TAG
{
  E_OPENAT_DRV_EVT evtId;
  union
  {
    uint16  alarmIndex;
  }param;  
}T_AMOPENAT_ALARM_MESSAGE;

typedef VOID (*PALARM_MESSAGE)(T_AMOPENAT_ALARM_MESSAGE *pAlarmMessage);

typedef struct T_AMOPENAT_ALARM_CONFIG_TAG
{
    PALARM_MESSAGE pAlarmMessageCallback;
}T_AMOPENAT_ALARM_CONFIG;


/*+\NewReq WM-725\xiongjunqun\2013.3.25\ [OpenAt]����FM�ӿ�*/
typedef enum
{
    /// 87.5-108 MHz    (US/Europe)
    OPENAT_FM_BAND_US_EUROPE = 0,
    /// 76-91 MHz       (Japan)
    OPENAT_FM_BAND_JAPAN,
    /// 76-108 MHz      (World Wide)
    OPENAT_FM_BAND_WORLD,
    /// 65-76 MHz       (East Europe)
    OPENAT_FM_BAND_EAST_EUROPE,

    OPENAT_FM_BAND_QTY
} E_AMOPENAT_FM_BAND;

typedef enum
{
    OPENAT_FM_MUTE = 0,
    OPENAT_FM_VOL_1,
    OPENAT_FM_VOL_2,
   	OPENAT_FM_VOL_3,
    OPENAT_FM_VOL_4,
    OPENAT_FM_VOL_5,
    OPENAT_FM_VOL_6,
   	OPENAT_FM_VOL_7,

    OPENAT_FM_VOL_QTY
} E_AMOPENAT_FM_VOL_LEVEL;


typedef struct
{
    /// FM Frequency(KHZ)
    UINT32              freqKHz;
    /// Volume
    E_AMOPENAT_FM_VOL_LEVEL     volume;
    /// Enable/Disable Bass Boost
    BOOL                bassBoost;
    /// Enable/Disable Mono output of stereo radios
    BOOL                forceMono;
} T_AMOPENAT_FM_INIT;

typedef struct 
{
  E_OPENAT_DRV_EVT evtId;
  struct
  {
  	UINT32 freqKHz; 
  	BOOL found;
  	BOOL stereo;
  }param;
}T_AMOPENAT_FM_MESSAGE;

typedef void (*PFM_MESSAGE)(UINT32 freq, BOOL found, BOOL stereoStatus);

typedef struct T_AMOPENAT_FM_PARAM_TAG
{

    PFM_MESSAGE messageCallback;
    E_AMOPENAT_FM_BAND fmBand;
	T_AMOPENAT_FM_INIT fmInit;
}T_AMOPENAT_FM_PARAM;
/*-\NewReq WM-725\xiongjunqun\2013.3.25\ [OpenAt]����FM�ӿ�*/


typedef VOID (*PINBANDINFO_CALLBACK)(void);

/*+\NEW\brezen\2013.12.30\��������ӿ�*/
#define OPENAT_BT_NAME_MAX_LEN (59)
#define OPENAT_BT_PB_LENGHT     (20)  
#define OPENAT_BT_MAX_SDAP_UUID_NO (15)
#define OPENAT_BT_MAX_PHONE_NUMBER 48

#define OPENAT_BT_SUPPORT_SPP_PROFILE (1<<0)
#define OPENAT_BT_SUPPORT_HFP_PROFILE (1<<1)
#define OPENAT_BT_SUPPORT_PBAP_PROFILE (1<<2)

/*app��������*/
typedef enum
{
  OPENAT_BT_MGR_DISCORY_REQ_CMD,
  OPENAT_BT_MGR_DISCORY_CANCEL_CMD,
  OPENAT_BT_MGR_SET_SCANNABLE_CMD,
  OPENAT_BT_MGR_READ_LOCAL_ADDR_CMD,
  OPENAT_BT_MGR_WRITE_LOCAL_NAME_CMD,
  OPENAT_BT_MGR_SLEEP_MODE_CMD,

  OPENAT_BT_BM_BONDING_REQ_CMD,
  OPENAT_BT_BM_DEBOND_REQ_CMD,
  OPENAT_BT_PBAP_CON_REQ_CMD,
  OPENAT_BT_PBAP_GET_NEXT_REQ_CMD,
  OPENAT_BT_PBAP_DISCON_REQ_CMD,

  OPENAT_BT_HFG_CONNECT_REQ,
  OPENAT_BT_HFG_DISCONNECT_REQ,
  OPENAT_BT_HFG_DIAL_REQ,
  OPENAT_BT_HFG_HUNGUP_REQ,
  OPENAT_BT_HFG_CALL_ANSWER_REQ,
  OPENAT_BT_HFG_SEND_DTMF_REQ,
  OPENAT_BT_HFG_PLAY_SCO_AUDIO_REQ,
  OPENAT_BT_HFG_CONNECT_SCO_AUDIO_REQ,
  OPENAT_BT_HFG_SET_MIC_GAIN_REQ,

  /*for cmu test*/
  OPENAT_BT_TEST_POWERON_REQ,
  OPENAT_BT_TEST_POWEROFF_REQ,
  OPENAT_BT_TEST_VOC_REQ,
  OPENAT_BT_TEST_DUT_REQ,
  OPENAT_BT_TEST_SINGLEHOP_REQ
  
  
}E_AMOPENAT_BT_CMD;
/*app��BT����Ӧ*/
typedef enum
{
  OPENAT_BT_MGR_LINK_ACCEPT_RSP,
  OPENAT_BT_MGR_PIN_CODE_RSP,

  OPENAT_BT_MAX_RSP
  
}E_AMOPENAT_BT_RSP;

/*BT�����ϱ�����BT��ӦAPP����*/
typedef enum
{
  OPENAT_BT_POWER_ON_IND,
  OPENAT_BT_BM_WRITE_SCANENABLE_MODE_CNF,
  OPENAT_BT_BM_READ_LOCAL_ADDR_CNF,
  OPENAT_BT_BM_WRITE_LOCAL_NAME_CNF,
  OPENAT_BT_BM_LINK_CONNECT_ACCEPT_IND,
  OPENAT_BT_BM_PIN_CODE_IND,
  OPENAT_BT_BM_BONDING_RESULT_IND,
  OPENAT_BT_BM_DEBOND_CNF,
  OPENAT_BT_BM_DISCOVERY_RESULT_IND,
  OPENAT_BT_BM_DISCOVERY_CNF,
  OPENAT_BM_BM_DISCOVERY_CANCEL_CNF,

  OPENAT_BT_PBAP_DATA_IND,
  OPENAT_BT_PBAP_DISCON_CNF,

  OPENAT_BT_HFG_CONNECT_IND,
  OPENAT_BT_HFG_CONNECT_CNF,
  OPENAT_BT_HFG_DISCONNECT_IND,
  OPENAT_BT_HFG_DISCONNECT_CNF,
  OPENAT_BT_HFG_SCO_CON_CNF,
  OPENAT_BT_HFG_SCO_DISCON_CNF,
  OPENAT_BT_PHONE_CALL_INFO_IND,
  OPENAT_BT_HFG_SCO_INFO_IND,

  OPENAT_BT_SPP_CONNECT_IND,
  OPENAT_BT_SPP_DISCONNECT_IND,
  OPENAT_BT_SPP_DISCONNECT_CNF,
  OPENAT_BT_SPP_CONNECT_CNF,
  OPENAT_BT_SPP_DATA_IND,
  OPENAT_BT_SPP_SEND_DATA_CNF,

  OPENAT_BT_POWER_OFF_IND,

  OPENAT_BT_DRV_EVENT_INVALID
}E_AMOPENAT_BT_EVENT;

typedef enum
{
  OPENAT_BTBM_ADP_P_OFF_I_OFF = 0,
  OPENAT_BTBM_ADP_P_OFF_I_ON,
  OPENAT_BTBM_ADP_P_ON_I_OFF,
  OPENAT_BTBM_ADP_P_ON_I_ON,
  OPENAT_BTBM_ADP_P_ON_I_ON_L
}E_AMOPENAT_BT_SCANNABLE;

typedef enum
{
	OPENAT_PBAP_DISCON_SUCCSEE,
	OPENAT_PBAP_DISCON_OPPER_UNSUPPORT,
	OPENAT_PBAP_DISCON_OPPER_NOT_ACCEPT
	
}E_AMOPENAT_BT_PBAP_DISCON_STATUS;


typedef struct
{
  UINT8	    name_len;
  UINT8	    name[OPENAT_BT_NAME_MAX_LEN];
}T_AMOPENAT_BT_NAME;
typedef struct
{
  UINT32   addrLap;
  UINT8    addrUap;
  UINT16   addrNap;
}T_AMOPENAT_BT_ADDR;


typedef struct
{
	UINT16 name[OPENAT_BT_PB_LENGHT];
	UINT8 name_length;

	UINT8 phone_num[OPENAT_BT_PB_LENGHT];
	UINT8 call_type;
	UINT8 data_time[17];

}T_AMOPENAT_BT_PBAP_CALL_HISTORY;

typedef struct
{
	UINT16 name[OPENAT_BT_PB_LENGHT];
	UINT8 name_length;
	UINT8 phone_num[3][OPENAT_BT_PB_LENGHT];

}T_AMOPENAT_BT_PBAP_INFO;

typedef union
{
	T_AMOPENAT_BT_PBAP_CALL_HISTORY history_info_array[10];
	T_AMOPENAT_BT_PBAP_INFO         pbap_info_array[10];

}U_AMOPENAT_BT_PBAP_ITEM_ARRAY;


/*BT_BM_WRITE_SCANENABLE_MODE_CNF*/
typedef struct
{
    UINT8	result;
}T_AMOPENAT_BT_WRITE_SCANNABLE_MODE_CNF;

/*BT_BM_READ_LOCAL_ADDR_CNF*/
typedef struct
{
    UINT8	         result;
    T_AMOPENAT_BT_ADDR  bd_addr;
}T_AMOPENAT_BT_READ_LOCAL_ADDR_CNF;

/*BT_BM_WRITE_LOCAL_NAME_CNF*/
typedef struct
{
    UINT8	result;
}T_AMOPENAT_BT_WRITE_LOCAL_NAME_CNF;

/*BT_BM_LINK_CONNECT_ACCEPT_IND*/
typedef struct
{
    T_AMOPENAT_BT_ADDR	bd_addr;
}T_AMOPENAT_BT_LINK_CONNECT_ACCEPT_IND;

/*BT_BM_PIN_CODE_IND*/
typedef struct
{
    T_AMOPENAT_BT_ADDR	bd_addr;
    UINT8	name_len;
    UINT8	name[OPENAT_BT_NAME_MAX_LEN];
}T_AMOPENAT_BT_BM_PIN_CODE_INT;

/*BT_BM_BONDING_RESULT_IND*/
typedef struct
{
    UINT8           result;
    T_AMOPENAT_BT_ADDR	bd_addr;
    UINT8	KeyType;
    UINT8  linkKey[0x10]; //support linkkey store in MMI	
    UINT32  cod;
}T_AMOPENAT_BT_BONDING_RESULT_IND;
typedef struct
{
  UINT8 result;
  T_AMOPENAT_BT_ADDR bd_addr;
}T_AMOPENAT_BT_DEBOND_CNF;
/*BT_PBAP_DATA_IND*/
typedef struct
{
	UINT8 	        con_type;//0 phone book; 1 combined call historys
	UINT32          tid;
	UINT8           status;
	UINT16          current_item_num;
	UINT16          all_item_num;
	U_AMOPENAT_BT_PBAP_ITEM_ARRAY  pbap_data_array;
	UINT8           is_spp_ind;
}T_AMOPENAT_BT_PBAP_BT_DATA_IND;

/*OPENAT_BT_PBAP_DISCON_CNF*/
typedef struct
{
	UINT8 con_type;//0 phone book; 1 combined call historys
	UINT32 tid;
	E_AMOPENAT_BT_PBAP_DISCON_STATUS status;
}T_AMOPENAT_BT_PBAP_CON_DISCON_INFO;

/*BT_BM_DISCOVERY_RESULT_IND*/
typedef struct  
{
    T_AMOPENAT_BT_ADDR bd_addr;
    UINT32	cod;
    UINT8	name_len;
    UINT8	name[OPENAT_BT_NAME_MAX_LEN];
}T_AMOPENAT_BT_BM_DISCOVERY_RESULT_IND;

/*BT_BM_DISCOVERY_CNF*/
typedef struct
{
    UINT8	result;
    UINT8	total_number;
}T_AMOPENAT_BT_BM_DISCOVERY_CNF;
/*BM_BM_DISCOVERY_CANCEL_CNF*/
typedef struct
{
    UINT8	result;
    UINT8	total_number;
}T_AMOPENAT_BT_BM_DISCOVERY_CANCEL_CNF;
/*BT_HFG_CONNECT_IND*/
typedef struct
{
    T_AMOPENAT_BT_ADDR bt_addr;
    UINT32   connection_id;    
    UINT8         profile; //handle if take earphone role
}T_AMOPENAT_BT_HFG_CONNECT_IND;
/*BT_HFG_CONNECT_CNF*/
typedef struct
{
    UINT16                 result;
    T_AMOPENAT_BT_ADDR     bt_addr;
    UINT32                 connection_id;
    UINT8                  profile; //handle if take earphone role
}T_AMOPENAT_BT_HFG_CONNECT_CNF;

typedef struct
{
    UINT16        result;
    T_AMOPENAT_BT_ADDR bt_addr;
    UINT32   connection_id;
}T_AMOPENAT_BT_HFG_DISCONNECT_CNF;

typedef struct
{
    T_AMOPENAT_BT_ADDR    bt_addr;
    UINT32   connection_id;
}T_AMOPENAT_BT_HFG_DISCONNECT_IND;

typedef struct
{
  UINT8 call_director;   // 1 is incoming call, 2 is out going call
	UINT8 call_status;    // 0 is no call,1 is setup,2 is active
	UINT8 call_number[OPENAT_BT_MAX_PHONE_NUMBER];
	UINT8 call_number_length;
}T_AMOPENAT_BT_PHONE_CALL_INFO;
typedef struct
{
	UINT16			result;
	UINT8 	    connection_id;
}T_AMOPENAT_BT_HFG_SCO_CON_CNF;
typedef struct
{
	UINT16 	       result;
	UINT8 	       connection_id;
}T_AMOPENAT_BT_HFG_SCO_DISCON_CNF;

typedef struct
{
  BOOL connect;
}T_AMOPENAT_BT_HFG_SCO_INFO_IND;

typedef struct
{
	UINT8	 port;	/* virtual port number*/
	UINT8        result;
	UINT16       maxFrameSize; /*һ�δ�������ֽ���*/
	T_AMOPENAT_BT_ADDR addr;
}T_AMOPENAT_BT_SPP_CONN_IND;

typedef struct
{
	UINT8	             port;	/* virtual port number*/
  UINT8              result;  /* This is result field of this returned cnf msg */
	T_AMOPENAT_BT_ADDR addr;

}T_AMOPENAT_BT_SPP_DISCONN_IND;

typedef struct
{
	UINT8	 port;	/* virtual port number*/
 	UINT8         result;  /* This is result field of this returned cnf msg */	
	T_AMOPENAT_BT_ADDR addr;
}T_AMOPENAT_BT_SPP_DISCONN_CNF;

typedef struct
{
	UINT8	 port;	/* virtual port number*/
	UINT8         result;  /* This is result field of this returned cnf msg */
	UINT16        maxFrameSize; /*һ�δ�������ֽ���*/
	T_AMOPENAT_BT_ADDR addr;
}T_AMOPENAT_BT_SPP_CONN_CNF;

typedef struct
{
	UINT8	       port;	 /* virtual port number*/
	UINT32       dataLen;
	T_AMOPENAT_BT_ADDR addr;
}T_AMOPENAT_BT_SPP_DATA_IND_T;

typedef struct
{
	T_AMOPENAT_BT_ADDR addr;
	UINT8	         port;	 /* virtual port number*/
	UINT8          result;  /* This is result field of this returned cnf msg */
}T_AMOPENAT_BT_SPP_SEND_DATA_CNF;
typedef union
{
  T_AMOPENAT_BT_WRITE_SCANNABLE_MODE_CNF writeScannableModeCnf;
  T_AMOPENAT_BT_READ_LOCAL_ADDR_CNF      readLocalAddrCnf;
  T_AMOPENAT_BT_WRITE_LOCAL_NAME_CNF     writeLocalNameCnf;
  T_AMOPENAT_BT_LINK_CONNECT_ACCEPT_IND  linkConnectAcceptInd;
  T_AMOPENAT_BT_BM_PIN_CODE_INT          pinCodeInd;
  T_AMOPENAT_BT_BONDING_RESULT_IND       bondingResultInd;
  T_AMOPENAT_BT_DEBOND_CNF               debondCnf;
  T_AMOPENAT_BT_PBAP_BT_DATA_IND         pbapBtDataInd;
  T_AMOPENAT_BT_PBAP_CON_DISCON_INFO     pbapDisconInfo;
  T_AMOPENAT_BT_BM_DISCOVERY_RESULT_IND  discoveryResultInd;
  T_AMOPENAT_BT_BM_DISCOVERY_CNF         discoveryCnf;
  T_AMOPENAT_BT_BM_DISCOVERY_CANCEL_CNF  discoveryCancelCnf;
  T_AMOPENAT_BT_HFG_CONNECT_IND          hfgConnectInd;
  T_AMOPENAT_BT_HFG_CONNECT_CNF          hfgConnectCnf;
  T_AMOPENAT_BT_HFG_DISCONNECT_IND       hfgDisconnectInd;
  T_AMOPENAT_BT_HFG_DISCONNECT_CNF       hfgDisconnectCnf;
  T_AMOPENAT_BT_PHONE_CALL_INFO          phoneCallInfo;
  T_AMOPENAT_BT_HFG_SCO_CON_CNF          hfgScoConCnf;
  T_AMOPENAT_BT_HFG_SCO_DISCON_CNF       hfgScoDisConCnf;
  T_AMOPENAT_BT_HFG_SCO_INFO_IND         hfgScoInfoInd;
  T_AMOPENAT_BT_SPP_CONN_IND             sppConnInd;
  T_AMOPENAT_BT_SPP_DISCONN_IND          sppDiscConInd;
  T_AMOPENAT_BT_SPP_DISCONN_CNF          sppDiscConCnf;
  T_AMOPENAT_BT_SPP_CONN_CNF             sppConnCnf;
  T_AMOPENAT_BT_SPP_DATA_IND_T           sppDataInd;
  T_AMOPENAT_BT_SPP_SEND_DATA_CNF        sppSendDataCnf;
}U_AMOPENAT_BT_EVENT_PARAM;

typedef VOID (*OPENAT_BLUETOOTH_EVT_HANDLE)(E_AMOPENAT_BT_EVENT event,U_AMOPENAT_BT_EVENT_PARAM* param);


typedef struct
{
	T_AMOPENAT_BT_ADDR	  bd_addr;
	UINT8         linkKey[0x10];
	UINT8         KeyType;
	UINT32   	      cod;
}T_AMOPENAT_BT_DEV_PAIRED;

typedef struct 
{
	UINT32 		                devNum;
	T_AMOPENAT_BT_DEV_PAIRED	devInfo_Array[10];
}T_AMOPENAT_BT_DEV_INFO_ARRAY;

typedef struct 
{
  E_AMOPENAT_BT_SCANNABLE scannable;
  T_AMOPENAT_BT_NAME      btName;
  UINT32                  supportProfile; /*OPENAT_BT_SUPPORT_SPP_PROFILE*/
  T_AMOPENAT_BT_DEV_INFO_ARRAY pairedDevices;
  OPENAT_BLUETOOTH_EVT_HANDLE pMessageHandle;
}T_AMOPENAT_BT_PARAM;


typedef union
{
  struct _mgr_discory_req
  {
    UINT32	                cod;              /**/
    UINT8	                  inquiry_timeoutms; /*��ѯ��ʱ*/
    UINT8	                  inquiry_max_count; /*����ѯ����*/
    BOOL                  	discovery_with_name;
  }mgrDiscoryReq;
  struct _mgr_discory_cancle
  {
    UINT8 nullParm;
  }mgrDiscoryCancle;
  struct _mgr_set_scannable
  {
    E_AMOPENAT_BT_SCANNABLE scannable;
  }mgrSetScannable;
  struct _mgr_read_local_addr
  {
    UINT8 nullParm;
  }mgrReadLocalAddr;
  struct _mgr_write_local_name
  {
    T_AMOPENAT_BT_NAME localName;
  }mgrWriteLocalName;
  struct _bm_bonding_req
  {
    T_AMOPENAT_BT_ADDR remoteAddr;
    UINT8             sdap_len;
    UINT32            sdap_uuid[OPENAT_BT_MAX_SDAP_UUID_NO];
  }bmBondingReq;
  struct _bm_debond_req
  {
    T_AMOPENAT_BT_ADDR remoteAddr;
  }bmDebondReq;
  struct _pbap_con_req
  {
    T_AMOPENAT_BT_ADDR remoteAddr;
  }pbapConReq;
  struct _pbap_get_next_req
  {
    UINT8 nullParm;
  }pbapGetNextReq;
  struct _pbap_con_cancel
  {
    UINT8 nullParm;
  }pbapConCancel;
  struct _hfp_con_req
  {
    T_AMOPENAT_BT_ADDR remoteAddr;
  }hfpConReq;
  struct _hfp_dial_req
  {
    UINT8 call_number[OPENAT_BT_MAX_PHONE_NUMBER];
    UINT8 call_number_length;
  }hfpDialReq;
  struct _hfp_hungup_req
  {
    UINT8 nullParm;
  }hfpHungUpReq;
  struct _hfp_call_answer
  {
    UINT8 nullParam;
  }hfpCallAnswer;
  struct _hfp_send_dtmf
  {
    UINT8 assicNum;
  }hfpSendDTMF;
  struct _hfp_sco_audio_open_req
  {
    BOOL open;
  }hfpScoAudoReq;
  struct _hfp_disconnect_req
  {
    T_AMOPENAT_BT_ADDR remoteAddr;
    UINT32 connectId;
  }hfpDisconnectReq;
  struct _hfp_connect_sco_req
  {
    BOOL connect;
    UINT32 connect_id;
  }hfpConnectSCOReq;
  struct _hfg_set_mic_gain_req
  {
    INT8 anaGain; /*0 ~ 21dB*/
    INT8 adcGain; /*-12 ~ 16dB*/
    INT8 nsGain; /*1 ~ 16*/
  }hfgSetMicGainReq;
  struct _hfg_set_sleep_mode_req
  {
    BOOL quickSleepMode;
  }hfgSetSleepModeReq;
}U_AMOPENAT_BT_CMD_PARAM;

typedef union
{
  struct _mgr_link_accept
  {
    BOOL       accept;
    T_AMOPENAT_BT_ADDR remote_bd_addr;
  }mgrLinkAccept;
  struct _mgr_pin_code
  {
    T_AMOPENAT_BT_ADDR	remote_bd_addr;
    UINT8	pin_len;
    UINT8	pin_code[16];
  }mgrPinCode;
  
}U_AMOPENAT_BT_RSP_PARAM;
/*-\NEW\brezen\2013.12.30\��������ӿ�*/

/** flash�������
*/
typedef enum
{
    
    OPENAT_MEMD_ERR_NO = 0, ///< No error occured

    
    OPENAT_MEMD_ERR_ERASE = -10, ///< An error occurred during the erase operation
 
    
    OPENAT_MEMD_ERR_WRITE, ///< An error occurred during the write operation

    
    OPENAT_MEMD_ERR_ALIGN,/**< This error occurs when a command requiring 
							 sector aligned addresses as parameters is called with unaligned addresses. 
							 (Those are 
							 #memd_FlashErrors memd_FlashErase, #memd_FlashLock and #memd_FlashUnlock)*/

    
    OPENAT_MEMD_ERR_PROTECT, /**< An erase or write operation was attempted on a protected sector*/
    
    OPENAT_MEMD_ERR_SUSPEND, ///< erase suspend.
    
    OPENAT_MEMD_ERR_BUSY, ///< device is busy.
    
}E_AMOPENAT_MEMD_ERR;

/*+\NEW\RUFEI\2014.4.4\�����ⲿ���Ź����ýӿ�*/
/*CUSTOM*/
typedef enum
{
    OPENAT_CUST_EX_WATCH_DOG_MODE_DEFAULT,
    OPENAT_CUST_EX_WATCH_DOG_MODE_CUST,

    OPENAT_CUST_EX_WATCH_DOG_MODE_NUM
}E_OPEANT_CUST_EX_WATCH_DOG_MODE;

typedef void (*CUSHandle)(void);

typedef union
{
    struct _default_mode_cfg
    {
        E_AMOPENAT_GPIO_PORT port;
        CUSHandle                      handle; /*reset�ӿ�ʹ��*/
    }defaultModeCfg;
    struct _cust_mode_cfg
    {
        CUSHandle                      handle;/*reset�ӿ�ʹ��*/
    }custModeCfg;
}U_AMOPENAT_EX_WATCH_DOG_CFG;
/*-\NEW\RUFEI\2014.4.4\�����ⲿ���Ź����ýӿ�*/

/*+\NEW\RUFEI\2014.8.20\����gps�ӿ�ʵ��*/
typedef struct
{
    E_AMOPENAT_UART_PORT port;
    T_AMOPENAT_UART_PARAM cfg;
}T_AMOPENAT_RDAGPS_UART_CFG;

typedef struct
{
    E_AMOPENAT_I2C_PORT port;
}T_AMOPENAT_RDAGPS_I2C_CFG;

typedef struct
{
    E_AMOPENAT_GPIO_PORT pinPowerOnPort;
    E_AMOPENAT_GPIO_PORT pinResetPort;
    E_AMOPENAT_GPIO_PORT pinBpWakeupGpsPort;
    BOOL                             pinBpWakeupGpsPolarity;
    E_AMOPENAT_GPIO_PORT pinGpsWakeupBpPort;
    BOOL                             pinGpsWakeupBpPolarity;
}T_AMOPENAT_RDAGPS_GPS_CFG;
typedef struct
{
    T_AMOPENAT_RDAGPS_I2C_CFG    i2c;
    T_AMOPENAT_RDAGPS_GPS_CFG   gps;
}T_AMOPENAT_RDAGPS_PARAM;
/*-\NEW\RUFEI\2014.8.20\����gps�ӿ�ʵ��*/

/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* AM_OPENAT_DRV_H */
