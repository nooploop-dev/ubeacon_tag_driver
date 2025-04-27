cmake_minimum_required(VERSION 3.20)

set(LIB_NAME ubeacon_tag)
project(${LIB_NAME})

add_library(${LIB_NAME} INTERFACE ubeacon_tag_ble.h ubeacon_tag_common.h
            ubeacon_tag_iic.h ubeacon_tag_uart.h)
target_include_directories(${LIB_NAME} INTERFACE .)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  include(FetchContent)
  if(NOT Catch2_FOUND)
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v3.7.1)
    FetchContent_MakeAvailable(Catch2)
  endif()
  if(NOT fmt_FOUND)
    FetchContent_Declare(
      fmt
      GIT_REPOSITORY https://github.com/fmtlib/fmt.git
      GIT_TAG 11.1.4)
    FetchContent_MakeAvailable(fmt)
  endif()
endif()

enable_testing()
add_subdirectory(test)
