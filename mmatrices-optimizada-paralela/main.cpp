#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace std::chrono;

#define NUM_THREADS 4
#define BLOCK_SIZE 64
#define N 1000

int main() {

    // Creamos la matriz como un array de punteros (Matriz real i,j)
    double** A = new double*[N];
    double** B = new double*[N];
    double** C = new double*[N];

    for (int i = 0; i < N; ++i) {
        A[i] = new double[N];
        B[i] = new double[N];
        C[i] = new double[N];
    }

    // Inicialización (Ejemplo con valores 1.0)
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = 1.0;
            B[i][j] = 2.0;
            C[i][j] = 0.0;
        }
    }

    cout << "Multiplicando matrices (formato Matriz[i][j]) en paralelo" << endl;

    auto start = omp_get_wtime();
#pragma omp parallel for collapse(2)
    for (int bi = 0; bi < N; bi += BLOCK_SIZE) {
        for (int bj = 0; bj < N; bj += BLOCK_SIZE) {
            for (int bk = 0; bk < N; bk += BLOCK_SIZE) {
                for (int i = bi; i < bi + BLOCK_SIZE && i < N; i++) {
                    for (int j = bj; j < bj + BLOCK_SIZE && j < N; j++) {
                        double sum = 0;
                        for (int k = bk; k < bk + BLOCK_SIZE && k < N; k++) {
                            sum += A[i][k] * B[k][j];
                        }
                        #pragma omp atomic
                        C[i][j] += sum;
                    }
                }
            }
        }
    }

    auto stop = omp_get_wtime();
    auto duration = stop - start;

    cout << "Tiempo: " << duration << " s" << endl;

    // Liberar memoria (Importante en C++)
    for (int i = 0; i < N; ++i) {
        delete[] A[i]; delete[] B[i]; delete[] C[i];
    }
    delete[] A; delete[] B; delete[] C;

    return 0;
}