#include <iostream>
#include <cuda_runtime.h>

// El KERNEL: Lo que se ejecuta en la GPU
__global__ void matrixMulKernel(double* d_A, double* d_B, double* d_C, int N) {
    // Calculamos la fila y columna que le corresponde a este hilo
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

int main() {
    int N = 1024; // Tamaño de la matriz N x N
    size_t size = N * N * sizeof(double);

    // 1. Reservar memoria en el Host (CPU)
    double *h_A = (double*)malloc(size);
    double *h_B = (double*)malloc(size);
    double *h_C = (double*)malloc(size);

    // Inicializar matrices... (puedes poner valores aleatorios aquí)

    // 2. Reservar memoria en el Device (GPU)
    double *d_A, *d_B, *d_C;
    cudaMalloc(&d_A, size);
    cudaMalloc(&d_B, size);
    cudaMalloc(&d_C, size);

    // 3. Copiar datos de la CPU a la GPU
    cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

    // 4. Configurar la malla de hilos (Grids y Blocks)
    // Usamos bloques de 16x16 o 32x32 hilos
    dim3 threadsPerBlock(32, 32);
    dim3 blocksPerGrid((N + threadsPerBlock.x - 1) / threadsPerBlock.x,
                       (N + threadsPerBlock.y - 1) / threadsPerBlock.y);

    // 5. Lanzar el Kernel
    matrixMulKernel<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, N);

    // 6. Copiar el resultado de vuelta a la CPU
    cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);

    std::cout << "Multiplicación en GPU completada." << std::endl;

    // 7. Liberar memoria
    cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
    free(h_A); free(h_B); free(h_C);

    return 0;
}