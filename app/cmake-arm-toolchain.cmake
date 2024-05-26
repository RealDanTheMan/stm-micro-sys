include(CMakeForceCompiler)

# Hint bare metal targets
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Hint target MCU
set(MCU_FAMILY STM32F3xx)
set(MCU_LINE STM32F303x8)

#set(MCU_FAMILY STM32F4xx)
#set(MCU_LINE STM32F401xE)

#set(MCU_LINKER_SCRIPT STM32F103RBTx_FLASH.ld)
set(MCU_ARCH cortex-m4)
set(MCU_FLOAT_ABI soft)
set(MCU_FPU fpv4-sp-d16)

add_definitions(-D${MCU_LINE})
add_definitions(-DUSE_HAL_LIBRARY)

# Hint arm gcc compiler
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_COMPILER /usr/bin/arm-none-eabi-gcc)

# Other arm tools
set(OBJSIZE ${COMPILER_PREFIX}arm-none-eabi-size)
set(OBJCOPY ${COMPILER_PREFIX}arm-none-eabi-objcopy)
set(OBJDUMP ${COMPILER_PREFIX}arm-none-eabi-objdump)
set(DEBUGGER ${COMPILER_PREFIX}arm-none-eabi-gdb)

# Remove linker flags
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "") 
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") 
set(CMAKE_SHARED_LIBRARY_LINK_ASM_FLAGS "")

# Common STM32-ARM flags
#set(COMMON_FLAGS "-mcpu=${MCU_ARCH} -mthumb -mthumb-interwork -mfloat-abi=${MCU_FLOAT_ABI}")
#set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs")
set(COMMON_FLAGS "-Wall -mcpu=${MCU_ARCH} -specs=nosys.specs -mlittle-endian -specs=nano.specs -mthumb -mthumb-interwork -mfloat-abi=${MCU_FLOAT_ABI} -ffunction-sections -fdata-sections -g -fno-common -fmessage-length=0")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")

# Set library options
set(SHARED_LIBS OFF)
set(STATIC_LIBS ON)