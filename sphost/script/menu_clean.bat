@echo off

if "%1"=="" title iCatch UI Utility Process

if not "%1"=="" pushd %1

REM ------------------------------------------------------------------------------------------------
REM Environment configration
REM ------------------------------------------------------------------------------------------------

set FILE_PRJ_DEF=..\lastprj.def
set FILE_MENU_TAB=..\customization\hostfw\ui\menu\app_menu_tab.c

for /f "tokens=3,* delims== " %%a in (%FILE_PRJ_DEF%) do (
	set PRJ_DEF=%%a
	goto done
)
:done

set PATH_MENU_SST=..\resource\%PRJ_DEF%\A\RO_RES\UI\SST\MULTI_LANG
set PATH_ICON_SFN=..\resource\%PRJ_DEF%\A\RO_RES\UI\ICON
set PATH_FONT_SFN=..\resource\%PRJ_DEF%\A\RO_RES\UI\FONT
set PATH_ICON_SRC=..\resource\%PRJ_DEF%\A\RO_RES\UI\ICON_SRC

set PATH_MENU_SST_DL=..\_%PRJ_DEF%\download\A\RO_RES\UI\SST
set PATH_ICON_SFN_DL=..\_%PRJ_DEF%\download\A\RO_RES\UI\ICON
set PATH_FONT_SFN_DL=..\_%PRJ_DEF%\download\A\RO_RES\UI\FONT
set FILE_PART_IMG_A=..\_%PRJ_DEF%\download\AIMG
set FILE_PART_IMG_B=..\_%PRJ_DEF%\download\BIMG

REM ------------------------------------------------------------------------------------------------
REM clean process
REM ------------------------------------------------------------------------------------------------

echo Clean sst files...
del /f /q %PATH_MENU_SST%\*.* 1>NUL 2>NUL
del /f /q %PATH_MENU_SST_DL%\*.* 1>NUL 2>NUL

echo Clean icon sfn files...
pushd %PATH_ICON_SRC%
dir /a:-hd /b >out.tmp
for /f "tokens=1,*delims=." %%a in (out.tmp) do (
	echo Icon height: %%a...
	popd
	del /f /q %PATH_ICON_SFN%\ICON_%%a.SFN 1>NUL 2>NUL
	del /f /q %PATH_ICON_SFN%\ICON_%%a.SFN.LZO 1>NUL 2>NUL
	del /f /q %PATH_ICON_SFN_DL%\ICON_%%a.SFN 1>NUL 2>NUL
	pushd %PATH_ICON_SRC%
)
del /f /q out.tmp 1>NUL 2>NUL
popd

echo Clean font sfn files...
pushd %PATH_FONT_SFN%
dir /b *.txt >out.tmp
echo debug: > debug.tmp
for /f "tokens=1,*delims=." %%a in (out.tmp) do (
	rd /s /q %%a 1>NUL 2>NUL
	del /f /q %%a.SFN 1>NUL 2>NUL
	del /f /q %%a.SFN.LZO 1>NUL 2>NUL
	del /f /q %%a_*.SFN 1>NUL 2>NUL
	del /f /q %%a_*.SFN.LZO 1>NUL 2>NUL
	del /f /q %%a.FLG 1>NUL 2>NUL
	if not "%%a"=="ASCII" (
		if not "%%a"=="DS_UNICODE" (
			del /f /q %%a.TXT 1>NUL 2>NUL
			echo %%a
		)
	)
	popd
	del /f /q %PATH_FONT_SFN_DL%\%%a.SFN 1>NUL 2>NUL
	del /f /q %PATH_FONT_SFN_DL%\%%a_*.SFN 1>NUL 2>NUL
	pushd %PATH_FONT_SFN%
	
	echo list:%%a >> debug.tmp
)

REM mask for debug.
REM del /f /q out.tmp 1>NUL 2>NUL
popd

del /f /q %FILE_MENU_TAB% 1>NUL 2>NUL
del /f /q %FILE_PART_IMG_A% 1>NUL 2>NUL
del /f /q %FILE_PART_IMG_B% 1>NUL 2>NUL

REM ------------------------------------------------------------------------------------------------
REM clean end
REM ------------------------------------------------------------------------------------------------

if not "%1"=="" popd
