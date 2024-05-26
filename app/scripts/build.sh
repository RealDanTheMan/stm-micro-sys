#!/usr/bin/env bash

# Derive paths
BUILDSCRIPT=$(readlink -f "$0")
BUILDSCRIPTPATH=$(dirname "$BUILDSCRIPT")/
APPPATH=$(readlink -f "$BUILDSCRIPTPATH../")/
CORELIBPATH=$(readlink -f "$APPPATH../corelib")/
BUILDPATH=$(readlink -f "$APPPATH../build/")/

# Define linker script that matched target MCU
LINKERSCRIPT=$(readlink -f "$CORELIBPATH/linkers/STM32-F303K8.ld")
#LINKERSCRIPT=$(readlink -f "$CORELIBPATH/linkers/STM32-F401RE.ld")

echo
echo Setting up build directory ...
echo 
echo "-- BuildScriptPath: $BUILDSCRIPTPATH"
echo "-- AppPath: $APPPATH"
echo "-- CoreLibPath: $CORELIBPATH"
echo "-- BuildPath: $BUILDPATH"
echo "-- LinkerScript: $LINKERSCRIPT"
echo "-- -- -- -- -- -- -- --"
echo 

# Clear build directory and copy linker script 
rm -rf $BUILDPATH
mkdir $BUILDPATH
cp $LINKERSCRIPT $BUILDPATH/$(basename $LINKERSCRIPT)

echo Runing cmake commands ...
echo 

cmake .  -B$BUILDPATH -DCMAKE_TOOLCHAIN_FILE=$APPPATH/cmake-arm-toolchain.cmake
cmake --build $BUILDPATH

echo 
echo Runing make commands ...
echo

cd $BUILDPATH
make

echo
echo Converting to arm bin executable ...

cd ..
arm-none-eabi-objcopy -O binary $BUILDPATH/bin/stm-micro-sys $BUILDPATH/bin/stm-micro-sys.bin

#OpenOCD configs are platform specific!
echo Generating OpenOCD config files for debugging
OCDCONFIG=/usr/local/share/openocd/scripts/board/st_nucleo_f3.cfg
cp $OCDCONFIG $BUILDPATH/bin/$(basename $OCDCONFIG)

echo
echo Build script finished.