cmake_minimum_required(VERSION 3.12)
project(bootypic VERSION 1.9.3 LANGUAGES C)

# assumes LINKER_SCRIPT, BOOTYPIC_DEVICE_DIR
# but making these options breaks the script

set(FIRMWARE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../PowerBoard/src")

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

target_compile_options(bootypic PRIVATE -msmall-data)

set(LINKER_SCRIPT "${BOOTYPIC_DEVICE_DIR}/linkerscript_boot.gld")
set_target_properties(bootypic
  PROPERTIES
  LINK_DEPENDS "${LINKER_SCRIPT}")

target_link_options(bootypic
  PRIVATE
  # linker script
  "LINKER:-D_LINK_BOOT,--script=${LINKER_SCRIPT},--save-gld=${CMAKE_CURRENT_BINARY_DIR}/linkerscript.gld"
  # don't make interrupts. Bootloader doesn't use them.
  "LINKER:--no-isr,--no-ivt"
  # remove unused code, save output for debugging
  "LINKER:--gc-sections,-Map=${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_PROPERTY:NAME>.map"
  )

set(BOOTYPIC_HEX "bootypic.hex")

add_custom_command(
  TARGET bootypic
  POST_BUILD
  COMMAND "${XC16_bin2hex_EXECUTABLE}" "$<TARGET_FILE:bootypic>"
  BYPRODUCTS "${BOOTYPIC_HEX}"
)

install(FILES "$<TARGET_FILE:bootypic>" DESTINATION "." RENAME "bootypic-${PROJECT_VERSION}.elf")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${BOOTYPIC_HEX}" DESTINATION "." RENAME "bootypic-${PROJECT_VERSION}.hex")
