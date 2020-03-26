cmake_minimum_required(VERSION 3.10)
project(Zad2 C)

set(CMAKE_C_STANDARD 11)

add_library(MatrixLib library.c library.h)

add_executable(Matrix main.c)
add_executable(CreateRand createRandMatrices.c)

target_link_libraries(Matrix MatrixLib m)
target_link_libraries(CreateRand m)