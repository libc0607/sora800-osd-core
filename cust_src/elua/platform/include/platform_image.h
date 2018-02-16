

#ifndef _PLATFORM_IMAGE_H_
#define _PLATFORM_IMAGE_H_

#include "type.h"

typedef struct image_tag
{
     u16 width;
     u16 height;
     u8  format;
     u16*             buffer;     
} image_t;

int platform_image_decode_jpeg(const char *filename,image_t *image_info);
int platform_free_image_jpeg_decodedata(u16 *buf);

BOOL platform_image_decode_png(u8* buffer,int size, u16 x, u16 y, u16 left, u16 top, u16 right, u16 bottom);

#endif
