#!/usr/bin/env bash

# Derive paths
DEPLOYSCRIPT=$(readlink -f "$0")
DEPLOYSCRIPTPATH=$(dirname "$DEPLOYSCRIPT")/
APPPATH=$(readlink -f "$DEPLOYSCRIPTPATH../")/
BUILDPATH=$(readlink -f "$APPPATH../build/")/

echo
echo Setting up build directory ...
echo 
echo "-- DeployScriptPath: $BUILDSCRIPTPATH"
echo "-- AppPath: $APPPATH"
echo "-- BuildPath: $BUILDPATH"
echo "-- -- -- -- -- -- -- --"
echo 

st-flash --reset write $BUILDPATH/bin/stm-micro-sys.bin  0x8000000
