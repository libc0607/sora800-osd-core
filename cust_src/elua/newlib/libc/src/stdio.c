/**************************************************************************
 *              Copyright (C), AirM2M Tech. Co., Ltd.
 *
 * Name:    stdio.c
 * Author:  liweiqiang
 * Version: V0.1
 * Date:    2012/9/15
 *
 * Description:
 * 
 **************************************************************************/

#include "stdio.h"
#include "string.h"
#include "fcntl.h"
#include "errno.h"
#include "assert.h"
#include "local.h"
#include "reent.h"

#define	POS_ERR	(-(fpos_t)1)

int
lualibc_printf(const char *fmt, ...)
{
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = _vfprintf_r(stdout, fmt, ap);
    va_end(ap);

    return ret;
}

int
lualibc_putc(int c, FILE *fp)
{
    return (*fp->_write)(fp->_cookie, (const char *)&c, 1);
}

int
lualibc_fprintf(FILE *fp, const char *fmt, ...)
{
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = _vfprintf_r(fp, fmt, ap);
    va_end(ap);

    return ret;
}

int
lualibc_fputs(const char *s, FILE *fp)
{
    return (*fp->_write)(fp->_cookie, s, strlen(s));
}

char *lualibc_fgets(char *buf, int n, FILE *fp)
{
    int character = 0;
    int idx = 0;
    
    if((NULL == buf) || (n <= 1) || (NULL == fp))
    {
        return NULL;
    }

    /*+\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    #ifdef AM_LUA_UNCOMPRESS_SCRIPT_TABLE_ACESS_SUPPORT
    if(LUA_UNCOMPRESS_FILE & fp->_type)
    {
        return fgets_ext(buf,n,fp);
    }
    #endif
    /*-\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/

    character = getc(fp);

    if(EOF == character)
    {
        return NULL;
    }
    
    while(EOF != character)
    {
        if(idx >= (n-1))
        {
            break;
        }

        buf[idx] = character;
        if(0x0A == character)
        {
            buf[idx+1] = 0;
            break;
        }
        
        idx++;
        character = getc(fp);
    }

    if(EOF == character)
    {
        buf[idx] = 0;
    }
    
    buf[n-1] = 0;
    return buf;
}

int
lualibc_fflush(FILE *fp)
{   
    return 0;
}

/*
 * Return the (stdio) flags for a given mode.  Store the flags
 * to be passed to an open() syscall through *optr.
 * Return 0 on error.
 */
int __sflags(const char *mode, int *optr)
{
	int ret, m, o;

	switch (*mode++) {

	case 'r':	/* open for reading */
		ret = __SRD;
		m = O_RDONLY;
		o = 0;
		break;

	case 'w':	/* open for writing */
		ret = __SWR;
		m = O_WRONLY;
		o = O_CREAT | O_TRUNC;
		break;

	case 'a':	/* open for appending */
		ret = __SWR;
		m = O_WRONLY;
		o = O_CREAT | O_APPEND;
		break;

	default:	/* illegal mode */
		errno = EINVAL;
		return (0);
	}

	/* [rwa]\+ or [rwa]b\+ means read and write */
	if (*mode == '+' || (*mode == 'b' && mode[1] == '+')) {
		ret = __SRW;
		m = O_RDWR;
	}
	*optr = m | o;
	return (ret);
}

FILE *lualibc_fopen(const char *file, const char *mode)
{
    int f;
    FILE *fp;
    int flags, oflags;
    int fileNameLen = strlen(file);
    /*+\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    #ifdef AM_LUA_UNCOMPRESS_SCRIPT_TABLE_ACESS_SUPPORT
    FILE *pFile = fopen_ext(file, mode);
    if((pFile != NULL) && (pFile->_flags == 0)) /*LUA_SCRIPT_TABLE_UPDATE_SECTION*/
    {
        return pFile;
    }
    #endif
    /*-\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
        
    if((flags = __sflags(mode, &oflags)) == 0)
        return (NULL);

    if((fp = __sfp()) == NULL)
        return (NULL);

    if((f = _open_r(file, oflags, 0)) < 0){
        fp->_flags = 0;         /* release */
        /*+\NEW\zhuth\2014.8.14\��������ѹ���ɹ���ɾ������������������*/
        #ifdef AM_LUA_UNCOMPRESS_SCRIPT_TABLE_ACESS_SUPPORT
        return pFile;
        #else
        return (NULL);
        #endif
        /*-\NEW\zhuth\2014.8.14\��������ѹ���ɹ���ɾ������������������*/
    }

    fp->_file = f;
    fp->_flags = flags;
    fp->_cookie = fp;
    fp->_read = __sread;
    fp->_write = __swrite;
    fp->_seek = __sseek;
    fp->_close = __sclose;

    // ƽ̨���ļ�ϵͳ����������
    //if(oflags & O_APPEND)
    //    (void)__sseek((void *)fp, (fpos_t)0, SEEK_END);

    printf("%s %s\r\n", __FUNCTION__, file);

    /*+\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    #ifdef AM_LUA_UNCOMPRESS_SCRIPT_TABLE_ACESS_SUPPORT
	//ǿ���ü����ļ�
#ifdef AM_LUA_CRYPTO_SUPPORT
    if(strncmp(&file[fileNameLen - 5],".luae", 5) == 0)
    {
        //�ߵ����� �����ļ�        
        fp->_type = ENC_FILE;
    }
    else
#endif
    {
        fp->_type = COMMON_FILE;
    } 
    #endif
    /*-\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/

    return fp;
}

int lualibc_fclose(FILE *fp)
{
    int r = 0;

    /*+\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    #ifdef AM_LUA_UNCOMPRESS_SCRIPT_TABLE_ACESS_SUPPORT
    if(LUA_UNCOMPRESS_FILE & fp->_type)
    {
        return fclose_ext(fp);
    }
    #endif
    /*-\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    
    if(fp->_flags == 0){    /* not open! */
        errno = EBADF;
        return (EOF);
    }

    //r = fp->_flags & __SWR ? __sflush(fp) : 0; ���ڲ���Ҫ��������ֱ�ӷ���0
    
    if(fp->_close != NULL && (*fp->_close)(fp->_cookie)){
        r = EOF;
    }
    
    fp->_flags = 0;         /* release */
    
    return r;
}

FILE *lualibc_freopen(const char *file, const char *mode, FILE *fp)
{
    errno = ENOTSUP;
    
    return (FILE*)(0);
}

int lualibc_getc(FILE *fp)
{
    char c;

    /*+\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    #ifdef AM_LUA_UNCOMPRESS_SCRIPT_TABLE_ACESS_SUPPORT
    if(LUA_UNCOMPRESS_FILE & fp->_type)
    {
        return getc_ext(fp);
    }
    #endif
    /*-\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    
    if(fread((void *)&c, 1, 1, fp) != 1){
        return (EOF);
    }

    return c;
}

int lualibc_ungetc(int c, FILE *fp)
{
    // ����û��ʵ��������stdio ���ݲ�֧��stdin ungetc
    ASSERT(fp != stdin);

    /*+\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    #ifdef AM_LUA_UNCOMPRESS_SCRIPT_TABLE_ACESS_SUPPORT
    if(LUA_UNCOMPRESS_FILE & fp->_type)
    {
        return ungetc_ext(c,fp);
    }
    #endif
    /*-\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/

    fseek(fp, -1, SEEK_CUR);

    return 0;
}


#define DEC_BUFF_SIZE 512

//#define CRYPTO_DEBUG

size_t lualibc_fread(void *buf, size_t size, size_t count, FILE *fp)
{
    size_t resid;
    int len;
	
    unsigned int* data = NULL;
    unsigned char* temp = buf;
    unsigned int offset  = ftell(fp);
    unsigned int act_low_boundary = (offset & 0xFFFFFE00); /*��512����Ķ�ȡ�ļ�����ʼλ��*/
    unsigned int read_count;        /*��Ҫ���ļ��ж�ȡ�ĳ���*/
    unsigned int act_up_boundary;   /*��512����Ķ�ȡ�ļ��Ľ���λ��*/
    unsigned int act_count;         /*��ȡ������Ч���ݳ���*/

    /*+\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    #ifdef AM_LUA_UNCOMPRESS_SCRIPT_TABLE_ACESS_SUPPORT
    if(LUA_UNCOMPRESS_FILE & fp->_type)
    {
        return fread_ext(buf,size,count,fp);
    }
    #endif
    /*-\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/

    if((resid = count * size) == 0)
        return (0);

#ifdef AM_LUA_CRYPTO_SUPPORT
    if(ENC_FILE == fp->_type)
    {
        act_up_boundary = ((offset + resid + DEC_BUFF_SIZE - 1) & 0xFFFFFE00);
        //���±���512B���䣬��ȡ�ĳ���>=Ҫ��ĳ���
        read_count = act_up_boundary - act_low_boundary; 

        /*������8���ֽڵ��ڴ棬�Ա�֤��4�ֽڶ���*/
        data = (unsigned int*)malloc(4 + read_count + 4);
        
        /*��֤4�ֽڶ���*/
        temp = (unsigned char*)((((unsigned int)data + 3) >> 2) << 2);

        /*���ļ�ָ���Ƶ���512�����λ��*/
        lualibc_fseek(fp, act_low_boundary, SEEK_SET);
        
    }
    else
#endif

    {
        read_count = resid;
    }

    len = (*fp->_read)(fp->_cookie, temp, read_count);

    if(len <= 0){
        if(len == 0)
            fp->_flags |= __SEOF;
        else
            fp->_flags |= __SERR;

#ifdef AM_LUA_CRYPTO_SUPPORT
        if(ENC_FILE == fp->_type)
        {
            free(data);
        }
#endif

        return (0);
    }
    else
    {
#ifdef AM_LUA_CRYPTO_SUPPORT
        if(ENC_FILE == fp->_type)
        {
            int decCount = len / DEC_BUFF_SIZE;
            int i = 0;
            int real_size;
            
            act_count = resid;

            /*���û�ж����㹻������ݣ���ζ�ſ쵽�ļ���ĩβ��*/
            if(read_count > len)
            {
                lualibc_fseek(fp, 0, SEEK_END);
                real_size = lualibc_ftell(fp);

                if(real_size - offset < resid)
                {
                    //�ļ����ܶ�ȡ����󳤶�
                    act_count = real_size - offset;
                }
            }
            
            /*���ļ�ָ���Ƶ���ʵ��λ��*/
            lualibc_fseek(fp, offset + act_count, SEEK_SET);

#ifdef CRYPTO_DEBUG
            printf("liulean info  %d %d %d %d %d %d\r\n", 
                act_low_boundary, 
                act_up_boundary, 
                offset + count,
                offset - act_low_boundary, //��߶�ȡ�Ķ��೤��
                count,
                decCount);
#endif

            while(i < decCount)
            {
                platform_decode((unsigned int*)(temp + DEC_BUFF_SIZE * i), -((DEC_BUFF_SIZE) / 4));
                i++;
            }

            platform_decode((unsigned int*)(temp + DEC_BUFF_SIZE * i), -((len % DEC_BUFF_SIZE) / 4));
            
            memcpy(buf, &temp[offset - act_low_boundary], act_count);
            free(data);
        }
        else
#endif
        {
            act_count = len;
        }
    }
    
    return (act_count / size);
}

size_t lualibc_fwrite(const void *buf, size_t size, size_t count, FILE *fp)
{
    size_t len;
    int w;
    
    if(cantwrite(fp)){
        errno = EBADF;
        return (EOF);
    }

    len = size*count;

    w = (*fp->_write)(fp->_cookie, buf, len);

    if(w <= 0){
        fp->_flags |= __SERR;
        return (EOF);
    }
    
    return w/size;
}

int lualibc_fseek(FILE *fp, long offset, int whence)
{
    /*+\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    #ifdef AM_LUA_UNCOMPRESS_SCRIPT_TABLE_ACESS_SUPPORT
    if(LUA_UNCOMPRESS_FILE & fp->_type)
    {
        return fseek_ext(fp,offset,whence);
    }
    #endif
    /*-\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    if(fp->_seek == NULL){
        errno = ESPIPE;
        return (EOF);
    }

    if((*fp->_seek)(fp->_cookie, offset, whence) == POS_ERR){
        fp->_flags &= ~__SEOF;
        return (EOF);
    }

    return (0);
}

FILE *lualibc_tmpfile(void)
{
    errno = ENOTSUP;
    
    return (FILE*)0;
}

int lualibc_fscanf(FILE *fp, const char *fmt, ...)
{
    errno = ENOTSUP;
    
    return (-1);
}

long lualibc_ftell(FILE *fp)
{
	fpos_t pos;

    /*+\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/
    #ifdef AM_LUA_UNCOMPRESS_SCRIPT_TABLE_ACESS_SUPPORT
    if(LUA_UNCOMPRESS_FILE & fp->_type)
    {
        return ftell_ext(fp);
    }
    #endif
    /*-\NEW\zhuth\2014.3.2\ͨ���ļ���¼�����luadb��δѹ�����ļ�*/

	if (fp->_seek == NULL) {
		errno = ESPIPE;			/* historic practice */
		return ((off_t)-1);
	}

	if (fp->_flags & __SOFF)
		pos = fp->_offset;
	else {
		pos = (*fp->_seek)(fp->_cookie, (fpos_t)0, SEEK_CUR);
	}

    return pos;
}

int lualibc_setvbuf(FILE *fp, char *buf, int mode, size_t size)
{
    errno = ENOTSUP;
    
    return (-1);
}

/*+\NEW\liweiqiang\2013.5.11\����remove�ӿ�*/
int lualibc_remove(const char *filename)
{
    if(_unlink_r(filename) == -1)
        return -1;

    return 0;
}
/*-\NEW\liweiqiang\2013.5.11\����remove�ӿ�*/

int lualibc_rename(const char *old, const char *new)
{
    if(_rename_r(old, new) < 0)
        return -1;

    return 0;
}

char* lualibc_tmpnam(char *s)
{
    errno = ENOSYS;
    return NULL;
}

/*
 * Small standard I/O/seek/close functions.
 * These maintain the `known seek offset' for seek optimisation.
 */
int
__sread(void *cookie, char *buf, int n)
{
	FILE *fp = cookie;
	int ret;
	
	ret = _read_r(fp->_file, buf, n);
	/* if the read succeeded, update the current offset */
	if (ret >= 0)
		fp->_offset += ret;
	else
		fp->_flags &= ~__SOFF;	/* paranoia */
	return (ret);
}

int
__swrite(void *cookie, const char *buf, int n)
{
	FILE *fp = cookie;

	if (fp->_flags & O_APPEND)
		(void) _lseek_r(fp->_file, (off_t)0, SEEK_END);
	fp->_flags &= ~__SOFF;	/* in case FAPPEND mode is set */
	return (_write_r(fp->_file, buf, n));
}

fpos_t
__sseek(void *cookie, fpos_t offset, int whence)
{
	FILE *fp = cookie;
	off_t ret;
	
	ret = _lseek_r(fp->_file, (off_t)offset, whence);
	if (ret == (off_t)-1)
		fp->_flags &= ~__SOFF;
	else {
		fp->_flags |= __SOFF;
		fp->_offset = ret;
	}
	return (ret);
}

int
__sclose(void *cookie)
{
	return (_close_r(((FILE *)cookie)->_file));
}

