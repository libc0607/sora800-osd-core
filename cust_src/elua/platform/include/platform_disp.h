/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    platform_disp.h
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/3/26
 *
 * Description:
 *          platform display 接口
 **************************************************************************/

#ifndef _PLATFORM_DISP_H_
#define _PLATFORM_DISP_H_

#if defined LUA_DISP_LIB

typedef struct PlatformRectTag
{
    u16 ltx;        //left top x y
    u16 lty;
    u16 rbx;        //right bottom x y
    u16 rby;
}PlatformRect;

// 颜色定义 RGB(5,6,5)
#define COLOR_WHITE 0xffff
#define COLOR_BLACK 0x0000

typedef enum PlatformLcdBusTag
{
    PLATFORM_LCD_BUS_SPI4LINE,
    PLATFORM_LCD_BUS_PARALLEL,

/*+\new\liweiqiang\2014.10.11\添加lcd i2c spi接口 */
    PLATFORM_LCD_BUS_I2C,
    PLATFORM_LCD_BUS_SPI,
/*-\new\liweiqiang\2014.10.11\添加lcd i2c spi接口 */
    
    PLATFORM_LCD_BUS_QTY,
}PlatformLcdBus;

/*+\new\liweiqiang\2014.10.11\添加lcd i2c spi接口 */
typedef union {
    struct {
        int bus_id;
        int pin_rs;
        int pin_cs;
        int freq;
    } bus_spi;
    
    struct {
        int bus_id;
        int freq;
        int slave_addr;
        int cmd_addr;
        int data_addr;
    } bus_i2c;
} lcd_itf_t;
/*-\new\liweiqiang\2014.10.11\添加lcd i2c spi接口 */

typedef struct PlatformDispInitParamTag
{
    u16 width;  // lcd设备宽度
    u16 height; // lcd设备高度
    u8  bpp; // bits per pixel lcd设备色深 1:黑白 16:16位色彩屏
    u16 x_offset;
    u16 y_offset;
    u32 *pLcdCmdTable;    //lcd初始化指令表
    u16 tableSize;         //lcd初始化指定表大小
/*+\NEW\liweiqiang\2013.12.18\增加lcd睡眠命令支持 */
    u32 *pLcdSleepCmd;  // lcd sleep指令表
    u16 sleepCmdSize;
    u32 *pLcdWakeCmd;   // lcd wake指令表
    u16 wakeCmdSize;
/*-\NEW\liweiqiang\2013.12.18\增加lcd睡眠命令支持 */
/*+\NEW\liweiqiang\2017.12.8\增加lcd RASET/CASET/GRAMWRITE 命令支持 */
    u32 *pLcdAddressCmd;   // lcd address&ram write指令表
    u16 addressCmdSize;
/*-\NEW\liweiqiang\2017.12.8\增加lcd RASET/CASET/GRAMWRITE 命令支持 */
    PlatformLcdBus bus;
/*+\new\liweiqiang\2014.10.11\添加lcd i2c接口 */
    lcd_itf_t lcd_itf;
/*-\new\liweiqiang\2014.10.11\添加lcd i2c接口 */
    int pin_rst; //reset pin
    /*+\new\liweiqiang\2014.10.21\增加不同黑白屏填充色处理 */
    int hwfillcolor; // lcd物理填充色
    /*-\new\liweiqiang\2014.10.21\增加不同黑白屏填充色处理 */
/*+\NEW\2013.4.10\增加黑白屏显示支持 */
    int pin_cs; // cs pin
    int pin_rs; // rs pin
    u8 *framebuffer;
/*-\NEW\2013.4.10\增加黑白屏显示支持 */
}PlatformDispInitParam;

typedef enum LCD_LAYER_FORMAT_TAG
{
    LAYER_FORMAT_RGB565,
    LAYER_FORMAT_RGBA,
}LCD_LAYER_FORMAT;

typedef struct LCD_LAYER_INFO_tag
{   
    LCD_LAYER_FORMAT format;
    PlatformRect     rect;
    u8*              buffer;
    BOOL             used;
}LCD_LAYER_INFO;

typedef enum LCD_LAYER_ID_TAG
{
    LAYER_ID_0,
    LAYER_ID_1,
    LAYER_ID_2,
    LAYER_ID_MAX,
    LAYER_ID_INVALID = 0xff
}LCD_LAYER_ID;


void platform_disp_init(PlatformDispInitParam *pParam);

void platform_get_lcd_info(u16 *pWidth, u16 *pHeight, u8 *pBpp);

void platform_disp_close(void);


#ifdef AM_LAYER_SUPPORT
void platform_disp_clear(int left, int top, int right, int bottom);
void platform_disp_update(int left, int top, int right, int bottom, int offsetX, int offsetY);
BOOL create_layer(  int layer_id,  
                        int format ,  
                        int left   ,  
                        int top    ,  
                        int right  ,  
                        int bottom);

void destory_layer(  int layer_id);
void set_active_layer(int layer_id);
int get_active_layer(void);
void set_layer_start_pos(int layer_id, u32 startX,  u32 startY);
void set_layer_visible(BOOL layer_visible[LAYER_ID_MAX] );

void set_layer_format(int layer_id, int format);
void layer_move_image(int layer_id, BOOL move_left);
void platform_disp_drawline(int x0, int y0, int x1, int y1, int width, int color);
void  get_active_layer_size(u16* width, u16*height);

#else
void platform_disp_update(void);
void platform_disp_clear(void);
#endif
void platform_disp_puttext(const char *string, u16 x, u16 y);

/*+\NEW\liweiqiang\2013.12.6\增加图片透明色设置 */
/*+\NEW\liweiqiang\2013.11.4\增加BMP图片显示支持 */
/*+\NewReq NEW\zhutianhua\2013.12.24\显示图片的指定区域*/
int platform_disp_putimage(const char *filename, u16 x, u16 y, int transcolor, u16 left, u16 top, u16 right, u16 bottom, int transtype);
/*-\NewReq NEW\zhutianhua\2013.12.24\显示图片的指定区域*/
/*-\NEW\liweiqiang\2013.11.4\增加BMP图片显示支持 */
/*-\NEW\liweiqiang\2013.12.6\增加图片透明色设置 */

/*+\NEW\lisiqi\2017.11.11\增加二维码显示支持 */
int platform_disp_qrcode(u8 *data, u16 w, u16 disp_w, u16 start_x, u16 start_y);
/*-\NEW\lisiqi\2017.11.11\增加二维码显示支持 */

/*+\NEW\liweiqiang\2013.12.7\增加矩形显示支持 */
int platform_disp_drawrect(int x1, int y1, int x2, int y2, int color);
/*-\NEW\liweiqiang\2013.12.7\增加矩形显示支持 */

/*+\NEW\liweiqiang\2013.12.9\增加前景色\背景色设置 */
int platform_disp_setcolor(int color);
int platform_disp_setbkcolor(int color);
/*-\NEW\liweiqiang\2013.12.9\增加前景色\背景色设置 */

/*+\NEW\liweiqiang\2013.12.9\增加非中文字体设置 */
int platform_disp_loadfont(const char *name);
int platform_disp_setfont(int id);
/*-\NEW\liweiqiang\2013.12.9\增加非中文字体设置 */

#endif

#endif//_PLATFORM_DISP_H_
