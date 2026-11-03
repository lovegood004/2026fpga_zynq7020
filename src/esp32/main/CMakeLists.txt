idf_component_register(SRCS "main.cpp" "ble_stethoscope_raw.cpp" "../../ps/src/ui/raw.cpp"
    INCLUDE_DIRS "." "../../ps/src/ui"
    REQUIRES espressif__arduino-esp32 bt nvs_flash esp_timer driver)
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)

# GCC 15 flags legacy shared parser formatting; preserve the board-tested source
# and keep this warning visible without failing only this translation unit.
set_property(SOURCE "${CMAKE_CURRENT_LIST_DIR}/../../ps/src/ui/raw.cpp"
    APPEND PROPERTY COMPILE_OPTIONS "-Wno-error=misleading-indentation")
