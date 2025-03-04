cmake_minimum_required(VERSION 2.8)

project(modbus)
include_directories(include)
set(U source/util)
set(C source/common)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
   set(SOURCE_EXE
   ${U}/modbus.cpp
   ${U}/crc.cpp
   ${U}/unixcomm.cpp
   ${C}/funcs.cpp
   ${C}/ini.cpp )
else()
   set(SOURCE_EXE
   ${U}/modbus.cpp
   ${U}/crc.cpp
   ${U}/w32comm.cpp
   ${C}/funcs.cpp
   ${C}/ini.cpp )
endif()
add_executable(modbus ${SOURCE_EXE})
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
   target_compile_options(brig PRIVATE -Wall -Wextra -Wpedantic)
endif()
