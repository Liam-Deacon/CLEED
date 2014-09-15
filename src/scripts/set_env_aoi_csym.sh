#! /bin/bash
export CLEED_HOME=/usr/local/LEED_programs/CLEED_AOI
#Script to set environment variables for multiple-angle search, symmetrical CLEED
export CLEED_PHASE=$CLEED_HOME/PHASE
export CSEARCH_LEED=$CLEED_HOME/BIN/caoi_leed
export CAOI_LEED=$CLEED_HOME/BIN/cleed_csym
export CSEARCH_RFAC=$CLEED_HOME/BIN/caoi_rfac
export CAOI_RFAC=$CLEED_HOME/BIN/crfac
alias cleed=$CLEED_HOME/BIN/c_search
echo "Set environment variables for multiple-search, symmetrical CLEED"
