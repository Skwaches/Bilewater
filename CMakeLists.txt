cmake_minimum_required(VERSION 4.2)
project(bilehaven)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(VENDORED "Use vendored libraries" ON)
set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../.Vendor)

if(VENDORED)
	add_subdirectory(${VENDOR_DIR}/SDL SDL)
else()
	find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

set(executable bilehaven)

add_executable(${executable}
	src/input.cpp
	src/main.cpp
)

target_include_directories(${executable} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
target_link_libraries(${executable} PRIVATE SDL3::SDL3)
	

