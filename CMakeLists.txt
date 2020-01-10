cmake_minimum_required(VERSION 3.12)
project(bootypic VERSION 1.9.3 LANGUAGES C)

# assumes LINKER_SCRIPT, BOOTYPIC_DEVICE_DIR
# but making these options breaks the script

set(FIRMWARE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../Power_Board/src")

add_executable(bootypic
  bootloader.c
  "${BOOTYPIC_DEVICE_DIR}/boot_user.c"
  "${FIRMWARE_SRC}/power.c"
  "${FIRMWARE_SRC}/battery.c"
  "${FIRMWARE_SRC}/stdfunctions.c"
  "${FIRMWARE_SRC}/i2clib.c"
  )

target_include_directories(bootypic
  PRIVATE
  "${BOOTYPIC_DEVICE_DIR}"
  "${FIRMWARE_SRC}"
  )

target_compile_options(bootypic PRIVATE -ffunction-sections)
target_compile_definitions(bootypic PRIVATE BOOTYPIC)

set_target_properties(bootypic
  PROPERTIES
  LINK_DEPENDS "${LINKER_SCRIPT}")
target_link_options(bootypic
  PRIVATE
  "LINKER:-D_LINK_BOOT,--no-isr"
  "LINKER:--script=${LINKER_SCRIPT},--save-gld=${CMAKE_CURRENT_BINARY_DIR}/linkerscript.gld,-Map=${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_PROPERTY:NAME>.map,--gc-sections")

set(BOOTYPIC_HEX "bootypic.hex")

add_custom_command(
  TARGET bootypic
  POST_BUILD
  COMMAND "${XC16_bin2hex_EXECUTABLE}" "$<TARGET_FILE:bootypic>"
  BYPRODUCTS "${BOOTYPIC_HEX}"
)

install(FILES "$<TARGET_FILE:bootypic>" DESTINATION "." RENAME "bootypic-${PROJECT_VERSION}.elf")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${BOOTYPIC_HEX}" DESTINATION "." RENAME "bootypic-${PROJECT_VERSION}.hex")

set(MPLABX_VER v5.30)
if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
  set(MPLABX_ROOT "C:/Program Files (x86)/Microchip/MPLABX/${MPLABX_VER}")
else()
  set(MPLABX_ROOT "/opt/microchip/mplabx/${MPLABX_VER}")
endif()


#build target to flash an attached file with the bootloader
find_package(MPLABX)
add_custom_target(
  "Flash_Bootloader"
  COMMAND "${MPLABX_ipecmd_EXECUTABLE}"
  "-P${CMAKE_SYSTEM_PROCESSOR}" # part selection (required)
  "-E" # erase device first
  "-C" # check device is blank
  "-F${BOOTYPIC_HEX}"
  "-TPPK3" #PicKit3
  # "-YP"
  DEPENDS bootypic
  COMMENT "Connecting to PICKit3 to flash device..."
)