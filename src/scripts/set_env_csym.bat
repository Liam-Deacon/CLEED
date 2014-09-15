@echo off
REM Script to set environment variables for single-search, symmetrical CLEED
if not exist %CLEED_HOME% (set CLEED_HOME=\"%PROGRAMFILES%\CLEED\")
set CLEED_PHASE=%CLEED_HOME%\PHASE
set CSEARCH_LEED=%CLEED_HOME%\bin\cleed_csym.exe
REM set CAOI_LEED=%CLEED_HOME%\bin\caoi_leed.exe
set CSEARCH_RFAC=%CLEED_HOME%\bin\crfac.exe
REM set CAOI_RFAC=%CLEED_HOME%\bin\crfac.exe
echo "Set environment variables for single-search, symmetrical CLEED"
echo ""
echo "phase directory: %CLEED_PHASE%"
echo "cleed: %CSEARCH_LEED%"
echo "crfac: %CSEARCH_RFAC%"
REM echo "symmetry: %CSYM_LEED%"