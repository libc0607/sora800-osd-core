::********************************************************#
:: Copyright (C), AirM2M Tech. Co., Ltd.
:: Author: lifei
:: Description: AMOPENAT ����ƽ̨
:: Others:
:: History: 
::   Version�� Date:       Author:   Modification:
::   V0.1      2012.12.14  lifei     �����ļ�
::********************************************************#
@echo off

::������Ŀ¼�л�����bat�ļ����ڵ�Ŀ¼
PUSHD %~dp0

::+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
:: ��ȡ��ص�·����Ϣ
::+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
::��ȡ��Ŀ����Ŀ¼·��
set PROJ_BUILD_DIR=%cd%

::��ȡ��Ŀ·��
cd ..
set PROJ_DIR=%cd%
:: ��ȡ��Ŀ����
for /f %%i in ("%cd%") do set PROJ_NAME=%%~ni
echo PROJ_NAME=%PROJ_NAME%

::��ȡ�����Ŀ¼
cd ../..
set ROOT_DIR=%cd%

::�ص���Ŀ����Ŀ¼
cd %PROJ_BUILD_DIR%

::+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
:: ��ȡCygwin��װ·��
::+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
if "%CYGWIN_HOME%"=="" (call %ROOT_DIR%/platform/compilation/get_cygwin_install_path.bat)
if "%CYGWIN_HOME%"=="" (goto ErrorNoCygwin)

::+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
:: ���û���·��
::+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
:BuildStart
echo Cygwin install path: %CYGWIN_HOME%
::��Cygwin��bin�Լ�cross-compile����Ŀ¼�ӵ�PATH��
set PATH=%CYGWIN_HOME%\bin;/usr/bin;/crosscompiler/bin;C:\CSDTK\cooltools;

::+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
:: ����bash��ʼ����
::+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bash %ROOT_DIR%/cust_build.sh 1 2 clean
echo.
goto End


:ErrorNoCygwin
::echo Can't find Cygwin install path!
goto End

:End

@echo on