/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    platform_stubs.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2012/11/29
 *
 * Description:
 *   ʵ��newlib/stubs.c����Ҫƽ̨֧�ֵ�һЩstdio.c�Ľӿ�
 **************************************************************************/

#include "../../newlib/libc/include/stdio.h" //��ҪFILE���Ͷ���
#include "assert.h"

static char iobuf_temp[2048];

extern int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);

int platform_vfprintf(FILE *fp, const char *fmt, va_list ap)
{
    int len;

    len = vsnprintf(iobuf_temp, sizeof(iobuf_temp), fmt, ap);

    ASSERT(len < sizeof(iobuf_temp));

    return (*fp->_write)(fp->_cookie, iobuf_temp, len);
}

