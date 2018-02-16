/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    common_disp.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/3/26
 *
 * Description:
 *          display 接口实现
 **************************************************************************/

 #if defined LUA_DISP_LIB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "type.h"
#include "platform.h"
#include "platform_disp.h"
#include "platform_image.h"
#include "platform_lcd.h"
#include "LzmaLib.h"
/*+\NEW\zhuth\2014.2.16\支持png图片的显示*/
#ifdef AM_LPNG_SUPPORT
#include "png.h"
#endif
/*-\NEW\zhuth\2014.2.16\支持png图片的显示*/

#define MAX_FONTS       10


PlatformLcdBus lcd_bus;


/*+\NEW\2013.4.10\增加黑白屏显示支持 */
// 1bit位图数据组织方式同win32 1bit bmp
typedef struct DispBitmapTag
{
    u16 width;
    u16 height;
    u8 bpp;
    const u8 *data;
}DispBitmap;
/*-\NEW\2013.4.10\增加黑白屏显示支持 */

typedef struct FontInfoTag
{
    u8        width;
    u8        height;
    u8        size;
    u16       start;
    u16       end;
    const u8       *data;
}FontInfo;

static const u8 blankChar[16*16/8] = {0};

// font 
// 宋体16 ascii 0x20~0x7e
static const u8 sansFont16Data[]=
{
#if defined(BIG_FONT_SUPPORT)
#include "font24.dat"
#else
#include "font.dat"
#endif
};

static const FontInfo sansFont16 = 
{
#if defined(BIG_FONT_SUPPORT)
    12,
    24,
    (12+7)/8*24,
#else
    8,
    16,
    8*16/8,
#endif
    0x20,
    0x7E,
    sansFont16Data,
};

// 汉字宋体16
#if defined(FONT_HZ_COMPRESS)
static const u8 sansHzFont16DataZip[] =
{
#include "fonthz.zip.dat"
};

static const u8 *sansHzFont16Data = NULL;
#else
static const u8 sansHzFont16Data[] =
{
#if defined(BIG_FONT_SUPPORT)
#include "fonthz24dat"
#else
#include "fonthz.dat"
#endif
};

/*+\NEW\liweiqiang\2013.12.18\增加中文标点符号的显示支持 */
static const u8 sansHzFont16ExtData[] = 
{
    #include "fonthzext.dat"
};

/*按内码由小到大排列*/
static const u16 sansHzFont16ExtOffset[] =
{
//"、。—…‘’“”〔〕〈〉《》「」『』【】！（），－．：；？嗯"
    0xA1A2,0xA1A3,0xA1AA,0xA1AD,0xA1AE,0xA1AF,0xA1B0,0xA1B1,
    0xA1B2,0xA1B3,0xA1B4,0xA1B5,0xA1B6,0xA1B7,0xA1B8,0xA1B9,
    0xA1BA,0xA1BB,0xA1BE,0xA1BF,0xA3A1,0xA3A8,0xA3A9,0xA3AC,
    0xA3AD,0xA3AE,0xA3BA,0xA3BB,0xA3BF,0xE0C5
};
/*-\NEW\liweiqiang\2013.12.18\增加中文标点符号的显示支持 */
#endif

/*+\NEW\shenyuanyuan\2017.12.5\增加西班牙特殊字符的显示支持 */
#if defined(FONT_ESPANA_SUPPORT)
static const u8 EsFont16Data[] = 
{
    #include "frontes.dat"
};

/*按内码由小到大排列*/
static const u16 EsFont16Offset[] =
{
//"!?náéíóúü"
    0xA1,0xBF,0xF1,0xA8A2,0xA8A6,0xA8AA,0xA8AE,0xA8B2,0xA8B9
};

static FontInfo sansHzFontEs =
{
    8,
    16,
    (8+7)/8*16,
    0,
    0,
    NULL
};
#endif
/*-\NEW\shenyuanyuan\2017.12.5\增加西班牙特殊字符的显示支持 */

static FontInfo sansHzFont16 =
{
#if defined(BIG_FONT_SUPPORT)
    24,
    24,
    (24+7)/8*24,
#else
    16,
    16,
    (16+7)/8*16,
#endif
    0,
    0,
    NULL
};

static FontInfo dispFonts[MAX_FONTS];
static u8 curr_font_id = 0;
static FontInfo *dispHzFont;

// 背景色与显示颜色
 int disp_bkcolor = COLOR_WHITE;
 int disp_color = COLOR_BLACK;
/*+\new\liweiqiang\2014.10.21\增加不同黑白屏填充色处理 */
 int lcd_hwfillcolor = COLOR_BLACK;// lcd物理填充色设定,目前只支持黑白屏,存在黑底白字的屏也存在白底黑字的屏
/*-\new\liweiqiang\2014.10.21\增加不同黑白屏填充色处理 */

// lcd 设备参数
 u16 lcd_height;
 u16 lcd_width;
static u8 lcd_bpp;

// 显示缓冲区
 u8 *framebuffer = NULL;

#define RGB16TORGB24(color16) (((color16>>8)<<16) | (((color16&0x07e0)>>3)<<8) | ((color16&0x1f)<<3))

#ifdef AM_LAYER_SUPPORT
BOOL create_layer(  int layer_id,  
                        int format ,  
                        int left   ,  
                        int top    ,  
                        int right  ,  
                        int bottom)
{
    
    return platform_create_layer(layer_id, format, left, top, right, bottom);
}



void destory_layer(  int layer_id)
{
    platform_destory_layer(layer_id);
}


void set_active_layer(int layer_id)
{
    framebuffer = ((u32)platform_set_active_layer(layer_id) | 0xa0000000);
}

int get_active_layer(void)
{
    return platform_get_active_layer();
}

void  get_active_layer_size(u16* width, u16*height)
{
    platform_get_active_layer_size(width, height);
}

#if 0
E_OPENAT_LCD_LAYER_INFO* get_layer_info(int layer_id)
{
    return platform_get_layer_info(layer_id);
}
#endif

void set_layer_start_pos(int layer_id, u32 startX,  u32 startY)
{
    return platform_set_layer_start_pos(layer_id, startX, startY);
}

void set_layer_visible(BOOL layer_visible[LAYER_ID_MAX] )
{
   platform_set_layer_visible(layer_visible);
}

void set_layer_format(int layer_id, int format)
{
   u8* buffer = ((u32)platform_set_layer_format(layer_id, format) | 0xa0000000);
   
   if(get_active_layer() == layer_id)
   {
       framebuffer = buffer;
   }
}

void layer_move_image(int layer_id, BOOL move_left)
{
    u16 width;
    u16 height;
    int i;
    int pos = 0;
    u32 tick = platform_rtos_tick();
    u8* buffer = framebuffer;
    platform_get_active_layer_size(&width, &height);
    
    if(move_left)
    {
        pos = 0;
        
        for(i = 0 ; i < height; i++)
        {
            memmove(&buffer[pos], & buffer[pos + (lcd_width  << 2)], lcd_width << 3);
            pos += (width << 2) ;
        }
    }
    else
    {
        pos = (lcd_width<<2);
        int src = 0;
        for(i = 0 ; i < height; i++)
        {
            memmove(&buffer[pos], &buffer[src], lcd_width << 3);
            pos += (width  << 2);
            src += (width  << 2);
        }
    }

    platform_rtos_send_event(0xae1a9e00|move_left);
    platform_rtos_send_event(platform_rtos_tick()- tick);
}

#endif



static void fontInit(void)
{
    static BOOL inited = FALSE;

    if(inited)
        return;

    inited = TRUE;

#if defined(LUA_DISP_SUPPORT_HZ)
#if defined(FONT_HZ_COMPRESS)
    LzmaDecodeBufToBuf(sansHzFont16DataZip, sizeof(sansHzFont16DataZip), &sansHzFont16Data);
#endif

    sansHzFont16.data = sansHzFont16Data;
#else
    sansHzFont16.data = NULL;
#endif

/*+\NEW\shenyuanyuan\2017.12.5\增加西班牙特殊字符的显示支持 */
#if defined(FONT_ESPANA_SUPPORT)
    sansHzFontEs.data = EsFont16Data;
#endif
/*-\NEW\shenyuanyuan\2017.12.5\增加西班牙特殊字符的显示支持 */
    
    memset(dispFonts, 0, sizeof(dispFonts));

    dispFonts[0] = sansFont16;
    dispHzFont = &sansHzFont16;
}


#ifdef AIR202_LUA_UI_SPC
void platform_spc_disp_init(PlatformDispInitParam *pParam)
{
    // 只支持16位色屏幕或者黑白屏
    ASSERT(pParam->bpp == 16 || pParam->bpp == 1);

    lcd_bus = pParam->bus;

    lcd_width = pParam->width;
    lcd_height = pParam->height;

    lcd_bpp = pParam->bpp;

/*+\NEW\2013.4.10\增加黑白屏显示支持 */
    pParam->framebuffer = NULL;
/*-\NEW\2013.4.10\增加黑白屏显示支持 */

    // 初始化lcd设备
    platform_spc_lcd_init(pParam);
}
#else
void platform_disp_init(PlatformDispInitParam *pParam)
{
    // 只支持16位色屏幕或者黑白屏
    ASSERT(pParam->bpp == 16 || pParam->bpp == 24 || pParam->bpp == 1);

    lcd_bus = pParam->bus;
    
    lcd_width = pParam->width;
    lcd_height = pParam->height;

    lcd_bpp = pParam->bpp;

#ifndef AM_LAYER_SUPPORT
    // 分配显示缓冲区
    framebuffer = (u8*)((u32)malloc(lcd_width*lcd_height*lcd_bpp/8) | 0xa0000000);
#endif

/*+\NEW\2013.4.10\增加黑白屏显示支持 */
    pParam->framebuffer = framebuffer;
/*-\NEW\2013.4.10\增加黑白屏显示支持 */

/*+\bug0\zhy\2014.10.14\黑白屏默认为 黑底白屏*/
    if(lcd_bpp == 1)
    {
        disp_bkcolor = COLOR_WHITE;
        disp_color = COLOR_BLACK;

/*+\new\liweiqiang\2014.10.21\增加不同黑白屏填充色处理 */
        if(pParam->hwfillcolor != -1){
            lcd_hwfillcolor = pParam->hwfillcolor;
        }
/*-\new\liweiqiang\2014.10.21\增加不同黑白屏填充色处理 */
    }
/*-\bug0\zhy\2014.10.14\黑白屏默认为 黑底白屏*/

    fontInit();

    // 初始化lcd设备
    platform_lcd_init(pParam);
}
#endif


void platform_get_lcd_info(u16 *pWidth, u16 *pHeight, u8 *pBpp)
{
    *pWidth = lcd_width;
    *pHeight = lcd_height;
    *pBpp = lcd_bpp;   
}

void platform_disp_close(void)
{
    if(framebuffer != NULL)
    {
        free(framebuffer);
    }
}

#ifdef AM_LAYER_SUPPORT
void platform_disp_clear(int left, int top, int right, int bottom)
{
    u16 width;
    u32 tick = platform_rtos_tick();

    platform_get_active_layer_size(&width,NULL);
 
/*+\NEW\2013.4.10\增加黑白屏显示支持 */
    if(lcd_bpp == 1)
    {
/*+\bug0\zhy\2014.10.14\黑白屏设置背景色修改*/
/*+\new\liweiqiang\2014.10.21\增加不同黑白屏填充色处理 */
        if(disp_bkcolor^lcd_hwfillcolor) // 与填充色不一样就不填充
/*-\new\liweiqiang\2014.10.21\增加不同黑白屏填充色处理 */
            memset(framebuffer, 0x00, lcd_width*lcd_height*lcd_bpp/8);
        else // 与填充色一致就填充
            memset(framebuffer, 0xFF, lcd_width*lcd_height*lcd_bpp/8);
/*-\bug0\zhy\2014.10.14\黑白屏设置背景色修改*/
    }
    else 
/*-\NEW\2013.4.10\增加黑白屏显示支持 */
    {
        u16 row,col;
        u32 temp;
        
        if(platform_active_layer_is_RGBA())
        {
            u32 *pPixel32 = (u32*)framebuffer;
            u8 bgra[4];
            u32* rgb8888 = (u32*)bgra;
            
            if(disp_bkcolor == 0)
            {
                memset(framebuffer, 0, (right - left + 1)* (bottom - top + 1) * 4);
            }
            else
            {           
                bgra[2] = ((disp_bkcolor >> 11) << 3);
                bgra[1] = (((disp_bkcolor &0x7e0) >> 5) << 2);
                bgra[0] = (((disp_bkcolor &0x1f)) << 3);
                bgra[3] = 255;

                temp = top * width;

                for(row = top; row <= bottom; row++)
                {
                    for(col = left; col <= right; col++)
                    {
                        pPixel32[temp + col] = *rgb8888;
                    }
                    
                    temp += width;
                }
            }
        }
        else
        {
            u16* pPixel16 = (u16*)framebuffer;

            temp = top * width;

            for(row = top; row <= bottom; row++)
            {
                for(col = left; col <= right; col++)
                {
                    pPixel16[temp + col] = disp_bkcolor;
                }
                
                temp += width;
            }
        }
    }
    platform_rtos_send_event(platform_rtos_tick() - tick);
}
#else
void platform_disp_clear(void)
{
/*+\NEW\2013.4.10\增加黑白屏显示支持 */
    if(lcd_bpp == 1)
    {
/*+\bug0\zhy\2014.10.14\黑白屏设置背景色修改*/
/*+\new\liweiqiang\2014.10.21\增加不同黑白屏填充色处理 */
        if(disp_bkcolor^lcd_hwfillcolor) // 与填充色不一样就不填充
/*-\new\liweiqiang\2014.10.21\增加不同黑白屏填充色处理 */
            memset(framebuffer, 0x00, lcd_width*lcd_height*lcd_bpp/8);
        else // 与填充色一致就填充
            memset(framebuffer, 0xFF, lcd_width*lcd_height*lcd_bpp/8);
/*-\bug0\zhy\2014.10.14\黑白屏设置背景色修改*/
    }
    else 
/*-\NEW\2013.4.10\增加黑白屏显示支持 */
    {
        u16 *pPixel16;
        u16 row,col;
        
        pPixel16 = (u16*)framebuffer;
        
        for(col = 0; col < lcd_width; col++)
        {
            for(row = 0; row < lcd_height; row++)
            {
                pPixel16[row*lcd_width + col] = disp_bkcolor;
            }
        }
    }
}
#endif

#ifdef AM_LAYER_SUPPORT
void platform_disp_update(int left, int top, int right, int bottom, int offsetX, int offsetY)
{
    PlatformRect rect;
    PlatformRect roiRect;
    
    rect.ltx = offsetX;
    rect.lty = offsetY;
    rect.rbx = right - left + offsetX;
    rect.rby = bottom - top + offsetY;

    roiRect.ltx = left;
    roiRect.lty = top;
    roiRect.rbx = right;
    roiRect.rby = bottom;
    
    platform_lcd_update(&rect, &roiRect);
}
#else
void platform_disp_update(void)
{
    PlatformRect rect;
    
    rect.ltx = 0;
    rect.lty = 0;
    rect.rbx = lcd_width-1;
    rect.rby = lcd_height-1;
    
    platform_lcd_update(&rect, framebuffer);
}
#endif

/*+\NEW\2013.4.10\增加黑白屏显示支持 */
static void disp_bitmap_bpp1(const DispBitmap *pBitmap, u16 startX, u16 startY)
{
    u16 bx,by,x,y,page,bwbytes;
    u16 endX, endY;

    if(pBitmap->bpp != 1)
    {
        printf("[disp_bitmap_bpp1]: not support bpp %d", pBitmap->bpp);
        return;
    }
    
    endX = MIN(lcd_width,startX + pBitmap->width);
    endY = MIN(lcd_height,startY + pBitmap->height);

    bwbytes = (pBitmap->width+7)/8;

    for(x = startX,bx = 0; x < endX; x++,bx++)
    {
        for(y = startY,by = 0; y < endY; y++,by++)
        {
            page = y/8;
/*+\bug0\zhy\2014.10.14\黑白屏设置前景色修改*/
            if((disp_color^lcd_hwfillcolor) == 0) /*\new\liweiqiang\2014.10.21\增加不同黑白屏填充色处理 */
            {
                        if(pBitmap->data[bwbytes*by+bx/8]&(0x80>>(bx%8)))
                        {
                            framebuffer[page*lcd_width+x] |= 1<<(y%8);
                        }
                        else
                        {
                            //framebuffer[page*lcd_width+x] &= ~(1<<(y%8));
                        }
            }
            else
            {
                        if(pBitmap->data[bwbytes*by+bx/8]&(0x80>>(bx%8)))
                        {
                             framebuffer[page*lcd_width+x] &= ~(1<<(y%8));
                        }
                        else
                        {                  
                            //framebuffer[page*lcd_width+x] |= 1<<(y%8);
                        }
            }
/*-\bug0\zhy\2014.10.14\黑白屏设置前景色修改*/
        }
    }
}

#ifdef AM_LAYER_SUPPORT
static void disp_1bitbmp_bpp16(const DispBitmap *pBitmap, u16 startX, u16 startY)
{
    u16 bx,by,x,y,bwbytes;
    u16 endX, endY;
    u16 layer_width;
    
    ASSERT(pBitmap->bpp == 1);

    platform_get_active_layer_size(&layer_width, NULL) ;

    endX = MIN(lcd_width, startX + pBitmap->width);
    endY = MIN(lcd_height,startY + pBitmap->height);
    
    bwbytes = (pBitmap->width+7)/8;

    if(platform_active_layer_is_RGBA()){
        u32 *buffer32 = (u32 *)framebuffer;
        u32 temp;
        temp = disp_color;
        temp = RGB16TORGB24(temp) | 0xff000000;
        
        for(x = startX,bx = 0; x < endX; x++,bx++)
        {
            for(y = startY,by = 0; y < endY; y++,by++)
            {            
                if(pBitmap->data[bwbytes*by+bx/8]&(0x80>>(bx%8)))
                {
                    //填充颜色
                    buffer32[y*layer_width + x] = temp;
                }
            }
        }
    } else {
        u16 *buffer16 = (u16 *)framebuffer;
        u16 temp = disp_color;
        for(x = startX,bx = 0; x < endX; x++,bx++)
        {
            for(y = startY,by = 0; y < endY; y++,by++)
            {            
                if(pBitmap->data[bwbytes*by+bx/8]&(0x80>>(bx%8)))
                {
                    //填充颜色
                    buffer16[y*layer_width + x] = temp;
                }
            }
        }
    }
}

#else
static void disp_1bitbmp_bpp16(const DispBitmap *pBitmap, u16 startX, u16 startY)
{
    u16 bx,by,x,y,bwbytes;
    u16 endX, endY;
    u16 *buffer16 = (u16*)framebuffer;
    u16 temp;
    
    ASSERT(pBitmap->bpp == 1);

    if(lcd_bus != PLATFORM_LCD_BUS_SPI)
        temp = disp_color;
    else
        temp = (disp_color >> 8 ) | ((disp_color & 0xff) << 8);

    endX = MIN(lcd_width,startX + pBitmap->width);
    endY = MIN(lcd_height,startY + pBitmap->height);
    
    bwbytes = (pBitmap->width+7)/8;
    
    for(x = startX,bx = 0; x < endX; x++,bx++)
    {
        for(y = startY,by = 0; y < endY; y++,by++)
        {            
            if(pBitmap->data[bwbytes*by+bx/8]&(0x80>>(bx%8)))
            {
                //填充颜色
                buffer16[y*lcd_width + x] = temp;
            }
            else
            {
                //填充背景色
                // 直接叠加显示,暂不支持背景色设置
                //buffer16[y*lcd_width + x] = disp_bkcolor;
            }
        }
    }
}
#endif
/*-\NEW\2013.4.10\增加黑白屏显示支持 */

static void getFontBitmap(DispBitmap *pBitmap, u16 charcode)
{
    const FontInfo *pInfo = &dispFonts[curr_font_id];

    pBitmap->bpp = 1;
    pBitmap->width = pInfo->width;
    pBitmap->height = pInfo->height;

    if(pInfo->data)
    {
        if(charcode >= pInfo->start && charcode <= pInfo->end)
        {
            u32 index = charcode - pInfo->start;

            pBitmap->data = pInfo->data + index*pInfo->size;            
        }
        else
        {
            pBitmap->data = blankChar;
        }
    }
    else
    {
        pBitmap->data = blankChar;
    }
}

static void getHzBitmap(DispBitmap *pBitmap, u16 charcode)
{
    const FontInfo *pInfo = dispHzFont;

    pBitmap->bpp = 1;
    pBitmap->width = pInfo->width;
    pBitmap->height = pInfo->height;

    if(pInfo->data)
    {
        u8 byte1, byte2;
        u32 index;

        byte1 = charcode>>8;
        byte2 = charcode&0x00ff;

        if(byte1 >= 0xB0 && byte1 <= 0xF7 &&
            byte2 >= 0xA1 && byte2 <= 0xFE)
        {
            index = (byte1 - 0xB0)*(0xFE - 0xA1 + 1) + byte2 - 0xA1;
            pBitmap->data = pInfo->data + index*pInfo->size;
        }
        else
        {
            pBitmap->data = blankChar;

        /*+\NEW\liweiqiang\2013.12.18\增加中文标点符号的显示支持 */
            for(index = 0; index < sizeof(sansHzFont16ExtOffset)/sizeof(u16); index++)
            {
                if(charcode < sansHzFont16ExtOffset[index])
                {
                    break;
                }

                if(charcode == sansHzFont16ExtOffset[index])
                {
                    pBitmap->data = sansHzFont16ExtData + index*pInfo->size;
                    break;
                }
            }
        /*-\NEW\liweiqiang\2013.12.18\增加中文标点符号的显示支持 */
        }
    }
    else
    {
        pBitmap->data = blankChar;
    }
}

/*+\NEW\shenyuanyuan\2017.12.5\增加西班牙特殊字符的显示支持 */
#if defined(FONT_ESPANA_SUPPORT)
static void getEsFontBitmap(DispBitmap *pBitmap, u16 charcode)
{
    const FontInfo *pInfo = &sansHzFontEs;

    pBitmap->bpp = 1;
    pBitmap->width = pInfo->width;
    pBitmap->height = pInfo->height;
    u32 index;

    if(pInfo->data)
    {
        pBitmap->data = blankChar;
        
        for(index = 0; index < sizeof(EsFont16Offset)/sizeof(u16); index++)
        {
            if(charcode < EsFont16Offset[index])
            {
                break;
            }

            if(charcode == EsFont16Offset[index])
            {
                pBitmap->data = EsFont16Data + index*pInfo->size;
                break;
            }
        }
    }
    else
    {
        pBitmap->data = blankChar;
    }
}
#endif
/*-\NEW\shenyuanyuan\2017.12.5\增加西班牙特殊字符的显示支持 */

static void getCharBitmap(DispBitmap *pBitmap, u16 charcode)
{
    if(charcode >= 0xA0A0)
    {
       /*+\NEW\shenyuanyuan\2017.12.5\增加西班牙特殊字符的显示支持 */
       #if defined(FONT_ESPANA_SUPPORT)
        if(charcode >= 0xa8a1 && charcode <= 0xa8bf)    
	    {
            getEsFontBitmap(pBitmap, charcode);
        }
        else
       #endif
       /*-\NEW\shenyuanyuan\2017.12.5\增加西班牙特殊字符的显示支持 */
        {
            getHzBitmap(pBitmap, charcode);
        }
    }
    else
    {
        /*+\NEW\shenyuanyuan\2017.12.5\增加西班牙特殊字符的显示支持 */
        #if defined(FONT_ESPANA_SUPPORT)
        if(charcode > 0x7e)
        {
            getEsFontBitmap(pBitmap, charcode); 
        }
        else
        #endif
        /*-\NEW\shenyuanyuan\2017.12.5\增加西班牙特殊字符的显示支持 */
        {
             getFontBitmap(pBitmap, charcode); 
        }
    }
}

#if 0 // 清除矩形区接口
static void disp_clear_rect(int ltx, int lty, int rbx, int rby)
{
    u16 *buffer16 = (u16*)framebuffer;
    int x_pos, y_pos;

    ltx = ltx >= lcd_width ? ltx : lcd_width;
    lty = lty >= lcd_height ? lty : lcd_height;
    rbx = rbx >= lcd_width ? rbx : lcd_width;
    rby = rby >= lcd_height ? rby : lcd_height;

    for(x_pos = ltx; x_pos < rbx; x_pos++)
        for(y_pos = lty; y_pos < rby; y_pos++)
            buffer16[y_pos*lcd_width + x_pos] = disp_bkcolor;
}
#endif

void platform_disp_puttext(const char *string, u16 x, u16 y)
{
    int len = strlen(string);
    const u8 *pText = (const u8 *)string;
    int i;
    DispBitmap bitmap;
    u16 charcode;

    if(x >= lcd_width)
        x = 0;

    if(y >= lcd_height)
        y = 0;


    for(i = 0; i < len; )
    {
        if(pText[i] == '\r' || pText[i] == '\n'){
            i++;
            goto next_line;
        }

        
        if(pText[i]&0x80)
        {
            if(pText[i+1]&0x80)
            {
                // gb2312 chinese char
                charcode = pText[i]<<8 | pText[i+1];
                i += 2;
            }
            /*+\NEW\shenyuanyuan\2017.12.5\增加西班牙特殊字符的显示支持 */
            #if defined(FONT_ESPANA_SUPPORT)
            else if(pText[i+1] == 0x01)
            {
                charcode = pText[i];
                i += 2;
            }
            #endif
            /*-\NEW\shenyuanyuan\2017.12.5\增加西班牙特殊字符的显示支持 */
            else
            {
                charcode = '?';
                i += 1;
            }
        }
        else
        {
            // ascii char
            charcode = pText[i];
            i += 1;
        }


        getCharBitmap(&bitmap, charcode);

        if(lcd_bpp == 1)
        {
            if(bitmap.width == 14 && bitmap.height == 14)
            {
                disp_bitmap_bpp1(&bitmap, (u16)(x+1), (u16)(y+1));
                bitmap.width = 16;
                bitmap.height = 16;
            }
            else
            {
                disp_bitmap_bpp1(&bitmap, x, y);
            }
        }
        else
            disp_1bitbmp_bpp16(&bitmap, x, y);

next_line:   
        // 自动换行显示
        if(x + bitmap.width >= lcd_width)
        {
            y += bitmap.height;
            x = 0;
        }
        else
        {
            x += bitmap.width;
        }

        //自动回显
        if( y >= lcd_height)
        {
            y = 0;
        }
    }
}

/*+\NEW\liweiqiang\2013.11.4\增加BMP图片显示支持 */
/*	Support for BMP files		*/
#ifdef WIN32
#define __attribute__(x)
#endif

#ifdef WIN32 
#pragma pack(push,pack1,1)
#endif
typedef struct _bitmap_file_header
{
    u16 file_type;
    u32 file_size;
    u16 reserved1;
    u16 reserved2;
    u32 bitmap_offset;
} __attribute__((packed)) bitmap_file_header ;

typedef struct _bitmap_info_header
{
    u32 header_size;
    u32 width;
    u32 height;
    u16 number_of_planes;
    u16 bits_per_pixel;
    u32 compression;
    u32 bitmap_size;
    u32 device_width;
    u32 device_height;
    u32 number_of_colors;
    u32 number_of_important_colors;
} __attribute__((packed)) bitmap_info_header ;
#ifdef WIN32 
#pragma pack(pop,pack1)
#endif

#define RGB24ToRGB16(r,g,b) (((r&0x00f8)<<8)|((g&0x00fc)<<3)|((b&0xf8)>>3))

/*+\NEW\liweiqiang\2013.12.6\支持bpp16的bmp显示 */
/*+\NewReq NEW\zhutianhua\2013.12.24\显示图片的指定区域*/
static int put_bmp_file_buff(const u8 *bitmap_buffer, int x, int y, int transcolor, int left, int top, int right, int bottom)
/*-\NewReq NEW\zhutianhua\2013.12.24\显示图片的指定区域*/
{
    bitmap_file_header *p_fileHeader = (bitmap_file_header *)bitmap_buffer;
    bitmap_info_header *p_infoHeader = (bitmap_info_header *)(bitmap_buffer+sizeof(bitmap_file_header));
    const u8 *data_buf;
    u16 data_r,data_g,data_b;
    u16 width, height;
    u16 rowIndex,colIndex;
    u16 bitmapRowBytes;
    u16 rgb16;

/*+\NEW\liweiqiang\2013.11.12\修正显示图片x,y坐标无法设置 */
    int bitmapRowIndex,bmpColIndex;       
    u32 data_index;

    u16 *buffer16 = (u16*)framebuffer;
    u16 bmp_bpp = p_infoHeader->bits_per_pixel;
    u16 temp;
    
    /*+\NewReq NEW\zhutianhua\2013.12.24\显示图片的指定区域*/
    if((left > right) || (top > bottom))
    {
        printf("put_bmp_buffer: rect error\n");
        return PLATFORM_ERR;
    }

    if((left == 0) && (top == 0) && (right == 0) && (bottom == 0))
    {
        width = MIN(p_infoHeader->width + x, lcd_width);
        height = MIN(p_infoHeader->height + y, lcd_height);  
        bottom = p_infoHeader->height - 1;
    }
    else
    {
        width = MIN(right - left + 1 + x, lcd_width);
        height = MIN(bottom - top + 1 + y, lcd_height); 
    }
    /*-\NewReq NEW\zhutianhua\2013.12.24\显示图片的指定区域*/

    data_buf = bitmap_buffer + p_fileHeader->bitmap_offset;
    
  //  platform_assert(NULL, 0);
   // bitmapRowBytes = ((p_infoHeader->width*bmp_bpp + 31)&~31)>>3; //4字节对齐
    bitmapRowBytes = ((p_infoHeader->width*bmp_bpp + 31)&~31)>>3; //4字节对齐
    /*+\new\liweiqiang\2014.9.9\增加黑白image在黑白屏上的显示接口 */
    if (lcd_bpp == 1 && bmp_bpp == 1)
    {
        int page;
        u8 lcdfill = lcd_hwfillcolor == COLOR_WHITE ? 1:0;
        u8 bmpfill = 1;
        u8 finalfill;

        if (p_infoHeader->number_of_planes == 1)
        {
            const u8 *fill_plate_p = bitmap_buffer \
                + sizeof(bitmap_file_header) /* 跳过文件头 */ \
                + sizeof(bitmap_info_header) /* 跳过信息头 */ \
                + 4 /* 跳过0值的调色板信息 */;

            if(fill_plate_p[0] == 0xff && fill_plate_p[1] == 0xff && fill_plate_p[2] == 0xff){
                // 1值调色板如果是全ff 表示白色 其他值都判定为黑色
                bmpfill = 1;
            } else {
                bmpfill = 0;
            }
        }

        // lcd的填充色与位图填充色的一致结果
        finalfill = ((lcdfill^bmpfill) == 0) ? 0x80 : 0x00;

        for(rowIndex = y, bitmapRowIndex = p_infoHeader->height - top - 1; 
            rowIndex < height && bitmapRowIndex >= p_infoHeader->height - bottom - 1;
            rowIndex++, bitmapRowIndex--)
        {
            page = rowIndex/8;

            for(colIndex = x, bmpColIndex = left; colIndex < width; colIndex++, bmpColIndex++)
            {
                /* 如果该点为需要填充 并且位图填充色与lcd填充色一致 则填充 */
                if(0 == ((data_buf[bitmapRowBytes*bitmapRowIndex+bmpColIndex/8]&(0x80>>(bmpColIndex%8)))^(finalfill>>(bmpColIndex%8))))
                    framebuffer[page*lcd_width+colIndex] |= 1<<(rowIndex%8);
                /* 否则不填充 */
                else
                    framebuffer[page*lcd_width+colIndex] &= ~(1<<(rowIndex%8));
            }
        }

        return PLATFORM_OK;    
    }
    
    if(lcd_bpp != 16)
    {
        printf("put_bmp_buffer: not color lcd\n");
        return PLATFORM_ERR;
    }
    
    if(bmp_bpp != 24 && bmp_bpp != 16)
    {
        printf("put_bmp_buffer: bmp not support bpp %d\n", bmp_bpp);
        return PLATFORM_ERR;
    }
    /*-\new\liweiqiang\2014.9.9\增加黑白image在黑白屏上的显示接口 */
    
    /*+\NewReq NEW\zhutianhua\2013.12.24\显示图片的指定区域*/
    for(rowIndex = y, bitmapRowIndex = p_infoHeader->height - top - 1; 
        rowIndex < height && bitmapRowIndex >= p_infoHeader->height - bottom - 1;
        rowIndex++, bitmapRowIndex--)
    {
        for(colIndex = x, bmpColIndex = left; colIndex < width; colIndex++, bmpColIndex++)
    /*-\NewReq NEW\zhutianhua\2013.12.24\显示图片的指定区域*/
        {
            data_index = bitmapRowIndex*bitmapRowBytes + bmpColIndex*bmp_bpp/8;

            if(24 == bmp_bpp)
            {
                data_r = data_buf[data_index+2];
                data_g = data_buf[data_index+1];
                data_b = data_buf[data_index];
                /*+\NEW\liweiqiang\2013.11.12\白色区域透明显示 */
                rgb16 = RGB24ToRGB16(data_r,data_g,data_b);
            }
            else
            {
                rgb16 = *(u16*)&data_buf[data_index];
            }

/*+\NEW\liweiqiang\2013.12.6\增加图片透明色设置 */
            if(-1 == transcolor || rgb16 != transcolor)
/*-\NEW\liweiqiang\2013.12.6\增加图片透明色设置 */
            {
	            if(lcd_bus != PLATFORM_LCD_BUS_SPI)
	                temp = rgb16;
	            else
                	temp = (rgb16 >> 8 ) | ((rgb16 & 0xff) << 8);

                buffer16[rowIndex*lcd_width+colIndex] = temp;
            }
            /*-\NEW\liweiqiang\2013.11.12\白色区域透明显示 */
        }
    }
/*-\NEW\liweiqiang\2013.11.12\修正显示图片y坐标无法设置 */

    
    return PLATFORM_OK;
}
/*-\NEW\liweiqiang\2013.12.6\支持bpp16的bmp显示 */

/*+\NEW\zhuth\2014.2.14\支持png图片的显示*/
#ifdef AM_LPNG_SUPPORT

#ifdef AM_LAYER_SUPPORT

static int put_png_file_buff(const char *filename, int x, int y, int transcolor, int left, int top, int right, int bottom, int transtype)
{
    long size;
    FILE* file;
    u8* buffer;
    BOOL needSoftDecode = TRUE;
#if 1
    u32 tick1  = platform_rtos_tick();

    if(LAYER_FORMAT_RGB565 == platform_get_active_layer_format())
    {
        file = fopen(filename, "rb");

        if(NULL == file)
        {
            printf("[put_png_file_buff]: %s file not exist\n", filename);
            return PLATFORM_ERR;
        }
                
        fseek(file, 0, SEEK_END);
        size = ftell (file);
        fseek(file, 0, SEEK_SET);

        buffer = malloc(size);
        fread(buffer, size, 1, file);
        fclose(file);
        
        needSoftDecode = !platform_image_decode_png(buffer, size, x, y, left, top, right, bottom);
        free(buffer);
    }
    
    u32 tick2  = platform_rtos_tick();

    if(needSoftDecode)
    {
        png_FILE_p fp;
        png_structp read_ptr;
        png_infop read_info_ptr;
        png_bytep row_buf = NULL;
        png_uint_32 width, height, row_idx, tmp_idx;
        png_byte color_type,channel;
        png_size_t row_bytes;

        u8 data_alpha;
        u16 rgb16;
        u16 fr,fg,fb;
        u16 br,bg,bb;
        u16 layer_width;
        u16 temp;

        if((left > right) || (top > bottom))
        {
            printf("[put_png_file_buff]: rect error\n");
            return PLATFORM_ERR;
        }

        fp = fopen(filename, "rb");
        if(NULL == fp)
        {
            printf("[put_png_file_buff]: %s file not exist\n", filename);
            return PLATFORM_ERR;
        }

        read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        read_info_ptr = png_create_info_struct(read_ptr);    

        #ifdef PNG_SETJMP_SUPPORTED
        if (setjmp(png_jmpbuf(read_ptr)))
        {
           printf("[put_png_file_buff]: %s libpng read error\n", filename);
           png_free(read_ptr, row_buf);
           row_buf = NULL;
           png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
           fclose(fp);
           return PLATFORM_ERR;
        } 
        #endif

        png_init_io(read_ptr, fp);
        png_set_bgr(read_ptr);
        
        png_read_info(read_ptr, read_info_ptr);
        width = png_get_image_width(read_ptr, read_info_ptr);
        height = png_get_image_height(read_ptr, read_info_ptr);
        color_type = png_get_color_type(read_ptr, read_info_ptr);
        channel = png_get_channels(read_ptr, read_info_ptr);
        row_bytes = png_get_rowbytes(read_ptr, read_info_ptr);

        if((3 != channel) && (4 != channel))
        {
            printf("[put_png_file_buff]: channel %d error\n", channel);
            png_free(read_ptr, row_buf);
            row_buf = NULL;
            png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
            fclose(fp);
            return PLATFORM_ERR;
        }

        row_buf = (png_bytep)png_malloc(read_ptr, row_bytes);
        if(NULL == row_buf)
        {
            printf("[put_png_file_buff]: %d row no buf error\n", row_bytes);
            png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
            fclose(fp);
            return PLATFORM_ERR;
        }

        if((left == 0) && (top == 0) && (right == 0) && (bottom == 0))
        {
            left = 0;
            top = 0;
            right = width - 1;
            bottom = height - 1;
        }

        platform_get_active_layer_size(&layer_width, NULL) ;
        
        if(platform_active_layer_is_RGBA())
        {
            u32 *buffer32 = (u32*)framebuffer;
            buffer32 += y*layer_width + x;

            if(channel == 4){
                for(row_idx = 0; row_idx < height; row_idx++){
                    png_read_row(read_ptr, row_buf, NULL);
                    memcpy(buffer32, row_buf, row_bytes);
                    buffer32 += layer_width;
                }
            } else {
                u8 *buffer8;
                png_bytep temp_buf;
                for(row_idx = 0; row_idx < height; row_idx++){
                    png_read_row(read_ptr, row_buf, NULL);
                    buffer8 = (u8*)buffer32;
                    temp_buf = row_buf;
                    for(tmp_idx = 0; tmp_idx < width; tmp_idx++){
                        *buffer8++ = *temp_buf++;
                        *buffer8++ = *temp_buf++;
                        *buffer8++ = *temp_buf++;
                        *buffer8++ = 0xff;
                    }
                    buffer32 += layer_width;
                }
            }
        }
        else
        {
            u16 *buffer16 = (u16*)framebuffer;
            u16 *temp_buff16;
            png_bytep temp_buf;

            buffer16 += y*layer_width+x;
            
            for(row_idx = 0; row_idx < height; row_idx++)
            {
                png_read_row(read_ptr, row_buf, NULL);
                temp_buf = row_buf;
                temp_buff16 = buffer16;

                for(tmp_idx = 0; tmp_idx < width; tmp_idx++){
                    fb = *temp_buf++;
                    fg = *temp_buf++;
                    fr = *temp_buf++;

                    if(channel == 4){
                        data_alpha = *temp_buf++;

                        temp = *temp_buff16;

                        br = (((temp >> 11) & 0x1f) << 3);
                        bg = (((temp >> 5) & 0x3f )<< 2);
                        bb = (((temp ) & 0x1f) << 3);
                        
                        fr = (fr * data_alpha + br * (255 - data_alpha))/255;
                        fg = (fg * data_alpha + bg * (255 - data_alpha))/255; 
                        fb = (fb * data_alpha + bb * (255 - data_alpha))/255;
                    }
                    
                    rgb16 = RGB24ToRGB16(fr,fg,fb);
                    *temp_buff16++ = rgb16;
                }
                buffer16 += layer_width;
            }
        }
        
        #ifdef PNG_STORE_UNKNOWN_CHUNKS_SUPPORTED
          #ifdef PNG_READ_UNKNOWN_CHUNKS_SUPPORTED
                png_free_data(read_ptr, read_info_ptr, PNG_FREE_UNKN, -1);
          #endif
        #endif

        png_free(read_ptr, row_buf);
        row_buf = NULL;
        png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
        fclose(fp);
#endif
    }
    
    printf("[put_png_file_buff]: %d %d %d %d\n", platform_rtos_tick () - tick1, platform_rtos_tick () - tick2, needSoftDecode, platform_active_layer_is_RGBA());
    return PLATFORM_OK;
}
#else
static int put_png_file_buff(const char *filename, int x, int y, int transcolor, int left, int top, int right, int bottom, int transtype)
{
    png_FILE_p fp;
    png_structp read_ptr;
    png_infop read_info_ptr;
    png_bytep row_buf = NULL;
    png_uint_32 width, height, row_idx, tmp_idx;
    png_byte color_type,channel;
    png_size_t row_bytes;

    u16 *buffer16 = (u16*)framebuffer;
    u16 data_r,data_g,data_b;
    u8 data_alpha,proc;
    u16 rgb16;
    u32 bgrgb888,fgrgb888,dstrgb888;
    short fr,fg,fb;
    short br,bg,bb;
    u8 dr,dg,db;
    u8 fpercent, bpercent;
    u32 rgb888;
    u8 *buffer24 = (u8*)framebuffer;
    u32 layer_width = lcd_width;
    u16 temp;

    
    printf("[put_png_file_buff]: \n");
   
    if(strcmp(filename,"BAT.png") == 0)
    {
        printf("%s,%d,%d,%d,%d,%d,%d,%d", filename, x, y, transcolor, left, top, right, bottom);
    }

    if((left > right) || (top > bottom))
    {
        printf("[put_png_file_buff]: rect error\n");
        return PLATFORM_ERR;
    }

    fp = fopen(filename, "rb");
    if(NULL == fp)
    {
        printf("[put_png_file_buff]: %s file not exist\n", filename);
        return PLATFORM_ERR;
    }

    read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    read_info_ptr = png_create_info_struct(read_ptr);    

    #ifdef PNG_SETJMP_SUPPORTED
    if (setjmp(png_jmpbuf(read_ptr)))
    {
       printf("[put_png_file_buff]: %s libpng read error\n", filename);
       png_free(read_ptr, row_buf);
       row_buf = NULL;
       png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
       fclose(fp);
       return PLATFORM_ERR;
    } 
    #endif

    png_init_io(read_ptr, fp);
    png_set_bgr(read_ptr);
    
    png_read_info(read_ptr, read_info_ptr);
    width = png_get_image_width(read_ptr, read_info_ptr);
    height = png_get_image_height(read_ptr, read_info_ptr);
    color_type = png_get_color_type(read_ptr, read_info_ptr);
    channel = png_get_channels(read_ptr, read_info_ptr);
    row_bytes = png_get_rowbytes(read_ptr, read_info_ptr);
    /*if(strcmp(filename,"BAT.png") == 0)
    {
        printf("[put_png_file_buff]: width=%d,height=%d,color_type=%d,channel=%d,row_bytes=%d\n", width, height, color_type, channel,row_bytes);
    }*/
    if((3 != channel) && (4 != channel))
    {
        printf("[put_png_file_buff]: channel %d error\n", channel);
        png_free(read_ptr, row_buf);
        row_buf = NULL;
        png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
        fclose(fp);
        return PLATFORM_ERR;
    }

    row_buf = (png_bytep)png_malloc(read_ptr, row_bytes);
    if(NULL == row_buf)
    {
        printf("[put_png_file_buff]: %d row no buf error\n", row_bytes);
        png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
        fclose(fp);
        return PLATFORM_ERR;
    }

    if((left == 0) && (top == 0) && (right == 0) && (bottom == 0))
    {
        left = 0;
        top = 0;
        right = width - 1;
        bottom = height - 1;
        
        if(lcd_bpp == 24 && channel == 3)
        {
            int write_index = ((y)*layer_width + x) * 3;
            
            printf("[put_png_file_buff]: FAST READ\n");

            for(row_idx=0; row_idx< height; row_idx++)
            {
                png_read_row(read_ptr, row_buf, NULL);
                memcpy(&buffer24[write_index],
                        row_buf,
                        row_bytes);
                
                write_index += layer_width * 3;
            }

            #ifdef PNG_STORE_UNKNOWN_CHUNKS_SUPPORTED
              #ifdef PNG_READ_UNKNOWN_CHUNKS_SUPPORTED
                    png_free_data(read_ptr, read_info_ptr, PNG_FREE_UNKN, -1);
              #endif
            #endif

            png_free(read_ptr, row_buf);
            row_buf = NULL;
            png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
            fclose(fp);

            return PLATFORM_OK;

        }
    }

    for(row_idx=0; row_idx<height; row_idx++)
    {
    	 if(row_idx > bottom)
    	 {
    	     break;
    	 }
    	 
        png_read_row(read_ptr, row_buf, NULL);
        if(row_idx < top)
    	 {
    	     continue;
    	 }
    	 
        for(tmp_idx=left; tmp_idx<=right; tmp_idx++)
        {
            proc = 0;
            
            if(channel==4)
            {
               data_alpha = row_buf[tmp_idx*channel + 3];
            }

            if(lcd_bpp == 16)
            {
                fr = row_buf[tmp_idx*channel + 2];
                fg = row_buf[tmp_idx*channel + 1];
                fb = row_buf[tmp_idx*channel + 0];

                rgb16 = RGB24ToRGB16(fr,fg,fb);
                
                if(-1 == transcolor || rgb16 != transcolor || transtype == 2)
                {
                    if(channel==4)
                    {
                        //0:正常处理全透，非全透的当做不透明处理
                        //1:正常处理所有透明度
                        //2:正常处理全透，非全透的当成与transcolor相同的透明度处理
                        if((transtype==0 && data_alpha==0)
                            || (transtype==1)
                            || (transtype==2)
                            )
                        {
                            proc = 1;

				            if(lcd_bus == PLATFORM_LCD_BUS_SPI)
                            {
			                	temp = (buffer16[(y+row_idx-top)*layer_width+(x+tmp_idx-left)] >> 8 ) | 
                                    ((buffer16[(y+row_idx-top)*layer_width+(x+tmp_idx-left)] & 0xff) << 8);
                            }   
                            else
                            {
                                temp = buffer16[(y+row_idx-top)*layer_width+(x+tmp_idx-left)];
                            }
                            
                            br = (temp >> 8) & 0xf8;
                            bg = (temp >> 3) & 0xfc;
                            bb = (temp << 3) & 0xf8;
                            
                            
                           if(transtype==2 && data_alpha != 0)
                            {
                                data_alpha = transcolor;
                            }
                           
                            dr = (fr * data_alpha + br * (255 - data_alpha))/255;
                            dg = (fg * data_alpha + bg * (255 - data_alpha))/255; 
                            db = (fb * data_alpha + bb * (255 - data_alpha))/255;

				            if(lcd_bus != PLATFORM_LCD_BUS_SPI)
				                temp = RGB24ToRGB16(dr, dg, db);
				            else
			                	temp = (RGB24ToRGB16(dr, dg, db) >> 8 ) | ((RGB24ToRGB16(dr, dg, db) & 0xff) << 8);

                            buffer16[(y+row_idx-top)*layer_width+(x+tmp_idx-left)] = temp;
                            
                        }                    
                    }
                    
                    if(proc==0)
                    {
					
			            if(lcd_bus != PLATFORM_LCD_BUS_SPI)
			                temp = rgb16;
			            else
		                	temp = (rgb16 >> 8 ) | ((rgb16 & 0xff) << 8);

                        buffer16[(y+row_idx-top)*layer_width+(x+tmp_idx-left)] = temp;
                    }
                }
            }
            else  if(lcd_bpp == 24)
            {
            
                fr = row_buf[tmp_idx*channel + 2];
                fg = row_buf[tmp_idx*channel + 1];
                fb = row_buf[tmp_idx*channel + 0];

                if(channel==4)
                {
                    //0:正常处理全透，非全透的当做不透明处理
                    //1:正常处理所有透明度
                    //2:正常处理全透，非全透的当成与transcolor相同的透明度处理
                    if(data_alpha == 0xff)
                    {
                        buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 2] = fr;// dstrgb888 >> 16;
                        buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 1] = fg;//(dstrgb888    & 0xff00)>> 8;
                        buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 0] = fb;// dstrgb888 &0xff;
						continue;
                    }
                    else if(data_alpha == 0)
                    {
                        continue;
                    }
                    else
                    {
					    if(transtype== 2 && data_alpha != 0)
						{
						    fpercent = transcolor;
						}
						else
						{
						    fpercent = data_alpha;
						}
                        br = buffer24[((y+row_idx-top)*layer_width + (x+tmp_idx-left)) * 3 + 2] ;
                        bg = buffer24[((y+row_idx-top)*layer_width + (x+tmp_idx-left)) * 3 + 1] ;
                        bb = buffer24[((y+row_idx-top)*layer_width + (x+tmp_idx-left)) * 3 + 0] ;
                                                    
                        dr = ((((fr - br)* fpercent) >> 8) ) + br; 
                        dg = ((((fg - bg)* fpercent) >> 8) ) + bg; 
                        db = ((((fb - bb)* fpercent) >> 8) ) + bb; 
                        
                        buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 2] = dr;// dstrgb888 >> 16;
                        buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 1] = dg;//(dstrgb888  & 0xff00)>> 8;
                        buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 0] = db;// dstrgb888 &0xff;
                    }     
                }
				else
				{
                    buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 2] = fr;// dstrgb888 >> 16;
                    buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 1] = fg;//(dstrgb888    & 0xff00)>> 8;
                    buffer24[((y+row_idx-top)*layer_width+(x+tmp_idx-left))*3 + 0] = fb;// dstrgb888 &0xff;
				}

                
            }
        }
    }

    #ifdef PNG_STORE_UNKNOWN_CHUNKS_SUPPORTED
      #ifdef PNG_READ_UNKNOWN_CHUNKS_SUPPORTED
            png_free_data(read_ptr, read_info_ptr, PNG_FREE_UNKN, -1);
      #endif
    #endif

    png_free(read_ptr, row_buf);
    row_buf = NULL;
    png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
    fclose(fp);
    
    return PLATFORM_OK;
}
#endif
#endif
/*-\NEW\zhuth\2014.2.14\支持png图片的显示*/


static int put_jpg_file_buff(const char *filename, int x, int y, int transcolor, int left, int top, int right, int bottom)
{
    image_t image_info;
    char* actname;
    int res, devid;
    u16* buffer = (u16*)framebuffer;

  // Look for device, return error if not found or if function not implemented

    
    if((left > right) || (top > bottom))
    {
        printf("[put_jpg_file_buff]: rect error\n");
        return PLATFORM_ERR;
    }


    if(!platform_image_decode_jpeg(filename, &image_info))
    {
        int startPos; 
        int i;
        int j;
        
        if((left == 0) && (top == 0) && (right == 0) && (bottom == 0))
        {
            left = 0;
            top = 0;
            right = image_info.width - 1;
            bottom = image_info.height - 1;
        }

        startPos = top *image_info.width + left;

        for(i = top;  i <= bottom; i ++)
        {

#if 0
            for(j = 0 ; j <= right - left ; j ++) 
            {
                framebuffer[(y*lcd_width + x + j) << 1] = image_info.buffer[(startPos + j) ] >> 0x8;
                framebuffer[((y*lcd_width + x + j) << 1) + 1] = image_info.buffer[(startPos + j)] & 0xff;
            }

#else
            memcpy(&buffer[y*lcd_width + x],
                &image_info.buffer[startPos], 
                (right - left + 1) * 2);
#endif
            
            startPos += image_info.width;
            y++;
        }

        platform_free_image_jpeg_decodedata(image_info.buffer);
    }
    
    return PLATFORM_OK;
}



int show_jpg_file(const char *filename, int* width, int* height)
{
    image_t image_info;
    char* actname;
    int res, devid;
    u8* buffer = NULL;
    int startY = 0;
    int left, top, right, bottom;
    
    if(!platform_image_decode_jpeg(filename, &image_info))
    {
        int startPos; 
        int i;
        int j;
        u8 temp;
        
        left = 0;
        top = 0;
        right = image_info.width - 1;
        bottom = image_info.height - 1;
        
        startPos = 0;
        
        buffer = (u8*)image_info.buffer;

        if(lcd_bus == PLATFORM_LCD_BUS_SPI)
        {
            for(i = top;  i <= bottom; i ++)
            {
                for(j = 0 ; j <= right - left ; j ++) 
                {
                    temp = image_info.buffer[(startPos + j)];
                    
                    buffer[(startPos + j) << 1] = image_info.buffer[(startPos + j) ] >> 0x8;
                    buffer[((startPos + j) << 1) + 1] = temp;
                }

                #if 0
                memcpy(&buffer[y*lcd_width + x],
                    &image_info.buffer[startPos], 
                    (right - left + 1) * 2);
                #endif
                
                startPos += image_info.width;
            }
        }

        *width = image_info.width;
        *height = image_info.height;

        #ifdef AIR202_LUA_UI_SPC
            lcd_bus_spc_spi_write_data_buf(buffer, image_info.width * image_info.height * (lcd_bpp / 8));
        #endif
        platform_free_image_jpeg_decodedata(image_info.buffer);
    }
    
    return PLATFORM_OK;
}




/*+\bug NEW\zhutianhua\2013.12.24\显示图片的指定区域*/
int platform_disp_putimage(const char *filename, u16 x, u16 y, int transcolor, u16 left, u16 top, u16 right, u16 bottom, int transtype)
/*-\bug NEW\zhutianhua\2013.12.24\显示图片的指定区域*/
{
    u8 *buff;
    FILE *fp;
    u32 len;
    int result;

    /*+\NEW\zhuth\2014.2.16\支持png图片的显示*/
    if(strstr(filename, ".bmp") || strstr(filename, ".BMP"))
    {
        fp = fopen(filename, "rb");

        if(NULL == fp)
        {
            printf("[putimage]: %s file not exist\n", filename);
            return PLATFORM_ERR;
        }
    
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        buff = malloc(len);
        if(NULL == buff)
        {
            printf("[putimage]:not enought memory\n");
            fclose(fp);
            return PLATFORM_ERR;
        }
        
        fread(buff, 1, len, fp);
        fclose(fp);
    
        /*+\NewReq NEW\zhutianhua\2013.12.24\显示图片的指定区域*/
        result = put_bmp_file_buff(buff, x, y, transcolor, left, top, right, bottom);
        /*-\NewReq NEW\zhutianhua\2013.12.24\显示图片的指定区域*/
        free(buff);
    }
    #ifdef AM_LPNG_SUPPORT
    else if(strstr(filename, ".png") || strstr(filename, ".PNG"))
    {
        result = put_png_file_buff(filename, x, y, transcolor, left, top, right, bottom, transtype);
    }
    #endif
    /*-\NEW\zhuth\2014.2.16\支持png图片的显示*/

    else if(strstr(filename, ".jpg") || strstr(filename, ".JPG"))
    {
        result = put_jpg_file_buff(filename, x, y, transcolor, left, top, right, bottom);
    }   
    else
    {
        printf("[putimage]:only support bmp and png!\n");
        return PLATFORM_ERR;
    }
    
    return result;
}
/*-\NEW\liweiqiang\2013.11.4\增加BMP图片显示支持 */


#ifdef AM_LAYER_SUPPORT
void platform_disp_drawline(int x0, int y0, int x1, int y1, int width, int color)
{
	int abs_y;
	int abs_x;
	int count;
	int x, y, dx, dy, e, i;
    u16 *buf16 = (u16*)framebuffer;

	dx = x1 - x0;
	dy = y1 - y0;
	e = -dx;
	x = x0;
	y = y0;

	abs_y = abs(dy);
	abs_x = abs(dx);

	count = (abs_x > abs_y) ? abs_x : abs_y;

	e = (abs_x > abs_y) ? -abs_x : -abs_y;


	for (i = 0; i < count; i++) 
	{
		int j;
        
		if(abs_x > abs_y)
        {      
			for (j = -width / 2; j < (width + 1) / 2; j++)
			{
				buf16[(y + j)*lcd_width + x] = color;
			}
        }
		else
		{
		    for (j = -width / 2; j < (width + 1) / 2; j++)
			{
				buf16[(y)*lcd_width + x + j] = color;
			}
        }
        
		(abs_x > abs_y) ? ((dx > 0) ? x++ : x--) : ((dy > 0) ? y++ : y--);

		e = e + 2 * ((abs_x > abs_y)  ? abs_y : abs_x);

		if (e >= 0)
		{
			(abs_x > abs_y) ? ((dy > 0) ? y++ : y--) : ((dx > 0) ? x++ : x--);

			e = e - 2 * ((abs_x > abs_y) ? abs_x: abs_y);
		}
	}
}
#endif

/*+\NEW\liweiqiang\2013.12.7\增加矩形显示支持 */
int platform_disp_drawrect(int x1, int y1, int x2, int y2, int color)
{
    int i,j;
    x1 = MIN(x1, lcd_width-1);
    x2 = MIN(x2, lcd_width-1);
    y1 = MIN(y1, lcd_height-1);
    y2 = MIN(y2, lcd_height-1);

    if(x1 >= x2 || y1 >= y2)
    {
        printf("[platform_disp_drawrect]: range error %d %d %d %d!\n", x1, y1, x2, y2);
        return PLATFORM_ERR;
    }

    if(lcd_bpp == 16)
    {
        u16 *buf16 = (u16*)framebuffer;
        u16 temp;
        
        if(-1 == color)
        {
            //画矩形框
            int height = y2 - y1;
            int pixels = height*lcd_width;
            
            buf16 += y1*lcd_width;

            if(lcd_bus != PLATFORM_LCD_BUS_SPI)
                temp = disp_color;
            else
                temp = (disp_color >> 8 ) | ((disp_color & 0xff) << 8);

                
            for(i = x1; i <= x2; i++)
            {
                buf16[i] = temp;
                buf16[pixels+i] = temp;
            }

            for(j = y1; j <= y2; j++)
            {
                buf16[x1] = temp;
                buf16[x2] = temp;
                buf16 += lcd_width;
            }
        }
        else
        {
            //以填充色填充矩形
            buf16 += y1*lcd_width;

            for(j = y1; j <= y2; j++)
            {
                for(i = x1; i <= x2; i++)
                {
                    buf16[i] = color;
                }
                buf16 += lcd_width;
            }
        }
    }
/*+\bug0\zhy\2014.10.15\黑白屏添加*/
    else if(lcd_bpp == 1)
    {
        if(color == COLOR_BLACK || color == COLOR_WHITE)
        {
            u16 x,y,page;
            
            if((color^lcd_hwfillcolor) == 0) /*\new\liweiqiang\2014.10.21\增加不同黑白屏填充色处理 */
            {

                for(x = x1; x < x2; x++)
                {
                    for(y = y1;y < y2; y++)
                    {
                        page = y/8;
                        framebuffer[page*lcd_width+x] |= 1<<(y%8);
                    }
                }
            }
            else
            {
                for(x = x1; x < x2; x++)
                {
                    for(y = y1;y < y2; y++)
                    {
                        page = y/8;
                        framebuffer[page*lcd_width+x] &= ~(1<<(y%8));
                    }
                }
            }
        }
        else
        {
            printf("[platform_disp_drawrect]: lcd_bpp = 1,color must be balck or white\n");
        }
    }
/*-\bug0\zhy\2014.10.15\黑白屏添加*/
    else
    {
        // 暂不支持其他屏
        printf("[platform_disp_drawrect]: not support bpp %d\n", lcd_bpp);
        return PLATFORM_ERR;
    }

    return PLATFORM_OK;
}
/*-\NEW\liweiqiang\2013.12.7\增加矩形显示支持 */

/*+\NEW\liweiqiang\2013.12.9\增加前景色\背景色设置 */
int platform_disp_setcolor(int color)
{
    int old_color = disp_color;
    disp_color = color;
    return old_color;
}

int platform_disp_setbkcolor(int color)
{
    int old_color = disp_bkcolor;
    disp_bkcolor = color;
    return old_color;
}
/*-\NEW\liweiqiang\2013.12.9\增加前景色\背景色设置 */

/*+\NEW\liweiqiang\2013.12.9\增加非中文字体设置 */
#ifdef WIN32 
#pragma pack(push,pack1,1)
#endif
typedef struct FontFileInfoTag
{
    u8        width;
    u8        height;
    u8        type;
    u16       start;
    u16       end;
}__attribute__((packed)) FontFileInfo;
#ifdef WIN32 
#pragma pack(pop,pack1)
#endif

static int load_file_data(const char *name, u8 **buf_pp)
{
    FILE *fp = NULL;
    int size;
    char *buf = NULL;
    
    fp = fopen(name, "rb");
    
    if(fp == NULL)
    {
        printf("[load_file_data]: file not exist!\n");
        goto load_error;
    }
    
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    buf = malloc(size);
    if(NULL ==  buf)
    {
        printf("[load_file_data]: no memory\n");
        goto load_error;
    }
    fseek(fp, 0, SEEK_SET);
    fread(buf, 1, size, fp);
    fclose(fp);
    
    *buf_pp = buf;
    return size;
    
load_error:
    if(fp)
        fclose(fp);
    
    if(buf)
        free(buf);
    return 0;
}

int platform_disp_loadfont(const char *name)
{
    u8 *buf = NULL;
    int size = 0;
    FontFileInfo *fileInfo_p;
    u32 charsize;
    BOOL found = FALSE;
    int i;
    int retcode = 0;

    size = load_file_data(name, &buf);
    
    if(size <= sizeof(FontFileInfo))
    {
        retcode = -1;
        goto load_font_error;
    }

    fileInfo_p = (FontFileInfo *)buf;

    if(fileInfo_p->type != 0)
    {
        // 只支持连续字符方式的字库
        retcode = -2;
        goto load_font_error;
    }

    if(fileInfo_p->end < fileInfo_p->start)
    {
        retcode = -3;
        goto load_font_error;
    }
    
    charsize = (fileInfo_p->width+7)/8;
    charsize *= fileInfo_p->height;

    if(charsize*(fileInfo_p->end - fileInfo_p->start + 1) != size - sizeof(FontFileInfo))
    {
        retcode = -4;
        goto load_font_error;
    }

    for(i = 0; i < MAX_FONTS && !found; i++)
    {
        if(dispFonts[i].data == NULL)
        {
            found = TRUE;
            break;
        }
    }
    
    if(!found)
    {
        retcode = -5;
        goto load_font_error;
    }

    dispFonts[i].width = fileInfo_p->width;
    dispFonts[i].height = fileInfo_p->height;
    dispFonts[i].size = charsize;
    dispFonts[i].start = fileInfo_p->start;
    dispFonts[i].end = fileInfo_p->end;
    dispFonts[i].data = buf + sizeof(FontFileInfo);

    return i;

load_font_error:
    if(buf)
        free(buf);

    printf("[platform_disp_loadfont]:error code %d\n", retcode);

    return -1;
}

int platform_disp_setfont(int id)
{
    int old_font_id;

    /*+\NEW\liweiqiang\2013.12.10\修正负字体id导致花屏 */
    if(id < 0 || id >= MAX_FONTS || NULL == dispFonts[id].data)
    /*-\NEW\liweiqiang\2013.12.10\修正负字体id导致花屏 */
    {
        printf("[platform_disp_setfont]: error font id\n");
        return -1;
    }

    old_font_id = curr_font_id;
    curr_font_id = id;
    return old_font_id;
}
/*-\NEW\liweiqiang\2013.12.9\增加非中文字体设置 */

/*+\NEW\lisiqi\2017.11.11\增加二维码显示支持 */
int platform_disp_qrcode(u8 *data, u16 w, u16 disp_w, u16 start_x, u16 start_y)
{
	int i, j;
	u16 color, end_x, end_y, bx, by, page;
	unsigned int write_len = 0;
	u16 *buf16 = (u16*)framebuffer;
    int scale;
    int x_width, y_width;
    
	if ( disp_w < w || (start_x >= lcd_width) || (start_y >= lcd_width) )
	{
		goto __error;
	}

    x_width = lcd_width - start_x;
    y_width = lcd_height - start_y;
    
    x_width = MIN(x_width, y_width);
    disp_w = MIN(disp_w, x_width);

    if(disp_w < w) goto __error;
    
    scale = disp_w / w;
    disp_w = w*scale;

    end_x = disp_w + start_x;
    end_y = disp_w + start_y;
    
	switch (lcd_bpp)
	{
	case 1:
		for (i = start_x, bx = 0; i < end_x; i++, bx++)
		{
			for (j = start_y, by = 0; j < end_y; j++, by++)
			{
				if (data[(bx/scale) + (by/scale) * w] & 1)
				{
					color = COLOR_BLACK;
				}
				else
				{
					color = COLOR_WHITE;
				}
				page = j/8;
	            if((color^lcd_hwfillcolor) == 0)
	            {
	            	framebuffer[page*lcd_width + i] |= 1<<(j%8);
	            }
	            else
	            {
	                framebuffer[page*lcd_width + i] &= ~(1<<(j%8));
	            }
	            write_len++;
			}
		}
		break;
	case 16:
		for (i = start_x, bx = 0; i < end_x; i++, bx++)
		{
            for (j = start_y, by = 0; j < end_y; j++, by++)
            {
                if (data[(bx/scale) + (by/scale) * w] & 1)
                {
                    color = COLOR_BLACK;
                }
                else
                {
                    color = COLOR_WHITE;
                }
                buf16[i + j * lcd_width] = color;
                write_len++;
            }
		}
		break;
	default:
		goto __error;
	}
	return write_len;

__error:
	write_len = 0;
	return write_len;
}
/*-\NEW\lisiqi\2017.11.11\增加二维码显示支持 */
#endif

