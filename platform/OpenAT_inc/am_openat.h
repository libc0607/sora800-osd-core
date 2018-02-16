/*********************************************************
  Copyright (C), AirM2M Tech. Co., Ltd.
  Author: lifei
  Description: AMOPENAT ����ƽ̨
  Others:
  History: 
    Version�� Date:       Author:   Modification:
    V0.1      2012.12.14  lifei     �����ļ�
	V0.2      2012.12.26  brezen    ���pmd�ӿ�
	V0.3      2012.12.29  brezen    ���spi�ӿ�
	V0.4      2013.01.08  brezen    �޸�spi�ӿ�
	V0.5      2O13.01.14  brezen    1�����Ӻڰ�����ʼ������
	                                2�����Ӻڰ��������ӿ�
									3�����ٺڰ���ָ���ʱ�䣬���㵥��ʹ��ָ��ˢ��
    V0.6      2013.01.14  brezen    �޸ĺڰ��������ӿڲ���
	V0.7      2013.01.15  brezen    �޸ĺڰ��������ӿڲ���
    V0.8      2013.01.17  brezen    1�����ϵͳ��Դ���ƽӿ� 2�����ϵͳ���ػ��ӿ�
    V0.9      2013.01.23  brezen    �޸�SPI�ı���warning  
    V1.0      2013.01.28  brezen    ���PSAM���ӿ�
    V1.1      2013.01.30  brezen    �޸�poweron_system��������
    V1.2      2013.02.06  Jack.li   �������ͷ�ӿ�
    V1.3      2013.02.07  Jack.li   �����Ƶ¼�ơ���Ƶ���Žӿ�
    V1.4      2013.02.10  Jack.li   �޸Ĳ�����ʼ���ӿ�
    V1.5      2013.02.26  brezen    ���enter_deepsleep/exit_deepsleep�ӿ�
    V1.6      2013.03.21  maliang    �ļ�ϵͳ�ӿںͲ�����Ƶ�ļ��ӿڵ��ļ�����Ϊunicode little ending����
    V1.7      2013.04.03  Jack.li    ����I2C�ӿ�
    V1.8      2013.05.15  xc        ����tts�ӿ�
	V1.9      2013.07.18  brezen    ���set_dte_at_filter�ӿ�
	V2.0      2013.07.22  brezen    ���send_data_to_dte  send_data_to_ci set_dte_device�ӿ�
	V2.1      2013.08.20  brezen    ���PSAM��˫����֤ʧ�ܣ�rw_psam��ӷֲ����Ͳ���stopClock
	V2.2      2013.09.16  brezen    ���flush_file�ӿڣ�����֮ǰǿ��д��flash
	V2.3      2013.09.24  brezen    ���NV�ӿ�
	V2.4      2013.09.26  brezen    ֧������PSAM��
	V2.5      2013.12.30  brezen    ��������ӿ�
	V2.6      2014.6.26   brezen    �������spp�ӿ�
*********************************************************/
#ifndef AM_OPENAT_H
#define AM_OPENAT_H

#include "am_openat_system.h"
#include "am_openat_fs.h"
#include "am_openat_drv.h"
#include "am_openat_vat.h"
#include "am_openat_socket.h"

/*+\NEW\WZQ\2014.11.7\����SSL RSA����*/
#include "openat_SSLRSA.h"
/*-\NEW\WZQ\2014.11.7\����SSL RSA����*/

#include "am_openat_image.h"

typedef struct T_AMOPENAT_INTERFACE_VTBL_TAG
{
    /*******************************************
    **                 SYSTEM                 **
    *******************************************/
    /****************************** �߳̽ӿ� ******************************/
    HANDLE (*create_task)(                          /* �����߳̽ӿ� */
                            PTASK_MAIN pTaskEntry,  /* �߳������� */
                            PVOID pParameter,       /* ��Ϊ�������ݸ��߳������� */
                            PVOID pStackAddr,       /* �߳�ջ��ַ����ǰ��֧�֣��봫��NULL */
                            UINT16 nStackSize,      /* �߳�ջ��С */
                            UINT8 nPriority,        /* �߳����ȼ����ò���Խ���߳����ȼ�Խ�� */
                            UINT16 nCreationFlags,  /* �߳�������ǣ� ��ο�E_AMOPENAT_OS_CREATION_FLAG */
                            UINT16 nTimeSlice,      /* ��ʱ��֧�֣��봫��0 */
                            PCHAR pTaskName         /* �߳����� */
                          );
    VOID (*start_task)(                             /* �����߳̽ӿ� */
                            HANDLE hTask,           /* �߳̾����create_task�ӿڷ���ֵ */
                            PVOID pParameter        /* ��Ϊ�������ݸ��߳������������滻create_task�ӿڴ����pParameter���� */
                      );
    VOID (*stop_task)(                              /* ֹͣ�߳̽ӿ� */
                            HANDLE hTask            /* �߳̾����create_task�ӿڷ���ֵ */
                     );
    BOOL (*delete_task)(                            /* ɾ���߳̽ӿ� */
                            HANDLE hTask            /* �߳̾����create_task�ӿڷ���ֵ */
                       );
    BOOL (*suspend_task)(                           /* �����߳̽ӿ� */
                            HANDLE hTask            /* �߳̾����create_task�ӿڷ���ֵ */
                        );
    BOOL (*resume_task)(                            /* �ָ��߳̽ӿ� */
                            HANDLE hTask            /* �߳̾����create_task�ӿڷ���ֵ */
                       );
    HANDLE (*current_task)(                         /* ��ȡ��ǰ�߳̽ӿ� */
                            VOID
                          );
    BOOL (*get_task_info)(                          /* ��ȡ��ǰ�̴߳�����Ϣ�ӿ� */
                            HANDLE hTask,           /* �߳̾����create_task�ӿڷ���ֵ */
                            T_AMOPENAT_TASK_INFO *pTaskInfo /* �߳���Ϣ�洢�ӿ� */
                         );

    /*+\NEW\WZQ\2014.11.10\����sscanf�ӿ�*/
    int (*sscanf)(
        const char *buf, 
        const char *fmt, ...);
    /*-\NEW\WZQ\2014.11.10\����sscanf�ӿ�*/

    /****************************** �߳���Ϣ���нӿ� ******************************/
    BOOL (*wait_message)(                           /* ��ȡ�߳���Ϣ�ӿڣ������ӿ� */
                            HANDLE hTask,           /* �߳̾����create_task�ӿڷ���ֵ */
                            PVOID* ppMessage,       /* �洢��Ϣָ�� */
                            UINT32 nTimeOut         /* �ȴ���Ϣ��ʱʱ�䣬Ŀǰ��֧�� */
                        );
    BOOL (*send_message)(                           /* ������Ϣ�ӿڣ���ӵ���Ϣ����β�� */
                            HANDLE hTask,           /* �߳̾����create_task�ӿڷ���ֵ */
                            PVOID pMessage          /* �洢��Ϣָ�� */
                        );
    BOOL (*send_high_priority_message)(             /* ���͸����ȼ���Ϣ�ӿڣ���ӵ���Ϣ����ͷ�� */
                            HANDLE hTask,           /* �߳̾����create_task�ӿڷ���ֵ */
                            PVOID pMessage          /* Ҫ������Ϣָ�� */
                                      );
    BOOL (*available_message)(                      /* �����Ϣ�������Ƿ�����Ϣ */
                            HANDLE hTask            /* �߳̾����create_task�ӿڷ���ֵ */
                             );

    /****************************** ʱ��&��ʱ���ӿ� ******************************/
    HANDLE (*create_timer)(                         /* ������ʱ���ӿ� */
                            PTIMER_EXPFUNC pFunc,   /* ��ʱ����ʱ������ */
                            PVOID pParameter        /* ��Ϊ�������ݸ���ʱ����ʱ������ */
                          );
    BOOL (*start_timer)(                            /* ������ʱ���ӿ� */
                            HANDLE hTimer,          /* ��ʱ�������create_timer�ӿڷ���ֵ */
                            UINT32 nMillisecondes   /* ��ʱ��ʱ�� */
                       );
    BOOL (*stop_timer)(                             /* ֹͣ��ʱ���ӿ� */
                            HANDLE hTimer           /* ��ʱ�������create_timer�ӿڷ���ֵ */
                      );
    BOOL (*delete_timer)(                           /* ɾ����ʱ���ӿ� */
                            HANDLE hTimer           /* ��ʱ�������create_timer�ӿڷ���ֵ */
                        );
    BOOL (*available_timer)(                        /* ��鶨ʱ���Ƿ��Ѿ������ӿ� */
                            HANDLE hTimer           /* ��ʱ�������create_timer�ӿڷ���ֵ */
                           );
    BOOL (*get_minute_tick)(                        /* minute indication infterface */
                            PMINUTE_TICKFUNC pFunc  /* if pFunc != NULL, one MINUTE interval timer will be started. else the timer will be stop */
                           );
    BOOL (*get_system_datetime)(                    /* ��ȡϵͳʱ��ӿ� */
                            T_AMOPENAT_SYSTEM_DATETIME* pDatetime/* �洢ʱ��ָ�� */
                           );
    BOOL (*set_system_datetime)(                    /* ����ϵͳʱ��ӿ� */
                            T_AMOPENAT_SYSTEM_DATETIME* pDatetime/* �洢ʱ��ָ�� */
                           );
    /****************************** ALARM�ӿ� ******************************/
    BOOL (*init_alarm)(                                        /* ���ӳ�ʼ���ӿ� */
                            T_AMOPENAT_ALARM_CONFIG *pConfig   /* �������ò��� */
                       ); 
    BOOL (*set_alarm)(                                        /* ��������/ɾ���ӿ� */
                            T_AMOPENAT_ALARM_PARAM *pAlarmSet    /* �������ò��� */
                       );
    /****************************** �ٽ���Դ�ӿ� ******************************/
    HANDLE (*enter_critical_section)(               /* �����ٽ���Դ���ӿڣ��ر������ж� */
                            VOID
                                    );
    VOID (*exit_critical_section)(                  /* �˳��ٽ���Դ���ӿڣ������ж� */
                            HANDLE hSection         /* �ٽ���Դ�������enter_critical_section�ӿڷ���ֵ */
                                 );
    /****************************** �ź����ӿ� ******************************/
    HANDLE (*create_semaphore)(                     /* �����ź����ӿ� */
                            UINT32 nInitCount       /* �ź������� */
                              );
    BOOL (*delete_semaphore)(                       /* ɾ���ź����ӿ� */
                            HANDLE hSem             /* �ź��������create_semaphore�ӿڷ���ֵ */
                            );
    BOOL (*wait_semaphore)(                         /* ��ȡ�ź����ӿ� */
                            HANDLE hSem,            /* �ź��������create_semaphore�ӿڷ���ֵ */
                            UINT32 nTimeOut         /* ��ȡ�ź�����ʱʱ�䣬if nTimeOut < 5ms, means forever */
                          );
    BOOL (*release_semaphore)(
                            HANDLE hSem             /* �ź��������create_semaphore�ӿڷ���ֵ */
                             );
    UINT32 (*get_semaphore_value)                   /* ��ȡ������ֵ*/
                            (
                            HANDLE hSem             /* �ź��������create_semaphore�ӿڷ���ֵ */  
                            );
    /****************************** �ڴ�ӿ� ******************************/
    PVOID (*malloc)(                                /* �ڴ�����ӿ� */
                            UINT32 nSize            /* ������ڴ��С */
                   );
    PVOID (*realloc)(                               /**/
                            PVOID pMemory,          /* �ڴ�ָ�룬malloc�ӿڷ���ֵ */
                            UINT32 nSize            /* ������ڴ��С */
                    );
    VOID (*free)(                                   /* �ڴ��ͷŽӿ� */
                            PVOID pMemory           /* �ڴ�ָ�룬malloc�ӿڷ���ֵ */
                );
    /****************************** ����ӿ� ******************************/
    BOOL (*sleep)(                                  /* ϵͳ˯�߽ӿ� */
                            UINT32 nMillisecondes   /* ˯��ʱ�� */
                 );
    UINT32 (*get_system_tick)(                      /* ��ȡϵͳtick�ӿ� */
                            VOID
                             );
    UINT32 (*rand)(                                 /* ��ȡ������ӿ� */
                            VOID
                  );
    VOID (*srand)(                                  /* ������������ӽӿ� */
                            UINT32 seed             /* ��������� */
                 );
    VOID (*shut_down)(                              /* �ػ��ӿ� */
                            VOID
                     );
    VOID (*restart)(                                /* �����ӿ� */
                            VOID
                   );
/*+\NEW\liweiqiang\2013.7.1\[OpenAt]����ϵͳ��Ƶ���ýӿ�*/
    VOID (*sys_request_freq)(                       /* ��Ƶ���ƽӿ� */
                            E_AMOPENAT_SYS_FREQ freq/* ��Ƶֵ */
                   );
/*-\NEW\liweiqiang\2013.7.1\[OpenAt]����ϵͳ��Ƶ���ýӿ�*/
    /*******************************************
    **              FILE SYSTEM               **
    *******************************************/
    INT32 (*open_file)(                             /* ���ļ��ӿ� *//* �����������ֵ��0��ʼ��С��0������ */
/*+\BUG WM-719\maliang\2013.3.21\�ļ�ϵͳ�ӿںͲ�����Ƶ�ļ��ӿڵ��ļ�����Ϊunicode little ending����*/
                            char* pcFileName,       /* �ļ�ȫ·������ unicode little endian*/
                            UINT32 iFlag,           /* �򿪱�־ */
	                        UINT32 iAttr            /* �ļ����ԣ���ʱ��֧�֣�������0 */
                      );
    INT32 (*close_file)(                            /* �ر��ļ��ӿ� */
                            INT32 iFd               /* �ļ������open_file �� create_file ���ص���Ч���� */
                       );
    INT32 (*read_file)(                             /* ��ȡ�ļ��ӿ� */
                            INT32 iFd,              /* �ļ������open_file �� create_file ���ص���Ч���� */
                            UINT8 *pBuf,            /* ���ݱ���ָ�� */
                            UINT32 iLen             /* buf���� */
                      );
    INT32 (*write_file)(                            /* д���ļ��ӿ�*/
                            INT32 iFd,              /* �ļ������open_file �� create_file ���ص���Ч���� */
                            UINT8 *pBuf,            /* ��Ҫд�������ָ�� */
                            UINT32 iLen             /* ���ݳ��� */
                       );
    INT32 (*flush_file)(                            /* ����д��flash*/
                            INT32 iFd               /* �ļ������open_file �� create_file ���ص���Ч���� */
                       );    
    INT32 (*seek_file)(                             /* �ļ���λ�ӿ� */
                            INT32 iFd,              /* �ļ������open_file �� create_file ���ص���Ч���� */
                            INT32 iOffset,          /* ƫ���� */
                            UINT8 iOrigin           /* ƫ����ʼλ�� */
                      );
    INT32 (*create_file)(                           /* �����ļ��ӿ� */
                            char* pcFileName,       /* �ļ�ȫ·������ unicode little endian*/
                            UINT32 iAttr            /* �ļ����ԣ���ʱ��֧�֣�������0 */
                        );
    INT32 (*delete_file)(                           /* ɾ���ļ��ӿ� */
                            char* pcFileName        /* �ļ�ȫ·������ unicode little endian*/
                        );
    INT32 (*rename_file)(                           /*�ļ��������ӿ�*/
                            char* pcOldName, 
                            char* pcNewName
                         );
    INT32 (*change_size)(
                        INT32 iFd,
                        UINT32 uSize
                    );
    INT32 (*change_dir)(                            /* �л���ǰ����Ŀ¼�ӿ� */
                            char* pcDirName       /* Ŀ¼·�� unicode little endian */
                       );
    INT32 (*make_dir)(                              /* ����Ŀ¼�ӿ� */
                            char* pcDirName,        /* Ŀ¼·�� unicode little endian */
                            UINT32 iMode            /* Ŀ¼���ԣ���ϸ��μ� E_AMOPENAT_FILE_ATTR */
                     );
    INT32 (*remove_dir)(                            /* ɾ��Ŀ¼�ӿ� *//* ��Ŀ¼����Ϊ�գ��ӿڲ��ܷ��سɹ� */
                            char* pcDirName         /* Ŀ¼·�� unicode little endian */
                       );
    INT32 (*remove_dir_rec)(                        /* �ݹ�ɾ��Ŀ¼�ӿ� *//* ��Ŀ¼�������ļ���Ŀ¼���ᱻɾ�� */
                            char* pcDirName         /* Ŀ¼·�� unicode little endian */
                           );
    INT32 (*get_current_dir)(                       /* ��ȡ��ǰĿ¼�ӿ� */
                            char* pcCurDir,    /* �洢Ŀ¼��Ϣ unicode little endian */
                            UINT32 uUnicodeSize     /* �洢Ŀ¼��Ϣ�ռ��С */
                            );
    INT32 (*find_first_file)(                       /* �����ļ��ӿ� */
                            char* pszFileNameUniLe,/* Ŀ¼·�����ļ�ȫ·�� unicode little endian */
/*-\BUG WM-719\maliang\2013.3.21\�ļ�ϵͳ�ӿںͲ�����Ƶ�ļ��ӿڵ��ļ�����Ϊunicode little ending����*/
                            PAMOPENAT_FS_FIND_DATA  pFindData /* ���ҽ������ */
                            );
    INT32 (*find_next_file)(                        /* ���������ļ��ӿ� */
                            INT32 iFd,              /* �����ļ������Ϊ find_first_file �ӿڷ��ز��� */
                            PAMOPENAT_FS_FIND_DATA  pFindData /* ���ҽ������ */
                           );
    INT32 (*find_close)(                            /* ���ҽ����ӿ� */
                            INT32 iFd               /* �����ļ������Ϊ find_first_file �ӿڷ��ز��� */
                       );
/*+\NewReq WM-743\maliang\2013.3.28\[OpenAt]���ӽӿڻ�ȡ�ļ�ϵͳ��Ϣ*/
    INT32 (*get_fs_info)(                            /* ��ȡ�ļ�ϵͳ��Ϣ�ӿ� */
                            E_AMOPENAT_FILE_DEVICE_NAME       devName,            /*��ȡ�Ŀ�device name����Ϣ*/
                            T_AMOPENAT_FILE_INFO               *fileInfo                   /*�ļ�ϵͳ����Ϣ*/
                       );
/*-\NewReq WM-743\maliang\2013.3.28\[OpenAt]���ӽӿڻ�ȡ�ļ�ϵͳ��Ϣ*/
    
    /*+\NewReq\Jack.li\2013.1.17\����T���ӿ�*/
    INT32 (*init_tflash)(                            /* ��ʼ��T���ӿ� */
                            PAMOPENAT_TFLASH_INIT_PARAM pTlashInitParam/* T����ʼ������ */
                       );
    /*-\NewReq\Jack.li\2013.1.17\����T���ӿ�*/

    E_AMOPENAT_MEMD_ERR (*flash_erase)(              /*flash��д 64K/4KB����*/
                            UINT32 startAddr,
                            UINT32 endAddr
                       );
    E_AMOPENAT_MEMD_ERR (*flash_write)(              /*дflash*/
                            UINT32 startAddr,
                            UINT32 size,
                            UINT32* writenSize,
                            CONST UINT8* buf
                       );
    E_AMOPENAT_MEMD_ERR (*flash_read)(               /*��flash*/
                            UINT32 startAddr,
                            UINT32 size,
                            UINT32* readSize,
                            UINT8* buf
                       );
    
	UINT32(*flash_get_valid_region)(/*��ȡAPP����flash�ռ�*/
								PVOID* addr, /*[out] flash���ÿռ��ַ*/
								UINT32* size /*[out] flash���ÿռ��С*/
								); 
	
	BOOL  (*flash_set_newapp)(/*������APP�洢��ַ*/
								CONST char* file/*��APP�洢��ַ*/
								);
    
    /*******************************************
    **                 NV                     **
    *******************************************/    
    /*��Ϊ����Ľӿڻ�ֱ�Ӳ���flash��������ϵͳ��������Ҫ���жϻ���Ҫ��Ƚϸߵ�TASK������*/    
    INT32 (*nv_init)(                                /*NV ��ʼ���ӿ�*/
                      UINT32 addr1,                  /*NV ��ŵ�ַ1 4KByte��ַ���� ��С4KByte*/
                      UINT32 addr2                   /*NV ��ŵ�ַ2 4KByte��ַ���� ��С4KByte*/
                    );

    INT32 (*nv_add)(                                 /*����һ��NV�洢����*/
                      UINT32 nv_id,                  /*NV ID Ŀǰֻ֧��0-255*/
                      UINT32 nv_size                 /*NV �����С,��λByte,���512Byte*/
                    );

    INT32 (*nv_delete)(                              /*ɾ��NV*/
                      UINT32 nv_id
                      );                 

    INT32 (*nv_read)(                                /*��ȡNV����*/
                     UINT32 nv_id,                   /*NV ID Ŀǰֻ֧��0-255*/
                     UINT8* buf,                     /*buf*/
                     UINT32 bufSize,                 /*buf�Ĵ�С*/
                     UINT32* readSize                /*ʵ�ʶ�ȡ����*/
                    );
    
    INT32 (*nv_write)(                               /*д��NV����*/
                      UINT32 nv_id,                  /*NV ID Ŀǰֻ֧��0-255*/
                      UINT8* buf,                    /*buf*/
                      UINT32 bufSize,                /*buf�Ĵ�С*/
                      UINT32* writeSize              /*ʵ��д�볤��*/
                     );          
    /*******************************************
    **                Hardware                **
    *******************************************/
    /****************************** GPIO ******************************/
    BOOL (*config_gpio)(                          
                            E_AMOPENAT_GPIO_PORT port,  /* GPIO��� */
                            T_AMOPENAT_GPIO_CFG *cfg    /* ��������� */
                       );
    BOOL (*set_gpio)(                               
                            E_AMOPENAT_GPIO_PORT port,  /* GPIO��� */
                            UINT8 value                 /* 0 or 1 */
                    );
/*+:\NewReq WM-475\brezen\2012.12.14\�޸�gpio�ӿ� */				
    BOOL (*read_gpio)(                            
                            E_AMOPENAT_GPIO_PORT port,  /* GPIO��� */
                            UINT8* value                /* ��� 0 or 1 */
                      );
/*-:\NewReq WM-475\brezen\2012.12.14\�޸�gpio�ӿ� */

/*+\BUG WM-720\rufei\2013.3.21\ ����gpio��close�ӿ�*/
    BOOL (*close_gpio)(                            
                            E_AMOPENAT_GPIO_PORT port/* GPIO��� */
                      );
/*-\BUG WM-720\rufei\2013.3.21\ ����gpio��close�ӿ�*/
    
    /****************************** PMD ******************************/
    BOOL (*init_pmd)(     
                            E_AMOPENAT_PM_CHR_MODE chrMode,     /* ��緽ʽ */
/*+\NEW WM-746\rufei\2013.3.30\����оƬIC���*/
                            T_AMOPENAT_PMD_CFG*    cfg,         /*�������*/
/*-\NEW WM-746\rufei\2013.3.30\����оƬIC���*/
                            PPM_MESSAGE            pPmMessage   /* ��Ϣ�ص����� */
                    );
    VOID (*get_batteryStatus)(
                            T_AMOPENAT_BAT_STATUS* batStatus    /* ���״̬ OUT */
                             );
    VOID (*get_chargerStatus)(
                            T_AMOPENAT_CHARGER_STATUS* chrStatus/* �����״̬ OUT */
                             );
/*+\NEW\RUFEI\2014.2.13\����OPENAT��ѯ�����HW״̬�ӿ�*/
    E_AMOPENAT_CHR_HW_STATUS (*get_chargerHwStatus)(
                            VOID
                            );
/*+NEW\zhuwangbin\2017.2.10\��ӳ�������ѯ�ӿ�*/
	int (*get_chg_param)(BOOL *battStatus, u16 *battVolt, u8 *battLevel, BOOL *chargerStatus, u8 *chargeState);
/*+NEW\zhuwangbin\2017.2.10\��ӳ�������ѯ�ӿ�*/
/*-\NEW\RUFEI\2014.2.13\����OPENAT��ѯ�����HW״̬�ӿ�*/
    BOOL (*poweron_system)(                                     /* �������� */  
                            E_AMOPENAT_STARTUP_MODE simStartUpMode,/* ����SIM����ʽ */
                            E_AMOPENAT_STARTUP_MODE nwStartupMode/* ����Э��ջ��ʽ */
                          );
    VOID (*poweroff_system)(                                    /* �����ػ��������ر�Э��ջ�͹��� */        
                            VOID
                           );
    BOOL (*poweron_ldo)(                                        /* ��LDO */
                            E_AMOPENAT_PM_LDO    ldo,
                            UINT8                level          /*0-7 0:�ر� 1~7��ѹ�ȼ�*/
                       );
    VOID (*enter_deepsleep)                                     /* ����˯�� */
                       (
                         VOID
                       );
    VOID (*exit_deepsleep)                                      /* �˳�˯�� */
                       (
                         VOID
                       );

    void (*sys32k_clk_out)(bool);//�ӿ�Ϊ9501��ʼ���ṩ32Kʱ��Ƶ��
/*+NEW OPEANT-104\RUFEI\2014.6.17\ ���ӻ�ȡ����ԭ��ֵ�ӿ�*/

     E_AMOPENAT_POWERON_REASON (*get_poweronCasue )(                            /*��ȡ����ԭ��ֵ*/
                                                    VOID
                                                    );
/*-NEW OPEANT-104\RUFEI\2014.6.17\ ���ӻ�ȡ����ԭ��ֵ�ӿ�*/
    /****************************** UART ******************************/
    BOOL (*config_uart)(
                            E_AMOPENAT_UART_PORT port,          /* UART ��� */
                            T_AMOPENAT_UART_PARAM *cfg          /* ��ʼ������ */
                       );

/*+\NEW\liweiqiang\2013.4.20\���ӹر�uart�ӿ�*/
    BOOL (*close_uart)(
                            E_AMOPENAT_UART_PORT port           /* UART ��� */
                       );
/*-\NEW\liweiqiang\2013.4.20\���ӹر�uart�ӿ�*/

    UINT32 (*read_uart)(                                        /* ʵ�ʶ�ȡ���� */
                            E_AMOPENAT_UART_PORT port,          /* UART ��� */
                            UINT8* buf,                         /* �洢���ݵ�ַ */
                            UINT32 bufLen,                      /* �洢�ռ䳤�� */
                            UINT32 timeoutMs                    /* ��ȡ��ʱ ms */
                       );

    UINT32 (*write_uart)(                                       /* ʵ��д�볤�� */
                            E_AMOPENAT_UART_PORT port,          /* UART ��� */
                            UINT8* buf,                         /* д�����ݵ�ַ */
                            UINT32 bufLen                      /* д�����ݳ��� */
                        );

    BOOL (*uart_tx_done)(                                       /* �����Ƿ����*/
                            E_AMOPENAT_UART_PORT port          /* UART ��� */
                        );
                      
/*+\NEW\liweiqiang\2014.4.12\���Ӵ��ڽ����ж�ʹ�ܽӿ� */
    BOOL (*uart_enable_rx_int)(
                            E_AMOPENAT_UART_PORT port,          /* UART ��� */
                            BOOL enable                         /* �Ƿ�ʹ�� */
                                );
/*-\NEW\liweiqiang\2014.4.12\���Ӵ��ڽ����ж�ʹ�ܽӿ� */

/*+\NEW\zhuwangbin\2017.3.10\AIR200 ��Ŀ���uart1ȫpin�Ź���*/
	void (*uart1_all_pin_set)(void);
/*+\NEW\zhuwangbin\2017.3.10\AIR200 ��Ŀ���uart1ȫpin�Ź���*/
    /*+\NEW\zhutianhua\2018.2.5 16:51\�������uart buf�ӿ�*/
    BOOL (*clear_uart)(E_AMOPENAT_UART_PORT port, E_AMOPENAT_UART_BUF bufType);
    /*-\NEW\zhutianhua\2018.2.5 16:51\�������uart buf�ӿ�*/

/*+\NEW\liweiqiang\2013.12.25\���host uart�������ݹ��� */
    /****************************** HOST ******************************/
    BOOL (*host_init)(PHOST_MESSAGE hostCallback);
    BOOL (*host_send_data)(uint8 *data, uint32 len);
/*-\NEW\liweiqiang\2013.12.25\���host uart�������ݹ��� */

    /******************************* SPI ******************************/
    BOOL (*config_spi)(
                            E_AMOPENAT_SPI_PORT  port,          /* SPI ��� */
                            T_AMOPENAT_SPI_PARAM *cfg           /* ��ʼ������ */
                      );
    UINT32 (*read_spi)(                                         /* ʵ�ʶ�ȡ���� */
                            E_AMOPENAT_SPI_PORT port,          /* SPI ��� */
                            UINT8* buf,                         /* �洢���ݵ�ַ */
                            UINT32 bufLen                       /* �洢�ռ䳤�� */
                      );
    UINT32 (*write_spi)(                                        /* ʵ��д�볤�� */
                            E_AMOPENAT_SPI_PORT port,          /* SPI ��� */
                            CONST UINT8* buf,                   /* д�����ݵ�ַ */
                            UINT32 bufLen                       /* д�����ݳ��� */
                       );
    UINT32 (*rw_spi)(                                          /* ȫ˫����ʽ��д����д������ͬ */
                            E_AMOPENAT_SPI_PORT port,          /* SPI ��� */
                            CONST UINT8* txBuf,                 /* д���� */
                            UINT8* rxBuf,                       /* ������*/
                            UINT32 len                          /* ��д���� */
                    );
                   
    BOOL (*close_spi)(
                            E_AMOPENAT_SPI_PORT  port
                    );      
    /******************************* I2C ******************************/
    BOOL (*open_i2c)(
                            E_AMOPENAT_I2C_PORT  port,          /* I2C ��� */
                            T_AMOPENAT_I2C_PARAM *param         /* ��ʼ������ */
                      );
    BOOL (*close_i2c)(
                            E_AMOPENAT_I2C_PORT  port           /* I2C ��� */
                      );
    UINT32 (*write_i2c)(                                        /* ʵ��д�볤�� */
                            E_AMOPENAT_I2C_PORT port,          /* I2C ��� */
                            UINT8 salveAddr,
                            CONST UINT8 *pRegAddr,              /* I2C����Ĵ�����ַ */
                            CONST UINT8* buf,                   /* д�����ݵ�ַ */
                            UINT32 bufLen                       /* д�����ݳ��� */
                       );
    UINT32 (*read_i2c)(                                         /* ʵ�ʶ�ȡ���� */
                            E_AMOPENAT_I2C_PORT port,          /* I2C ��� */
                            UINT8 slaveAddr, 
                            CONST UINT8 *pRegAddr,              /* I2C����Ĵ�����ַ */
                            UINT8* buf,                         /* �洢���ݵ�ַ */
                            UINT32 bufLen                       /* �洢�ռ䳤�� */
                      );
    BOOL  (*open_bt)(
                            T_AMOPENAT_BT_PARAM* param
                     );
    BOOL  (*close_bt)(
                            VOID
                     );
    BOOL  (*poweron_bt)
                      (
                            VOID
                      );
                      
    BOOL  (*poweroff_bt)
                      (
                            VOID
                      );  
    BOOL  (*send_cmd_bt)
                      (
                            E_AMOPENAT_BT_CMD cmd, 
                            U_AMOPENAT_BT_CMD_PARAM* param
                      );    
    BOOL  (*build_rsp_bt)
                      (
                            E_AMOPENAT_BT_RSP rsp,
                            U_AMOPENAT_BT_RSP_PARAM* param
                      );                              
    BOOL  (*connect_spp)                                        /*������ΪDevA�豸�������������ӣ����ӽ��OPENAT_BT_SPP_CONNECT_CNF
                                                                  �����ΪDevB�豸�����Զ������������ӣ��ǾͲ���Ҫ��������ӿڣ�
                                                                  �Զ����Ӻ���յ�OPENAT_BT_SPP_CONNECT_IND��Ϣ*/
                      (
                            T_AMOPENAT_BT_ADDR* addr,
                            T_AMOPENAT_UART_PARAM* portParam    /*��ʱ��֧��,����дNULL��Ĭ������Ϊ9600,8(data),1(stop),none(parity)*/
                      );
    BOOL  (*disconnect_spp)                                     /*�Ͽ����ӣ���� OPENAT_BT_SPP_DISCONNECT_CNF*/
                      (
                            UINT8   port                        /*�˿ںţ�����OPENAT_BT_SPP_CONNECT_IND/OPENAT_BT_SPP_CONNECT_CNF���ϱ�*/
                      );                      
    INT32  (*write_spp)                                         /*���ͽ�����ڻص��������OPENAT_BT_SPP_SEND_DATA_CNF�¼����ϱ�*/
                                                                /*����ֵΪʵ��ִ��д��ĳ��ȣ����Ϊ0��ʾ����û�����ݱ����ͣ�Ҳû��
                                                                  OPENAT_BT_SPP_SEND_DATA_CNF�¼��ϱ�*/
                      (
                            UINT8   port,                       /*�˿ںţ�����OPENAT_BT_SPP_CONNECT_IND/OPENAT_BT_SPP_CONNECT_CNF���ϱ�*/
                            UINT8*  buf,                        /*���ܴ���"rls��ͷ���ַ������������Ϊ������RFCOMM��״̬������rls0*/
                            UINT32  bufLen                      /*һ����ഫ��T_AMOPENAT_BT_SPP_CONN_IND.maxFrameSize��С�ֽڵ�����*/
                      );
    INT32  (*read_spp)                                          /*�ص��������յ�OPENAT_BT_SPP_DATA_IND�¼��󣬵��øýӿڶ�ȡ*/
                                                                /*����ֵΪʵ�ʶ�ȡ����*/
                      (
                            UINT8   port,                       /*�˿ںţ�����OPENAT_BT_SPP_CONNECT_IND/OPENAT_BT_SPP_CONNECT_CNF���ϱ�*/
                            UINT8*  buf,
                            UINT32  bufLen
                      );                      
    /****************************** AUDIO ******************************/
    BOOL (*open_tch)(                                           /* ����������ͨ����ʼʱ���� */
                            VOID
                    );
    BOOL (*close_tch)(                                          /* �ر�������ͨ������ʱ���� */
                            VOID
                     );
    BOOL (*play_tone)(                                          /* ����TONE���ӿ� */
                            E_AMOPENAT_TONE_TYPE toneType,      /* TONE������ */
                            UINT16 duration,                    /* ����ʱ�� */
                            E_AMOPENAT_SPEAKER_GAIN volume      /* �������� */
                     );
    BOOL (*stop_tone)(                                          /* ֹͣ����TONE���ӿ� */
                            VOID
                     );
    BOOL (*play_dtmf)(                                          /* ����DTMF���ӿ� */
                            E_AMOPENAT_DTMF_TYPE dtmfType,      /* DTMF���� */
                            UINT16 duration,                    /* ����ʱ�� */
                            E_AMOPENAT_SPEAKER_GAIN volume      /* �������� */
                     );
    BOOL (*stop_dtmf)(                                          /* ֹͣ����DTMF���ӿ� */
                            VOID
                     );
/*+\NewReq WM-584\maliang\2013.2.21\[OpenAt]֧��T������MP3*/
    BOOL (*play_music)(T_AMOPENAT_PLAY_PARAM*  playParam);
/*-\NewReq WM-584\maliang\2013.2.21\[OpenAt]֧��T������MP3*/
    BOOL (*stop_music)(                                         /* ֹͣ��Ƶ���Žӿ� */
                            VOID
                      );
    BOOL (*pause_music)(                                        /* ��ͣ��Ƶ���Žӿ� */
                            VOID
                       );
    BOOL (*resume_music)(                                       /* ֹͣ��Ƶ���Žӿ� */
                            VOID
                        );
/*+\NewReq WM-710\maliang\2013.3.18\ [OpenAt]���ӽӿ�����MP3���ŵ���Ч*/
    BOOL (*set_eq)(                                       /* ����MP3��Ч*/
                            E_AMOPENAT_AUDIO_SET_EQ setEQ
                        );
/*-\NewReq WM-710\maliang\2013.3.18\ [OpenAt]���ӽӿ�����MP3���ŵ���Ч*/
    BOOL (*open_mic)(                                           /* ����MIC�ӿ� */
                            VOID
                    );
    BOOL (*close_mic)(                                          /* �ر�MIC�ӿ� */
                            VOID
                     );
    BOOL (*mute_mic)(                                           /* MIC�����ӿ� */
                            VOID
                    );
    BOOL (*unmute_mic)(                                         /* ���MIC�����ӿ� */
                            VOID
                      );
    BOOL (*set_mic_gain)(                                       /* ����MIC����ӿ� */
                            UINT16 micGain                      /* ����MIC�����棬���Ϊ20 */
                        );
    BOOL (*open_speaker)(                                       /* ���������ӿ� */
                            VOID
                        );
    BOOL (*close_speaker)(                                      /* �ر��������ӿ� */
                            VOID
                         );
    BOOL (*mute_speaker)(                                       /* �����������ӿ� */
                            VOID
                        );
    BOOL (*unmute_speaker)(                                     /* ��������������ӿ� */
                            VOID
                          );
    BOOL (*set_speaker_gain)(                                   /* ���������������� */
                            E_AMOPENAT_SPEAKER_GAIN speakerGain /* ���������������� */
                            );
    E_AMOPENAT_SPEAKER_GAIN (*get_speaker_gain)(                /* ��ȡ������������ӿ� */
                            VOID
                                               );
    BOOL (*set_channel)(                                        /* ������Ƶͨ���ӿ� */
                            E_AMOPENAT_AUDIO_CHANNEL channel    /* ͨ�� */
                       );
    VOID (*set_channel_with_same_mic)(                          /* ���ù���ͬһ��MIC��Ƶͨ���ӿ� */
                        E_AMOPENAT_AUDIO_CHANNEL channel_1,     /* ͨ�� 1 */
                        E_AMOPENAT_AUDIO_CHANNEL channel_2      /* ͨ�� 2 */
                   );
/*+\BUG WM-882\rufei\2013.7.18\����ͨ������*/
    BOOL(*set_hw_channel)(
                          E_AMOPENAT_AUDIO_CHANNEL hfChanne,    /*�ֱ�ͨ��*/
                          E_AMOPENAT_AUDIO_CHANNEL erChanne,    /*����ͨ��*/
                          E_AMOPENAT_AUDIO_CHANNEL ldChanne    /*����ͨ��*/
                         );
/*-\BUG WM-882\rufei\2013.7.18\����ͨ������*/
    E_AMOPENAT_AUDIO_CHANNEL (*get_current_channel)(            
                            VOID
                                                   );
/*+\NewReq WM-711\maliang\2013.3.18\[OpenAt]���ӽӿڴ򿪻�ر���Ƶ�ػ�����*/
/*+\New\lijiaodi\2014.7.30\�޸���Ƶ�ػ����Խӿڣ�����IsSpkLevelAdjust��SpkLevel������
                           ���IsSpkLevelAdjustΪFALSE,spkLevelΪĬ�ϵ�ֵ������ΪSpkLevelָ����ֵ*/
    BOOL  (*audio_loopback)(BOOL  start,                    /*��ʼ��ֹͣ�ػ�����*/
                                        E_AMOPENAT_AUDIO_LOOPBACK_TYPE type,   /*�ػ����Ե�����*/
                                        BOOL IsSpkLevelAdjust,   /*SPK������С�Ƿ��ָ��*/
                                        UINT8 SpkLevel);   /*SPKָ����������СSpkLevelȡֵ��ΧAUD_SPK_MUTE--AUD_SPK_VOL_7*/
/*-\New\lijiaodi\2014.7.30\�޸���Ƶ�ػ����Խӿڣ�����IsSpkLevelAdjust��SpkLevel������
                           ���IsSpkLevelAdjustΪFALSE,spkLevelΪĬ�ϵ�ֵ������ΪSpkLevelָ����ֵ*/
/*-\NewReq WM-711\maliang\2013.3.18\[OpenAt]���ӽӿڴ򿪻�ر���Ƶ�ػ�����*/

    BOOL  (*audio_inbandinfo)(PINBANDINFO_CALLBACK callback); 

    BOOL (*audioRec_start)(OPENAT_REC_MODE_T codec,BOOL loop_mode,AUDIO_REC_CALLBACK recHanlder);
    BOOL (*audioRec_stop)(void);
    
    /****************************** ADC ******************************/
    BOOL (*init_adc)(
                            E_AMOPENAT_ADC_CHANNEL chanle
                    );
    BOOL (*read_adc)(
                            E_AMOPENAT_ADC_CHANNEL chanle,      /* ADCͨ�� */
                            UINT16* adcValue,                   /* ADCֵ������Ϊ��*/   
                            UINT16* voltage                     /* ��ѹֵ������Ϊ��*/
                    );
    /****************************** LCD ******************************/
    /* MONO */                                                  /* �ڰ���*/			
    BOOL (*init_mono_lcd)(                                      /* ��Ļ��ʼ���ӿ� */
                            T_AMOPENAT_MONO_LCD_PARAM*  monoLcdParamP
                    );
    VOID (*send_mono_lcd_command)(                              /* ��������ӿ� */
                            UINT8 cmd                           /* ���� */
                                 );
    VOID (*send_mono_lcd_data)(                                 /* �������ݽӿ� */
                            UINT8 data                          /* ���� */
                              );
    VOID (*update_mono_lcd_screen)(                             /* ������Ļ�ӿ� */
                            T_AMOPENAT_LCD_RECT_T* rect         /* ��Ҫˢ�µ����� */
                                  );
    VOID (*clear_mono_lcd)(                                     /* ������һ������ʵ��LCD RAM����ʾ��������� */
                            UINT16 realHeight,                  /* ʵ��LCD RAM �߶� */
                            UINT16 realWidth                    /* ʵ��LCD RAM ��ȣ�������4�ı��� */
                          );
    /* COLOR */                                                 /* ��ɫ�� */
    BOOL (*init_color_lcd)(                                     /* ��Ļ��ʼ���ӿ� */
                            T_AMOPENAT_COLOR_LCD_PARAM *param   /* ������ʼ������ */
                          );
    VOID (*send_color_lcd_command)(                             /* ��������ӿ� */
                            UINT8 cmd                           /* ���� */
                                  );
    VOID (*send_color_lcd_data)(                                /* �������ݽӿ� */
                            UINT8 data                          /* ���� */
                               );
    VOID (*update_color_lcd_screen)(                            /* ������Ļ�ӿ� */
                            T_AMOPENAT_LCD_RECT_T* rect,        /* ��Ҫˢ�µ����� */
                            UINT16 *pDisplayBuffer              /* ˢ�µĻ����� */
                                   );
    VOID (*set_layer_visible)(
                            BOOL layerVisible[OPENAT_LAYER_COUNT]
                            );
    
    BOOL (*create_layer)(
                            UINT32 layer_id, 
                            E_OPENAT_LCD_LAYER_FORMAT format, 
                            T_AMOPENAT_LCD_RECT_T*  rect
                            );

    VOID (*destory_layer)(
                            UINT32 layer_id
                         );

    UINT8* (*set_active_layer)(
                               UINT32 layer_id
                               );

    
    UINT8* (*set_layer_format)(
                               UINT32 layer_id,
                               E_OPENAT_LCD_LAYER_FORMAT format
                               );

    
    UINT32 (*get_active_layer)(VOID);

    
    VOID (*set_layer_start_pos)(
                                UINT32 layer_id, 
                                UINT32 startX, 
                                UINT32 startY
                                );

    E_OPENAT_LCD_LAYER_INFO* (*get_layer_info)(
                                              UINT32 layer_id
                                              );
											  
    /****************************** CAMERA ******************************/
    BOOL (*camera_init)(                                        /* ����ͷ��ʼ���ӿ� */
                        T_AMOPENAT_CAMERA_PARAM *cameraParam    /* ����ͷ��ʼ������ */
                        );
    
    BOOL (*camera_poweron)(                                     /* ������ͷ */
                        BOOL videoMode                          /* �Ƿ���Ƶģʽ */
                        );
    BOOL (*camera_poweroff)(                                    /* �ر� ����ͷ */
                        void
                        );
    BOOL (*camera_preview_open)(                                /* ��ʼԤ��*/
                        T_AMOPENAT_CAM_PREVIEW_PARAM *previewParam /* Ԥ������ */
                        );
    BOOL (*camera_preview_close)(                               /* �˳�Ԥ�� */
                        void                    
                        );
    BOOL (*camera_capture)(                                     /* ���� */
                        T_AMOPENAT_CAM_CAPTURE_PARAM *captureParam /* ���� */
                        );
    BOOL (*camera_save_photo)(                                  /* ������Ƭ */
                        INT32 iFd                               /* ���������Ƭ�ļ���� */
                        );

    /*+\NEW\Jack.li\2013.2.9\��������ͷ��Ƶ¼�ƽӿ� */
    BOOL (*camera_videorecord_start)(                           /* ��ʼ¼����Ƶ */
                        INT32 iFd                               /* ¼���ļ���� */
                        );
    BOOL (*camera_videorecord_pause)(                           /* ��ͣ¼����Ƶ */
                        void                    
                        );
    BOOL (*camera_videorecord_resume)(                          /* �ָ�¼����Ƶ */
                        void                    
                        );
    BOOL (*camera_videorecord_stop)(                            /* ֹͣ¼����Ƶ */
                        void                    
                        );
    /*-\NEW\Jack.li\2013.2.9\��������ͷ��Ƶ¼�ƽӿ� */
    
    /*-\NEW\Jack.li\2013.1.28\��������ͷ����*/
    
    /*+\NEW\Jack.li\2013.2.10\������Ƶ���Žӿ� */
    BOOL (*video_open)(                                         /* ����Ƶ���� */
                        T_AMOPENAT_VIDEO_PARAM *param           /* ��Ƶ���� */
                        );
    BOOL (*video_close)(                                        /* �ر���Ƶ���� */
                        void
                        );
    BOOL (*video_get_info)(                                     /* ��ȡ��Ƶ��Ϣ */
                        T_AMOPENAT_VIDEO_INFO *pInfo            /* ��Ƶ��Ϣ */
                        );
    BOOL (*video_play)(                                         /* ���� */
                        void
                        );
    BOOL (*video_pause)(                                        /* ��ͣ */
                        void
                        );
    BOOL (*video_resume)(                                       /* �ָ� */
                        void
                        );
    BOOL (*video_stop)(                                         /* ֹͣ */
                        void
                        );
    /*-\NEW\Jack.li\2013.2.10\������Ƶ���Žӿ� */

    /* NULL */
    /****************************** KEYPAD ******************************/
    BOOL (*init_keypad)(                                        /* ���̳�ʼ���ӿ� */
                            T_AMOPENAT_KEYPAD_CONFIG *pConfig   /* �������ò��� */
                       );

    /****************************** TOUCHSCREEN ******************************/
    BOOL (*init_touchScreen)(                                   /* ��������ʼ���ӿ� */
                            PTOUCHSCREEN_MESSAGE pTouchScreenMessage /* ������Ϣ�ص����� */
                            );

    VOID (*touchScreenSleep)(                                   /* ������˯�߽ӿ� */
                            BOOL sleep                          /* �Ƿ�Ҫ˯�� */
                            );
    /******************************** PSAM ***********************************/
    /* ע��:::PSAM���ӿ��ڲ����豸ʱ�ᵼ�µ����߱�����ֱ���豸����Ӧ����2s+��ʱ */
    E_AMOPENAT_PSAM_OPER_RESULT (*open_psam)(                   /* ��psam */
                            E_AMOPENAT_PSAM_ID id               /* Ӳ��SIM���ӿ� */
                                            );
    VOID (*close_psam)(                                         /* �ر�psam */
                            E_AMOPENAT_PSAM_ID id               /* Ӳ��SIM���ӿ� */
                      );
	/*  rw_psam�ӿ�ʹ��˵��
		psam ָ���
		-------------------------
	    ����  |	ֵ
		-------------------------
		CLA     80
		INS  	82
		P1	    00
		P2	    00����Կ�汾��KID��
		Lc	    08
		DATA	��������
		-------------------------

	�����ҪDATA������PSAMָ���Ҫ�ֲ����ͣ�
	  ��һ�� ����DATA֮ǰ������֣�ͬʱ����rxLen=1��stopClock = FALSE
	  	����ֵ����ΪIns �� ~Ins 
	  		���������Ins�����������
	  		���������~Ins�������ڶ���
	  �ڶ��� ���DATA��ʣ������ݴ���1���ֽڣ�����һ���ֽڵ�DATAʣ�����ݣ�ͬʱ����rxLen=1��stopClock = FALSE��
	        ����ֱ�ӽ��������
	     ����ֵ�Ĵ���͵�һ������ֵ����һ��
		   
	  ����������DATA�е�ʣ�����ݣ�ͬʱ����stopClock=TRUE,rxLen������Ҫ����
	*/					  
    E_AMOPENAT_PSAM_OPER_RESULT (*rw_psam)(                     /* �������� */
                            E_AMOPENAT_PSAM_ID id,              /* Ӳ��SIM���ӿ� */
                            CONST UINT8*  txBuf,                /* д���� */
                            UINT16        txLen,                /* д���泤�� */
                            UINT8*        rxBuf,                /* ������ */
                            UINT16        rxLen,                /* �����泤�� */
                            BOOL          stopClock             /* ����ֿ���������ΪFALSE, ����һ�η��ͻ���Ϊ�ֲ����͵����һ������ΪTRUE*/  
                                          );
    E_AMOPENAT_PSAM_OPER_RESULT (*reset_psam)(                  /* ��λPSAM */
                            E_AMOPENAT_PSAM_ID id,              /* Ӳ��SIM���ӿ� */
                            UINT8*      atrBuf,                 /* ATR ���� */
                            UINT16      atrBufLen,              /* ATR ���泤�� */
                            E_AMOPENAT_PSAM_VOLT_CLASS volt     /* ������ѹ */
                                             );
    E_AMOPENAT_PSAM_OPER_RESULT (*setfd_psam)(                  /* ����Fֵ��Dֵ��Ĭ��F=372 D=1 */
                            E_AMOPENAT_PSAM_ID id,              /* Ӳ��SIM���ӿ� */
                            UINT16      f,                      /* Fֵ */
                            UINT8       d                       /* Dֵ */
                                             );

/*+\BUG WM-690\rufei\2013.3.18\AT+SPWMû��ʵ��PWM1��PWM2*/
    /******************************** PWM ***********************************/
    BOOL (*open_pwm)(
                            E_AMOPENAT_PWM_PORT port
                     );
    BOOL (*close_pwm)(
                            E_AMOPENAT_PWM_PORT port
                      );
    BOOL (*set_pwm)(
                            T_AMOPENAT_PWM_CFG *pwmcfg
                    );
/*-\BUG WM-690\rufei\2013.3.18\AT+SPWMû��ʵ��PWM1��PWM2*/

    /****************************** FM ******************************/
	BOOL (*open_fm)(											/* ��FM */
                            T_AMOPENAT_FM_PARAM *fmParam        /* ��ʼ������ */
                   );

	BOOL (*tune_fm)(											/* ����ָ��Ƶ�� */
                            UINT32 frequency                    /* Ƶ��(KHZ) */
                   );

	BOOL (*seek_fm)(											/* ������һ��̨ */
                            BOOL seekDirection					/* TRUE:Ƶ�����ӵķ��� FALSE::Ƶ�ʼ�С�ķ��� */		
                   );

	BOOL (*stopseek_fm)(										/* ֹͣ���� */
                            void
                       );

	BOOL (*setvol_fm)(											/* ������Ч */
                            E_AMOPENAT_FM_VOL_LEVEL volume, 	/* �������� */
                            BOOL bassBoost, 
                            BOOL forceMono
                     );

	BOOL (*getrssi_fm)(											/* ��ȡFM�ź� */
                            UINT32* pRssi
                      );

	BOOL (*close_fm)(											/* �ر�FM */
                            void
                    );


    /*******************************************
    **               AT COMMAND               **
    *******************************************/
    BOOL (*init_at)(                                            /* ����ATͨ·��ʼ���ӿ� */
                            PAT_MESSAGE pAtMessage              /* AT��Ϣ�ص����� */
                   );
    BOOL (*send_at_command)(                                    /* ����AT����ӿ� */
                            UINT8 *pAtCommand,                  /* AT���� */
                            UINT16 nLength                      /* AT����� */
                           );
						   
    BOOL (*set_dte_at_filter)(                                  /*����DTE��CI֮��AT�����ַ����˺���*/
                            PAT_FILTER_MESSAGE dte_data_ind_cb, /*DTE->CI�����ݣ�һ����AT����*/
                            PAT_FILTER_MESSAGE ci_data_ind_cb   /*CI->DTE�����ݣ�һ����AT�����*/
                            );
    BOOL (*send_data_to_dte)(                                   /*�������ݸ�DTE�豸����ʱֻ֧��AT����*/
                            UINT8 channel,                      /*ͨ��*/
                            const UINT8* data,                  /*����*/
                            UINT16  dataLen                     /*���ݳ���*/
                            );
    BOOL (*send_data_to_ci) (                                   /*�������ݸ�CI����ʱֻ֧��AT����*/
                            UINT8 channel,                      /*ͨ��*/
                            const UINT8* data,                  /*����*/
                            UINT16 dataLen                      /*���ݳ���*/
                            );
    BOOL (*set_dte_device) (                                    /*����DTE��Ӧ�������豸��cust_main�����ã�
                                                                  ��ʱ��֧��ʹ�ù���������*/
                            T_AMOPENAT_DTE_DEVICE device
                           );
                            
/*+\NEW WM-733\xc\2013.04.19\�޸ļ��ܿ�����(���openat��ȡ�ӿ�) */
    /*******************************************
    **               ���ܿ�����               **
    *******************************************/
    BOOL (*set_encinfo)(                         /* ������Կ��Ϣ */
                        UINT8 *encInfo,
                        UINT32 len
              );

    BOOL (*get_encinfo)(                         /* ��ȡ��Կ��Ϣ */
                        UINT8 *encInfo,
                        UINT32 len
              );

    UINT8 (*get_encresult)(                         /* ��ȡ����У���� */
                        void
              );
/*+\NEW WM-733\xc\2013.05.06\�޸ļ��ܿ�����5(��ӻ�ȡ�����͵Ľӿ�) */
    UINT8 (*get_cardtype)(                         /* ��ȡ������ 0δ֪  1���ܿ�  2��ͨ��  */
                        void
              );
/*-\NEW WM-733\xc\2013.05.06\�޸ļ��ܿ�����5(��ӻ�ȡ�����͵Ľӿ�) */
              
/*+\NEW WM-733\xc\2013.04.23\�޸ļ��ܿ�����2(��openat�ӿڴ���at������Կ��Ϣ������ź���) */
    BOOL (*set_enc_data_ok)(                         /* mmi��׼������Կ��Ϣ��Ҫ���������֪ͨ */
                        void
              );
/*-\NEW WM-733\xc\2013.04.23\�޸ļ��ܿ�����2(��openat�ӿڴ���at������Կ��Ϣ������ź���) */
/*+\NEW WM-733\xc\2013.04.19\�޸ļ��ܿ�����(���openat��ȡ�ӿ�) */

/*+\NEW\xiongjunqun\2014.04.02\����TTS�Ĵ���*/
/* delete TTS���÷���AT ָ�����ʽ */
/*-\NEW\xiongjunqun\2014.04.02\����TTS�Ĵ���*/
/*+\NEW AMOPENAT-91 \zhangyang\2013.11.19\����USB HID����*/
    void (*uhid_open)(
        void (*handler)(uint8 *, uint32));
    void (*uhid_close)(
        void);
    int32 (*uhid_write)(
        uint8 *data_p, 
        uint32 length);
/*-\NEW AMOPENAT-91 \zhangyang\2013.11.19\����USB HID����*/

/*+\NEW\RUFEI\2014.4.4\�����ⲿ���Ź����ýӿ�*/
    BOOL(*cus_config_ex_watch_dog)(
            E_OPEANT_CUST_EX_WATCH_DOG_MODE mode,
            U_AMOPENAT_EX_WATCH_DOG_CFG *cfg
            );
    BOOL (*cus_reset_ex_watch_dog)(
            void);
    BOOL(*cus_close_ex_watch_dog)(
        E_OPEANT_CUST_EX_WATCH_DOG_MODE mode);
/*-\NEW\RUFEI\2014.4.4\�����ⲿ���Ź����ýӿ�*/

/*+\NEW\RUFEI\2014.8.20\����gps�ӿ�ʵ��*/
    /*******************************************
    **               RDAGPS                      **
    *******************************************/
    BOOL (*rdaGps_open)(
        T_AMOPENAT_RDAGPS_PARAM *cfg);
    BOOL (*rdaGps_close)(
        T_AMOPENAT_RDAGPS_PARAM *cfg);
/*-\NEW\RUFEI\2014.8.20\����gps�ӿ�ʵ��*/
    /*******************************************
    **                 DEBUG                  **
    *******************************************/
    VOID (*print)(                                              /* trace log����ӿ� */
                            CHAR * fmt, ...
                 );
    VOID (*send_event)(
                            UINT32  event
                   );
    VOID (*assert)(                                             /* ���Խӿ� */
                            BOOL condition,                     /* ���� */
                            CHAR *func,                         /* �������� */
                            UINT32 line                         /* ���� */
                  );

    VOID (*enable_watchdog)(BOOL enable);                            /*�򿪿��Ź�*/

/*+\NEW\WZQ\2014.11.7\����SSL RSA����*/
    /*******************************************
      **                 SSL RSA                  **
      *******************************************/

    OPENAT_BIGNUM*(*bn_new)(void);
    
    OPENAT_BN_CTX*(*bn_CTX_new)(void);
    
    OPENAT_BIGNUM*(*bn_bin2bn)(const unsigned char *s,
                        int len,
                        OPENAT_BIGNUM *ret
                        );
    
    int (*bn_bn2bin)(const OPENAT_BIGNUM *a, 
                        unsigned char *to);
    
    int (*bn_mod_exp_mont)(OPENAT_BIGNUM *rr,
                            const OPENAT_BIGNUM *a, 
                            const OPENAT_BIGNUM *p,
                            const OPENAT_BIGNUM *m, 
                            OPENAT_BN_CTX *ctx, 
                            OPENAT_BN_MONT_CTX *in_mont
                            );
    
    int (*bn_mod_exp)(OPENAT_BIGNUM *r,
                        const OPENAT_BIGNUM *a, 
                        const OPENAT_BIGNUM *p, 
                        const OPENAT_BIGNUM *m,
                        OPENAT_BN_CTX *ctx
                        );
    
    void (*bn_free)(OPENAT_BIGNUM *a);
    
    void (*bn_CTX_free)(OPENAT_BN_CTX *ctx);
    
    int (*bn_hex2bn)(OPENAT_BIGNUM **bn, 
                     const char *a
                     );
/*-\NEW\WZQ\2014.11.7\����SSL RSA����*/

/*+\NEW\WZQ\2014.11.7\����18030�ֿ�*/
    /*******************************************
       **                 GB18030                  **
       *******************************************/

    const unsigned char* (*get_GB18030_graph)(unsigned char* gbkCode,
                             unsigned int gbkCodeLen);
/*-\NEW\WZQ\2014.11.7\����18030�ֿ�*/

    int (*get_env_usage)(void );
/*+:\NewReq\brezen\2017.2.22\lua���� */	
    void (*decode)(UINT32* data, INT32 len);
/*-:\NewReq\brezen\2017.2.22\lua���� */	
	VOID (*set_fault_mode)(E_OPENAT_FAULT_MODE mode); /*�豸�쳣ʱ��ѡ���������ǵ���ģʽ*/

  
    /*******************************************
    **                 socket                 **
    *******************************************/
    int (*socket)(int domain, int type, int protocol);
                           
    struct hostent* (*gethostbyname)(const char *name);
                           
    int (*close) (int fd);

    int (*setsockopt) (int socketfd, 
                            int level, 
                            int optname,
                            void *optval_p, 
                            socklen_t optlen);
    int (*getsockopt) (int socketfd, 
                            int level, 
                            int optname,
                            void *optval_p, 
                            socklen_t* optlen);
    int (*bind) (int socketfd, 
                          const struct sockaddr *my_addr, 
                          socklen_t addrlen);
                          
    int (*connect) (int socketfd, const struct sockaddr *addr, socklen_t addrlen);
                             
    int (*listen) (int socketfd, 
                           int backlog);
    int (*accept) (int socketfd, 
                            struct sockaddr *addr, 
                            socklen_t *addrlen);
                            
    int (*recv) (int socketfd, 
                          void *buf, 
                          size_t len,
                          int flags);
    
    int (*recvfrom) (int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen);

    int (*send) (int socketfd,
                          const void *msg,
                          size_t len,
                          int flags);
                          
    int (*sendto) (int socketfd,
                            const void *buf,
                            size_t len,
                            int flags,
                            const struct sockaddr *to_p, 
                            socklen_t tolen);
                     
    int (*select) (int maxfdp1, 
                            fd_set *readset,
                            fd_set *writeset,
                            fd_set *exceptset,
                            struct timeval *timeout);
	int (*socket_errno)(int socketfd);
    VOID (*ping)    (
                            const struct sockaddr *addr_p,
                            int pingCount,
                            int Timeout, 
                            F_AMOPENAT_PING_IND ind
                           );
    BOOL(*network_get_status)  (
                            T_OPENAT_NETWORK_STATUS* status
                            );
    BOOL(*network_set_cb)     (
                            F_OPENAT_NETWORK_IND_CB indCb
                          );
    BOOL(*network_connect)     (
                            T_OPENAT_NETWORK_CONNECT* connectParam
                          );
                          
    BOOL(*network_disconnect)  (
                            BOOL flymode
                          );   

    INT32 (*imgs_decode_jpeg)(
                                CONST char * filename,
                                T_AMOPENAT_IMAGE_INFO *imageinfo
                            );
    
    INT32 (*imgs_free_jpeg_decodedata)(
                                    UINT16* buffer
                            );      

    BOOL (*imgs_decode_png)(
                            UINT8* buffer,
                            UINT32 size,
                            UINT16 x, 
                            UINT16 y, 
                            UINT16 left, 
                            UINT16 top, 
                            UINT16 right, 
                            UINT16 bottom
                          );
}T_AMOPENAT_INTERFACE_VTBL;

/*+\BUG WM-656\lifei\2013.03.07\[OpenAT] �޸�cust����������*/
#define OPENAT_CUST_VTBL_DEFUALT_MAGIC 0x87654321
/*-\BUG WM-656\lifei\2013.03.07\[OpenAT] �޸�cust����������*/
#define OPENAT_CUST_APP_ROM_MAX_LEN       0x50000

typedef enum E_AMOPENAT_CUST_INIT_RESULT_TAG
{
    OPENAT_CUST_INIT_RES_OK,        /* �ͻ������ʼ���ɹ������Ե���cust_main���� */
    OPENAT_CUST_INIT_RES_ERROR,     /* �ͻ������ʼ��ʧ�ܣ��������cust_main���� */
    OPENAT_CUST_INIT_RES_MAX
}E_AMOPENAT_CUST_INIT_RESUL;
    
typedef struct T_AMOPENAT_CUST_VTBL_TAG
{
    const UINT32 magic;
    const UINT32 imgROLMA;
    const UINT32 imgROVMA;
    const UINT32 imgROLength;
    const UINT32 imgRWLMA;
    const UINT32 imgRWVMA;
    const UINT32 imgRWLength;
    const UINT32 imgZIBase;
    const UINT32 imgZILength;

    UINT8 (*cust_init)(                                 /* �ͻ������ʼ���ӿ�*/
                            T_AMOPENAT_INTERFACE_VTBL *pVtable/* OPENAT����ƽ̨�ӿڱ� */
                      );
    VOID (*cust_main)(                                  /* �ͻ����������� */
                            VOID
                     );
}T_AMOPENAT_CUST_VTBL;

#endif /* AM_OPENAT_H */

