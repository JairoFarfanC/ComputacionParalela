cmake_minimum_required(VERSION 3.28)
project(mmatrices_cuda LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 23)

add_executable(mmatrices_cuda main.cu)
