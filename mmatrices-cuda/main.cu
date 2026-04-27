#include <iostream>
#include <cuda_runtime.h>

__global__ void matrixMulKernel(double* d_A, double* d_B, double* d_C, int N) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < N && col < N) {
        double temp = 0;
        for (int k = 0; k < N; ++k) {
            temp += d_A[row * N + k] * d_B[k * N + col];
        }
        d_C[row * N + col] = temp;
    }
}

int main(int argc, char** argv) {

    if (argc < 2){
        std::cout<< "Tienes que ejecutar pasando el tamaño por parámetro, por ejemplo:\n" << std::endl;
        std::cout<< "./mmatrices_cuda 1024\n" << std::endl;

        return -1;
    }

    int N;

    try {
        N = std::stoi(argv[1]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: El argumento debe ser un número entero." << std::endl;
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: El número es demasiado grande." << std::endl;
        return 1;
    }

    size_t size = N * N * sizeof(double);

    double *h_A = (double*)malloc(size);
    double *h_B = (double*)malloc(size);
    double *h_C = (double*)malloc(size);

    // Inicialización simple
    for(int i=0; i<N*N; i++) { h_A[i] = 1.0; h_B[i] = 2.0; }

    double *d_A, *d_B, *d_C;
    cudaMalloc(&d_A, size);
    cudaMalloc(&d_B, size);
    cudaMalloc(&d_C, size);

    cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

    // --- MEDICIÓN DE TIEMPO INICIO ---
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    dim3 threadsPerBlock(32, 32);
    dim3 blocksPerGrid((N + threadsPerBlock.x - 1) / threadsPerBlock.x,
                       (N + threadsPerBlock.y - 1) / threadsPerBlock.y);

    // Registramos el inicio
    cudaEventRecord(start);

    matrixMulKernel<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, N);

    // Registramos el final
    cudaEventRecord(stop);
    cudaEventSynchronize(stop); // Esperamos a que la GPU termine

    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);
    // --- MEDICIÓN DE TIEMPO FIN ---

    cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);

    std::cout << "N: " << N << "x" << N << std::endl;
    std::cout << "Tiempo de ejecución en GPU: " << milliseconds << " ms" << std::endl;

    // Limpieza
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
    free(h_A); free(h_B); free(h_C);

    return 0;
}