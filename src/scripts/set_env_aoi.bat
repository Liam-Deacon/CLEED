@echo off
REM Script to set environment variables for mutliple-angle search, non-symmetrical CLEED
if not exist %CLEED_HOME% (set CLEED_HOME=\"%PROGRAMFILES%\CLEED\")
set CLEED_PHASE=%CLEED_HOME%\PHASE
set CSEARCH_LEED=%CLEED_HOME%\bin\caoi_leed.exe
set CAOI_LEED=%CLEED_HOME%\bin\cleed_nsym.exe
set CSEARCH_RFAC=%CLEED_HOME%\bin\caoi_rfac.exe
set CAOI_RFAC=%CLEED_HOME%\bin\crfac.exe
echo "Set environment variables for multiple-search, non-symmetrical CLEED"
echo ""
echo "phase directory: %CLEED_PHASE%"
echo "cleed: %CSEARCH_LEED%"
echo "cleed aoi: %CAOI_LEED%"
echo "crfac: %CSEARCH_RFAC%"
echo "crfac aoi: %CAOI_RFAC%"
echo "symmetry: %CSYM_LEED%"