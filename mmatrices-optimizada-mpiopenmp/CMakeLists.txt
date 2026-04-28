cmake_minimum_required(VERSION 3.20) # Versión estándar estable
project(mmatrices_optimizada_mpiopenmp)

set(CMAKE_CXX_STANDARD 20)

# 1. BUSCAR MPI
# Esto buscará automáticamente las rutas de mpi.h y las librerías
find_package(MPI REQUIRED)

# 2. BUSCAR OPENMP
# Esto busca los flags necesarios (como -fopenmp)
find_package(OpenMP REQUIRED)

add_executable(mmatrices_optimizada_mpiopenmp main.cpp)

# 3. VINCULAR TODO AL EJECUTABLE
# Esto le dice a CLion: "Usa los headers y librerías que encontraste arriba"
target_link_libraries(mmatrices_optimizada_mpiopenmp
        PRIVATE
        MPI::MPI_CXX
        OpenMP::OpenMP_CXX
)