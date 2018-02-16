#ifdef AM_CAMERA_SUPPORT
#include "string.h"
#include "malloc.h"

#include "rda_pal.h"
#include "cycle_queue.h"
#include "assert.h"

#include "platform.h"
#include "platform_conf.h"
#include "platform_rtos.h"


#define CAM_BY3A01_ID   0x01


const AMOPENAT_CAMERA_REG cameraInitReg [] =
{
//bf3a01 initial
//initial black
{0xfe, 0x00},
{0x3d, 0x00},
{0x30, 0x3b},
{0x31, 0x3b},
{0x34, 0x01},
{0x35, 0x0a},
             
{0xfe, 0x01},
{0xe0, 0xba},//VCLK=48M//9a
{0xe1, 0x03},
{0xe2, 0x05},
{0xe3, 0x42},//bit[1]=1,bypass PLL
{0xe4, 0x22},
{0xe5, 0x03},//add lzq
             
{0xe7, 0x05},
             
//{0x51, 0x00, 0x01},//VSYNC:active low  HSYNC:active high
{0x50, 0x00},//SP mode
{0x66, 0x01},
{0x52, 0xf1},
{0x53, 0x20},//10
{0x5d, 0x00},
             
{0xfe, 0x00},
{0x00, 0x4b},//bit[5]:1,mirror;bit[4]:1,flip;
{0x02, 0x10},//dummy pixel
{0x15, 0x0a},
{0x3c, 0x9d},//0x9a bit[3:0]:black lock 20140918
{0x41, 0x02},// anti blc vibration
{0x3e, 0x68},
{0x0f, 0x13},
	           
//initial AWB
{0xfe, 0x00},
{0xa0, 0x54},
{0xb0, 0x19},//blue gain
{0xb1, 0x2d},//red gain
             
//initial AE 
{0xfe, 0x01},
{0x00, 0x08},
{0x0e, 0x03},
{0x0f, 0x30},//{0x0e,0x0f}:exposure time
{0x10, 0x18},//global gain
             
{0xfe, 0x00},
{0x84, 0xa2},//denoise; bit[7:4],the larger the smaller noise
{0x82, 0x08},//denoise; The larger the smaller noise;
{0x86, 0x23},//bit[6:4]:bright edge enhancement;bit[2:0]:dark edge enhancement;
             
//gamma tongtouxing hao
{0xfe, 0x00},
{0x60, 0x25},
{0x61, 0x2a},
{0x62, 0x28},
{0x63, 0x28},
{0x64, 0x20},
{0x65, 0x1d},
{0x66, 0x17},
{0x67, 0x15},
{0x68, 0x0f},
{0x69, 0x0e},
{0x6a, 0x0a},
{0x6b, 0x06},
{0x6c, 0x05},
{0x6d, 0x04},
{0x6e, 0x02},
             
{0x72, 0x0c},//gamma offset
{0x73, 0x0c},//gamma offset
{0x74, 0x44},//bit[7:4] and bit[3:0]ֵԽ������Խ��
	           
/*//gamma default
{0xfe, 0x00},
{0x60, 0x38},
{0x61, 0x30},
{0x62, 0x24},
{0x63, 0x1f},
{0x64, 0x1c},
{0x65, 0x16},
{0x66, 0x12},
{0x67, 0x0f},
{0x68, 0x0d},
{0x69, 0x0c},
{0x6a, 0x0b},
{0x6b, 0x09},
{0x6c, 0x09},
{0x6d, 0x08},
{0x6e, 0x07},
             
//gamma low noise
{0xfe, 0x00},
{0x60, 0x24},
{0x61, 0x30},
{0x62, 0x24},
{0x63, 0x1d},
{0x64, 0x1a},
{0x65, 0x14},
{0x66, 0x11},
{0x67, 0x0f},
{0x68, 0x0e},
{0x69, 0x0d},
{0x6a, 0x0c},
{0x6b, 0x0b},
{0x6c, 0x0a},
{0x6d, 0x09},
{0x6e, 0x09},*/
             
//outdoor color
{0xfe, 0x00},
{0xc7, 0x21},
{0xc8, 0x19},
{0xc9, 0x84},
{0xca, 0x64},
{0xcb, 0x89},
{0xcc, 0x3f},
{0xcd, 0x16},
             
//indoor color default
{0xfe, 0x00},
{0xc0, 0x05},
{0xc1, 0x07},
{0xc2, 0x30},
{0xc3, 0x28},
{0xc4, 0x3c},
{0xc5, 0x10},
{0xc6, 0x96},
             
/*//indoor color vivid green OK
{0xfe, 0x00},
{0xc0, 0x26},
{0xc1, 0x1b},
{0xc2, 0x5e},
{0xc3, 0x58},
{0xc4, 0x7d},
{0xc5, 0x2b},
{0xc6, 0x96},  */
             
/*//indoor color vivid blue OK
{0xfe, 0x00},
{0xc0, 0x39},
{0xc1, 0x31},
{0xc2, 0x76},
{0xc3, 0x64},
{0xc4, 0x57},
{0xc5, 0x07},
{0xc6, 0x96},
*/           
             
//AWB        
{0xfe, 0x00},
{0xb2, 0x01},
{0xb3, 0x11},//green gain
{0xa2, 0x11},//low limit of blue gain in indoor scene //0x18 20140918
{0xa3, 0x36},//high limit of blue gain in indoor scene
{0xa4, 0x11},//low limit of red gain in indoor scene
{0xa5, 0x36},//high limit of red gain in indoor scene
{0xa7, 0x80},//blue target
{0xa8, 0x7f},//red target
{0xa9, 0x15},
{0xaa, 0x10},
{0xab, 0x10},
{0xac, 0x2c},
{0xad, 0xf0},
{0xae, 0x20},//0x80->0x20 Ycbcr LIMIT
{0xb4, 0x18},//low limit of blue gain in outdoor scene
{0xb5, 0x1a},//high limit of blue gain in outdoor scene
{0xb6, 0x1c},//low limit of red gain in outdoor scene
{0xb7, 0x30},//high limit of red gain in outdoor scene
{0xd0, 0x4c},
             
//AE         
{0xfe, 0x01},
{0x04, 0x4f},//AE target
{0x09, 0x0d},//bit[5:0]:max integration time step
{0x0a, 0x45},
{0x0b, 0x82},//minimum integration time
{0x0c, 0x2e},//50hz banding
{0x0d, 0x26},//60hz banding
{0x15, 0x42},
{0x17, 0xb5},
{0x18, 0x28},//The value should be smaller than the 50hz banding. add 20140702
{0x1b, 0x28},//minimum global gain
{0x1c, 0x50},
{0x1d, 0x39},
{0x1e, 0x5d},
{0x1f, 0x77},//max global gain
             
{0xfe, 0x00},
{0xce, 0x48},//Contrast
             
//saturation
{0xfe, 0x01},
{0x64, 0xc0},//blue saturation
{0x65, 0xb0},//red saturation
             
{0xfe, 0x01}, 
{0x59, 0x00},//bit[3:0]: skip frame counter 0, don't skip frame,else skip as many as FRAME_CNT_REG frames
//delay 400ms 
{0xfe, 0x00},
{0xfe, 0x00},
{0x3d, 0xff},
{0xa0, 0x55},
{0xfe, 0x01},
{0x00, 0x05},


	
};



BOOL platform_camera_poweron(BOOL video_mode)
{
    T_AMOPENAT_CAMERA_PARAM initParam =
    {
        NULL,
        OPENAT_I2C_2, 
        0x6e,
        AMOPENAT_CAMERA_REG_ADDR_8BITS|AMOPENAT_CAMERA_REG_DATA_8BITS,
        
        TRUE,
        FALSE,
        FALSE, 
        
        240,
        320,
        
        CAMERA_IMAGE_FORMAT_YUV422,
        cameraInitReg,
        sizeof(cameraInitReg)/sizeof(AMOPENAT_CAMERA_REG),
        {0xfd, CAM_BY3A01_ID}
    };

    IVTBL(camera_init)(&initParam);
    
    return IVTBL(camera_poweron)(video_mode);
}


BOOL platform_camera_poweroff(void)
{
    return IVTBL(camera_poweroff)();
}


BOOL platform_camera_preview_open(u16 offsetx, u16 offsety, u16 startx, u16 starty, u16 endx, u16 endy)
{
    T_AMOPENAT_CAM_PREVIEW_PARAM previewParam;
    previewParam.startX = startx;
    previewParam.startY = starty;
    previewParam.endX = endx;
    previewParam.endY = endy;
    previewParam.offsetX = offsetx;
    previewParam.offsetY = offsety;
    previewParam.recordAudio = FALSE;
    
    return IVTBL(camera_preview_open)(&previewParam);
}



BOOL platform_camera_preview_close(void)
{
    return IVTBL(camera_preview_close)();
}


BOOL platform_camera_capture(u16 width, u16 height)
{
    T_AMOPENAT_CAM_CAPTURE_PARAM captureParam;
    captureParam.imageWidth = width;
    captureParam.imageHeight = height;
    
    return IVTBL(camera_capture)(&captureParam);
}


BOOL platform_camera_save_photo(const char* filename)
{
    int length;
    UINT16 *unicode_path;
    char* actname;
    int fb;
    BOOL result;
    if((find_dm_entry( filename, &actname ) ) == -1 )
    {
        return -1;
    }
    


    IVTBL(delete_file)(actname);
    fb = IVTBL(create_file)(actname, NULL);


    result = IVTBL(camera_save_photo)(fb );
    IVTBL(close_file)(fb);
    return result;
}
#endif
