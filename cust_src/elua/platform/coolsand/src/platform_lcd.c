/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    platform_lcd.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/3/26
 *
 * Description:
 *           lcd接口实现
 **************************************************************************/
 #if defined LUA_DISP_LIB

#include "string.h"
#include "malloc.h"

#include "rda_pal.h"
#include "assert.h"
#include "platform.h"
#include "platform_lcd.h"

typedef void (*lcd_init_op)();
typedef void (*lcd_cmd_data_op)(uint16 val);
typedef void (*lcd_update_op)(PlatformRect *pRect, u8 *buffer);
typedef void (*lcd_write_buf_op)(uint8 *buf, uint32 len);

#ifdef AM_LAYER_SUPPORT
typedef void (*lcd_update_layer_op)(PlatformRect *pRect, PlatformRect *pRoiRect);
#endif

#define delayMilliseconds(ms) 		IVTBL(sleep)((ms)*16384/1000)

static u8 lcd_bpp;
static u16 lcd_x_offset;
 u16 lcd_y_offset;
/*+\NEW\liweiqiang\2013.12.18\增加lcd睡眠命令支持 */
static u32 *lcd_sleep_cmd;
static u32 *lcd_wake_cmd;
/*-\NEW\liweiqiang\2013.12.18\增加lcd睡眠命令支持 */

/*+\NEW\liweiqiang\2017.12.8\增加lcd RASET/CASET/GRAMWRITE 命令支持 */
static u32 *lcd_address_cmd;
/*-\NEW\liweiqiang\2017.12.8\增加lcd RASET/CASET/GRAMWRITE 命令支持 */

#ifdef AM_LAYER_SUPPORT
static lcd_update_layer_op lcd_bus_update_layer;
#endif

static lcd_update_op    lcd_bus_update;
static lcd_cmd_data_op  lcd_bus_write_cmd;
static lcd_cmd_data_op  lcd_bus_write_data;
static lcd_write_buf_op lcd_bus_write_data_buf = NULL;

static PlatformLcdBus lcd_bus;
static lcd_itf_t lcd_bus_itf;

/*+\new\liweiqiang\2014.10.27\修正黑白屏开机花屏问题 */
static uint8 lcd_first_update = 1;
/*-\new\liweiqiang\2014.10.27\修正黑白屏开机花屏问题 */

extern E_AMOPENAT_GPIO_PORT platform_pio_get_gpio_port(int port_pin);

static void lcd_bus_spi4line_init(const PlatformDispInitParam *pParam);
static void lcd_bus_parallel_init(const PlatformDispInitParam *pParam);
static void lcd_bus_i2c_init(const PlatformDispInitParam *pParam);
static void lcd_bus_spi_init(const PlatformDispInitParam *pParam);

#ifdef AIR202_LUA_UI_SPC
static void lcd_spc_bus_spi_init(const PlatformDispInitParam *pParam);
#endif

static const lcd_init_op lcd_init_table[PLATFORM_LCD_BUS_QTY] = {
    lcd_bus_spi4line_init,
    lcd_bus_parallel_init,
    lcd_bus_i2c_init,
#ifdef AIR202_LUA_UI_SPC
    lcd_spc_bus_spi_init,
#else
    lcd_bus_spi_init,
#endif
};

static void lcd_write_update_area(PlatformRect *pRect){
    u16 ltx = pRect->ltx + lcd_x_offset;
    u16 lty = pRect->lty + lcd_y_offset;
    u16 rbx = pRect->rbx + lcd_x_offset;
    u16 rby = pRect->rby + lcd_y_offset;
    
    if(lcd_address_cmd){
        if(lcd_address_cmd[0] == 1){
            // 同HX8347指令集设计 CASTART CAEND RASTART RAEND GRAMWRITE
            u16 data_list[8] = {
                ltx>>8, ltx&0x00ff, rbx>>8, rbx&0x00ff, lty>>8, lty&0x00ff, rby>>8, rby&0x00ff
            };
            int i;
            
            for(i = 0; i < 8; i++){
                lcd_bus_write_cmd(lcd_address_cmd[i+1]);
                lcd_bus_write_data(data_list[i]);
            }
            lcd_bus_write_cmd(lcd_address_cmd[9]);
        } else {
            PUB_TRACE("lcd_address_cmd unknwon cmds type");
        }
    } else {
        lcd_bus_write_cmd(0x2A);
        lcd_bus_write_data(ltx>>8);
        lcd_bus_write_data(ltx&0x00ff);
        lcd_bus_write_data(rbx>>8);
        lcd_bus_write_data(rbx&0x00ff);
        
        lcd_bus_write_cmd(0x2B);
        lcd_bus_write_data(lty>>8);
        lcd_bus_write_data(lty&0x00ff);
        lcd_bus_write_data(rby>>8);
        lcd_bus_write_data(rby&0x00ff);
        
        lcd_bus_write_cmd(0x2C);
    }
}

#ifdef AM_LAYER_SUPPORT
static void lcd_bus_color_update_layer(PlatformRect *pRect, PlatformRect *pRoiRect){
    T_AMOPENAT_LCD_RECT_T rect;
    
    rect.ltX = pRoiRect->ltx;
    rect.ltY = pRoiRect->lty;
    rect.rbX = pRoiRect->rbx;
    rect.rbY = pRoiRect->rby;

    lcd_write_update_area(pRect);
    
   IVTBL(update_color_lcd_screen)(&rect, NULL);
}
#endif

static void lcd_bus_color_update(PlatformRect *pRect, u8 *buffer){
    T_AMOPENAT_LCD_RECT_T rect;

    rect.ltX = pRect->ltx;
    rect.ltY = pRect->lty;
    rect.rbX = pRect->rbx;
    rect.rbY = pRect->rby;

   lcd_write_update_area(pRect);
   IVTBL(update_color_lcd_screen)(&rect, (UINT16 *)buffer);
}


static void lcd_spi_color_update(PlatformRect *pRect, u8 *buffer){
    T_AMOPENAT_LCD_RECT_T rect;
    
    rect.ltX = pRect->ltx;
    rect.ltY = pRect->lty;
    rect.rbX = pRect->rbx;
    rect.rbY = pRect->rby;
    
    lcd_write_update_area(pRect);
	
    IVTBL(send_event)(0x1cd1cd1);
    lcd_bus_write_data_buf(buffer, (rect.rbY - rect.ltY + 1)* (rect.rbX - rect.ltX + 1)  * (lcd_bpp / 8));
    IVTBL(send_event)(0x1cd1cde);
}

static void lcd_bus_mono_update(PlatformRect *pRect, u8 *buffer)
{
    T_AMOPENAT_LCD_RECT_T rect;
    
    rect.ltX = pRect->ltx;
    rect.ltY = pRect->lty;
    rect.rbX = pRect->rbx;
    rect.rbY = pRect->rby;
    
    IVTBL(update_mono_lcd_screen)(&rect);
}



static void lcd_common_mono_update(PlatformRect *pRect, u8 *buffer){
    int page;
    int width = pRect->rbx + 1;

    
    
	for(page = 0; page < (pRect->rby+1+7)/8; page++)
	{  
	#if 0
        // set page
        lcd_bus_write_cmd(0xB0|page);
        // set start column
        lcd_bus_write_cmd(0x00 /*+ col%16*/);
        lcd_bus_write_cmd(0x10 /*+ col/16*/);
        #endif
        //lcd_bus_write_cmd(0x00|(((u8)lcd_x_offset)&0x0F));
        //lcd_bus_write_cmd(0x10|((((u8)lcd_x_offset)&0xF0)>>4));
        //lcd_bus_write_cmd(0x00);
        //lcd_bus_write_cmd(0x10);
        lcd_bus_write_cmd(0x00 + lcd_x_offset%16);
        lcd_bus_write_cmd(0x10 + lcd_x_offset/16);
        lcd_bus_write_cmd(0xB0|page);
        lcd_bus_write_cmd(0x40|lcd_y_offset);

        lcd_bus_write_data_buf(&buffer[page*width], width);
	}
}

static void lcd_bus_i2c_write_cmd(uint16 cmd){
    platform_i2c_send_data( lcd_bus_itf.bus_i2c.bus_id, 
                            lcd_bus_itf.bus_i2c.slave_addr, 
                            (const u8 *)&lcd_bus_itf.bus_i2c.cmd_addr, 
                            (const u8 *)&cmd, 
                            1);
}

static void lcd_bus_i2c_write_data(uint16 data){
    platform_i2c_send_data( lcd_bus_itf.bus_i2c.bus_id, 
                            lcd_bus_itf.bus_i2c.slave_addr, 
                            (const u8 *)&lcd_bus_itf.bus_i2c.data_addr, 
                            (const u8 *)&data, 
                            1);
}

static void lcd_bus_i2c_write_data_buf(uint8 *buf, uint32 len){
    platform_i2c_send_data( lcd_bus_itf.bus_i2c.bus_id, 
                            lcd_bus_itf.bus_i2c.slave_addr, 
                            (const u8 *)&lcd_bus_itf.bus_i2c.data_addr, 
                            buf, 
                            len);
}

static void lcd_bus_spi_write_cmd(uint16 cmd){
    IVTBL(set_gpio)(lcd_bus_itf.bus_spi.pin_rs, 0);
    IVTBL(write_spi)(OPENAT_SPI_1, (const UINT8 *)&cmd, 1);
}

static void lcd_bus_spi_write_data(uint16 data){
    IVTBL(set_gpio)(lcd_bus_itf.bus_spi.pin_rs, 1);
    IVTBL(write_spi)(OPENAT_SPI_1, (const UINT8 *)&data, 1);
}

static void lcd_bus_spi_write_data_buf(uint8 *buf, uint32 len){
    IVTBL(set_gpio)(lcd_bus_itf.bus_spi.pin_rs, 1);
    IVTBL(write_spi)(OPENAT_SPI_1, buf, len);
}

#ifdef AIR202_LUA_UI_SPC
 void lcd_bus_spc_spi_write_data_buf(uint8 *buf, uint32 len){
    IVTBL(write_spi)(OPENAT_SPI_1, buf, len);
}
#endif

static void lcd_msg_callback(T_AMOPENAT_LCD_MESSAGE *pMsg)
{
    if(OPENAT_DRV_EVT_LCD_REFRESH_REQ == pMsg->evtId)
    {
        PlatformRect rect; 
        PlatformRect roiRect = *(PlatformRect*)&pMsg->param.refreshReq.rect;
        rect.ltx = 0;
        rect.lty = 0;
        rect.rbx = roiRect.rbx - roiRect.ltx;
        rect.rby = roiRect.rby - roiRect.lty;
        lcd_bus_color_update_layer(&rect, &roiRect);
    }
}

static void lcd_bus_spi4line_init(const PlatformDispInitParam *pParam){
    if(lcd_bpp !=1){
        T_AMOPENAT_COLOR_LCD_PARAM param;
        
        param.width = pParam->width;
        param.height = pParam->height;
#ifdef AM_CAMERA_SUPPORT
        param.msgCallback = lcd_msg_callback;
#else
        param.msgCallback = NULL;
#endif
        param.bus = OPENAT_LCD_SPI4LINE;
        param.lcdItf.spi.frequence = pParam->lcd_itf.bus_spi.freq;   
        param.lcdItf.spi.csPort = platform_pio_get_gpio_port(pParam->pin_cs);
        param.lcdItf.spi.rstPort = platform_pio_get_gpio_port(pParam->pin_rst);

        IVTBL(init_color_lcd)(&param);

#ifdef AM_LAYER_SUPPORT
        lcd_bus_update_layer = lcd_bus_color_update_layer;
#endif

        lcd_bus_update = lcd_bus_color_update;
        lcd_bus_write_cmd = (lcd_cmd_data_op)IVTBL(send_color_lcd_command);
        lcd_bus_write_data = (lcd_cmd_data_op)IVTBL(send_color_lcd_data);
    } else {
        T_AMOPENAT_MONO_LCD_PARAM lcdParam;
        
        lcdParam.height = pParam->height;
        lcdParam.width = pParam->width;
        lcdParam.xoffset = pParam->x_offset;
        lcdParam.yoffset = pParam->y_offset;
        lcdParam.frequence = pParam->lcd_itf.bus_spi.freq;   
        lcdParam.fameBuffer = pParam->framebuffer;
        lcdParam.pixelBits = 1;
        lcdParam.csPort = platform_pio_get_gpio_port(pParam->pin_cs);
        lcdParam.rstPort = platform_pio_get_gpio_port(pParam->pin_rst);;
        
        IVTBL(init_mono_lcd)(&lcdParam);

        lcd_bus_update = lcd_bus_mono_update;
        lcd_bus_write_cmd = (lcd_cmd_data_op)IVTBL(send_mono_lcd_command);
        lcd_bus_write_data = (lcd_cmd_data_op)IVTBL(send_mono_lcd_data);
    }
}

static void lcd_bus_parallel_init(const PlatformDispInitParam *pParam){
    T_AMOPENAT_COLOR_LCD_PARAM param;
    
    ASSERT(lcd_bpp == 16);
    
    param.width = pParam->width;
    param.height = pParam->height;
    param.msgCallback = NULL;
    param.bus = OPENAT_LCD_PARALLEL_8800;
    param.lcdItf.parallel.csPort = platform_pio_get_gpio_port(pParam->pin_cs);
    param.lcdItf.parallel.rstPort = platform_pio_get_gpio_port(pParam->pin_rst);
    
    IVTBL(init_color_lcd)(&param);
    
    lcd_bus_update = lcd_bus_color_update;
    lcd_bus_write_cmd = (lcd_cmd_data_op)IVTBL(send_color_lcd_command);
    lcd_bus_write_data = (lcd_cmd_data_op)IVTBL(send_color_lcd_data);
}

static void lcd_common_reset(E_AMOPENAT_GPIO_PORT pin_rst){
    T_AMOPENAT_GPIO_CFG cfg;
    
    cfg.mode = OPENAT_GPIO_OUTPUT;
    cfg.param.defaultState = 1;
    IVTBL(config_gpio)(pin_rst, &cfg);
    
    delayMilliseconds(5);
    IVTBL(set_gpio)(pin_rst, 0);
    delayMilliseconds(5);
    IVTBL(set_gpio)(pin_rst, 1);
    delayMilliseconds(50);
}

static void lcd_bus_i2c_init(const PlatformDispInitParam *pParam){
    PlatformI2CParam i2c_param;
    
    ASSERT(lcd_bpp == 1);

    memcpy(&lcd_bus_itf, &pParam->lcd_itf, sizeof(lcd_itf_t));
    
    i2c_param.speed = pParam->lcd_itf.bus_i2c.freq;
    i2c_param.slaveAddr = pParam->lcd_itf.bus_i2c.slave_addr;
    
    platform_i2c_setup(pParam->lcd_itf.bus_i2c.bus_id, &i2c_param);
    
    lcd_common_reset(platform_pio_get_gpio_port(pParam->pin_rst));
    
    lcd_bus_update = lcd_common_mono_update;
    lcd_bus_write_cmd = lcd_bus_i2c_write_cmd;
    lcd_bus_write_data = lcd_bus_i2c_write_data;
    lcd_bus_write_data_buf = lcd_bus_i2c_write_data_buf;
}


#ifdef AIR202_LUA_UI_SPC
static void lcd_spc_bus_spi_init(const PlatformDispInitParam *pParam){
    T_AMOPENAT_SPI_PARAM spiParam;
    

    memcpy(&lcd_bus_itf, &pParam->lcd_itf, sizeof(lcd_itf_t));
    lcd_bus_itf.bus_spi.pin_rs = platform_pio_get_gpio_port(pParam->pin_rs);
    
    spiParam.fullDuplex = FALSE;
    spiParam.cpol = TRUE;
    spiParam.cpha = 0;
    spiParam.dataBits = 8;
    spiParam.clock = pParam->lcd_itf.bus_spi.freq;
    spiParam.withCS = TRUE;
    
    IVTBL(config_spi)(OPENAT_SPI_1, &spiParam);
           
    lcd_bus_write_cmd = lcd_bus_spi_write_cmd;
    lcd_bus_write_data = lcd_bus_spi_write_data;
    lcd_bus_write_data_buf = lcd_bus_spi_write_data_buf;
}
#else
static void lcd_bus_spi_init(const PlatformDispInitParam *pParam){
    T_AMOPENAT_SPI_PARAM spiParam;
    
    ASSERT(lcd_bpp == 1 || lcd_bpp == 16);
    
    memcpy(&lcd_bus_itf, &pParam->lcd_itf, sizeof(lcd_itf_t));
    lcd_bus_itf.bus_spi.pin_rs = platform_pio_get_gpio_port(pParam->pin_rs);
    
    spiParam.fullDuplex = FALSE;
    spiParam.cpol = TRUE;
    spiParam.cpha = 0;
    spiParam.dataBits = 8;
    spiParam.clock = pParam->lcd_itf.bus_spi.freq;
    spiParam.withCS = TRUE;

    IVTBL(config_spi)(OPENAT_SPI_1, &spiParam);

    {
        T_AMOPENAT_GPIO_CFG cfg;
        
        cfg.mode = OPENAT_GPIO_OUTPUT;
        cfg.param.defaultState = 0;
        IVTBL(config_gpio)(platform_pio_get_gpio_port(pParam->pin_rs), &cfg);
    }
    
    lcd_common_reset(platform_pio_get_gpio_port(pParam->pin_rst));
    
    if(lcd_bpp == 1)
    {
        lcd_bus_update = lcd_common_mono_update;
    }
    else
    {
        lcd_bus_update = lcd_spi_color_update;
    }
        
    lcd_bus_write_cmd = lcd_bus_spi_write_cmd;
    lcd_bus_write_data = lcd_bus_spi_write_data;
    lcd_bus_write_data_buf = lcd_bus_spi_write_data_buf;
}
#endif

static void write_command_table(const UINT32 *table, UINT16 size)
{
    UINT16 flag;
    UINT16 value;
    UINT16 index;

    for(index = 0; index < size && table[index] != (UINT32)-1; index++)
    {
        flag = table[index]>>16;
        value = table[index]&0xffff;

        switch(flag)
        {
            case 1:
                delayMilliseconds(value);
                break;

            case 0:
            case 2:
                lcd_bus_write_cmd(value&0x00ff);
                break;

            case 3:
                lcd_bus_write_data(value&0x00ff);
                break;

            default:
                ASSERT(0);
                break;
        }
    }
}

#ifdef AIR202_LUA_UI_SPC
void platform_spc_lcd_init(const PlatformDispInitParam *pParam)
{
    lcd_bpp = pParam->bpp;
    lcd_x_offset = pParam->x_offset;
    lcd_y_offset = pParam->y_offset;

    lcd_bus = pParam->bus;
    
    IVTBL(send_event)(0xd1a1daaa);
    IVTBL(send_event)(pParam->lcd_itf.bus_spi.freq);

    lcd_init_table[lcd_bus](pParam);
}
#else

void platform_lcd_init(const PlatformDispInitParam *pParam)
{
    lcd_bpp = pParam->bpp;
    lcd_x_offset = pParam->x_offset;
    lcd_y_offset = pParam->y_offset;

    IVTBL(send_event)(0xd1a1daaa);
    IVTBL(send_event)(pParam->lcd_itf.bus_spi.freq);
    
/*+\NEW\liweiqiang\2013.12.18\增加lcd睡眠命令支持 */
    lcd_sleep_cmd = malloc(sizeof(UINT32)*(pParam->sleepCmdSize+1));
    memcpy(lcd_sleep_cmd, pParam->pLcdSleepCmd, sizeof(UINT32)*pParam->sleepCmdSize);
    lcd_sleep_cmd[pParam->sleepCmdSize] = -1;
    
    lcd_wake_cmd = malloc(sizeof(UINT32)*(pParam->wakeCmdSize+1));
    memcpy(lcd_wake_cmd, pParam->pLcdWakeCmd, sizeof(UINT32)*pParam->wakeCmdSize);
    lcd_wake_cmd[pParam->wakeCmdSize] = -1;
/*-\NEW\liweiqiang\2013.12.18\增加lcd睡眠命令支持 */

/*+\NEW\liweiqiang\2017.12.8\增加lcd RASET/CASET/GRAMWRITE 命令支持 */
    if(pParam->addressCmdSize > 0){
        lcd_address_cmd = malloc(sizeof(UINT32)*(pParam->addressCmdSize+1));
        memcpy(lcd_address_cmd, pParam->pLcdAddressCmd, sizeof(UINT32)*pParam->addressCmdSize);
        lcd_address_cmd[pParam->addressCmdSize] = -1;  
    }
/*-\NEW\liweiqiang\2017.12.8\增加lcd RASET/CASET/GRAMWRITE 命令支持 */

    lcd_bus = pParam->bus;

    lcd_init_table[lcd_bus](pParam);

/*+\new\liweiqiang\2014.10.27\修正黑白屏开机花屏问题 */
    if(lcd_bpp == 1){
        write_command_table(pParam->pLcdCmdTable, pParam->tableSize);
    } else {
        delayMilliseconds(50);
        
        write_command_table(pParam->pLcdCmdTable, pParam->tableSize);
        
        delayMilliseconds(100);
    }
/*-\new\liweiqiang\2014.10.27\修正黑白屏开机花屏问题 */
}

#ifdef AM_LAYER_SUPPORT
void platform_lcd_update(PlatformRect *pRect, PlatformRect *pRoiRect)
{
    lcd_bus_update_layer(pRect, pRoiRect);
    //lcd_bus_update(pRect, buffer);

/*+\new\liweiqiang\2014.10.27\修正黑白屏开机花屏问题 */
    if(lcd_first_update){
        lcd_first_update = 0;

        if(lcd_bpp == 1){
            lcd_bus_write_cmd(0xAF);
        }
    }
/*-\new\liweiqiang\2014.10.27\修正黑白屏开机花屏问题 */
}
#else
void platform_lcd_update(PlatformRect *pRect, u8 *buffer)
{
    lcd_bus_update(pRect, buffer);

/*+\new\liweiqiang\2014.10.27\修正黑白屏开机花屏问题 */
    if(lcd_first_update){
        lcd_first_update = 0;

        if(lcd_bpp == 1){
            lcd_bus_write_cmd(0xAF);
        }
    }
/*-\new\liweiqiang\2014.10.27\修正黑白屏开机花屏问题 */
}
#endif

/*+\NEW\liweiqiang\2013.12.18\增加lcd睡眠命令支持 */
void platform_lcd_powersave(int sleep_wake)
{
    if(sleep_wake)
    {
        if(lcd_sleep_cmd)
        {
            write_command_table(lcd_sleep_cmd, (UINT16)-1);
        }
    }
    else
    {
        if(lcd_wake_cmd)
        {
            write_command_table(lcd_wake_cmd, (UINT16)-1);
        }
    }
}
/*-\NEW\liweiqiang\2013.12.18\增加lcd睡眠命令支持 */
#endif



#ifdef AM_LAYER_SUPPORT
BOOL platform_create_layer(  int layer_id,  
                                    int format ,  
                                    int left   ,  
                                    int top    ,  
                                    int right  ,  
                                    int bottom)
{
    T_AMOPENAT_LCD_RECT_T rect;

    if(layer_id < 0 || layer_id >= OPENAT_LAYER_COUNT)
    {
        return FALSE;
    }

    if(format != OPENAT_LAYER_FORMAT_RGB565 && format != OPENAT_LAYER_FORMAT_RGBA)
    {
        return FALSE;
    }

    if(left >= right || top >= bottom)
    {
        return FALSE;
        
    }

    rect.ltX = left;
    rect.ltY = top;
    rect.rbX = right;
    rect.rbY = bottom;
    return IVTBL(create_layer)(layer_id, format, &rect);
}



void platform_destory_layer(  u32 layer_id)
{
    IVTBL(destory_layer)(layer_id);
}


u8* platform_set_active_layer(u32 layer_id)
{
    return IVTBL(set_active_layer)(layer_id);
}

BOOL  platform_active_layer_is_RGBA(VOID)
{
    E_OPENAT_LCD_LAYER_INFO* info = IVTBL(get_layer_info)(IVTBL(get_active_layer)());

    return info->format == LAYER_FORMAT_RGBA;
}

void  platform_get_active_layer_size(u16* width, u16*height)
{
    E_OPENAT_LCD_LAYER_INFO* info = IVTBL(get_layer_info)(IVTBL(get_active_layer)());
    if(width)
        *width   = info->rect.rbX - info->rect.ltX + 1;
    if(height) 
        *height  = info->rect.rbY - info->rect.ltY + 1;
}

int  platform_get_active_layer_format(void)
{
    E_OPENAT_LCD_LAYER_INFO* info = IVTBL(get_layer_info)(IVTBL(get_active_layer)());

    return info->format;
}



UINT32 platform_get_active_layer(void)
{
    return IVTBL(get_active_layer)();
}



E_OPENAT_LCD_LAYER_INFO* platform_get_layer_info(u32 layer_id)
{
    return IVTBL(get_layer_info)(layer_id);
}


void platform_set_layer_start_pos(u32 layer_id, u32 startX,  u32 startY)
{
     IVTBL(set_layer_start_pos)(layer_id, startX, startY);
}


void platform_set_layer_visible(BOOL layer_visible[LAYER_ID_MAX] )
{
    IVTBL(set_layer_visible)(layer_visible);
}

UINT8* platform_set_layer_format(u32 layer_id, int format)
{
   return IVTBL(set_layer_format)(layer_id, format);
}
#endif
#endif

