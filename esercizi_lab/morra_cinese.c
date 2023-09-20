#include<pthread.h>
#include<semaphore.h>
#include<stdbool.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef enum{PLAYER1_N, PLAYER2_N, JUDGE_N, SCOREBOARD_N } thread_n;
typedef enum { CARTA, FORBICE, SASSO } mossa;
char *nome_mosse[3] = {"carta", "forbice", "sasso"};

#define exit_with_err(s, e)                                                    \
    do {                                                                       \
        fprintf(stderr, "%s: %s\n", (s), strerror((e)));                       \
        exit(EXIT_FAILURE);                                                    \
    } while (0)

typedef struct{
    mossa mosse[2];
    char vincitore;
    bool done;
    sem_t sem[4];
} match;

typedef struct{
    pthread_t tid;
    thread_n thread_n;
    unsigned n_matches;
    match *match;
}thread_data;

void init_sem(sem_t *sem){
    int err;

    if(err=sem_init(&sem[PLAYER1_N], 0, 1)!=0)
        exit_with_err("sem_init", err);
    if(err=sem_init(&sem[PLAYER2_N], 0, 1)!=0)
        exit_with_err("sem_init", err);
    if(err=sem_init(&sem[JUDGE_N], 0, 1)!=0)
        exit_with_err("sem_init", err);
    if(err=sem_init(&sem[SCOREBOARD_N], 0, 1)!=0)
        exit_with_err("sem_init", err);
}

void player(void *arg){
    int err;
    char mossa;
    thread_data *td=(thread_data*)arg;

    while(1){
        if(err=sem_wait(&td->match->sem[td->thread_n]) !=0){
            exit_with_err("sem_wait", err);
        }

        if(td->match->done) break;

        td->match->mosse[td->thread_n]=rnad()%3;

        printf("P%d: mossa '%s'\n", td->thread_n+1, nome_mosse[td->match->mosse[td->thread_n]]);

        if(err=sem_post(&td->match->sem[JUDGE_N]) !=0)
            exit_with_err("sem_post", err);
    }

    pthread_exit(NULL);
}

char whowins(mossa *mosse) {
    if (mosse[0] == mosse[1])
        return 0;

    if ((mosse[0] == CARTA && mosse[1] == SASSO) ||
        (mosse[0] == FORBICE && mosse[1] == CARTA) ||
        (mosse[0] == SASSO && mosse[1] == FORBICE))
        return 1;

    return 2;
}


