#! /bin/csh
# c shell script to set environment variables for CLEED (no symmetry & no aoi)
setenv CLEED_HOME=~/CLEED                       # home directory for CLEED
setenv CLEED_PHASE=$CLEED_HOME/PHASE            # path to phase shift files
setenv CSEARCH_LEED=$CLEED_HOME/BIN/cleed_nsym  # path to csym or cnsym executables
setenv CSEARCH_RFAC=$CLEED_HOME/BIN/crfac       # path to crfac executable
