cmake_minimum_required(VERSION 3.16)
set(HS_REFERENCE_ARDUINO_COMPONENT "${CMAKE_CURRENT_LIST_DIR}/../deps/espressif__arduino-esp32" CACHE PATH "Archived upstream Arduino/BLE component")
if(NOT EXISTS "${HS_REFERENCE_ARDUINO_COMPONENT}/libraries/BLE/src/BLEDevice.cpp")
    message(FATAL_ERROR "Existing BLE component is missing; no automatic dependency download")
endif()
set(ENV{IDF_COMPONENT_MANAGER} "0")
set(EXTRA_COMPONENT_DIRS "${HS_REFERENCE_ARDUINO_COMPONENT}")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(COMPONENTS main)
project(heart_raw_bridge_v2)

# IDF 6 HAL aggregate initializers warn under GCC 15 when included by Esp.cpp.
# Keep the warning visible and retain -Werror for every other diagnostic/source.
# No upstream archive or installed SDK file is modified.
idf_component_get_property(hs_arduino_lib espressif__arduino-esp32 COMPONENT_LIB)
set_property(SOURCE "${HS_REFERENCE_ARDUINO_COMPONENT}/cores/esp32/Esp.cpp"
    TARGET_DIRECTORY ${hs_arduino_lib} APPEND PROPERTY COMPILE_OPTIONS
    "-Wno-error=missing-field-initializers")
