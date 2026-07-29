# Platform selection for Zyngine
# ZYN_PLATFORM = desktop | espidf | arduino (arduino not implemented yet)

set(ZYN_PLATFORM "desktop" CACHE STRING "Zyngine platform backend")
set_property(CACHE ZYN_PLATFORM PROPERTY STRINGS desktop espidf arduino)

if(ZYN_PLATFORM STREQUAL "desktop")
    set(ZYN_PLATFORM_DESKTOP ON)
    add_compile_definitions(ZYN_PLATFORM_DESKTOP=1)
elseif(ZYN_PLATFORM STREQUAL "espidf")
    set(ZYN_PLATFORM_ESPIDF ON)
    add_compile_definitions(ZYN_PLATFORM_ESPIDF=1)
elseif(ZYN_PLATFORM STREQUAL "arduino")
    message(FATAL_ERROR "Arduino platform backend is not implemented yet. See src/platform/arduino/README.md")
else()
    message(FATAL_ERROR "Unknown ZYN_PLATFORM=${ZYN_PLATFORM}")
endif()

message(STATUS "Zyngine platform: ${ZYN_PLATFORM}")
