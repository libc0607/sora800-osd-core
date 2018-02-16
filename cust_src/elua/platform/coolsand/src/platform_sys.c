/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    platform_sys.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/5/10
 *
 * Description:
 *   ʵ��newlib����Ҫƽ̨֧�ֵ�һЩsystem �ӿ�
 **************************************************************************/

#include <string.h>
#include "rda_pal.h"


int platform_sys_unlink(const char *path)
{
    return IVTBL(delete_file)(path);
}

int platform_sys_file_rename(const char *old, const char* new)
{
    return IVTBL(rename_file)(old, new);
}

void platform_decode(unsigned int* data, int len)
{
    IVTBL(decode)((UINT32*)data, len);
}

