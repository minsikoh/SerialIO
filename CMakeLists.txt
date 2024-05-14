cmake_minimum_required(VERSION 3.22.1)

project(SerialIO)

set(CMAKE_CXX_STANDARD 11)

add_executable(sioTest main.cpp serialio.cpp networkmanager.cpp)
