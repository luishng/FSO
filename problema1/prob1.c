// gcc pthread.c -o pthread -lpthread

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 9
#define ELEMENTS 9
#define NOME_ARQUIVO "1.txt"

int sudoku[9][9];

void *coluna(void *param);
void *linha(void *param);
void *grid(void *param);

struct QUADRANTE
{
    int id;
};


int main(int argc, char *argv[])
{
     
    FILE *arquivo;

    arquivo = fopen(strcat("sudokus/",NOME_ARQUIVO), "r");
    if(arquivo == NULL){
        printf("arquivo nao encontrado.");
        system("pause");
        return -1;
    }
    
    for(int i=0; i<9; i++){
        for(int j=0; j<9; j++){
            if(fscanf(arquivo, "%d ", &sudoku[i][j])){
                continue;   
            }
            else{
                if(fscanf(arquivo, "%d\n", &sudoku[i][j]) || fscanf(arquivo, "%dEOF", &sudoku[i][j])){
                    continue;
                }else{
                    printf("Arquivo com formato incorreto");
                    return -1;
                }
                
            }
        }
    }
    printf("Arquivo lido.");



    
    pthread_t tid_quadrante[NUM_THREADS];
    pthread_attr_t attrs[NUM_THREADS];

    pthread_t tid_linha;
    pthread_t tid_coluna;

    pthread_attr_t attr_linha;
    pthread_attr_t attr_coluna;

    pthread_attr_init(&attr_coluna);
    pthread_attr_init(&attr_linha);
    pthread_create(&tid_coluna, &attr_coluna, coluna, NULL);
    pthread_create(&tid_linha, &attr_linha, linha, NULL);

    struct QUADRANTE args[NUM_THREADS];
    
    for(int i = 0; i < NUM_THREADS; i++)
    {
        args[i].id = i;
        pthread_attr_init(&attrs[i]);
        pthread_create(&tid_quadrante[i], &attrs[i], grid, &args[i]);
    }

    for(int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(tid_quadrante[i], NULL);
    }

    pthread_join(tid_linha, NULL);
    pthread_join(tid_coluna, NULL);

  return 0;
}

void *grid(void *param)
{
    struct QUADRANTE *arg = (struct ARG *)param;
    int id = arg->id;

    pthread_exit(0);
}

void *coluna(void *param)
{
    int i, upper = atoi(param);

    pthread_exit(0);
}

void *linha(void *param)
{
    int i, upper = atoi(param);

    pthread_exit(0);
}