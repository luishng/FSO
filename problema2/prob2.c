#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <math.h>
#include <errno.h>

//estados do monitor AE
#define AJUDANDO 0
#define DORMINDO 1

//estados do aluno
#define PROGRAMANDO 0
#define RECEBENDO_AJUDA 1
#define ESPERANDO 2

int gera_random(int minimo, int modulo);

sem_t *sem_fila, *sem_cadeira_monitor, *sem_monitor;

//-1 segnifica q n tem ninguem sendo ajudado
int aluno_sendo_ajudado = -1;

struct Aluno
{
    int id;
    int max_tentativas;
};

void *liga_monitor(){
    
    int estado = DORMINDO;

    while(1){
        if(estado == DORMINDO){
            printf("Monitor: está dormindo.\n");
            sem_wait(sem_monitor);
            estado= AJUDANDO;
        }else if(estado == AJUDANDO){
            printf("Monitor: ajudando aluno %d", aluno_sendo_ajudado);
            while(aluno_sendo_ajudado>0){
                sleep(1);
            }
            estado=DORMINDO;
        }
    }
}

void *liga_aluno(void *param){

    struct Aluno *aluno = (struct Aluno *)param;
    int id = aluno->id;
    int max_tentativas = aluno->max_tentativas;
    int tentativa = 0;
    int estado = PROGRAMANDO;
    int tempo;

    while(tentativa < max_tentativas){
        if(estado == PROGRAMANDO){
            tempo = gera_random(1,5);
            sleep(tempo);
            printf("Aluno %d: procurando ajuda.\n", id);
            estado = ESPERANDO;
        }else if(estado == RECEBENDO_AJUDA){
            tentativa++;
            aluno_sendo_ajudado = id;
            printf("Aluno %d: esta recebendo sua %d ajuda.\n", id, tentativa);
            tempo = gera_random(1,5);
            sleep(tempo);
            aluno_sendo_ajudado = -1;
            estado = PROGRAMANDO;
            sem_post(sem_cadeira_monitor);
        }else if(estado == ESPERANDO){
            if(sem_trywait(sem_fila) == EAGAIN){
                estado = PROGRAMANDO;
            }else{
                //sentando na cadeira do monitor
                sem_wait(sem_cadeira_monitor);
                //saindo da fila
                sem_post(sem_fila);
                //acordando monitor
                sem_post(sem_monitor);
                estado = RECEBENDO_AJUDA;
            }
        }
    }
    printf("O Aluno %d foi embora feliz.\n", id);
    pthread_exit(0);
    
}

int gera_random(int minimo,int modulo){
    return (minimo + (random()%(modulo+1)))%modulo;
}
int main(){
    int i;
    int numero_alunos = gera_random(3, 40);
    int numero_cadeiras = numero_alunos/2 + numero_alunos%2;

    printf("Sistema: exitem %d alunos e %d cadeiras.\n", numero_alunos, numero_cadeiras);

    sem_fila = sem_open("fila", O_CREAT, 0644, numero_cadeiras);
    sem_cadeira_monitor = sem_open("cadeira_monitor", O_CREAT, 0644, 1);
    sem_monitor = sem_open("monitor", O_CREAT, 0644, 0);


    pthread_t tid_alunos[numero_alunos];
    pthread_attr_t attrs_alunos[numero_alunos];

    pthread_t tid_monitor;
    pthread_attr_t attr_monitor;

    pthread_attr_init(&attr_monitor);
    pthread_create(&tid_monitor, &attr_monitor, liga_monitor, NULL);

    struct Aluno alunos[numero_alunos];
    
    for(i = 1; i <= numero_alunos; i++)
    {
        alunos[i].id = i;
        pthread_attr_init(&attrs_alunos[i]);
        pthread_create(&tid_alunos[i], &attrs_alunos[i], liga_aluno, &alunos[i]);
    }

    for(int i = 0; i < numero_alunos; i++)
    {
        pthread_join(tid_alunos[i], NULL);
    }
    printf("Todos os alunos acabaram.");

    pthread_cancel(tid_monitor);

    return 0;
}
