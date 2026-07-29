cmake_minimum_required(VERSION 3.16)
project(zyngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Clay requires C++20 (designated initializers / language mode). Engine code stays
# compatible with C++17 idioms otherwise.

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ZynginePlatform)

option(ZYN_BUILD_EXAMPLES "Build desktop smoke example" ON)

set(ZYN_CORE_SOURCES
    src/core/engine.cpp
    src/graphics/renderer.cpp
    src/graphics/texture.cpp
    src/ui/clay/clay_host.cpp
    src/ui/focus_nav.cpp
    src/ui/widgets/widgets.cpp
)

if(ZYN_PLATFORM STREQUAL "desktop")
    set(ZYN_PLATFORM_SOURCES
        src/platform/desktop/display.cpp
        src/platform/desktop/filesystem.cpp
        src/platform/desktop/gpio.cpp
        src/platform/desktop/log.cpp
        src/platform/desktop/mutex.cpp
        src/platform/desktop/time.cpp
    )
elseif(ZYN_PLATFORM STREQUAL "espidf")
    message(FATAL_ERROR
        "Build ESP-IDF targets with idf.py from examples/espidf_smoke, not this root CMakeLists.")
endif()

add_library(zyngine STATIC ${ZYN_CORE_SOURCES} ${ZYN_PLATFORM_SOURCES})
add_library(zyngine::zyngine ALIAS zyngine)

target_compile_features(zyngine PUBLIC cxx_std_20)

target_include_directories(zyngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/boards>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/clay>
        $<INSTALL_INTERFACE:include>
)

if(ZYN_PLATFORM STREQUAL "desktop")
    find_package(raylib QUIET)
    if(NOT raylib_FOUND)
        include(FetchContent)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.5
        )
        FetchContent_MakeAvailable(raylib)
    endif()
    target_link_libraries(zyngine PUBLIC raylib)
    if(UNIX AND NOT APPLE)
        target_link_libraries(zyngine PUBLIC pthread dl m)
    endif()
endif()

if(ZYN_BUILD_EXAMPLES AND ZYN_PLATFORM STREQUAL "desktop")
    add_executable(desktop_smoke examples/desktop_smoke/main.cpp)
    target_link_libraries(desktop_smoke PRIVATE zyngine)
endif()

install(TARGETS zyngine ARCHIVE DESTINATION lib)
install(DIRECTORY include/zyngine DESTINATION include)
