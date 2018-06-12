@echo off

if "%1"=="" title iCatch date stamp font generation process

REM ------------------------------------------------------------------------------------------------
REM Select the font height for date stamp or add new one below
REM
REM %V% Enable
REM %X% Disable
REM ------------------------------------------------------------------------------------------------

set V=call :REG_DS_H
set X=REM
set "DS_LIST_HEIGHT="

%X% 12
%X% 16
%X% 24
%X% 28
%X% 32
%V% 48
%X% 64

REM ------------------------------------------------------------------------------------------------
REM Define the font color for date stamp
REM
REM The color format is "R(8bit)<<16 | G(8bit)<<8 | B(8bit)<<0", 
REN     e.g. The color yellow should be "0xffff00"
REM 
REM Notes:
REM 1) The available value for R/G/B is (0x11~0xff)
REM 2) The font stroke color fixed at 0x101010 (Black)
REM 
REM ------------------------------------------------------------------------------------------------

set /a FONT_COLOR=0xffffff

REM ------------------------------------------------------------------------------------------------
REM LZO compress support for "SFN" and "SST" files
REM 
REM 1: Enable
REM 0: Disable
REM ------------------------------------------------------------------------------------------------

set /a RES_LZO_SUPPORT=0

REM ------------------------------------------------------------------------------------------------
REM User setting report
REM ------------------------------------------------------------------------------------------------

if "%1"=="" cls
echo ^|
echo ^| USER SETTING REPORT
echo ^|

set /a DS_HEIGHT_CNT=0
for %%a in (%DS_LIST_HEIGHT%) do (
	set /a DS_HEIGHT_CNT+=1
)
echo ^> The date stamp height below will be installed:
for %%a in (%DS_LIST_HEIGHT%) do (
	echo ^    %%a
)

echo ^> LZO compress support:
if %RES_LZO_SUPPORT% gtr 0 (
	set "LZO_COMPRESS=call :LZO_HDL"
	echo ^    Enable
) else (
	set "LZO_COMPRESS=REM"
	echo ^    Disable
)

REM ------------------------------------------------------------------------------------------------
REM Environment configration
REM ------------------------------------------------------------------------------------------------

set /a CLR_TMP_FILE=1

set FILE_FONT_GEN=..\tool\EZGUI\IMU_font.exe
set FILE_FONT_SFN=..\tool\EZGUI\IMU_font2SFN.exe
set FILE_ICAT_LZO=..\tool\EZGUI\iCatLzo.exe


set FILE_DS_BASE_NAME=DATESTAMP

set FILE_PRJ_DEF=..\lastprj.def

if not "%1"=="" pushd %1

if not exist %FILE_PRJ_DEF% (
	echo Please configure your project firstly
	if not "%1"=="" popd
	pause
	goto :eof
)

for /f "tokens=3,* delims== " %%a in (%FILE_PRJ_DEF%) do (
	set PRJ_DEF=%%a
	goto done
)
:done

set PATH_MENU_FONT=..\resource\%PRJ_DEF%\A\RO_RES\UI\FONT
set PATH_MENU_FONT_DS=%PATH_MENU_FONT%\DATESTAMP
set FILE_PART_IMG_A=..\_%PRJ_DEF%\download\AIMG
set FILE_PART_IMG_B=..\_%PRJ_DEF%\download\BIMG

set PATH_UI_DL=..\_%PRJ_DEF%\download\A\RO_RES\UI
set PATH_FONT_SFN_DL=%PATH_UI_DL%\FONT

if not exist %PATH_FONT_SFN_DL% md %PATH_FONT_SFN_DL%

REM ------------------------------------------------------------------------------------------------
REM Generate SFN file for date stamp font
REM ------------------------------------------------------------------------------------------------

rd /s /q %PATH_MENU_FONT_DS% 1>NUL 2>NUL

for %%a in (%DS_LIST_HEIGHT%) do (
	if not exist %PATH_MENU_FONT_DS%\%%a md %PATH_MENU_FONT_DS%\%%a
	
	echo Generate BMP files for "Date Stamp" with the font height of %%a...
	%FILE_FONT_GEN% %PATH_MENU_FONT%\DS_UNICODE.TXT %PATH_MENU_FONT_DS%\%%a %%a %FONT_COLOR%
	
	echo Generate the font SFN file "%FILE_DS_BASE_NAME%_%%a.SFN" with the height of %%a...
	%FILE_FONT_SFN% %PATH_MENU_FONT_DS%\%%a %PATH_MENU_FONT%\%FILE_DS_BASE_NAME%_%%a.SFN %ROT_OPT%
	
	%LZO_COMPRESS% %PATH_MENU_FONT%\%FILE_DS_BASE_NAME%_%%a.SFN
	echo copy %PATH_MENU_FONT%\%FILE_DS_BASE_NAME%_%%a.SFN %PATH_FONT_SFN_DL%\
	copy /y %PATH_MENU_FONT%\%FILE_DS_BASE_NAME%_%%a.SFN %PATH_FONT_SFN_DL%\ 1>NUL
)

REM ------------------------------------------------------------------------------------------------
REM Main process over
REM ------------------------------------------------------------------------------------------------
REM ------------------------------------------------------------------------------------------------
REM Clear temp files
REM ------------------------------------------------------------------------------------------------

if %CLR_TMP_FILE% gtr 0 (
	rd /s /q %PATH_MENU_FONT_DS% 1>NUL 2>NUL
)

if not "%1"=="" popd

goto :eof

REM ------------------------------------------------------------------------------------------------
REM Local functions
REM ------------------------------------------------------------------------------------------------

:REG_DS_H
set "DS_LIST_HEIGHT=%DS_LIST_HEIGHT% %1"
goto :eof

:LZO_HDL
echo LZO compress file "%1"
%FILE_ICAT_LZO% -c %1 %1.LZO
move /y %1.LZO %1
goto :eof
