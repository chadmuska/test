@echo off

for /f "tokens=3 delims= " %%i in (..\lastprj.def) do (
	set PRJ=%%i
)

set IMGPATH=..\_%PRJ%\download
set POSPATH=..\_%PRJ%\download\A\RO_RES\UI\ICON_POS

if exist %IMGPATH%\AIMG (
	del /F %IMGPATH%\AIMG
)	

if exist %IMGPATH%\BIMG	(
	del /F %IMGPATH%\BIMG
)

if exist %POSPATH% (
	del /f /q %POSPATH%
)


