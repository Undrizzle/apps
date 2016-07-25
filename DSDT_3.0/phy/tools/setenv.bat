@echo off
set REQ_OS=%1

if "%REQ_OS%"==""        set OS_RUN=VXWORKS
if "%REQ_OS%"=="VXWORKS" set OS_RUN=VXWORKS
if "%REQ_OS%"=="vxworks" set OS_RUN=VXWORKS
if "%REQ_OS%"=="vxWorks" set OS_RUN=VXWORKS

set DSP_VCT=FALSE
set VCT_PENDING=TRUE
set MAD_USER_BASE=\Tornado_ARM\target\config\88E6401\mad
set MAD_PROJ_NAME=madDrv
set MAD_TOOL_DIR=%MAD_USER_BASE%\tools
set USER_API_NAME=
set USER_NAME=
set RELEASE=YES
set TARGET_CPU=ARM
rem set TARGET_CPU=MIPS

if "%OS_RUN%"=="VXWORKS" goto VXWORKS_ENV

echo Unknown Target OS!
echo Supported Target OS is vxworks and wince.
echo Assumes VxWorks as a Target OS.

:VXWORKS_ENV

set TOR_VER=2.1
set ENDIAN=EL
set WIND_BASE=C:\Tornado_ARM
rem set WIND_BASE=C:\Tornado2.1
set WIND_HOST_TYPE=x86-win32

set PATH=%WIND_BASE%\host\%WIND_HOST_TYPE%\bin;%PATH%

echo Environment Variable for VxWorks has been set.

goto DONE

:DONE
cd %MAD_USER_BASE%\src

