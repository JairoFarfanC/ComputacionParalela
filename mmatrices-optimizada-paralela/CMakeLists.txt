cmake_minimum_required(VERSION 4.0)
project(mmatrices_paralela)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenMP REQUIRED)

add_executable(mmatrices_paralela main.cpp)

target_link_libraries(mmatrices_paralela PUBLIC OpenMP::OpenMP_CXX)
