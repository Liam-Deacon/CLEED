@echo OFF
REM Update resource file
set RES="C:\Users\Liam\Dropbox\Programming\Python\Extract-PyV\src"
echo Updating resource file: %RES%\res\res.qrc...
set COMPILER=pyrcc4
cd %RES%
%COMPILER% res\res.qrc -o res_rc.py
echo Updated 'res_rc.py'
pause