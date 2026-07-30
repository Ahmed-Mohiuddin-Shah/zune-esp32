# Zyngine as an ESP-IDF component
#
# This component is picked up when examples/espidf_smoke sets EXTRA_COMPONENT_DIRS
# to the repo's components/ folder (or when this directory is named zyngine under components/).

set(ZYN_ROOT "${CMAKE_CURRENT_LIST_DIR}/../..")

set(ZYN_SRCS
    "${ZYN_ROOT}/src/core/engine.cpp"
    "${ZYN_ROOT}/src/graphics/renderer.cpp"
    "${ZYN_ROOT}/src/graphics/texture.cpp"
    "${ZYN_ROOT}/src/ui/clay/clay_host.cpp"
    "${ZYN_ROOT}/src/ui/focus_nav.cpp"
    "${ZYN_ROOT}/src/ui/widgets/widgets.cpp"
    "${ZYN_ROOT}/src/platform/espidf/display.cpp"
    "${ZYN_ROOT}/src/platform/espidf/encoder.cpp"
    "${ZYN_ROOT}/src/platform/espidf/filesystem.cpp"
    "${ZYN_ROOT}/src/platform/espidf/gpio.cpp"
    "${ZYN_ROOT}/src/platform/espidf/log.cpp"
    "${ZYN_ROOT}/src/platform/espidf/mutex.cpp"
    "${ZYN_ROOT}/src/platform/espidf/time.cpp"
)

idf_component_register(
    SRCS ${ZYN_SRCS}
    INCLUDE_DIRS
        "${ZYN_ROOT}/include"
        "${ZYN_ROOT}/boards"
        "${ZYN_ROOT}/third_party/clay"
    REQUIRES
        driver
        esp_driver_ledc
        esp_lcd
        esp_timer
        freertos
        esp_lcd_ili9341
        nvs_flash
)

target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_20)
target_compile_definitions(${COMPONENT_LIB} PUBLIC ZYN_PLATFORM_ESPIDF=1)
# Clay uses designated/partial aggregate inits; IDF turns -Wmissing-field-initializers into errors.
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)
