import numpy as np
import time
import multiprocessing as mp
import matplotlib.pyplot as plt


# --- 1. SECUENCIAL CLÁSICO ---
def multiplicacion_clasica(A, B, N):
    C = np.zeros((N, N))
    for i in range(N):
        for j in range(N):
            for k in range(N):
                C[i][j] += A[i][k] * B[k][j]
    return C



# --- Versión 2: Bloques (Secuencial) ---
def multiplicacion_bloques(A, B, N, BS):
    C = np.zeros((N, N))
    for ii in range(0, N, BS):
        for jj in range(0, N, BS): # con ii y jj dividos la matriz en bloques
            for kk in range(0, N, BS): # kk selecciona el bloque
                # ahora es simplemente realizar la multiplicacion matricial normal en esos subloques
                for i in range(ii, min(ii + BS, N)):
                    for j in range(jj, min(jj + BS, N)):
                        for k in range(kk, min(kk + BS, N)):
                            C[i][j] += A[i][k] * B[k][j]
    return C

# --- Versión 3: Bloques (Paralela) ---
def trabajador_paralelo(id_proceso, num_procesos, A, B, N, BS, dict_retorno):

    # REPARTO DE TRABAJO: Dividir filas
    filas_por_proc = N // num_procesos
    inicio_i = id_proceso * filas_por_proc # calcula la fila por la que empieza cada proceso
    if id_proceso == num_procesos - 1: # si eres el ultimo te quedas hasta la ultima fila. Asi aseguras no quedarte corto
        fin_i = N 
    else:
        fin_i = (id_proceso + 1) * filas_por_proc
    
    # Cada hilo crea su parte de la matriz resultado
    # Esto evita CONDICIONES DE CARRERA donde se sobreescribe
    C_local = np.zeros((fin_i - inicio_i, N))
    
    # Algoritmo de bloques aplicado solo a su rango de filas. Es decir cada proceso empieza en una fila distinta
    for ii in range(inicio_i, fin_i, BS):
        for jj in range(0, N, BS):
            for kk in range(0, N, BS):
                for i in range(ii, min(ii + BS, fin_i)):
                    # Ajuste de índice local para escribir en C_local
                    idx_local = i - inicio_i
                    for j in range(jj, min(jj + BS, N)):
                        for k in range(kk, min(kk + BS, N)):
                            C_local[idx_local][j] += A[i][k] * B[k][j]
    
    dict_retorno[id_proceso] = C_local



if __name__ == "__main__":

    # --- CONFIGURACIÓN ---
    tamanos = [100, 200, 400]    # Tamaños para gráficos de tamaño
    BS = 32                      # Tamaño de bloque
    n_fijo = 400                 # Tamaño fijo para gráfico de hilos
    max_hilos = mp.cpu_count()
    hilos_lista = [1, 2, 4, max_hilos] if max_hilos > 4 else [1, 2, max_hilos]

    # Contenedores para datos
    tiempos_clasico = []
    tiempos_bloques = []
    tiempos_paralelo_N = []
    tiempos_por_hilos = []

    print("Iniciando Experimento 1: Variando Tamano N...")
    for N in tamanos:
        A = np.random.rand(N, N)
        B = np.random.rand(N, N)

        # 1. Clásico
        start = time.time()
        multiplicacion_clasica(A, B, N)
        tiempos_clasico.append(time.time() - start)

        # 2. Bloques
        start = time.time()
        multiplicacion_bloques(A, B, N, BS)
        tiempos_bloques.append(time.time() - start)

        # 3. Paralelo (con todos los hilos)
        manager = mp.Manager()
        resultados = manager.dict()
        procesos = []
        start = time.time()
        for i in range(max_hilos):
            p = mp.Process(target=trabajador_paralelo, args=(i, max_hilos, A, B, N, BS, resultados))
            procesos.append(p)
            p.start()
        for p in procesos: p.join()
        tiempos_paralelo_N.append(time.time() - start)

    print("Iniciando Experimento 2: Variando Número de Hilos...")
    A_fijo = np.random.rand(n_fijo, n_fijo)
    B_fijo = np.random.rand(n_fijo, n_fijo)
    for h in hilos_lista:
        manager = mp.Manager()
        res = manager.dict()
        procs = []
        start = time.time()
        for i in range(h):
            p = mp.Process(target=trabajador_paralelo, args=(i, h, A_fijo, B_fijo, n_fijo, BS, res))
            procs.append(p)
            p.start()
        for p in procs: p.join()
        tiempos_por_hilos.append(time.time() - start)

    # --- GENERACIÓN DE TABLA DE TIEMPOS (Por consola) ---
    print("\nTABLA DE TIEMPOS (Segundos)")
    print(f"{'N':<10} | {'Clásico':<12} | {'Bloques':<12} | {'Paralelo':<12}")
    print("-" * 55)
    for i in range(len(tamanos)):
        print(f"{tamanos[i]:<10} | {tiempos_clasico[i]:<12.4f} | {tiempos_bloques[i]:<12.4f} | {tiempos_paralelo_N[i]:<12.4f}")

    # --- GRÁFICOS ---
    
    # 1. Tiempo vs Tamaño de Matriz
    plt.figure(figsize=(10, 5))
    plt.plot(tamanos, tiempos_clasico, 'o-', label='Clásico')
    plt.plot(tamanos, tiempos_bloques, 's-', label='Bloques (1 hilo)')
    plt.plot(tamanos, tiempos_paralelo_N, '^-', label=f'Paralelo ({max_hilos} hilos)')
    plt.title('Gráfico 1: Tiempo vs Tamano de Matriz')
    plt.xlabel('N (Tamaño)')
    plt.ylabel('Tiempo (s)')
    plt.legend(); plt.grid(); plt.show()

    # 2. Tiempo vs Número de Hilos
    plt.figure(figsize=(10, 5))
    plt.plot(hilos_lista, tiempos_por_hilos, 'o-', color='orange')
    plt.title(f'Gráfico 2: Tiempo vs Número de Hilos (N={n_fijo})')
    plt.xlabel('Número de Hilos')
    plt.ylabel('Tiempo (s)')
    plt.grid(); plt.show()

    # 3. Gráfico de Speedup
    # Speedup = T(1 hilo) / T(p hilos)
    speedups = [tiempos_por_hilos[0] / t for t in tiempos_por_hilos]
    plt.figure(figsize=(10, 5))
    plt.plot(hilos_lista, speedups, 'o-', label='Speedup Real')
    plt.plot(hilos_lista, hilos_lista, '--', color='gray', label='Speedup Ideal')
    plt.title(f'Gráfico 3: Speedup vs Número de Hilos (N={n_fijo})')
    plt.xlabel('Número de Hilos')
    plt.ylabel('Speedup')
    plt.legend(); plt.grid(); plt.show()