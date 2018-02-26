/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    platform_conf.h
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2012/10/8
 *
 * Description:
 * 
 **************************************************************************/

#ifndef __PLATFORM_CONF_H__
#define __PLATFORM_CONF_H__

#include "auxmods.h"

// *****************************************************************************
// ����ƽ̨Ҫ�����Ĺ���
#define BUILD_LUA_INT_HANDLERS
#define BUILD_C_INT_HANDLERS

/*+\NEW\liweiqiang\2013.12.6\���ڳ��y500K��dl�ڴ��,��ôαlibc��malloc��dlmalloc���� */
#if DLMALLOC_DEFAULT_GRANULARITY > 500*1024
#define USE_DLMALLOC_ALLOCATOR
#else
#define USE_PLATFORM_ALLOCATOR
#endif
/*-\NEW\liweiqiang\2013.12.6\���ڳ��y500K��dl�ڴ��,��ôαlibc��malloc��dlmalloc���� */

// *****************************************************************************
// Configuration data

// Virtual timers (0 if not used)
#define VTMR_NUM_TIMERS       0

// Number of resources (0 if not available/not implemented)
#define NUM_PIO               2 // port 0:gpio; port 1:gpio ex;
#define NUM_SPI               0
#define NUM_UART              4 //ʵ��ֻ?2�������� id0-���Y�ɰ汾Ϊuart2 id1-uart1 id2-uart2 id3-hostuart
#define NUM_TIMER             2
#define NUM_PWM               0
#define NUM_ADC               8
#define NUM_CAN               0
#define NUM_I2C               3

#define PIO_PIN_EX            5 /*gpio ex 0~6,7,8*/
#define PIO_PIN_ARRAY         {32 /* gpio_num 32 */, PIO_PIN_EX}

//?���at����ͨ��
#define PLATFORM_UART_ID_ATC              0x7f

//host uart debugͨ��
#define PLATFORM_PORT_ID_DEBUG            0x80

//����??ͨ��
#define CON_UART_ID           (platform_get_console_port())
#define CON_UART_SPEED        115200
#define CON_TIMER_ID          0

// PIO prefix ('0' for P0, P1, ... or 'A' for PA, PB, ...)
#define PIO_PREFIX            '0'

/*+\NEW\liweiqiang\2013.7.16\����iconv�ַ�����ת���� */
#ifdef LUA_ICONV_LIB
#define ICONV_LINE   _ROM( AUXLIB_ICONV, luaopen_iconv, iconv_map )
#else
#define ICONV_LINE   
#endif
/*-\NEW\liweiqiang\2013.7.16\����iconv�ַ�����ת���� */

/*+\NEW\liweiqiang\2014.2.9\����zlib�� */
#ifdef LUA_ZLIB_LIB
#define ZLIB_LINE   _ROM( AUXLIB_ZLIB, luaopen_zlib, zlib_map )
#else
#define ZLIB_LINE
#endif
/*-\NEW\liweiqiang\2014.2.9\����zlib�� */

/*+\NEW\liweiqiang\2014.1.17\AM002_LUA��֧����ʾ�ӿ� */
#ifdef LUA_DISP_LIB
#define DISP_LIB_LINE   _ROM( AUXLIB_DISP, luaopen_disp, disp_map )
#else
#define DISP_LIB_LINE
#endif
/*-\NEW\liweiqiang\2014.1.17\AM002_LUA��֧����ʾ�ӿ� */

/*+\NEW\liulean\2015.6.15\���ӻ�ȡĬ��APN�Ŀ� */

#ifdef LUA_APN_LIB
#define APN_LINE   _ROM( AUXLIB_APN, luaopen_apn, apn_map )
#else
#define APN_LINE
#endif
/*-\NEW\liulean\2015.6.15\���ӻ�ȡĬ��APN�Ŀ� */
#ifndef AM_OPENAT_JSON_NOT_SUPPORT
#define JSON_LIB_LINE   _ROM( AUXLIB_JSON, luaopen_cjson, json_map )
#else
#define JSON_LIB_LINE 
#endif


#define LUA_PLATFORM_LIBS_ROM \
    _ROM( AUXLIB_BIT, luaopen_bit, bit_map ) \
    _ROM( AUXLIB_BITARRAY, luaopen_bitarray, bitarray_map ) \
    _ROM( AUXLIB_PACK, luaopen_pack, pack_map ) \
    _ROM( AUXLIB_PIO, luaopen_pio, pio_map ) \
    _ROM( AUXLIB_UART, luaopen_uart, uart_map ) \
    _ROM( AUXLIB_SPI, luaopen_spi, spi_map ) \
    _ROM( AUXLIB_I2C, luaopen_i2c, i2c_map ) \
    _ROM( AUXLIB_RTOS, luaopen_rtos, rtos_map ) \
    DISP_LIB_LINE \
    _ROM( AUXLIB_PMD, luaopen_pmd, pmd_map ) \
    _ROM( AUXLIB_ADC, luaopen_adc, adc_map ) \
    ICONV_LINE \
    _ROM( AUXLIB_AUDIOCORE, luaopen_audiocore, audiocore_map ) \
    ZLIB_LINE \
    JSON_LIB_LINE \
    _ROM( AUXLIB_WATCHDOG, luaopen_watchdog, watchdog_map ) \
    _ROM( AUXLIB_CPU, luaopen_cpu, cpu_map) \
    APN_LINE \
    _ROM( AUXLIB_GPSCORE, luaopen_gpscore, gpscore_map) \
    _ROM( AUXLIB_CRYPTO, luaopen_crypto, crypto_map ) \
    _ROM( AUXLIB_IMAGE, luaopen_image, image_map ) \
	_ROM( AUXLIB_IMAGE, luaopen_mpu9250, mpu9250_map ) \
    _ROM( AUXLIB_QRENCODE, luaopen_qr_encode, qr_encode_map ) \




    // Interrupt queue size
#define PLATFORM_INT_QUEUE_LOG_SIZE 5

#define CPU_FREQUENCY         ( 26 * 1000 * 1000 )

// Interrupt list
#define INT_GPIO_POSEDGE      ELUA_INT_FIRST_ID
#define INT_GPIO_NEGEDGE      ( ELUA_INT_FIRST_ID + 1 )
#define INT_ELUA_LAST         INT_GPIO_NEGEDGE
    
#define PLATFORM_CPU_CONSTANTS \
     _C( INT_GPIO_POSEDGE ),\
     _C( INT_GPIO_NEGEDGE )

#endif //__PLATFORM_CONF_H__
