# ------------------------------------------------------------------------ #
#                             AirM2M Ltd.                                  # 
#                                                                          #
# Name: version.mk                                                         #
#                                                                          #
# Author: liweiqiang                                                            #
# Verison: V0.1                                                            #
# Date: 2013.3.4                                                         #
#                                                                          #
# File Description:                                                        #
#                                                                          #
#  �汾�����ļ�                                                            #
# ------------------------------------------------------------------------ #

# �汾���ɹ��� Luat_V$(BUILD_VER)_$(HW_VER)[_$(FUNCTION)]

#BUILD��
BUILD_VER =0019
#Ӳ���汾��
HW_VER = 8955
#��ѡ���ܣ�����TTS�ȣ������ð汾�����ض�����
OPTION_FUN = TTS_UI

### ��Ҫ���õ����� ###
# ģ��/�ֻ���Ŀ��
MODULE_TYPE=A6390

# �ͻ�ͬһ��Ŀ��ͬӲ���汾��ͬһ��Ŀ��ͬӦ��
CUST_HW_TYPE=H


# ����汾��
ifeq "${BUILD_VER}" ""
${error MUST define BUILD_VER}
else
SW_SN=${BUILD_VER}
endif

# ģ��/�ֻ������
MODULE_HW_TYPE=13

# ƽ̨����汾��
PLATFORM_VER=CT8955



# ------------------------------------------------------------------------ #
# �汾�Ŷ���
# ------------------------------------------------------------------------ #
# �ڲ��汾��
IN_VER=SW_$(MODULE_TYPE)_$(CUST_HW_TYPE)_V$(SW_SN)_M$(MODULE_HW_TYPE)_$(PLATFORM_VER)_$(HW_VER)

# �ⲿ�汾�ţ�Ĭ�϶��壩
ifneq "${OPTION_FUN}"  ""
EX_VER=Luat_V$(SW_SN)_$(HW_VER)_$(OPTION_FUN)
else
EX_VER=Luat_V$(SW_SN)_$(HW_VER)
endif
# ------------------------------------------------------------------------ #
# �汾��
# ------------------------------------------------------------------------ #
LOCAL_EXPORT_FLAG += \
   IN_VER=\"$(IN_VER)\" \
   EX_VER=\"$(EX_VER)\" \
   
ifeq "${AM_VER_ECHO_SUPPORT}" "TRUE"
ECHO_EX_VER:
	@echo $(EX_VER)
endif
