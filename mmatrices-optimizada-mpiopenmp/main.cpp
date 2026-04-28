#include <iostream>
#include <vector>
#include <mpi.h>
#include <omp.h>
#include <cstdlib> // Necesaria para atoi

using namespace std;

#define BLOCK_SIZE 32

int main(int argc, char** argv) {
    int rank, size;

    // Inicializar MPI con soporte para hilos
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // --- CAPTURA DE N DESDE ARGUMENTOS ---
    if (argc < 2) {
        if (rank == 0) {
            cout << "Error: Falta el parámetro N." << endl;
            cout << "Uso: " << argv[0] << " <valor_de_N>" << endl;
        }
        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]);

    // Determinar cuántas filas le tocan a cada proceso
    if (N % size != 0) {
        if (rank == 0) cout << "Error: N (" << N << ") debe ser divisible por el numero de procesos (" << size << ")" << endl;
        MPI_Finalize();
        return 1;
    }

    int rows_per_proc = N / size;

    // --- ASIGNACIÓN DE MEMORIA DINÁMICA ---
    // Nota: A y C solo necesitan memoria completa en el proceso 0 para Scatter/Gather
    double* A = NULL;
    double* B = new double[N * N];
    double* C = NULL;

    if (rank == 0) {
        A = new double[N * N];
        C = new double[N * N];

        #pragma omp parallel for collapse(2)
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                A[i * N + j] = 1.0;
                B[i * N + j] = 2.0;
                C[i * N + j] = 0.0;
            }
        }
        cout << "Iniciando multiplicacion hibrida (N=" << N << ")" << endl;
        cout << "(MPI: " << size << " procesos, OpenMP: " << omp_get_max_threads() << " hilos/proc)" << endl;
    }

    // Buffer local para cada proceso
    double* local_A = new double[rows_per_proc * N];
    double* local_C = new double[rows_per_proc * N];

    // El proceso 0 envía la matriz B completa a todos
    MPI_Bcast(B, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // El proceso 0 reparte las filas de A entre todos
    MPI_Scatter(A, rows_per_proc * N, MPI_DOUBLE, local_A, rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Inicializar el buffer local de C
    for (int i = 0; i < rows_per_proc * N; i++) local_C[i] = 0.0;

    double start_time = MPI_Wtime();

    // --- CÁLCULO HÍBRIDO ---
    #pragma omp parallel for schedule(dynamic)
    for (int bi = 0; bi < rows_per_proc; bi += BLOCK_SIZE) {
        for (int bj = 0; bj < N; bj += BLOCK_SIZE) {
            for (int bk = 0; bk < N; bk += BLOCK_SIZE) {
                for (int i = bi; i < bi + BLOCK_SIZE && i < rows_per_proc; i++) {
                    for (int j = bj; j < bj + BLOCK_SIZE && j < N; j++) {
                        double sum = 0;
                        for (int k = bk; k < bk + BLOCK_SIZE && k < N; k++) {
                            sum += local_A[i * N + k] * B[k * N + j];
                        }
                        #pragma omp atomic
                        local_C[i * N + j] += sum;
                    }
                }
            }
        }
    }

    double end_time = MPI_Wtime();

    // Reunir los resultados
    MPI_Gather(local_C, rows_per_proc * N, MPI_DOUBLE, C, rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Tiempo total: " << end_time - start_time << " s" << endl;
        delete[] A;
        delete[] C;
    }

    // Limpieza
    delete[] B;
    delete[] local_A;
    delete[] local_C;

    MPI_Finalize();
    return 0;
}