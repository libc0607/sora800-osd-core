/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    platform_pmd.h
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/3/26
 *
 * Description:
 *          platform power manage �ӿ�
 **************************************************************************/

#ifndef _PLATFORM_PMD_H_
#define _PLATFORM_PMD_H_

typedef enum PlatformLdoIdTag
{
    PLATFORM_LDO_KEYPAD,
    PLATFORM_LDO_LCD,

/*+\NEW\liweiqiang\2013.5.8\����KP_LEDR,G,B����*/
    PLATFORM_LDO_KP_LEDR,
    PLATFORM_LDO_KP_LEDG,
    PLATFORM_LDO_KP_LEDB,
/*+\NEW\liweiqiang\2013.5.8\����KP_LEDR,G,B����*/

/*+\NEW\liweiqiang\2013.6.1\����LDO_VIB���ƽӿ�*/
    PLATFORM_LDO_VIB,
/*-\NEW\liweiqiang\2013.6.1\����LDO_VIB���ƽӿ�*/

/*+\NEW\liweiqiang\2013.10.10\����LDO_VLCD����POWER_VLCD*/
    PLATFORM_LDO_VLCD,
/*-\NEW\liweiqiang\2013.10.10\����LDO_VLCD����POWER_VLCD*/

/*+\NEW\liweiqiang\2013.11.8\����LDO_VASW,VMMC����*/
    PLATFORM_LDO_VASW,
    PLATFORM_LDO_VMMC,
/*-\NEW\liweiqiang\2013.11.8\����LDO_VASW,VMMC����*/

/*+\new\liweiqiang\2014.5.9\����LDO_VCAM���� */
    PLATFORM_LDO_VCAM,
/*-\new\liweiqiang\2014.5.9\����LDO_VCAM���� */

    PLATFORM_LDO_QTY
}PlatformLdoId;

/*+\NEW\liweiqiang\2013.9.8\����pmd.init���ó������ӿ� */
/*+\NEW\liweiqiang\2014.2.8\���Ƶ�Դ�������ýӿ� */
#define PMD_CFG_INVALID_VALUE           (0xffff)

typedef struct PlatformPmdCfgTag
{
    u16             battFullLevel;
    u16             battRechargeLevel;
    u16             poweronLevel;
    u16             poweroffLevel;
    u16             currentFirst;
    u16             battlevelFirst;
    u16             currentSecond;
    u16             battlevelSecond;
    u16             currentThird;
    /*+\NEW\zhuth\2014.11.6\��Դ�������ò���������Ƿ����ص�����*/
    u16             batdetectEnable;
    /*-\NEW\zhuth\2014.11.6\��Դ�������ò���������Ƿ����ص�����*/
    u16             intervaltimeFirst;
    u16             intervaltimeSecond;
    u16             intervaltimeThird;
    u16             pluschgctlEnable;
    u16             pluschgonTime;
    u16             pluschgoffTime;
}PlatformPmdCfg;
/*-\NEW\liweiqiang\2014.2.8\���Ƶ�Դ�������ýӿ� */

int platform_pmd_init(PlatformPmdCfg *pmdCfg);
/*-\NEW\liweiqiang\2013.9.8\����pmd.init���ó������ӿ� */

int platform_ldo_set(PlatformLdoId id, int level);

//sleep_wake: 1 sleep 0 wakeup
int platform_pmd_powersave(int sleep_wake);

/*+\NEW\liweiqiang\2014.2.13\����pmd.charger��ѯ�����״̬�ӿ� */
int platform_pmd_get_charger(void);
/*-\NEW\liweiqiang\2014.2.13\����pmd.charger��ѯ�����״̬�ӿ� */

#endif//_PLATFORM_PMD_H_
