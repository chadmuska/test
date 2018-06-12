@echo off

REM ------------------------------------------------------------------------------------------------
REM generate host svn ver
REM ------------------------------------------------------------------------------------------------
REM call .\ver_gen.bat

if "%1"=="" title iCatch UI Utility Process

REM ------------------------------------------------------------------------------------------------
REM Select the font height for osd or add new one below
REM
REM %V% Enable
REM %X% Disable
REM ------------------------------------------------------------------------------------------------

set V=call :REG_FONT_H
set X=REM
set "FONT_LIST_HEIGHT="

%X% 16
%X% 22
%X% 28
%V% 32
%V% 42

REM ------------------------------------------------------------------------------------------------
REM Select the icon height or add new one below
REM
REM !!NOTE!!: If host add new different height which is NOT in the below list,
REM           Please also update the folder path for "IMU_DPD_FILES" in hostfw Makefile.
REM
REM %V% Enable
REM %X% Disable
REM ------------------------------------------------------------------------------------------------

set V=call :REG_ICON_H
set X=REM
set "ICON_LIST_HEIGHT="

%V% 32
%V% 48

REM ------------------------------------------------------------------------------------------------
REM Select the multiple language
REM
REM %V% Enable
REM %X% Disable
REM ------------------------------------------------------------------------------------------------

set V=call :REG_LANG
set X=REM
set "MULTI_LANGUAGE="

%V% ENGLISH
%V% SCHINESE
%V% TCHINESE
%X% GERMAN
%X% ITALIAN
%X% SPANISH
%X% PORTUGUESE
%X% DUTCH
%X% VIETNAMESE
%X% FRENCH
%X% GREEK
%X% THAI
%X% CZECH
%X% HUNGARIAN
%X% POLISH
%X% TURKISH
%X% RUSSIAN
%X% SWEDISH
%X% ROMANIAN
%X% CROATION
%X% MALAY
%X% ARABIC
%X% JAPANESE
%X% KOREAN
%X% HINDI
%X% MARATHI
%X% URDU
%X% BENGALI
%X% PUNJABI
%X% TELEGU
%X% ORIYA
%X% NORWEGIAN
%X% FINNISH
%X% DANISH

REM ------------------------------------------------------------------------------------------------
REM resource compress support for "SFN" and "SST" files
REM
REM 1: Enable
REM 0: Disable
REM ------------------------------------------------------------------------------------------------

set /a RES_COMPRESS_SUPPORT=1

if %RES_COMPRESS_SUPPORT% EQU 1 (
REM	set COMPRESS_CHOICE=LZO
	set COMPRESS_CHOICE=SNAPPY
)

REM ------------------------------------------------------------------------------------------------
REM Panel rotation support
REM
REM 1: Enable
REM 0: Disable
REM ------------------------------------------------------------------------------------------------

set /a PNL_ROT_SUPPORT=0

REM ------------------------------------------------------------------------------------------------
REM User setting report
REM ------------------------------------------------------------------------------------------------

echo ^|
echo ^| IMU USER SETTING REPORT
echo ^|

set /a FONT_HEIGHT_CNT=0
for %%a in (%FONT_LIST_HEIGHT%) do (
	set /a FONT_HEIGHT_CNT+=1
)
echo ^> The font height below will be installed:
for %%a in (%FONT_LIST_HEIGHT%) do (
	echo ^    %%a
)

set /a ICON_HEIGHT_CNT=0
for %%a in (%ICON_LIST_HEIGHT%) do (
	set /a ICON_HEIGHT_CNT+=1
)
echo ^> The icon height below will be installed:
for %%a in (%ICON_LIST_HEIGHT%) do (
	echo ^    %%a
)

echo ^> The language(s) below will be installed:
for %%a in (%MULTI_LANGUAGE%) do (
	echo ^    %%a
)

echo ^> resource compress support:

if %RES_COMPRESS_SUPPORT% gtr 0 (
REM	setlocal EnableDelayedExpansion
REM	call .\GetBaseVer.cmd
REM	if !base_ver! lss 16834 (
REM		echo *** warning *** Current base f/w version: %base_ver%
REM		echo *** warning *** Please update to 16834 or later to support this feature
REM		set /a RES_COMPRESS_SUPPORT=0
REM	)
REM	setlocal DisableDelayedExpansion
)
if %RES_COMPRESS_SUPPORT% gtr 0 (
	set "RES_COMPRESS=call :RES_COM_HDL"
	echo ^    Enable
) else (
	set "RES_COMPRESS=REM"
	echo ^    Disable
)

echo ^> Panel rotation support:
if %PNL_ROT_SUPPORT% gtr 0 (
	set "ROT_OPT=1"
	echo ^    Enable
) else (
	set "ROT_OPT="
	echo ^    Disable
)

REM ------------------------------------------------------------------------------------------------
REM Environment configration
REM ------------------------------------------------------------------------------------------------

set FILE_PRJ_DEF=..\lastprj.def

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

set PRJ_DEF_LOWER=%PRJ_DEF%
::echo lower:%PRJ_DEF_LOWER% UPPER:%PRJ_DEF%
set "lower=a b c d e f g h i j k l m n o p q r s t u v w x y z"
for %%i in (%lower%) do call set PRJ_DEF_LOWER=%%prj_def_lower:%%i=%%i%%

set /a CLR_TMP_FILE=1

set FILE_MENU_CVT=..\tool\EZGUI\IMU_menu.exe
set FILE_ICON_GEN=..\tool\EZGUI\IMU_icon2SFN.exe
set FILE_FONT_GEN=..\tool\EZGUI\IMU_font.exe
set FILE_FONT_SFN=..\tool\EZGUI\IMU_font2SFN.exe

if "%COMPRESS_CHOICE%"=="SNAPPY" (
	set FILE_ICAT_COM=..\tool\EZGUI\iCatSnappy.exe
) else (
	set FILE_ICAT_COM=..\tool\EZGUI\iCatLzo.exe
)



set FILE_MENU_EXCEL=..\resource\%PRJ_DEF%\MENU\iCatchTek.xls

set PATH_ICON_SRC=..\resource\%PRJ_DEF%\A\RO_RES\UI\ICON_SRC
set PATH_ICON_SFN=..\resource\%PRJ_DEF%\A\RO_RES\UI\ICON
set PATH_MENU_SST=..\resource\%PRJ_DEF%\A\RO_RES\UI\SST
set PATH_MENU_FONT=..\resource\%PRJ_DEF%\A\RO_RES\UI\FONT
set PATH_MENU_FONT_COM=%PATH_MENU_FONT%\ASCII

set FILE_LANG_CHK=SST.FLG
set FILE_ICON_BASE_NAME=ICON

set FILE_MENU_TAB=..\customization\hostfw\ui\menu\app_menu_tab_%PRJ_DEF_LOWER%.c
set PATH_HOST_INC=..\customization\hostfw\include
set FILE_ICON_DEF=app_icon_def_%PRJ_DEF_LOWER%.h
set FILE_UI_PARA=app_ui_para_%PRJ_DEF_LOWER%.h

set FILE_PART_IMG_A=..\_%PRJ_DEF%\download\AIMG
set FILE_PART_IMG_B=..\_%PRJ_DEF%\download\BIMG

set PATH_UI_DL=..\_%PRJ_DEF%\download\A\RO_RES\UI
set PATH_MENU_SST_DL=%PATH_UI_DL%\SST
set PATH_ICON_SFN_DL=%PATH_UI_DL%\ICON
set PATH_FONT_SFN_DL=%PATH_UI_DL%\FONT

if not exist %PATH_MENU_SST_DL% md %PATH_MENU_SST_DL%
if not exist %PATH_ICON_SFN_DL% md %PATH_ICON_SFN_DL%
if not exist %PATH_FONT_SFN_DL% md %PATH_FONT_SFN_DL%

for %%a in (%ICON_LIST_HEIGHT%) do (
	if not exist %PATH_ICON_SRC%\%%a (
		echo The path "%PATH_ICON_SRC%\%%a" is NOT exist
		echo Icon height "%%a" can NOT be supported
		echo Please check the icon height list in "%0"

		if not "%1"=="" popd
		pause
		goto :eof
	)
)

REM ------------------------------------------------------------------------------------------------
REM Excel file handle process
REM ------------------------------------------------------------------------------------------------

echo Start to parsing excel...
%FILE_MENU_CVT% %FILE_MENU_EXCEL% %FILE_MENU_TAB% %PATH_HOST_INC%\%FILE_UI_PARA% %MULTI_LANGUAGE%

REM ------------------------------------------------------------------------------------------------
REM Generate SST files
REM ------------------------------------------------------------------------------------------------

echo Clean old sst files...
del /f /q %PATH_MENU_SST%\*.*

for %%a in (%MULTI_LANGUAGE%) do (
REM	echo move %PATH_HOST_INC%\%%a.sst %PATH_MENU_SST%\%%a.SST
	move /y %PATH_HOST_INC%\%%a.sst %PATH_MENU_SST%\%%a.SST
	del /f /q %PATH_HOST_INC%\%%a.bin 1>NUL 2>NUL

	%RES_COMPRESS% %PATH_MENU_SST%\%%a.SST
	echo copy %PATH_MENU_SST%\%%a.SST %PATH_MENU_SST_DL%\%%a.SST
	copy /y %PATH_MENU_SST%\%%a.SST %PATH_MENU_SST_DL%\%%a.SST 1>NUL
)

REM ------------------------------------------------------------------------------------------------
REM Generate SFN file for icon
REM ------------------------------------------------------------------------------------------------

echo Start to generate SFN file for icon...

for %%a in (%ICON_LIST_HEIGHT%) do (
	echo Generate the icon SFN file "%FILE_ICON_BASE_NAME%_%%a.SFN" with the height of %%a...

	%FILE_ICON_GEN% %PATH_ICON_SRC%\%%a %PATH_ICON_SFN%\%FILE_ICON_BASE_NAME%_%%a.SFN %PATH_HOST_INC%\%FILE_ICON_DEF% %ROT_OPT%

	%RES_COMPRESS% %PATH_ICON_SFN%\%FILE_ICON_BASE_NAME%_%%a.SFN
	echo copy %PATH_ICON_SFN%\%FILE_ICON_BASE_NAME%_%%a.SFN %PATH_ICON_SFN_DL%\%FILE_ICON_BASE_NAME%_%%a.SFN
	copy /y %PATH_ICON_SFN%\%FILE_ICON_BASE_NAME%_%%a.SFN %PATH_ICON_SFN_DL%\%FILE_ICON_BASE_NAME%_%%a.SFN 1>NUL
)

REM ------------------------------------------------------------------------------------------------
REM Generate SFN file for font
REM ------------------------------------------------------------------------------------------------

echo Start to generate SFN files for font...

move /y %PATH_HOST_INC%\sst.txt %PATH_MENU_FONT%\SST.TXT
move /y %PATH_HOST_INC%\sst.flg %PATH_MENU_FONT%\SST.FLG
rd /s /q %PATH_MENU_FONT_COM% 1>NUL 2>NUL
md %PATH_MENU_FONT_COM% 1>NUL
for %%b in (%FONT_LIST_HEIGHT%) do (
	if not exist %PATH_MENU_FONT_COM%\%%b md %PATH_MENU_FONT_COM%\%%b

	echo Generate BMP files for "SST" with the font height of %%b...
	%FILE_FONT_GEN% %PATH_MENU_FONT%\SST.TXT %PATH_MENU_FONT_COM%\%%b %%b

	echo Generate BMP files for "ASCII" with the font height of %%b...
	%FILE_FONT_GEN% %PATH_MENU_FONT%\ASCII.TXT %PATH_MENU_FONT_COM%\%%b %%b
)

set "SST_OPT="

setlocal EnableDelayedExpansion
for %%a in (%MULTI_LANGUAGE%) do (
REM	echo move %PATH_HOST_INC%\%%a.txt %PATH_MENU_FONT%\%%a.TXT
	move /y %PATH_HOST_INC%\%%a.txt %PATH_MENU_FONT%\%%a.TXT

	rd /s /q %PATH_MENU_FONT%\%%a 1>NUL 2>NUL
	md %PATH_MENU_FONT%\%%a 1>NUL

	set FONT_NAME=%%a
	for %%b in (%FONT_LIST_HEIGHT%) do (
		if not exist %PATH_MENU_FONT%\!FONT_NAME!\%%b md %PATH_MENU_FONT%\!FONT_NAME!\%%b

		call :CHK_LANG !FONT_NAME!
		echo Generate BMP files for "!FONT_NAME!" with the font height of %%b...
		%FILE_FONT_GEN% %PATH_MENU_FONT%\!FONT_NAME!.TXT %PATH_MENU_FONT%\!FONT_NAME!\%%b %%b !SST_OPT!
		copy /y %PATH_MENU_FONT_COM%\%%b\*.* %PATH_MENU_FONT%\!FONT_NAME!\%%b 1>NUL

		if %FONT_HEIGHT_CNT% gtr 1 (
			set FONT_SFN_NAME=!FONT_NAME!_%%b.SFN
		) else (
			set FONT_SFN_NAME=!FONT_NAME!.SFN
		)
		echo Generate the font SFN file "!FONT_SFN_NAME!" with the height of %%b...
		%FILE_FONT_SFN% %PATH_MENU_FONT%\!FONT_NAME!\%%b %PATH_MENU_FONT%\!FONT_SFN_NAME! %ROT_OPT%

		%RES_COMPRESS% %PATH_MENU_FONT%\!FONT_SFN_NAME!
		echo copy %PATH_MENU_FONT%\!FONT_SFN_NAME! %PATH_FONT_SFN_DL%\!FONT_SFN_NAME!
		copy /y %PATH_MENU_FONT%\!FONT_SFN_NAME! %PATH_FONT_SFN_DL%\!FONT_SFN_NAME! 1>NUL
	)
)
setlocal DisableDelayedExpansion

REM ------------------------------------------------------------------------------------------------
REM Main process over
REM ------------------------------------------------------------------------------------------------
REM ------------------------------------------------------------------------------------------------
REM Clear temp files
REM ------------------------------------------------------------------------------------------------

if %CLR_TMP_FILE% gtr 0 (
	pushd %PATH_MENU_FONT%
	dir /b *.txt >out.tmp
	for /f "tokens=1,*delims=." %%a in (out.tmp) do (
		echo Clear tmp files for "%%a"...
		rd /s /q %%a 1>NUL 2>NUL
		del /f /q %%a.FLG 1>NUL 2>NUL
		popd
		pushd %PATH_MENU_FONT%
	)
	del /f /q out.tmp 1>NUL 2>NUL
	popd
)

del /f /q %FILE_PART_IMG_A% 1>NUL 2>NUL
del /f /q %FILE_PART_IMG_B% 1>NUL 2>NUL

if not "%1"=="" popd

goto :eof

REM ------------------------------------------------------------------------------------------------
REM Local functions
REM ------------------------------------------------------------------------------------------------

:REG_LANG
set "MULTI_LANGUAGE=%MULTI_LANGUAGE% %1"
goto :eof

:REG_FONT_H
set "FONT_LIST_HEIGHT=%FONT_LIST_HEIGHT% %1"
goto :eof

:REG_ICON_H
set "ICON_LIST_HEIGHT=%ICON_LIST_HEIGHT% %1"
goto :eof

:CHK_LANG
set "SST_OPT="
for /f "tokens=1,*" %%a in (%PATH_MENU_FONT%\%FILE_LANG_CHK%) do (
	if "%%a"=="%1" (
		set "SST_OPT=SST"
		set LANG_NAME_SST=%%a
	)
)
if not "%SST_OPT%"=="" (
	echo Handle the language "%LANG_NAME_SST%" as "%SST_OPT%"...
)
goto :eof

:RES_COM_HDL
echo compress resource files "%1"
%FILE_ICAT_COM% -c %1 %1.TMP
move /y %1.TMP %1
goto :eof
