cmake_minimum_required(VERSION 2.8.4)
project(gameboy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp mmu.cpp cartridge.cpp cpu.cpp cpu_funcs.cpp cpu_maps.cpp)
add_executable(gameboy ${SOURCE_FILES})

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})
