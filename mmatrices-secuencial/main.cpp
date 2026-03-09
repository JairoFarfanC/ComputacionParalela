#include <iostream>
#include <vector>
#include <chrono> // Para medir el tiempo con precisión

using namespace std;
using namespace std::chrono;

// Función para inicializar matrices con valores aleatorios
void inicializar(vector<double>& M, int N) {
    for (int i = 0; i < N * N; ++i) {
        M[i] = (double)(rand() % 10);
    }
}

int main() {
    int N = 1000; // Prueba con 500, 1000, 2000 según tu enunciado

    // Usamos vectores para manejar la memoria dinámicamente
    vector<double> A(N * N);
    vector<double> B(N * N);
    vector<double> C(N * N, 0.0);

    inicializar(A, N);
    inicializar(B, N);

    cout << "Multiplicando matrices de tamaño " << N << "x" << N << "..." << endl;

    // --- Inicio de la medición ---
    auto start = high_resolution_clock::now();

    // Algoritmo Secuencial Clásico (Tres bucles i, j, k)
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            double suma = 0.0;
            for (int k = 0; k < N; ++k) {
                // Acceso indexado: Fila i, Columna k * Fila k, Columna j
                suma += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = suma;
        }
    }

    auto stop = high_resolution_clock::now();
    // --- Fin de la medición ---

    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Tiempo de ejecución secuencial: " << duration.count() << " ms" << endl;

    return 0;
}