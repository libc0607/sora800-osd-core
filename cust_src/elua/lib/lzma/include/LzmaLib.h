/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    lzmalib.h
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2013/5/10
 *
 * Description:
 *          lzmaѹ���ļ��ӿ�,target�˽�����ѹ��Դ��
 **************************************************************************/

#ifndef _LZMA_LIB_H_
#define _LZMA_LIB_H_

/*+\NEW\2013.7.11\liweiqiang\����lzma��ѹbuf���ļ��Ľӿ�*/
int LzmaDecodeBufToFile(const unsigned char *inbuff, const unsigned int inlen,
                        const char *outfile);
/*-\NEW\2013.7.11\liweiqiang\����lzma��ѹbuf���ļ��Ľӿ�*/

int LzmaUncompressFile(const char *infile, const char *outfile);

#endif/*_LZMA_LIB_H_*/

