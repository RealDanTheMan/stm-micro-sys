cmake_minimum_required(VERSION 2.8)

project(stm32-minimal C)

# Binary output paths
set(CMAKE_BINARY_DIR ../build/bin/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(PROJECT_SOURCE_DIR ./src/)

# STM32-ARM CMSIS paths
set(STM32_DRIVER_DIR ../corelib/stm32)
set(STM32_DRIVER_DEVICE ${STM32_DRIVER_DIR}/Device/ST/)
set(STM32_DRIVER_CORE ${STM32_DRIVER_DIR}/Include/)

# Setup project include headers
include_directories(${STM32_DRIVER_DEVICE}/${MCU_FAMILY}/Include)
include_directories(${STM32_DRIVER_CORE}/)
include_directories(${PROJECT_SOURCE_DIR})

# Core Lib sources
set(CORE_LIB_SRC ../corelib/src/)
set(CORE_LIB_SOURCES 
${CORE_LIB_SRC}/base.c
${CORE_LIB_SRC}/clock.c
${CORE_LIB_SRC}/gpio.c
${CORE_LIB_SRC}/sys.c
${CORE_LIB_SRC}/uart.c
${CORE_LIB_SRC}/serial.c
${CORE_LIB_SRC}/i2c.c
${CORE_LIB_SRC}/ccbuff.c
${CORE_LIB_SRC}/drivers/lcd_ic.c
${CORE_LIB_SRC}/drivers/dht22.c)

# Main project (user) source files
set(SOURCES 
${PROJECT_SOURCE_DIR}/crt0.c 
${PROJECT_SOURCE_DIR}/main.c )

# Linker script that matches target MCU
set(LINKER_SCRIPT ./STM32-F303K8.ld)
#set(LINKER_SCRIPT ./STM32-F401RE.ld)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT}")

# Add platform specification define
# Platform enumerator can be found in platformdefs.h
#add_definitions( -DPLATFORMID=1UL ) # F401RE MCU
add_definitions( -DPLATFORMID=2UL ) # F303K8 MCU

# Define compile queue
add_executable(stm-micro-sys ${CORE_LIB_SOURCES} ${SOURCES} ${STM_DRIVER_SRC})