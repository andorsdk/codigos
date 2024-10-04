#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CLIENTES 100
#define CLIENTES_POR_FURGON 20

// Definir semáforos y variables de control
sem_t clientes_en_sucursal;  // Controla cuántos clientes están en la sucursal
sem_t operacion_en_curso;    // Controla si una operación de transporte está en curso
pthread_mutex_t mutex;       // Protege el acceso a la variable de número de clientes
int clientes = 0;            // Número de clientes en la sucursal



void* llegada_cliente(void* arg) {
    while (1) {
        sleep(rand() % 3 + 1);  // Simula tiempo de llegada aleatorio
        sem_wait(&clientes_en_sucursal);  // Espera si la sucursal está llena
        
        pthread_mutex_lock(&mutex);  // Bloquea la variable clientes
        clientes++;
        printf("Cliente %d llegó a la sucursal.\n", clientes);
        pthread_mutex_unlock(&mutex);  // Libera la variable clientes
        
        if (clientes == CLIENTES_POR_FURGON) {
            sem_post(&operacion_en_curso);  // Inicia el transporte cuando hay suficientes clientes
        }
    }
}

void* furgon(void* arg) {
    while (1) {
        sem_wait(&operacion_en_curso);  // Espera hasta que haya suficientes clientes para transportar
        
        printf("Furgoneta está transportando %d clientes.\n", CLIENTES_POR_FURGON);
        sleep(2);  // Simula el tiempo de transporte
        
        pthread_mutex_lock(&mutex);  // Bloquea la variable clientes
        clientes -= CLIENTES_POR_FURGON;  // Reduce el número de clientes
        pthread_mutex_unlock(&mutex);  // Libera la variable clientes
        
        for (int i = 0; i < CLIENTES_POR_FURGON; i++) {
            sem_post(&clientes_en_sucursal);  // Permite que nuevos clientes entren a la sucursal
        }
        
        printf("Furgoneta regresó y dejó espacio para más clientes.\n");
    }
}

int main() {
    pthread_t hilo_clientes[MAX_CLIENTES];
    pthread_t hilo_furgon;
    
    // Inicialización de semáforos y mutex
    sem_init(&clientes_en_sucursal, 0, CLIENTES_POR_FURGON);  // Solo CLIENTES_POR_FURGON clientes pueden estar en la sucursal a la vez
    sem_init(&operacion_en_curso, 0, 0);  // Al inicio no hay operación en curso
    pthread_mutex_init(&mutex, NULL);
    
    // Crear hilo para manejar la furgoneta
    pthread_create(&hilo_furgon, NULL, furgon, NULL);
    
    // Crear hilos para manejar la llegada de clientes
    for (int i = 0; i < MAX_CLIENTES; i++) {
        pthread_create(&hilo_clientes[i], NULL, llegada_cliente, NULL);
    }
    
    // Esperar a que los hilos terminen (no terminan en este caso, es un bucle infinito)
    pthread_join(hilo_furgon, NULL);
    for (int i = 0; i < MAX_CLIENTES; i++) {
        pthread_join(hilo_clientes[i], NULL);
    }
    
    // Destrucción de semáforos y mutex (no alcanzable por el bucle infinito)
    sem_destroy(&clientes_en_sucursal);
    sem_destroy(&operacion_en_curso);
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
