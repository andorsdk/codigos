#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h> // Para usar intptr_t

// Definir los semáforos
sem_t mostradores_comida;
sem_t mostradores_postre;
sem_t abridores;

// Función que simula el comportamiento de un soldado
void* soldado(void* arg) {
    // Recuperar el ID del soldado desde el puntero
    int id = (intptr_t) arg;
    printf("Soldado %d entra al comedor.\n", id);
    
    // Obtener comida en uno de los mostradores de comida
    sem_wait(&mostradores_comida);
    printf("Soldado %d está tomando comida en un mostrador.\n", id);
    sleep(rand() % 2 + 1);  // Simula el tiempo para tomar la comida
    sem_post(&mostradores_comida);

    // Decidir si toma refresco o agua
    int refresco = rand() % 2;  // 0: agua, 1: refresco
    if (refresco) {
        // Si quiere refresco, necesita un abridor
        sem_wait(&abridores);
        printf("Soldado %d ha tomado refresco y está usando un abridor.\n", id);
        sleep(rand() % 1 + 1);  // Simula el tiempo de uso del abridor
        sem_post(&abridores);
    }

    // Obtener postre en uno de los mostradores de postre
    sem_wait(&mostradores_postre);
    printf("Soldado %d está tomando postre en un mostrador.\n", id);
    sleep(rand() % 2 + 1);  // Simula el tiempo para tomar el postre
    sem_post(&mostradores_postre);

    printf("Soldado %d ha terminado de comer y sale del comedor.\n", id);
    
    pthread_exit(NULL);
}

int main() {
    // Inicializar los semáforos
    sem_init(&mostradores_comida, 0, 5);  // 5 mostradores de comida
    sem_init(&mostradores_postre, 0, 5);  // 5 mostradores de postre
    sem_init(&abridores, 0, 60);          // 60 abridores

    pthread_t soldados[1000];

    // Crear 1000 hilos (soldados)
    for (int i = 1; i <= 1000; i++) {
        // Pasar el ID del soldado haciendo casting a un tipo compatible con void*
        pthread_create(&soldados[i], NULL, soldado, (void*)(intptr_t)i);
    }

    // Esperar a que todos los soldados terminen
    for (int i = 1; i <= 1000; i++) {
        pthread_join(soldados[i], NULL);
    }

    // Destruir los semáforos
    sem_destroy(&mostradores_comida);
    sem_destroy(&mostradores_postre);
    sem_destroy(&abridores);

    return 0;
}
