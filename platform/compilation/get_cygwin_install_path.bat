
::+\BUG WM-499\lifei\2012.12.27\OpenAT�ͻ��޷���ȡcygwin��װ·��::
::+\BUG WM-491\lifei\2012.12.26\����ʱ�Զ���ȡrevision���::

::����ע�����Ϣ��ȡcygwin�İ�װĿ¼

::Windows 2000            �汾 5.0
::Windows XP              �汾 5.1
::Windows Server 2003     �汾 5.2
::Windows Server 2003 R2  �汾 5.2
::Windows Vista           �汾 6.0
::Windows Server 2008     �汾 6.0
::Windows Server 2008 R2  �汾 6.1
::Windows 7               �汾 6.1

::ver|find /i "5.1" >nul && goto :WinXP
::ver|find /i "5.2" >nul && goto :WinServer2003
::ver|find /i "6.0" >nul && goto :WinVista
::ver|find /i "6.1" >nul && goto :Win7
::default
::goto :WinXP

::��ȡCygwin��װ·��
echo Find Cygwin install path @WinXP
for /f "skip=4 tokens=1-3" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Cygnus Solutions\Cygwin\mounts v2\/" /v native') do (
  set "CYGWIN_HOME=%%c"
)
if "%CYGWIN_HOME%"=="" (goto Win7) else (goto FindCygwinPathEnd)

:Win7
echo Find Cygwin install path @Win7
for /f "skip=2 tokens=1-3" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Cygnus Solutions\Cygwin\mounts v2\/" /v native') do (
  set "CYGWIN_HOME=%%c"
)
if "%CYGWIN_HOME%"=="" (goto WinServer2003) else (goto FindCygwinPathEnd)

:WinServer2003
echo Find Cygwin install path @WinServer2003
for /f "skip=1 tokens=1-3" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Cygnus Solutions\Cygwin\mounts v2\/" /v native') do (
  set "CYGWIN_HOME=%%c"
)

:FindCygwinPathEnd
if "%CYGWIN_HOME%"=="" (echo Cannot find Cygwin install path! && pause)
if "%TORTOISESVN_HOME%"=="" (
    if exist "%ROOT_DIR%/env/compilation/get_tortoisesvn_install_path.bat" (call %ROOT_DIR%/env/compilation/get_tortoisesvn_install_path.bat)
)

::-\BUG WM-491\lifei\2012.12.26\����ʱ�Զ���ȡrevision���::
::-\BUG WM-499\lifei\2012.12.27\OpenAT�ͻ��޷���ȡcygwin��װ·��::
