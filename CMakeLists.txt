cmake_minimum_required(VERSION 3.15)
project(OSLab4_original)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(Receiver)
add_subdirectory(Sender)
