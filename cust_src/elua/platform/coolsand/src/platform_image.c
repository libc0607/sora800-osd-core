/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    platform_audio.c
 * Author:  lijiaodi
 * Version: V0.1
 * Date:    2017/06/29
 *
 * Description:
 *          Í¼Æ¬½âÂë½Ó¿Ú
 **************************************************************************/

#include "rda_pal.h"
#include "string.h"

#include "platform.h"
#include "platform_malloc.h"
#include "platform_image.h"


BOOL platform_image_decode_png(u8* buffer,int size, u16 x, u16 y, u16 left, u16 top, u16 right, u16 bottom)
{

   IVTBL(print)("platform_image_decode_png ");

   return IVTBL(imgs_decode_png)(buffer, size, x, y, left, top, right, bottom);
}

int platform_image_decode_jpeg(const char *filename,image_t *image_info)
{
    int ret;
    T_AMOPENAT_IMAGE_INFO opat_imageinfo;

    char* actname;
    
        
    if((find_dm_entry( filename, &actname ) ) == -1 )
    {
        return -1;
    }
    

    IVTBL(print)("platform_image_decode %s", actname);

    ret = IVTBL(imgs_decode_jpeg)(actname, &opat_imageinfo);

    if(ret == 0)
    {
        image_info->width = opat_imageinfo.width;
        image_info->height= opat_imageinfo.height;
        image_info->format= opat_imageinfo.format;
        image_info->buffer= opat_imageinfo.buffer;
        IVTBL(print)("platform_image_decode %d %d %d ", image_info->width, image_info->height, image_info->format);
    }

    
    return ret;
}

int platform_free_image_jpeg_decodedata(u16 *buf)
{
    int ret;

    ret = IVTBL(imgs_free_jpeg_decodedata)(buf);

    IVTBL(print)("platform_free_image_decodedata ret:%d\n",ret);
    return ret;
}

