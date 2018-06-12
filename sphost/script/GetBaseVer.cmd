@echo off
for /f "tokens=2,3 delims= " %%a in (..\..\spca6330\inc\version.h) do (
	if "%%a"=="BASEFW_RELEASE_VERSION" set ver_str=%%b
)
set /a base_ver=%ver_str:"=%
REM echo %base_ver%