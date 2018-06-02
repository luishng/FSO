#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <math.h>
#include <errno.h>
#include <signal.h>

//estados do monitor AE
#define AJUDANDO 0
#define DORMINDO 1

//estados do aluno
#define PROGRAMANDO 0
#define RECEBENDO_AJUDA 1
#define ESPERANDO 2

unsigned int gera_random(int minimo, int modulo);
void clean_semaphores();

sem_t *sem_fila, *sem_cadeira_monitor, *sem_monitor;

//-1 significa que nao tem ninguem sendo ajudado
int aluno_sendo_ajudado = -1;

struct Aluno
{
    int id;
    int max_tentativas;
};

void finalizar(){
    clean_semaphores();
    printf(".\n.\n.\n.\n.\n.\nMonitoria encerrada com sucesso.\n");
    exit(0);
}

void *liga_monitor(){
    int estado = DORMINDO;

    while(1){
        if(estado == DORMINDO){
            printf("Monitor: esta dormindo.\n");
            sem_wait(sem_monitor);
            estado = AJUDANDO;
        }else if(estado == AJUDANDO){
            printf("Monitor: Esta ajudando o aluno: %d\n", aluno_sendo_ajudado);
            
            while(aluno_sendo_ajudado > 0){
               //nothing to do
            }
    
            estado = DORMINDO;
        }
    }
}

void *liga_aluno(void *param){
    struct Aluno *aluno = (struct Aluno *)param;
    int id = aluno->id;
    int max_tentativas = aluno->max_tentativas;
    int tentativa = 0;
    int estado = PROGRAMANDO;
    unsigned int tempo;

    while(tentativa < max_tentativas){
        if(estado == PROGRAMANDO){
            tempo = gera_random(1,5);
            sleep(tempo);
            printf("Aluno %d: procurando ajuda.\n", id);
            estado = ESPERANDO;
        }else if(estado == RECEBENDO_AJUDA){
            tentativa++;
            aluno_sendo_ajudado = id;
            //acordando monitor
            sem_post(sem_monitor);
            printf("Aluno %d: esta recebendo sua %d ajuda.\n", id, tentativa);
            tempo = gera_random(1,5);
            sleep(tempo);
            aluno_sendo_ajudado = -1;
            estado = PROGRAMANDO;
            sem_post(sem_cadeira_monitor);
        }else if(estado == ESPERANDO){
            if(sem_trywait(sem_fila) == EAGAIN){
                estado = PROGRAMANDO;
                printf("Aluno %d: não encontrou espeço na fila e voltara a programar.\n", id);
            }else{
                //sentando na cadeira do monitor
                sem_wait(sem_cadeira_monitor);
                //saindo da fila
                sem_post(sem_fila);
                estado = RECEBENDO_AJUDA;
            }
        }
    }

    printf("O Aluno %d foi embora feliz.\n", id);
    pthread_exit(0);
}

unsigned int gera_random(int minimo, int maximo){
    return (rand() % (maximo - minimo + 1)) + minimo;
}

void clean_semaphores(){
    sem_unlink("fila");   
    sem_unlink("cadeira_monitor");   
    sem_unlink("monitor");   
    sem_close(sem_fila);   
    sem_close(sem_cadeira_monitor);   
    sem_close(sem_monitor);  
}

int main(){
    signal(SIGINT, finalizar);
    int i;
    srand(time(NULL));
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
        alunos[i].max_tentativas = 3;
        pthread_attr_init(&attrs_alunos[i]);
        pthread_create(&tid_alunos[i], &attrs_alunos[i], liga_aluno, &alunos[i]);
    }

    for(i = 1; i <= numero_alunos; i++)
    {
        pthread_join(tid_alunos[i], NULL);
    }

    printf("\nTodos os alunos acabaram.\n\n");
    pthread_cancel(tid_monitor);

    clean_semaphores(); 

    return 0;
}
