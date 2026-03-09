#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main(int argc, char **argv) {

    if (argc < 2) {
        cerr << "Debes introducir el tamaño de la matriz cuadrada, entre 500, 1000 y 2000" << endl;
        return -1;
    }

    int N = atoi(argv[1]);
    int BLOCK_SIZE = 32; //Tamaño estándar usado en este problema

    if (N!=500 && N!=1000 && N!=2000) {
        cerr << "El tamaño debe ser 500, 1000 o 2000" << endl;
        return -1;
    }

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

    // --- ALGORITMO SECUENCIAL PERO CON TILING ---
    for (int bi = 0; bi < N; bi += BLOCK_SIZE) {
        for (int bj = 0; bj < N; bj += BLOCK_SIZE) {
            for (int bk = 0; bk < N; bk += BLOCK_SIZE) {
                for (int i = bi; i < bi + BLOCK_SIZE && i < N; i++) {
                    for (int j = bj; j < bj + BLOCK_SIZE && j < N; j++) {
                        double sum = 0;
                        for (int k = bk; k < bk + BLOCK_SIZE && k < N; k++) {
                            sum += A[i][k] * B[k][j];
                        }
                        C[i][j] += sum;
                    }
                }
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