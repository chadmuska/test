@echo off

REM ***********************************
REM            config env
REM ***********************************

set TOOL=..\tool\EZGUI\iconlayout2bin.exe
set FILE_POS=..\customization\hostfw\include\app_icon_def.h

for /f "tokens=3,* delims== " %%i in (..\lastprj.def) do (
	set DSTPATH=..\_%%i\download\A\RO_RES\UI\ICON_POS
	set SRCPATH=..\resource\%%i\A\RO_RES\UI\ICON_SRC\iconlayout
	echo %DSTPATH%
	echo %SRCPATH%
	goto done
)

:done

if not exist %DSTPATH% (
	md %DSTPATH%
)

call iconPosClean.bat

REM ************************************************************
REM			       convert ini file to bin file
REM   if you wnat to add some files to convert,
REM   you can directly copy the ini files into the SRCPATH folder 
REM ************************************************************

for /r %SRCPATH% %%i in (*.ini) do %TOOL% 0 %%i %FILE_POS% %DSTPATH%

