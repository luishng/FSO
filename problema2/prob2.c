#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//estados do monitor AE
#define AJUDANDO 0
#define DORMINDO 1

//estados do aluno
#define PROGRAMANDO 0
#define RECEBENDO_AJUDA 1
#define ESPERANDO 2


int *fila;
struct Aluno
{
    int id;
    int max_tentativas;
};

void *liga_aluno(void *param){

    struct Aluno *aluno = (struct Aluno *)param;
    int id = aluno->id;
    int max_tentativas = aluno->max_tentativas;
    int tentativa = 0;
    int estado = PROGRAMANDO;
    int tempo;

    while(tentativa < max_tentativas){
        if(estado == PROGRAMANDO){
            tempo = gera_random(5);
            sleep(tempo);
            
            if(verifica_vaga()){
                if(entrar_na_fila()){
                    estado == ESPERANDO;
                }else{
                    estado == PROGRAMANDO;
                }
            }else{
                //dar lock no monitor
                estado == RECEBENDO_AJUDA;
            }
        }else if(estado == RECEBENDO_AJUDA){
            tentativa++;
            printf("O aluno %d esta recebendo sua %d ajuda.\n", id, tentativa);
            tempo = gera_random(5);
            sleep(tempo);
            printf("O aluno %d demorou %d segundos concluindo sua %d ajuda.\n", id, tempo, tentativa);

            //liberar monitor
            estado == PROGRAMANDO;
        }else if(estado == ESPERANDO){
            while(true){
                if(verifica_posicao_fila(id)){
                    //lock monitor
                    estado == RECEBENDO_AJUDA;
                    break;
                }
                sleep(1);

            }
        }
    }
    printf("O Aluno %d foi embora feliz.\n", id);
    pthread_exit(0);
    
}
int main(){

    return 0;
}