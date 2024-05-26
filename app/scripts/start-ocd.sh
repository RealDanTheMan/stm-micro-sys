#!/usr/bin/env bash

# Derive paths
STARTSCRIPT=$(readlink -f "$0")
STARTSCRIPTPATH=$(dirname "$STARTSCRIPT")/
APPPATH=$(readlink -f "$STARTSCRIPTPATH../")/
BUILDPATH=$(readlink -f "$APPPATH../build/")

echo
echo Setting up build directory ...
echo 
echo "-- StartPath: $BUILDSCRIPTPATH"
echo "-- AppPath: $APPPATH"
echo "-- BuildPath: $BUILDPATH"
echo "-- -- -- -- -- -- -- --"
echo 

openocd  -f /usr/share/openocd/scripts/interface/stlink.cfg -f $BUILDPATH/bin/st_nucleo_f3.cfg