cmake_minimum_required(VERSION 3.5)

project(zita-resampler-simd VERSION "1.6.0" LANGUAGES CXX)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$")
  message(FATAL_ERROR "Only GCC and Clang are supported currently.")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

find_package(OpenMP REQUIRED)

add_library(zita-resampler-simd SHARED
  "libs/resampler-table.cc"
  "libs/vresampler.cc"
  "libs/cpucheck.cc")
target_include_directories(zita-resampler-simd
  PUBLIC "${PROJECT_SOURCE_DIR}/libs")
set_target_properties(zita-resampler-simd PROPERTIES
  SOVERSION "${PROJECT_VERSION}")
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
  find_package(Threads REQUIRED)
  target_link_libraries(zita-resampler-simd PUBLIC ${CMAKE_THREAD_LIBS_INIT})
  message("Threads ${CMAKE_THREAD_LIBS_INIT}")
endif()

target_sources(zita-resampler-simd PRIVATE "libs/vresampler-simd-default.cc")

macro(zr_compile_target TARGET)
  target_sources(zita-resampler-simd PRIVATE "libs/cputest-${TARGET}.cc" "libs/vresampler-simd-${TARGET}.cc")
  target_compile_definitions(zita-resampler-simd PRIVATE "BUILD_TARGET_${TARGET}")
endmacro()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|i.86)$")
  zr_compile_target("sse")
  zr_compile_target("avx")
  zr_compile_target("avx512f")
else()
  zr_compile_target("omp")
endif()

set_property(TARGET zita-resampler-simd PROPERTY COMPILE_FLAGS ${OpenMP_CXX_FLAGS})

install(TARGETS zita-resampler-simd
  LIBRARY DESTINATION "lib"
  ARCHIVE DESTINATION "lib"
  RUNTIME DESTINATION "bin")
install(DIRECTORY "libs/zita-resampler"
  DESTINATION "include")

add_executable(vresampler-benchmark "test/vresampler-benchmark.cc")
target_link_libraries(vresampler-benchmark PRIVATE zita-resampler-simd)
