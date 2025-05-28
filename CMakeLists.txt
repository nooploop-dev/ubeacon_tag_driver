cmake_minimum_required(VERSION 3.16)

set(LIB_NAME ubeacon_tag)
project(${LIB_NAME})

add_library(
  ${LIB_NAME} STATIC ubeacon_tag_ble.h ubeacon_tag_common.h ubeacon_tag_iic.h
                     ubeacon_tag_uart.h ubeacon_tag_uart.c)
target_include_directories(${LIB_NAME} PUBLIC .)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)

  if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1)
  FetchContent_MakeAvailable(Catch2)
  enable_testing()
  add_subdirectory(test)
  add_subdirectory(app)
endif()
