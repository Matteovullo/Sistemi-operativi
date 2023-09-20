/*
Scrivi un programma in linguaggio C che simuli un torneo di calcio tra N squadre. Il programma accetta un argomento sulla riga di comando che indica il numero di squadre che partecipano al torneo.

Il programma crea un thread per ogni squadra, un thread arbitro e un thread tabellone. I thread condivideranno una struttura dati e utilizzeranno i semafori per gestire la sincronizzazione.

La struttura dati dovrà contenere i dati relativi a ogni squadra con i seguenti specifici campi:
Nome squadra
Punteggio
Le squadre dovranno essere rappresentate dai thread S1, S2, ..., SN. Ogni squadra giocherà una partita contro le altre squadre. Al termine di ogni partita, comunica il punteggio all'arbitro A.

L'arbitro A riceve i punteggi dalle squadre e determina i risultati delle partite. Una volta che tutti i risultati sono stati determinati, comunica il risultato al thread tabellone T.

Il thread tabellone T gestisce la classifica delle squadre e, alla fine del torneo, stampa la classifica finale.

I thread dovranno terminare spontaneamente alla fine del torneo.
*/

#include<pthread.h>
#include<stdio.h>
#include<semaphore.h>
#include<string.h>
#include<stdlib.h>
#include "lib-misc.h"

#define NUM_SQUADRE 10

//typedef enum{ VITTORIA, SCONFITTA, PAREGGIO } risultato;
//typedef enum{ PLAYER1_N, PLAYER2_N, JUDGE_N, SCOREBOARD_N } thread_n;

typedef struct{
    //char vincitore;
    //risultato risultato;
    sem_t sem;
}partita;

typedef struct{
    char nome[100];
    short int punteggio;
    int id;
    pthread_t tip;
    partita* partita;
}squadra_t;

/*
typedef struct{
    partita *partita;
    pthread_t tip;
    thread_n thread_n;
}thread_data;
*/

squadra_t classifica[NUM_SQUADRE];

void squadra(void* arg){
    int err;
    squadra_t *td=(squadra_t*)(arg);

    for(int i=0; i<10; i++){

        if(td->id==i) continue;

        if(sem_wait(&td->partita->sem)!=0)
            exit_with_err("sem_wait", err);

        int r=rand()%3;
        if(r == 1) 
            classifica[td->id - 1].punteggio += 3;
        if(r == 2) 
            classifica[td->id - 1].punteggio++;

        if(sem_post(&td->partita->sem)!=0)
            exit_with_err("sem_post", err);
    } 

    pthread_exit(NULL);
}

/*
void arbitro(void* arg){
    for(int i=0; i<NUM_SQUADRE; i++){
        sem_wait()
    }
}*/

void init_sem(sem_t sem) {
    int err;

    if ((err = sem_init(sem, 0, 1)) != 0)
        exit_with_err("sem_init", err);
}

void tabellone(void* arg){
    int err;
    squadra_t *td=(squadra_t*)(arg);
    partita *p=malloc(sizeof(partita));

    printf("Classifica finale:\n");
    for(int i=0; i<NUM_SQUADRE; i++){
        if(sem_wait(td->partita->sem)!=0);
        printf("Posizione %d: Squadra %s - Punteggio: %d\n", i + 1, classifica[i].nome,
               classifica[i].punteggio);
    }
    pthread_exit(NULL);
}

int main(int argc, char argv[]){
    int err;
    srand(time(NULL));
    squadra_t sq[NUM_SQUADRE];
    partita *p=malloc(sizeof(partita));

    init_sem(p->sem);

    for(int i=0; i<NUM_SQUADRE; i++){
        sq[i].id=i+1;
    }

    for(int i=0; i<NUM_SQUADRE; i++){
        if(err=pthread_join(sq[i].tip, NULL) !=0)
            exit_with_err("pthread_join", err);
    }

    for(int i=0; i<NUM_SQUADRE; i++){
        sem_destroy(&p->sem);
    }

    free(p);
    exit(EXIT_SUCCESS);
}