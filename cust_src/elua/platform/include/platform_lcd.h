/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    platform_lcd.h
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/3/26
 *
 * Description:
 *          platform lcd ½Ó¿Ú
 **************************************************************************/

#ifndef _PLATFORM_LCD_H_
#define _PLATFORM_LCD_H_

#if defined LUA_DISP_LIB

#include "platform_disp.h"

void platform_lcd_init(const PlatformDispInitParam *pParam);

#ifdef AM_LAYER_SUPPORT
BOOL  platform_active_layer_is_RGBA(void);
u8* platform_set_active_layer(u32 layer_id);
void platform_destory_layer(  u32 layer_id);
int  platform_get_active_layer_format(void);
u32 platform_get_active_layer(void);
void platform_set_layer_start_pos(u32 layer_id, u32 startX,  u32 startY);
void platform_set_layer_visible(BOOL layer_visible[LAYER_ID_MAX] );
u8* platform_set_layer_format(u32 layer_id, int format);
BOOL platform_create_layer(  int layer_id,  
                                    int format ,  
                                    int left   ,  
                                    int top    ,  
                                    int right  ,  
                                    int bottom);

void platform_lcd_update(PlatformRect *pRect, PlatformRect *pRoiRect);
void  platform_get_active_layer_size(u16* width, u16*height);

#else
void platform_lcd_update(PlatformRect *pRect, u8 *buffer);
#endif
#endif

#endif//_PLATFORM_LCD_H_
