#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int inicio;
    int fin;
    double *A;
    double *B;
} DatosHilo;

double resultado_total = 0.0;
pthread_mutex_t mutex;

void *calcular_producto_punto(void *arg) {
    DatosHilo *datos = (DatosHilo *)arg;
    double suma = 0.0;
    for (int i = datos->inicio; i < datos->fin; i++) {
        suma += datos->A[i] * datos->B[i];
    }
    pthread_mutex_lock(&mutex);
    resultado_total += suma;
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

double producto_punto_paralelo(double *A, double *B, int N, int num_hilos) {
    pthread_t hilos[num_hilos];
    DatosHilo datos[num_hilos];
    resultado_total = 0.0;

    int elementos_por_hilo = N / num_hilos;
    int resto = N % num_hilos;

    pthread_mutex_init(&mutex, NULL);

    int inicio = 0;
    for (int i = 0; i < num_hilos; i++) {
        int fin = inicio + elementos_por_hilo;
        if (i < resto) fin++;
        datos[i].inicio = inicio;
        datos[i].fin = fin;
        datos[i].A = A;
        datos[i].B = B;
        pthread_create(&hilos[i], NULL, calcular_producto_punto, &datos[i]);
        inicio = fin;
    }

    for (int i = 0; i < num_hilos; i++) {
        pthread_join(hilos[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    return resultado_total;
}

double producto_punto_secuencial(double *A, double *B, int N) {
    double suma = 0.0;
    for (int i = 0; i < N; i++) {
        suma += A[i] * B[i];
    }
    return suma;
}

int main() {
    int N, num_hilos;

    printf("Ingrese el tamaño de los vectores N: ");
    scanf("%d", &N);

    printf("Ingrese el número de hilos: ");
    scanf("%d", &num_hilos);

    double *A = malloc(N * sizeof(double));
    double *B = malloc(N * sizeof(double));
    if (!A || !B) {
        fprintf(stderr, "Error al asignar memoria.\n");
        return 1;
    }

    printf("Ingrese los %d elementos del vector A:\n", N);
    for (int i = 0; i < N; i++) {
        scanf("%lf", &A[i]);
    }

    printf("Ingrese los %d elementos del vector B:\n", N);
    for (int i = 0; i < N; i++) {
        scanf("%lf", &B[i]);
    }

    clock_t start_seq = clock();
    double resultado_seq = producto_punto_secuencial(A, B, N);
    clock_t end_seq = clock();
    double tiempo_seq = (double)(end_seq - start_seq) / CLOCKS_PER_SEC;

    clock_t start_par = clock();
    double resultado_par = producto_punto_paralelo(A, B, N, num_hilos);
    clock_t end_par = clock();
    double tiempo_par = (double)(end_par - start_par) / CLOCKS_PER_SEC;

    printf("\nResultado Secuencial: %.2f\n", resultado_seq);
    printf("Tiempo Secuencial: %.6f segundos\n", tiempo_seq);

    printf("\nResultado Paralelo: %.2f\n", resultado_par);
    printf("Tiempo Paralelo: %.6f segundos\n", tiempo_par);

    free(A);
    free(B);
    return 0;
}
