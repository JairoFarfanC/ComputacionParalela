1. Análisis del Algoritmo Secuencial Básico y la Jerarquía de Memoria

El algoritmo de multiplicación de matrices estándar, aunque matemáticamente sencillo, presenta un rendimiento ineficiente en arquitecturas de computación modernas debido a cómo se gestiona la memoria caché.
1.1. El Problema del Acceso a Memoria

En el algoritmo de tres bucles anidados (i,j,k), el acceso a los datos de las matrices A y B no es simétrico:

    Matriz A (Acceso por filas): El bucle más interno recorre los elementos A[i][k]. Al ser un lenguaje que almacena matrices por filas (Row-Major), los elementos contiguos en el código están contiguos en la memoria física. Esto genera un acceso secuencial perfecto, aprovechando al máximo cada línea de caché cargada.

    Matriz B (Acceso por columnas): El bucle interno accede a B[k][j]. Al incrementar k, el algoritmo salta de una fila a otra para mantener la misma columna j.

    Impacto técnico: Si la matriz es de tamaño N=2000, cada salto entre B[k][j] y B[k+1][j] implica saltar 2000×taman˜o(double) bytes. Esto excede con creces el tamaño de una línea de caché típica (64 bytes).

1.2. Fallos de Caché (Cache Misses)

La CPU no lee datos individuales de la RAM, sino que trae "bloques" (líneas de caché). Cuando el algoritmo accede a la columna de la matriz B:

    Se carga una línea de caché que contiene B[k][j] y los elementos adyacentes de esa misma fila.

    Sin embargo, el algoritmo inmediatamente después pide B[k+1][j], que está en una fila distinta.

    La línea de caché cargada anteriormente se desperdicia casi por completo.

    Para matrices grandes, cuando el bucle vuelve a necesitar los datos de la primera fila, estos ya han sido expulsados de la caché por nuevos datos, provocando un ciclo constante de Cache Misses.

1.3. La Solución: Optimización por Bloques (Tiling)

Para mitigar este problema, implementamos la técnica de Tiling. Esta técnica consiste en:

    Dividir las matrices en sub-matrices (bloques) de tamaño T×T.

    Maximizar la localidad temporal: Trabajamos intensivamente con un bloque pequeño que cabe completamente en la caché L1 o L2 antes de pasar al siguiente.

    Reducción de Latencia: Al mantener los datos "cerca" del procesador, reducimos el tiempo que la CPU pasa ociosa esperando a que los datos lleguen desde la memoria principal (RAM).

Resumen de Problemas Detectados
Problema	Causa	Consecuencia
Bajo aprovechamiento de ancho de banda	Acceso por columnas en la matriz B.	La CPU desperdicia datos cargados en caché.
Alta latencia	Saltos de memoria (Stride) superiores al tamaño de página.	Constantes esperas de la CPU a la memoria RAM.
Escalabilidad Pobre	Complejidad O(N3) con mala localidad.	El tiempo de ejecución crece exponencialmente al aumentar N.
