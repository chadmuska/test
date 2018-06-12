@echo off
cd res
IF %ERRORLEVEL% EQU 0 (
	del /Q *.*
	cd ..
	del /Q *.csv
	del /Q *.txt
	cd ..
	cd ..
	cd ..
	IF EXIST diq.def (
		FOR /F "skip=1 delims=\ " %%i IN (diq.def) DO del %%i
		del diq.def
	)
)
