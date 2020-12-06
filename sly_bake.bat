@echo off

set SLY_USRDIR=D:\Emulators\rpcs3\dev_hdd0\game\NPUA80663\USRDIR
set SLY_TOOLS=D:\Nikos\Reversing\Sly\Code\SlyCooper\cmake-build-relwithdebinfo
set SLY1_ROOT=%SLY_USRDIR%\Sly1
set SLY1_DATA=%SLY1_ROOT%\extracted
set PSARC="D:\Nikos\Reversing\Tools\PS3 Tools\psarc.exe"

REM Replace 'y' below with nothing to disable compressing the level below
set ENABLE_COMPRESS_LEVEL=y
set LEVEL_PATH=D:\Nikos\Reversing\Sly\Code\sly_playground\tests\jb_intro_W.dec
set LEVEL_NAME_FINAL=jb_intro_W

if defined ENABLE_COMPRESS_LEVEL (
    %SLY_TOOLS%\sly_compressor.exe "%LEVEL_DIR%\%LEVEL_NAME%" "%SLY1_DATA%\%LEVEL_NAME_FINAL%"
)

%SLY_TOOLS%\sly_packer.exe "%SLY1_DATA%" %SLY1_ROOT%

%PSARC% create -y --output=%SLY_USRDIR%\Sly1.psarc --inputfile=%SLY_USRDIR%\Sly1_filelist.txt