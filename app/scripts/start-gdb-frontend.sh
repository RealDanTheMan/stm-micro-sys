#!/usr/bin/env bash

# Derive paths
STARTSCRIPT=$(readlink -f "$0")
STARTSCRIPTPATH=$(dirname "$STARTSCRIPT")/
APPPATH=$(readlink -f "$STARTSCRIPTPATH../")/
BUILDPATH=$(readlink -f "$APPPATH../build/")
BIN=$(readlink -f "$BUILDPATH/bin/stm-micro-sys")
GDBFRONTEND=/home/mrdan/local/packages/gdb-frontend/gdb-frontend/gdbfrontend
ARMGDB=$(realpath /usr/bin/arm-none-eabi-gdb)

echo
echo "-- StartPath: $BUILDSCRIPTPATH"
echo "-- AppPath: $APPPATH"
echo "-- BuildPath: $BUILDPATH"
echo "-- Bin: $BIN"
echo "-- GDBFrontEnd: $GDBFRONTEND"
echo "-- GDB: $ARMGDB"
echo "-- -- -- -- -- -- -- --"
echo 


bash $GDBFRONTEND -g $ARMGDB -G "$BIN"
#bash $GDBFRONTEND -g $(realpath /usr/bin/gdb) -G --data-directory=$(realpath /usr/share/gdb/data-directory/)gdb