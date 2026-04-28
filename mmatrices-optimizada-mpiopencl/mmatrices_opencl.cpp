#include <iostream>
#include <vector>
#include <fstream>
#include <CL/cl.h>
#include <mpi.h>
#include <cstdlib> // Necesaria para atoi

using namespace std;

// Función para cargar el kernel
string loadKernel(const char* filename) {
    ifstream in(filename);
    if (!in.is_open()) return "";
    return string((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // --- MANEJO DE ARGUMENTOS ---
    if (argc < 2) {
        if (rank == 0) cout << "Error: Uso: " << argv[0] << " <N>" << endl;
        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]); // Capturamos N del argumento
    int rows_per_proc = N / size;

    // Validación mínima para evitar errores de división o memoria
    if (N % size != 0) {
        if (rank == 0) cout << "Error: N debe ser divisible por el número de procesos MPI." << endl;
        MPI_Finalize();
        return 1;
    }

    // --- ASIGNACIÓN DINÁMICA ---
    double *A = NULL;
    double *B = new double[N * N];
    double *C = NULL;

    double *local_A = new double[rows_per_proc * N];
    double *local_C = new double[rows_per_proc * N];

    if (rank == 0) {
        A = new double[N * N];
        C = new double[N * N];
        for (int i = 0; i < N * N; i++) { A[i] = 1.0; B[i] = 2.0; }
    }

    // Distribuir datos
    MPI_Bcast(B, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, rows_per_proc * N, MPI_DOUBLE, local_A, rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // --- CONFIGURACIÓN OPENCL ---
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);
    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

    string source = loadKernel("kernel.cl");
    const char* src = source.c_str();
    cl_program program = clCreateProgramWithSource(context, 1, &src, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "mat_mul", NULL);

    // Buffers
    cl_mem bufA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * rows_per_proc * N, local_A, NULL);
    cl_mem bufB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(double) * N * N, B, NULL);
    cl_mem bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(double) * rows_per_proc * N, NULL, NULL);

    clSetKernelArg(kernel, 0, sizeof(int), &N);
    clSetKernelArg(kernel, 1, sizeof(int), &rows_per_proc);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufA);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), &bufB);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), &bufC);

    double start = MPI_Wtime();

    // Ejecutar Kernel (Asegúrate de que local_size divida a global_size)
    size_t local_size[2] = {32, 32};
    size_t global_size[2] = {(size_t)rows_per_proc, (size_t)N};
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, local_size, 0, NULL, NULL);
    clFinish(queue);

    double end = MPI_Wtime();

    clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0, sizeof(double) * rows_per_proc * N, local_C, 0, NULL, NULL);

    MPI_Gather(local_C, rows_per_proc * N, MPI_DOUBLE, C, rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Tiempo MPI+OpenCL (N=" << N << "): " << end - start << " s" << endl;
        delete[] A; delete[] C;
    }

    clReleaseMemObject(bufA); clReleaseMemObject(bufB); clReleaseMemObject(bufC);
    clReleaseKernel(kernel); clReleaseProgram(program);
    clReleaseCommandQueue(queue); clReleaseContext(context);
    delete[] B; delete[] local_A; delete[] local_C;

    MPI_Finalize();
    return 0;
}