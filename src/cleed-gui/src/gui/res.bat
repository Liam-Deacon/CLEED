@echo OFF
REM Update resource file
set RES="%DROPBOX%\Programming\Python\cleed-gui\src\gui"
echo Updating resource file: %res%\res\res.qrc...
set COMPILER=pyrcc4
cd %RES%
%COMPILER% res\res.qrc -o res_rc.py
echo Updated 'res_rc.py'
pause