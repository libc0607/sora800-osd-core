
LOCAL_MODULE_DEPENDS  += cust_src/elua/lua
LOCAL_MODULE_DEPENDS  += cust_src/elua/newlib
LOCAL_MODULE_DEPENDS  += cust_src/elua/newlib/libc
LOCAL_MODULE_DEPENDS  += cust_src/elua/shell
LOCAL_MODULE_DEPENDS  += cust_src/elua/modules
LOCAL_MODULE_DEPENDS  += cust_src/elua/platform
LOCAL_MODULE_DEPENDS  += cust_src/elua/platform/coolsand

#+\NEW\liweiqiang\2013.5.11\�����Խ�ѹluazipĿ¼���ļ�֧��,ѹ���㷨lzma
ifeq ($(strip $(AM_LZMA_SUPPORT)), TRUE)
LOCAL_MODULE_DEPENDS  += cust_src/elua/lib/lzma
endif
#-\NEW\liweiqiang\2013.5.11\�����Խ�ѹluazipĿ¼���ļ�֧��,ѹ���㷨lzma

#+\NEW\liweiqiang\2013.7.16\����iconv�ַ�����ת���� 
LOCAL_MODULE_DEPENDS  += cust_src/elua/lib/iconv
#-\NEW\liweiqiang\2013.7.16\����iconv�ַ�����ת���� 

#+\NEW\zhutianhua\2014.1.21\���zlib��
ifeq ($(strip $(AM_ZLIB_SUPPORT)), TRUE)
LOCAL_MODULE_DEPENDS  += cust_src/elua/lib/zlib
LOCAL_MODULE_DEPENDS  += cust_src/elua/lib/zlib/zlib_pal
endif
#-\NEW\zhutianhua\2014.1.21\���zlib��

#+\NEW\zhutianhua\2014.1.24\���libpng��
ifeq ($(strip $(AM_LPNG_SUPPORT)), TRUE)
LOCAL_MODULE_DEPENDS  += cust_src/elua/lib/lpng
LOCAL_MODULE_DEPENDS  += cust_src/elua/lib/lpng/lpng_pal
endif
#-\NEW\zhutianhua\2014.1.24\���libpng��

LOCAL_MODULE_DEPENDS  += cust_src/elua/lib/json
#/*begin\NEW\zhutianhua\2017.4.17 15:7\����crypto�㷨��*/
LOCAL_MODULE_DEPENDS  += cust_src/elua/lib/crypto
#/*end\NEW\zhutianhua\2017.4.17 15:7\����crypto�㷨��*/

#/*begin\NEW\lijiaodi\2017.6.30 15:7\����qr_encode��*/
LOCAL_MODULE_DEPENDS  += cust_src/elua/lib/qr_encode
#/*end\NEW\lijiaodi\2017.6.30 15:7\����qr_encode��*/
