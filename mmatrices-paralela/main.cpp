#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace std::chrono;

int main() {
    int N = 1000;

    double** A = new double*[N];
    double** B = new double*[N];
    double** C = new double*[N];

    for (int i = 0; i < N; ++i) {
        A[i] = new double[N];
        B[i] = new double[N];
        C[i] = new double[N];
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = 1.0;
            B[i][j] = 2.0;
            C[i][j] = 0.0;
        }
    }

    vector<int> num_hilos = {2, 4, 8, 16};
    for (int th: num_hilos){
        omp_set_num_threads(th);

        cout << "Multiplicando matrices " << N << "x" << N << " con " << th << " hilos" << endl;

        auto start = high_resolution_clock::now();

        #pragma omp parallel for
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                double suma = 0.0;
                for (int k = 0; k < N; ++k) {
                    suma += A[i][k] * B[k][j];
                }
                C[i][j] = suma;
            }
        }

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        cout << "Tiempo total: " << duration.count() << " ms" << endl;

    }

    // Liberar memoria
    for (int i = 0; i < N; ++i) {
        delete[] A[i]; delete[] B[i]; delete[] C[i];
    }
    delete[] A; delete[] B; delete[] C;

    return 0;
}