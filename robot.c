#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_PIEZAS 10   // Capacidad máxima de la cinta
#define CONTROL_PIEZAS 3 // Piezas necesarias para control de calidad

// Estructura de la cinta transportadora
typedef struct {
    int piezas;
    sem_t mutex;       // Semáforo para acceso exclusivo a la cinta
    sem_t lleno;       // Semáforo para la capacidad de la cinta
    sem_t vacio;       // Semáforo para la disponibilidad de piezas
} CintaTransportadora;

CintaTransportadora cinta;

// Máquina de inyección: produce una pieza y la deja en la cinta
void* maquina_inyectar(void* arg) {
    while (1) {
        sem_wait(&cinta.lleno);       // Espera si la cinta está llena
        sem_wait(&cinta.mutex);       // Bloquea la cinta

        cinta.piezas++;
        printf("Máquina inyectó una pieza. Piezas en la cinta: %d\n", cinta.piezas);

        sem_post(&cinta.mutex);       // Libera la cinta
        sem_post(&cinta.vacio);       // Indica que hay una pieza disponible

        sleep(1); // Simula el tiempo de producción
    }
}

// Robot: recoge una pieza de la cinta y la pone en la caja
void* robot_recoger(void* arg) {
    while (1) {
        sem_wait(&cinta.vacio);       // Espera si no hay piezas
        sem_wait(&cinta.mutex);       // Bloquea la cinta

        if (cinta.piezas > 0) {       // Verifica que haya al menos una pieza
            cinta.piezas--;
            printf("Robot recogió una pieza. Piezas en la cinta: %d\n", cinta.piezas);
        }

        sem_post(&cinta.mutex);       // Libera la cinta
        sem_post(&cinta.lleno);       // Indica que hay espacio en la cinta

        sleep(2); // Simula el tiempo de recogida
    }
}

// Operario de control de calidad: recoge tres piezas para revisión
void* control_calidad(void* arg) {
    while (1) {
        sem_wait(&cinta.mutex);       // Bloquea la cinta para verificar
        if (cinta.piezas >= CONTROL_PIEZAS) {  // Solo si hay al menos 3 piezas
            cinta.piezas -= CONTROL_PIEZAS;
            printf("Operario recogió %d piezas para control de calidad. Piezas en la cinta: %d\n", CONTROL_PIEZAS, cinta.piezas);

            // Libera espacio por las 3 piezas retiradas
            sem_post(&cinta.lleno);
            sem_post(&cinta.lleno);
            sem_post(&cinta.lleno);
        } else {
            printf("Operario no encontró suficientes piezas para control de calidad. Piezas actuales: %d\n", cinta.piezas);
        }
        sem_post(&cinta.mutex);       // Libera la cinta
        sleep(3); // Espera antes de intentarlo de nuevo
    }
}

int main() {
    // Inicializar la cinta transportadora
    cinta.piezas = 0;
    sem_init(&cinta.mutex, 0, 1);     // Semáforo mutex para acceso exclusivo
    sem_init(&cinta.lleno, 0, MAX_PIEZAS);  // Semáforo para capacidad de la cinta
    sem_init(&cinta.vacio, 0, 0);     // Semáforo para piezas disponibles

    // Crear los threads
    pthread_t thread_maquina, thread_robot, thread_operario;

    pthread_create(&thread_maquina, NULL, maquina_inyectar, NULL);
    pthread_create(&thread_robot, NULL, robot_recoger, NULL);
    pthread_create(&thread_operario, NULL, control_calidad, NULL);

    // Esperar a que terminen (nunca terminarán en este caso)
    pthread_join(thread_maquina, NULL);
    pthread_join(thread_robot, NULL);
    pthread_join(thread_operario, NULL);

    // Destruir los semáforos
    sem_destroy(&cinta.mutex);
    sem_destroy(&cinta.lleno);
    sem_destroy(&cinta.vacio);

    return 0;
}
