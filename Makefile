#********************************************************#
# Copyright (C), AirM2M Tech. Co., Ltd.
# Author: lifei
# Description: AMOPENAT ����ƽ̨
# Others:
# History: 
#   Version�� Date:       Author:   Modification:
#   V0.1      2012.12.14  lifei     �����ļ�
#********************************************************#

#-----------------------------------
#
# �����Makefile
#
#-----------------------------------

#������Ҫ�����ģ���б�
include module_list.mk

# Set this to any non-null string to signal a module which
# generates a binary (must contain a "main" entry point). 
# If left null, only a library will be generated.
IS_ENTRY_POINT := yes

# Assembly / C code
S_SRC := 
C_SRC := 

include ${SOFT_WORKDIR}/platform/compilation/cust_rules.mk
