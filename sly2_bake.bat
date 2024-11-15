@echo off

set SLY2_USRDIR=D:\Emulators\rpcs3\dev_hdd0\game\NPEA00342\USRDIR
set SLY_TOOLS=D:\Nikos\Reversing\Sly\Code\SlyTools\cmake-build-relwithdebinfo
set SLY_TOOLS_REL=D:\Nikos\Reversing\Sly\Code\SlyTools\cmake-build-release
set SLY2_ROOT=%SLY2_USRDIR%\Sly2
REM set SLY2_DATA=C:\sly2_testing\extracted
REM set SLY2_DATA=%SLY2_ROOT%\extracted
set SLY2_DATA=C:\Users\Nikos\Downloads

REM Replace 'y' below with nothing to disable compressing the level below
set ENABLE_COMPRESS_LEVEL=y
REM set LEVEL_PATH=D:\Emulators\rpcs3\dev_hdd0\game\NPEA00342\USRDIR\Sly2\levels_decompressed\f_nightclub_exterior.slyZ.dec
set LEVEL_PATH=C:\Users\Nikos\Downloads\f_nightclub_exterior.slyZ.dec
set LEVEL_NAME_FINAL=f_nightclub_exterior.slyZ

if defined ENABLE_COMPRESS_LEVEL (
    echo --- Compressing level %LEVEL_NAME_FINAL% ---
    %SLY_TOOLS_REL%\sly_compressor.exe %LEVEL_PATH% %SLY2_DATA%\%LEVEL_NAME_FINAL%
)

echo --- Packing to .WAC/.WAL ---
REM %SLY_TOOLS%\sly_2_3_packer_.exe "%SLY2_DATA%" %SLY2_ROOT%\SLY2.WAL
%SLY_TOOLS%\sly_2_3_packer_fast.exe %SLY2_DATA%\%LEVEL_NAME_FINAL% %SLY2_ROOT%\SLY2.WAL

del %SLY2_DATA%\%LEVEL_NAME_FINAL%
REM del %SLY2_DATA%\%LEVEL_NAME_FINAL%.dec
