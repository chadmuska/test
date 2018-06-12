@echo off

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Generate the file "ver_svn.h" and log the SVN version into it
::
:: Double click this batch file or use command as below:
::
::     ver_gen.bat [dir to be checked] [input file] [output file]
::                                                                                         By Phil
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Environment configure
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

set STR_LOG1=#ifndef __VER_SVN_H__
set STR_LOG2=#define __VER_SVN_H__
set STR_LOG3=#define _svn_ver_   $WCREV$
set STR_LOG4=#define _svn_url_  "$WCURL$"
set STR_LOG5=#endif /* __VER_SVN_H__ */

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Set the default configure
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

set FILE_INPUT=..\customization\hostfw\ver_svn.h
set FILE_OUTPUT=..\customization\hostfw\ver_svn.h
set FILE_BIN=..\..\tool\SubWCRev.exe
set DIR_TAG=..\..\

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: check the parameter
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if "%1" neq "" set DIR_TAG=%1

if "%2" neq "" (
	set FILE_INPUT=%2
	if "%3" neq "" (
		set FILE_OUTPUT=%3
	) else (
		set FILE_OUTPUT=%2
	)

	set FILE_BIN=%DIR_TAG%\bin\SubWCRev.exe
)

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Check if the version is controlled by SVN
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

REM if not exist %DIR_TAG%\.svn\ (
REM 	echo The directory "%DIR_TAG%" is not controlled by SVN
REM 	goto :EOF
REM )

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: File generate and version log
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

echo %STR_LOG1%> %FILE_INPUT%
echo %STR_LOG2%>>%FILE_INPUT%
echo %STR_LOG3%>>%FILE_INPUT%
echo %STR_LOG4%>>%FILE_INPUT%
echo %STR_LOG5%>>%FILE_INPUT%

if exist %FILE_OUTPUT% (
	copy /y %FILE_OUTPUT% %FILE_OUTPUT%.bak 1>NUL 2>NUL
)

%FILE_BIN% %DIR_TAG% %FILE_INPUT% %FILE_OUTPUT%

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Check the result
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

findstr /C:"WCREV" %FILE_OUTPUT% 1>NUL 2>NUL
if %errorlevel% equ 0 (
	echo SVN version convert fail
	echo Please check the software verion of "TortoiseSVN" ^(V1.6.* or latest needed^)
	
	copy /y %FILE_OUTPUT%.bak %FILE_OUTPUT% 1>NUL 2>NUL
	pause
)

if not exist %FILE_OUTPUT% (
	echo Can NOT find the file "%FILE_OUTPUT%"
	echo You can rename the backup file ".\kernel\ver_svn_h" to ".\kernel\ver_svn.h"
	echo NOTE: After you do that, the revision of SVN will be NOT correct
	pause
)

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Complete
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

if exist %FILE_OUTPUT%.bak (
	del /f /q %FILE_OUTPUT%.bak 1>NUL 2>NUL
)

:EOF