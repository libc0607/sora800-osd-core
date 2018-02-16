/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    fcntl.h
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2012/11/27
 *
 * Description:
 * 
 **************************************************************************/

#ifndef __FCNTL_H__
#define __FCNTL_H__

#define O_RDONLY (1<<3)
#define O_WRONLY (1<<4)
#define O_RDWR (O_RDONLY|O_WRONLY)

#ifndef O_CREAT
#define O_CREAT (1<<2)  
#endif

#ifndef O_EXCL
#define O_EXCL (1<<6)  
#endif

#ifndef O_TRUNC
#define O_TRUNC (1<<1)  
#endif

#ifndef O_APPEND
#define O_APPEND (1<<0)
#endif

#endif //__FCNTL_H__