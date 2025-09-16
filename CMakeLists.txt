cmake_minimum_required(VERSION 3.10)
project(crush VERSION 0.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(crush
    src/main.cpp
    src/cli.cpp
    src/compressor.hpp
    src/compressor.cpp
)
