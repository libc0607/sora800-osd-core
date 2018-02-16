::����ע�����Ϣ��ȡTortoiseSVN�İ�װĿ¼

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
:WinXP
echo Find TortoiseSVN install path @WinXP
for /f "skip=4 tokens=2*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\TortoiseSVN" /v Directory') do (
  set "TORTOISESVN_HOME=%%b"
)
goto FindTortoiseSVNPathEnd

:Win7
echo Find TortoiseSVN install path @Win7
for /f "skip=2 tokens=2*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\TortoiseSVN" /v Directory') do (
  set "TORTOISESVN_HOME=%%b"
)
goto FindTortoiseSVNPathEnd

:WinServer2003
echo Find TortoiseSVN install path @WinServer2003
for /f "skip=1 tokens=2*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\TortoiseSVN" /v Directory') do (
  set "TORTOISESVN_HOME=%%b"
)
goto FindTortoiseSVNPathEnd

:FindTortoiseSVNPathEnd
if "%TORTOISESVN_HOME%"=="" (echo Cannot find TortoisSVN install path! && exit)
