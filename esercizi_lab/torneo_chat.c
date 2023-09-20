#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define NUM_SQUADRE 10

typedef struct {
    char nome[100];
    int punteggio;
    pthread_t thread_id;
    sem_t sem;
} Squadra;

typedef struct {
    int vincitore;
    sem_t sem;
} Partita;

Squadra classifica[NUM_SQUADRE];
Partita partite[NUM_SQUADRE][NUM_SQUADRE];

void* squadra(void* arg) {
    Squadra* squadra = (Squadra*)arg;
    int squadra_id = squadra->thread_id - 1;

    for (int i = 0; i < NUM_SQUADRE; i++) {
        if (i == squadra_id)
            continue; // Salta la partita contro se stessa

        int punteggio = rand() % 5; // Genera un punteggio casuale da 0 a 4

        printf("Partita: Squadra %s vs Squadra %s - Punteggio: %d\n", squadra->nome,
               classifica[i].nome, punteggio);

        sem_wait(&partite[squadra_id][i].sem);
        partite[squadra_id][i].vincitore = squadra_id + 1;
        sem_post(&partite[squadra_id][i].sem);

        sem_wait(&partite[i][squadra_id].sem);
        partite[i][squadra_id].vincitore = squadra_id + 1;
        sem_post(&partite[i][squadra_id].sem);

        sem_wait(&squadra->sem);
        squadra->punteggio += punteggio;
        sem_post(&squadra->sem);
    }

    pthread_exit(NULL);
}

void* tabellone(void* arg) {
    printf("Classifica finale:\n");
    for (int i = 0; i < NUM_SQUADRE; i++) {
        sem_wait(&classifica[i].sem);
    }

    for (int i = 0; i < NUM_SQUADRE; i++) {
        int max_punteggio = -1;
        int vincitore = -1;

        for (int j = 0; j < NUM_SQUADRE; j++) {
            sem_wait(&classifica[j].sem);

            if (classifica[j].punteggio > max_punteggio) {
                max_punteggio = classifica[j].punteggio;
                vincitore = j;
            }

            sem_post(&classifica[j].sem);
        }

        printf("Posizione %d: Squadra %s - Punteggio: %d\n", i + 1, classifica[vincitore].nome,
               classifica[vincitore].punteggio);

        sem_wait(&classifica[vincitore].sem);
        classifica[vincitore].punteggio = -1; // Imposta il punteggio a -1 per evitare di selezionarlo nuovamente come vincitore
        sem_post(&classifica[vincitore].sem);
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    if (argc > 1) {
        //NUM_SQUADRE = atoi(argv[1]);
        if (NUM_SQUADRE < 2 || NUM_SQUADRE > 10) {
            printf("Numero di squadre non valido. Il numero di squadre deve essere compreso tra 2 e 10.\n");
            return 1;
        }
    }

    for (int i = 0; i < NUM_SQUADRE; i++) {
        printf("Inserisci il nome della squadra %d: ", i + 1);
        scanf("%s", classifica[i].nome);
        classifica[i].punteggio = 0;
        sem_init(&classifica[i].sem, 0, 1);

        for (int j = 0; j < NUM_SQUADRE; j++) {
            sem_init(&partite[i][j].sem, 0, 1);
            partite[i][j].vincitore = 0;
        }

        pthread_create(&classifica[i].thread_id, NULL, squadra, &classifica[i]);
    }

    pthread_t thread_tabellone;
    pthread_create(&thread_tabellone, NULL, tabellone, NULL);

    for (int i = 0; i < NUM_SQUADRE; i++) {
        pthread_join(classifica[i].thread_id, NULL);
    }

    pthread_join(thread_tabellone, NULL);

    for (int i = 0; i < NUM_SQUADRE; i++) {
        sem_destroy(&classifica[i].sem);

        for (int j = 0; j < NUM_SQUADRE; j++) {
            sem_destroy(&partite[i][j].sem);
        }
    }

    return 0;
}
