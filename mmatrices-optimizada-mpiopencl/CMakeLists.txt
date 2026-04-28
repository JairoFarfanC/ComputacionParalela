cmake_minimum_required(VERSION 3.20)
project(mmatrices_hibrido_opencl CXX)

set(CMAKE_CXX_STANDARD 20)

# 1. Buscar MPI
find_package(MPI REQUIRED)

# 2. Buscar OpenCL
# En Linux, esto suele buscar /usr/include/CL y /usr/lib/libOpenCL.so
find_package(OpenCL REQUIRED)

add_executable(mmatrices_hibrido_opencl mmatrices_opencl.cpp)

# 3. Vincular las rutas de inclusión y las librerías
# Esto soluciona el error de "mpi.h not found" y "cl.h not found"
target_link_libraries(mmatrices_hibrido_opencl
        PRIVATE
        MPI::MPI_CXX
        OpenCL::OpenCL
)

# Opcional: Copiar el archivo .cl al directorio donde se compila
# para que el programa lo encuentre al ejecutarse
configure_file(kernel.cl ${CMAKE_CURRENT_BINARY_DIR}/kernel.cl COPYONLY)