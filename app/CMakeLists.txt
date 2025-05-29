set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# DEBUG 模式下定义DEBUG宏
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

# 移除-DNDEBUG 保留assert
string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
string(REPLACE "-DNDEBUG" "" CMAKE_C_FLAGS_RELWITHDEBINFO
               ${CMAKE_C_FLAGS_RELWITHDEBINFO})
string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO
               ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})

if(MSVC)
  add_compile_options(/utf-8)
  add_compile_options(/W4)
  add_compile_options(/permissive-)
  add_compile_options(/wd4201)
  add_compile_options(/wd4200)
  add_compile_options(/bigobj)
  add_compile_definitions(NOMINMAX)
  add_compile_definitions(_WIN32_WINNT=0x0A00)
else()
  add_compile_options(-Wall -Wextra -pedantic -Wno-gnu-anonymous-struct
                      -Wno-nested-anon-types -Wno-zero-length-array)
endif()

FetchContent_Declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG v2.5.0)
FetchContent_MakeAvailable(CLI11)
# CLI11::CLI11

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 11.1.4)
FetchContent_MakeAvailable(fmt)
# fmt::fmt

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.15.3)
FetchContent_MakeAvailable(spdlog)
# spdlog::spdlog

FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-34-2)
FetchContent_MakeAvailable(asio)
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
# asio

set(TARGET_NAME ubeacon_tag_logger)
add_executable(${TARGET_NAME} main.cpp serial_port.cpp serial_port.hpp
                              msg_handle.hpp msg_handle.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE ubeacon_tag CLI11::CLI11 fmt::fmt
                                             spdlog::spdlog asio)
target_compile_definitions(${TARGET_NAME} PRIVATE APP_NAME="${TARGET_NAME}")
