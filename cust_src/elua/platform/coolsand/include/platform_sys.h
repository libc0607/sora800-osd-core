/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    platform_sys.h
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/5/10
 *
 * Description:
 *   ʵ��newlib����Ҫƽ̨֧�ֵ�һЩsystem �ӿ�
 **************************************************************************/

#ifndef _PLATFORM_SYS_H_
#define _PLATFORM_SYS_H_

int platform_sys_unlink(const char *path);
void platform_decode(unsigned int* data, int len);


#endif/*_PLATFORM_SYS_H_*/

