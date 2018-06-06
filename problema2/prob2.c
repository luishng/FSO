#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>

//estados do monitor AE
#define PROCURANDO 0
#define AJUDANDO 1
#define DORMINDO 2

//estados do aluno
#define PROGRAMANDO 0
#define RECEBENDO_AJUDA 1
#define ESPERANDO 2

unsigned int gera_random(int minimo, int modulo);
void clean_semaphores();

sem_t *sem_fila, *sem_cadeira_monitor, *sem_monitor;

//-1 significa que nao tem ninguem sendo ajudado
int aluno_sendo_ajudado = -1;
int numero_cadeiras = 0;

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
    int estado = PROCURANDO;
    int cadeiras_disponiveis_fila = 0;

    while(1){
        sem_getvalue(sem_fila, &cadeiras_disponiveis_fila); 
        if(estado == PROCURANDO){
            printf("Monitor: olha a fila\n");
            if(cadeiras_disponiveis_fila == numero_cadeiras){
                printf("Monitor: nao encontrou ninguem.\n");
                estado = DORMINDO;
            }else{
                printf("Monitor: encontrou alguem na fila.\n");
                estado = AJUDANDO;
            }
        }else if(estado == DORMINDO){
            printf("Monitor: esta dormindo.\n");
            sem_wait(sem_monitor);
            estado = AJUDANDO;
        }else if(estado == AJUDANDO){
            while(aluno_sendo_ajudado < 0){
               //nothing to do
            }
            
            printf("Monitor: Esta ajudando o aluno: %d\n", aluno_sendo_ajudado);
            
            while(aluno_sendo_ajudado > 0){
               //nothing to do
            }
    
            estado = PROCURANDO;
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
    int sem_monitor_value;

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
            sem_getvalue(sem_monitor, &sem_monitor_value);
            if(sem_monitor_value == 0){
                sem_post(sem_monitor);
            }
            printf("Aluno %d: esta recebendo sua %d ajuda.\n", id, tentativa);
            tempo = gera_random(1,5);
            sleep(tempo);
            aluno_sendo_ajudado = -1;
            estado = PROGRAMANDO;
            sem_post(sem_cadeira_monitor);
        }else if(estado == ESPERANDO){
            if(sem_trywait(sem_fila) == -1){
                estado = PROGRAMANDO;
                printf("Aluno %d: nao encontrou espaco na fila e voltara a programar.\n", id);
            }else{
                printf("Aluno %d: entrou fila.\n", id);
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

void mostra_sem_values(){

    int sem_fila_value, sem_monitor_value, sem_cadeira_monitor_value;
    sem_getvalue(sem_monitor, &sem_monitor_value);
    sem_getvalue(sem_fila, &sem_fila_value);
    sem_getvalue(sem_cadeira_monitor, &sem_cadeira_monitor_value);

    printf("Sistema: sem_monitor: %d\n", sem_monitor_value);
    printf("Sistema: sem_fila: %d\n", sem_fila_value);
    printf("Sistema: sem_cadeira_monitor: %d\n\n", sem_cadeira_monitor_value);    
}

int main(){
    clean_semaphores();
    signal(SIGINT, finalizar);
    srand(time(NULL));
    int i;
    int numero_alunos = gera_random(3, 40);
    numero_cadeiras = numero_alunos/2 + numero_alunos%2;

    printf("Sistema: exitem %d alunos e %d cadeiras.\n\n", numero_alunos, numero_cadeiras);
    
    sem_fila = sem_open("fila", O_CREAT, 0644, numero_cadeiras);
    sem_cadeira_monitor = sem_open("cadeira_monitor", O_CREAT, 0644, 1);
    sem_monitor = sem_open("monitor", O_CREAT, 0644, 0);

    mostra_sem_values();

    pthread_t tid_alunos[numero_alunos+1];
    pthread_attr_t attrs_alunos[numero_alunos+1];

    pthread_t tid_monitor;
    pthread_attr_t attr_monitor;

    pthread_attr_init(&attr_monitor);
    pthread_create(&tid_monitor, &attr_monitor, liga_monitor, NULL);

    struct Aluno alunos[numero_alunos+1];
    
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

    pthread_cancel(tid_monitor);
    printf("\nTodos os alunos acabaram.\n\n");

    mostra_sem_values();

    clean_semaphores(); 

    return 0;
}
