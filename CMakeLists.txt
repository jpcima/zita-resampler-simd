cmake_minimum_required(VERSION 3.5)

project(zita-resampler-simd VERSION "1.6.0" LANGUAGES CXX)

if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

find_package(OpenMP REQUIRED)

add_library(zita-resampler-simd SHARED
  "libs/resampler-table.cc"
  "libs/vresampler.cc")
target_include_directories(zita-resampler-simd
  PUBLIC "${PROJECT_SOURCE_DIR}/libs")
set_target_properties(zita-resampler-simd PROPERTIES
  SOVERSION "${PROJECT_VERSION}")
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
  find_package(Threads REQUIRED)
  target_link_libraries(zita-resampler-simd PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif()

target_sources(zita-resampler-simd PRIVATE "libs/vresampler-simd-default.cc")
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  target_sources(zita-resampler-simd PRIVATE "libs/vresampler-simd-sse.cc")
  target_sources(zita-resampler-simd PRIVATE "libs/vresampler-simd-avx.cc")
  target_sources(zita-resampler-simd PRIVATE "libs/vresampler-simd-avx512.cc")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^i.86$")
  target_sources(zita-resampler-simd PRIVATE "libs/vresampler-simd-sse.cc")
  target_sources(zita-resampler-simd PRIVATE "libs/vresampler-simd-avx.cc")
  target_sources(zita-resampler-simd PRIVATE "libs/vresampler-simd-avx512.cc")
else()
  target_sources(zita-resampler-simd PRIVATE "libs/vresampler-simd-omp.cc")
endif()

set_property(TARGET zita-resampler-simd PROPERTY COMPILE_FLAGS ${OpenMP_CXX_FLAGS})

install(TARGETS zita-resampler-simd
  LIBRARY DESTINATION "lib"
  ARCHIVE DESTINATION "lib"
  RUNTIME DESTINATION "bin")
install(DIRECTORY "libs/zita-resampler"
  DESTINATION "include")
