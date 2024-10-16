#
# Bare-Bone CMakeLists.txt
#

# To create compile_commands.json
#
# $ mkdir build; cd build
# $ cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..
# $ cd ..; ln -s ./build/compile_commands.json ./
#
cmake_minimum_required(VERSION 3.10)
project(test)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_executable(test test.cpp)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "--debug -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -Wall -Wextra -Wpedantic")

set(HOMEBREW_PREFIX "/opt/homebrew")
set(CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX}")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(test ${OpenCV_LIBS})
