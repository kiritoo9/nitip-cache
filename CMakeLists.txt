cmake_minimum_required(VERSION 3.20)

project(Nitip)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CSS_STANDARD_REQUIRED ON)

add_executable(
    nitip
    src/main.cpp
    src/engine.cpp
    src/server.cpp
)