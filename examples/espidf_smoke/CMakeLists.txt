# ESP-IDF smoke example for Zyngine
#
# From this directory (with ESP-IDF exported):
#   idf.py set-target esp32s3
#   idf.py build
#   idf.py -p PORT flash monitor

cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(zyngine_espidf_smoke)
