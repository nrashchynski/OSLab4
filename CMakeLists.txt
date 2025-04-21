cmake_minimum_required(VERSION 3.10)
project(OSLab4)

set(CMAKE_CXX_STANDARD 17)

add_executable(receiver src/receiver.cpp)
add_executable(sender src/sender.cpp)
