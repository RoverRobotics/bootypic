cmake_minimum_required(VERSION 3.12)
project(bootypic VERSION 1.9.3 LANGUAGES C)

STRING(TOLOWER "pic${CMAKE_SYSTEM_PROCESSOR}" MCU_NAME)
STRING(TOLOWER "p${CMAKE_SYSTEM_PROCESSOR}" MCU_NAME_2)

set(DEVICE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/devices/${MCU_NAME}")
set(FIRMWARE_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/../Power_Board/src")

add_executable(bootypic
  bootloader.c
  "${DEVICE_SRC}/boot_user.c"
  "${FIRMWARE_SRC}/power.c"
  "${FIRMWARE_SRC}/battery.c"
  "${FIRMWARE_SRC}/stdfunctions.c"
  "${FIRMWARE_SRC}/i2clib.c"
  )

target_include_directories(bootypic
  PRIVATE
  "${DEVICE_SRC}"
  "${FIRMWARE_SRC}"
  )

target_compile_options(bootypic PRIVATE -O1 -ffunction-sections)
target_compile_definitions(bootypic PRIVATE BOOTYPIC)
set(APP_LINKER_SCRIPT "${DEVICE_SRC}/${MCU_NAME_2}_app.gld")
set(BOOT_LINKER_SCRIPT "${DEVICE_SRC}/${MCU_NAME_2}_boot.gld")

set_target_properties(bootypic
  PROPERTIES LINK_DEPENDS ${BOOT_LINKER_SCRIPT})
target_link_options(bootypic
  PRIVATE
  "LINKER:--gc-sections"
  "LINKER:--heap=2048"
  "LINKER:--coresident,--defsym,__SP_init=0x800,--defsym,__SPLIM_init=0x1000,-mreserve=data@0x0:0x1000"
  "LINKER:--script=${BOOT_LINKER_SCRIPT},-Map=${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_PROPERTY:NAME>.map,--no-isr,--save-gld,-D__DEFINE_RESET"
  )
# $<TARGET_LINKER_FILE:myLib>
#file(GENERATE OUTPUT filename CONTENT "$<TARGET_LINKER_FILE:bootypic>")
add_custom_command(
  TARGET bootypic POST_BUILD
  COMMAND "${XC16_bin2hex_EXECUTABLE}" "$<TARGET_FILE:bootypic>"
  BYPRODUCTS bootypic.hex
)
