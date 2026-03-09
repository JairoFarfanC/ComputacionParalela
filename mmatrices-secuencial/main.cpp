#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main() {
    int N = 1000;

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
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = 1.0;
            B[i][j] = 2.0;
            C[i][j] = 0.0;
        }
    }

    cout << "Multiplicando matrices (formato Matriz[i][j])..." << endl;

    auto start = high_resolution_clock::now();

    // --- ALGORITMO SECUENCIAL CLÁSICO ---
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Tiempo: " << duration.count() << " ms" << endl;

    // Liberar memoria (Importante en C++)
    for (int i = 0; i < N; ++i) {
        delete[] A[i]; delete[] B[i]; delete[] C[i];
    }
    delete[] A; delete[] B; delete[] C;

    return 0;
}