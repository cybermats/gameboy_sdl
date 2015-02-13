cmake_minimum_required(VERSION 2.8.4)
project(gameboy)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${gameboy_SOURCE_DIR}/cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp mmu.cpp cartridge.cpp cpu.cpp cpu_funcs.cpp cpu_maps.cpp gpu.cpp sdl_wrapper.cpp)
add_executable(gameboy ${SOURCE_FILES})

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(gameboy ${SDL2_LIBRARY})
