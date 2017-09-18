@echo off
set ROOT=%~pd0
set OPENSSL_SOURCEDIR=%ROOT%openssl-1.0.??
set OPENSSL_INSTALDIR=%ROOT%openssl
set MAKEFLAGS=

if not "%1" == "" goto help

call :testroot %ROOT%
if errorlevel 1 goto error0

nmake -? >NUL 2>&1
if errorlevel 1 goto error1

ml64 -? >NUL 2>&1
if errorlevel 1 goto error1

perl -v >NUL 2>&1
if errorlevel 1 goto error2

cd /D %OPENSSL_SOURCEDIR% >NUL 2>&1
if errorlevel 1 goto error3

perl Configure VC-WIN64A no-ec --prefix=%OPENSSL_INSTALDIR%
if errorlevel 1 goto error

sed -i "/perl/ s/\\\\/\\//g" ms/do_win64a.bat 
call ms\do_win64a
if errorlevel 1 goto error

sed -i "/PERL/ s/\\\\/\\//g; /PERL/ s/\\$@/\$(@:\\\\=\\/)/;" ms/nt*.mak
nmake -f ms\nt.mak clean

nmake -f ms\nt.mak
if errorlevel 1 goto error

nmake -f ms\nt.mak install
if errorlevel 1 goto error

cd ..\Stack
if errorlevel 1 goto error

nmake -f win32_msc.mak clean
if errorlevel 1 goto error

nmake -f win32_msc.mak
if errorlevel 1 goto error

cd ..
RMDIR /S /Q WindowsFile
MKDIR WindowsFile
COPY Stack\core\*.h WindowsFile\
COPY Stack\platforms\win32\*.h WindowsFile\
COPY Stack\proxystub\clientproxy\*.h WindowsFile\
COPY Stack\proxystub\serverstub\*.h WindowsFile\
COPY Stack\securechannel\*.h WindowsFile\
COPY Stack\stackcore\*.h WindowsFile\
COPY Stack\transport\https\*.h WindowsFile\
COPY Stack\transport\tcp\*.h WindowsFile\
COPY openssl\lib\* lib\win32\
cd /D %OPENSSL_SOURCEDIR%
COPY tmp32\lib.pdb ..\lib\win32\
goto done

:testroot
if "%1" == "%ROOT%" exit /B 0
exit /B 1

:error0
echo fatal error: cannot continue.
echo the directory %ROOT% must not contain blanks
goto done

:error1
echo fatal error: cannot continue.
echo this batch has to be called from a
echo 64bit visual studio command shell
goto done

:error2
echo fatal error: cannot continue.
echo perl has to be in the path
goto done

:error3
echo fatal error: cannot continue.
echo openssl sources must be at %OPENSSL_SOURCEDIR%
goto done

:error
echo fatal error: cannot continue.

:help
echo this batch has to be called from a
echo 64bit visual studio command shell
echo the directory %ROOT% must not contain blanks
echo openssl sources must be at %OPENSSL_SOURCEDIR%
echo perl has to be in the path

:done