__kernel void mat_mul(
    const int N,
    const int local_rows,
    __global const double* A,
    __global const double* B,
    __global double* C)
{
    int i = get_global_id(0); // Fila local
    int j = get_global_id(1); // Columna

    if (i < local_rows && j < N) {
        double sum = 0.0;
        for (int k = 0; k < N; k++) {
            sum += A[i * N + k] * B[k * N + j];
        }
        C[i * N + j] = sum;
    }
}