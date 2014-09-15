#! /bin/bash
#Script to set environment variables for single-search, symmetrical CLEED
export CLEED_HOME=/usr/local/LEED_programs/CLEED
export CLEED_PHASE=$CLEED_HOME/PHASE
export CAOI_LEED=$CLEED_HOME/BIN/cleed_csym
export CSEARCH_RFAC=$CLEED_HOME/BIN/crfac
export CAOI_RFAC=$CLEED_HOME/BIN/crfac
alias cleed=$CLEED_HOME/BIN/c_search
echo "Set environment variables for single-search, symmetrical CLEED"
