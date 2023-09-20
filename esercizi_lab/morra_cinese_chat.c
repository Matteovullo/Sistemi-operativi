#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_PARTITE 10

// Struttura dati per una singola partita
typedef struct {
    char mossa_p1;
    char mossa_p2;
    int vincitore;
} Partita;

// Variabili globali
Partita partita;
sem_t sem_p1, sem_p2, sem_g, semaforo_t;

// Funzione per generare una mossa casuale
char genera_mossa_casuale() {
    char mosse[3] = {'C', 'F', 'S'};
    int index = rand() % 3;
    return mosse[index];
}

// Thread P1 (giocatore 1)
void* P1(void* arg) {
    for (int i = 0; i < NUM_PARTITE; i++) {
        partita.mossa_p1 = genera_mossa_casuale();
        printf("P1: Ho scelto %c\n", partita.mossa_p1);
        sem_post(&sem_g);
        sem_wait(&sem_p1);
        sem_wait(&semaforo_t);
    }
    pthread_exit(NULL);
}

// Thread P2 (giocatore 2)
void* P2(void* arg) {
    for (int i = 0; i < NUM_PARTITE; i++) {
        partita.mossa_p2 = genera_mossa_casuale();
        printf("P2: Ho scelto %c\n", partita.mossa_p2);
        sem_post(&sem_g);
        sem_wait(&sem_p2);
        sem_wait(&semaforo_t);
    }
    pthread_exit(NULL);
}

// Thread G (giudice)
void* G(void* arg) {
    for (int i = 0; i < NUM_PARTITE; i++) {
        sem_wait(&sem_g);
        
        if (partita.mossa_p1 == partita.mossa_p2) {
            partita.vincitore = 0; // Partita patta
        } else if ((partita.mossa_p1 == 'C' && partita.mossa_p2 == 'F') ||
                   (partita.mossa_p1 == 'F' && partita.mossa_p2 == 'S') ||
                   (partita.mossa_p1 == 'S' && partita.mossa_p2 == 'C')) {
            partita.vincitore = 1; // Vincitore: giocatore 1
        } else {
            partita.vincitore = 2; // Vincitore: giocatore 2
        }
        printf("G: Vincitore: %d\n", partita.vincitore);
        sem_post(&sem_p1);
        sem_post(&sem_p2);
        sem_post(&semaforo_t);
    }
    pthread_exit(NULL);
}

// Thread T (tabellone)
void* T(void* arg) {
    int vittorie_p1 = 0;
    int vittorie_p2 = 0;

    for (int i = 0; i < NUM_PARTITE; i++) {
        sem_wait(&semaforo_t);

        if (partita.vincitore == 1) {
            vittorie_p1++;
        } else if (partita.vincitore == 2) {
            vittorie_p2++;
        }

        printf("T: Classifica - P1: %d, P2: %d\n", vittorie_p1, vittorie_p2);

        if (vittorie_p1 == NUM_PARTITE || vittorie_p2 == NUM_PARTITE) {
            break;
        }

        sem_post(&sem_p1);
        sem_post(&sem_p2);
    }

    if (vittorie_p1 > vittorie_p2) {
        printf("T: Vincitore del torneo: P1\n");
    } else if (vittorie_p2 > vittorie_p1) {
        printf("T: Vincitore del torneo: P2\n");
    } else {
        printf("T: Il torneo è terminato in pareggio\n");
    }

    sem_post(&sem_p1);
    sem_post(&sem_p2);

    pthread_exit(NULL);
}

int main() {
    // Inizializzazione dei semafori
    sem_init(&sem_p1, 0, 0);
    sem_init(&sem_p2, 0, 0);
    sem_init(&sem_g, 0, 0);
    sem_init(&semaforo_t, 0, 0);

    // Creazione dei thread
    pthread_t thread_p1, thread_p2, thread_g, thread_t;
    pthread_create(&thread_p1, NULL, P1, NULL);
    pthread_create(&thread_p2, NULL, P2, NULL);
    pthread_create(&thread_g, NULL, G, NULL);
    pthread_create(&thread_t, NULL, T, NULL);

    // Join dei thread
    pthread_join(thread_p1, NULL);
    pthread_join(thread_p2, NULL);
    pthread_join(thread_g, NULL);
    pthread_join(thread_t, NULL);

    // Deallocazione dei semafori
    sem_destroy(&sem_p1);
    sem_destroy(&sem_p2);
    sem_destroy(&sem_g);
    sem_destroy(&semaforo_t);

    return 0;
}
